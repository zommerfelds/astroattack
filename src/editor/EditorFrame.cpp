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
#include <boost/make_shared.hpp>
#include <boost/property_tree/ptree.hpp>

using boost::property_tree::ptree;

namespace {
int glArgs[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};
}

class PropTreeElem
{
public:
    PropTreeElem(ptree& parent, const ptree::assoc_iterator& it) : parent (parent), it (it) {}
    ptree& parent;
    ptree::assoc_iterator it; // it->first is node name, it->second is ptree
};

class NewPropDialog : public NewPropDialogBase
{
public:
    NewPropDialog(wxWindow* parent) : NewPropDialogBase(parent) {}

    std::string getKey()   { return (const char*)m_keyText->GetValue().mb_str(wxConvUTF8); }
    std::string getValue() { return (const char*)m_valueText->GetValue().mb_str(wxConvUTF8); }
};

class EditPropDialog : public EditPropDialogBase
{
public:
    EditPropDialog(wxWindow* parent, const std::string& keyName, const std::string& curValue) : EditPropDialogBase(parent)
    {
        m_keyLabel->SetLabel(wxString(keyName.c_str(), wxConvUTF8));
        *m_valueText << wxString(curValue.c_str(), wxConvUTF8);
    }

    std::string getValue() { return (const char*)m_valueText->GetValue().mb_str(wxConvUTF8); }
};

BEGIN_EVENT_TABLE(EditorFrame, wxFrame)
    EVT_CLOSE   (EditorFrame::onClose)
    EVT_MENU    (wxID_NEW,    EditorFrame::onMenuNew)
    EVT_MENU    (wxID_OPEN,   EditorFrame::onMenuOpen)
    EVT_MENU    (wxID_SAVE,   EditorFrame::onMenuSave)
    EVT_MENU    (wxID_SAVEAS, EditorFrame::onMenuSaveAs)
    EVT_MENU    (wxID_EXIT,   EditorFrame::onMenuExit)
    EVT_MENU    (wxID_ABOUT,  EditorFrame::onMenuAbout)
    EVT_BUTTON  (ID_BUTEDITPROP, EditorFrame::onPropEditBut)
    EVT_BUTTON  (ID_BUTNEWPROP, EditorFrame::onPropNew)
    EVT_LIST_ITEM_ACTIVATED  (ID_PROPLIST, EditorFrame::onPropEditList)
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

    m_sidePanel->SetSize(200, -1);

    m_propWarningPanel->Hide();

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
        enableComponent((Component*)(evt.GetData()));
    }
    else if (evt.GetId() == ID_PROPLIST)
    {
        enableProperty();
    }

    m_propWarningPanel->Show( !m_propWarningPanel->IsShown() );
    Layout();
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

void EditorFrame::enableComponent(Component* comp)
{
    disableComponent();
    m_selectedComp = comp;

    m_butRemoveComp->Enable();

    m_compLabel->SetLabel(wxString(comp->getTypeId().c_str(), wxConvUTF8));
    m_compPanel->Layout();

    m_compIdField->Enable();
    m_compIdField->Clear();
    *m_compIdField << wxString(comp->getId().c_str(), wxConvUTF8);

    m_propTree = boost::make_shared<ptree>();
    comp->writeToPropertyTree(*m_propTree);

    m_propTreeElems.clear();
    updatePropList(*m_propTree, 0, "");

    m_butNewProp->Enable();
}

void EditorFrame::disableComponent()
{
    m_selectedComp = NULL;

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
    m_butEditProp->Enable();
}

void EditorFrame::disableProperty()
{
    m_butRemoveProp->Disable();
    m_butEditProp->Disable();
}

int EditorFrame::updatePropList(ptree& propTree, int index, std::string prefix)
{
    for (ptree::assoc_iterator it = propTree.ordered_begin(); it != propTree.not_found(); ++it)
    {
        std::string name = prefix + it->first;
        std::string value = it->second.data();
        ptree& childTree = it->second;
        wxString nameWx (name.c_str(), wxConvUTF8);
        wxString valueWx (value.c_str(), wxConvUTF8);

        if (it->second.empty())
        {
            m_propList->InsertItem(index, nameWx); // col. 1
            m_propList->SetItem(index, 1, valueWx); // col. 2
            m_propTreeElems.push_back(boost::make_shared<PropTreeElem>(boost::ref(propTree), it));
            m_propList->SetItemPtrData(index, (wxUIntPtr)m_propTreeElems[m_propTreeElems.size()-1].get());
            index++;
        }
        else
            index = updatePropList(childTree, index, prefix+it->first+".");
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
    long selected = m_compList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED); // NOTE: could use m_selectedComp
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

int EditorFrame::showDialog(wxDialog& diag)
{
    m_canvas.showCursor(true);
    int result = diag.ShowModal();
    m_canvas.showCursor(false);
    return result;
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

    disableEntity();

    Refresh();
}

void EditorFrame::onMenuOpen(wxCommandEvent&)
{
    wxFileDialog openFileDialog(this, _("Open level file"), wxT("data/Levels"), _(""),
            wxT("AstroAttack level files (*.lvl)|*.lvl"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);

    // show the dialog
    if (showDialog(openFileDialog) == wxID_CANCEL)
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
    if (showDialog(saveFileDialog) == wxID_CANCEL)
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
    showDialog(dialog);
}

void EditorFrame::onPropEditBut(wxCommandEvent&)
{
    onPropEdit();
}

void EditorFrame::onPropEditList(wxListEvent&)
{
    onPropEdit();
}

void EditorFrame::onPropEdit()
{
    long selected = m_propList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    PropTreeElem& pt = *(PropTreeElem*)m_propList->GetItemData(selected);

    std::string id = pt.it->first;

    EditPropDialog dialog(this, id, pt.it->second.data());
    if (showDialog(dialog) == wxID_CANCEL)
        return;
    Refresh();

    pt.parent.erase(pt.parent.to_iterator(pt.it));
    pt.parent.put(id, dialog.getValue());
    m_selectedComp->loadFromPropertyTree(*m_propTree);
    enableComponent(m_selectedComp); // update
}

void EditorFrame::onPropNew(wxCommandEvent&)
{
    NewPropDialog dialog(this);
    if (showDialog(dialog) == wxID_CANCEL)
        return;
    Refresh();

    m_propTree->put(dialog.getKey(), dialog.getValue());
    m_selectedComp->loadFromPropertyTree(*m_propTree);
    enableComponent(m_selectedComp); // update
}

int EditorFrame::keyEvent(wxKeyEvent& evt)
{
    // ignore key event if we are in a text field
    if (wxString(evt.GetEventObject()->GetClassInfo()->GetClassName())
          == wxString(m_entityIdField->GetClassInfo()->GetClassName()))
        return -1;
    /*if (evt.GetId() == ID_COMPIDFIELD || evt.GetId() == ID_ENTITYIDFIELD)
        return -1;*/

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
