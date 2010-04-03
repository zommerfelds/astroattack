/*----------------------------------------------------------\
|                      Component.cpp                        |
|                      -------------                        |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "Component.h"
#include "EventManager.h" // Steuert die Spielerreignisse

EventManager* Component::eventManager = NULL;

// Astro Attack - Christian Zommerfelds - 2009
