/*----------------------------------------------------------\
|                     CompPosition.h                        |
|                     --------------                        |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// Spiechert eine Position und Winkel eines Objekts
// WIRD GERADE NICHT VERWENDET, DA BOX2D DIESE FUNKTION ÜBERNIMMT

#ifndef COMPPOSITION_H
#define COMPPOSITION_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "../Component.h"
#include <boost/scoped_ptr.hpp>

class Vector2D;

//--------------------------------------------//
//----------- CompPosition Klasse ------------//
//--------------------------------------------//
class CompPosition : public Component
{
public:
    CompPosition();
    ~CompPosition();

    // Base component methods
	const CompIdType& FamilyID() const { return COMPONENT_ID; }
    const CompIdType& ComponentId() const { return COMPONENT_ID; }
	static const CompIdType COMPONENT_ID;

    // =========== Getters ==========
    const Vector2D& GetPosition() const; // get current drawing (smooth) position. If a CampPhysics exist, it gets the position from there.
    const Vector2D& GetPosIgnoreCompPhys() const; // this is not meant to be extensively used. this bypasses the check for CompPhysics position.
    float GetOrientation() const;

    // =========== Setters ===========
    void SetOrientation(float orientation);
    void SetPosition(const Vector2D& pos);

private:
    void OnUpdate();

    boost::scoped_ptr<Vector2D> m_pPosition;
    float m_orientation;
};
//--------------------------------------------//
//-------- Ende CompPosition Klasse ----------//
//--------------------------------------------//

#endif

// Astro Attack - Christian Zommerfelds - 2009
