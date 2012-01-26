/*
 * ComponentManager.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#include "ComponentManager.h"

#include "common/components/CompPhysics.h"
#include "common/components/CompShape.h"
#include "common/components/CompPosition.h"

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

// components that have to to be ordered must be placed here
ComponentIdType order[] = {
    CompShape::getTypeIdStatic(),
    CompPosition::getTypeIdStatic(),
    CompPhysics::getTypeIdStatic()
};

const int numOrdered = 3;//sizeof(order) / sizeof (order[0]);

bool compareComps(boost::shared_ptr<Component> first, boost::shared_ptr<Component> second)
{
    ComponentIdType* pos1 = std::find(order, order+numOrdered, first->getEntityId());
    ComponentIdType* pos2 = std::find(order, order+numOrdered, second->getEntityId());

    if (pos1 != pos2)
        return pos1 < pos2;

    return first.get() < second.get();
}

}

void ComponentManager::addEntity(const EntityIdType& id, ComponentList& components)
{
    components.sort(compareComps);

    // put components in a map
    ComponentMap compMap;
    foreach(boost::shared_ptr<Component> comp, components)
    {
        comp->m_entityId = id;
        comp->m_compManager = this;
        compMap.insert( std::make_pair(comp->getTypeId(), comp) );
    }

    m_entities[id] = compMap; // if there is an entity with the same ID before, it will get deleted

    // trigger events
    foreach(boost::shared_ptr<Component> comp, components)
    {
        m_gameEvents.newComponent.fire(*comp);
    }
    m_gameEvents.newEntity.fire(id);
}

void ComponentManager::removeEntity(const EntityIdType& id)
{
    EntityMap::iterator it = m_entities.find(id);
    if (it == m_entities.end())
        return;

    m_gameEvents.deleteEntity.fire( id );

    foreach(ComponentMap::value_type& pair, it->second)
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

Component* ComponentManager::getComponent(const EntityIdType& entId, const ComponentTypeId& compType, const ComponentIdType& compId)
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
/*
Component* ComponentManager::getComponent(const EntityIdType& entId, const ComponentTypeId& compType)
{
    EntityMap::const_iterator eit = m_entities.find(entId);
    if ( eit == m_entities.end() )
        return NULL;
    ComponentMap::const_iterator cit = eit->second.find( compType );
    if ( cit == eit->second.end() )
        return NULL;
    else
        return cit->second.get();
}
const Component* ComponentManager::getComponent(const EntityIdType& entId, const ComponentTypeId& compType) const
{
    EntityMap::const_iterator eit = m_entities.find(entId);
    if ( eit == m_entities.end() )
        return NULL;
    ComponentMap::const_iterator cit = eit->second.find( compType );
    if ( cit == eit->second.end() )
        return NULL;
    else
        return cit->second.get();
}
*/
/*
Component* ComponentManager::getComponentById(const EntityIdType& id, const ComponentIdType& compId)
{

}

const Component* ComponentManager::getComponentById(const EntityIdType& id, const ComponentIdType& compId) const
{

}
*/
/*
std::vector<Component*> ComponentManager::getComponents(const EntityIdType& id, const ComponentTypeId& compType )
{
    std::vector<Component*> ret;

    EntityMap::iterator eit = m_entities.find(id);
    if ( eit == m_entities.end() )
        return ret;
    std::pair<ComponentMap::iterator, ComponentMap::iterator> equalRange = eit->second.equal_range(compType);

    for (ComponentMap::iterator it = equalRange.first; it != equalRange.second; ++it )
        ret.push_back(it->second.get());
    return ret;
}

std::vector<const Component*> ComponentManager::getComponents(const EntityIdType& id, const ComponentTypeId& compType ) const
{
    std::vector<const Component*> ret;

    EntityMap::const_iterator eit = m_entities.find(id);
    if ( eit == m_entities.end() )
        return ret;
    std::pair<ComponentMap::const_iterator, ComponentMap::const_iterator> equalRange = eit->second.equal_range(compType);

    for (ComponentMap::const_iterator it = equalRange.first; it != equalRange.second; ++it )
        ret.push_back(it->second.get());
    return ret;
}
*/

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
