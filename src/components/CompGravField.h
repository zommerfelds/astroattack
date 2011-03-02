/*
 * CompGravField.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */
 
// Komponente für Gravitationsfelder

#ifndef COMPGRAVFIELD_H
#define COMPGRAVFIELD_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "../Component.h"
#include <set>

enum GravType { Directional, Radial };

#include "Vector2D.h"
namespace pugi { class xml_node; }

//--------------------------------------------//
//--------- CompGravField Klasse -------------//
//--------------------------------------------//
class CompGravField : public Component
{
public:
    CompGravField();

    const CompIdType& getComponentId() const { return COMPONENT_ID; }
    static const CompIdType COMPONENT_ID; // eindeutige ID für diese Komponentenart (gleich wie Klassennamen, siehe CompGravField.cpp)

    // Setters
	void setGravType( GravType t ) { m_gravType = t; }
	void setGravDir( const Vector2D& dir );
	void setGravCenter( const Vector2D& center, float strenght );
    void setPriority( int priority );

    // Getters
	GravType getGravType() const { return m_gravType; }
	const Vector2D& getGravDir() const { return m_gravitationDir; }
	const Vector2D& getGravCenter() const { return m_gravitationCenter; }
	float getStrenght() const { return m_strenght; }
	int getPriority() const { return m_priority; }

	// Get the acceleration of a body with center of mass "centerOfMass"
	Vector2D getAcceleration(const Vector2D& centerOfMass) const;

    static boost::shared_ptr<CompGravField> loadFromXml(const pugi::xml_node& compElem);
    void writeToXml(pugi::xml_node& compNode) const;

private:

	GravType m_gravType;
	Vector2D m_gravitationDir;
	Vector2D m_gravitationCenter;
	float m_strenght; // only with Radial
    int m_priority; // Gravitationsfeld-priorität: wenn sich 2 felder überlappen, gilt der mit der grössten priorität (0-100 is gültig)

};

#endif
