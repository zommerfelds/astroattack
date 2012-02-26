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
		std::cerr << "Could not load editor configuration file. " << e.what() << std::endl
			      << "Loading default configuration." << std::endl;
        editorConfig.clear();
        editorConfig.put("LogConsoleLevel", "Debug");
        editorConfig.put("LogFileLevel", "Debug");
        editorConfig.put("LogFileName", "AAEditor.log");
    }
    setUpLoggerFromPropTree(editorConfig);

    m_events.reset(new GameEvents);
    m_physics.reset(new PhysicsSystem(*m_events));
    m_renderer.reset(new RenderSystem(*m_events));

    m_editor.reset(new Editor(*m_events, *m_physics));

    m_frame = new EditorFrame(*m_editor, *m_renderer); // will be deleted by wxWidgets
    m_frame->Layout();
    m_frame->Show(true);

    m_editor->loadLevel("data/Levels/level_editor.lvl");

    return true;
}

int EditorApp::OnExit()
{
    m_renderer->deInit();
    return 0;
}

int EditorApp::FilterEvent(wxEvent& event)
{
    if (event.GetEventType() == wxEVT_KEY_DOWN)
        return m_frame->keyEvent(static_cast<wxKeyEvent&>(event));

    return -1;
}

