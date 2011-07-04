/*
 * CompTrigger_Effects.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include <sstream>
#include <boost/make_shared.hpp>

#include "CompTrigger_Effects.h"
#include "CompVisualMessage.h"
#include "../ComponentManager.h"
#include "../Physics.h"

// ========= KillEntity ===========
EffectKillEntity::EffectKillEntity(GameEvents& gameEvents, std::string entityToKill)
: m_gameEvents (gameEvents), m_entityToKill ( entityToKill )
{}

void EffectKillEntity::fire()
{
    m_gameEvents.wantToDeleteEntity.fire( m_entityToKill );
    return;
}

// ========= DispMessage ===========
EffectDispMessage::EffectDispMessage(GameEvents& gameEvents, std::string message, int timeMs, ComponentManager& compManager)
: m_gameEvents (gameEvents),
  m_message (message),
  m_remainingUpdates ( (int)((float)timeMs*0.001f/cPhysicsTimeStep) ),
  m_fired (false),
  m_compManager ( compManager ),
  m_msgEntityId (),
  m_totalTimeMs ( timeMs )
{}


void EffectDispMessage::fire()
{
    m_fired = true;
    std::string entityId;
    for ( int i = 0;; ++i )
    {
        std::stringstream ss;
        ss << "_Message" << i;
        if ( m_compManager.getAllEntities().count(ss.str()) == 0 )
        {
            entityId = ss.str();
            break;
        }
    }
    ComponentList entity;
    m_msgEntityId = entityId;
    boost::shared_ptr<CompVisualMessage> compMsg = boost::shared_ptr<CompVisualMessage>(new CompVisualMessage("EffectDispMessage", m_gameEvents, m_message));
    entity.push_back(compMsg);

    m_compManager.addEntity(entityId, entity);
}

bool EffectDispMessage::update()
{
    --m_remainingUpdates;
    if ( m_remainingUpdates == 0 )
    {
        m_gameEvents.wantToDeleteEntity.fire(m_msgEntityId);
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
    if ( m_fired && m_remainingUpdates > 0 && m_compManager.getAllEntities().count(m_msgEntityId) != 0 )
        m_gameEvents.wantToDeleteEntity.fire(m_msgEntityId);
}

// ========= EndLevel ===========
EffectEndLevel::EffectEndLevel(GameEvents& gameEvents, std::string message, bool win ) :
  m_gameEvents (gameEvents),
  m_message( message ),
  m_win ( win )
{}

void EffectEndLevel::fire()
{
    m_gameEvents.levelEnd.fire(m_win, m_message);
}

// ========= ChangeVariable ===========
EffectChangeVariable::EffectChangeVariable(std::map<const std::string,int>::iterator itVariable, const ChangeType& changeType, int num )
: m_itVariable ( itVariable ),
  m_changeType ( changeType ),
  m_num ( num )
{
}

void EffectChangeVariable::fire()
{
    switch ( m_changeType )
    {
    case Set:
        m_itVariable->second = m_num;
        break;
    case Add:
        m_itVariable->second += m_num;
        break;
    case Multiply:
        m_itVariable->second *= m_num;
        break;
    case Divide:
        m_itVariable->second /= m_num;
        break;
    default:
        break;
    }
}
