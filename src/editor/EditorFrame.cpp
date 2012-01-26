/*
 * EditorFrame.cpp
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
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

BEGIN_EVENT_TABLE(EditorFrame, wxFrame)
    EVT_CLOSE   (EditorFrame::onClose)
    EVT_MENU    (wxID_NEW, EditorFrame::onMenuNew)
    EVT_MENU    (wxID_OPEN, EditorFrame::onMenuOpen)
    EVT_MENU    (wxID_SAVE, EditorFrame::onMenuSave)
    EVT_MENU    (wxID_SAVEAS, EditorFrame::onMenuSaveAs)
    EVT_MENU    (wxID_EXIT, EditorFrame::onMenuExit)
    EVT_MENU    (wxID_ABOUT, EditorFrame::onMenuAbout)
END_EVENT_TABLE()

EditorFrame::EditorFrame(Editor& editor, RenderSubSystem& renderer)
: EditorFrameBase (NULL),
  m_editor (editor),
  m_canvas (editor, m_glpanel, glArgs, renderer),
  m_timer (m_canvas)
{
    wxBoxSizer* sizer;
    sizer = new wxBoxSizer(wxVERTICAL);
    m_glpanel->SetSizer( sizer );

    sizer->Add(&m_canvas, 1, wxEXPAND);

    //m_bitmap1->SetBitmap()

    //timer->Start(100);
}

EditorFrame::~EditorFrame()
{
}

void EditorFrame::onClose(wxCloseEvent& evt)
{
    m_timer.Stop();
    evt.Skip();
}

void EditorFrame::onMenuNew(wxCommandEvent&)
{
    m_editor.clearLevel();
    m_fileName = "";
    Refresh();
}

void EditorFrame::onMenuOpen(wxCommandEvent&)
{
    wxFileDialog openFileDialog(this, _("Open level file"), wxT("data/Levels"), _(""),
            wxT("AstroAttack level files (*.lvl)|*.lvl"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);

    // show the dialog
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return; // the user changed idea...

    m_fileName = (const char*)openFileDialog.GetPath().mb_str(wxConvUTF8);
    m_editor.loadLevel(m_fileName);

    Refresh();
}

void EditorFrame::onMenuSave(wxCommandEvent& evt)
{
    if (m_fileName.empty())
        onMenuSaveAs(evt);
    else
        m_editor.saveLevel(m_fileName);
}

void EditorFrame::onMenuSaveAs(wxCommandEvent&)
{
    wxFileDialog saveFileDialog(this, _("Save level file"), wxT("data/Levels"), _(""),
            wxT("AstroAttack level files (*.lvl)|*.lvl"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

    // show the dialog
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
        return; // the user changed idea...

    m_fileName = (const char*)saveFileDialog.GetPath().mb_str(wxConvUTF8);
    m_editor.saveLevel(m_fileName);
}

void EditorFrame::onMenuExit(wxCommandEvent&)
{
    Close();
}

void EditorFrame::onMenuAbout(wxCommandEvent&)
{
    AboutDialog dialog(this);
    dialog.ShowModal();
}
