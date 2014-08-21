/*
 * CompPosition.h
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */

// Spiechert eine Position und Winkel eines Objekts
// WIRD GERADE NICHT VERWENDET, DA BOX2D DIESE FUNKTION ÜBERNIMMT

#ifndef COMPPOSITION_H
#define COMPPOSITION_H

#include "common/Component.h"
#include "common/Vector2D.h"

class CompPhysics;

//--------------------------------------------//
//----------- CompPosition Klasse ------------//
//--------------------------------------------//
class CompPosition : public Component
{
public:
    CompPosition(const ComponentId& id, GameEvents& gameEvents, const Vector2D& pos = Vector2D());

    // Base component methods
    const ComponentTypeId& getTypeId() const { return getTypeIdStatic(); }
    static const ComponentTypeId& getTypeIdStatic();

    // =========== Getters ==========
    Vector2D getDrawingPosition() const; // get current drawing (smooth) position. If a CampPhysics exist, it gets the position from there.
    float getDrawingOrientation() const;


    Vector2D getPosition() const;
    float getOrientation() const;

    // =========== Setters ===========
    // (no setters currently available because not needed)
    //void setOrientation(float orientation);
    //void setPosition(const Vector2D& pos);

    void loadFromPropertyTree(const boost::property_tree::ptree& propTree);
    void writeToPropertyTree(boost::property_tree::ptree& propTree) const;

private:
    void OnUpdate();

    Vector2D m_position;
    float m_orientation;

    CompPhysics* m_compPhysics;

    friend class PhysicsSystem; // Physics sub-system is allowed to change the position
};

#endif
