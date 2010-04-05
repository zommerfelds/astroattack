/*----------------------------------------------------------\
|                        Input.cpp                          |
|                        ---------                          |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "main.h" // wichtige Definitionen und Dateien einbinden

#include "Vector2D.h"
#include "Input.h"

const float cMouseSensivityFactor = 0.001f;

InputSubSystem::InputSubSystem() : m_pKeystates ( NULL ),
                                   m_mousestates ( 0 ),
                                   m_RMouseConsumed ( false ),
                                   m_LMouseConsumed ( false ),
                                   m_pWindowMousePos ( new Vector2D(0.5f,0.5f) )
{
    // Tasten zuweisen:
    // (siehe SDL_keysym.h für Tastencodes)
    m_keyMap[Up] = SDLK_w;
    m_keyMap[Down] = SDLK_s;
    m_keyMap[Right] = SDLK_d;
    m_keyMap[Left] = SDLK_a;
    m_keyMap[Jump] = SDLK_SPACE;
    m_keyMap[CameraUp] = SDLK_UP;
    m_keyMap[CameraDown] = SDLK_DOWN;
    m_keyMap[CameraRight] = SDLK_RIGHT;
    m_keyMap[CameraLeft] = SDLK_LEFT;
    m_keyMap[CameraZoomIn] = SDLK_KP_PLUS;
    m_keyMap[CameraZoomOut] = SDLK_KP_MINUS;
    m_keyMap[CameraRotateCw] = SDLK_KP_MULTIPLY;
    m_keyMap[CameraRotateCcw] = SDLK_KP_DIVIDE;
    m_keyMap[CameraResetAngle] = SDLK_KP_ENTER;
    m_keyMap[Enter] = SDLK_RETURN;
    m_keyMap[Esc] = SDLK_ESCAPE;
    m_keyMap[F4] = SDLK_F4;
    m_keyMap[Alt] = SDLK_LALT;
    m_keyMap[EnterEditor] = SDLK_e;
    m_keyMap[EditorCreateEntity] = SDLK_RETURN;
    m_keyMap[EditorCancelBlock] = SDLK_DELETE;
    m_keyMap[EditorCancelVertex] = SDLK_BACKSPACE;
    m_keyMap[EditorNextTexture] = SDLK_PAGEUP;
    m_keyMap[EditorPrevTexture] = SDLK_PAGEDOWN;
    m_keyMap[EditorToggleHelp] = SDLK_h;
    m_keyMap[SlideShowNext] = SDLK_RIGHT;
    m_keyMap[SlideShowBack] = SDLK_LEFT;
    m_keyMap[SlideShowSkip] = SDLK_DELETE;
}

InputSubSystem::~InputSubSystem()
{
}

// Eingabestatus speichern (Maus und Tastatur)
void InputSubSystem::Update()
{
    // Eingabe aktualisieren
    m_pKeystates = SDL_GetKeyState ( NULL );
    std::set<Key>::iterator next;
    for ( std::set<Key>::iterator it = m_pKeysConsumed.begin(); it != m_pKeysConsumed.end(); it=next )
    {
        next = it;
        ++it;
        if ( KeyState(*it)==false )
            m_pKeysConsumed.erase( it );
    }

    if ( gAaConfig.GetInt("FullScreen") == 0 )
    {
        int mousePosX;
        int mousePosY;
        m_mousestates = SDL_GetMouseState ( &mousePosX, &mousePosY );
        m_pWindowMousePos->x = mousePosX / ( float ) gAaConfig.GetInt("ScreenWidth");
        m_pWindowMousePos->y = mousePosY / ( float ) gAaConfig.GetInt("ScreenHeight");
        //std::cout << "Mousestate: " << (int)m_mousestates << " " << (LMouseKeyState()?"Left ":"NotLeft ") << (RMouseKeyState()?"Right\n":"NotRight\n");
    }
    else
    {
        int relMouseX = 0, relMouseY = 0;
        m_mousestates = SDL_GetRelativeMouseState( &relMouseX, &relMouseY );
        m_pWindowMousePos->x += relMouseX * cMouseSensivityFactor * gAaConfig.GetFloat("MouseSensitivity");
        m_pWindowMousePos->y += relMouseY * cMouseSensivityFactor * gAaConfig.GetFloat("MouseSensitivity");
        if ( m_pWindowMousePos->x < 0.0f )
            m_pWindowMousePos->x = 0.0f;
        if ( m_pWindowMousePos->x > 1.0f )
            m_pWindowMousePos->x = 1.0f;
        if ( m_pWindowMousePos->y < 0.0f )
            m_pWindowMousePos->y = 0.0f;
        if ( m_pWindowMousePos->y > 1.0f )
            m_pWindowMousePos->y = 1.0f;
    }
    if ( ((m_mousestates & SDL_BUTTON ( SDL_BUTTON_RIGHT ))!=0)==false )
        m_RMouseConsumed = false;
    if ( ((m_mousestates & SDL_BUTTON ( SDL_BUTTON_LEFT ))!=0)==false )
        m_LMouseConsumed = false;
}

bool InputSubSystem::KeyState( Key key ) const
{
    std::map<Key,SDLKey>::const_iterator i = m_keyMap.find(key);
    if ( i == m_keyMap.end() )
        return false;
    if ( !m_pKeystates )
        return false;
    if ( m_pKeysConsumed.count( key )==1 )
        return false;
    return ( m_pKeystates[ i->second ] == 1 );
}

bool InputSubSystem::KeyStateConsume( Key key )
{
    std::map<Key,SDLKey>::const_iterator i = m_keyMap.find(key);
    if ( i == m_keyMap.end() )
        return false;
    if ( !m_pKeystates )
        return false;
    m_pKeysConsumed.insert( key );
    return ( m_pKeystates[ i->second ] == 1 );
}

bool InputSubSystem::RMouseKeyState() const
{
    if ( m_RMouseConsumed )
        return false;
    return ( (m_mousestates & SDL_BUTTON ( SDL_BUTTON_RIGHT ))!=0 );
}

bool InputSubSystem::RMouseKeyStateConsume()
{
    m_RMouseConsumed = true;
    return ( (m_mousestates & SDL_BUTTON ( SDL_BUTTON_RIGHT ))!=0 );
}

bool InputSubSystem::LMouseKeyState() const
{
    if ( m_LMouseConsumed )
        return false;
    return ( (m_mousestates & SDL_BUTTON ( SDL_BUTTON_LEFT ))!=0 );
}

bool InputSubSystem::LMouseKeyStateConsume()
{
    m_LMouseConsumed = true;
    return ( (m_mousestates & SDL_BUTTON ( SDL_BUTTON_LEFT ))!=0 );
}

const Vector2D* InputSubSystem::MousePos() const
{
    return m_pWindowMousePos.get();
}

MouseState InputSubSystem::GetMouseStateInArea( const Rect& rButtonRect ) const
{
    if ( rButtonRect.x1 > m_pWindowMousePos->x || rButtonRect.x2 < m_pWindowMousePos->x ||
         rButtonRect.y1 > m_pWindowMousePos->y || rButtonRect.y2 < m_pWindowMousePos->y )
        return Away;
    else if ( LMouseKeyState() )
        return PressedL;
    else if ( RMouseKeyState() )
        return PressedR;
    else
        return MouseOver;
}

MouseState InputSubSystem::GetMouseStateInAreaConsume( const Rect& rButtonRect )
{
    if ( rButtonRect.x1 > m_pWindowMousePos->x || rButtonRect.x2 < m_pWindowMousePos->x ||
         rButtonRect.y1 > m_pWindowMousePos->y || rButtonRect.y2 < m_pWindowMousePos->y )
        return Away;
    else if ( LMouseKeyState() )
    {
        LMouseKeyStateConsume();
        return PressedL;
    }
    else if ( RMouseKeyState() )
    {
        RMouseKeyStateConsume();
        return PressedR;
    }
    else
        return MouseOver;
}

// Astro Attack - Christian Zommerfelds - 2009
