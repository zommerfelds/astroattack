/*
 * EditorFrame.cpp
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#include "EditorFrame.h"
#include "Editor.h"
#include "common/Component.h"
#include "common/GameEvents.h"
#include "common/Foreach.h"

#include <vector>

namespace {
int glArgs[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};
}

BEGIN_EVENT_TABLE(EditorFrame, wxFrame)
    EVT_CLOSE   (EditorFrame::onClose)
    EVT_MENU    (wxID_NEW,    EditorFrame::onMenuNew)
    EVT_MENU    (wxID_OPEN,   EditorFrame::onMenuOpen)
    EVT_MENU    (wxID_SAVE,   EditorFrame::onMenuSave)
    EVT_MENU    (wxID_SAVEAS, EditorFrame::onMenuSaveAs)
    EVT_MENU    (wxID_EXIT,   EditorFrame::onMenuExit)
    EVT_MENU    (wxID_ABOUT,  EditorFrame::onMenuAbout)
    EVT_LIST_ITEM_ACTIVATED  (wxID_ANY, EditorFrame::onComponentActivated)
END_EVENT_TABLE()

EditorFrame::EditorFrame(Editor& editor, RenderSystem& renderer)
: EditorFrameBase (NULL),
  m_editor (editor),
  m_canvas (editor, m_glpanel, *this, glArgs, renderer),
  m_timer (m_canvas)
{
    wxBoxSizer* sizer;
    sizer = new wxBoxSizer(wxVERTICAL);
    m_glpanel->SetSizer( sizer );

    sizer->Add(&m_canvas, 1, wxEXPAND);

    // Add first column to component list
    wxListItem col0;
    col0.SetId(0);
    col0.SetText(_("Type"));
    col0.SetWidth(100);
    m_compList->InsertColumn(0, col0);

    // Add second column
    wxListItem col1;
    col1.SetId(1);
    col1.SetText(_("ID"));
    col1.SetWidth(70);
    m_compList->InsertColumn(1, col1);

    std::vector<wxAcceleratorEntry> entries;
    entries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_RETURN,   ACC_ID_ENTER));
    entries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_ESCAPE,   ACC_ID_ESCAPE));
    entries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_BACK,     ACC_ID_BACKSPACE));
    entries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_PAGEUP,   ACC_ID_NEXTTEXTURE));
    entries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_PAGEDOWN, ACC_ID_PREVTEXTURE));
    wxAcceleratorTable accel(entries.size(), &entries[0]);
    SetAcceleratorTable(accel);

    // add handler for all accelerators
    for (int i = ACC_ID_lowest; i != ACC_ID_highest; i++)
    {
        Connect(i, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(EditorFrame::onCustomAccelerator));
    }

    //timer->Start(100);
}

EditorFrame::~EditorFrame()
{
}

void EditorFrame::update()
{
    wxString label = _("Entity: ");
    m_compList->DeleteAllItems();

    if (m_editor.getGuiData().selectedEntity)
    {
        EntityId id = m_editor.getGuiData().selectedEntity->first;
        std::vector<Component*> components = m_editor.getGuiData().selectedEntity->second;

        label.Append(wxString(id.c_str(), wxConvUTF8));

        int index = 0;
        foreach(Component* comp, components)
        {
            wxString compTypeId (comp->getTypeId().substr(4).c_str(), wxConvUTF8);
            wxString compId (comp->getId().c_str(), wxConvUTF8);

            m_compList->InsertItem(index, compTypeId); // col. 1
            m_compList->SetItem(index, 1, compId); // col. 2
            index++;
        }
    }

    m_entityLabel->SetLabel(label);
}

void EditorFrame::onComponentActivated(wxListEvent&)
{
    std::cerr << "activated!" << std::endl;
}

void EditorFrame::onCustomAccelerator(wxCommandEvent& evt)
{
    bool skipEvent = false;

    if (evt.GetId() == ACC_ID_ENTER && m_editor.getGuiData().indexCurVertex != 0)
        m_editor.cmdCreateBlock();
    else if (evt.GetId() == ACC_ID_ESCAPE)
        m_editor.cmdCancelBlock();
    else if (evt.GetId() == ACC_ID_BACKSPACE)
        m_editor.cmdCancelVertex();
    else if (evt.GetId() == ACC_ID_NEXTTEXTURE)
        m_editor.cmdNextTexture();
    else if (evt.GetId() == ACC_ID_PREVTEXTURE)
        m_editor.cmdPrevTexture();
    else
        skipEvent = true;

    if (skipEvent)
        evt.Skip();

    Refresh();
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
    m_canvas.resetCamera();

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
    m_canvas.resetCamera();

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
