/*----------------------------------------------------------\
|                     PlayingState.h                        |
|                     --------------                        |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// PlayingState ist das häufigste State.
// Wenn der Spieler am Spielen ist, ist PlayingState aktiv.
// Alle nötigen Aktionen werden hier durchgeführt.

#ifndef PLAYINGSTATE_H
#define PLAYINGSTATE_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "../GameStates.h"
#include "../EventManager.h"
#include <boost/scoped_ptr.hpp>
#include <set>
#include <string>

class GameWorld;
class GameCamera;

// TEMP!
extern std::string levelName;

//--------------------------------------------//
//----------- PlayingState Klasse ------------//
//--------------------------------------------//
class PlayingState : public GameState
{
public:
    PlayingState( SubSystems* pSubSystems, std::string levelFileName );
    ~PlayingState();

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
    
    boost::scoped_ptr<GameWorld> m_pGameWorld;          // Spielwelt
    boost::scoped_ptr<GameCamera> m_pGameCamera;        // Kamera

    RegisterObj m_registerObj;
    RegisterObj m_registerObj2;
    void EntityDeleted( const Event* deletedEvent );
    void GameOver( const Event* endGameEvent );
    std::set< std::string > m_entitiesToDelete1;
    std::set< std::string > m_entitiesToDelete2;
    int m_curentDeleteSet;

    bool m_wantToEndGame;
    float m_alphaOverlay;
    std::string m_gameOverMessage;

    std::string m_levelFileName;
};
//--------------------------------------------//
//-------- Ende PlayingState Klasse ----------//
//--------------------------------------------//

#endif

// Astro Attack - Christian Zommerfelds - 2009
