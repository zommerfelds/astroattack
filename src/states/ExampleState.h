/*----------------------------------------------------------\
|                      MainState.h                          |
|                      -----------                          |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2008                             |
\----------------------------------------------------------*/

#ifndef MAINSTATE_H
#define MAINSTATE_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "../GameStates.h"

class MainState : public GameState
{
public:
    ~MainState();

	void Init();        // State starten
	void Cleanup();     // State abbrechen

	void Pause();       // State anhalten
	void Resume();      // State wiederaufnehmen

    void Frame();       // pro Frame
	void Update();      // Spiel aktualisieren
	void Draw();        // Spiel zeichnen
};

#endif

// Astro Attack - Christian Zommerfelds - 2008
