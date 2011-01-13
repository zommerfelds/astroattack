/*----------------------------------------------------------\
|                       Component.h                         |
|                       -----------                         |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// Basisklasse für eine Komponente

#ifndef COMPONENT_H
#define COMPONENT_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include <string>

struct GameEvents;
class Entity;
typedef std::string CompIdType;
typedef std::string CompNameType;

class Component {
public:

    Component() : m_pOwnerEntity(),m_name("std") {} // Konstruktor
    virtual ~Component() {} // Destruktor

    virtual const CompIdType& ComponentId() const = 0;   // Komponent ID -> Name des Komponententyps
    const CompNameType& GetName() const { return m_name; } // Eigener Namen (optional)
    void SetName( std::string name ) { m_name = name; }

    void SetOwnerEntity( Entity* obj ) { m_pOwnerEntity = obj; } // Einheit, die die Komponente besitzt, festlegen
    Entity* GetOwnerEntity() { return m_pOwnerEntity; } // Einheit, die die Komponente besitzt, abfragen

    static GameEvents* gameEvents; // EventManager für alle Komponenten

private:
    Entity* m_pOwnerEntity; // Einheit, die die Komponente besitzt
    std::string m_name;
};

#endif

// Astro Attack - Christian Zommerfelds - 2009
