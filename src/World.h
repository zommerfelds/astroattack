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
    GameWorld( GameEvents& events );
    ~GameWorld();

    void addEntity( const boost::shared_ptr<Entity>& pEntity );
    void removeEntity( const EntityIdType& id );
    Entity* getEntity( const EntityIdType& id ) const;
    const EntityMap& getAllEntities() const;

    int getVariable( const WorldVariableType& varName );
    WorldVariblesMap::iterator getItToVariable( const WorldVariableType& varName );
    void setVariable( const WorldVariableType& varName, int value );

    void writeWorldToLogger( Logger& log );

private:
    GameEvents& m_gameEvents;

    EntityMap m_entities;
    WorldVariblesMap m_variables;
};

#endif
