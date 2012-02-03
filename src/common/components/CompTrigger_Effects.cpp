/*
 * CompTrigger_Effects.cpp
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#include "CompTrigger_Effects.h"

#include "CompVisualMessage.h"
#include "common/ComponentManager.h"
#include "common/Physics.h"
#include "common/GameEvents.h"

#include <sstream>
#include <boost/make_shared.hpp>

EffectKillEntity::EffectKillEntity(GameEvents& gameEvents, const EntityId& entityToKill)
: m_gameEvents (gameEvents), m_entityToKill (entityToKill)
{}

void EffectKillEntity::fire()
{
    m_gameEvents.wantToDeleteEntity.fire(m_entityToKill);
    return;
}

EffectDispMessage::EffectDispMessage(GameEvents& gameEvents, const std::string& message, int timeMs)
: m_gameEvents (gameEvents),
  m_message (message),
  m_totalTimeMs (timeMs)
{}


void EffectDispMessage::fire()
{
    m_gameEvents.dispMessage.fire(m_message, m_totalTimeMs);
}

EffectEndLevel::EffectEndLevel(GameEvents& gameEvents, const std::string& message, bool win ) :
  m_gameEvents (gameEvents),
  m_message( message ),
  m_win ( win )
{}

void EffectEndLevel::fire()
{
    m_gameEvents.levelEnd.fire(m_win, m_message);
}

EffectModifyVariable::EffectModifyVariable(GameEvents& gameEvents, const EntityId& entity, const ComponentId& var, const ModifyId& changeType, int num)
: m_gameEvents (gameEvents),
  m_entity (entity),
  m_var (var),
  m_modId (changeType),
  m_num (num)
{
}

void EffectModifyVariable::fire()
{
    m_gameEvents.variableModification.fire(m_entity, m_var, boost::bind(&EffectModifyVariable::modifyVar, this, _1));
}

int EffectModifyVariable::modifyVar(int x)
{
    switch (m_modId)
    {
    case Set:
        return m_num;
    case Add:
        return x + m_num;
    case Multiply:
        return x * m_num;
    case Divide:
        return x / m_num;
    default:
        assert(false);
        return 0;
    }
}

