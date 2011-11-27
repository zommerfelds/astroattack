/*
 * EditorFrame.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "EditorFrame.h"
#include "common/GameEvents.h"

namespace {
int glArgs[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};
}

EditorFrame::EditorFrame(Editor& editor, GameEvents& events)
: EditorFrameBase(NULL),
  m_canvas(editor, m_glpanel, glArgs, events),
  m_timer(m_canvas)
{
	wxBoxSizer* sizer;
	sizer = new wxBoxSizer( wxVERTICAL );
	m_glpanel->SetSizer( sizer );

	sizer->Add(&m_canvas, 1, wxEXPAND);

	//timer->Start(100);
}

EditorFrame::~EditorFrame()
{
}

void EditorFrame::OnClose(wxCloseEvent& evt)
{
    m_timer.Stop();
    evt.Skip();
}

BEGIN_EVENT_TABLE(EditorFrame, wxFrame)
	EVT_CLOSE(EditorFrame::OnClose)
END_EVENT_TABLE()
