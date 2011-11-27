/*
 * CompPlayerController.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// CompPlayerController ist die Komponente, die den Spieler steuert.
// Nur die Spielereinheit hat diese Komponente.
// Sie wandelt Tastendrücke in physikalische Kräfte um, steuert Animationen, usw.

#ifndef COMPPLAYERCONTROLLER_H
#define COMPPLAYERCONTROLLER_H

#include <map>
#include "common/Component.h"
#include "common/World.h"
#include "common/Vector2D.h"

class GameEvents;
class CompPhysics;

// Constants
const int cMaxRecharge = 15;                    // wie wie muss der Spieler warten bis der Racketenrucksack startet?

class CompPlayerController : public Component
{
public:
    CompPlayerController(const ComponentIdType& id, GameEvents& gameEvents, WorldVariablesMap::iterator itJetPackVar);

    const ComponentTypeId& getTypeId() const { return COMPONENT_TYPE_ID; }
    static const ComponentTypeId COMPONENT_TYPE_ID;

    void loadFromPropertyTree(const boost::property_tree::ptree& propTree);
    void writeToPropertyTree(boost::property_tree::ptree& propTree) const;

private:

    friend class PlayerController;

    void setLowFriction( CompPhysics* playerCompPhysics );
    void setHighFriction( CompPhysics* playerCompPhysics );
    bool m_currentFrictionIsLow;

    void updateAnims(bool flyingUp, bool movingOnGround, bool usingJetpack);

    WorldVariablesMap::iterator m_itJetPackVar;

    // Player controller fields
    bool m_spaceKeyDownLastUpdate;    // ob die Leerschlagtaste letztes Frame gerade gedrückt wurde
    bool m_playerCouldWalkLastUpdate; // ob der Spieler in der letzte überprüfung laufen konnte
    int m_rechargeTime;               // wie lange hat der Spieler schon den Racketenrucksack aufgeladen?
    int m_walkingTime;                // number of updates the player is walking (pressing walk key on ground)
    Vector2D m_rotationPoint;
};

#endif
