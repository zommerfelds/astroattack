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
#include "../GameEvents.h"
#include <boost/scoped_ptr.hpp>
#include <set>
#include <string>

class GameWorld;
class GameCamera;
class Entity;
class EventConnection;

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

    EventConnection m_eventConnection1;
    EventConnection m_eventConnection2;
    void OnEntityDeleted( Entity* pEntity );
    void OnLevelEnd(bool win, std::string msg);
    std::set< std::string > m_entitiesToDelete1;
    std::set< std::string > m_entitiesToDelete2;
    int m_curentDeleteSet;

    bool m_wantToEndGame;
    float m_alphaOverlay;
    std::string m_gameOverMessage;

    std::string m_levelFileName;

    bool m_showLoadingScreenAtCleanUp;
};
//--------------------------------------------//
//-------- Ende PlayingState Klasse ----------//
//--------------------------------------------//

#endif

// Astro Attack - Christian Zommerfelds - 2009
