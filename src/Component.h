/*
 * Component.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Basisklasse für eine Komponente

#ifndef COMPONENT_H
#define COMPONENT_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include <string>

struct GameEvents;
class Entity;
namespace pugi { class xml_node; }
typedef std::string CompIdType;
typedef std::string CompNameType;

class Component {
public:

    Component() : m_pOwnerEntity (NULL), m_name ("std") {} // Konstruktor
    virtual ~Component() {} // Destruktor

    virtual const CompIdType& ComponentId() const = 0;   // Komponent ID -> Name des Komponententyps
    const CompNameType& GetName() const { return m_name; } // Eigener Namen (optional)
    void SetName( std::string name ) { m_name = name; }

    void SetOwnerEntity( Entity* obj ) { m_pOwnerEntity = obj; } // Einheit, die die Komponente besitzt, festlegen
    Entity* GetOwnerEntity() { return m_pOwnerEntity; } // Einheit, die die Komponente besitzt, abfragen
    const Entity* GetOwnerEntity() const { return m_pOwnerEntity; } // Einheit, die die Komponente besitzt, abfragen

    //static boost::shared_ptr<ComponentType> LoadFromXml(const pugi::xml_node& compElem); // every component must to implement this
    virtual void WriteToXml(pugi::xml_node& compElem) const = 0;

    static GameEvents* gameEvents; // EventManager für alle Komponenten

private:
    Entity* m_pOwnerEntity; // Einheit, die die Komponente besitzt
    std::string m_name;
};

#endif
