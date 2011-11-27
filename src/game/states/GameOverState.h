/*
 * GameOverState.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Wenn Spiel vorbei ist.

#ifndef MAINMENUSTATE_H
#define GAMEOVERSTATE_H

#include <string>
#include "game/GameState.h"

const float cBackgroundColor[3] = { 0.3f, 0.1f, 0.2f };

//--------------------------------------------//
//---------- GameOverState Klasse ------------//
//--------------------------------------------//
class GameOverState : public GameState
{
public:
    GameOverState( SubSystems& subSystems, const std::string& stringToShow, const std::string& levelFileName );

    const GameStateId& getId() const { return STATE_ID; }

	void init();        // State starten
	void cleanup();     // State abbrechen

	void pause();       // State anhalten
	void resume();      // State wiederaufnehmen
    void frame( float deltaTime );

	void update();      // Spiel aktualisieren
	void draw( float accumulator );        // Spiel zeichnen
private:
    static const GameStateId STATE_ID;

    std::string m_stringToShow;
    std::string m_levelFileName;
};

#endif
