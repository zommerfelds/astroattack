/*
 * EditorApp.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#ifndef EDITOR_APP_H
#define EDITOR_APP_H

#include <wx/wx.h>
#include <boost/scoped_ptr.hpp>

struct GameEvents;
class RenderSubSystem;
class PhysicsSubSystem;

class EditorApp : public wxApp
{
public:
    virtual bool OnInit();
    virtual void CleanUp();

private:
    boost::scoped_ptr<GameEvents> m_events;
    boost::scoped_ptr<RenderSubSystem> m_renderer;
    boost::scoped_ptr<PhysicsSubSystem> m_physics;
};

#endif
