/*
 * CompTrigger.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

// Auslöser-Komponente
// Ein Auslöser hat eine oder meherer Bedingungen. Wenn diese erfüllt sind,
// werden eins oder mehrere Effekte aufgeführt.

#ifndef COMPTRIGGER_H
#define COMPTRIGGER_H

#include "common/Component.h"
#include "common/IdTypes.h"

#include <vector>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

class GameEvents;
class Condition;
class Effect;

typedef std::vector< boost::shared_ptr<Condition> > ConditionVector;
typedef std::vector< boost::shared_ptr<Effect> > EffectVector;

//--------------------------------------------//
//---------- CompTrigger Klasse --------------//
//--------------------------------------------//
class CompTrigger : public Component
{
public:
    CompTrigger(const ComponentId& id, GameEvents& gameEvents);

    const ComponentType& getTypeId() const { return getTypeIdStatic(); }
    static const ComponentType& getTypeIdStatic();

    void addCondition( boost::shared_ptr<Condition> );
    void addEffect( boost::shared_ptr<Effect> );

    ConditionVector& getConditions() { return m_conditions; }
    EffectVector& getEffects() { return m_effects; }

    void loadFromPropertyTree(const boost::property_tree::ptree& propTree);
    void writeToPropertyTree(boost::property_tree::ptree& propTree) const;

private:
    ConditionVector m_conditions;
    EffectVector m_effects;

    void onConditionUpdate();

    bool m_fired;
};

typedef boost::function<void ()> UpdateHandler;

// Basisklasse Bedingung
class Condition
{
public:
    Condition();
    virtual ~Condition() {}

    void setUpdateHandler(const UpdateHandler& updateHandler);

    virtual ConditionId getId() const = 0;

    bool getConditionState() const;

    /*virtual void loadFromPropertyTree(const boost::property_tree::ptree& propTree) = 0;
    virtual void writeToPropertyTree(boost::property_tree::ptree& propTree) const = 0;*/

protected:
    void setConditionState(bool);

private:
    bool m_state;
    UpdateHandler m_updateHandler;
};

// Basisklasse Effekt
class Effect
{
public:
    virtual ~Effect() {}

    virtual EffectId getId() const = 0;

    virtual void fire() = 0;
};

#endif
