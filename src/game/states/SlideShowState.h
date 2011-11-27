/*
 * SlideShowState.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// SlideShowState ist für eine Bildshow zuständig (wie z.B. die Intro geschichte)
// Es werden Bilder und dazugehörenden Text angezeigt.

#ifndef SLIDESHOWSTATE_H
#define SLIDESHOWSTATE_H

#include <string>
#include <vector>

#include "game/GameState.h"

// Ein einzelner "Dia" / Slide
struct Slide
{
    std::vector<std::string> textPages;
    std::string imageFileName;
};

// Eine ganze "Dia-Show"
struct SlideShow
{
    int timerDelay; // 0 = kein Timer
    std::string musicFileName;
    std::vector<Slide> slides;
};

//--------------------------------------------//
//--------- SlideShowState Klasse ------------//
//--------------------------------------------//
class SlideShowState : public GameState
{
public:
    SlideShowState( SubSystems& subSystems, const std::string& slideXmlFile );

    const GameStateId& getId() const { return STATE_ID; }

	void init();        // State starten
	void cleanup();     // State abbrechen

	void pause();       // State anhalten
	void resume();      // State wiederaufnehmen
    void frame( float deltaTime );

	void update();      // Spiel aktualisieren
	void draw( float accumulator );        // Spiel zeichnen
private:
	void loadSlideShow();

    static const GameStateId STATE_ID;

    // Slideshow Informationen
    const std::string m_slideDataFile;
    unsigned int m_currentSlide;
    unsigned int m_currentTextPage;
    SlideShow m_slideShow;
    float m_overlayAlpha;
    bool m_fadeOut;
    bool m_goBack;

    float m_imageCornerOffsetX[4];
    float m_imageCornerOffsetY[4];
    float m_imageCornerOffsetMasterX;
    float m_imageCornerOffsetMasterY;

    unsigned int m_dispCharCount; // Anzahl zu zeichnende Buchstaben vom Text, das momentan angezeigt wird

    unsigned int m_textUpdateCounter;
    bool m_nextKeyDownOld;
    bool m_backKeyDownOld;

    float m_imageCornerOffsetXTarget[4];
    float m_imageCornerOffsetYTarget[4];
    float m_imageCornerOffsetXDir[4];
    float m_imageCornerOffsetYDir[4];

};

#endif
