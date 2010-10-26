/*----------------------------------------------------------\
|                       GameEvents.h                        |
|                       ------------                        |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2010                             |
\----------------------------------------------------------*/
// Für die Kommunikation zwischen Komponente und Systeme

#ifndef GAMEEVENTS_H
#define GAMEEVENTS_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "Event.h"
#include <string>

class Entity;

struct GameEvents {
    // -- quitGame --
    // Das ganze Spiel wird beendet
    Event0 quitGame;

    // -- newEntity --
    // A new entity is being created
    // Args: pointer to the entity being created
    Event1<Entity*> newEntity;

    // -- watToDeleteEntity --
    // Somebody wants to delete an entity
    // Args: pointer to the entity that should be deleted
    Event1<Entity*> wantToDeleteEntity;

    // -- deleteEntity --
    // An entity is being deleted
    // Args: pointer to the entity being deleted
    Event1<Entity*> deleteEntity;

    // -- gameUpdate --
    // The next internal game iteration is being run (not frames)
    Event0 gameUpdate;

    // -- levelEnd --
    // The player finished the current level
    // Args: bool: has the player won?, string: level end message       // Level wird beended
    Event2<bool,std::string> levelEnd;
};

#endif

// Astro Attack - Christian Zommerfelds - 2010
