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
Component* Entity::GetFirstComponent(const CompIdType& rCompId )
{
    ComponentMap::const_iterator i = m_components.find( rCompId );
    if ( i == m_components.end() )
        return NULL;
    else
        return i->second.get();
}

std::vector<Component*> Entity::GetComponents(const CompIdType& rCompId )
{
    unsigned int count = m_components.count( rCompId );
    if ( count == 0 )
        return std::vector<Component*>();
    std::vector<Component*> ret (count);

    ComponentMap::iterator it = m_components.lower_bound(rCompId);

    for (unsigned int i = 0; i < count; ++i, ++it )
    {
        ret[i] = it->second.get();
    }
    return ret;
}

 // Setzt eine neue Komponente in die Einheit
void Entity::SetComponent(boost::shared_ptr<Component> pNewComp)
{
    CompIdType compId = pNewComp->ComponentId();
    m_components.insert( std::pair< CompIdType, boost::shared_ptr<Component> >(compId, boost::shared_ptr<Component>(pNewComp) ) );
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

const ComponentMap* Entity::GetAllComponents()
{
    return &m_components;
}

// Astro Attack - Christian Zommerfelds - 2009
