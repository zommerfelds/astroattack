/*
 * EditorState.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Level-Editor

#ifndef EDITORSTATE_H
#define EDITORSTATE_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "../GameStates.h"

#include <string>

#include "../World.h"
#include "../CameraController.h"
#include "../Vector2D.h"

//--------------------------------------------//
//------------ EditorState Klasse ------------//
//--------------------------------------------//
class EditorState : public GameState
{
public:
    EditorState( SubSystems& subSystems );

    const StateIdType& StateID() const { return stateId; }

	void Init();                    // State starten
	void Cleanup();                 // State abbrechen

	void Pause();                   // State anhalten
	void Resume();                  // State wiederaufnehmen

    void Frame( float deltaTime );  // pro Frame
	void Update();                  // Spiel aktualisieren
	void Draw( float accumulator ); // Spiel zeichnen
private:
    static const StateIdType stateId;

    GameWorld m_gameWorld;          // Spielwelt
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
//--------------------------------------------//
//--------- Ende EditorState Klasse ----------//
//--------------------------------------------//

#endif
