/*
 * CompVisualAnimation.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// CompVisualAnimation ist eine Komponente für Animationen.
// Jede Einheit (Entity), die eine Animation hat, hat eine CompVisualAnimation.
// Das Speicher allerdings nur die Informationen der Animation,
// das Zeichnen wird von Rederer übernomen.

#ifndef COMPVISUALANIMATION_H
#define COMPVISUALANIMATION_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "../Component.h"
#include "../Texture.h"
#include "../GameEvents.h"

#include "../Vector2D.h"
namespace pugi { class xml_node; }

//--------------------------------------------//
//------ CompVisualAnimation Klasse ----------//
//--------------------------------------------//
class CompVisualAnimation : public Component
{
public:
    CompVisualAnimation( const AnimInfo* pAnimInfo );

    const CompIdType& ComponentId() const { return COMPONENT_ID; }
	static const CompIdType COMPONENT_ID;

    // Welche Textur muss rerade gezeichnet werden? (Eine Animation besteht aus mehreren Texturen)
    TextureIdType GetCurrentTexture() const;

    // Die Mitte der Animation (relativ zur Position der Einheit)
    const Vector2D& Center() { return m_center; }

    // Dimensionen (Grösse)
    void SetDimensions( float hw, float hh ) { m_halfWidth = hw; m_halfHeight = hh; }
    void GetDimensions( float *hw, float *hh ) { *hw = m_halfWidth; *hh = m_halfHeight; }

    // Animation steuern
    void Start();                                              // Starten
    void Continue();                                           // Fortfahren
    void End() { m_running = false; }                          // Sofort enden
    void Finish() { m_wantToFinish = true; }                   // Enden wenn ein Stopppunkt erreicht wurde
    void SetDirection(int dir) { if (dir>0) m_direction=1; else m_direction= -1; } // 1 ist vorwärz und 0 ist rückwärz
    int IsRunning() const { return m_running; }                // ob die Animation gerade läuft

    // Animationsstand (z.B. Rennen, Springen, Reden )
    void SetState( StateIdType new_state );
    StateIdType GetState() { return m_curState; }    

    const AnimInfo* GetAnimInfo() const { return m_animInfo; }    

    // Ausrichtung ( wenn flip=true wird die Animation eifach horizontal gespiegelt )
    void SetFlip( bool flip ) { m_flip = flip; }
    bool GetFlip() { return m_flip; }

    static boost::shared_ptr<CompVisualAnimation> LoadFromXml(const pugi::xml_node& compElem, const AnimationManager& animMngr);
    void WriteToXml(pugi::xml_node& compNode) const;

private:

    // Animation Aktualisieren
    void OnUpdate();

    EventConnection m_eventConnection;

    // für Anzeige
    Vector2D m_center;
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
