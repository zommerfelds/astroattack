/*----------------------------------------------------------\
|                     EditorState.h                         |
|                     -------------                         |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// Level-Editor

#ifndef EDITORSTATE_H
#define EDITORSTATE_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "../GameStates.h"

#include <boost/scoped_ptr.hpp>
#include <string>

class GameWorld;
class GameCamera;
class Vector2D;

//--------------------------------------------//
//------------ EditorState Klasse ------------//
//--------------------------------------------//
class EditorState : public GameState
{
public:
    EditorState( SubSystems* pSubSystems );
    ~EditorState();

    const StateIdType& StateID() const { return stateId; }

	void Init();                    // State starten
	void Cleanup();                 // State abbrechen

	void Pause();                   // State anhalten
	void Resume();                  // State wiederaufnehmen

    void Frame( float deltaTime );  // pro Frame
	void Update();                  // Spiel aktualisieren
	void Draw( float accumulator ); // Spiel zeichnen
private:
    static StateIdType stateId;

    boost::scoped_ptr<GameWorld> m_pGameWorld;          // Spielwelt
    boost::scoped_ptr<GameCamera> m_pGameCamera;        // Kamera

    boost::scoped_ptr<Vector2D> m_pClickedPoints[8];
    int m_currentPoint;
    std::string m_currentTexture;
    unsigned int m_currentTextureNum;
    bool m_helpTextOn;

};
//--------------------------------------------//
//--------- Ende EditorState Klasse ----------//
//--------------------------------------------//

#endif

// Astro Attack - Christian Zommerfelds - 2009
