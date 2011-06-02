/*
 * CompTrigger_Effects.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include <sstream>
#include <boost/make_shared.hpp>

#include "CompTrigger_Effects.h"
#include "CompVisualMessage.h"
#include "../World.h"
#include "../Physics.h"

// ========= KillEntity ===========
EffectKillEntity::EffectKillEntity(GameEvents& gameEvents, std::string entityToKill, const GameWorld& world)
: m_gameEvents (gameEvents), m_entityToKill ( entityToKill ), m_world ( world )
{}

void EffectKillEntity::fire()
{
    m_gameEvents.wantToDeleteEntity.fire( *m_world.getEntity(m_entityToKill) );
    return;
}

// ========= DispMessage ===========
EffectDispMessage::EffectDispMessage(GameEvents& gameEvents, std::string message, int timeMs, GameWorld& world)
: m_gameEvents (gameEvents),
  m_message (message),
  m_remainingUpdates ( (int)((float)timeMs*0.001f/cPhysicsTimeStep) ),
  m_fired (false),
  m_world ( world ),
  m_pMsgEntity (NULL),
  m_totalTimeMs ( timeMs )
{}


void EffectDispMessage::fire()
{
    m_fired = true;
    std::string entityName;
    for ( int i = 0;; ++i )
    {
        std::stringstream ss;
        ss << "_Message" << i;
        if ( !m_world.getEntity( ss.str() ) )
        {
            entityName = ss.str();
            break;
        }
    }
    m_msgEntityName = entityName;
    boost::shared_ptr<Entity> pEntity = boost::make_shared<Entity>(entityName);
    m_pMsgEntity = pEntity.get();
    boost::shared_ptr<CompVisualMessage> compMsg = boost::shared_ptr<CompVisualMessage>(new CompVisualMessage(m_gameEvents, m_message));
    compMsg->setId( "autoname" );
    pEntity->addComponent( compMsg );

    m_world.addEntity( pEntity );
}

bool EffectDispMessage::update()
{
    --m_remainingUpdates;
    if ( m_remainingUpdates == 0 && m_pMsgEntity != NULL )
    {
        m_gameEvents.wantToDeleteEntity.fire(*m_pMsgEntity);
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
    if ( m_pMsgEntity && m_fired && m_remainingUpdates > 0 && m_world.getEntity(m_msgEntityName) )
        m_gameEvents.wantToDeleteEntity.fire(*m_pMsgEntity);
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
        break;;
    }
}
