/*
 * Gui.cpp
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */

#include "Gui.h"
#include "common/Renderer.h"

GuiSystem::GuiSystem( RenderSystem& renderer, InputSubSystem& input ) : m_renderer (renderer), m_input (input), m_clear(false), m_isUpdating ( false )
{}

void GuiSystem::update()
{
    m_isUpdating = true;
    for ( WidgetMap::iterator it = m_widgets.begin(); it != m_widgets.end(); ++it )
    {
        if ( m_groupsToHide.count( it->first ) == 0 )
            for ( std::vector< boost::shared_ptr<Widget> >::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2 )
            {
                MouseState newMouseState = m_input.getMouseStateInArea((*it2)->m_area, true);
                if ( (*it2)->m_mouseState != newMouseState )
                    (*it2)->onMouseStateChanged(newMouseState);
                (*it2)->m_mouseState = newMouseState;
            }
    }
    if ( m_clear )
        clearContainers();
    m_isUpdating = false;
}

void GuiSystem::draw()
{
    for ( WidgetMap::iterator it = m_widgets.begin(); it != m_widgets.end(); ++it )
    {
        if ( m_groupsToHide.count( it->first ) == 0 )
            for ( std::vector< boost::shared_ptr<Widget> >::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2 )
            {
                (*it2)->draw( &m_renderer );
            }
    }
}

void GuiSystem::addWidget(const GroupId& groupId, boost::shared_ptr<Widget> pWidget)
{
    WidgetMap::iterator it = m_widgets.find( groupId );
    if ( it == m_widgets.end() )
        m_widgets.insert( std::make_pair(groupId,std::vector< boost::shared_ptr<Widget> >(1,pWidget) ) );
    else
        it->second.push_back( pWidget );
}

void GuiSystem::deleteGroup(const GroupId& groupId)
{
    m_widgets.erase( groupId );
}

/////////// Widgets /////////////

//** Label
WidgetLabel::WidgetLabel( float x, float y, const std::string& text, const FontManager& fontMngr ) : m_text ( text )
{
    float w = 0.0f, h = 0.0f;
    fontMngr.getDimensions(text, "FontW_m", w, h);
    setArea( Rect( x, x+w, y, y+h ) );
}

void WidgetLabel::draw( RenderSystem* pRenderer )
{
    /*float vertexCoord[8] = { GetArea().x1, GetArea().y1,
                               GetArea().x1, GetArea().y2,
                               GetArea().x2, GetArea().y2,
                               GetArea().x2, GetArea().y1 };
    pRenderer->DrawColorQuad( vertexCoord, 0.1f, 0.9f, 0.3f, 0.3f, true );*/
    pRenderer->drawString( m_text, "FontW_m", getArea().x1, getArea().y1 );
}

WidgetLabel::~WidgetLabel() {}

//** Button
WidgetButton::WidgetButton(const Rect& area, const std::string& caption, const ButCallback& clickedCallbackFunc, const ButCallback& mouseOverCallbackFunc)
: Widget( area ),
  m_caption (caption),
  m_butCallbackFunc ( clickedCallbackFunc ),
  m_mouseOverCallbackFunc ( mouseOverCallbackFunc ),
  m_oldMouseState ( getMouseState() )
{
}

WidgetButton::~WidgetButton() {}

void WidgetButton::onMouseStateChanged( MouseState newState )
{
    if ( newState == PressedL )
        m_butCallbackFunc();
    else if ( newState == MouseOver && m_oldMouseState != PressedL )
        m_mouseOverCallbackFunc();
    m_oldMouseState = newState;
}

void WidgetButton::draw( RenderSystem* pRenderer )
{
    float vertexCoord[8] = { getArea().x1, getArea().y1,
                             getArea().x1, getArea().y2,
                             getArea().x2, getArea().y2,
                             getArea().x2, getArea().y1 };
    pRenderer->drawColorQuad( vertexCoord, 0.1f, 0.9f, 0.3f, (getMouseState()==MouseOver||getMouseState()==PressedL)?0.3f:0.0f, true );
    pRenderer->drawString( m_caption, "FontW_m", (getArea().x1+getArea().x2)*0.5f, (getArea().y1+getArea().y2)*0.5f, AlignCenter, AlignCenter, 1.0f, 1.0f, 1.0f, (getMouseState()==MouseOver||getMouseState()==PressedL)?1.0f:0.5f );
}
