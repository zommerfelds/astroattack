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

class Editor;
class RenderSubSystem;

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

struct GameEvents;

class EditorFrame : public EditorFrameBase {
public:
    /**
     * renderer: uninitialized renderer
     */
    EditorFrame(Editor& editor, RenderSubSystem& renderer);
    ~EditorFrame();
    void onClose(wxCloseEvent&);
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
