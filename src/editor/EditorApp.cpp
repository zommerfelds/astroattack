/*
 * EditorApp.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include <boost/property_tree/info_parser.hpp>

#include "EditorApp.h"
#include "EditorFrame.h"
#include "Editor.h"

#include "common/GameEvents.h"
#include "common/World.h"
#include "common/DataLoader.h"
#include "common/Physics.h"

IMPLEMENT_APP(EditorApp)

namespace {
const std::string cConfigFileName = "data/editorConfig.info";
}

using boost::property_tree::ptree;

bool EditorApp::OnInit()
{
	ptree editorConfig;
    boost::property_tree::info_parser::read_info(cConfigFileName, editorConfig); // TODO: handle fail
    setUpLoggerFromPropTree(editorConfig);

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
