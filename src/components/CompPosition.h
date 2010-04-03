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
	const CompIdType& FamilyID() const { return m_componentId; }
    const CompIdType& ComponentId() const { return m_componentId; }

private:
	static CompIdType m_componentId;

public:
    // =========== Zugriff ==========
    Vector2D* Position() const
    {
        return m_pPosition.get();    // Position
    }

    float Orientation() const
    {
        return m_orientation;    // Orientierung
    }

    // =========== Festlegen ===========
    void SetOrientation ( float orientation )
    {
        m_orientation = orientation;
    }

private:
    boost::scoped_ptr<Vector2D> m_pPosition;
    float m_orientation;
};
//--------------------------------------------//
//-------- Ende CompPosition Klasse ----------//
//--------------------------------------------//

#endif

// Astro Attack - Christian Zommerfelds - 2009
