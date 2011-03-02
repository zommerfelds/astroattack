/*
 * CompVisualMessage.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Zeigt eine Nachricht auf dem Bildschirm

#ifndef COMPVISUALMESSAGE_H
#define COMPVISUALMESSAGE_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "../Component.h"
#include <string>
#include <boost/shared_ptr.hpp>
namespace pugi { class xml_node; }

//--------------------------------------------//
//-------- CompVisualMessage Klasse ----------//
//--------------------------------------------//
class CompVisualMessage : public Component
{
public:
    CompVisualMessage( std::string text );

    const CompIdType& getComponentId() const { return COMPONENT_ID; }
	static const CompIdType COMPONENT_ID; // eindeutige ID für diese Komponentenart (gleich wie Klassennamen, siehe CompCollectable.cpp)

    const std::string& getMsg() const { return m_text; }

    static boost::shared_ptr<CompVisualMessage> loadFromXml(const pugi::xml_node& compElem);
    void writeToXml(pugi::xml_node& compNode) const;

private:
    std::string m_text;
};

#endif
