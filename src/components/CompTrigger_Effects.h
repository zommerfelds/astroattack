/*
 * CompTrigger_Effects.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Enzelne Effekte für CompTrigger Komponente

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "CompTrigger.h"
#include <string>
#include <map>
class GameWorld;

// ========= KillEntity ===========
class EffectKillEntity : public Effect
{
public:
    EffectKillEntity( std::string entityToKill, GameWorld* pGW );
    EffectIdType ID() const { return "KillEntity"; }
    void Fire();
    bool Update() { return false; }
    std::string GetEntityName() const { return m_entityToKill; }
private:
    std::string m_entityToKill;
    GameWorld* m_pGW;
};

// ========= DispMessage ===========
class EffectDispMessage : public Effect
{
public:
    EffectDispMessage( std::string message, int timeMs, GameWorld* pGW );
    EffectIdType ID() const { return "DispMessage"; }
    ~EffectDispMessage();
    void Fire();
    bool Update();
    std::string GetMessage() const { return m_message; }
    int GetTotalTime() const { return m_totalTimeMs; }
private:
    std::string m_message;
    int m_remainingUpdates;
    bool m_fired;
    std::string m_msgEntityName;
    GameWorld* m_pGW;
    Entity* m_pMsgEntity;
    int m_totalTimeMs;
};

// ========= EndLevel ===========
class EffectEndLevel : public Effect
{
public:
    EffectEndLevel( std::string message, bool win ) : m_message( message ), m_win ( win ) {}
    EffectIdType ID() const { return "EndLevel"; }
    void Fire();
    bool Update() { return false; }
    std::string GetMessage() const { return m_message; }
    bool GetWin() const { return m_win; }
private:
    std::string m_message;
    bool m_win;
};

// ========= ChangeVariable ===========
enum ChangeType
{
    Set,
    Increase,
    Multiply,
    Divide
};

class EffectChangeVariable : public Effect
{
public:
    EffectChangeVariable( std::map<const std::string, int>::iterator itVariable, ChangeType changeType, int num );
    EffectIdType ID() const { return "ChangeVariable"; }
    void Fire();
    bool Update() { return true; }
    int GetNum() const { return m_num; }
    std::string GetVariable() const { return m_itVariable->first; }
    ChangeType GetChangeType() const { return m_changeType; }
private:
    std::map<const std::string, int>::iterator m_itVariable;

    ChangeType m_changeType;
    int m_num;
};
