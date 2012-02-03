/*
 * GameEvents.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

// Für die Kommunikation zwischen Komponente und Systeme

#ifndef GAMEEVENTS_H
#define GAMEEVENTS_H

#include "common/Event.h"
#include "IdTypes.h"
#include <string>
#include <boost/function.hpp>

class Component;
class CompShape;

struct GameEvents {

    // A new entity is being created
    // Arg: entity being created
    Event1<const EntityId&> newEntity;

    // Somebody wants to delete an entity
    // Arg: entity that should be deleted
    Event1<const EntityId&> wantToDeleteEntity;

    // An entity is being deleted
    // Arg: entity being deleted
    Event1<const EntityId&> deleteEntity;

    // Is called for each component when an entity is created
    Event1<Component&> newComponent;

    // Is called for each component when an entity is deleted
    Event1<Component&> deleteComponent;

    // The player finished the current level
    // Args: has the player won?, level end message
    Event2<bool, const std::string&> levelEnd;

    // Quit the whole program
    // Arg: should the game restart?
    Event1<bool> quitGame;

    // Two shapes have touched each other
    // Note: multiple events can be created per touch
    Event2<CompShape&, CompShape&> newContact;

    // When a new message will be displayed
    // Args: message, amount of time (-1 for standard)
    Event2<std::string&, int> dispMessage;

    // A CompVariable has been updated
    // Note that the value did possibly not change
    // Args: entity ID, component ID (variable name), updated value
    Event3<const EntityId&, const ComponentId&, int> variableUpdate;

    // Request to make a variable modification
    // Args: entity ID, component ID (variable name), function to modify value
    // Note: can be used to request a variableUpdate event using an identity function
    Event3<const EntityId&, const ComponentId&, const boost::function<int (int)>& > variableModification;
};

#endif
