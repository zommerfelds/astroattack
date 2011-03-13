/*
 * Entity.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "Entity.h"
#include "Logger.h"

// Gibt einen Zeiger zur aufgeforderten Komponente. Fall es keinen Komponent des gefragten Typs gibt, gibt es NULL zurück.
Component* Entity::getComponent(const ComponentTypeId& rCompId )
{
    ComponentMap::const_iterator i = m_components.find( rCompId );
    if ( i == m_components.end() )
        return NULL;
    else
        return i->second.get();
}

const Component* Entity::getComponent(const ComponentTypeId& rCompId ) const
{
    ComponentMap::const_iterator i = m_components.find( rCompId );
    if ( i == m_components.end() )
        return NULL;
    else
        return i->second.get();
}

std::vector<Component*> Entity::getComponents(const ComponentTypeId& rCompId )
{
    std::pair<ComponentMap::iterator, ComponentMap::iterator> equalRange = m_components.equal_range(rCompId);
    std::vector<Component*> ret;

    for (ComponentMap::iterator it = equalRange.first; it != equalRange.second; ++it )
        ret.push_back(it->second.get());
    return ret;
}

std::vector<const Component*> Entity::getComponents(const ComponentTypeId& rCompId ) const
{
    std::pair<ComponentMap::const_iterator, ComponentMap::const_iterator> equalRange = m_components.equal_range(rCompId);
    std::vector<const Component*> ret;

    for (ComponentMap::const_iterator it = equalRange.first; it != equalRange.second; ++it )
        ret.push_back(it->second.get());
    return ret;
}


 // Setzt eine neue Komponente in die Einheit
void Entity::addComponent(const boost::shared_ptr<Component>& pNewComp)
{
    ComponentTypeId compId = pNewComp->getTypeId();
    m_components.insert( std::make_pair(compId, pNewComp) );
    pNewComp->setOwnerEntity( this );
}

void Entity::writeEntityInfoToLogger( Logger& log )
{
    log.write( "--- Entity name: %s ---\n", m_Id.c_str() );
    for ( ComponentMap::iterator it = m_components.begin(); it != m_components.end(); ++it )
    {
        log.write( " Component: %s\n" , it->second->getId().c_str() );
    }
    log.write( "\n" );
}

const ComponentMap& Entity::getAllComponents() const
{
    return m_components;
}
