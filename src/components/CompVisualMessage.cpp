/*----------------------------------------------------------\
|                 CompVisualMessage.cpp                     |
|                 ---------------------                     |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "CompVisualMessage.h"

#include "contrib/pugixml/pugixml.hpp"
#include <boost/make_shared.hpp>

// eindeutige ID
const CompIdType CompVisualMessage::COMPONENT_ID = "CompVisualMessage";

// Konstruktor
CompVisualMessage::CompVisualMessage( std::string text )
: m_text ( text )
{
}

CompVisualMessage::~CompVisualMessage()
{
}

boost::shared_ptr<CompVisualMessage> CompVisualMessage::LoadFromXml(const pugi::xml_node& compElem)
{
    const char* msg = compElem.child("msg").attribute("text").value();

    return boost::make_shared<CompVisualMessage>(msg);
}

void CompVisualMessage::WriteToXml(pugi::xml_node& compNode) const
{
    pugi::xml_node textNode = compNode.append_child("msg");
    textNode.append_attribute("text").set_value(GetMsg().c_str());
}

// Astro Attack - Christian Zommerfelds - 2009
