/*
 * GameOverState.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// GameOverState.h für mehr Informationen

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "GameOverState.h"
#include "PlayingState.h"
#include "../Renderer.h"
#include "../GameApp.h"
#include "../Input.h"

#include <boost/make_shared.hpp>

// eindeutige ID
const StateIdType GameOverState::stateId = "GameOverState";

GameOverState::GameOverState( SubSystems& subSystems, std::string stringToShow, std::string levelFileName )
: GameState( subSystems ),
  m_stringToShow ( stringToShow ),
  m_levelFileName ( levelFileName )
{
}

void GameOverState::init()        // State starten
{
    // GUI modus (Graphical User Interface)
    getSubSystems().renderer.setMatrix(RenderSubSystem::GUI);
}

void GameOverState::cleanup()     // State abbrechen
{
}

void GameOverState::pause()       // State anhalten
{
}

void GameOverState::resume()      // State wiederaufnehmen
{
}

void GameOverState::update()      // Spiel aktualisieren
{
    if ( getSubSystems().input.getKeyState( Enter ) )
    {
        boost::shared_ptr<PlayingState> playState (new PlayingState(getSubSystems(), m_levelFileName)); // Zum Spiel-Stadium wechseln
        getSubSystems().stateManager.changeState( playState );
        return;
    }
}

void GameOverState::frame( float /*deltaTime*/ )
{
    getSubSystems().input.update(); // neue Eingaben lesen
}

void GameOverState::draw( float /*accumulator*/ )        // Spiel zeichnen
{
    RenderSubSystem& renderer = getSubSystems().renderer;

    renderer.drawOverlay( cBackgroundColor[0], cBackgroundColor[1], cBackgroundColor[2], 1.0f );
    renderer.drawString( m_stringToShow.c_str(), "FontW_b", 2.0f, 1.5f, AlignCenter, AlignCenter );

    renderer.drawString( "ENTER: erneut starten   ESC: abbrechen", "FontW_m", 2.0f, 3.0f, AlignCenter, AlignBottom );
}
