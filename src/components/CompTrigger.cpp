/*----------------------------------------------------------\
|                    CompTrigger.cpp                        |
|                    ---------------                        |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "CompTrigger.h"
#include <boost/bind.hpp>

// eindeutige ID
CompIdType CompTrigger::m_componentId = "CompTrigger";

// Nur um Warnungen zu vermeiden (dass x nicht benutzt wird)
#define NOT_USED(x) x

CompTrigger::CompTrigger() : m_fired ( false )
{
    m_registerObj.RegisterListener( GameUpdate, boost::bind( &CompTrigger::Update, this, _1 ) );
}

CompTrigger::~CompTrigger()
{
}

void CompTrigger::Update( const Event* gameUpdatedEvent )
{
    NOT_USED(gameUpdatedEvent); // gameUpdatedEvent wird nie gebraucht (NOT_USED is um Warnungen zu vermeiden)

    if ( !m_fired )
    {
        bool conditionsAreTrue = true;

        for ( unsigned int i = 0; i < m_conditions.size(); ++i )
        {
            if ( !m_conditions[i]->ConditionIsTrue() )
            {
                conditionsAreTrue = false;
                break;
            }
        }

        if (conditionsAreTrue)
        {
            for ( unsigned int i = 0; i < m_effects.size(); ++i )
            {
                m_effects[i]->Fire();
            }
            m_fired = true;
        }
    }
    else
    {
        for ( unsigned int i = 0; i < m_effects.size(); ++i )
        {
            m_effects[i]->Update();
        }
    }
}

void CompTrigger::AddCondition( boost::shared_ptr<Condition> pCond )
{
    pCond->m_pCompTrigger = this;
    m_conditions.push_back( pCond );
}

void CompTrigger::AddEffect( boost::shared_ptr<Effect> pTrig )
{
    pTrig->m_pCompTrigger = this;
    m_effects.push_back( pTrig );
}

// Astro Attack - Christian Zommerfelds - 2009
