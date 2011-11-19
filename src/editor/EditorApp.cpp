/*
 * EditorApp.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "EditorApp.h"
#include "EditorFrame.h"
#include "Editor.h"

#include "common/GameEvents.h"
#include "common/World.h"
#include "common/DataLoader.h"
#include "common/Physics.h"

IMPLEMENT_APP(EditorApp)

bool EditorApp::OnInit()
{
	GameEvents* events = new GameEvents;
	World* world = new World(*events);
	new PhysicsSubSystem(*events); // need physics system?

	Editor* editor = new Editor(*world);

	EditorFrame* frame = new EditorFrame(*editor, *events);
    frame->Show(true);

	// TODO catch exception
    DataLoader::loadWorld( "data/player.info", *world, *events );
    DataLoader::loadWorld( "data/Levels/level_editor.info", *world, *events );

    // TODO: need this?
    world->setVariable( "Collected", 0 );
    world->setVariable( "JetpackEnergy", 1000 );
    world->setVariable( "Health", 1000 );

    return true;
}
