/*
 * EditorApp.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#include "EditorApp.h"

#include "EditorFrame.h"
#include "Editor.h"

#include "common/GameEvents.h"
#include "common/Physics.h"
#include "common/Renderer.h"

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

    m_events.reset(new GameEvents);
    m_physics.reset(new PhysicsSubSystem(*m_events)); // need physics system?
    m_renderer.reset(new RenderSubSystem(*m_events));

    Editor* editor = new Editor(*m_events);

    EditorFrame* frame = new EditorFrame(*editor, *m_renderer);
    frame->Show(true);

    editor->loadLevel("data/Levels/level_editor.lvl");

    return true;
}

void EditorApp::CleanUp()
{
    m_renderer->deInit();
}

