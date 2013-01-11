/*
 * CompPathMove.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#ifndef COMPPATHMOVE_H
#define COMPPATHMOVE_H

#include "common/Component.h"
#include "common/Vector2D.h"

#include <vector>

class PathMovePoint
{
public:
    enum MotionMode {
        Uniform, Accelerated
    };
    MotionMode mode;
    float linAccel;
    float angAccel;
    float time;
    float linVel;
    float angVel;
    float topLinVel;
    float topAngVel;

    static PathMovePoint makeUniformByTime(float time)                       { return PathMovePoint(Uniform, 0, 0, time, 0, 0, 0, 0); }
    static PathMovePoint makeUniformByLinVel(float linVel)                   { return PathMovePoint(Uniform, 0, 0, 0, linVel, 0, 0, 0); }
    static PathMovePoint makeUniformByAngVel(float angVel)                   { return PathMovePoint(Uniform, 0, 0, 0, 0, angVel, 0, 0); }
    static PathMovePoint makeLinAccelerated(float linAccel, float topLinVel) { return PathMovePoint(Accelerated, linAccel, 0, 0, 0, 0, topLinVel, 0); }
    static PathMovePoint makeAngAccelerated(float angAccel, float topAngVel) { return PathMovePoint(Accelerated, 0, angAccel, 0, 0, 0, 0, topAngVel); }

private:
    PathMovePoint(MotionMode m, float lA, float aA, float t, float lV, float aV, float tLV, float tAV) : mode (m), linAccel (lA), angAccel (aA), time (t), linVel (lV), angVel (aV), topLinVel (tLV), topAngVel (tAV) {}
};

class CompPathMove : public Component
{
public:
    CompPathMove(const ComponentId& id, GameEvents& gameEvents);

    const ComponentTypeId& getTypeId() const { return getTypeIdStatic(); }
    static const ComponentTypeId& getTypeIdStatic();

    const PathMovePoint& getPathPoint(size_t i) const;
    int                  getNumPathPoints()     const { return m_points.size(); }

    const ComponentId& getPathId()           const { return m_pathId; }
    bool               getRepeat()           const { return m_repeat; }
    bool               getResetAngle()       const { return m_resetAngle; }
    //float              getVelocity()         const { return m_velocity; }
    int                getCurrentPathPoint() const { return m_currentPathPoint; }

    //void setCurrentPathPoint(int i) { m_currentPathPoint = i; }

    void loadFromPropertyTree(const boost::property_tree::ptree& propTree);
    void writeToPropertyTree(boost::property_tree::ptree& propTree) const;
private:
    ComponentId m_pathId;
    bool m_repeat;
    bool m_resetAngle;
    //float m_velocity;

    int m_currentPathPoint;

    int m_updateCount;
    int m_numUpdatesToNextPoint;
    int m_numUpdatesToAccelerate;
    Vector2D m_detalLinearVelocity;
    float m_detalAngularVelocity;

    std::vector<PathMovePoint> m_points;

    friend class PhysicsSystem;
};

#endif
