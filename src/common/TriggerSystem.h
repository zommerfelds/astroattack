/*
 * TriggerSystem.h
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */

#ifndef TRIGGERSYSTEM_H
#define TRIGGERSYSTEM_H

#include "common/Event.h"

#include <set>

class GameEvents;
class CompTrigger;
class Component;

class TriggerSystem
{
public:
    TriggerSystem(GameEvents& gameEvents);

    void update();

private:
    void onRegisterComp(Component&);
    void onUnregisterComp(Component&);

    GameEvents& m_gameEvents;

    std::set< CompTrigger* > m_triggerComps;

    EventConnections m_eventConnections;
};

#endif /* TRIGGERSYSTEM_H */
