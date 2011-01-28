/*----------------------------------------------------------\
|                         GUI.cpp                           |
|                         -------                           |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "GUI.h"
#include "Renderer.h"

GuiSubSystem::GuiSubSystem( RenderSubSystem* pRenderer, InputSubSystem* pInput ) : m_pRenderer (pRenderer), m_pInput (pInput), m_clear(false), m_isUpdating ( false )
{
}

GuiSubSystem::~GuiSubSystem()
{
}

void GuiSubSystem::Update()
{
    m_isUpdating = true;
    for ( WidgetMap::iterator it = m_widgets.begin(); it != m_widgets.end(); ++it )
    {
        if ( m_groupsToHide.count( it->first ) == 0 )
            for ( std::vector< boost::shared_ptr<Widget> >::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2 )
            {
                MouseState newMouseState = m_pInput->GetMouseStateInAreaConsume( (*it2)->m_area );
                if ( (*it2)->m_mouseState != newMouseState )
                    (*it2)->MouseStateChanged(newMouseState);
                (*it2)->m_mouseState = newMouseState;
            }
    }
    if ( m_clear )
        ClearContainers();
    m_isUpdating = false;
}

void GuiSubSystem::Draw()
{
    for ( WidgetMap::iterator it = m_widgets.begin(); it != m_widgets.end(); ++it )
    {
        if ( m_groupsToHide.count( it->first ) == 0 )
            for ( std::vector< boost::shared_ptr<Widget> >::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2 )
            {
                (*it2)->Draw( m_pRenderer );
            }
    }
}

void GuiSubSystem::InsertWidget( GroupId groupId, const boost::shared_ptr<Widget>& pWidget )
{
    WidgetMap::iterator it = m_widgets.find( groupId );
    if ( it == m_widgets.end() )
        m_widgets.insert( std::make_pair(groupId,std::vector< boost::shared_ptr<Widget> >(1,pWidget) ) );
    else
        it->second.push_back( pWidget );
}

void GuiSubSystem::DeleteGroup( GroupId groupId )
{
    m_widgets.erase( groupId );
}

/////////// Widgets /////////////

//** Label
WidgetLabel::WidgetLabel( float x, float y, std::string text, const FontManager* fontMngr ) : m_text ( text )
{
    float w = 0.0f, h = 0.0f;
    fontMngr->GetDimensions(text, "FontW_m", w, h);
    SetArea( Rect( x, x+w/4.0f, y, y+h/3.0f ) );
}

void WidgetLabel::Draw( RenderSubSystem* pRenderer )
{
    /*float vertexCoord[8] = { GetArea().x1*4.0f, GetArea().y1*3.0f,
                             GetArea().x1*4.0f, GetArea().y2*3.0f,
                             GetArea().x2*4.0f, GetArea().y2*3.0f,
                             GetArea().x2*4.0f, GetArea().y1*3.0f };
    pRenderer->DrawColorQuad( vertexCoord, 0.1f, 0.9f, 0.3f, 0.3f, true );*/
    pRenderer->DrawString( m_text, "FontW_m", GetArea().x1*4, GetArea().y1*3 );
}

WidgetLabel::~WidgetLabel() {}

//** Button
WidgetButton::WidgetButton( Rect area, std::string caption, ButCallbackFunc clickedCallbackFunc, ButCallbackFunc mouseOverCallbackFunc )
: Widget( area ),
  m_caption (caption),
  m_butCallbackFunc ( clickedCallbackFunc ),
  m_mouseOverCallbackFunc ( mouseOverCallbackFunc ),
  m_oldMouseState ( GetMouseState() )
{
}

WidgetButton::~WidgetButton() {}

void WidgetButton::MouseStateChanged( MouseState newState )
{
    if ( newState == PressedL )
        m_butCallbackFunc();
    else if ( newState == MouseOver && m_oldMouseState != PressedL )
        m_mouseOverCallbackFunc();
    m_oldMouseState = newState;
}

void WidgetButton::Draw( RenderSubSystem* pRenderer )
{
    float vertexCoord[8] = { GetArea().x1*4.0f, GetArea().y1*3.0f,
                             GetArea().x1*4.0f, GetArea().y2*3.0f,
                             GetArea().x2*4.0f, GetArea().y2*3.0f,
                             GetArea().x2*4.0f, GetArea().y1*3.0f };
    pRenderer->DrawColorQuad( vertexCoord, 0.1f, 0.9f, 0.3f, (GetMouseState()==MouseOver||GetMouseState()==PressedL)?0.3f:0.0f, true );
    pRenderer->DrawString( m_caption, "FontW_m", (GetArea().x1+GetArea().x2)*2.0f /* /2*4 */, (GetArea().y1+GetArea().y2)*1.5f /* /2*3 */, AlignCenter, AlignCenter, 1.0f, 1.0f, 1.0f, (GetMouseState()==MouseOver||GetMouseState()==PressedL)?1.0f:0.5f );
}

// Astro Attack - Christian Zommerfelds - 2009
