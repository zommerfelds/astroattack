/*----------------------------------------------------------\
|                   CompCollectable.h                       |
|                   -----------------                       |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// Komponente für Sammelbare Objekte

#ifndef COMPCOLLECTABLE_H
#define COMPCOLLECTABLE_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "../Component.h"
#include "../EventManager.h"
#include <string>

//--------------------------------------------//
//--------- CompCollectable Klasse -----------//
//--------------------------------------------//
class CompCollectable : public Component
{
public:

    CompCollectable( std::map<const std::string, int>::iterator itCollectableVariable );
    ~CompCollectable();
    const CompIdType& ComponentId() const { return m_componentId; }

private:

	static CompIdType m_componentId; // eindeutige ID für diese Komponentenart (gleich wie Klassennamen, siehe CompCollectable.cpp)

    void Collision( const Event* contactEvent );

    RegisterObj m_registerObj;
    bool m_wasCollected;
    std::map<const std::string, int>::iterator m_itCollectableVariable;
};
//--------------------------------------------//
//------- Ende CompCollectable Klasse --------//
//--------------------------------------------//

#endif

// Astro Attack - Christian Zommerfelds - 2009
