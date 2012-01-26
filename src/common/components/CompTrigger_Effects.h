/*
 * CompTrigger_Effects.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

// Enizelne Effekte für CompTrigger Komponente

#ifndef COMPTRIGGER_EFFECTS_H
#define COMPTRIGGER_EFFECTS_H

#include "CompTrigger.h"
#include "common/IdTypes.h"

#include <map>
#include <string>

class ComponentManager;

// ========= KillEntity ===========
class EffectKillEntity : public Effect
{
public:
    EffectKillEntity(GameEvents& gameEvents, const std::string& entityToKill);
    EffectId getId() const { return "KillEntity"; }
    void fire();
    bool update() { return false; }
    std::string getEntityName() const { return m_entityToKill; }
private:
    GameEvents& m_gameEvents;
    EntityIdType m_entityToKill;
};

// ========= DispMessage ===========
class EffectDispMessage : public Effect
{
public:
    EffectDispMessage(GameEvents& gameEvents, const std::string& message, int timeMs, ComponentManager& compManager);
    ~EffectDispMessage();
    EffectId getId() const { return "DispMessage"; }
    void fire();
    bool update();
    std::string getMessage() const { return m_message; }
    int getTotalTime() const { return m_totalTimeMs; }
private:
    GameEvents& m_gameEvents;
    std::string m_message;
    int m_remainingUpdates;
    bool m_fired;
    ComponentManager& m_compManager;
    EntityIdType m_msgEntityId;
    int m_totalTimeMs;
};

// ========= EndLevel ===========
class EffectEndLevel : public Effect
{
public:
    EffectEndLevel(GameEvents& gameEvents, const std::string& message, bool win);
    EffectId getId() const { return "EndLevel"; }
    void fire();
    bool update() { return false; }
    std::string getMessage() const { return m_message; }
    bool isWin() const { return m_win; }
private:
    GameEvents& m_gameEvents;
    std::string m_message;
    bool m_win;
};

// ========= ChangeVariable ===========
enum ChangeType
{
    Set,
    Add,
    Multiply,
    Divide
};

class EffectChangeVariable : public Effect
{
public:
    EffectChangeVariable( std::map<const std::string, int>::iterator itVariable, const ChangeType& changeType, int num );
    EffectId getId() const { return "ChangeVariable"; }
    void fire();
    bool update() { return true; }
    int getNum() const { return m_num; }
    std::string getVariable() const { return m_itVariable->first; }
    ChangeType getChangeType() const { return m_changeType; }
private:
    std::map<const std::string, int>::iterator m_itVariable;

    ChangeType m_changeType;
    int m_num;
};

#endif /* COMPTRIGGER_EFFECTS_H */
