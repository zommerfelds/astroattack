/*
 * CompPosition.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// CompPosition.h für mehr Informationen

#include <boost/property_tree/ptree.hpp>

#include "CompPosition.h"
#include "CompPhysics.h"
#include "../Entity.h"

// eindeutige ID
const ComponentTypeId CompPosition::COMPONENT_TYPE_ID = "CompPosition";

// TODO: use weak_ptr for CompPhysics pointer (store for multiple uses)

CompPosition::CompPosition() : m_position (), m_orientation ( 0.0f ) {}

/*void CompPosition::setPosition(const Vector2D& pos)
{
    // TODO: set Box2D pos
    m_position = pos;
}*/

const Vector2D& CompPosition::getDrawingPosition() const
{
    const CompPhysics* compPhys = getOwnerEntity()->getComponent<CompPhysics>(); // important TODO: this can segfault
    if (compPhys)
        return compPhys->getSmoothPosition();
    return m_position;
}

float CompPosition::getDrawingOrientation() const
{
    const CompPhysics* compPhys = getOwnerEntity()->getComponent<CompPhysics>();
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

