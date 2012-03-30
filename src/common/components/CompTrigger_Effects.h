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

class EffectKillEntity : public Effect
{
public:
    EffectKillEntity(GameEvents& gameEvents, const EntityId& entityToKill);
    EffectId        getId() const { return getIdStatic(); }
    static EffectId getIdStatic() { return "KillEntity"; }
    void fire();
    std::string getEntityId() const { return m_entityToKill; }
private:
    GameEvents& m_gameEvents;
    EntityId m_entityToKill;
};

class EffectDispMessage : public Effect
{
public:
    EffectDispMessage(GameEvents& gameEvents, const std::string& message, int timeMs);
    EffectId        getId() const { return getIdStatic(); }
    static EffectId getIdStatic() { return "DispMessage"; }
    void fire();
    std::string getMessage() const { return m_message; }
    int getTotalTime() const { return m_totalTimeMs; }
private:
    GameEvents& m_gameEvents;
    std::string m_message;
    int m_totalTimeMs;
};

class EffectEndLevel : public Effect
{
public:
    EffectEndLevel(GameEvents& gameEvents, const std::string& message, bool win);
    EffectId        getId() const { return getIdStatic(); }
    static EffectId getIdStatic() { return "EndLevel"; }
    void fire();
    bool update() { return false; }
    std::string getMessage() const { return m_message; }
    bool isWin() const { return m_win; }
private:
    GameEvents& m_gameEvents;
    std::string m_message;
    bool m_win;
};

enum ModifyId
{
    Set,
    Add,
    Multiply,
    Divide
};

class EffectModifyVariable : public Effect
{
public:
    EffectModifyVariable(GameEvents& gameEvents, const EntityId& entity, const ComponentId& var, const ModifyId& changeType, int num);
    EffectId        getId() const { return getIdStatic(); }
    static EffectId getIdStatic() { return "ModifyVariable"; }
    void fire();
    int getNum() const { return m_num; }
    EntityId getEntity() const { return m_entity; }
    ComponentId getVariable() const { return m_var; }
    ModifyId getModId() const { return m_modId; }
private:
    int modifyVar(int);
    GameEvents& m_gameEvents;
    EntityId m_entity;
    ComponentId m_var;
    ModifyId m_modId;
    int m_num;
};

#endif /* COMPTRIGGER_EFFECTS_H */
