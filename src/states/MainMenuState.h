/*----------------------------------------------------------\
|                     MainMenuState.h                       |
|                     ---------------                       |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// Das MainMenuState ist aktiv wenn der Spieler sich beim Hauptmenü befindet.
// Hier werden alle nötigen Aktionen im Menü durchgeführt.

#ifndef MAINMENUSTATE_H
#define MAINMENUSTATE_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "../GameStates.h"

#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>

struct Button;

enum SubMenu 
{
    Main,
    Play,
    Credits,
    Options
};

//--------------------------------------------//
//---------- MainMenuState Klasse ------------//
//--------------------------------------------//
class MainMenuState : public GameState
{
public:
    MainMenuState( SubSystems* pSubSystems, SubMenu startingSubMenu = Main );
    ~MainMenuState();

    const StateIdType& StateID() const { return stateId; }

	void Init();        // State starten
	void Cleanup();     // State abbrechen

	void Pause();       // State anhalten
	void Resume();      // State wiederaufnehmen
    void Frame( float deltaTime );

	void Update();      // Spiel aktualisieren
	void Draw( float accumulator ); // Spiel zeichnen
private:
    static const StateIdType stateId;

    // --- Callbacks für GUI ---
    void CallbackButPlay();
    void CallbackButEditor();
    void CallbackButCredits();
    void CallbackButOptions();
    void CallbackButExit();
    void CallbackButBack();
    void CallbackOpenLevel( std::string filename );
    void CallbackOpenSlideShow( std::string filename );
    void CallbackSound();
    void CallbackResolution( int w, int h );
    void CallbackButApplyConfig();
    void CallbackButDiscardConfig();

    // Bannerinformationen
    float m_titleVertexOffsetXTarget[4];
    float m_titleVertexOffsetYTarget[4];
    float m_titleVertexOffsetXDir[4];
    float m_titleVertexOffsetYDir[4];
    float m_titleVertexCoordOffset[8];
    float m_titleIntensityAngle;

    SubMenu m_subMenu;

    bool m_wantToQuit;              // ob Spiel beendet werden soll
    bool m_goToEditor;              // ob der Editor geöffnet werden soll
    bool m_goToPlay;                // ob ein Level gestartet werden soll
    bool m_goToSlideShow;           // ob eine Bildershow gestartet werden soll
    std::string m_fileNameToOpen;   // wie heisst die Datei, die geöffnet werden soll (level oder slide show)
    bool m_appliedConfig;           // ob Spieler die Einstellungen angenommen hat ()
};
//--------------------------------------------//
//------- Ende MainMenuState Klasse ----------//
//--------------------------------------------//

#endif

// Astro Attack - Christian Zommerfelds - 2009
