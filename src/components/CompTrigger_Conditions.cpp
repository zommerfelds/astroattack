/*----------------------------------------------------------\
|                CompTrigger_Conditions.cpp                 |
|                --------------------------                 |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "CompTrigger_Conditions.h"
#include "CompPhysics.h"

ConditionCompareVariable::ConditionCompareVariable( std::map<const std::string, int>::iterator itVariable, CompareOperator comp, int numToCompareWith )
: m_itVariable ( itVariable ),
  m_compareType ( comp ),
  m_numToCompareWith ( numToCompareWith )
{
}

bool ConditionCompareVariable::ConditionIsTrue()
{
    switch ( m_compareType )
    {
    case GreaterThan:
        return m_itVariable->second > m_numToCompareWith;
    case GreaterThanOrEqualTo:
        return m_itVariable->second >= m_numToCompareWith;
    case LessThan:
        return m_itVariable->second < m_numToCompareWith;
    case LessThanOrEqualTo:
        return m_itVariable->second <= m_numToCompareWith;
    case EqualTo:
        return m_itVariable->second == m_numToCompareWith;
    case NotEqualTo:
        return m_itVariable->second != m_numToCompareWith;
    default:
        return false;
    }
}

#include <boost/bind.hpp>
#include <Box2D/Box2D.h>
#include "../Entity.h"
#include "../Component.h"

ConditionEntityTouchedThis::ConditionEntityTouchedThis( std::string entityName )
: m_entityName ( entityName )/*, m_touched ( false )*/
{
    //m_registerObj.RegisterListener( ContactAdd, boost::bind( &ConditionEntityTouchedThis::Collision, this, _1 ) );
}

bool ConditionEntityTouchedThis::ConditionIsTrue()
{
    //return m_touched;

    // TODO: all physics components
    CompPhysics* thisCompPhysics = static_cast<CompPhysics*>( m_pCompTrigger->GetOwnerEntity()->GetFirstComponent("CompPhysics") );
    if ( thisCompPhysics == NULL )
        return false; // TODO: handle this

    b2ContactEdge* contactEdge = thisCompPhysics->GetBody()->GetContactList();

    for ( b2ContactEdge* contact_edge = contactEdge; contact_edge; contact_edge = contact_edge->next ) // TODO: refactor
        if ( contact_edge->contact->IsTouching() )
        {
            Component* otherCompPhys[2] = { static_cast<Component*>(  contact_edge->contact->GetFixtureA()->GetUserData() ),
                static_cast<Component*>( contact_edge->contact->GetFixtureB()->GetUserData() ) };
            for (int i=0;i<2;i++)
            {                
                if ( otherCompPhys[i] && otherCompPhys[i]->GetOwnerEntity()->GetId() == m_entityName )
                {
                    return true;
                }
                
            }
        }
    return false;
}

/*void ConditionEntityTouchedThis::Collision( const Event* contactEvent )
{
    if ( contactEvent == NULL )
        return;
    const b2Contact* pContact = static_cast<const b2Contact*>( contactEvent->data );
    if( pContact == NULL )
        return;
    Component* compPhys = static_cast<Component*>( pContact->GetFixtureA()->GetUserData() );
    if ( compPhys )
    {
        if ( compPhys->GetOwnerEntity() == m_pCompTrigger->GetOwnerEntity() )
        {
            Component* comp = static_cast<Component*>( pContact->GetFixtureB()->GetUserData() );
            if ( comp->GetOwnerEntity()->GetId() == m_entityName )
            {
                m_touched = true;
                return;
            }
        }
    }
    compPhys = static_cast<Component*>( pContact->GetFixtureB()->GetUserData() );
    if ( compPhys )
    {
        if ( compPhys->GetOwnerEntity() == m_pCompTrigger->GetOwnerEntity() )
        {
            Component* comp = static_cast<Component*>( pContact->GetFixtureA()->GetUserData() );
            if ( comp->GetOwnerEntity()->GetId() == m_entityName )
            {
                m_touched = true;
                return;
            }
        }
    }
}*/

// Astro Attack - Christian Zommerfelds - 2009
