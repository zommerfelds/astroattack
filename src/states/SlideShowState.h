/*----------------------------------------------------------\
|                    SlideShowState.h                       |
|                    ----------------                       |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// SlideShowState ist für eine Bildshow zuständig (wie z.B. die Intro geschichte)
// Es werden Bilder und dazugehörenden Text angezeigt.

#ifndef SLIDESHOWSTATE_H
#define SLIDESHOWSTATE_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "../GameStates.h"

#include <string>
#include <vector>

// Ein einzelner "Dia" / Slide
struct Slide
{
    std::string text;
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
    SlideShowState( SubSystems* pSubSystems, std::string slideXmlFile );
    ~SlideShowState();

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

    // Slideshow Informationen
    std::string m_slideXmlFile;
    unsigned int m_currentSlide;
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
//--------------------------------------------//
//------- Ende SlideShowState Klasse ---------//
//--------------------------------------------//

#endif

// Astro Attack - Christian Zommerfelds - 2009
