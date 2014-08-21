/*
 * TriggerSystem.cpp
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */

#include "TriggerSystem.h"
#include "GameEvents.h"
#include "components/CompTrigger.h"
#include "components/CompTrigger_Conditions.h"
#include "common/Foreach.h"
#include <boost/bind.hpp>

namespace {
int identity(int x) { return x; }
}

TriggerSystem::TriggerSystem(GameEvents& gameEvents)
: m_gameEvents (gameEvents)
{
    m_eventConnections.add(gameEvents.newComponent   .registerListener( boost::bind( &TriggerSystem::onRegisterComp, this, _1 ) ));
    m_eventConnections.add(gameEvents.deleteComponent.registerListener( boost::bind( &TriggerSystem::onUnregisterComp, this, _1 ) ));
}

void TriggerSystem::onRegisterComp(Component& component)
{
    if (component.getTypeId() != CompTrigger::getTypeIdStatic())
        return;

    CompTrigger* compTrigg = static_cast<CompTrigger*>(&component);
    m_triggerComps.insert(compTrigg);

    foreach (boost::shared_ptr<Condition> cond, compTrigg->getConditions())
    {
        if (cond->getId() == ConditionCompareVariable::getIdStatic())
        {
            ConditionCompareVariable& condVar = static_cast<ConditionCompareVariable&>(*cond);
            m_gameEvents.variableModification.fire(condVar.getEntity(), condVar.getVariable(), &identity); // force an update
        }
    }
}

void TriggerSystem::onUnregisterComp(Component& component)
{
    if (component.getTypeId() != CompTrigger::getTypeIdStatic())
        return;

    m_triggerComps.erase(static_cast<CompTrigger*>(&component));
}

void TriggerSystem::update()
{
}
