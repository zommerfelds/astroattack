/*----------------------------------------------------------\
|                      CompVisual.h                         |
|                      ------------                         |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2008                             |
\----------------------------------------------------------*/

#ifndef COMPVISUAL_H
#define COMPVISUAL_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "../Component.h"

class CompVisual : public Component
{
public:
	const CompIdType& FamilyID() const { return m_familyID; }
private:
	static CompIdType m_familyID;
};

#endif

// Astro Attack - Christian Zommerfelds - 2008
