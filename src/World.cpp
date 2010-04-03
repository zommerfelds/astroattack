/*----------------------------------------------------------\
|                        World.cpp                          |
|                        ---------                          |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "World.h"
#include "EventManager.h" // Steuert die Spielerreignisse

// Konstruktor
GameWorld::GameWorld( EventManager* pEventManager )
: m_pEventManager ( pEventManager )
{
}

#include "main.h"
#include "Logger.h"
GameWorld::~GameWorld()
{
    EntityMap::iterator next = m_entities.begin();
    EntityMap::iterator it = next;
    for ( ; it != m_entities.end(); it=next )
    {
        next = it;
        ++next;
        RemoveEntity( it->first );
    }
}

void GameWorld::AddEntity( boost::shared_ptr<Entity> pEntity )
{
    EntityIdType id = pEntity->GetId();
    boost::shared_ptr<Entity> pOldEntity = m_entities[id];
    pOldEntity.reset();
    m_entities[id] = pEntity;

    m_pEventManager->InvokeEvent( Event( NewEntity, pEntity.get() ) );
}

boost::shared_ptr<Entity> GameWorld::GetEntity( EntityIdType id ) const
{
    EntityMap::const_iterator i = m_entities.find( id );
    if ( i == m_entities.end() )
        return boost::shared_ptr<Entity>();
    else
        return i->second;
}

void GameWorld::RemoveEntity( EntityIdType id )
{
    m_pEventManager->InvokeEvent( Event( DeleteEntity, m_entities[id].get() ) );
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

void GameWorld::SetVariable( const WorldVariableType varName, int value )
{
    m_variables[varName] = value;
}

WorldVariblesMap::iterator GameWorld::GetItToVariable( const WorldVariableType& varName )
{
    // es wird nur ein neues Elemen hinzugefügt, falls noch keines existiert
    return m_variables.insert( std::pair<const WorldVariableType, int>( varName, 0 ) ).first;

    /*WorldVariblesMap::iterator it = m_variables.find( varName );
    if ( it == m_variables.end() )
        m_variables.insert( std::pair<const WorldVariableType, int>( varName, 0 ) ).first;
    else
        return m_variables.find( varName );*/
}

const EntityMap* GameWorld::GetAllEntities() const
{
    return &m_entities;
}

// Astro Attack - Christian Zommerfelds - 2009
