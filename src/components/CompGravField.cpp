/*
 * CompGravField.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "CompGravField.h"
#include <boost/make_shared.hpp>

#include "../contrib/pugixml/pugixml.hpp"

// eindeutige ID
const CompIdType CompGravField::COMPONENT_ID = "CompGravField";

// Konstruktor
CompGravField::CompGravField() : m_gravType ( Directional ), m_priority( 50 )
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

boost::shared_ptr<CompGravField> CompGravField::loadFromXml(const pugi::xml_node& compElem)
{
    pugi::xml_node gravElem = compElem.child("grav");
    std::string typeStr = gravElem.attribute("type").value();

    boost::shared_ptr<CompGravField> compGrav = boost::make_shared<CompGravField>();

    if (typeStr == "directional")
    {
        compGrav->setGravType(Directional);

        float gx = gravElem.attribute("gx").as_float();
        float gy = gravElem.attribute("gy").as_float();

        compGrav->setGravDir(Vector2D(gx, gy));
    }
    else if (typeStr == "radial")
    {
        compGrav->setGravType(Radial);

        float cx = gravElem.attribute("cx").as_float();
        float cy = gravElem.attribute("cy").as_float();
        float s = gravElem.attribute("s").as_float();

        compGrav->setGravCenter(Vector2D(cx, cy), s);
    }
    return compGrav;
}

void CompGravField::writeToXml(pugi::xml_node& compNode) const
{
    pugi::xml_node gravNode = compNode.append_child("grav");
    switch (getGravType())
    {
    case Directional:
    {
        gravNode.append_attribute("type").set_value("directional");
        gravNode.append_attribute("gx").set_value(getGravDir().x);
        gravNode.append_attribute("gy").set_value(getGravDir().y);
    }
    case Radial:
    {
        gravNode.append_attribute("type").set_value("radial");
        gravNode.append_attribute("cx").set_value(getGravCenter().x);
        gravNode.append_attribute("cy").set_value(getGravCenter().y);
        gravNode.append_attribute("s").set_value(getStrenght());
    }
    }
}
