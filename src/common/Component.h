/*
 * Component.h
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */

// Basisklasse für eine Komponente

#ifndef COMPONENT_H
#define COMPONENT_H

#include "IdTypes.h"
#include <string>
#include <vector>

class GameEvents;

class ComponentManager;

// forward declare ptree (PropertyTree), quite complex but we don't want to include the big header here
namespace boost { namespace property_tree {
    template<class Key, class Data, class KeyCompare> class basic_ptree;
    typedef basic_ptree<std::string, std::string, std::less<std::string> > ptree;
}}

class Component {
public:

    Component(const ComponentId& id, GameEvents& gameEvents);
    virtual ~Component() {} // Destruktor

    static const ComponentTypeId& getTypeIdStatic();
    virtual const ComponentTypeId& getTypeId() const = 0;
    const ComponentId& getId() const { return m_id; }
    void setId(const ComponentId& id) { m_id = id; }

    const EntityId& getEntityId() { return m_entityId; }

    virtual void loadFromPropertyTree(const boost::property_tree::ptree& propTree) = 0;
    virtual void writeToPropertyTree(boost::property_tree::ptree& propTree) const = 0;

    template <typename CompType>       CompType* getSiblingComponent(const ComponentId& compId="");
    template <typename CompType> const CompType* getSiblingComponent(const ComponentId& compId="") const;
    //      Component* getSiblingComponent(const ComponentTypeId& compType);
    //const Component* getSiblingComponent(const ComponentTypeId& compType) const;

    template <typename CompType> std::vector<      CompType*> getSiblingComponents();
    template <typename CompType> std::vector<const CompType*> getSiblingComponents() const;
    //std::vector<      Component*> getSiblingComponents(const ComponentTypeId& compType);
    //std::vector<const Component*> getSiblingComponents(const ComponentTypeId& compType) const;

    static const ComponentId DEFAULT_ID;
protected:
    GameEvents& m_gameEvents;

private:
    ComponentManager* m_compManager; // this is used for getting the sibling components

    EntityId m_entityId;
    ComponentId m_id;

    friend class ComponentManager; // ComponentManager will set the entity ID and m_compManager
};

// IMPLEMENTATION
// needs to be implemented here because of templates
#include "ComponentManager.h"

template <typename CompType>
CompType* Component::getSiblingComponent(const ComponentId& compId)
{
    if (m_compManager == NULL)
        return NULL;
    return m_compManager->getComponent<CompType>(m_entityId, compId);
}

template <typename CompType>
const CompType* Component::getSiblingComponent(const ComponentId& compId) const
{
    if (m_compManager == NULL)
        return NULL;
    return m_compManager->getComponent<CompType>(m_entityId, compId);
}

template <typename CompType>
std::vector<CompType*> Component::getSiblingComponents()
{
    if (m_compManager == NULL)
        return std::vector<CompType*>();
    return m_compManager->getComponents<CompType>(m_entityId);
}

template <typename CompType>
std::vector<const CompType*> Component::getSiblingComponents() const
{
    if (m_compManager == NULL)
        return std::vector<const CompType*>();
    return m_compManager->getComponents<CompType>(m_entityId);
}

#endif
