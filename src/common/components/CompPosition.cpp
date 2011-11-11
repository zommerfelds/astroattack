/*
 * CompPosition.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// CompPosition.h für mehr Informationen

#include <boost/property_tree/ptree.hpp>

#include "CompPosition.h"
#include "CompPhysics.h"

// eindeutige ID
const ComponentTypeId CompPosition::COMPONENT_TYPE_ID = "CompPosition";

// TODO: store CompPhysics pointer for multiple uses (maybe weak_ptr?)

CompPosition::CompPosition(const ComponentIdType& id, GameEvents& gameEvents)
: Component(id, gameEvents), m_position (), m_orientation ( 0.0f )
{}

/*void CompPosition::setPosition(const Vector2D& pos)
{
    // TODO: set Box2D pos
    m_position = pos;
}*/

Vector2D CompPosition::getDrawingPosition() const
{
    const CompPhysics* compPhys = getSiblingComponent<CompPhysics>(); // important TODO: this can segfault
    if (compPhys)
        return compPhys->getSmoothPosition();
    return m_position;
}

float CompPosition::getDrawingOrientation() const
{
    const CompPhysics* compPhys = getSiblingComponent<CompPhysics>();
    if (compPhys)
        return compPhys->getSmoothAngle();
    return m_orientation;
}

/*void CompPosition::setOrientation(float orientation)
{
    // TODO: set Box2D angle
    m_orientation = orientation;
}*/

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

