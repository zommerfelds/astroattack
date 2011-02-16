/*
 * World.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Spielwelt
// Speichert alle Entities (und somit auch alle Komponente)

#ifndef WORLD_H
#define WORLD_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include <vector>
#include <map>
#include <string>

class Vector2D;
struct GameEvents;
class Logger;

#include "Entity.h"

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

typedef std::string WorldVariableType;

typedef std::map<const EntityIdType, boost::shared_ptr<Entity> > EntityMap;
typedef std::map<const WorldVariableType, int> WorldVariblesMap;

/*
    Hier ist die Spielwelt gespeichert.
*/
class GameWorld
{
public:
    GameWorld( GameEvents* pEventManager );
    ~GameWorld();

    void AddEntity( const boost::shared_ptr<Entity>& pEntity );
    void RemoveEntity( const EntityIdType& id );
    boost::shared_ptr<Entity> GetEntity( const EntityIdType& id ) const;
    const EntityMap* GetAllEntities() const;

    int GetVariable( const WorldVariableType& varName );
    WorldVariblesMap::iterator GetItToVariable( const WorldVariableType& varName );
    void SetVariable( const WorldVariableType& varName, int value );

    void WriteWorldToLogger( Logger& log );

private:
    GameEvents* m_pGameEvents;

    EntityMap m_entities;
    WorldVariblesMap m_variables;
};

#endif
