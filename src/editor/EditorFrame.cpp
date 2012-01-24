/*
 * EditorFrame.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "EditorFrame.h"
#include "Editor.h"
#include "common/GameEvents.h"

// TEMP
#include <iostream>
using namespace std;

namespace {
int glArgs[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};
}

EditorFrame::EditorFrame(Editor& editor, GameEvents& events)
: EditorFrameBase(NULL),
  m_editor (editor),
  m_canvas (editor, m_glpanel, glArgs, events),
  m_timer (m_canvas)
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

void EditorFrame::OnMenuOpen(wxCommandEvent&)
{
    wxFileDialog openFileDialog(this, _("Open level file"), wxT("data/Levels"), _(""),
            wxT("AstroAttack level files (*.lvl)|*.lvl"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);

    // show the dialog
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return; // the user changed idea...

    cout << "user wants to open '" << (const char*)openFileDialog.GetPath().mb_str(wxConvUTF8) << "'" << endl;

    m_editor.loadLevel((const char*)openFileDialog.GetPath().mb_str(wxConvUTF8));
}

void EditorFrame::OnMenuExit(wxCommandEvent&)
{
    Close();
}

BEGIN_EVENT_TABLE(EditorFrame, wxFrame)
    EVT_CLOSE   (EditorFrame::OnClose)
    EVT_MENU    (wxID_OPEN, EditorFrame::OnMenuOpen)
    EVT_MENU    (wxID_EXIT, EditorFrame::OnMenuExit)
END_EVENT_TABLE()
