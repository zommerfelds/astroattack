/*
 * CompPathMove.cpp
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */

#include "CompPathMove.h"

#include "common/Foreach.h"
#include "common/DataLoader.h"

#include <boost/scoped_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

using boost::property_tree::ptree;

namespace {
    PathMovePoint nullPathPoint = PathMovePoint::makeUniformByTime(0);
    const std::string cMode = "mode";
    const std::string cModeUniform = "uniform";
    const std::string cModeAccelerated = "accelerated";
    const std::string cLinVel = "linVel";
    const std::string cAngVel = "angVel";
    const std::string cTopLinVel = "topLinVel";
    const std::string cTopAngVel = "topAngVel";
    const std::string cLinAccel = "linAccel";
    const std::string cAngAccel = "angAccel";
    const std::string cTime = "time";
    const std::string cPoint = "point";
}

const ComponentTypeId& CompPathMove::getTypeIdStatic()
{
    static boost::scoped_ptr<ComponentTypeId> typeId (new ComponentTypeId("CompPathMove"));
    return *typeId;
}

CompPathMove::CompPathMove(const ComponentId& id, GameEvents& gameEvents)
: Component(id, gameEvents),
  m_repeat (false),
  m_resetAngle (false),
  //m_velocity (1.0f),
  m_currentPathPoint (0),
  m_updateCount (0),
  m_numUpdatesToNextPoint (0),
  m_numUpdatesToAccelerate (0),
  m_detalLinearVelocity (),
  m_detalAngularVelocity (0.0f)
{
}

const PathMovePoint& CompPathMove::getPathPoint(size_t i) const
{
    if (i >= m_points.size())
    {
        log(Error) << "CompPathMove::getPathPoint(i): index out of bounds\n";
        return nullPathPoint;
    }
    return m_points[i];
}

void CompPathMove::loadFromPropertyTree(const boost::property_tree::ptree& propTree)
{
    m_pathId = propTree.get("path_id", "");
    m_repeat = propTree.get("repeat", false);
    m_resetAngle = propTree.get("resetAngle", false);
    //m_velocity = propTree.get<float>("velocity");
    //if (propTree.get<std::string>("mode") != "relative")
    //    throw DataLoadException("CompPathMove: Only 'relatve' mode is supported");

    foreach(const ptree::value_type &v, propTree)
    {
        if (v.first != cPoint) continue;

        const ptree& point = v.second;
        PathMovePoint::MotionMode mode;

        std::string modeStr = point.get<std::string>(cMode);
        if      (modeStr == cModeUniform)     mode = PathMovePoint::Uniform;
        else if (modeStr == cModeAccelerated) mode = PathMovePoint::Accelerated;
        else throw DataLoadException("PathMovePoint: invalid mode '"+modeStr+"'");
        float linAccel  = point.get(cLinAccel, 0.0f);
        float angAccel  = point.get(cAngAccel, 0.0f);
        float topLinVel = point.get(cTopLinVel, 0.0f);
        float topAngVel = point.get(cTopAngVel, 0.0f);
        float linVel    = point.get(cLinVel, 0.0f);
        float angVel    = point.get(cAngVel, 0.0f);
        float time      = point.get(cTime, 0.0f);

        switch (mode)
        {
        case PathMovePoint::Uniform:
            if (time != 0)
                m_points.push_back(PathMovePoint::makeUniformByTime(time));
            else if (linVel != 0)
                m_points.push_back(PathMovePoint::makeUniformByLinVel(linVel));
            else if (angVel != 0)
                m_points.push_back(PathMovePoint::makeUniformByAngVel(angVel));
            else
                throw DataLoadException("invalid uniform PathMovePoint");
            break;
        case PathMovePoint::Accelerated:
            if (linAccel != 0.0f)
                m_points.push_back(PathMovePoint::makeLinAccelerated(linAccel, topLinVel));
            else if (angAccel != 0.0f)
                m_points.push_back(PathMovePoint::makeAngAccelerated(angAccel, topAngVel));
            else
                throw DataLoadException("invalid accelerated PathMovePoint");
            break;
        }
    }
}

void CompPathMove::writeToPropertyTree(boost::property_tree::ptree& propTree) const
{
    if (m_pathId != "")
        propTree.put("path_id", m_pathId);
    if (m_repeat == true)
        propTree.put("repeat", m_repeat);
    if (m_resetAngle == true)
        propTree.put("resetAngle", m_resetAngle);
    //propTree.put("velocity", m_velocity);

    foreach(const PathMovePoint& p, m_points)
    {
        ptree pointPropTree;
        switch (p.mode)
        {
        case PathMovePoint::Uniform:
            pointPropTree.add(cMode, cModeUniform);
            if (p.time != 0.0f)
                pointPropTree.add(cTime, p.time);
            else if (p.linVel != 0.0f)
                pointPropTree.add(cLinVel, p.linVel);
            else
                pointPropTree.add(cAngVel, p.angVel);
            break;
        case PathMovePoint::Accelerated:
            pointPropTree.add(cMode, cModeAccelerated);
            if (p.linAccel != 0.0f)
            {
                pointPropTree.add(cLinAccel, p.linAccel);
                if (p.topLinVel != 0.0f)
                    pointPropTree.add(cTopLinVel, p.topLinVel);
            }
            else
            {
                pointPropTree.add(cAngAccel, p.angAccel);
                if (p.topAngVel != 0.0f)
                    pointPropTree.add(cTopAngVel, p.topAngVel);
            }
            break;
        }
        propTree.add_child(cPoint, pointPropTree);
    }
}

