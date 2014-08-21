/*
 * CompPath.cpp
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */

#include "CompPath.h"
#include "common/Foreach.h"
#include "common/DataLoader.h"

#include <boost/scoped_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

using boost::property_tree::ptree;

namespace {
    PathPoint nullPathPoint(Vector2D(), 0.0f);
}

const ComponentTypeId& CompPath::getTypeIdStatic()
{
    static boost::scoped_ptr<ComponentTypeId> typeId (new ComponentTypeId("CompPath"));
    return *typeId;
}

CompPath::CompPath(const ComponentId& id, GameEvents& gameEvents)
: Component(id, gameEvents)
{
}

const PathPoint& CompPath::getPathPoint(size_t i) const
{
    if (i >= m_points.size())
    {
        log(Error) << "CompPath::getPathPoint(i): index out of bounds\n";
        return nullPathPoint;
    }
    return m_points[i];
}


void CompPath::loadFromPropertyTree(const boost::property_tree::ptree& propTree)
{
    foreach(const ptree::value_type &v, propTree)
    {
        if (v.first == "type" || v.first == "id") continue; // TODO: type/id shouldn't be here
        if (v.first != "point")
            throw DataLoadException(std::string("invalid node '") + v.first + "'");

        const ptree& point = v.second;
        float x = point.get<float>("x");
        float y = point.get<float>("y");
        float a = point.get<float>("a");
        m_points.push_back(PathPoint(Vector2D(x, y), a));
    }
}

void CompPath::writeToPropertyTree(boost::property_tree::ptree& propTree) const
{
    foreach(const PathPoint& p, m_points)
    {
        ptree pointPropTree;
        pointPropTree.add("x", p.position.x);
        pointPropTree.add("y", p.position.y);
        pointPropTree.add("a", p.angle);
        propTree.add_child("point", pointPropTree);
    }
}

