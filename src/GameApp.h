/*
 * GameApp.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#ifndef GAMEAPP_H
#define GAMEAPP_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include <vector>
#include <string>

#include "Event.h"

#include "Input.h"
#include "Physics.h"
#include "Renderer.h"
#include "GameStates.h"
#include "Sound.h"
#include "Gui.h"
#include "GameEvents.h"
union SDL_Event;


// Alle wichtigen Untersysteme des Spieles
struct SubSystems
{
    SubSystems();
    ~SubSystems();
    bool init();
    void deInit();
    StateManager stateManager;   // States
    GameEvents events;           // Spielereignisse
    InputSubSystem input;        // Eingabe
    PhysicsSubSystem physics;    // Physik
    RenderSubSystem renderer;    // Ausgabe
    SoundSubSystem sound;        // Sound
    GuiSubSystem gui;            // Grafische Benutzeroberfläche
    bool isLoading; // wenn true: Zeitakkumulator wird neu gestartet, sobald die Hauptschleife einmal durch ist
                    // das verhindert, dass die Zeit währed Ladezeiten gezählt wird (und am Ende des Landens mehrere Updates nachgeholt werden)
};

/*
    Das ist die Hauptklasse des Spiels.
    Sie initialisiert, führt aus und deinitialisiert das Spiel.
*/
class GameApp
{
public:
    GameApp(const std::vector<std::string>& args);
    ~GameApp();

    void run();     // Spiel starten (nach der Initialisierung ), d.h. Hauptschleife starten

private:
    void init();    // Astro Attack initialisieren
    void deInit();  // Speicher wieder freigeben

    void mainLoop(); // Hauptschleife

    bool m_isInit;

    SubSystems m_subSystems; // Untersysteme

    bool m_quit; // Ob Programm beenden werden soll
    void onQuit() { m_quit = true; } // Spiel beenden (Wird von einem Event aufgerufen)

    EventConnection m_eventConnection; // TODO: scoped_ptr

    void updateGame();
    void handleSdlQuitEvents( SDL_Event&, bool& quit );
    void calcFPS( unsigned int curTime );

    unsigned int m_fpsMeasureStart;
    unsigned int m_framesCounter;
    unsigned int m_fps;

    std::string m_levelToLoad; // empty if shoud go to main menu
    bool m_startGame; // true if the game should be started (or false if only a message should be printed i.e. '--version')

    bool m_fullScreen;         // if the game should be started in full screen mode
    bool m_overRideFullScreen; // if the variable above should override the full screen option in the configuration file

    // Initialisationsfunktionen in Init.cpp
    bool initSDL();
    bool initVideo();

    void parseArguments( const std::vector<std::string>& args ); // Programmargumente verarbeiten
};


#endif
