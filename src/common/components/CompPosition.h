/*
 * CompPosition.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Spiechert eine Position und Winkel eines Objekts
// WIRD GERADE NICHT VERWENDET, DA BOX2D DIESE FUNKTION ÜBERNIMMT

#ifndef COMPPOSITION_H
#define COMPPOSITION_H

#include "common/Component.h"
#include "common/Vector2D.h"

//--------------------------------------------//
//----------- CompPosition Klasse ------------//
//--------------------------------------------//
class CompPosition : public Component
{
public:
    CompPosition(const ComponentIdType& id, GameEvents& gameEvents);

    // Base component methods
    const ComponentTypeId& getTypeId() const { return COMPONENT_TYPE_ID; }
	static const ComponentTypeId COMPONENT_TYPE_ID;

    // =========== Getters ==========
    Vector2D getDrawingPosition() const; // get current drawing (smooth) position. If a CampPhysics exist, it gets the position from there.
    float getDrawingOrientation() const;

    // NOTE
    // probably a getPosition() and a getOrientation() should come here, but for the moment nobody needs them

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

    friend class PhysicsSubSystem; // Physics sub-system is allowed to change the position
};

#endif
