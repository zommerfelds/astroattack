/*----------------------------------------------------------\
|                  CompCollectable.cpp                      |
|                  -------------------                      |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "CompCollectable.h"
#include <boost/bind.hpp>
#include <Box2D/Box2D.h>
#include "../Entity.h"

// eindeutige ID
const CompIdType CompCollectable::m_componentId = "CompCollectable";

// Konstruktor
CompCollectable::CompCollectable( std::map<const std::string, int>::iterator itCollectableVariable )
: m_eventConnection (),
  m_wasCollected ( false ),
  m_itCollectableVariable ( itCollectableVariable )
{
    //m_eventConnection = gameEvents->.RegisterListener( ContactAdd, boost::bind( &CompCollectable::Collision, this, _1 ) );
}

CompCollectable::~CompCollectable()
{
}

// Is einer der zwei Entities ich selber und der Spieler?
// Wenn ja, dann wurde dieses Objekt gesammelt.
void CompCollectable::OnCollision() // callback wird gelöscht und iterator wird ungültig
{
#if 0
    if ( contactEvent == NULL )
        return;
    if ( m_wasCollected )
        return;
    const b2Contact* pContact = static_cast<const b2Contact*>( contactEvent->data );
    if( pContact == NULL )
        return;
    Component* compPhys = static_cast<Component*>( pContact->GetFixtureA()->GetUserData() );
    if ( compPhys )
    {
        if ( compPhys->GetOwnerEntity() == GetOwnerEntity() )
        {
            Component* comp = static_cast<Component*>( pContact->GetFixtureB()->GetUserData() );
            if ( comp->GetOwnerEntity()->GetId() == "Player" )
            {
                ++m_itCollectableVariable->second;
                gameEvents->InvokeEvent( Event(WantToDeleteEntity,GetOwnerEntity()) );
                m_wasCollected = true;
                return;
            }
        }
    }
    compPhys = static_cast<Component*>( pContact->GetFixtureB()->GetUserData() );
    if ( compPhys )
    {
        if ( compPhys->GetOwnerEntity() == GetOwnerEntity() )
        {
            Component* comp = static_cast<Component*>( pContact->GetFixtureA()->GetUserData() );
            if ( comp->GetOwnerEntity()->GetId() == "Player" )
            {
                ++m_itCollectableVariable->second;
                gameEvents->InvokeEvent( Event(WantToDeleteEntity,GetOwnerEntity()) );
                m_wasCollected = true;
                return;
            }
        }
    }
#endif
}

// Astro Attack - Christian Zommerfelds - 2009
