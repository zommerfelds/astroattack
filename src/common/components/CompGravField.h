/*
 * CompGravField.h
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */
 
// Komponente für Gravitationsfelder

#ifndef COMPGRAVFIELD_H
#define COMPGRAVFIELD_H

#include "common/Component.h"
#include "common/Vector2D.h"
#include <set>

class CompGravField : public Component
{
public:
    enum GravType { Directional, Radial };

    CompGravField(const ComponentId& id, GameEvents& gameEvents);

    const ComponentTypeId& getTypeId() const { return getTypeIdStatic(); }
    static const ComponentTypeId& getTypeIdStatic(); // eindeutige ID für diese Komponentenart (gleich wie Klassennamen, siehe CompGravField.cpp)

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

    void loadFromPropertyTree(const boost::property_tree::ptree& propTree);
    void writeToPropertyTree(boost::property_tree::ptree& propTree) const;

private:

    GravType m_gravType;
    Vector2D m_gravitationDir;
    Vector2D m_gravitationCenter;
    float m_strenght; // only with Radial
    int m_priority; // Gravitationsfeld-priorität: wenn sich 2 felder überlappen, gilt der mit der grössten priorität (0-100 is gültig)

};

#endif
