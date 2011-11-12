/*
 * EditorState.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Level-Editor

#ifndef EDITORSTATE_H
#define EDITORSTATE_H

#include <string>

#include "common/GameState.h"
#include "common/World.h"
#include "game/CameraController.h"
#include "common/Vector2D.h"

//--------------------------------------------//
//------------ EditorState Klasse ------------//
//--------------------------------------------//
class EditorState : public GameState
{
public:
    EditorState( SubSystems& subSystems );

    const GameStateId& getId() const { return STATE_ID; }

	void init();                    // State starten
	void cleanup();                 // State abbrechen

	void pause();                   // State anhalten
	void resume();                  // State wiederaufnehmen

    void frame( float deltaTime );  // pro Frame
	void update();                  // Spiel aktualisieren
	void draw( float accumulator ); // Spiel zeichnen
private:
    static const GameStateId STATE_ID;

    World m_gameWorld;          // Spielwelt
    CameraController m_cameraController;        // Kamera

    Vector2D m_clickedPoints[8];
    int m_currentPoint;
    std::string m_currentTexture;
    unsigned int m_currentTextureNum;
    bool m_helpTextOn;

    bool m_mouseButDownOld;
    bool m_cancelVertexKeyDownOld;
    bool m_createEntityKeyDownOld;
    bool m_nextTextureKeyDownOld;
    bool m_prevTextureKeyDownOld;
    bool m_helpKeyDownOld;
};

#endif