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
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

enum GravType { Directional, Radial };

class Vector2D;
namespace pugi { class xml_node; }

//--------------------------------------------//
//--------- CompGravField Klasse -------------//
//--------------------------------------------//
class CompGravField : public Component
{
public:
    CompGravField();
    ~CompGravField(); // need to implement destructor manually because of scoped_ptr (incomplete type)

    const CompIdType& ComponentId() const { return COMPONENT_ID; }
    static const CompIdType COMPONENT_ID; // eindeutige ID für diese Komponentenart (gleich wie Klassennamen, siehe CompGravField.cpp)

    // Setters
	void SetGravType( GravType t ) { m_gravType = t; }
	void SetGravDir( const Vector2D& dir );
	void SetGravCenter( const Vector2D& center, float strenght );
    void SetPriority( int priority );

    // Getters
	GravType GetGravType() const { return m_gravType; }
	const Vector2D& GetGravDir() const { return *m_pGravitationDir; }
	const Vector2D& GetGravCenter() const { return *m_pGravitationCenter; }
	float GetStrenght() const { return m_strenght; }
	int GetPriority() const { return m_priority; }

	// Get the acceleration of a body with center of mass "centerOfMass"
	Vector2D GetAcceleration(const Vector2D& centerOfMass) const;

    static boost::shared_ptr<CompGravField> LoadFromXml(const pugi::xml_node& compElem);
    void WriteToXml(pugi::xml_node& compNode) const;

private:

	GravType m_gravType;
	boost::scoped_ptr<Vector2D> m_pGravitationDir;
	boost::scoped_ptr<Vector2D> m_pGravitationCenter;
	float m_strenght; // only with Radial
    int m_priority; // Gravitationsfeld-priorität: wenn sich 2 felder überlappen, gilt der mit der grössten priorität (0-100 is gültig)

};
//--------------------------------------------//
//------- Ende CompGravField Klasse --------//
//--------------------------------------------//

#endif
