/*----------------------------------------------------------\
|                   CompGravField.cpp                       |
|                   -----------------                       |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "CompGravField.h"
#include <boost/bind.hpp>
#include <Box2D/Box2D.h>
#include "CompPhysics.h"
#include "../Entity.h"
#include "../Vector2D.h"
//#include "../main.h"

// eindeutige ID
CompIdType CompGravField::m_componentId = "CompGravField";

// Konstruktor
CompGravField::CompGravField() : m_gravType ( Directional ), m_pGravitationDir ( new Vector2D ),
m_pGravitationCenter ( new Vector2D ), m_priority( 50 )
{
    m_pGravitationDir->Set( 0.0, 1.0 );
    m_pGravitationCenter->Set( 0.0, 1.0 );

    //m_registerObj1.RegisterListener( ContactPersist, boost::bind( &CompGravField::OnGameUpdate, this, _1 ) );
    //m_registerObj1.RegisterListener( ContactPersist, boost::bind( &CompGravField::Contact, this, _1 ) );
    //m_registerObj2.RegisterListener( ContactAdd, boost::bind( &CompGravField::DeleteContact, this, _1 ) );
}

CompGravField::~CompGravField()
{
}

/*void CompGravField::OnGameUpdate( const Event* contactEvent )
{
}*/

#if 0
void CompGravField::Contact( const Event* contactEvent )
{
    if ( contactEvent == NULL )
        return;
    const b2Contact* pContact = static_cast<const b2Contact*>( contactEvent->data );
    if( pContact == NULL )
        return;

    CompPhysics* fieldCompPhysics = static_cast<CompPhysics*>( GetOwnerEntity()->GetFirstComponent("CompPhysics") );
    if ( fieldCompPhysics == NULL )
        return; // keine Physikkomponente, also abbrechen

    const b2Body* pBody1 = pContact->GetFixtureA()->GetBody();
    const b2Body* pBody2 = pContact->GetFixtureB()->GetBody();

    const b2Body* pBodyContact = NULL;

    if ( pBody1 == fieldCompPhysics->GetBody() )
        pBodyContact = pBody2;
    else if ( pBody2 == fieldCompPhysics->GetBody() )
        pBodyContact = pBody1;
    else
        return;

    // Wenn die Mitte des Körpers im Gravitationsfeld ist
    if ( fieldCompPhysics->GetFixture()->TestPoint( pBodyContact->GetWorldCenter() ) == true )
    {
        /*CompPhysics* phys = static_cast<CompPhysics*>(pBodyContact->GetUserData());
        CompGravField* gravField;
        int updatesSinceLastGravFieldChange = 0;
        bool gravFieldIsChanging = false;

        if ( phys->GetGravFieldInfo(const CompGravField* gravField, int& updatesSinceLastGravFieldChange, bool& gravFieldIsChanging) ) {
        m_bodiesInField.insert( pBodyContact );
        phys->SetGravField( this );
        }*/
    }
}
#endif

void CompGravField::SetGravDir(const Vector2D& dir)
{
    *m_pGravitationDir = dir;
}

void CompGravField::SetGravCenter(const Vector2D& center, float strenght)
{
    *m_pGravitationCenter = center;
    m_strenght = strenght;
}

void CompGravField::SetPriority( unsigned int priority )
{
    if ( priority > 1000 )
        return; // ACHTUNG: sende warnung
    m_priority = priority;
}

// Get the acceleration of a body with center of mass "centerOfMass"
Vector2D CompGravField::GetAcceleration(const Vector2D& centerOfMass) const
{
    if ( m_gravType == Directional ) 
    {
        return GetGravDir();
    }
    else if ( m_gravType == Radial )
    {
        Vector2D acc( GetGravCenter()-centerOfMass );
        acc.Normalise();
        acc *= m_strenght;
        return acc;
    }
    return Vector2D( 0.0f, 0.0f );
}

/*void CompGravField::ApplyForces() const
{
    for ( std::set<b2Body*>::const_iterator it = m_bodiesInField.begin(); it != m_bodiesInField.end(); ++it )
    {
        //std::string entityId = static_cast<CompPhysics*>((*it)->GetUserData())->GetOwnerEntity()->GetId();
        //gAaLog.Write ( "===> Body in GravField: %s\n", entityId.c_str() );

        Vector2D acc = GetAcceleration( Vector2D( (*it)->GetWorldCenter() ) );
        (*it)->ApplyForce( (*it)->GetMass()*b2Vec2(acc.x,acc.y), (*it)->GetWorldCenter() );
    }
}*/

/*void CompGravField::ClearBodiesInField()
{
    for ( std::set<b2Body*>::const_iterator it = m_bodiesInField.begin(); it != m_bodiesInField.end(); ++it )
    {
        CompPhysics* phys = static_cast<CompPhysics*>((*it)->GetUserData());
        //phys->SetGravField( NULL );
    }
    m_bodiesInField.clear();
}*/

// Astro Attack - Christian Zommerfelds - 2009
