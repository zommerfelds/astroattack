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
    EffectKillEntity( std::string entityToKill, const GameWorld& world );
    EffectIdType getId() const { return "KillEntity"; }
    void fire();
    bool update() { return false; }
    std::string getEntityName() const { return m_entityToKill; }
private:
    std::string m_entityToKill;
    const GameWorld& m_world;
};

// ========= DispMessage ===========
class EffectDispMessage : public Effect
{
public:
    EffectDispMessage( std::string message, int timeMs, GameWorld& world );
    ~EffectDispMessage();
    EffectIdType getId() const { return "DispMessage"; }
    void fire();
    bool update();
    std::string getMessage() const { return m_message; }
    int getTotalTime() const { return m_totalTimeMs; }
private:
    std::string m_message;
    int m_remainingUpdates;
    bool m_fired;
    std::string m_msgEntityName;
    GameWorld& m_world;
    Entity* m_pMsgEntity;
    int m_totalTimeMs;
};

// ========= EndLevel ===========
class EffectEndLevel : public Effect
{
public:
    EffectEndLevel( std::string message, bool win ) : m_message( message ), m_win ( win ) {}
    EffectIdType getId() const { return "EndLevel"; }
    void fire();
    bool update() { return false; }
    std::string getMessage() const { return m_message; }
    bool isWin() const { return m_win; }
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
    EffectChangeVariable( std::map<const std::string, int>::iterator itVariable, const ChangeType& changeType, int num );
    EffectIdType getId() const { return "ChangeVariable"; }
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
