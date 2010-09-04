/*----------------------------------------------------------\
|                  CompVisualAnimation.h                    |
|                  ---------------------                    |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// CompVisualAnimation ist eine Komponente für Animationen.
// Jede Einheit (Entity), die eine Animation hat, hat eine CompVisualAnimation.
// Das Speicher allerdings nur die Informationen der Animation,
// das Zeichnen wird von Rederer übernomen.

#ifndef COMPVISUALANIMATION_H
#define COMPVISUALANIMATION_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "../Component.h"
#include "../Texture.h"
#include "../EventManager.h"
#include <boost/scoped_ptr.hpp>

class Vector2D;

//--------------------------------------------//
//------ CompVisualAnimation Klasse ----------//
//--------------------------------------------//
class CompVisualAnimation : public Component
{
public:
    CompVisualAnimation( const AnimInfo* pAnimInfo );
    ~CompVisualAnimation();
	const CompIdType& ComponentId() const { return m_componentId; }

    // Welche Textur muss rerade gezeichnet werden? (Eine Animation besteht aus mehreren Texturen)
    TextureIdType GetCurrentTexture() const;

    // Die Mitte der Animation (relativ zur Position der Einheit)
    Vector2D* Center() { return m_center.get(); }

    // Dimensionen (Grösse)
    void SetDimensions( float hw, float hh ) { m_halfWidth = hw; m_halfHeight = hh; }
    void GetDimensions( float *hw, float *hh ) { *hw = m_halfWidth; *hh = m_halfHeight; }

    // Animation steuern
    void Start();                                              // Starten
    void Continue();                                           // Fortfahren
    void End() { m_running = false; }                          // Sofort enden
    void Finish() { m_wantToFinish = true; }                   // Enden wenn ein Stopppunkt erreicht wurde
    void SetDirection(int dir) { if (dir>0) m_direction=1; else m_direction= -1; } // 1 ist vorwärz und 0 ist rückwärz
    int IsRunning() { return m_running; }                      // ob die Animation gerade läuft

    // Animationsstand (z.B. Rennen, Springen, Reden )
    void SetState( StateIdType new_state );
    StateIdType GetState() { return m_curState; }    

    const AnimInfo* GetAnimInfo() const { return m_animInfo; }    

    // Ausrichtung ( wenn flip=true wird die Animation eifach horizontal gespiegelt )
    void SetFlip( bool flip ) { m_flip = flip; }
    bool GetFlip() { return m_flip; }
private:

    // Animation Aktualisieren
    void Update( const Event* inputUpdatedEvent );

	static CompIdType m_componentId;
    RegisterObj m_registerObj;

    // für Anzeige
    boost::scoped_ptr<Vector2D> m_center;
    float m_halfWidth;
    float m_halfHeight;

    // für Animation
    int m_currentFrame;
    int m_updateCounter;
    const AnimInfo * m_animInfo;
    StateIdType m_curState;
    bool m_running;
    bool m_wantToFinish;
    bool m_flip;
    int m_direction; // Animationsrichtung (vorwärz (1) oder rückwärz (-1))
};
//--------------------------------------------//
//---- Ende CompVisualAnimation Klasse -------//
//--------------------------------------------//

#endif

// Astro Attack - Christian Zommerfelds - 2009
