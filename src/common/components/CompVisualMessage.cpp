/*
 * CompVisualMessage.cpp
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */

#include "CompVisualMessage.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/scoped_ptr.hpp>

// eindeutige ID
const ComponentTypeId& CompVisualMessage::getTypeIdStatic()
{
	static boost::scoped_ptr<ComponentTypeId> typeId (new ComponentTypeId("CompVisualMessage"));
    return *typeId;
}

// Konstruktor
CompVisualMessage::CompVisualMessage(const ComponentId& id, GameEvents& gameEvents, const std::string& text) :
  Component(id, gameEvents),
  m_text ( text )
{}

void CompVisualMessage::setMsg(const std::string& text)
{
    m_text = text;
}

void CompVisualMessage::loadFromPropertyTree(const boost::property_tree::ptree& propTree)
{
    m_text = propTree.get<std::string>("text");
}

void CompVisualMessage::writeToPropertyTree(boost::property_tree::ptree& propTree) const
{
    propTree.add("text", getMsg());
}
