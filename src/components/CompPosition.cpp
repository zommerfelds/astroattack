/*----------------------------------------------------------\
|                    CompPosition.cpp                       |
|                    ----------------                       |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// CompPosition.h für mehr Informationen

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "CompPosition.h"
#include "CompPhysics.h"
#include "../Entity.h"
#include "../Vector2D.h"
#include <boost/make_shared.hpp>

#include "../contrib/pugixml/pugixml.hpp"

// eindeutige ID
const CompIdType CompPosition::COMPONENT_ID = "CompPosition";

// TODO: use weak_ptr for CompPhysics pointer (store for multiple uses)

CompPosition::CompPosition() : m_pPosition ( new Vector2D ), m_orientation ( 0.0f )
{
}

CompPosition::~CompPosition()
{
}

void CompPosition::SetPosition(const Vector2D& pos)
{
    // TODO: set Box2D pos
    *m_pPosition = pos;
}

const Vector2D& CompPosition::GetPosition() const
{
    const CompPhysics* compPhys = GetOwnerEntity()->GetComponent<CompPhysics>(); // important TODO: this can segfault
    if (compPhys)
        return compPhys->GetSmoothPosition();
    return *m_pPosition;
}

const Vector2D& CompPosition::GetPosIgnoreCompPhys() const
{
    return *m_pPosition;
}

float CompPosition::GetOrientation() const
{
    const CompPhysics* compPhys = GetOwnerEntity()->GetComponent<CompPhysics>();
    if (compPhys)
        return compPhys->GetSmoothAngle();
    return m_orientation;
}

void CompPosition::SetOrientation(float orientation)
{
    // TODO: set Box2D angle
    m_orientation = orientation;

}
boost::shared_ptr<CompPosition> CompPosition::LoadFromXml(const pugi::xml_node& compElem)
{
    float x = compElem.attribute("x").as_float();
    float y = compElem.attribute("y").as_float();
    float a = compElem.attribute("a").as_float();

    if (y < 0)
        a = 1;

    boost::shared_ptr<CompPosition> compPos = boost::make_shared<CompPosition> ();
    compPos->SetPosition(Vector2D(x, y));
    compPos->SetOrientation(a);
    return compPos;
}

void CompPosition::WriteToXml(pugi::xml_node& compNode) const
{
    compNode.append_attribute("x").set_value(GetPosition().x);
    compNode.append_attribute("y").set_value(GetPosition().y);
    compNode.append_attribute("a").set_value(GetOrientation());
}

// Astro Attack - Christian Zommerfelds - 2009
