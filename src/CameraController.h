/*
 * Camera.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */
 
// Steuert und rechnet Kameraverschiebungen

#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include <boost/shared_ptr.hpp>

class InputSubSystem;
class RenderSubSystem;
class GameWorld;

#include "Vector2D.h"

//----------//
//  Camera  //
//----------//

/*
    Dieses Klass bedient die Kamera (OpenGL-Ansicht).
    Die Methoden dieser Klasse werden genutzt um Kameraeinstellungen wie Zoom oder Lage zu ändern.
    Update() muss bei jedem Frame aufgerufen werden. (oder nur Look())
*/
class CameraController
{
public:
    CameraController( const InputSubSystem& inputSubSystem, RenderSubSystem& renderSubSystem, const GameWorld& world );

    // Initialisierung
    void init();

    void update( float deltaTime ); // Kamarabewegung aktualisieren und auf Eingabe, die die Kamera beeinflusst, reagieren
    void look() const; // OpenGL-Ansicht aktualisieren (wird von Update() automatisch aufgerufen)

    // Methoden um die Kamera zu bewegen:
    // timeToArrive ist die Zeit, die benötigt werden soll, damit die Kamera den Zielort erreicht
    void moveRelative( const Vector2D& rMove, float timeToArrive ); // rMove ist Position des Zielortes relativ zur Lage der Kamera
    void moveAbsolute( const Vector2D& rPos, float timeToArrive ); // rPos ist absolute Position des Zielortes

    // Um heran- oder herauszuzoomen
    void zoom( float zoom ); // zoom ist der Zoomfaktor ( < 1 bedeutet herauszoomen; > 1 bedeutet herazoomen )
    void setZoom( float zoom ); // zoom ist der Zoomfaktor ( < 1 bedeutet herauszoomen; > 1 bedeutet herazoomen )
    bool setFollowPlayer( bool follow ); // soll die Kamera den Spieles folgen oder nicht?

    void rotateAbsolute( float angle, float timeToArrive ); // Den Winkel der Kamera setzen. angle als neuer Winkel nehmen.
    void rotateRelative( float angle, float timeToArrive ); // Den Winkel der Kamera verändern. Um angle drehen. (+:Gegenuhrzegersinn, -:Uhrzeigersinn)

    Vector2D screenToWorld( const Vector2D& screenPos );
    Vector2D worldToScreen( const Vector2D& worldPos );

    // ========= Zugriff ==========
    const Vector2D& getCameraPos() const;
    float getCameraAngle() const;
    float getViewWidth() const // wie weit ist die Strecke vom linken Bildschirmrand zum Rechten?
    {
        return m_viewWidth;
    }
    float getViewHeight() const // oben - unten
    {
        return m_viewHeight;
    }

private:
    Vector2D m_position;
    float m_zoom;
    float m_rotation;
    float m_rotationVel;

    float m_viewWidth;
    float m_viewHeight;

    const InputSubSystem& m_inputSubSystem;
    RenderSubSystem& m_renderSubSystem;
    const GameWorld& m_world;

    bool m_isFollowingPlayer;
	float m_timeSinceLastSwitchHeading;
    char m_playerHeading; // -1: left, 0: not yet defined, 1: right

    // Kameratranslation
    bool m_isMovingSmoothly;
    Vector2D m_cameraDeparture;
    Vector2D m_cameraDestination;
    float m_moveTimeElapsed;
    float m_moveTotalTimeToArrive;

    // Kameradrehung
    bool m_isRotatingSmoothly;
    float m_startingAngle;
    float m_endingAngle;
    float m_rotateTimeElapsed;
    float m_rotateTotalTimeToArrive;
};

#endif
