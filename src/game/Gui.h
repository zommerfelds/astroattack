/*
 * Gui.h
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */

#ifndef GUI_H
#define GUI_H

#include "Input.h"

#include <map>
#include <set>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

class RenderSystem;
class Widget;
class WidgetLabel;
class WidgetButton;
class FontManager;

typedef std::string GroupId;

class GuiSystem
{
public:
    GuiSystem( RenderSystem& renderer, InputSubSystem& input );

    void update();
    void draw();

    void addWidget(const GroupId& groupId, boost::shared_ptr<Widget> pWidget);
    void deleteGroup(const GroupId& groupId);
    void hideGroup(const GroupId& groupId) { m_groupsToHide.insert( groupId ); }
    void showGroup(const GroupId& groupId) { m_groupsToHide.erase( groupId ); }
    void clear() { if (m_isUpdating) m_clear = true; else clearContainers(); }
private:
    RenderSystem& m_renderer;
    InputSubSystem& m_input;
    void clearContainers() { m_widgets.clear(); m_groupsToHide.clear(); m_clear = false; }

    typedef std::map< GroupId, std::vector< boost::shared_ptr<Widget> > > WidgetMap;
    WidgetMap m_widgets;
    std::set<GroupId> m_groupsToHide;
    bool m_clear; // falls alle Widgets gelöscht werden sollen im nächsten Update()
    bool m_isUpdating;
};

class Widget
{
public:
    Widget(const Rect& area) : m_area (area), m_mouseState (Away) {}
    Widget() : m_area(), m_mouseState ( Away ) {}

    virtual ~Widget() {};

    virtual void draw( RenderSystem* ) {};
    virtual void onMouseStateChanged( MouseState ) {};

    void setArea(const Rect& area) { m_area = area; }
    const Rect& getArea() const { return m_area; }
    const MouseState getMouseState() const { return m_mouseState; }
private:
    friend class GuiSystem;
    Rect m_area;
    MouseState m_mouseState;
};

class WidgetLabel : public Widget
{
public:
    WidgetLabel(float x, float y, const std::string& text, const FontManager& fontMngr);
    ~WidgetLabel();

    void draw( RenderSystem* pRenderer );
    void onMouseStateChanged( MouseState ) {}
private:
    std::string m_text;
};

#include <boost/function.hpp>
typedef boost::function0<void> ButCallback; // Einen Zeiger zu einer Funktion (boost bibliothek).

class WidgetButton : public Widget
{
public:
    WidgetButton(const Rect& area, const std::string& caption, const ButCallback& clickedCallbackFunc, const ButCallback& mouseOverCallbackFunc);
    ~WidgetButton();

    void draw( RenderSystem* pRenderer );
    void onMouseStateChanged( MouseState newState );
private:
    std::string m_caption;
    ButCallback m_butCallbackFunc;
    ButCallback m_mouseOverCallbackFunc;
    MouseState m_oldMouseState;
};

#endif
