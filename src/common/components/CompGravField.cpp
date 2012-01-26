/*
 * CompGravField.cpp
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#include "CompGravField.h"
// TODO: find out why you cannot change the order of this #includes
#include "common/DataLoader.h"
#include <boost/property_tree/ptree.hpp>

// eindeutige ID
const ComponentTypeId& CompGravField::getTypeIdStatic()
{
    static ComponentTypeId* typeId = new ComponentTypeId("CompGravField");
    return *typeId;
}

// Konstruktor
CompGravField::CompGravField(const ComponentIdType& id, GameEvents& gameEvents)
: Component(id, gameEvents), m_gravType ( Directional ), m_priority( 50 )
{
    m_gravitationDir.set( 0.0, 1.0 );
    m_gravitationCenter.set( 0.0, 1.0 );
}

void CompGravField::setGravDir(const Vector2D& dir)
{
    m_gravitationDir = dir;
}

void CompGravField::setGravCenter(const Vector2D& center, float strenght)
{
    m_gravitationCenter = center;
    m_strenght = strenght;
}

void CompGravField::setPriority( int priority )
{
    if ( std::abs(priority) > 100 )
        return; // ACHTUNG: sende warnung
    m_priority = priority;
}

// Get the acceleration of a body with center of mass "centerOfMass"
Vector2D CompGravField::getAcceleration(const Vector2D& centerOfMass) const
{
    if ( m_gravType == Directional ) 
    {
        return getGravDir();
    }
    else if ( m_gravType == Radial )
    {
        Vector2D acc( getGravCenter()-centerOfMass );
        acc.normalize();
        acc *= m_strenght;
        return acc;
    }
    return Vector2D( 0.0f, 0.0f );
}

void CompGravField::loadFromPropertyTree(const boost::property_tree::ptree& propTree)
{
    std::string typeStr = propTree.get<std::string>("grav_type");
    if (typeStr == "directional")
    {
        setGravType(Directional);

        float gx = propTree.get("gx", 0.0f);
        float gy = propTree.get("gy", 0.0f);

        setGravDir(Vector2D(gx, gy));
    }
    else if (typeStr == "radial")
    {
        setGravType(Radial);

        float cx = propTree.get("cx", 0.0f);
        float cy = propTree.get("cy", 0.0f);
        float s = propTree.get("s", 0.0f);

        setGravCenter(Vector2D(cx, cy), s);
    }
    else
        throw DataLoadException(std::string("invalid gravitation type '") + typeStr + "'");
}

void CompGravField::writeToPropertyTree(boost::property_tree::ptree& propTree) const
{
    switch (getGravType())
    {
    case Directional:
    {
        propTree.add("grav_type", "directional");
        propTree.add("gx", getGravDir().x);
        propTree.add("gy", getGravDir().y);
        break;
    }
    case Radial:
    {
        propTree.add("grav_type", "radial");
        propTree.add("cx", getGravCenter().x);
        propTree.add("cy", getGravCenter().y);
        propTree.add("s", getStrenght());
        break;
    }
    }
}

