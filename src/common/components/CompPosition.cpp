/*
 * CompPosition.cpp
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

// CompPosition.h für mehr Informationen

#include "CompPosition.h"

#include "CompPhysics.h"
#include <boost/scoped_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

// eindeutige ID
const ComponentTypeId& CompPosition::getTypeIdStatic()
{
    static boost::scoped_ptr<ComponentTypeId> typeId (new ComponentTypeId("CompPosition"));
    return *typeId;
}

CompPosition::CompPosition(const ComponentId& id, GameEvents& gameEvents, const Vector2D& pos)
: Component(id, gameEvents),
  m_position (pos),
  m_orientation (0.0f),
  m_compPhysics (NULL)
{}

/*void CompPosition::setPosition(const Vector2D& pos)
{
    // set Box2D pos
    m_position = pos;
}*/

/*void CompPosition::setOrientation(float orientation)
{
    // set Box2D angle
    m_orientation = orientation;
}*/

Vector2D CompPosition::getDrawingPosition() const
{
    //const CompPhysics* compPhys = getSiblingComponent<CompPhysics>();
    if (m_compPhysics)
        return m_compPhysics->getSmoothPosition();
    return m_position;
}

float CompPosition::getDrawingOrientation() const
{
    if (m_compPhysics)
        return m_compPhysics->getSmoothAngle();
    return m_orientation;
}

Vector2D CompPosition::getPosition() const
{
    return m_position;
}

float CompPosition::getOrientation() const
{
    return m_orientation;
}

void CompPosition::loadFromPropertyTree(const boost::property_tree::ptree& propTree)
{
    float x = propTree.get<float>("x");
    float y = propTree.get<float>("y");
    float a = propTree.get("a", 0.0f);

    m_position.set(x, y);
    m_orientation = a;
}

void CompPosition::writeToPropertyTree(boost::property_tree::ptree& propTree) const
{
    propTree.add("x", m_position.x);
    propTree.add("y", m_position.y);
    propTree.add("a", m_orientation);
}

