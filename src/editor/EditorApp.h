/*
 * EditorApp.h
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */

#ifndef EDITOR_APP_H
#define EDITOR_APP_H

#include <wx/wx.h>
#include <boost/scoped_ptr.hpp>

class GameEvents;
class RenderSystem;
class PhysicsSystem;
class Editor;
class EditorFrame;

class EditorApp : public wxApp
{
public:

private:
    bool OnInit();
    int OnExit();
    int FilterEvent(wxEvent& event);

    boost::scoped_ptr<GameEvents> m_events;
    boost::scoped_ptr<RenderSystem> m_renderer;
    boost::scoped_ptr<PhysicsSystem> m_physics;
    boost::scoped_ptr<Editor> m_editor;
    EditorFrame* m_frame;
};

#endif
