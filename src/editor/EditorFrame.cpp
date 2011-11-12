/*
 * EditorFrame.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "EditorFrame.h"

#include <wx/timer.h>

#include "GLCanvas.h"

#include "common/GameEvents.h"
#include "common/World.h"
#include "common/DataLoader.h"

class UpdateTimer : public wxTimer
{
    GLCanvas* pane;
public:
    UpdateTimer(GLCanvas* pane) {
		this->pane = pane;
	}

    void Notify()
	{
		pane->Refresh();
	}
};

EditorFrame::EditorFrame() : EditorFrameBase(NULL)
{
	wxBoxSizer* sizer;
	sizer = new wxBoxSizer( wxVERTICAL );
	m_glpanel->SetSizer( sizer );

	int glArgs[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};

	canvas = new GLCanvas(m_glpanel, glArgs);

	sizer->Add(canvas, 1, wxEXPAND);

	timer = new UpdateTimer(canvas);
	//timer->Start(100);

	GameEvents events;
	World world(events);

	// TODO catch exception
    DataLoader::loadWorld( "data/player.info", world, events );
    DataLoader::loadWorld( "data/Levels/level_editor.info", world, events );

    // TODO: need this?
    m_gameWorld.setVariable( "Collected", 0 );
    m_gameWorld.setVariable( "JetpackEnergy", 1000 );
    m_gameWorld.setVariable( "Health", 1000 );
}

EditorFrame::~EditorFrame()
{
	delete timer;
}

void EditorFrame::OnClose(wxCloseEvent& evt)
{
    timer->Stop();
    evt.Skip();
}

BEGIN_EVENT_TABLE(EditorFrame, wxFrame)
	EVT_CLOSE(EditorFrame::OnClose)
END_EVENT_TABLE()
