/*----------------------------------------------------------\
|                       Entity.cpp                          |
|                       ----------                          |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "Entity.h"
#include "Logger.h"

// Gibt einen Zeiger zur aufgeforderten Komponente. Fall es keinen Komponent des gefragten Typs gibt, gibt es NULL zurück.
Component* Entity::GetComponent(const CompIdType& rCompId )
{
    ComponentMap::const_iterator i = m_components.find( rCompId );
    if ( i == m_components.end() )
        return NULL;
    else
        return i->second.get();
}

const Component* Entity::GetComponent(const CompIdType& rCompId ) const
{
    ComponentMap::const_iterator i = m_components.find( rCompId );
    if ( i == m_components.end() )
        return NULL;
    else
        return i->second.get();
}

std::vector<Component*> Entity::GetComponents(const CompIdType& rCompId )
{
    std::pair<ComponentMap::iterator, ComponentMap::iterator> equalRange = m_components.equal_range(rCompId);
    std::vector<Component*> ret;

    for (ComponentMap::iterator it = equalRange.first; it != equalRange.second; ++it )
        ret.push_back(it->second.get());
    return ret;
}

std::vector<const Component*> Entity::GetComponents(const CompIdType& rCompId ) const
{
    std::pair<ComponentMap::const_iterator, ComponentMap::const_iterator> equalRange = m_components.equal_range(rCompId);
    std::vector<const Component*> ret;

    for (ComponentMap::const_iterator it = equalRange.first; it != equalRange.second; ++it )
        ret.push_back(it->second.get());
    return ret;
}


 // Setzt eine neue Komponente in die Einheit
void Entity::AddComponent(const boost::shared_ptr<Component>& pNewComp)
{
    CompIdType compId = pNewComp->ComponentId();
    m_components.insert( std::make_pair(compId, pNewComp) );
    pNewComp->SetOwnerEntity( this );
}

void Entity::WriteEntityInfoToLogger( Logger& log )
{
    log.Write( "--- Entity name: %s ---\n", m_Id.c_str() );
    for ( ComponentMap::iterator it = m_components.begin(); it != m_components.end(); ++it )
    {
        log.Write( " Component: %s\n" , it->second->ComponentId().c_str() );
    }
    log.Write( "\n" );
}

const ComponentMap* Entity::GetAllComponents() const
{
    return &m_components;
}

// Astro Attack - Christian Zommerfelds - 2009
