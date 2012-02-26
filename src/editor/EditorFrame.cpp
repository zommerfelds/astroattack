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
#include <boost/property_tree/ptree.hpp>

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
    //EVT_LIST_ITEM_ACTIVATED  (wxID_ANY, EditorFrame::onComponentActivated)
    EVT_LIST_ITEM_SELECTED   (wxID_ANY, EditorFrame::onListItemSelected)
    EVT_LIST_ITEM_DESELECTED (wxID_ANY, EditorFrame::onListItemDeselected)
    EVT_TEXT_ENTER (ID_ENTITYIDFIELD, EditorFrame::onEntityIdFieldEnter)
    EVT_TEXT_ENTER (ID_COMPIDFIELD, EditorFrame::onCompIdFieldEnter)
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
    col1.SetWidth(100);
    m_compList->InsertColumn(1, col1);

    // Add first column to property list
    col0.SetId(0);
    col0.SetText(_("Property"));
    col0.SetWidth(120);
    m_propList->InsertColumn(0, col0);

    // Add second column
    col1.SetId(1);
    col1.SetText(_("Value"));
    col1.SetWidth(100);
    m_propList->InsertColumn(1, col1);

    /*std::vector<wxAcceleratorEntry> entries;
    entries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_RETURN,   ACC_ID_ENTER));
    entries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_ESCAPE,   ACC_ID_ESCAPE));
    //entries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_BACK,     ACC_ID_BACKSPACE));
    entries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_PAGEUP,   ACC_ID_NEXTTEXTURE));
    entries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_PAGEDOWN, ACC_ID_PREVTEXTURE));
    wxAcceleratorTable accel(entries.size(), &entries[0]);
    SetAcceleratorTable(accel);

    // add handler for all accelerators
    for (int i = ACC_ID_lowest; i != ACC_ID_highest; i++)
    {
        Connect(i, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(EditorFrame::onCustomAccelerator));
    }*/

    //timer->Start(100);
}

EditorFrame::~EditorFrame()
{
}

void EditorFrame::update()
{
    if (m_editor.getGuiData().selectedEntity)
    {
        enableEntity(m_editor.getGuiData().selectedEntity->first, m_editor.getGuiData().selectedEntity->second);
    }
    else
    {
        disableEntity();
    }
}

void EditorFrame::onListItemSelected(wxListEvent& evt)
{
    if (evt.GetId() == ID_COMPLIST)
    {
        Component* comp = (Component*)(evt.GetData());
        enableComponent(comp);
    }
    else if (evt.GetId() == ID_PROPLIST)
    {
        enableProperty();
    }
}

void EditorFrame::onListItemDeselected(wxListEvent& evt)
{
    if (evt.GetId() == ID_COMPLIST)
    {
        disableComponent();
    }
    else if (evt.GetId() == ID_PROPLIST)
    {
        disableProperty();
    }
}

void EditorFrame::enableEntity(const EntityId& id, const std::vector<const Component*>& comps)
{
    disableEntity();

    *m_entityIdField << wxString(id.c_str(), wxConvUTF8);
    m_entityIdField->Enable();

    m_butNewComp->Enable();

    int index = 0;
    foreach(const Component* comp, comps)
    {
        wxString compTypeId (comp->getTypeId().substr(4).c_str(), wxConvUTF8);
        wxString compId (comp->getId().c_str(), wxConvUTF8);

        m_compList->InsertItem(index, compTypeId); // col. 1
        m_compList->SetItem(index, 1, compId); // col. 2
        m_compList->SetItemPtrData(index, (wxUIntPtr)comp);
        index++;
    }
}

void EditorFrame::disableEntity()
{
    m_entityIdField->Clear();
    m_entityIdField->Disable();

    m_compList->DeleteAllItems();

    m_butNewComp->Disable();
    disableComponent();
}

void EditorFrame::enableComponent(const Component* comp)
{
    disableComponent();

    m_butRemoveComp->Enable();

    m_compLabel->SetLabel(wxString(comp->getTypeId().c_str(), wxConvUTF8));
    m_compPanel->Layout();

    m_compIdField->Enable();
    m_compIdField->Clear();
    *m_compIdField << wxString(comp->getId().c_str(), wxConvUTF8);

    boost::property_tree::ptree propTree;
    comp->writeToPropertyTree(propTree);

    updatePropList(propTree, 0, "");

    m_butNewProp->Enable();
}

void EditorFrame::disableComponent()
{
    m_butRemoveComp->Disable();

    m_compLabel->SetLabel(wxT("Component"));
    m_compIdField->Clear();
    m_compIdField->Disable();

    m_propList->DeleteAllItems();

    m_butNewProp->Disable();
    disableProperty();
}

void EditorFrame::enableProperty()
{
    m_butRemoveProp->Enable();
}

void EditorFrame::disableProperty()
{
    m_butRemoveProp->Disable();
}

int EditorFrame::updatePropList(const boost::property_tree::ptree& propTree, int index, std::string prefix)
{
    for (boost::property_tree::ptree::const_assoc_iterator it = propTree.ordered_begin(); it != propTree.not_found(); ++it)
    {
        wxString name ((prefix + it->first).c_str(), wxConvUTF8);
        wxString value (it->second.data().c_str(), wxConvUTF8);

        if (it->second.empty())
        {
            m_propList->InsertItem(index, name); // col. 1
            m_propList->SetItem(index, 1, value); // col. 2
            index++;
        }
        else
            index = updatePropList(it->second, index, prefix+it->first+".");
    }
    return index;
}

void EditorFrame::onEntityIdFieldEnter(wxCommandEvent& evt)
{
    EntityId newId = (const char*)m_entityIdField->GetValue().mb_str(wxConvUTF8);
    if (m_editor.cmdSetNewEntityId(newId))
    {
        SetStatusText(_("Changed entity ID"));
    }
    else
    {
        SetStatusText(_("Entity ID already exists!"));
        m_entityIdField->Clear();
        *m_entityIdField << wxString(m_editor.getGuiData().selectedEntity->first.c_str(), wxConvUTF8);
    }
}


void EditorFrame::onCompIdFieldEnter(wxCommandEvent& evt)
{
    ComponentId newId = (const char*)m_compIdField->GetValue().mb_str(wxConvUTF8);
    long selected = m_compList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    Component* comp = (Component*)m_compList->GetItemData(selected);
    m_editor.cmdSetNewCompId(newId, comp);
    m_compList->SetItem(selected, 1, wxString(newId.c_str(), wxConvUTF8));
    SetStatusText(_("Changed component ID"));
}

/*void EditorFrame::onCustomAccelerator(wxCommandEvent& evt)
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
}*/

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

    disableEntity();

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

    disableEntity();

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

int EditorFrame::keyEvent(wxKeyEvent& evt)
{
    if (evt.GetId() == ID_COMPIDFIELD || evt.GetId() == ID_ENTITYIDFIELD)
        return -1;

    if (evt.GetKeyCode() == WXK_RETURN && m_editor.getGuiData().indexCurVertex != 0)
        m_editor.cmdCreateBlock();
    else if (evt.GetKeyCode() == WXK_ESCAPE)
        m_editor.cmdCancelBlock();
    else if (evt.GetKeyCode() == WXK_BACK)
        m_editor.cmdCancelVertex();
    else if (evt.GetKeyCode() == WXK_PAGEUP)
        m_editor.cmdNextTexture();
    else if (evt.GetKeyCode() == WXK_PAGEDOWN)
        m_editor.cmdPrevTexture();
    else
        return -1;

    Refresh();

    return true;
}
