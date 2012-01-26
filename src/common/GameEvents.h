/*
 * GameEvents.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

// Für die Kommunikation zwischen Komponente und Systeme

#ifndef GAMEEVENTS_H
#define GAMEEVENTS_H

#include "common/Event.h"
#include <string>

typedef std::string EntityIdType;

class Component;

struct GameEvents {
    // -- quitGame --
    // Das ganze Spiel wird beendet
    // Arg: restart?
    Event1<bool> quitGame;

    // -- newEntity --
    // A new entity is being created
    // Arg: entity being created
    Event1<const EntityIdType&> newEntity;

    Event1<Component&> newComponent;

    // -- watToDeleteEntity --
    // Somebody wants to delete an entity
    // Arg: entity that should be deleted
    Event1<const EntityIdType&> wantToDeleteEntity;

    // -- deleteEntity --
    // An entity is being deleted
    // Arg: entity being deleted
    Event1<const EntityIdType&> deleteEntity;

    Event1<Component&> deleteComponent;

    // -- gameUpdate --
    // The next internal game iteration is being run (not frames)
    Event0 gameUpdate;

    // -- levelEnd --
    // The player finished the current level
    // Args: has the player won?, level end message
    Event2<bool, const std::string&> levelEnd;
};

#endif
