/*
 * CompPhysics.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// CompPhysics.h für mehr Informationen

#include <Box2D/Box2D.h>       // extere Physikbibliothek
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <climits>

#include "../Logger.h"
#include "CompPhysics.h"

using boost::property_tree::ptree;

// eindeutige ID
const ComponentTypeId CompPhysics::COMPONENT_TYPE_ID = "CompPhysics";

// Konstruktor
CompPhysics::CompPhysics(const ComponentIdType& id, GameEvents& gameEvents, const BodyDef& rBodyDef) :
    Component(id, gameEvents),
    m_body (NULL),
    m_bodyDef (rBodyDef),
    m_localGravitationPoint (),
    m_smoothCenterOfMass (),
    m_smoothAngle (0.0f),
    m_previousAngle (0.0f),
    m_gravField (NULL),
    m_nUpdatesSinceGravFieldChange (UINT_MAX)
{}

void CompPhysics::addShapeDef( boost::shared_ptr<ShapeDef> pShapeDef )
{
    m_shapeInfos.push_back( pShapeDef );
}

bool CompPhysics::setShapeFriction(const ComponentIdType& shapeName, float friction)
{
    FixtureMap::const_iterator it = m_fixtureMap.find( shapeName );
    b2Fixture* fixture = it->second;
    if ( it != m_fixtureMap.end() )
    {
    	fixture->SetFriction(friction);
    }
    else
    {
    	gAaLog.write("setShapeFriction: bad shapeName '%s'", shapeName.c_str());
        return false;
    }

    // friction of existing contacts need to be updated because they don't change with b2Fixture::SetFriction
    for ( b2ContactEdge* contactEdge = m_body->GetContactList();
    	  contactEdge;
    	  contactEdge = contactEdge->next )
    {
    	if (fixture == contactEdge->contact->GetFixtureA() || fixture == contactEdge->contact->GetFixtureB() )
    		contactEdge->contact->ResetFriction(); // uses sqrt and multiplication
    }

    return true;
}

float CompPhysics::getMass() const
{
    return m_body->GetMass();
}

float CompPhysics::getAngle() const
{
    return m_body->GetAngle();
}

float CompPhysics::getLinearDamping() const
{
    if (m_body)
        return m_body->GetLinearDamping();
    return m_bodyDef.linearDamping;
}

float CompPhysics::getAngularDamping() const
{
    if (m_body)
        return m_body->GetAngularDamping();
    return m_bodyDef.angularDamping;
}

bool CompPhysics::isFixedRotation() const
{
    if (m_body)
        return m_body->IsFixedRotation();
    return m_bodyDef.fixedRotation;
}

bool CompPhysics::isBullet() const
{
    if (m_body)
        return m_body->IsBullet();
    return m_bodyDef.bullet;
}

Vector2D CompPhysics::getLinearVelocity() const
{
    return Vector2D( m_body->GetLinearVelocity() );
}

void CompPhysics::setLinearVelocity(const Vector2D& vel)
{
    m_body->SetLinearVelocity( *vel.to_b2Vec2() );
}

void CompPhysics::applyLinearImpulse(const Vector2D& impulse, const Vector2D& point)
{
    m_body->ApplyLinearImpulse(*impulse.to_b2Vec2(), *point.to_b2Vec2());
}

void CompPhysics::applyForce(const Vector2D& impulse, const Vector2D& point)
{
    m_body->ApplyForce(*impulse.to_b2Vec2(), *point.to_b2Vec2());
}

void CompPhysics::rotate( float deltaAngle, const Vector2D& localPoint )
{
    float newAngle = m_body->GetAngle() + deltaAngle;

    Vector2D worldRotationCenter( Vector2D(m_body->GetPosition()) + localPoint.rotated(m_body->GetAngle()) );
    Vector2D worldRotationCenterToBodyCenter ( -localPoint.rotated(newAngle) );
    Vector2D newPos = worldRotationCenter + worldRotationCenterToBodyCenter;

    m_body->SetTransform( *newPos.to_b2Vec2(), newAngle );
}


Vector2D CompPhysics::getPosition() const
{
    return m_body->GetPosition();
}

Vector2D CompPhysics::getSmoothPosition() const
{
    return m_smoothCenterOfMass - Vector2D(m_body->GetLocalCenter()).rotated(m_smoothAngle);
}

ContactVector CompPhysics::getContacts(bool getSensors) const
{
    std::vector<boost::shared_ptr<ContactInfo> > vecTouchInfo;
    for ( b2ContactEdge* contactEdge = m_body->GetContactList();
          contactEdge;
          contactEdge = contactEdge->next )
    {
        if ( !contactEdge->contact->IsTouching() ||
             ( !getSensors && ( contactEdge->contact->GetFixtureA()->IsSensor() ||
               contactEdge->contact->GetFixtureB()->IsSensor() ) ) )
            continue;
        b2WorldManifold worldManifold;
        contactEdge->contact->GetWorldManifold( &worldManifold );

        float normalFactor = 1.0f; // TODO is this needed?
        CompShape* thisShape;
        CompShape* otherShape;
        if (contactEdge->contact->GetFixtureA()->GetBody() != m_body)
        {
            normalFactor = -1.0f;
            thisShape = static_cast<CompShape*>(contactEdge->contact->GetFixtureB()->GetUserData());
            otherShape = static_cast<CompShape*>(contactEdge->contact->GetFixtureA()->GetUserData());
        }
        else
        {
            thisShape = static_cast<CompShape*>(contactEdge->contact->GetFixtureA()->GetUserData());
            otherShape = static_cast<CompShape*>(contactEdge->contact->GetFixtureB()->GetUserData());
        }

        boost::shared_ptr<ContactInfo> touchInfo = boost::shared_ptr<ContactInfo>(
                new ContactInfo(*static_cast<CompPhysics*>(contactEdge->other->GetUserData()),
                        *thisShape,
                        *otherShape,
                        worldManifold.points[0], // TODO: use all points (1 or 2?)
                        normalFactor * worldManifold.normal
                ));
        vecTouchInfo.push_back(touchInfo);
    }
    return vecTouchInfo;
}

const Vector2D& CompPhysics::getSmoothCenterOfMass() const
{
    return m_smoothCenterOfMass;
}

Vector2D CompPhysics::getCenterOfMass() const
{
    return Vector2D( m_body->GetWorldCenter() );
}

void CompPhysics::loadFromPropertyTree(const ptree& propTree)
{
    float linearDamping = propTree.get("damping.linear", 0.0f);
    float angularDamping = propTree.get("damping.angular", 0.0f);

    bool fixedRotation = propTree.get("isFixedRotation", false);

    bool isBullet = propTree.get("isBullet", false);

    Vector2D gravitationPoint;
    gravitationPoint.x = propTree.get("gravitationPoint.x", 0.0f);
    gravitationPoint.y = propTree.get("gravitationPoint.y", 0.0f);

    BodyDef body_def;
    body_def.angularDamping = angularDamping;
    body_def.fixedRotation = fixedRotation;
    body_def.bullet = isBullet;
    body_def.linearDamping = linearDamping;

    setLocalGravitationPoint(gravitationPoint);

    BOOST_FOREACH(const ptree::value_type &v, propTree)
    {
        if (v.first != "shape")
            continue;

        const ptree& shapeProps = v.second;
        std::string shapeName = shapeProps.get<std::string>("comp_id");

        float density = shapeProps.get("density", 0.0f);
        float friction = shapeProps.get("friction", 0.0f);
        float restitution = shapeProps.get("restitution", 0.0f);

        bool isSensor = shapeProps.get("isSensor", false);

        if (density != 0.0f)
            body_def.type = BodyDef::dynamicBody;

        addShapeDef(boost::make_shared<ShapeDef>(shapeName, density, friction, restitution, isSensor));
    }

    setBodyDef(body_def);
}

void CompPhysics::writeToPropertyTree(ptree& propTree) const
{
    // damping element

    if (getLinearDamping() != 0.0f || getAngularDamping() != 0.0f)
    {
        propTree.add("damping.linear", getLinearDamping());
        propTree.add("damping.angular", getAngularDamping());
    }

    if (m_localGravitationPoint != Vector2D(0.0f, 0.0f))
    {
        propTree.add("gravitationPoint.x", m_localGravitationPoint.x);
        propTree.add("gravitationPoint.y", m_localGravitationPoint.y);
    }

    if (isFixedRotation())
    {
        propTree.add("isFixedRotation", true);
    }

    if (isBullet())
    {
        propTree.add("isBullet", true);
    }

    for (ShapeInfoVec::const_iterator it = getShapeInfos().begin(); it != getShapeInfos().end(); ++it)
    {
        ptree shapePropTree;
        shapePropTree.add("comp_id", (*it)->compId);

        if ((*it)->density != 0)
            shapePropTree.add("density", (*it)->density);
        if ((*it)->friction != 0)
            shapePropTree.add("friction", (*it)->friction);
        if ((*it)->restitution != 0)
            shapePropTree.add("restitution", (*it)->restitution);

        if ((*it)->isSensor)
            shapePropTree.add("isSensor", true);
        propTree.add_child("shape", shapePropTree);
    }
}

Vector2D CompPhysics::globalToLocal(const Vector2D& global) const
{
    return m_body->GetWorldVector(*global.to_b2Vec2());
}
