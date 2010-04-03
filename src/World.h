/*----------------------------------------------------------\
|                         World.h                           |
|                         -------                           |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// Spielwelt
// Speichert alle Entities (und somit auch alle Komponente)

#ifndef WORLD_H
#define WORLD_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include <vector>
#include <map>
#include <string>

class Vector2D;
class EventManager;
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
    GameWorld( EventManager* pEventManager );
    ~GameWorld();

    void AddEntity( boost::shared_ptr<Entity> pEntity );
    void RemoveEntity( EntityIdType id );
    boost::shared_ptr<Entity> GetEntity( EntityIdType id ) const;
    const EntityMap* GetAllEntities() const;

    int GetVariable( const WorldVariableType& varName );
    WorldVariblesMap::iterator GetItToVariable( const WorldVariableType& varName );
    void SetVariable( const WorldVariableType varName, int value );

    void WriteWorldToLogger( Logger& log );

private:
    EventManager* m_pEventManager;

    EntityMap m_entities;
    WorldVariblesMap m_variables;
};

#endif

// Astro Attack - Christian Zommerfelds - 2009
