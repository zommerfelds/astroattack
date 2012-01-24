/*
 * EditorApp.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "EditorApp.h"

#include "EditorFrame.h"
#include "Editor.h"

#include "common/GameEvents.h"
#include "common/Physics.h"

#include <boost/property_tree/info_parser.hpp>

IMPLEMENT_APP(EditorApp)

namespace {
const std::string cConfigFileName = "data/editorConfig.info";
}

using boost::property_tree::ptree;

bool EditorApp::OnInit()
{
    ptree editorConfig;
    try
    {
        boost::property_tree::info_parser::read_info(cConfigFileName, editorConfig);
    }
    catch ( boost::property_tree::info_parser::info_parser_error& e )
    {
        std::cerr << "Could not load editor configuration file. Loading default." << std::endl;
        editorConfig.clear();
        editorConfig.put("LogConsoleLevel", "Debug");
        editorConfig.put("LogFileLevel", "Debug");
        editorConfig.put("LogFileName", "AAEditor.log");
    }
    setUpLoggerFromPropTree(editorConfig);

    GameEvents* events = new GameEvents;
    new PhysicsSubSystem(*events); // need physics system?

    Editor* editor = new Editor(*events);

    EditorFrame* frame = new EditorFrame(*editor, *events);
    frame->Show(true);

    //editor->loadLevel("data/Levels/level_editor.lvl");

    return true;
}
