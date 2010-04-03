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

// eindeutige ID
StateIdType GameOverState::stateId = "GameOverState";

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
    GetSubSystems()->renderer->MatrixGUI();
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
        boost::shared_ptr<PlayingState> playState ( new PlayingState( GetSubSystems(), levelName.c_str() ) ); // Zum Spiel-Stadium wechseln
        GetSubSystems()->stateManager->ChangeState( playState );
        return;
    }
}

#define NOT_USED(x) x
void GameOverState::Frame( float deltaTime )
{
    NOT_USED(deltaTime);
    GetSubSystems()->input->Update(); // neue Eingaben lesen
}

void GameOverState::Draw( float accumulator )        // Spiel zeichnen
{
    NOT_USED(accumulator);
    RenderSubSystem* pRenderer = GetSubSystems()->renderer.get();

    // Bildschirm leeren
    pRenderer->ClearScreen();

    pRenderer->DrawOverlay( 0.3f, 0.0f, 0.0f, 1.0f );
    pRenderer->DrawString( m_stringToShow.c_str(), 1.5f, 1.40f, "FontW_b" );

    pRenderer->DrawString( "ENTER: erneut starten   ESC: abbrechen", 1.45f, 2.92f, "FontW_s" );

    pRenderer->FlipBuffer(); // vom Backbuffer zum Frontbuffer wechseln (neues Bild zeigen)
}

// Astro Attack - Christian Zommerfelds - 2009
