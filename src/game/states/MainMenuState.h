/*
 * MainMenuState.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Das MainMenuState ist aktiv wenn der Spieler sich beim Hauptmenü befindet.
// Hier werden alle nötigen Aktionen im Menü durchgeführt.

#ifndef MAINMENUSTATE_H
#define MAINMENUSTATE_H

#include <vector>
#include <string>

#include "common/GameState.h"
#include "common/DataLoader.h"

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
    MainMenuState( SubSystems& subSystems, SubMenu startingSubMenu = Main );

    const GameStateId& getId() const { return STATE_ID; }

	void init();        // State starten
	void cleanup();     // State abbrechen

	void pause();       // State anhalten
	void resume();      // State wiederaufnehmen
    void frame(float deltaTime);

	void update();      // Spiel aktualisieren
	void draw(float accumulator); // Spiel zeichnen
private:
    static const GameStateId STATE_ID;

    // --- Callbacks für GUI ---
    void onPressedButPlay();
    void onPressedButEditor();
    void onPressedButCredits();
    void onPressedButOptions();
    void onPressedButExit();
    void onPressedButBack();
    void onPressedOpenLevel( const std::string& filename );
    void onPressedOpenSlideShow( const std::string& filename );
    void onPressedSound();
    void onPressedResolution( int w, int h );
    void onPressedButApplyConfig();
    void onPressedButDiscardConfig();

    // Bannerinformationen
    float m_titleIntensityPhase;

    SubMenu m_subMenu;

    bool m_wantToQuit;              // ob Spiel beendet werden soll
    bool m_goToEditor;              // ob der Editor geöffnet werden soll
    bool m_goToPlay;                // ob ein Level gestartet werden soll
    bool m_goToSlideShow;           // ob eine Bildershow gestartet werden soll
    std::string m_fileNameToOpen;   // wie heisst die Datei, die geöffnet werden soll (level oder slide show)
    int m_newWidth;
    int m_newHeight;

    ResourceIds m_menuResources;
};

#endif
