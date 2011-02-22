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
#include <boost/scoped_ptr.hpp>

class InputSubSystem;
class RenderSubSystem;
class GameWorld;
class Vector2D;

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
    ~CameraController(); // need to implement destructor manually because of scoped_ptr (incomplete type)

    // Initialisierung
    void Init();

    void Update( float deltaTime ); // Kamarabewegung aktualisieren und auf Eingabe, die die Kamera beeinflusst, reagieren
    void Look() const; // OpenGL-Ansicht aktualisieren (wird von Update() automatisch aufgerufen)

    // Methoden um die Kamera zu bewegen:
    // timeToArrive ist die Zeit, die benötigt werden soll, damit die Kamera den Zielort erreicht
    void MoveRelative( const Vector2D& rMove, float timeToArrive ); // rMove ist Position des Zielortes relativ zur Lage der Kamera
    void MoveAbsolute( const Vector2D& rPos, float timeToArrive ); // rPos ist absolute Position des Zielortes

    // Um heran- oder herauszuzoomen
    void Zoom( float zoom ); // zoom ist der Zoomfaktor ( < 1 bedeutet herauszoomen; > 1 bedeutet herazoomen )
    void SetZoom( float zoom ); // zoom ist der Zoomfaktor ( < 1 bedeutet herauszoomen; > 1 bedeutet herazoomen )
    bool SetFollowPlayer( bool follow ); // soll die Kamera den Spieles folgen oder nicht?

    void RotateAbsolute( float angle, float timeToArrive ); // Den Winkel der Kamera setzen. angle als neuer Winkel nehmen.
    void RotateRelative( float angle, float timeToArrive ); // Den Winkel der Kamera verändern. Um angle drehen. (+:Gegenuhrzegersinn, -:Uhrzeigersinn)

    Vector2D ScreenToWorld( const Vector2D& screenPos );
    Vector2D WorldToScreen( const Vector2D& worldPos );

    // ========= Zugriff ==========
    const Vector2D& GetCameraPos() const;
    float GetCameraAngle() const;
    float GetViewWidth() const // wie weit ist die Strecke vom linken Bildschirmrand zum Rechten?
    {
        return m_viewWidth;
    }
    float GetViewHeight() const // oben - unten
    {
        return m_viewHeight;
    }

private:
    boost::scoped_ptr<Vector2D> m_pPosition;
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
    boost::scoped_ptr<Vector2D> m_pCameraDeparture;
    boost::scoped_ptr<Vector2D> m_pCameraDestination;
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
