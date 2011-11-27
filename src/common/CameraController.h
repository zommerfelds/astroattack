/*
 * Camera.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */
 
// Steuert und rechnet Kameraverschiebungen

#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include <boost/shared_ptr.hpp>
#include "common/Vector2D.h"

class RenderSubSystem;
class ComponentManager;

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
    CameraController(RenderSubSystem& renderSubSystem, float aspectRatio);
    virtual ~CameraController() {};

    void update( float deltaTime ); // Kamarabewegung aktualisieren und auf Eingabe, die die Kamera beeinflusst, reagieren
    void look() const; // OpenGL-Ansicht aktualisieren (wird von Update() automatisch aufgerufen)

    void setAspectRatio(float);

    // Methoden um die Kamera zu bewegen:
    // timeToArrive ist die Zeit, die benötigt werden soll, damit die Kamera den Zielort erreicht
    void moveRelative( const Vector2D& rMove, float timeToArrive = 0.0f ); // rMove ist Position des Zielortes relativ zur Lage der Kamera
    void moveAbsolute( const Vector2D& rPos, float timeToArrive = 0.0f ); // rPos ist absolute Position des Zielortes

    // Um heran- oder herauszuzoomen
    void zoom( float zoom ); // zoom ist der Zoomfaktor ( < 1 bedeutet herauszoomen; > 1 bedeutet herazoomen )
    void setZoom( float zoom ); // zoom ist der Zoomfaktor ( < 1 bedeutet herauszoomen; > 1 bedeutet herazoomen )
    float getZoom() const {
    	return m_zoom;
    }

    void setZoomRange(float min, float max);

    void rotateAbsolute( float angle, float timeToArrive = 0.0f ); // Den Winkel der Kamera setzen. angle als neuer Winkel nehmen.
    void rotateRelative( float angle, float timeToArrive = 0.0f ); // Den Winkel der Kamera verändern. Um angle drehen. (+:Gegenuhrzegersinn, -:Uhrzeigersinn)

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

    float m_minZoom;
    float m_maxZoom;

    float m_viewWidth;
    float m_viewHeight;
    float m_refViewWidth;
    float m_refViewHeight;

    RenderSubSystem& m_renderSubSystem;

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
