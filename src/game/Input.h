/*
 * Input.h
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */

// Einlesung der Eingaben mithilfe von SDL

#ifndef INPUT_H
#define INPUT_H

#include "common/Vector2D.h"

#include <map>
#include <set>
#include <SDL.h>

enum Key
{
    MouseL,
    MouseR,
    Up,                     // Spieler nach oben
    Right,                  // Spieler nach rechts
    Down,                   // Spieler nach unten
    Left,                   // Spieler nach links
    Jump,                   // springen
    CameraUp,               // Kamera nach oben
    CameraRight,            // Kamera nach rechts
    CameraDown,             // Kamera nach unten
    CameraLeft,             // Kamera nach left
    CameraZoomIn,           // Kamera Zoom +
    CameraZoomOut,          // Kamera Zoom -
    CameraRotateCw,         // Kamera drehen im Uhrzeigersinn
    CameraRotateCcw,        // Kamera drehen im Gegenuhrzeigersinn
    CameraResetAngle,       // Kamerawinkel auf 0 zurücksetzen
    Enter,
    F4,
    Esc,
    Alt,
    EnterEditor,
    EditorCreateEntity,
    EditorCancelBlock,
    EditorCancelVertex,
    EditorNextTexture,
    EditorPrevTexture,
    EditorToggleHelp,
    SlideShowNext,
    SlideShowBack,
    SlideShowSkip
};

enum MouseState
{
    Away,
    MouseOver,
    PressedL,
    PressedR
};

// Bildschirmrechteck (um Fläche eines Knopfes darzustellen)
// x1 < x2; y1 < y2 (0/0 ist oben links, 1/1 ist unten rechts)
class Rect
{
public:
    Rect() : x1(0.0f),x2(1.0f),y1(0.0f),y2(1.0f) {}
    Rect(float x_1,float x_2,float y_1,float y_2) : x1(x_1),x2(x_2),y1(y_1),y2(y_2) {}
    float x1,x2;
    float y1,y2;
};

/*
    Diese Klasse misst die Eingaben des Benutzers. (Maus und Tastatur)
    Sie verarbeitet oder tut nichts mit den eingelesenen informationen.
    Jedesmal wenn Update() aufgerufen wird, werden die Informationen aktualisiert.
    Die Eigabe-Informationen können mit den Mathoden gelesen werden.
*/

class InputSubSystem
{
public:
    InputSubSystem();

public:
    void update(); // aktualisieren

    bool isKeyDown(Key key, bool consume=true);
    MouseState getMouseStateInArea(const Rect& buttonRect, bool consume=true);
    const Vector2D& getMousePos() const;   // Mausposition erhalten (0/0 ist oben links, 1/1 ist unten rechts)
    void warpMouseToCenter();

private:
    bool testKey(Key key);

    Uint8* m_sdlKeyStates;
    std::set<Key> m_consumedKeys;
    Uint8 m_sdlMouseStates;
    Vector2D m_windowMousePos;
    std::map<Key, SDLKey> m_keyMap;
};

#endif
