/*----------------------------------------------------------\
|                    CompGravField.h                        |
|                    ---------------                        |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// Komponente für Gravitationsfelder

#ifndef COMPGRAVFIELD_H
#define COMPGRAVFIELD_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "../Component.h"
#include "../GameEvents.h"
#include <set>
#include <boost/scoped_ptr.hpp>

class b2Body;
class Vector2D;

enum GravType { Directional, Radial };

//--------------------------------------------//
//--------- CompGravField Klasse -------------//
//--------------------------------------------//
class CompGravField : public Component
{
public:

    CompGravField();
    ~CompGravField();
    const CompIdType& ComponentId() const { return m_componentId; }

    // Setters
	void SetGravType( GravType t ) { m_gravType = t; }
	void SetGravDir( const Vector2D& dir );
	void SetGravCenter( const Vector2D& center, float strenght );
    void SetPriority( unsigned int priority );

    // Getters
	GravType GetGravType() const { return m_gravType; }
	const Vector2D& GetGravDir() const { return *m_pGravitationDir; }
	const Vector2D& GetGravCenter() const { return *m_pGravitationCenter; }
	float GetStrenght() const { return m_strenght; }
	unsigned int GetPriority() const { return m_priority; }

	// Get the acceleration of a body with center of mass "centerOfMass"
	Vector2D GetAcceleration(const Vector2D& centerOfMass) const;
	//void ApplyForces() const;

	//std::set<b2Body*>& GetBodiesInField() { return m_bodiesInField; }
	//void ClearBodiesInField();

private:
	//RegisterObj m_registerObj1;
	//RegisterObj m_registerObj2;

	//void Contact( const Event* contactEvent );
	//void DeleteContact( const Event* contactEvent );
    //void OnGameUpdate( const Event* contactEvent );

	// TODO: Save CompPhysics instead of b2Body
	//std::set<b2Body*> m_bodiesInField;

	GravType m_gravType;
	boost::scoped_ptr<Vector2D> m_pGravitationDir;
	boost::scoped_ptr<Vector2D> m_pGravitationCenter;
	float m_strenght; // only with Radial
    unsigned int m_priority; // Gravitationsfeld-priorität: wenn sich 2 felder überlappen, gilt der mit der grössten priorität (0-100 is gültig)

	static const CompIdType m_componentId; // eindeutige ID für diese Komponentenart (gleich wie Klassennamen, siehe CompGravField.cpp)

    //friend class PhysicsSubSystem; // Das Physik-System darf auf alles hier zugreifen!
};
//--------------------------------------------//
//------- Ende CompGravField Klasse --------//
//--------------------------------------------//

#endif

// Astro Attack - Christian Zommerfelds - 2009
