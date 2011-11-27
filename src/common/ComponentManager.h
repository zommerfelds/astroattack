/*
 * ComponentManager.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
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
struct GameEvents;
class Logger;

typedef std::list< boost::shared_ptr<Component> > ComponentList;
typedef std::multimap< const ComponentTypeId, boost::shared_ptr<Component> > ComponentMap;
typedef std::map< const EntityIdType, ComponentMap > EntityMap;

class ComponentManager
{
public:
    ComponentManager(GameEvents& events);
    ~ComponentManager();

    void addEntity(const EntityIdType& id, ComponentList& components);
    void removeEntity(const EntityIdType& id);
    const EntityMap& getAllEntities() const;

    // === GetComponent ===
    // There are 2 version of this method:
    // One that returns the component as its specific type using templates
    //   e.g. CompX x = GetComponent<CompX>();
    // And one that returns the component as a base Component type
    //   e.g. Component x = GetComponent("CompX");
    // Prefer using the first one (no need to cast)
    // Use the second version if you can only get the component ID at runtime (as a string)
    template <typename CompType>       CompType* getComponent(const EntityIdType& id);
    template <typename CompType> const CompType* getComponent(const EntityIdType& id) const;
          Component* getComponent(const EntityIdType& id, const ComponentTypeId& compType);
    const Component* getComponent(const EntityIdType& id, const ComponentTypeId& compType) const;

    // === GetComponents ===
    // Returns a vector of all matching components in an entity
    // As GetComponent, this method has multiple versions (see above)
    template <typename CompType> std::vector<      CompType*> getComponents(const EntityIdType& id);
    template <typename CompType> std::vector<const CompType*> getComponents(const EntityIdType& id) const;
    std::vector<      Component*> getComponents(const EntityIdType& id, const ComponentTypeId& compType);
    std::vector<const Component*> getComponents(const EntityIdType& id, const ComponentTypeId& compType) const;
    // Returns all components (regardless of entity)
    const ComponentMap* getComponents(const EntityIdType& id);

    void writeEntitiesToLogger(Logger& logger, LogLevel level);

private:
    GameEvents& m_gameEvents;

    EntityMap m_entities;
};

// needs to be implemented here because of templates
template <typename CompType>
CompType* ComponentManager::getComponent(const EntityIdType& id)
{
   return static_cast<CompType*>( getComponent(id, CompType::COMPONENT_TYPE_ID) );
}

template <typename CompType>
const CompType* ComponentManager::getComponent(const EntityIdType& id) const
{
   return static_cast<const CompType*>( getComponent(id, CompType::COMPONENT_TYPE_ID) );
}

template <typename CompType>
std::vector<CompType*> ComponentManager::getComponents(const EntityIdType& id)
{
    std::vector<CompType*> ret;

    EntityMap::iterator eit = m_entities.find(id);
    if ( eit == m_entities.end() )
        return ret;
    std::pair<ComponentMap::iterator, ComponentMap::iterator> equalRange = eit->second.equal_range(CompType::COMPONENT_TYPE_ID);

    for (ComponentMap::iterator it = equalRange.first; it != equalRange.second; ++it )
        ret.push_back( static_cast<CompType*>(it->second.get()) );
    return ret;
}

template <typename CompType>
std::vector<const CompType*> ComponentManager::getComponents(const EntityIdType& id) const
{
    std::vector<const CompType*> ret;

    EntityMap::const_iterator eit = m_entities.find(id);
    if ( eit == m_entities.end() )
        return ret;
    std::pair<ComponentMap::const_iterator, ComponentMap::const_iterator> equalRange = eit->second.equal_range(CompType::COMPONENT_TYPE_ID);

    for (ComponentMap::const_iterator it = equalRange.first; it != equalRange.second; ++it )
        ret.push_back( static_cast<CompType*>(it->second.get()) );
    return ret;
}

#endif /* COMPONENTMANAGER_H */
