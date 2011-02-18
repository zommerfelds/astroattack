/*
 * CompTrigger_Effects.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "CompTrigger_Effects.h"
#include "../World.h"
#include <boost/make_shared.hpp>
#include "../Physics.h"
#include "CompVisualMessage.h"
#include <sstream>

// ========= KillEntity ===========
EffectKillEntity::EffectKillEntity( std::string entityToKill, const GameWorld& world )
: m_entityToKill ( entityToKill ), m_world ( world )
{}

void EffectKillEntity::Fire()
{
    m_pCompTrigger->gameEvents->wantToDeleteEntity.Fire( *m_world.GetEntity(m_entityToKill) );
    return;
}

// ========= DispMessage ===========
EffectDispMessage::EffectDispMessage( std::string message, int timeMs, GameWorld& world )
: m_message (message),
  m_remainingUpdates ( (int)((float)timeMs*0.001f/PHYS_DELTA_TIME) ),
  m_fired (false),
  m_world ( world ),
  m_pMsgEntity (NULL),
  m_totalTimeMs ( timeMs )
{}


void EffectDispMessage::Fire()
{
    m_fired = true;
    std::string entityName;
    for ( int i = 0;; ++i )
    {
        std::stringstream ss;
        ss << "_Message" << i;
        if ( !m_world.GetEntity( ss.str() ) )
        {
            entityName = ss.str();
            break;
        }
    }
    m_msgEntityName = entityName;
    boost::shared_ptr<Entity> pEntity = boost::make_shared<Entity>(entityName);
    m_pMsgEntity = pEntity.get();
    boost::shared_ptr<CompVisualMessage> compMsg = boost::make_shared<CompVisualMessage>(m_message);
    compMsg->SetName( "autoname" );
    pEntity->AddComponent( compMsg );

    m_world.AddEntity( pEntity );
}

bool EffectDispMessage::Update()
{
    --m_remainingUpdates;
    if ( m_remainingUpdates == 0 && m_pMsgEntity != NULL )
    {
        m_pCompTrigger->gameEvents->wantToDeleteEntity.Fire(*m_pMsgEntity);
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
    if ( m_pMsgEntity && m_fired && m_remainingUpdates > 0 && m_world.GetEntity(m_msgEntityName) )
        m_pCompTrigger->gameEvents->wantToDeleteEntity.Fire(*m_pMsgEntity);
}

// ========= EndLevel ===========
void EffectEndLevel::Fire()
{
    m_pCompTrigger->gameEvents->levelEnd.Fire(m_win, m_message);
}

// ========= ChangeVariable ===========
EffectChangeVariable::EffectChangeVariable(std::map<const std::string,int>::iterator itVariable, const ChangeType& changeType, int num )
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
