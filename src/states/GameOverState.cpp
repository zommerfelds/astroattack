/*----------------------------------------------------------\
|                    GameOverState.cpp                      |
|                    -----------------                      |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
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

GameOverState::GameOverState( SubSystems* pSubSystems, std::string stringToShow )
: GameState( pSubSystems ),
  m_stringToShow ( stringToShow )
{
}

GameOverState::~GameOverState()
{
}

void GameOverState::Init()        // State starten
{
    // GUI modus (Graphical User Interface)
    GetSubSystems()->renderer->SetMatrix(RenderSubSystem::GUI);
}

void GameOverState::Cleanup()     // State abbrechen
{
}

void GameOverState::Pause()       // State anhalten
{
}

void GameOverState::Resume()      // State wiederaufnehmen
{
}

void GameOverState::Update()      // Spiel aktualisieren
{
    if ( GetSubSystems()->input->KeyState( Enter ) )
    {
        boost::shared_ptr<PlayingState> playState = boost::make_shared<PlayingState>(GetSubSystems(), levelName.c_str()); // Zum Spiel-Stadium wechseln
        GetSubSystems()->stateManager->ChangeState( playState );
        return;
    }
}

void GameOverState::Frame( float /*deltaTime*/ )
{
    GetSubSystems()->input->Update(); // neue Eingaben lesen
}

void GameOverState::Draw( float /*accumulator*/ )        // Spiel zeichnen
{
    RenderSubSystem* pRenderer = GetSubSystems()->renderer.get();

    // Bildschirm leeren
    pRenderer->ClearScreen();

    pRenderer->DrawOverlay( cBackgroundColor[0], cBackgroundColor[1], cBackgroundColor[2], 1.0f );
    pRenderer->DrawString( m_stringToShow.c_str(), "FontW_b", 2.0f, 1.5f, AlignCenter, AlignCenter );

    pRenderer->DrawString( "ENTER: erneut starten   ESC: abbrechen", "FontW_m", 2.0f, 3.0f, AlignCenter, AlignBottom );

    pRenderer->FlipBuffer(); // vom Backbuffer zum Frontbuffer wechseln (neues Bild zeigen)
}

// Astro Attack - Christian Zommerfelds - 2009
