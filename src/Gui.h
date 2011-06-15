/*
 * Gui.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#ifndef GUI_H
#define GUI_H

#include <map>
#include <set>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#include "Input.h"

class RenderSubSystem;
class Widget;
class WidgetLabel;
class WidgetButton;
class FontManager;

typedef std::string GroupId;

class GuiSubSystem
{
public:
    GuiSubSystem( RenderSubSystem& renderer, InputSubSystem& input );

    void update();
    void draw();

    void insertWidget( GroupId groupId, boost::shared_ptr<Widget> pWidget );
    void deleteGroup( GroupId groupId );
    void hideGroup( GroupId groupId ) { m_groupsToHide.insert( groupId ); }
    void showGroup( GroupId groupId ) { m_groupsToHide.erase( groupId ); }
    void clear() { if (m_isUpdating) m_clear = true; else clearContainers(); }
private:
    RenderSubSystem& m_renderer;
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
    Widget( Rect area ) : m_area( area ), m_mouseState ( Away ) {}
    Widget() : m_area(), m_mouseState ( Away ) {}

    virtual ~Widget() {};

    virtual void draw( RenderSubSystem* ) {};
    virtual void onMouseStateChanged( MouseState ) {};

    void setArea( Rect area ) { m_area = area; }
    const Rect& getArea() const { return m_area; }
    const MouseState getMouseState() const { return m_mouseState; }
private:
    friend class GuiSubSystem;
    Rect m_area;
    MouseState m_mouseState;
};

class WidgetLabel : public Widget
{
public:
    WidgetLabel( float x, float y, std::string text, const FontManager& fontMngr );
    ~WidgetLabel();

    void draw( RenderSubSystem* pRenderer );
    void onMouseStateChanged( MouseState ) {}
private:
    std::string m_text;
};

#include <boost/function.hpp>
typedef boost::function0<void> ButCallbackFunc; // Einen Zeiger zu einer Funktion (boost bibliothek).

class WidgetButton : public Widget
{
public:
    WidgetButton( Rect area, std::string caption, ButCallbackFunc clickedCallbackFunc, ButCallbackFunc mouseOverCallbackFunc );
    ~WidgetButton();

    void draw( RenderSubSystem* pRenderer );
    void onMouseStateChanged( MouseState newState );
private:
    std::string m_caption;
    ButCallbackFunc m_butCallbackFunc;
    ButCallbackFunc m_mouseOverCallbackFunc;
    MouseState m_oldMouseState;
};

#endif
