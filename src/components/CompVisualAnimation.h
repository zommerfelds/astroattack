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

    const CompIdType& getComponentId() const { return COMPONENT_ID; }
	static const CompIdType COMPONENT_ID;

    // Welche Textur muss rerade gezeichnet werden? (Eine Animation besteht aus mehreren Texturen)
    TextureIdType getCurrentTexture() const;

    // Die Mitte der Animation (relativ zur Position der Einheit)
    const Vector2D& center() { return m_center; }

    // Dimensionen (Grösse)
    void setDimensions( float hw, float hh ) { m_halfWidth = hw; m_halfHeight = hh; }
    void getDimensions( float *hw, float *hh ) { *hw = m_halfWidth; *hh = m_halfHeight; }

    // Animation steuern
    void start();                                              // Starten
    void carryOn();                                           // Fortfahren
    void end() { m_running = false; }                          // Sofort enden
    void finish() { m_wantToFinish = true; }                   // Enden wenn ein Stopppunkt erreicht wurde
    void setDirection(int dir) { if (dir>0) m_direction=1; else m_direction= -1; } // 1 ist vorwärz und 0 ist rückwärz
    int isRunning() const { return m_running; }                // ob die Animation gerade läuft

    // Animationsstand (z.B. Rennen, Springen, Reden )
    void setState( StateIdType new_state );
    StateIdType getState() { return m_curState; }    

    const AnimInfo* getAnimInfo() const { return m_animInfo; }    

    // Ausrichtung ( wenn flip=true wird die Animation eifach horizontal gespiegelt )
    void setFlip( bool flip ) { m_flip = flip; }
    bool getFlip() { return m_flip; }

    static boost::shared_ptr<CompVisualAnimation> loadFromXml(const pugi::xml_node& compElem, const AnimationManager& animMngr);
    void writeToXml(pugi::xml_node& compNode) const;

private:

    // Animation Aktualisieren
    void onUpdate();

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

#endif
