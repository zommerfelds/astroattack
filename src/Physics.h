/*----------------------------------------------------------\
|                        Physics.h                          |
|                        ---------                          |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// Steuerung der Physik

#ifndef PHYSICS_H
#define PHYSICS_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include <vector>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <Box2D/Box2D.h>
#include "Event.h" // TODO: use pimpl to hide this
extern const float PHYS_DELTA_TIME;

struct GameEvents;
class Entity;
class EventConnection;
class CompPhysics;
class CompGravField;
class Vector2D;
class RegisterObj;
class b2World;
class ContactListener;
class b2Shape;

/*
    Bei Update() wird die ganze Physik simuliert.
*/
class PhysicsSubSystem
{
public:
    PhysicsSubSystem( GameEvents* pGameEvents);
    ~PhysicsSubSystem();

    void Init();
    void Update();

    void Refilter( b2Shape* pShape ); // Recalculate collision filtering of pShape

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

    void RegisterPhysicsComp( Entity* pEntity );
    void UnregisterPhysicsComp( Entity* pEntity );
	void RegisterGravFieldComp( Entity* pEntity );
    void UnregisterGravFieldComp( Entity* pEntity );

    boost::scoped_ptr<b2World> m_world;
    float m_timeStep;
    int m_velocityIterations;
    int m_positionIterations;

    class ContactListener : public b2ContactListener
    {
    public:
        ContactListener( GameEvents* pEM ) : m_pEventManager(pEM) {}
	    /*void Add(const b2ContactPoint* point);
        void Persist(const b2ContactPoint* point);
	    void Remove(const b2ContactPoint* point);*/
    private:
        /*void CheckContactPoint(const b2ContactPoint* point);*/
        GameEvents* m_pEventManager;
    };

    boost::scoped_ptr<ContactListener> m_contactListener;
};

#endif

// Astro Attack - Christian Zommerfelds - 2009
