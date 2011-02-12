/*----------------------------------------------------------\
|                     GameOverState.h                       |
|                     ---------------                       |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// Wenn Spiel vorbei ist.

#ifndef MAINMENUSTATE_H
#define MAINMENUSTATE_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "../GameStates.h"
#include <string>

const float cBackgroundColor[3] = { 0.3f, 0.1f, 0.2f };

//--------------------------------------------//
//---------- GameOverState Klasse ------------//
//--------------------------------------------//
class GameOverState : public GameState
{
public:
    GameOverState( SubSystems* pSubSystems, std::string stringToShow, std::string levelFileName );
    ~GameOverState();

    const StateIdType& StateID() const { return stateId; }

	void Init();        // State starten
	void Cleanup();     // State abbrechen

	void Pause();       // State anhalten
	void Resume();      // State wiederaufnehmen
    void Frame( float deltaTime );

	void Update();      // Spiel aktualisieren
	void Draw( float accumulator );        // Spiel zeichnen
private:
    static const StateIdType stateId;

    std::string m_stringToShow;
    std::string m_levelFileName;
};
//--------------------------------------------//
//------- Ende GameOverState Klasse ----------//
//--------------------------------------------//

#endif

// Astro Attack - Christian Zommerfelds - 2009
