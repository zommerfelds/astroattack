/*
 * CompTrigger_Conditions.cpp
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#include "CompTrigger_Conditions.h"

#include "CompPhysics.h"

#include "common/components/CompShape.h"

#include "common/Logger.h"
#include "common/Foreach.h"
#include "common/GameEvents.h"

ConditionCompareVariable::ConditionCompareVariable(GameEvents& gameEvents, const EntityId& entity, const ComponentId& var, CompareOperator comp, int numToCompareWith)
: m_entity (entity),
  m_var (var),
  m_compareType (comp),
  m_numToCompareWith (numToCompareWith)
{
    m_eventConnection1 = gameEvents.variableUpdate.registerListener(boost::bind(&ConditionCompareVariable::onVariableUpdate, this, _1, _2, _3));
}

void ConditionCompareVariable::onVariableUpdate(const EntityId& entity, const ComponentId& var, int val)
{
    if (m_entity == entity && m_var == var)
    {
        switch ( m_compareType )
        {
        case GreaterThan:
            setConditionState(val > m_numToCompareWith);
            break;
        case GreaterThanOrEqualTo:
            setConditionState(val >= m_numToCompareWith);
            break;
        case LessThan:
            setConditionState(val < m_numToCompareWith);
            break;
        case LessThanOrEqualTo:
            setConditionState(val <= m_numToCompareWith);
            break;
        case EqualTo:
            setConditionState(val == m_numToCompareWith);
            break;
        case NotEqualTo:
            setConditionState(val != m_numToCompareWith);
            break;
        default:
            setConditionState(false);
            break;
        }
    }
}

ConditionContact::ConditionContact(GameEvents& gameEvents, const EntityId& entityId1, const EntityId& entityId2)
: m_entity1 (entityId1),
  m_entity2 (entityId2)
{
    m_eventConnection1 = gameEvents.newContact.registerListener(boost::bind(&ConditionContact::onNewContact, this, _1, _2));
}

void ConditionContact::onNewContact(CompShape& shape1, CompShape& shape2)
{
    if (   (shape1.getEntityId() == m_entity1 && shape2.getEntityId() == m_entity2)
        || (shape2.getEntityId() == m_entity1 && shape1.getEntityId() == m_entity2))
    {
        setConditionState(true);
    }
}
