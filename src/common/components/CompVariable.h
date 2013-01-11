/*
 * CompVariable.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#ifndef COMPVARIABLE_H
#define COMPVARIABLE_H

#include "../Component.h"

class CompVariable : public Component
{
public:
    CompVariable(const ComponentId& id, GameEvents& events);
    const ComponentTypeId& getTypeId() const { return getTypeIdStatic(); }
    static const ComponentTypeId& getTypeIdStatic();

    int getValue() const;
    void setValue(int val);

    void loadFromPropertyTree(const boost::property_tree::ptree& propTree);
    void writeToPropertyTree(boost::property_tree::ptree& propTree) const;
private:
    int m_value;
};

#endif /* COMPVARIABLE_H_ */
