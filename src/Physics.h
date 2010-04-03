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
extern const float PHYS_DELTA_TIME;

class Event;
class CompPhysics;
class CompGravField;
class Vector2D;
class RegisterObj;
class EventManager;
class b2World;
class ContactListener;
class b2Shape;

/*
    Bei Update() wird die ganze Physik simuliert.
*/
class PhysicsSubSystem
{
public:
    PhysicsSubSystem( EventManager* pEventManager);
    ~PhysicsSubSystem();

    void Init();
    void Update();

    void Refilter( b2Shape* pShape ); // Recalculate collision filtering of pShape

private:
    boost::scoped_ptr<Vector2D> m_pGravitationalAcc; // Fallbeschleunigung [m/s^2]

    std::vector< CompPhysics* > m_physicsComps;
	std::vector< CompGravField* > m_gravFields;

    boost::scoped_ptr<RegisterObj> m_registerObj;
    boost::scoped_ptr<RegisterObj> m_registerObj2;
	boost::scoped_ptr<RegisterObj> m_registerObj3;
    boost::scoped_ptr<RegisterObj> m_registerObj4;
    EventManager* m_pEventManager;

    void RegisterPhysicsComp( const Event* pEvent );
    void UnregisterPhysicsComp( const Event* pEvent );
	void RegisterGravFieldComp( const Event* pEvent );
    void UnregisterGravFieldComp( const Event* pEvent );

    boost::scoped_ptr<b2World> m_world;
    float m_timeStep;
    int m_velocityIterations;
    int m_positionIterations;

    class ContactListener : public b2ContactListener
    {
    public:
        ContactListener( EventManager* pEM ) : m_pEventManager(pEM) {}
	    /*void Add(const b2ContactPoint* point);
        void Persist(const b2ContactPoint* point);
	    void Remove(const b2ContactPoint* point);*/
    private:
        /*void CheckContactPoint(const b2ContactPoint* point);*/
        EventManager* m_pEventManager;
    };

    boost::scoped_ptr<ContactListener> m_contactListener;
};

#endif

// Astro Attack - Christian Zommerfelds - 2009
