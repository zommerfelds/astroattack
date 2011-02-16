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
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

// TODO: use pimpl to hide the next two includes
#include "Event.h"
#if 0
#include <Box2D/Box2D.h>
#endif

extern const float PHYS_DELTA_TIME;

struct GameEvents;
class Entity;
class EventConnection;
class CompPhysics;
class CompGravField;
class b2World;
class Vector2D;

class PhysicsSubSystem
{
public:
    PhysicsSubSystem( GameEvents* pGameEvents);
    ~PhysicsSubSystem(); // need to implement destructor manually because of scoped_ptr (incomplete type)

    void Init();
    void Update();

    void CalculateSmoothPositions(float accumulator);

private:
    boost::scoped_ptr<Vector2D> m_pGravitationalAcc; // Fallbeschleunigung [m/s^2]

    std::vector< CompPhysics* > m_physicsComps;
	std::vector< CompGravField* > m_gravFields;

    EventConnection m_eventConnection1;
    EventConnection m_eventConnection2;
	EventConnection m_eventConnection3;
    EventConnection m_eventConnection4;
    GameEvents* m_pGameEvents;

    void RegisterPhysicsComp( Entity& entity );
    void UnregisterPhysicsComp( Entity& entity );
	void RegisterGravFieldComp( Entity& entity );
    void UnregisterGravFieldComp( Entity& entity );

    boost::scoped_ptr<b2World> m_world;
    float m_timeStep;
    int m_velocityIterations;
    int m_positionIterations;
};

#endif
