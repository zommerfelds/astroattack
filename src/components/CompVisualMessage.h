/*----------------------------------------------------------\
|                  CompVisualMessage.h                      |
|                  -------------------                      |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// Zeigt eine Nachricht auf dem Bildschirm

#ifndef COMPVISUALMESSAGE_H
#define COMPVISUALMESSAGE_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "../Component.h"
#include <string>

//--------------------------------------------//
//-------- CompVisualMessage Klasse ----------//
//--------------------------------------------//
class CompVisualMessage : public Component
{
public:
    CompVisualMessage( std::string text );
    ~CompVisualMessage();
    const CompIdType& ComponentId() const { return COMPONENT_ID; }
	static const CompIdType COMPONENT_ID; // eindeutige ID für diese Komponentenart (gleich wie Klassennamen, siehe CompCollectable.cpp)

    std::string GetMsg() { return m_text; }

private:
    std::string m_text;
};
//--------------------------------------------//
//------ Ende CompVisualMessage Klasse -------//
//--------------------------------------------//

#endif

// Astro Attack - Christian Zommerfelds - 2009
