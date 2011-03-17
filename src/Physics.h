/*
 * Physics.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Steuerung der Physik

#ifndef PHYSICS_H
#define PHYSICS_H

#include <vector>
#include <Box2D/Box2D.h>

#include "Event.h"
#include "Vector2D.h"

struct GameEvents;
class Entity;
class EventConnection;
class CompPhysics;
class CompGravField;

extern const float cPhysicsTimeStep;

class PhysicsSubSystem
{
public:
    PhysicsSubSystem( GameEvents& gameEvents);

    void init();
    void update();

    void calculateSmoothPositions(float accumulator);

private:
    Vector2D m_pGravitationalAcc; // Fallbeschleunigung [m/s^2]

    std::vector< CompPhysics* > m_physicsComps;
	std::vector< CompGravField* > m_gravFields;

    EventConnection m_eventConnection1;
    EventConnection m_eventConnection2;
	EventConnection m_eventConnection3;
    EventConnection m_eventConnection4;
    GameEvents& m_gameEvents;

    void onRegisterEntity_phys( Entity& entity );
    void onUnregisterEntity_phys( Entity& entity );
	void onRegisterEntity_grav( Entity& entity );
    void onUnregisterEntity_grav( Entity& entity );

    b2World m_world;
    float m_timeStep;
    int m_velocityIterations;
    int m_positionIterations;
};

#endif
