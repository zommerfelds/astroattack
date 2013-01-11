/*
 * CompPath.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#ifndef COMPPATH_H
#define COMPPATH_H

#include "common/Vector2D.h"
#include "common/Component.h"
#include <vector>

class PathPoint
{
public:
    PathPoint(const Vector2D& v, float a) : position (v), angle (a) {}
    Vector2D position;
    float angle;
};

class CompPath : public Component
{
public:
    CompPath(const ComponentId& id, GameEvents& gameEvents);

    const ComponentTypeId& getTypeId() const { return getTypeIdStatic(); }
    static const ComponentTypeId& getTypeIdStatic();

    const PathPoint& getPathPoint(size_t i) const;
    int              getNumPathPoints()     const { return m_points.size(); }

    void loadFromPropertyTree(const boost::property_tree::ptree& propTree);
    void writeToPropertyTree(boost::property_tree::ptree& propTree) const;

private:

    std::vector<PathPoint> m_points;
};

#endif
