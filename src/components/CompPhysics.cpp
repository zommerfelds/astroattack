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
                                                      //m_oldGravField ( NULL ),
                                                      //m_gravFieldIsChanging ( false ),
                                                      //m_updatesSinceLastGravFieldChange ( 0 )
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

/*bool CompPhysics::GetSaveContacts() const
{
    return m_saveContacts;
}*/

/*void CompPhysics::GetGravFieldInfo(const CompGravField* pGravField, int* pUpdatesSinceLastGravFieldChange, bool* pGravFieldIsChanging) const
{
    pGravField = m_gravField;
    pUpdatesSinceLastGravFieldChange = m_updatesSinceLastGravFieldChange;
    pGravFieldIsChanging = gravFieldIsChanging;
}

void SetGravField(const CompGravField* pGravField)
{
    if ( m_gravField != NULL )
		m_oldGravField = m_gravField;
	m_gravField = gravField;
	m_framesTillSwitchGravFieldIsPossible = cUpdatesTillGravFieldChangeIsPossible;
}*/

// Astro Attack - Christian Zommerfelds - 2009
