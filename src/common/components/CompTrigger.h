/*
 * CompTrigger.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Auslöser-Komponente
// Ein Auslöser hat eine oder meherer Bedingungen. Wenn diese erfüllt sind,
// werden eins oder mehrere Effekte aufgeführt.

#ifndef COMPTRIGGER_H
#define COMPTRIGGER_H

#include <boost/shared_ptr.hpp>
#include <vector>

#include "common/Component.h"
#include "common/GameEvents.h"
#include "common/IdTypes.h"

class Condition;
class Effect;
class World;

//--------------------------------------------//
//---------- CompTrigger Klasse --------------//
//--------------------------------------------//
class CompTrigger : public Component
{
public:
    CompTrigger(const ComponentIdType& id, World& gameWorld, GameEvents& gameEvents);

    const ComponentTypeId& getTypeId() const { return COMPONENT_TYPE_ID; }
	static const ComponentTypeId COMPONENT_TYPE_ID; // eindeutige ID für diese Komponentenart (gleich wie Klassennamen, siehe CompCollectable.cpp)

    void addCondition( boost::shared_ptr<Condition> );
    void addEffect( boost::shared_ptr<Effect> );

    const std::vector< boost::shared_ptr<Condition> >& getConditions() const { return m_conditions; }
    const std::vector< boost::shared_ptr<Effect> >& getEffects() const { return m_effects; }

    void loadFromPropertyTree(const boost::property_tree::ptree& propTree);
    void writeToPropertyTree(boost::property_tree::ptree& propTree) const;

private:
    std::vector< boost::shared_ptr<Condition> > m_conditions;
    std::vector< boost::shared_ptr<Effect> > m_effects;

    World& m_gameWorld;
    EventConnection m_eventConnection;

    // Hier werden alle nötigen aktionen Durgeführt pro Aktualisierung
    void onUpdate();

    bool m_fired;
};

// Basisklasse Bedingung
class Condition
{
public:
    virtual ~Condition() {}

    virtual ConditionId getId() const = 0;

    virtual bool isConditionTrue() = 0;

    CompTrigger* m_pCompTrigger;
};

// Basisklasse Effekt
class Effect
{
public:
    virtual ~Effect() {}

    virtual EffectId getId() const = 0;

    virtual void fire() = 0;
    virtual bool update() = 0; // true zurückgeben wenn der Effekt vollstänkdig ausgeführt wurde

    CompTrigger* m_pCompTrigger;
};

#endif
