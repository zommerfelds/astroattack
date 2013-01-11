/*
 * ComponentManager.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#include "ComponentManager.h"

#include "common/components/CompPhysics.h"
#include "common/components/CompShape.h"
#include "common/components/CompPosition.h"
#include "common/components/CompPath.h"
#include "common/components/CompPathMove.h"

#include "common/GameEvents.h"
#include "common/Foreach.h"

#include <algorithm>

// Konstruktor
ComponentManager::ComponentManager( GameEvents& events )
: m_gameEvents ( events )
{}

ComponentManager::~ComponentManager()
{
    // NOTE: maybe we shouldn't create events here, since the world is being destroyed anyway...
    EntityMap::iterator next = m_entities.begin();
    EntityMap::iterator it = next;
    for ( ; it != m_entities.end(); it=next )
    {
        next = it;
        ++next;
        removeEntity( it->first );
    }
}

namespace
{

// Components that have to to be ordered must be placed here
// Components with lower indices will be initialized first
// NOTE: it would actually be nice if the order wouldn't matter, but for now this is simpler
ComponentId order[] = {
    CompShape::getTypeIdStatic(),
    CompPosition::getTypeIdStatic(),
    CompPhysics::getTypeIdStatic(),
    CompPath::getTypeIdStatic(),
    CompPathMove::getTypeIdStatic()
};

const int numOrdered = sizeof(order) / sizeof (order[0]);

bool compareComps(boost::shared_ptr<Component> first, boost::shared_ptr<Component> second)
{
    ComponentId* pos1 = std::find(order, order+numOrdered, first->getTypeId());
    ComponentId* pos2 = std::find(order, order+numOrdered, second->getTypeId());

    if (pos1 != pos2)
        return pos1 < pos2;

    return first.get() < second.get();
}

}

void ComponentManager::addEntity(const EntityId& id, ComponentList& components)
{
    components.sort(compareComps);

    // put components in a map
    ComponentMap compMap;
    foreach (boost::shared_ptr<Component> comp, components)
    {
        comp->m_entityId = id;
        comp->m_compManager = this;
        compMap.insert( std::make_pair(comp->getTypeId(), comp) );
    }

    m_entities[id] = compMap; // if there is an entity with the same ID before, it will get deleted

    // trigger events
    foreach (boost::shared_ptr<Component> comp, components)
    {
        m_gameEvents.newComponent.fire(*comp);
    }
    m_gameEvents.newEntity.fire(id);
}

void ComponentManager::removeEntity(const EntityId& id)
{
    EntityMap::iterator it = m_entities.find(id);
    if (it == m_entities.end())
        return;

    m_gameEvents.deleteEntity.fire( id );

    foreach (ComponentMap::value_type& pair, it->second)
    {
        Component& comp = *pair.second;
        m_gameEvents.deleteComponent.fire(comp);
    }

    m_entities.erase(it);
}


const EntityMap& ComponentManager::getAllEntities() const
{
    return m_entities;
}

bool ComponentManager::renameEntity(const EntityId& oldId, const EntityId& newId)
{
    if (m_entities.count(newId) > 0)
        return false;
    EntityMap::iterator it = m_entities.find(oldId);
    if (it == m_entities.end())
        return false;

    foreach (ComponentMap::value_type& v, it->second)
    {
        v.second->m_entityId = newId;
    }

    m_entities[newId] = it->second;
    m_entities.erase(it);

    return true;
}

Component* ComponentManager::getComponent(const EntityId& entId, const ComponentTypeId& compType, const ComponentId& compId) const
{
    EntityMap::const_iterator eit = m_entities.find(entId);
    if ( eit == m_entities.end() )
        return NULL;
    std::pair<ComponentMap::const_iterator,ComponentMap::const_iterator> eqRange = eit->second.equal_range(compType);
    for (ComponentMap::const_iterator it = eqRange.first; it != eqRange.second; ++it)
    {
        if (compId == "" || compId == it->second->getId())
            return it->second.get();
    }
    return NULL;
}

void ComponentManager::writeEntitiesToLogger(Logger& logger, LogLevel level)
{
    logger.setLevel(level);
    for ( EntityMap::iterator it = m_entities.begin(); it != m_entities.end(); ++it )
    {
        ComponentMap& comps = it->second;
        logger << "--- Entity name: " << it->first << " ---\n";
        for ( ComponentMap::iterator it2 = comps.begin(); it2 != comps.end(); ++it2 )
        {
            logger << " Component: " << it2->second->getTypeId() << " " << it2->second->getId() << "\n";
        }
        logger << "\n";
    }
}
