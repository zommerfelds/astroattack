/*
 * World.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

// Spielwelt
// Speichert alle Entities (und somit auch alle Komponente)

#ifndef WORLD_H
#define WORLD_H

#include "common/ComponentManager.h"
#include <map>
#include <string>

struct GameEvents;

typedef std::string WorldVariableId;
typedef std::map<const WorldVariableId, int> WorldVariablesMap;

/*
    Hier ist die Spielwelt gespeichert.
*/
class World
{
public:
    World( GameEvents& events );

    int getVariable( const WorldVariableId& varName );
    WorldVariablesMap::iterator getItToVariable( const WorldVariableId& varName );
    void setVariable( const WorldVariableId& varName, int value );

    ComponentManager& getCompManager() { return m_compManager; }
    const ComponentManager& getCompManager() const { return m_compManager; }

private:
    WorldVariablesMap m_variables;
    ComponentManager m_compManager;
};

#endif
