/*
 * Component.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Basisklasse für eine Komponente

#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <vector>

struct GameEvents;

struct ComponentManager;

typedef std::string ComponentTypeId;
typedef std::string EntityIdType;
typedef std::string ComponentIdType;


// forward declare ptree (PropertyTree), quite complex but I don't want to include the big header here
namespace boost { namespace property_tree {
    template<class Key, class Data, class KeyCompare> class basic_ptree;
    typedef basic_ptree<std::string, std::string, std::less<std::string> > ptree;
}}

class Component {
public:

    Component(const ComponentIdType& id, GameEvents& gameEvents);
    virtual ~Component() {} // Destruktor

    virtual const ComponentTypeId& getTypeId() const = 0;   // Komponenttyp -> Name des Komponententyps
    const ComponentIdType& getId() const { return m_id; } // Eigener Namen (optional)

    const EntityIdType& getEntityId() { return m_entityId; }

    virtual void loadFromPropertyTree(const boost::property_tree::ptree& propTree) = 0;
    virtual void writeToPropertyTree(boost::property_tree::ptree& propTree) const = 0;


    template <typename CompType>       CompType* getSiblingComponent();
    template <typename CompType> const CompType* getSiblingComponent() const;
          Component* getSiblingComponent(const ComponentTypeId& compType);
    const Component* getSiblingComponent(const ComponentTypeId& compType) const;

    template <typename CompType> std::vector<      CompType*> getSiblingComponents();
    template <typename CompType> std::vector<const CompType*> getSiblingComponents() const;
    std::vector<      Component*> getSiblingComponents(const ComponentTypeId& compType);
    std::vector<const Component*> getSiblingComponents(const ComponentTypeId& compType) const;

    static const ComponentIdType DEFAULT_ID;
protected:
    GameEvents& m_gameEvents;

private:
    ComponentManager* m_compManager; // this is used for getting the sibling components

    EntityIdType m_entityId;
    std::string m_id;

    friend class ComponentManager; // ComponentManager will set the entity ID and m_compManager
};

#include "ComponentManager.h"

template <typename CompType>
CompType* Component::getSiblingComponent()
{
    if (m_compManager == NULL)
        return NULL;
    return m_compManager->getComponent<CompType>(m_entityId);
}

template <typename CompType>
const CompType* Component::getSiblingComponent() const
{
    if (m_compManager == NULL)
        return NULL;
    return m_compManager->getComponent<CompType>(m_entityId);
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
