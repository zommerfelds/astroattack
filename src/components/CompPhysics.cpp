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

// Number of game updates a CompPhysics has to wait till it can change to an other GravField
const unsigned int cUpdatesTillGravFieldChangeIsPossible = 10;

// eindeutige ID
const CompIdType CompPhysics::m_componentId = "CompPhysics";

// Konstruktor
CompPhysics::CompPhysics( b2BodyDef* pBodyDef ) : m_body ( NULL ),
                                                  m_bodyDef( pBodyDef ),
                                                  m_localRotationPoint (),
                                                  m_localGravitationPoint (),
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

void CompPhysics::AddFixtureDef( const boost::shared_ptr<b2FixtureDef>& pFixtureDef, FixtureIdType name )
{
    // shape zur Liste hinzufügen
    m_fixtureDefs.push_back( std::make_pair(name,pFixtureDef) );
}

const b2Body* CompPhysics::GetBody() const
{
    return m_body;
}

b2Body* CompPhysics::GetBody()
{
    return m_body;
}

b2Fixture* CompPhysics::GetFixture() const
{
    return m_body->GetFixtureList();
}

b2Fixture* CompPhysics::GetFixture( FixtureIdType name ) const
{
    FixtureMap::const_iterator it = m_fixtureList.find( name );
    if ( it != m_fixtureList.end() )
        return it->second.pFixture;
    else
        return NULL;
}

bool CompPhysics::IsAllowedToSleep() const
{
    if (m_bodyDef)
        return m_bodyDef->allowSleep;
    else
        return false;
}

float CompPhysics::GetLinearDamping() const
{
    if (m_bodyDef)
        return m_bodyDef->linearDamping;
    else
        return 0.0f;
}

float CompPhysics::GetAngularDamping() const
{
    if (m_bodyDef)
        return m_bodyDef->angularDamping;
    else
        return 0.0f;
}

bool CompPhysics::IsFixedRotation() const
{
    if (m_bodyDef)
        return m_bodyDef->fixedRotation;
    else
        return false;
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
