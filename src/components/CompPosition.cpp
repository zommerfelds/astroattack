/*
 * CompPosition.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// CompPosition.h für mehr Informationen

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "CompPosition.h"
#include "CompPhysics.h"
#include "../Entity.h"
#include <boost/make_shared.hpp>

#include "../contrib/pugixml/pugixml.hpp"

// eindeutige ID
const CompIdType CompPosition::COMPONENT_ID = "CompPosition";

// TODO: use weak_ptr for CompPhysics pointer (store for multiple uses)

CompPosition::CompPosition() : m_position (), m_orientation ( 0.0f ) {}

void CompPosition::setPosition(const Vector2D& pos)
{
    // TODO: set Box2D pos
    m_position = pos;
}

const Vector2D& CompPosition::getPosition() const
{
    const CompPhysics* compPhys = getOwnerEntity()->getComponent<CompPhysics>(); // important TODO: this can segfault
    if (compPhys)
        return compPhys->getSmoothPosition();
    return m_position;
}

const Vector2D& CompPosition::getPosIgnoreCompPhys() const
{
    return m_position;
}

float CompPosition::getOrientation() const
{
    const CompPhysics* compPhys = getOwnerEntity()->getComponent<CompPhysics>();
    if (compPhys)
        return compPhys->getSmoothAngle();
    return m_orientation;
}

void CompPosition::setOrientation(float orientation)
{
    // TODO: set Box2D angle
    m_orientation = orientation;

}
boost::shared_ptr<CompPosition> CompPosition::loadFromXml(const pugi::xml_node& compElem)
{
    float x = compElem.attribute("x").as_float();
    float y = compElem.attribute("y").as_float();
    float a = compElem.attribute("a").as_float();

    if (y < 0)
        a = 1;

    boost::shared_ptr<CompPosition> compPos = boost::make_shared<CompPosition> ();
    compPos->setPosition(Vector2D(x, y));
    compPos->setOrientation(a);
    return compPos;
}

void CompPosition::writeToXml(pugi::xml_node& compNode) const
{
    compNode.append_attribute("x").set_value(getPosition().x);
    compNode.append_attribute("y").set_value(getPosition().y);
    compNode.append_attribute("a").set_value(getOrientation());
}
