/*
 * EditorFrame.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#ifndef EDITOR_FRAME_H
#define EDITOR_FRAME_H

#include "GLCanvas.h"
#include "gen/EditorFrameBase.h"
#include <wx/timer.h>

class Editor;

class UpdateTimer : public wxTimer
{
    GLCanvas& pane;
public:
    UpdateTimer(GLCanvas& pane) : pane (pane) {}

    void Notify()
    {
        pane.Refresh();
    }
};

struct GameEvents;

class EditorFrame : public EditorFrameBase {
public:
    EditorFrame(Editor& editor, GameEvents& events);
    ~EditorFrame();
    void OnClose(wxCloseEvent& evt);
    
private:
    GLCanvas m_canvas;
    UpdateTimer m_timer;

    DECLARE_EVENT_TABLE()
};

#endif
