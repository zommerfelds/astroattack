/*----------------------------------------------------------\
|                 CompVisualMessage.cpp                     |
|                 ---------------------                     |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "CompVisualMessage.h"

// eindeutige ID
const CompIdType CompVisualMessage::m_componentId = "CompVisualMessage";

// Konstruktor
CompVisualMessage::CompVisualMessage( std::string text )
: m_text ( text )
{
}

CompVisualMessage::~CompVisualMessage()
{
}

// Astro Attack - Christian Zommerfelds - 2009
