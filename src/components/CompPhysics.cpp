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

#include "CompPhysics.h"

using boost::property_tree::ptree;

// eindeutige ID
const ComponentTypeId CompPhysics::COMPONENT_TYPE_ID = "CompPhysics";

// Konstruktor
CompPhysics::CompPhysics(const BodyDef& rBodyDef) :
    m_body (NULL),
    m_bodyDef (rBodyDef),
    m_localRotationPoint (),
    m_localGravitationPoint (),
    m_smoothPosition (),
    m_smoothAngle (0.0f),
    m_gravField (NULL),
    m_remainingUpdatesTillGravFieldChangeIsPossible (0)
{}

void CompPhysics::addShapeDef( const boost::shared_ptr<ShapeDef>& pShapeDef )
{
    m_shapeInfos.push_back( pShapeDef );
}

bool CompPhysics::setShapeFriction(const ComponentId& shapeName, float friction)
{
    FixtureMap::const_iterator it = m_fixtureMap.find( shapeName );
    if ( it != m_fixtureMap.end() )
        it->second->SetFriction(friction);
    else
        return false;
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
    float current_angle = m_body->GetAngle();

    Vector2D worldRotationCenter( Vector2D(m_body->GetPosition()) + localPoint.rotated(current_angle) );
    Vector2D worldRotationCenterToBodyCenter ( -localPoint.rotated(current_angle+deltaAngle) );

    m_body->SetTransform( *(worldRotationCenter+worldRotationCenterToBodyCenter).to_b2Vec2(), current_angle+deltaAngle );
}


const Vector2D CompPhysics::getPosition() const
{
    return m_body->GetPosition();
}

const Vector2D& CompPhysics::getSmoothPosition() const
{
    return m_smoothPosition;
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
        boost::shared_ptr<ContactInfo> touchInfo = boost::make_shared<ContactInfo>();
        b2WorldManifold worldManifold;
        contactEdge->contact->GetWorldManifold( &worldManifold );
        touchInfo->normal = worldManifold.normal;
        if (contactEdge->contact->GetFixtureA()->GetBody() != m_body) // is this needed?
            touchInfo->normal = -touchInfo->normal;
        touchInfo->comp = static_cast<CompPhysics*>(contactEdge->other->GetUserData());
        touchInfo->point = worldManifold.points[0]; // TODO: use all points

        vecTouchInfo.push_back(touchInfo);
    }
    return vecTouchInfo;
}

Vector2D CompPhysics::getSmoothCenterOfMassPosition() const
{
    return m_smoothPosition + Vector2D(m_body->GetLocalCenter()).rotated(m_smoothAngle);
}

Vector2D CompPhysics::getCenterOfMassPosition() const
{
    return Vector2D( m_body->GetWorldCenter() );
}

void CompPhysics::loadFromPropertyTree(const ptree& propTree)
{
    float linearDamping = propTree.get("damping.linear", 0.0f);
    float angularDamping = propTree.get("damping.angular", 0.0f);

    bool fixedRotation = propTree.get("isFixedRotation", false);

    bool isBullet = propTree.get("isBullet", false);

    Vector2D rotationPoint;
    rotationPoint.x = propTree.get("rotationPoint.x", 0.0f);
    rotationPoint.y = propTree.get("rotationPoint.y", 0.0f);

    Vector2D gravitationPoint;
    gravitationPoint.x = propTree.get("gravitationPoint.x", 0.0f);
    gravitationPoint.y = propTree.get("gravitationPoint.y", 0.0f);

    BodyDef body_def;
    body_def.angularDamping = angularDamping;
    body_def.fixedRotation = fixedRotation;
    body_def.bullet = isBullet;
    body_def.linearDamping = linearDamping;

    setLocalRotationPoint(rotationPoint);
    setLocalGravitationPoint(gravitationPoint);

    BOOST_FOREACH(const ptree::value_type &v, propTree)
    {
        if (v.first != "shape")
            continue;

        const ptree& shapeProps = v.second;
        std::string shapeName = shapeProps.get<std::string>("comp_name");

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

    propTree.add("damping.linear", getLinearDamping());
    propTree.add("damping.angular", getAngularDamping());

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
        shapePropTree.add("comp_name", (*it)->compName);

        shapePropTree.add("density", (*it)->density);
        shapePropTree.add("friction", (*it)->friction);
        shapePropTree.add("restitution", (*it)->restitution);

        if ((*it)->isSensor)
            shapePropTree.add("isSensor", true);
        propTree.add_child("shape", shapePropTree);
    }
}
