/*----------------------------------------------------------\
|                        GameApp.h                          |
|                        ---------                          |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// Einlesung der Eingaben mithilfe von SDL

#ifndef INPUT_H
#define INPUT_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include <map>
#include <set>
#include <SDL/SDL.h>
#include <boost/scoped_ptr.hpp>

enum Key
{
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

class Vector2D;

enum MouseState
{
    MouseOver,
    PressedL,
    PressedR,
    Away
};

// Bildschirmrechteck (um Fläche eines Knopfes darzustellen)
// x1 < x2; y1 < y2 (0/0 ist oben links, 1/1 ist unten rechts)
struct Rect
{
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
    ~InputSubSystem();

public:
    void Update(); // aktualisieren

    bool KeyState ( Key key ) const;    // Status der SDL_Key Taste erhalten
    bool KeyStateConsume( Key key );    // gleich wie oben, aber der Status der Taste zurückgesetzt (nicht mehr gedrückt)
    bool RMouseKeyState( ) const;       // status des rechten Mausknopfes
    bool RMouseKeyStateConsume();       // gleich wie oben, aber der Status der Taste zurückgesetzt (nicht mehr gedrückt)
    bool LMouseKeyState( ) const;       // status des linken Mausknopfes
    bool LMouseKeyStateConsume();       // gleich wie oben, aber der Status der Taste zurückgesetzt (nicht mehr gedrückt)
    const Vector2D* MousePos() const;   // Mausposition erhalten (0/0 ist oben links, 1/1 ist unten rechts)
    MouseState GetMouseStateInArea( const Rect& rButtonRect ) const;    // Wird die gegebene Fläche gedrückt?
    MouseState GetMouseStateInAreaConsume( const Rect& rButtonRect );   // gleich wie oben, aber der Status der Taste zurückgesetzt (nicht mehr gedrückt)

private:
    Uint8 *m_pKeystates;
    std::set<Key> m_pKeysConsumed;
    Uint8 m_mousestates;
    bool m_RMouseConsumed;
    bool m_LMouseConsumed;
    boost::scoped_ptr<Vector2D> m_pWindowMousePos;
    std::map<Key,SDLKey> m_keyMap;
};

#endif

// Astro Attack - Christian Zommerfelds - 2009
