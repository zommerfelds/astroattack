/*
 * Physics.h
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */

// Steuerung der Physik

#ifndef PHYSICS_H
#define PHYSICS_H

#include "common/Event.h"
#include "common/components/CompGravField.h"
#include <vector>
#include <list>
#include <Box2D/Box2D.h>
#include <boost/optional.hpp>

class GameEvents;
class Component;
class EventConnection;
class CompPhysics;
class CompGravField;
class CompPathMove;
class CompPath;
class CompPosition;

extern const float cPhysicsTimeStep;

class PhysicsSystem
{
public:
    PhysicsSystem(GameEvents& gameEvents);

    void update();
    boost::optional<std::pair<EntityId, std::vector<const Component*> > > selectEntity(const Vector2D& pos);

    void calculateSmoothPositions(float accumulator);

private:
    std::vector< CompPhysics* > m_physicsComps;
    std::vector< CompGravField* > m_gravFields;
    std::list< CompPathMove* > m_pathMoves;

    EventConnection m_eventConnection1;
    EventConnection m_eventConnection2;
    GameEvents& m_gameEvents;

    void onRegisterComp(Component&);
    void onUnregisterComp(Component&);
    void onRegisterCompPhys(CompPhysics&);
    void onUnregisterCompPhys(CompPhysics&);
    void onRegisterCompGrav(CompGravField&);
    void onUnregisterCompGrav(CompGravField&);
    void onRegisterCompPathMove(CompPathMove&);
    void onUnregisterCompPathMove(CompPathMove&);

    void setNewTarget(CompPathMove& compPathMove, CompPhysics& compPhys, const CompPosition& compPos, const CompPath& compPath, size_t point);
    bool updatePathMove(CompPathMove&, CompPhysics&, const CompPosition&, const CompPath&); // return true if the CompPathMove finished the move

    b2World m_world;

    CompGravField m_rootGravField; // TODO: put this component in the manager?
};

#endif
