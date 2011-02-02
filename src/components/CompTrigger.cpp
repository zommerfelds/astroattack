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
const CompIdType CompTrigger::COMPONENT_ID = "CompTrigger";

CompTrigger::CompTrigger() : m_fired ( false )
{
    m_eventConnection = gameEvents->gameUpdate.RegisterListener( boost::bind( &CompTrigger::OnUpdate, this ) );
}

CompTrigger::~CompTrigger()
{
}

void CompTrigger::OnUpdate()
{
    if ( !m_fired )
    {
        bool conditionsAreTrue = true;

        for ( size_t i = 0; i < m_conditions.size(); ++i )
        {
            if ( !m_conditions[i]->ConditionIsTrue() )
            {
                conditionsAreTrue = false;
                break;
            }
        }

        if (conditionsAreTrue)
        {
            for ( size_t i = 0; i < m_effects.size(); ++i )
            {
                m_effects[i]->Fire();
            }
            m_fired = true;
        }
    }
    else
    {
        for ( size_t i = 0; i < m_effects.size(); ++i )
        {
            m_effects[i]->Update();
        }
    }
}

void CompTrigger::AddCondition( const boost::shared_ptr<Condition>& pCond )
{
    pCond->m_pCompTrigger = this;
    m_conditions.push_back( pCond );
}

void CompTrigger::AddEffect( const boost::shared_ptr<Effect>& pTrig )
{
    pTrig->m_pCompTrigger = this;
    m_effects.push_back( pTrig );
}

// Astro Attack - Christian Zommerfelds - 2009
