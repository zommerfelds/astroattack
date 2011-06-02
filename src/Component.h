/*
 * Component.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Basisklasse für eine Komponente

#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>

struct GameEvents;
class Entity;

typedef std::string ComponentTypeId;
typedef std::string ComponentId;

// forward declare ptree (PropertyTree), quite complex but I don't want to include the big header here
namespace boost { namespace property_tree {
    template<class Key, class Data, class KeyCompare> class basic_ptree;
    typedef basic_ptree<std::string, std::string, std::less<std::string> > ptree;
}}

class Component {
public:

    Component(GameEvents& gameEvents) : m_gameEvents(gameEvents), m_pOwnerEntity (NULL), m_id () {} // Konstruktor
    virtual ~Component() {} // Destruktor

    virtual const ComponentTypeId& getTypeId() const = 0;   // Komponenttyp -> Name des Komponententyps
    const ComponentId& getId() const { return m_id; } // Eigener Namen (optional)
    void setId( std::string id ) { m_id = id; }

    void setOwnerEntity( Entity* obj ) { m_pOwnerEntity = obj; } // Einheit, die die Komponente besitzt, festlegen
    Entity* getOwnerEntity() { return m_pOwnerEntity; } // Einheit, die die Komponente besitzt, abfragen
    const Entity* getOwnerEntity() const { return m_pOwnerEntity; } // Einheit, die die Komponente besitzt, abfragen

    virtual void loadFromPropertyTree(const boost::property_tree::ptree& propTree) = 0;
    virtual void writeToPropertyTree(boost::property_tree::ptree& propTree) const = 0;

protected:
    GameEvents& m_gameEvents;

private:

    Entity* m_pOwnerEntity; // Einheit, die die Komponente besitzt
    std::string m_id;
};

#endif
