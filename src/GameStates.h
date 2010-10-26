/*----------------------------------------------------------\
|                      GameStates.h                         |
|                      ------------                         |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// Basisklass für Game States

#ifndef GAMESTATES_H
#define GAMESTATES_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include <vector>
#include <boost/shared_ptr.hpp>
#include <string>

struct SubSystems;

typedef std::string StateIdType;

class GameState
{
public:
    GameState( SubSystems* pSubSystems );

    virtual const StateIdType& StateID() const = 0; // Komponente ID -> Name der Komponente

	virtual void Init() = 0;                    // State starten
	virtual void Cleanup() = 0;                 // State abbrechen

	virtual void Pause() = 0;                   // State anhalten
	virtual void Resume() = 0;                  // State wiederaufnehmen

	virtual void Update() = 0;                  // Spiel aktualisieren
    virtual void Frame( float deltaTime ) = 0;  // pro Frame einmal aufgerufen (am Anfang)
	virtual void Draw( float accumulator ) = 0; // Spiel zeichnen

    virtual ~GameState() {};                    // Destruktor

    SubSystems* GetSubSystems() { return m_pSubSystems; }
private:
    GameState() {}
    SubSystems* m_pSubSystems;
};

class StateManager
{
public:
    StateManager();
    ~StateManager();

    void ChangeState( const boost::shared_ptr<GameState>& pState );
	void PushState( const boost::shared_ptr<GameState>& pState );
	void PopState();

    GameState* GetCurrentState() { return m_states.back().get(); }
private:
    // Stack von States
    std::vector< boost::shared_ptr<GameState> > m_states;
};

#endif

// Astro Attack - Christian Zommerfelds - 2009
