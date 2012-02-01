/*
 * Physics.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

// Steuerung der Physik

#ifndef PHYSICS_H
#define PHYSICS_H

#include "common/Event.h"
#include "common/components/CompGravField.h"
#include <vector>
#include <Box2D/Box2D.h>
#include <boost/optional.hpp>

struct GameEvents;
class Component;
class EventConnection;
class CompPhysics;
class CompGravField;

extern const float cPhysicsTimeStep;

class PhysicsSubSystem
{
public:
    PhysicsSubSystem(GameEvents& gameEvents);

    void update();
    boost::optional<std::pair<EntityIdType, std::vector<Component*> > > selectEntity(const Vector2D& pos);

    void calculateSmoothPositions(float accumulator);

private:
    std::vector< CompPhysics* > m_physicsComps;
    std::vector< CompGravField* > m_gravFields;

    EventConnection m_eventConnection1;
    EventConnection m_eventConnection2;
    GameEvents& m_gameEvents;

    void onRegisterComp(Component&);
    void onUnregisterComp(Component&);
    void onRegisterCompPhys(CompPhysics&);
    void onUnregisterCompPhys(CompPhysics&);
    void onRegisterCompGrav(CompGravField&);
    void onUnregisterCompGrav(CompGravField&);

    b2World m_world;
    float m_timeStep;
    int m_velocityIterations;
    int m_positionIterations;

    CompGravField m_rootGravField; // TODO: put this component in the manager
};

#endif
