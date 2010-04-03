/*----------------------------------------------------------\
|                    CompPosition.cpp                       |
|                    ----------------                       |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// CompPosition.h für mehr Informationen

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "CompPosition.h"
#include "../Vector2D.h"

// eindeutige ID
CompIdType CompPosition::m_componentId = "CompPosition";

CompPosition::CompPosition() : m_pPosition ( new Vector2D ), m_orientation ( 0.0f )
{
}

CompPosition::~CompPosition()
{
}

// Astro Attack - Christian Zommerfelds - 2009
