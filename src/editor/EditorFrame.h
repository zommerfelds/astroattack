/*
 * EditorFrame.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#ifndef EDITOR_FRAME_H
#define EDITOR_FRAME_H

#include "GlCanvasController.h"
#include "gen/EditorGuiBase.h"
#include <wx/timer.h>

enum ACC_IDs
{
    ACC_ID_lowest = wxID_HIGHEST+100,

    ACC_ID_ENTER = ACC_ID_lowest,
    ACC_ID_ESCAPE,
    ACC_ID_BACKSPACE,
    ACC_ID_NEXTTEXTURE,
    ACC_ID_PREVTEXTURE,

    ACC_ID_highest
};

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

class EditorFrame : public EditorFrameBase {
public:
    /**
     * renderer: uninitialized renderer
     */
    EditorFrame(Editor& editor, RenderSystem& renderer);
    ~EditorFrame();

    void update();

    void onClose(wxCloseEvent&);
    void onCustomAccelerator(wxCommandEvent&);
    void onComponentActivated(wxListEvent&);
    void onMenuNew(wxCommandEvent&);
    void onMenuOpen(wxCommandEvent&);
    void onMenuSave(wxCommandEvent&);
    void onMenuSaveAs(wxCommandEvent&);
    void onMenuExit(wxCommandEvent&);
    void onMenuAbout(wxCommandEvent&);
    
private:
    Editor& m_editor;
    GlCanvasController m_canvas;
    UpdateTimer m_timer;
    std::string m_fileName;

    DECLARE_EVENT_TABLE()
};

#endif
