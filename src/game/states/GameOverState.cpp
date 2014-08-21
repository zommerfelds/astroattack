/*
 * GameOverState.cpp
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */

#include "GameOverState.h"

#include "PlayingState.h"

#include "game/GameApp.h"
#include "game/Input.h"

#include "common/Renderer.h"

#include <boost/make_shared.hpp>

// eindeutige ID
const GameStateId GameOverState::STATE_ID = "GameOverState";

GameOverState::GameOverState( SubSystems& subSystems, const std::string& stringToShow, const std::string& levelFileName )
: GameState( subSystems ),
  m_stringToShow ( stringToShow ),
  m_levelFileName ( levelFileName )
{
}

void GameOverState::init()        // State starten
{
    // GUI modus (Graphical User Interface)
    getSubSystems().renderer.setMatrix(RenderSystem::GUI);
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
    if ( getSubSystems().input.isKeyDown(Enter) )
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
    RenderSystem& renderer = getSubSystems().renderer;

    renderer.drawOverlay( cBackgroundColor[0], cBackgroundColor[1], cBackgroundColor[2], 1.0f );
    renderer.drawString( m_stringToShow, "FontW_b", 0.5f, 0.5f, AlignCenter, AlignCenter );

    renderer.drawString( "ENTER: erneut starten   ESC: abbrechen", "FontW_m", 0.5f, 1.0f, AlignCenter, AlignBottom );
}
