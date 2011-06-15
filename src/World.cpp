/*
 * World.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "World.h"

// Konstruktor
World::World( GameEvents& events )
: m_compManager ( events )
{}

int World::getVariable( const WorldVariableId& varName )
{
    WorldVariablesMap::const_iterator i = m_variables.find( varName );
    if ( i == m_variables.end() )
        return 0;
    else
        return i->second;
}

void World::setVariable( const WorldVariableId& varName, int value )
{
    m_variables[varName] = value;
}

WorldVariablesMap::iterator World::getItToVariable( const WorldVariableId& varName )
{
    // es wird ein neues Elemen hinzugefügt, falls noch keines existiert
    return m_variables.insert( std::make_pair( varName, 0 ) ).first;
}
