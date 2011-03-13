/*
 * World.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Spielwelt
// Speichert alle Entities (und somit auch alle Komponente)

#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

#include "Entity.h"

class Vector2D;
struct GameEvents;
class Logger;

typedef std::string WorldVariableId;
typedef std::map<const EntityIdType, boost::shared_ptr<Entity> > EntityMap;
typedef std::map<const WorldVariableId, int> WorldVariablesMap;

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

    int getVariable( const WorldVariableId& varName );
    WorldVariablesMap::iterator getItToVariable( const WorldVariableId& varName );
    void setVariable( const WorldVariableId& varName, int value );

    void writeWorldToLogger( Logger& log );

private:
    GameEvents& m_gameEvents;

    EntityMap m_entities;
    WorldVariablesMap m_variables;
};

#endif
