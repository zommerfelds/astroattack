/*----------------------------------------------------------\
|                        Entity.h                           |
|                        --------                           |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

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
    Entity( const EntityIdType& rId )
    {
        SetId(rId);
    }
    ~Entity()
    {
        ClearComponents();
    }

    const EntityIdType& GetId() const
    {
        return m_Id;
    }
    void SetId( const EntityIdType& rId )
    {
        m_Id = rId;
    }

    // Gibt einen Zeiger zur aufgeforderten Komponente. Fall es keinen Komponent des gefragten Typs gibt, gibt es NULL zurück.
    template<typename CompType>
    CompType* GetComponent();

    // Get component from string
    // Only use this version if you can only get the component name at runtime (as a string)
    Component* GetComponent(const CompIdType& rCompId );
    
    std::vector<Component*> GetComponents(const CompIdType& rCompId );
    const ComponentMap* GetAllComponents();
    void SetComponent(const boost::shared_ptr<Component>& pNewComp); // Setzt eine neue Komponente in die Einheit
    //ComponentMap* GetAllComponents() { return &m_components; }

    void ClearComponents()
    {      
        m_components.clear();
    }

    void WriteEntityInfoToLogger( Logger& log );

private:
    EntityIdType m_Id; // unique identifier for this object

    ComponentMap m_components;  // map of all components
};

// needs to be implemented here
template <typename CompType>
CompType* Entity::GetComponent()
{
   ComponentMap::const_iterator i = m_components.find( CompType::COMPONENT_ID );
   if ( i == m_components.end() )
       return NULL;
   else
       return static_cast<CompType*>(i->second.get());
}

#endif

// Astro Attack - Christian Zommerfelds - 2009
