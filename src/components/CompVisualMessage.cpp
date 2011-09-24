/*
 * CompVisualMessage.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include <boost/property_tree/ptree.hpp>

#include "CompVisualMessage.h"

// eindeutige ID
const ComponentTypeId CompVisualMessage::COMPONENT_TYPE_ID = "CompVisualMessage";

// Konstruktor
CompVisualMessage::CompVisualMessage(const ComponentIdType& id, GameEvents& gameEvents, const std::string& text) :
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
