/*----------------------------------------------------------\
|                 CompTrigger_Effects.cpp                   |
|                 -----------------------                   |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "CompTrigger_Effects.h"
#include "../World.h"

EffectKillEntity::EffectKillEntity( std::string entityToKill, GameWorld* pGW )
: m_entityToKill ( entityToKill ), m_pGW ( pGW )
{
}

void EffectKillEntity::Fire()
{
    m_pCompTrigger->eventManager->InvokeEvent( Event(WantToDeleteEntity,m_pGW->GetEntity(m_entityToKill).get()) );
    return;
}

#include "../Physics.h"
#include "CompVisualMessage.h"
#include <sstream>
EffectDispMessage::EffectDispMessage( std::string message, int timeMs, GameWorld* pGW )
: m_message (message), m_remainingUpdates ( (int)((float)timeMs*0.001f/PHYS_DELTA_TIME) ), m_fired (false), m_pGW ( pGW ), m_totalTimeMs ( timeMs )
{
}


void EffectDispMessage::Fire()
{
    m_fired = true;
    std::string entityName;
    for ( int i = 0;; ++i )
    {
        std::stringstream ss;
        ss << "_Message" << i;
        if ( !m_pGW->GetEntity( ss.str() ) )
        {
            entityName = ss.str();
            break;
        }
    }
    m_msgEntityName = entityName;
    boost::shared_ptr<Entity> pEntity( new Entity( entityName ) );
    m_pMsgEntity = pEntity.get();
    boost::shared_ptr<CompVisualMessage> compMsg( new CompVisualMessage(m_message) );
    compMsg->SetName( "autoname" );
    pEntity->SetComponent( compMsg );

    m_pGW->AddEntity( pEntity );
}

bool EffectDispMessage::Update()
{
    --m_remainingUpdates;
    if ( m_remainingUpdates == 0 )
    {
        m_pCompTrigger->eventManager->InvokeEvent( Event(WantToDeleteEntity, m_pMsgEntity) );
        return true;
    }
    else if ( m_remainingUpdates < 0 )
    {
        m_remainingUpdates = -1;
        return true;
    }
    return false;
}

EffectDispMessage::~EffectDispMessage()
{
    if ( m_pMsgEntity && m_fired && m_remainingUpdates > 0 && m_pGW->GetEntity(m_msgEntityName) )
        m_pCompTrigger->eventManager->InvokeEvent( Event(WantToDeleteEntity, m_pMsgEntity) );
}

void EffectEndLevel::Fire()
{
    if ( m_win )
        m_pCompTrigger->eventManager->InvokeEvent( Event(LevelEnd_Win, (void*)m_message.c_str()) );
    else
        m_pCompTrigger->eventManager->InvokeEvent( Event(LevelEnd_Lose, (void*)m_message.c_str()) );
}

/*void EffectLoseLevel::Fire()
{
    m_pCompTrigger->eventManager->InvokeEvent( Event(LevelEnd_Lose, (void*)m_message.c_str()) );
}*/

EffectChangeVariable::EffectChangeVariable(std::map<const std::string,int>::iterator itVariable, ChangeType changeType, int num )
: m_itVariable ( itVariable ),
  m_changeType ( changeType ),
  m_num ( num )
{
}

void EffectChangeVariable::Fire()
{
    switch ( m_changeType )
    {
    case Set:
        m_itVariable->second = m_num;
        break;
    case Increase:
        m_itVariable->second += m_num;
        break;
    case Multiply:
        m_itVariable->second *= m_num;
        break;
    case Divide:
        m_itVariable->second /= m_num;
        break;
    default:
        break;;
    }
}

// Astro Attack - Christian Zommerfelds - 2009
