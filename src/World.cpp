/*
 * World.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "World.h"
#include "GameEvents.h" // Steuert die Spielerreignisse

// Konstruktor
GameWorld::GameWorld( GameEvents& events )
: m_gameEvents ( events )
{}

GameWorld::~GameWorld()
{
    // NOTE: maybe we shouldn't create events here, since the world is being destroyed anyway...
    EntityMap::iterator next = m_entities.begin();
    EntityMap::iterator it = next;
    for ( ; it != m_entities.end(); it=next )
    {
        next = it;
        ++next;
        removeEntity( it->first );
    }
}

void GameWorld::addEntity( const boost::shared_ptr<Entity>& pEntity )
{
    EntityIdType id = pEntity->getId();
    m_entities[id] = pEntity; // if there is an entity with the same ID before, it will get deleted

    m_gameEvents.newEntity.fire( *pEntity );
}

Entity* GameWorld::getEntity( const EntityIdType& id ) const
{
    EntityMap::const_iterator i = m_entities.find( id );
    if ( i == m_entities.end() )
        return NULL;
    else
        return i->second.get();
}

void GameWorld::removeEntity( const EntityIdType& id )
{
    m_gameEvents.deleteEntity.fire( *m_entities[id] );
    m_entities.erase( id );
}

void GameWorld::writeWorldToLogger( Logger& log )
{
    for ( EntityMap::iterator it = m_entities.begin(); it != m_entities.end(); ++it )
    {
        it->second->writeEntityInfoToLogger( log );
    }
}

int GameWorld::getVariable( const WorldVariableType& varName )
{
    WorldVariblesMap::const_iterator i = m_variables.find( varName );
    if ( i == m_variables.end() )
        return 0;
    else
        return i->second;
}

void GameWorld::setVariable( const WorldVariableType& varName, int value )
{
    m_variables[varName] = value;
}

WorldVariblesMap::iterator GameWorld::getItToVariable( const WorldVariableType& varName )
{
    // es wird ein neues Elemen hinzugefügt, falls noch keines existiert
    return m_variables.insert( std::make_pair( varName, 0 ) ).first;
}

const EntityMap& GameWorld::getAllEntities() const
{
    return m_entities;
}
