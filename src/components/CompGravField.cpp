/*----------------------------------------------------------\
|                   CompGravField.cpp                       |
|                   -----------------                       |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "CompGravField.h"
#include "../Vector2D.h"
#include <boost/make_shared.hpp>

#include "contrib/pugixml/pugixml.hpp"

// eindeutige ID
const CompIdType CompGravField::COMPONENT_ID = "CompGravField";

// Konstruktor
CompGravField::CompGravField() : m_gravType ( Directional ), m_pGravitationDir ( new Vector2D ),
m_pGravitationCenter ( new Vector2D ), m_priority( 50 )
{
    m_pGravitationDir->Set( 0.0, 1.0 );
    m_pGravitationCenter->Set( 0.0, 1.0 );
}

void CompGravField::SetGravDir(const Vector2D& dir)
{
    *m_pGravitationDir = dir;
}

void CompGravField::SetGravCenter(const Vector2D& center, float strenght)
{
    *m_pGravitationCenter = center;
    m_strenght = strenght;
}

void CompGravField::SetPriority( int priority )
{
    if ( std::abs(priority) > 100 )
        return; // ACHTUNG: sende warnung
    m_priority = priority;
}

// Get the acceleration of a body with center of mass "centerOfMass"
Vector2D CompGravField::GetAcceleration(const Vector2D& centerOfMass) const
{
    if ( m_gravType == Directional ) 
    {
        return GetGravDir();
    }
    else if ( m_gravType == Radial )
    {
        Vector2D acc( GetGravCenter()-centerOfMass );
        acc.Normalise();
        acc *= m_strenght;
        return acc;
    }
    return Vector2D( 0.0f, 0.0f );
}

boost::shared_ptr<CompGravField> CompGravField::LoadFromXml(const pugi::xml_node& compElem)
{
    pugi::xml_node gravElem = compElem.child("grav");
    std::string typeStr = gravElem.attribute("type").value();

    boost::shared_ptr<CompGravField> compGrav = boost::make_shared<CompGravField>();

    if (typeStr == "directional")
    {
        compGrav->SetGravType(Directional);

        float gx = gravElem.attribute("gx").as_float();
        float gy = gravElem.attribute("gy").as_float();

        compGrav->SetGravDir(Vector2D(gx, gy));
    }
    else if (typeStr == "radial")
    {
        compGrav->SetGravType(Radial);

        float cx = gravElem.attribute("cx").as_float();
        float cy = gravElem.attribute("cy").as_float();
        float s = gravElem.attribute("s").as_float();

        compGrav->SetGravCenter(Vector2D(cx, cy), s);
    }
    return compGrav;
}

void CompGravField::WriteToXml(pugi::xml_node& compNode) const
{
    pugi::xml_node gravNode = compNode.append_child("grav");
    switch (GetGravType())
    {
    case Directional:
    {
        gravNode.append_attribute("type").set_value("directional");
        gravNode.append_attribute("gx").set_value(GetGravDir().x);
        gravNode.append_attribute("gy").set_value(GetGravDir().y);
    }
    case Radial:
    {
        gravNode.append_attribute("type").set_value("radial");
        gravNode.append_attribute("cx").set_value(GetGravCenter().x);
        gravNode.append_attribute("cy").set_value(GetGravCenter().y);
        gravNode.append_attribute("s").set_value(GetStrenght());
    }
    }
}

// Astro Attack - Christian Zommerfelds - 2009
