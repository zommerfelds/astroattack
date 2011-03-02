/*
 * Physics.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Steuerung der Physik

#ifndef PHYSICS_H
#define PHYSICS_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include <vector>

#include "Event.h"
#include <Box2D/Box2D.h>
#include "Vector2D.h"

extern const float PHYS_DELTA_TIME;

struct GameEvents;
class Entity;
class EventConnection;
class CompPhysics;
class CompGravField;
//class b2World;
//class Vector2D;

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

    void onRegisterPhysicsComp( Entity& entity );
    void onUnregisterPhysicsComp( Entity& entity );
	void onRegisterGravFieldComp( Entity& entity );
    void onUnregisterGravFieldComp( Entity& entity );

    b2World m_world;
    float m_timeStep;
    int m_velocityIterations;
    int m_positionIterations;
};

#endif
