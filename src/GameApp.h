/*
 * GameApp.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#ifndef GAMEAPP_H
#define GAMEAPP_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include <boost/scoped_ptr.hpp>
// Simple DirectMedia Layer (freie Plattform-übergreifende Multimedia-Programmierschnittstelle)
#include "SDL.h"
#include "GameEvents.h"

#include <vector>
#include <string>

//struct GameEvents;
class InputSubSystem;
class PhysicsSubSystem;
class GameWorld;
class GameCamera;
class RenderSubSystem;
class StateManager;
class SoundSubSystem;
class GuiSubSystem;

struct SubSystems;

/*
    Das ist die Hauptklasse des Spiels.
    Sie initialisiert, führt aus und deinitialisiert das Spiel.
*/
class GameApp
{
public:
    GameApp(const std::vector<std::string>& args);
    ~GameApp(); // need to implement destructor manually because of scoped_ptr (incomplete type)

    void Run();     // Spiel starten (nach der Initialisierung ), d.h. Hauptschleife starten

private:
    void Init();    // Astro Attack initialisieren
    void DeInit();  // Speicher wieder freigeben

    void MainLoop(); // Hauptschleife

    boost::scoped_ptr<SubSystems> m_pSubSystems; // Untersysteme

    bool m_quit; // Ob Programm beenden werden soll
    void OnQuit() { m_quit = true; } // Spiel beenden (Wird von einem Event aufgerufen)

    EventConnection m_eventConnection; // TODO: scoped_ptr

    void UpdateGame();
    void HandleSdlQuitEvents( SDL_Event& rSdlEvent, bool& rQuit );
    void CalcFPS( Uint32 curTime );

    Uint32 m_fpsMeasureStart;
    Uint32 m_framesCounter;

    std::string m_levelToLoad; // empty if shoud go to main menu
    bool m_startGame; // true if the game should be started (or false if only a message should be printed i.e. '--version')

    bool m_fullScreen;         // if the game should be started in full screen mode
    bool m_overRideFullScreen; // if the variable above should override the full screen option in the configuration file

    // Initialisationsfunktionen in Init.cpp
    bool InitSDL();
    bool InitVideo();

    void ParseArguments( const std::vector<std::string>& args ); // Programmargumente verarbeiten
};

// Alle wichtigen Untersysteme des Spieles
// TODO: use PIMPL and objects on the stack (no scoped_ptr)
struct SubSystems
{
    SubSystems();
    ~SubSystems();
    boost::scoped_ptr<StateManager> stateManager;   // States
    boost::scoped_ptr<GameEvents> events;           // Spielereignisse
    boost::scoped_ptr<InputSubSystem> input;        // Eingabe
    boost::scoped_ptr<PhysicsSubSystem> physics;    // Physik
    boost::scoped_ptr<RenderSubSystem> renderer;    // Ausgabe
    boost::scoped_ptr<SoundSubSystem> sound;        // Sound
    boost::scoped_ptr<GuiSubSystem> gui;            // Grafische Benutzeroberfläche
    bool isLoading; // wenn true: Zeitakkumulator wird neu gestartet, sobald die Hauptschleife einmal durch ist
                    // das verhindert, dass die Zeit währed Ladezeiten gezählt wird (und am Ende des Landens mehrere Updates nachgeholt werden)
};

#endif
