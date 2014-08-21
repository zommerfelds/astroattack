/*
 * CompVariable.cpp
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */

#include "CompVariable.h"

#include <boost/scoped_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

const ComponentTypeId& CompVariable::getTypeIdStatic()
{
    static boost::scoped_ptr<ComponentTypeId> typeId (new ComponentTypeId("CompVariable"));
    return *typeId;
}

CompVariable::CompVariable(const ComponentId& id, GameEvents& events)
: Component(id, events),
  m_value (0)
{
}

int CompVariable::getValue() const
{
    return m_value;
}

void CompVariable::setValue(int val)
{
    m_value = val;
}

void CompVariable::loadFromPropertyTree(const boost::property_tree::ptree& propTree)
{
    m_value = propTree.get<int>("value");
}

void CompVariable::writeToPropertyTree(boost::property_tree::ptree& propTree) const
{
    propTree.add("value", m_value);
}
