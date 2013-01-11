/*
 * ComponentManager.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include "Logger.h"
#include "IdTypes.h"

#include <map>
#include <list>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

class Component;
class GameEvents;
class Logger;

typedef std::list< boost::shared_ptr<Component> > ComponentList;
typedef std::multimap< ComponentTypeId, boost::shared_ptr<Component> > ComponentMap;
typedef std::map< EntityId, ComponentMap > EntityMap;

class Entity
{
public:
    EntityId id;
    ComponentList components;
};

class ComponentManager
{
public:
    ComponentManager(GameEvents& events);
    ~ComponentManager();

    void addEntity(const EntityId& id, ComponentList& components);
    void removeEntity(const EntityId& id);
    const EntityMap& getAllEntities() const;

    bool renameEntity(const EntityId& oldId, const EntityId& newId);

    // use <Component> as template if you don't have the static type
    template <typename CompType>       CompType* getComponent(const EntityId& entId, const ComponentId& compId="");
    template <typename CompType> const CompType* getComponent(const EntityId& entId, const ComponentId& compId="") const;

    // Returns a vector of all matching components in an entity
    template <typename CompType> std::vector<      CompType*> getComponents(const EntityId& id);
    template <typename CompType> std::vector<const CompType*> getComponents(const EntityId& id) const;

    void writeEntitiesToLogger(Logger& logger, LogLevel level);

private:
    GameEvents& m_gameEvents;

    EntityMap m_entities;

    Component* getComponent(const EntityId& entId, const ComponentTypeId& compType, const ComponentId& compId) const;
};

// IMPLEMENTATION
// needs to be implemented here because of templates

#include "Component.h"
template <typename CompType>
CompType* ComponentManager::getComponent(const EntityId& entId, const ComponentId& compId)
{
   return static_cast<CompType*>(getComponent(entId, CompType::getTypeIdStatic(), compId));
}

template <typename CompType>
const CompType* ComponentManager::getComponent(const EntityId& entId, const ComponentId& compId) const
{
   return static_cast<const CompType*>(getComponent(entId, CompType::getTypeIdStatic(), compId));
}

template <typename CompType>
std::vector<CompType*> ComponentManager::getComponents(const EntityId& id)
{
    std::vector<CompType*> ret;

    EntityMap::iterator eit = m_entities.find(id);
    if (eit == m_entities.end())
        return ret;

    ComponentMap::iterator begin;
    ComponentMap::iterator end;
    if (CompType::getTypeIdStatic() == Component::getTypeIdStatic())
    {
        begin = eit->second.begin();
        end = eit->second.end();
    }
    else
    {
        std::pair<ComponentMap::iterator, ComponentMap::iterator> equalRange = eit->second.equal_range(CompType::getTypeIdStatic());
        begin = equalRange.first;
        end = equalRange.second;
    }

    for (ComponentMap::iterator it = begin; it != end; ++it )
        ret.push_back( static_cast<CompType*>(it->second.get()) );
    return ret;
}

template <typename CompType>
std::vector<const CompType*> ComponentManager::getComponents(const EntityId& id) const
{
    std::vector<const CompType*> ret;

    EntityMap::const_iterator eit = m_entities.find(id);
    if (eit == m_entities.end())
        return ret;

    ComponentMap::iterator begin;
    ComponentMap::iterator end;
    if (CompType::getTypeIdStatic() == Component::getTypeIdStatic())
    {
        begin = eit->second.begin();
        end = eit->second.end();
    }
    else
    {
        std::pair<ComponentMap::iterator, ComponentMap::iterator> equalRange = eit->second.equal_range(CompType::getTypeIdStatic());
        begin = equalRange.first;
        end = equalRange.second;
    }

    for (ComponentMap::const_iterator it = begin; it != end; ++it )
        ret.push_back( static_cast<CompType*>(it->second.get()) );
    return ret;
}

#endif /* COMPONENTMANAGER_H */
