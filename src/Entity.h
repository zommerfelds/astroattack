/*
 * Entity.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#ifndef ENTITY_H
#define ENTITY_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include <string>
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "Component.h"

// TODO: get comp by name

class Logger;

typedef std::string EntityIdType;
typedef std::multimap< const CompIdType, boost::shared_ptr<Component> > ComponentMap;

class Entity
{
private:
    Entity() {}
public:
    Entity( const EntityIdType& rId ) { setId(rId); }

    const EntityIdType& getId() const { return m_Id; }
    void setId( const EntityIdType& rId ) { m_Id = rId; }

    // === GetComponent ===
    // There are 2 version of this method:
    // One that returns the component as its specific type using templates
    //   e.g. CompX x = GetComponent<CompX>();
    // And one that returns the component as a base Component type
    //   e.g. Component x = GetComponent("CompX");
    // Prefer using the first one (no need to cast)
    // Use the second version if you can only get the component ID at runtime (as a string)
    template <typename CompType>       CompType* getComponent();
    template <typename CompType> const CompType* getComponent() const;
          Component* getComponent(const CompIdType& rCompId );
    const Component* getComponent(const CompIdType& rCompId ) const;
    
    // === GetComponents ===
    // As GetComponent, this method has 2 versions (see above)
    // Returns a vector of all matching components
    template <typename CompType> std::vector<      CompType*> getComponents();
    template <typename CompType> std::vector<const CompType*> getComponents() const;
    std::vector<      Component*> getComponents(const CompIdType& rCompId );
    std::vector<const Component*> getComponents(const CompIdType& rCompId ) const;

    const ComponentMap& getAllComponents() const;

    void addComponent(const boost::shared_ptr<Component>& pNewComp); // Setzt eine neue Komponente in die Einheit

    void clearComponents() { m_components.clear(); }

    void writeEntityInfoToLogger( Logger& log );

private:
    EntityIdType m_Id; // unique identifier for this object

    ComponentMap m_components;  // map of all components
};

// needs to be implemented here because of templates
template <typename CompType>
CompType* Entity::getComponent()
{
   return static_cast<CompType*>( getComponent(CompType::COMPONENT_ID) );
}

template <typename CompType>
const CompType* Entity::getComponent() const
{
   return static_cast<const CompType*>( getComponent(CompType::COMPONENT_ID) );
}

template <typename CompType>
std::vector<CompType*> Entity::getComponents()
{
    std::pair<ComponentMap::iterator, ComponentMap::iterator> equalRange = m_components.equal_range(CompType::COMPONENT_ID);
    std::vector<CompType*> ret;

    for (ComponentMap::iterator it = equalRange.first; it != equalRange.second; ++it )
        ret.push_back( static_cast<CompType*>(it->second.get()) );
    return ret;
}

template <typename CompType>
std::vector<const CompType*> Entity::getComponents() const
{
    std::pair<ComponentMap::const_iterator, ComponentMap::const_iterator> equalRange = m_components.equal_range(CompType::COMPONENT_ID);
    std::vector<const CompType*> ret;

    for (ComponentMap::const_iterator it = equalRange.first; it != equalRange.second; ++it )
        ret.push_back( static_cast<CompType*>(it->second.get()) );
    return ret;
}

#endif
