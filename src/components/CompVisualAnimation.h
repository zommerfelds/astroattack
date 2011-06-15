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

#include "../Component.h"
#include "../Texture.h"
#include "../GameEvents.h"
#include "../Vector2D.h"

//--------------------------------------------//
//------ CompVisualAnimation Klasse ----------//
//--------------------------------------------//
class CompVisualAnimation : public Component
{
public:
    CompVisualAnimation(const ComponentIdType& id, GameEvents& gameEvents, const AnimationManager& animManager);

    const ComponentTypeId& getTypeId() const { return COMPONENT_TYPE_ID; }
	static const ComponentTypeId COMPONENT_TYPE_ID;

	void setAnim(const AnimationId& animInfoId);

    // Welche Textur muss rerade gezeichnet werden? (Eine Animation besteht aus mehreren Texturen)
    TextureId getCurrentTexture() const;

    // Die Mitte der Animation (relativ zur Position der Einheit)
    const Vector2D& center() { return m_center; }

    // Dimensionen (Grösse)
    void setDimensions( float hw, float hh ) { m_halfWidth = hw; m_halfHeight = hh; }
    void getDimensions( float *hw, float *hh ) { *hw = m_halfWidth; *hh = m_halfHeight; }

    // Animation steuern
    void start();                  // Starten
    void carryOn();                // Fortfahren
    void end();                    // Sofort enden
    void finish();                 // Enden wenn ein Stopppunkt erreicht wurde
    void setReverse(bool reverse);
    int isRunning() const;         // ob die Animation gerade läuft

    // Animationsstand (z.B. Rennen, Springen, Reden )
    void setState( AnimStateId new_state );
    AnimStateId getState() { return m_curState; }    

    const AnimInfo* getAnimInfo() const { return m_animInfo; }    

    // Ausrichtung ( wenn flip=true wird die Animation eifach horizontal gespiegelt )
    void setFlip( bool flip ) { m_flip = flip; }
    bool getFlip() { return m_flip; }

    void loadFromPropertyTree(const boost::property_tree::ptree& propTree);
    void writeToPropertyTree(boost::property_tree::ptree& propTree) const;

private:

    // Animation Aktualisieren
    void onUpdate();

    const AnimationManager& m_animManager;
    EventConnection m_eventConnection;

    // für Anzeige
    Vector2D m_center;
    float m_halfWidth;
    float m_halfHeight;

    // für Animation
    int m_currentFrame;
    int m_updateCounter;
    const AnimInfo* m_animInfo;
    AnimationId m_animInfoId;
    AnimStateId m_curState;
    bool m_running;
    bool m_wantToFinish;
    bool m_flip;
    int m_playDirection; // Animationsrichtung (vorwärz (1) oder rückwärz (-1))
};

#endif
