/*
 * PlayingState.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// PlayingState ist das häufigste State.
// Wenn der Spieler am Spielen ist, ist PlayingState aktiv.
// Alle nötigen Aktionen werden hier durchgeführt.

#ifndef PLAYINGSTATE_H
#define PLAYINGSTATE_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "../GameStates.h"
#include "../GameEvents.h"
#include <set>
#include <string>

#include "../World.h"
#include "../CameraController.h"
class Entity;
class EventConnection;

//--------------------------------------------//
//----------- PlayingState Klasse ------------//
//--------------------------------------------//
class PlayingState : public GameState
{
public:
    PlayingState( SubSystems& subSystems, std::string levelFileName );

    const StateIdType& getId() const { return stateId; }

	void init();        // State starten
	void cleanup();     // State abbrechen

	void pause();       // State anhalten
	void resume();      // State wiederaufnehmen
    void frame( float deltaTime );

	void update();      // Spiel aktualisieren
	void draw( float accumulator );        // Spiel zeichnen
private:
    static const StateIdType stateId;
    
    GameWorld m_gameWorld;          // Spielwelt
    CameraController m_cameraController;        // Kamera

    EventConnection m_eventConnection1;
    EventConnection m_eventConnection2;
    void onEntityDeleted( Entity& entity );
    void onLevelEnd(bool win, const std::string& msg);
    std::set< std::string > m_entitiesToDelete1;
    std::set< std::string > m_entitiesToDelete2;
    int m_curentDeleteSet;

    bool m_wantToEndGame;
    float m_alphaOverlay;
    std::string m_gameOverMessage;

    std::string m_levelFileName;

    bool m_showLoadingScreenAtCleanUp;
};

#endif
