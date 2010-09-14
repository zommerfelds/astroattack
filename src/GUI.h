/*----------------------------------------------------------\
|                          GUI.h                            |
|                          -----                            |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#ifndef GUI_H
#define GUI_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include <map>
#include <set>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

class RenderSubSystem;
//class InputSubSystem;

#include "Input.h"

class Widget;
class WidgetLabel;
class WidgetButton;
class FontManager;

typedef std::string GroupId;
typedef std::map< GroupId, std::vector< boost::shared_ptr<Widget> > > WidgetMap;

class GuiSubSystem
{
public:
    GuiSubSystem( RenderSubSystem* pRenderer, InputSubSystem* pInput );
    ~GuiSubSystem();

    void Update();
    void Draw();

    void InsertWidget( GroupId groupId, boost::shared_ptr<Widget> pWidget );
    void DeleteGroup( GroupId groupId );
    void HideGroup( GroupId groupId ) { m_groupsToHide.insert( groupId ); }
    void ShowGroup( GroupId groupId ) { m_groupsToHide.erase( groupId ); }
    void Clear() { if (m_isUpdating) m_clear = true; else ClearContainers(); }
private:
    RenderSubSystem* m_pRenderer;
    InputSubSystem* m_pInput;
    void ClearContainers() { m_widgets.clear(); m_groupsToHide.clear(); m_clear = false; }

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

    virtual void Draw( RenderSubSystem* /*pRenderer*/ ) {};
    virtual void MouseStateChanged( MouseState /*newState*/ ) {}

    void SetArea( Rect area ) { m_area = area; }
    const Rect& GetArea() const { return m_area; }
    const MouseState GetMouseState() const { return m_mouseState; }
private:
    friend class GuiSubSystem;
    Rect m_area;
    MouseState m_mouseState;
};

class WidgetLabel : public Widget
{
public:
    WidgetLabel( float x, float y, std::string text, const FontManager* fontMngr );
    ~WidgetLabel();

    void Draw( RenderSubSystem* pRenderer );
    //void MouseStateChanged( MouseState newState ) const {}
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

    void Draw( RenderSubSystem* pRenderer );
    void MouseStateChanged( MouseState newState );
private:
    std::string m_caption;
    ButCallbackFunc m_butCallbackFunc;
    ButCallbackFunc m_mouseOverCallbackFunc;
    MouseState m_oldMouseState;
};

#endif

// Astro Attack - Christian Zommerfelds - 2009
