/*
 * EditorFrame.h
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */

#ifndef EDITOR_FRAME_H
#define EDITOR_FRAME_H

#include "GlCanvasController.h"
#include "gen/EditorGuiBase.h"
#include "common/IdTypes.h"
#include <string>
#include <vector>
#include <wx/timer.h>
#include <boost/shared_ptr.hpp>

// forward declare ptree (PropertyTree), quite complex but we don't want to include the big header here
namespace boost { namespace property_tree {
    template<class Key, class Data, class KeyCompare> class basic_ptree;
    typedef basic_ptree<std::string, std::string, std::less<std::string> > ptree;
}}

/*enum ACC_IDs
{
    ACC_ID_lowest = wxID_HIGHEST+100,

    ACC_ID_ENTER = ACC_ID_lowest,
    ACC_ID_ESCAPE,
    ACC_ID_BACKSPACE,
    ACC_ID_NEXTTEXTURE,
    ACC_ID_PREVTEXTURE,

    ACC_ID_highest
};*/

class Editor;
class RenderSystem;

class UpdateTimer : public wxTimer
{
    GlCanvasController& pane;
public:
    UpdateTimer(GlCanvasController& pane) : pane (pane) {}

    void Notify()
    {
        pane.Refresh();
    }
};

class GameEvents;
class Component;
class PropTreeElem;

class EditorFrame : public EditorFrameBase {
public:
    /**
     * renderer: uninitialized renderer
     */
    EditorFrame(Editor& editor, RenderSystem& renderer);
    ~EditorFrame();

    void update();

    int keyEvent(wxKeyEvent&);

private:

    void onClose(wxCloseEvent&);
    void onListItemSelected(wxListEvent&);
    void onListItemDeselected(wxListEvent&);
    void onMenuNew(wxCommandEvent&);
    void onMenuOpen(wxCommandEvent&);
    void onMenuSave(wxCommandEvent&);
    void onMenuSaveAs(wxCommandEvent&);
    void onMenuExit(wxCommandEvent&);
    void onMenuAbout(wxCommandEvent&);
    void onMenuHelp(wxCommandEvent&);
    void onEntityIdFieldEnter(wxCommandEvent&);
    void onCompIdFieldEnter(wxCommandEvent&);
    void onPropEditBut(wxCommandEvent&);
    void onPropEditList(wxListEvent&);
    void onPropEdit();
    void onPropNew(wxCommandEvent&);

    void enableEntity(const EntityId& id, const std::vector<const Component*>& comps); // an Entity is selected
    void disableEntity();

    void enableComponent(Component* comp); // a component is selected
    void disableComponent();

    void enableProperty(); // a property inside a component is selected
    void disableProperty();

    int showDialog(wxDialog&);

    int updatePropList(boost::property_tree::ptree& propTree, int index, std::string prefix);

    Editor& m_editor;
    GlCanvasController m_canvas;
    UpdateTimer m_timer;
    std::string m_fileName;
    Component* m_selectedComp;
    boost::shared_ptr<boost::property_tree::ptree> m_propTree;
    std::vector<boost::shared_ptr<PropTreeElem> > m_propTreeElems;

    DECLARE_EVENT_TABLE()
};

#endif
