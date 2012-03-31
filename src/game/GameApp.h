/*
 * GameApp.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#ifndef GAMEAPP_H
#define GAMEAPP_H

#include "Input.h"
#include "Gui.h"
#include "PlayerController.h"

#include "game/GameState.h"

#include "common/Event.h"
#include "common/Physics.h"
#include "common/Renderer.h"
#include "common/Sound.h"
#include "common/TriggerSystem.h"
#include "common/GameEvents.h"

#include <vector>
#include <string>

union SDL_Event;

// Alle wichtigen Untersysteme des Spieles
class SubSystems
{
public:
    SubSystems();
    ~SubSystems();
    bool init();
    void deInit();
    StateManager stateManager;   // States
    GameEvents events;           // Spielereignisse
    InputSubSystem input;        // Eingabe
    PhysicsSystem physics;       // Physik
    RenderSystem renderer;       // Ausgabe
    TriggerSystem triggerSys;
    SoundSystem sound;           // Sound
    GuiSystem gui;               // Grafische Benutzeroberfläche
    PlayerController playerController;
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

    bool doRestart(); // tells whether the app wants to be should be re-run after destruction

private:
    void init();    // Astro Attack initialisieren
    void deInit();  // Speicher wieder freigeben

    void mainLoop(); // Hauptschleife

    bool m_isInit;

    SubSystems m_subSystems; // Untersysteme

    bool m_quit; // Ob Programm beenden werden soll
    void onQuit(bool restart = false); // Spiel beenden (Wird von einem Event aufgerufen)

    EventConnection m_eventConnection;

    void updateGame();
    void handleSdlQuitEvents( SDL_Event& );
    void calcFPS( unsigned int curTime );

    unsigned int m_fpsMeasureStart;
    unsigned int m_framesCounter;
    unsigned int m_fps;

    std::string m_levelToLoad; // empty if shoud go to main menu
    bool m_startGame; // true if the game should be started (or false if only a message should be printed i.e. '--version')

    bool m_fullScreen;         // if the game should be started in full screen mode
    bool m_overrideFullScreen; // if the variable above should override the full screen option in the configuration file

    bool m_doRestart;

    // Initialisationsfunktionen in Init.cpp
    bool initSDL();
    bool initVideo();

    void parseArguments( const std::vector<std::string>& args ); // Programmargumente verarbeiten
};


#endif
