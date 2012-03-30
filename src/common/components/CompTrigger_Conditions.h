/*
 * CompTrigger_Conditions.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

// Enzelne Bedingungen für CompTrigger Komponente

#ifndef COMPTRIGGER_CONDITIONS_H
#define COMPTRIGGER_CONDITIONS_H

#include "CompTrigger.h"
#include "common/Event.h"
#include "common/IdTypes.h"
#include <map>
#include <string>

class CompShape;
class GameEvents;

enum CompareOperator
{
    GreaterThan,
    GreaterThanOrEqualTo,
    LessThan,
    LessThanOrEqualTo,
    EqualTo,
    NotEqualTo
};

class ConditionCompareVariable : public Condition
{
public:
    ConditionCompareVariable(GameEvents& gameEvents, const EntityId& entity, const ComponentId& var, CompareOperator comp, int numToCompareWith);

    ConditionId getId()        const { return getIdStatic(); }
    static ConditionId getIdStatic() { return "CompareVariable"; }
    CompareOperator getCompareType() const { return m_compareType; }
    int             getNum()         const { return m_numToCompareWith; }
    EntityId    getEntity()      const { return m_entity; }
    ComponentId getVariable()    const { return m_var; }
    
private:
    void onVariableUpdate(const EntityId& entity, const ComponentId& var, int val);

    EntityId m_entity;
    ComponentId m_var;
    CompareOperator m_compareType;
    int m_numToCompareWith;
    EventConnection m_eventConnection1;
};

class ConditionContact : public Condition
{
public:
    ConditionContact(GameEvents& gameEvents, const EntityId& entityId1, const EntityId& entityId2);

    ConditionId  getId()  const { return getIdStatic(); }
    static ConditionId getIdStatic() { return "ConditionContact"; }
    EntityId getEntity1() const { return m_entity1; }
    EntityId getEntity2() const { return m_entity2; }

private:
    void onNewContact(CompShape& shape1, CompShape& shape2);

    EntityId m_entity1;
    EntityId m_entity2;
    EventConnection m_eventConnection1;
};

#endif /* COMPTRIGGER_CONDITIONS_H */
