/*
 * GameEvents.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Für die Kommunikation zwischen Komponente und Systeme

#ifndef GAMEEVENTS_H
#define GAMEEVENTS_H

#include <string>

#include "Event.h"

typedef std::string EntityIdType;

class Component;

struct GameEvents {
    // -- quitGame --
    // Das ganze Spiel wird beendet
    Event0 quitGame;

    // -- newEntity --
    // A new entity is being created
    // Args: pointer to the entity being created
    Event1<const EntityIdType&> newEntity;

    Event1<Component&> newComponent;

    // -- watToDeleteEntity --
    // Somebody wants to delete an entity
    // Args: pointer to the entity that should be deleted
    Event1<const EntityIdType&> wantToDeleteEntity;

    // -- deleteEntity --
    // An entity is being deleted
    // Args: pointer to the entity being deleted
    Event1<const EntityIdType&> deleteEntity;

    Event1<Component&> deleteComponent;

    // -- gameUpdate --
    // The next internal game iteration is being run (not frames)
    Event0 gameUpdate;

    // -- levelEnd --
    // The player finished the current level
    // Args: bool: has the player won?, string: level end message       // Level wird beended
    Event2<bool, const std::string&> levelEnd;
};

#endif
