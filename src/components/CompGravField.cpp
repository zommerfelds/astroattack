/*----------------------------------------------------------\
|                   CompGravField.cpp                       |
|                   -----------------                       |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "CompGravField.h"
#include "../Vector2D.h"

// eindeutige ID
const CompIdType CompGravField::COMPONENT_ID = "CompGravField";

// Konstruktor
CompGravField::CompGravField() : m_gravType ( Directional ), m_pGravitationDir ( new Vector2D ),
m_pGravitationCenter ( new Vector2D ), m_priority( 50 )
{
    m_pGravitationDir->Set( 0.0, 1.0 );
    m_pGravitationCenter->Set( 0.0, 1.0 );
}

void CompGravField::SetGravDir(const Vector2D& dir)
{
    *m_pGravitationDir = dir;
}

void CompGravField::SetGravCenter(const Vector2D& center, float strenght)
{
    *m_pGravitationCenter = center;
    m_strenght = strenght;
}

void CompGravField::SetPriority( unsigned int priority )
{
    if ( priority > 1000 )
        return; // ACHTUNG: sende warnung
    m_priority = priority;
}

// Get the acceleration of a body with center of mass "centerOfMass"
Vector2D CompGravField::GetAcceleration(const Vector2D& centerOfMass) const
{
    if ( m_gravType == Directional ) 
    {
        return GetGravDir();
    }
    else if ( m_gravType == Radial )
    {
        Vector2D acc( GetGravCenter()-centerOfMass );
        acc.Normalise();
        acc *= m_strenght;
        return acc;
    }
    return Vector2D( 0.0f, 0.0f );
}

// Astro Attack - Christian Zommerfelds - 2009
