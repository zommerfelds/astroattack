/*
 * Input.cpp
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */

#include "Input.h"
#include "Configuration.h"
#include "common/Logger.h"

InputSubSystem::InputSubSystem() : m_sdlKeyStates ( NULL ),
                                   m_sdlMouseStates ( 0 ),
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
    m_keyMap[SlideShowSkip] = SDLK_TAB;
}

// Eingabestatus speichern (Maus und Tastatur)
void InputSubSystem::update()
{
    // Eingabe aktualisieren
    m_sdlKeyStates = SDL_GetKeyState( NULL );
    std::set<Key>::iterator next;
    for ( std::set<Key>::iterator it = m_consumedKeys.begin(); it != m_consumedKeys.end(); it=next )
    {
        next = it;
        ++next;
        if ( testKey(*it) == false )
            m_consumedKeys.erase( it );
    }

    if ( !gConfig.get<bool>("FullScreen") )
    {
        int mousePosX;
        int mousePosY;
        m_sdlMouseStates = SDL_GetMouseState( &mousePosX, &mousePosY );
        m_windowMousePos.x = mousePosX / ( float ) gConfig.get<int>("ScreenWidth");
        m_windowMousePos.y = mousePosY / ( float ) gConfig.get<int>("ScreenHeight");
    }
    else
    {
        int relMouseX = 0, relMouseY = 0;
        m_sdlMouseStates = SDL_GetRelativeMouseState(&relMouseX, &relMouseY);
        m_windowMousePos.x += relMouseX * gConfig.get<float>("MouseSensitivity") / gConfig.get<int>("ScreenWidth");
        m_windowMousePos.y += relMouseY * gConfig.get<float>("MouseSensitivity") / gConfig.get<int>("ScreenHeight");
        if ( m_windowMousePos.x < 0.0f )
            m_windowMousePos.x = 0.0f;
        if ( m_windowMousePos.x > 1.0f )
            m_windowMousePos.x = 1.0f;
        if ( m_windowMousePos.y < 0.0f )
            m_windowMousePos.y = 0.0f;
        if ( m_windowMousePos.y > 1.0f )
            m_windowMousePos.y = 1.0f;
    }
    if ( (m_sdlMouseStates & SDL_BUTTON(SDL_BUTTON_LEFT)) == 0 )
        m_consumedKeys.erase( MouseL );
    if ( (m_sdlMouseStates & SDL_BUTTON(SDL_BUTTON_RIGHT)) == 0 )
        m_consumedKeys.erase( MouseR );
}

bool InputSubSystem::testKey(Key key)
{
    if (key == MouseL)
        return ( (m_sdlMouseStates & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0 );
    else if (key == MouseR)
        return ( (m_sdlMouseStates & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0 );
    else
    {
        std::map<Key,SDLKey>::const_iterator i = m_keyMap.find(key);
        if ( i == m_keyMap.end() ) {
            log(Warning) << "Asking for key <" << key << "> which is not in key map\n";
            return false;
        }
        if ( !m_sdlKeyStates )
            return false;
        return ( m_sdlKeyStates[ i->second ] == 1 );
    }
}

bool InputSubSystem::isKeyDown(Key key, bool consume)
{
    if ( m_consumedKeys.count( key ) == 1 )
        return false;
    bool isPressed = testKey(key);
    if (consume && isPressed)
        m_consumedKeys.insert(key);
    return isPressed;
}

const Vector2D& InputSubSystem::getMousePos() const
{
    return m_windowMousePos;
}

void InputSubSystem::warpMouseToCenter()
{
    SDL_WarpMouse((Uint16)(gConfig.get<int>("ScreenWidth")/2), (Uint16)(gConfig.get<int>("ScreenHeight")/2));
}

MouseState InputSubSystem::getMouseStateInArea(const Rect& buttonRect, bool consume)
{
    if ( buttonRect.x1 > m_windowMousePos.x || buttonRect.x2 < m_windowMousePos.x ||
         buttonRect.y1 > m_windowMousePos.y || buttonRect.y2 < m_windowMousePos.y )
        return Away;
    else if ( isKeyDown(MouseL, consume) )
        return PressedL;
    else if ( isKeyDown(MouseR, consume) )
        return PressedR;
    else
        return MouseOver;
}
