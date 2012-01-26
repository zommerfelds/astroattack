/*
 * Input.cpp
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#include "Input.h"
#include "Configuration.h"

const float cMouseSensivityFactor = 0.001f;

InputSubSystem::InputSubSystem() : m_pKeystates ( NULL ),
                                   m_mousestates ( 0 ),
                                   m_RMouseConsumed ( false ),
                                   m_LMouseConsumed ( false ),
                                   m_windowMousePos ( 0.5f, 0.5f )
{
    // Tasten zuweisen:
    // (siehe SDL_keysym.h für Tastencodes)
    // Possible optimization: use array instead of map
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
    /*m_keyMap[CameraZoomIn] = SDLK_1;
    m_keyMap[CameraZoomOut] = SDLK_2;
    m_keyMap[CameraRotateCw] = SDLK_4;
    m_keyMap[CameraRotateCcw] = SDLK_3;
    m_keyMap[CameraResetAngle] = SDLK_5;*/
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

// Eingabestatus speichern (Maus und Tastatur)
void InputSubSystem::update()
{
    // Eingabe aktualisieren
    m_pKeystates = SDL_GetKeyState ( NULL );
    std::set<Key>::iterator next;
    for ( std::set<Key>::iterator it = m_pKeysConsumed.begin(); it != m_pKeysConsumed.end(); it=next )
    {
        next = it;
        ++it;
        if ( getKeyState(*it)==false )
            m_pKeysConsumed.erase( it );
    }

    if ( gConfig.get<bool>("FullScreen") == 0 )
    {
        int mousePosX;
        int mousePosY;
        m_mousestates = SDL_GetMouseState ( &mousePosX, &mousePosY );
        m_windowMousePos.x = mousePosX / ( float ) gConfig.get<int>("ScreenWidth");
        m_windowMousePos.y = mousePosY / ( float ) gConfig.get<int>("ScreenHeight");
    }
    else
    {
        int relMouseX = 0, relMouseY = 0;
        m_mousestates = SDL_GetRelativeMouseState( &relMouseX, &relMouseY );
        m_windowMousePos.x += relMouseX * cMouseSensivityFactor * gConfig.get<float>("MouseSensitivity");
        m_windowMousePos.y += relMouseY * cMouseSensivityFactor * gConfig.get<float>("MouseSensitivity");
        if ( m_windowMousePos.x < 0.0f )
            m_windowMousePos.x = 0.0f;
        if ( m_windowMousePos.x > 1.0f )
            m_windowMousePos.x = 1.0f;
        if ( m_windowMousePos.y < 0.0f )
            m_windowMousePos.y = 0.0f;
        if ( m_windowMousePos.y > 1.0f )
            m_windowMousePos.y = 1.0f;
    }
    if ( (m_mousestates & SDL_BUTTON ( SDL_BUTTON_RIGHT ))==0 )
        m_RMouseConsumed = false;
    if ( (m_mousestates & SDL_BUTTON ( SDL_BUTTON_LEFT ))==0 )
        m_LMouseConsumed = false;
}

bool InputSubSystem::getKeyState( Key key ) const
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

bool InputSubSystem::getKeyStateConsume( Key key )
{
    std::map<Key,SDLKey>::const_iterator i = m_keyMap.find(key);
    if ( i == m_keyMap.end() )
        return false;
    if ( !m_pKeystates )
        return false;
    bool inserted = m_pKeysConsumed.insert( key ).second;
    return inserted && ( m_pKeystates[ i->second ] == 1 );
}

bool InputSubSystem::getRMouseKeyState() const
{
    if ( m_RMouseConsumed )
        return false;
    return ( (m_mousestates & SDL_BUTTON ( SDL_BUTTON_RIGHT ))!=0 );
}

bool InputSubSystem::getRMouseKeyStateConsume()
{
    if ( m_RMouseConsumed )
        return false;
    m_RMouseConsumed = true;
    return ( (m_mousestates & SDL_BUTTON ( SDL_BUTTON_RIGHT ))!=0 );
}

bool InputSubSystem::getLMouseKeyState() const
{
    if ( m_LMouseConsumed )
        return false;
    return ( (m_mousestates & SDL_BUTTON ( SDL_BUTTON_LEFT ))!=0 );
}

bool InputSubSystem::getLMouseKeyStateConsume()
{
    if ( m_LMouseConsumed )
        return false;
    m_LMouseConsumed = true;
    return ( (m_mousestates & SDL_BUTTON ( SDL_BUTTON_LEFT ))!=0 );
}

const Vector2D& InputSubSystem::getMousePos() const
{
    return m_windowMousePos;
}

void InputSubSystem::putMouseOnCenter()
{
    SDL_WarpMouse((Uint16)(gConfig.get<int>("ScreenWidth")/2), (Uint16)(gConfig.get<int>("ScreenHeight")/2));
}

MouseState InputSubSystem::getMouseStateInArea( const Rect& rButtonRect ) const
{
    if ( rButtonRect.x1 > m_windowMousePos.x || rButtonRect.x2 < m_windowMousePos.x ||
         rButtonRect.y1 > m_windowMousePos.y || rButtonRect.y2 < m_windowMousePos.y )
        return Away;
    else if ( getLMouseKeyState() )
        return PressedL;
    else if ( getRMouseKeyState() )
        return PressedR;
    else
        return MouseOver;
}

MouseState InputSubSystem::getMouseStateInAreaConsume( const Rect& rButtonRect )
{
    if ( rButtonRect.x1 > m_windowMousePos.x || rButtonRect.x2 < m_windowMousePos.x ||
         rButtonRect.y1 > m_windowMousePos.y || rButtonRect.y2 < m_windowMousePos.y )
        return Away;
    else if ( getLMouseKeyState() )
    {
        getLMouseKeyStateConsume();
        return PressedL;
    }
    else if ( getRMouseKeyState() )
    {
        getRMouseKeyStateConsume();
        return PressedR;
    }
    else
        return MouseOver;
}
