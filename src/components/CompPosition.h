/*
 * CompPosition.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Spiechert eine Position und Winkel eines Objekts
// WIRD GERADE NICHT VERWENDET, DA BOX2D DIESE FUNKTION ÜBERNIMMT

#ifndef COMPPOSITION_H
#define COMPPOSITION_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "../Component.h"
#include <boost/shared_ptr.hpp>

#include "../Vector2D.h"
namespace pugi { class xml_node; }

//--------------------------------------------//
//----------- CompPosition Klasse ------------//
//--------------------------------------------//
class CompPosition : public Component
{
public:
    CompPosition();

    // Base component methods
	const CompIdType& FamilyID() const { return COMPONENT_ID; }
    const CompIdType& ComponentId() const { return COMPONENT_ID; }
	static const CompIdType COMPONENT_ID;

    // =========== Getters ==========
    const Vector2D& GetPosition() const; // get current drawing (smooth) position. If a CampPhysics exist, it gets the position from there.
    const Vector2D& GetPosIgnoreCompPhys() const; // this is not meant to be extensively used. this bypasses the check for CompPhysics position.
    float GetOrientation() const;

    // =========== Setters ===========
    void SetOrientation(float orientation);
    void SetPosition(const Vector2D& pos);

    static boost::shared_ptr<CompPosition> LoadFromXml(const pugi::xml_node& compElem);
    void WriteToXml(pugi::xml_node& compNode) const;

private:
    void OnUpdate();

    Vector2D m_position;
    float m_orientation;
};
//--------------------------------------------//
//-------- Ende CompPosition Klasse ----------//
//--------------------------------------------//

#endif
