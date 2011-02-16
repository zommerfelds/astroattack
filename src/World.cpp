/*
 * World.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "World.h"
#include "GameEvents.h" // Steuert die Spielerreignisse

// Konstruktor
GameWorld::GameWorld( GameEvents* pEventManager )
: m_pGameEvents ( pEventManager )
{
}

#include "main.h"
#include "Logger.h"
GameWorld::~GameWorld()
{
    // NOTE: maybe we shouldn't create events here, since the world is being destroyed anyway...
    EntityMap::iterator next = m_entities.begin();
    EntityMap::iterator it = next;
    for ( ; it != m_entities.end(); it=next )
    {
        next = it;
        ++next;
        RemoveEntity( it->first );
    }
}

void GameWorld::AddEntity( const boost::shared_ptr<Entity>& pEntity )
{
    EntityIdType id = pEntity->GetId();
    m_entities[id] = pEntity; // if there is an entity with the same ID before, it will get deleted

    m_pGameEvents->newEntity.Fire( *pEntity );
}

boost::shared_ptr<Entity> GameWorld::GetEntity( const EntityIdType& id ) const
{
    EntityMap::const_iterator i = m_entities.find( id );
    if ( i == m_entities.end() )
        return boost::shared_ptr<Entity>();
    else
        return i->second;
}

void GameWorld::RemoveEntity( const EntityIdType& id )
{
    m_pGameEvents->deleteEntity.Fire( *m_entities[id] );
    m_entities.erase( id );
}

void GameWorld::WriteWorldToLogger( Logger& log )
{
    for ( EntityMap::iterator it = m_entities.begin(); it != m_entities.end(); ++it )
    {
        it->second->WriteEntityInfoToLogger( log );
    }
}

int GameWorld::GetVariable( const WorldVariableType& varName )
{
    WorldVariblesMap::const_iterator i = m_variables.find( varName );
    if ( i == m_variables.end() )
        return 0;
    else
        return i->second;
}

void GameWorld::SetVariable( const WorldVariableType& varName, int value )
{
    m_variables[varName] = value;
}

WorldVariblesMap::iterator GameWorld::GetItToVariable( const WorldVariableType& varName )
{
    // es wird nur ein neues Elemen hinzugefügt, falls noch keines existiert
    return m_variables.insert( std::make_pair( varName, 0 ) ).first;

    /*WorldVariblesMap::iterator it = m_variables.find( varName );
    if ( it == m_variables.end() )
        m_variables.insert( std::make_pair( varName, 0 ) ).first;
    else
        return m_variables.find( varName );*/
}

const EntityMap* GameWorld::GetAllEntities() const
{
    return &m_entities;
}
