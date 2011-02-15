/*----------------------------------------------------------\
|                    CompPhysics.cpp                        |
|                    ---------------                        |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// CompPhysics.h für mehr Informationen

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "CompPhysics.h"
#include <Box2D/Box2D.h>       // extere Physikbibliothek
#include <boost/make_shared.hpp>
#include "../contrib/pugixml/pugixml.hpp"

// eindeutige ID
const CompIdType CompPhysics::COMPONENT_ID = "CompPhysics";

// Konstruktor
CompPhysics::CompPhysics( const BodyDef& rBodyDef ) : m_body ( NULL ),
                                                      m_bodyDef( rBodyDef ),
                                                      m_localRotationPoint (),
                                                      m_localGravitationPoint (),
                                                      m_smoothPosition (),
                                                      m_smoothAngle (0.0f),
                                                      m_gravField ( NULL ),
                                                      m_remainingUpdatesTillGravFieldChangeIsPossible ( 0 )
{
}

CompPhysics::~CompPhysics()
{
}

void CompPhysics::AddShapeDef( const boost::shared_ptr<ShapeDef>& pShapeDef )
{
    m_shapeInfos.push_back( pShapeDef );
}

bool CompPhysics::SetShapeFriction(const CompNameType& shapeName, float friction)
{
    FixtureMap::const_iterator it = m_fixtureMap.find( shapeName );
    if ( it != m_fixtureMap.end() )
        it->second->SetFriction(friction);
    else
        return false;
    return true;
}

float CompPhysics::GetMass() const
{
    return m_body->GetMass();
}

float CompPhysics::GetAngle() const
{
    return m_body->GetAngle();
}

float CompPhysics::GetLinearDamping() const
{
    if (m_body)
        return m_body->GetLinearDamping();
    return m_bodyDef.linearDamping;
}

float CompPhysics::GetAngularDamping() const
{
    if (m_body)
        return m_body->GetAngularDamping();
    return m_bodyDef.angularDamping;
}

bool CompPhysics::IsFixedRotation() const
{
    if (m_body)
        return m_body->IsFixedRotation();
    return m_bodyDef.fixedRotation;
}

bool CompPhysics::IsBullet() const
{
    if (m_body)
        return m_body->IsBullet();
    return m_bodyDef.bullet;
}

Vector2D CompPhysics::GetLinearVelocity() const
{
    return Vector2D( m_body->GetLinearVelocity() );
}

void CompPhysics::SetLinearVelocity(const Vector2D& vel)
{
    m_body->SetLinearVelocity( *vel.To_b2Vec2() );
}

void CompPhysics::ApplyLinearImpulse(const Vector2D& impulse, const Vector2D& point)
{
    m_body->ApplyLinearImpulse(*impulse.To_b2Vec2(), *point.To_b2Vec2());
}

void CompPhysics::ApplyForce(const Vector2D& impulse, const Vector2D& point)
{
    m_body->ApplyForce(*impulse.To_b2Vec2(), *point.To_b2Vec2());
}

void CompPhysics::Rotate( float deltaAngle, const Vector2D& localPoint )
{
    float current_angle = m_body->GetAngle();

    Vector2D worldRotationCenter( Vector2D(m_body->GetPosition()) + localPoint.Rotated(current_angle) );
    Vector2D worldRotationCenterToBodyCenter ( -localPoint.Rotated(current_angle+deltaAngle) );

    m_body->SetTransform( *(worldRotationCenter+worldRotationCenterToBodyCenter).To_b2Vec2(), current_angle+deltaAngle );
}

ContactVector CompPhysics::GetContacts(bool getSensors) const
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

Vector2D CompPhysics::GetCenterOfMassPosition() const
{
    return Vector2D( m_body->GetWorldCenter() );
}

boost::shared_ptr<CompPhysics> CompPhysics::LoadFromXml(const pugi::xml_node& compElem)
{
    pugi::xml_node dampElem = compElem.child("damping");
    float linearDamping = dampElem.attribute("linear").as_float();
    float angularDamping = dampElem.attribute("angular").as_float();

    bool fixedRotation = !compElem.child("fixedRotation").empty();

    bool isBullet = !compElem.child("bullet").empty();

    pugi::xml_node rotPtElem = compElem.child("rotationPoint");
    Vector2D rotationPoint;
    rotationPoint.x = rotPtElem.attribute("x").as_float();
    rotationPoint.y = rotPtElem.attribute("y").as_float();

    pugi::xml_node gravPtElem = compElem.child("gravitationPoint");
    Vector2D gravitationPoint;
    gravitationPoint.x = gravPtElem.attribute("x").as_float();
    gravitationPoint.y = gravPtElem.attribute("y").as_float();

    BodyDef body_def;
    body_def.angularDamping = angularDamping;
    body_def.fixedRotation = fixedRotation;
    body_def.bullet = isBullet;
    body_def.linearDamping = linearDamping;

    boost::shared_ptr<CompPhysics> compPhysics = boost::make_shared<CompPhysics> ();
    compPhysics->SetLocalRotationPoint(rotationPoint);
    compPhysics->SetLocalGravitationPoint(gravitationPoint);

    for (pugi::xml_node shapeElem = compElem.child("shape"); shapeElem; shapeElem = shapeElem.next_sibling("shape"))
    {
        std::string shapeName = shapeElem.attribute("comp_name").value();

        float density = shapeElem.attribute("density").as_float();
        float friction = shapeElem.attribute("friction").as_float();
        float restitution = shapeElem.attribute("restitution").as_float();

        bool isSensor = shapeElem.attribute("isSensor").as_bool();

        if (density != 0.0f)
            body_def.type = BodyDef::dynamicBody;

        compPhysics->AddShapeDef(boost::make_shared<ShapeDef>(shapeName, density, friction, restitution, isSensor));
    }

    compPhysics->SetBodyDef(body_def);
    return compPhysics;
}

void CompPhysics::WriteToXml(pugi::xml_node& compNode) const
{
    // damping element
    pugi::xml_node dampNode = compNode.append_child("damping");
    dampNode.append_attribute("linear").set_value(GetLinearDamping());
    dampNode.append_attribute("angular").set_value(GetAngularDamping());

    if (IsFixedRotation())
    {
        compNode.append_child("fixedRotation");
    }

    if (IsBullet())
    {
        compNode.append_child("isBullet");
    }

    for (ShapeInfoVec::const_iterator it = GetShapeInfos().begin(); it != GetShapeInfos().end(); ++it)
    {
        pugi::xml_node shapeNode = compNode.append_child("shape");
        shapeNode.append_attribute("comp_name").set_value((*it)->compName.c_str());

        shapeNode.append_attribute("density").set_value((*it)->density);
        shapeNode.append_attribute("friction").set_value((*it)->friction);
        shapeNode.append_attribute("restitution").set_value((*it)->restitution);

        if ((*it)->isSensor)
            shapeNode.append_attribute("isSensor").set_value("true");
    }
}

// Astro Attack - Christian Zommerfelds - 2009
