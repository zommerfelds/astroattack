/*
 * CompVisualMessage.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Zeigt eine Nachricht auf dem Bildschirm

#ifndef COMPVISUALMESSAGE_H
#define COMPVISUALMESSAGE_H

#include "common/Component.h"
#include <string>

//--------------------------------------------//
//-------- CompVisualMessage Klasse ----------//
//--------------------------------------------//
class CompVisualMessage : public Component
{
public:
    CompVisualMessage(const ComponentIdType& id, GameEvents& gameEvents, const std::string& text="");

    const ComponentTypeId& getTypeId() const { return COMPONENT_TYPE_ID; }
    static const ComponentTypeId COMPONENT_TYPE_ID; // eindeutige ID für diese Komponentenart (gleich wie Klassennamen, siehe CompCollectable.cpp)

    void setMsg(const std::string&);
    const std::string& getMsg() const { return m_text; }

    void loadFromPropertyTree(const boost::property_tree::ptree& propTree);
    void writeToPropertyTree(boost::property_tree::ptree& propTree) const;

private:
    std::string m_text;
};

#endif
