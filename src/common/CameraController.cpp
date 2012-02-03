/*
 * Camera.cpp
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#include "CameraController.h"

#include "common/Renderer.h"

#include <cmath>
#include <cfloat>
#include <algorithm>
#include <boost/make_shared.hpp>

// Konstruktor
// w*h = 1
// w/h = AR
CameraController::CameraController(RenderSystem& renderSubSystem, float aspectRatio)
  : m_position (),
    m_zoom ( 1.0f ),
    m_rotation ( 0.0f ),
    m_rotationVel ( 0.0f ),
    m_minZoom ( FLT_MIN ),
    m_maxZoom ( FLT_MAX ),
    m_viewWidth ( 0.0f ),     // will be set later
    m_viewHeight ( 0.0f ),    // "
    m_refViewWidth ( 0.0f ),  // "
    m_refViewHeight ( 0.0f ), // "
    m_renderSubSystem ( renderSubSystem ),
    m_isMovingSmoothly ( false ),
    m_cameraDeparture (),
    m_cameraDestination (),
    m_moveTimeElapsed ( 0.0f ),
    m_moveTotalTimeToArrive ( 0.0f ),
    m_isRotatingSmoothly ( false ),
    m_startingAngle ( 0.0f ),
    m_endingAngle ( 0.0f ),
    m_rotateTimeElapsed ( 0.0f ),
    m_rotateTotalTimeToArrive ( 0.0f )
{
    setAspectRatio(aspectRatio);
}

namespace
{
    float mod2pi(float x)
    {
        float result = fmod(x, 2*cPi);
        if ( result < 0 )
            result += 2*cPi;
        return result;
    }
}

void CameraController::setAspectRatio(float ar) {
    m_viewWidth = sqrt(ar);
    m_viewHeight = 1/m_viewWidth;
    m_refViewWidth  = m_viewWidth;
    m_refViewHeight = m_viewHeight;
    m_renderSubSystem.setViewSize( m_viewWidth, m_viewHeight );
}

// Kamarabewegung aktualisieren und auf Eingabe, die die Kamera beeinflusst, reagieren
void CameraController::update ( float deltaTime ) // time_span in seconds
{
    // Falls die Kamera mitten in einer Bewegung ist, muss sie etwas gegen den Zielort verschoben werden
    if ( m_isMovingSmoothly )
    {
        m_moveTimeElapsed += deltaTime; // die verstrichene Zeit seit Bewegungsanfang wird erhöht
        float t = m_moveTimeElapsed/m_moveTotalTimeToArrive; // t ist ein Skalar und gibt den Bruchteil
        // des Weges an (oder der Zeit), den wir schon erreicht haben
        // t geht von 0 (Start) bis zu 1 (Ziel), falls wir schon über 1 sind, dann ist das Ziel schon erreicht.
        if ( t > 1.0f )
        {
            // Ziel erreicht
            m_isMovingSmoothly = false;
            t = 1.0;
        }

        // t [0-1] ist gleichmässig auf die Kamerastrecke verteillt (linear). D.h. konstante Geschwindigket:
        //
        //     s |    o Ziel
        //       |   /
        //       |  /                s = erreichte Strecke der Kamera
        //       | /                 t = Bruchteil der Gesamtzeit
        //       |/
        // Start o-----| t
        //       0     1
        //
        // Wir wollen aber eine ruhige Kamerafahrt, d.h. zuerst eine Beschleunigung und dann eine Abbremsung.
        // Doch anstatt mit aufwendigen Geschwindigkeiten und Beschleunigungen zu rechnen, verzerren wir einfach die t Variable,
        // so dass die oben gezeichnete Linie zu einer 'S'-Kurve wird, die am anfang wenig steil ist, bei t=0.5 am steilsten ist und
        // dann wieder abflacht:
        //
        //     s |                    o Ziel
        //       |                x
        //       |             x
        //       |           x                   s = erreichte Strecke der Kamera
        //       |          x                    t = Bruchteil der Gesamtzeit
        //       |         x
        //       |       x
        //       |    x
        // Start o--------------------| t
        //       0         0.5        1
        //
        // Die este Häfte der Kurve ist wie der Anfang einer einfachen exponentiellen Kurve (x^2) (von 0 bis 1).
        // Damit wir also t richtig hinbekommen, machen wir die este Häfte zu eine x^2 kurve.
        // In der ersten Hälfte geht t von 0 bis 0.5. Wir müssen t also provisorisch verdoppeln damit wir eine [0 bis 1] Kurve haben.
        // Dann ist die kurve jedoch doppelt so gross, deshalb noch durch 2 teilen. Am schluss:
        // Wenn t<0.5, dann wird t zu (t*2)^2/2 = 2*t^2
        //
        // In der zweiten Hälfte der Kurve gleich verzerren, jedoch dann die ergänzung zu 1 nehmen. (es ist wie eine gespiegelte x^2 Kurve)
        // Wir haben aber Zahlen über 0.5 und wollen sie wie vorher behandeln, also zuerst 1-t rechnen, dann sind sie wie in der erste Hälfte.
        // Dann das Gleiche tun wie vorher (2*t^2).
        // Dann die Ergänzung zu 1 rechnen (wieder 1-t). Am schluss gibt es:
        // Wenn t>0.5, dann wird t zu (1-t) dann zu (2*t^2) dann zu (1-t).
        //
        // Bei t=0.5 bleibt es gleich (ist Zentrum der Verzerrung).

        if ( t < 0.5 )
            t = t*t*2;
        if ( t > 0.5 )
        {
            t = 1-t;
            t = t*t*2;
            t = 1-t;
        }

        // Neue Position der Kamera berechnen
        // Geradengleichung: r = a + t*b      | a = Start, b = Zielrichtung, t = Skalar
        Vector2D new_pos = m_cameraDeparture + ( m_cameraDestination-m_cameraDeparture ) *t;

        moveAbsolute( new_pos, 0.0f ); // Kameraposition ein wenig gegen richtung Ziel verschieben
    }

    if ( m_isRotatingSmoothly )
    {
        m_rotateTimeElapsed += deltaTime; // die verstrichene Zeit seit Rotationsanfang wird erhöht
        float t = m_rotateTimeElapsed/m_rotateTotalTimeToArrive; // t ist ein Skalar und gibt den Bruchteil
        // der Rotation an (oder der Zeit), die wir schon erreicht haben
        // t geht von 0 (Start) bis zu 1 (Ziel), falls wir schon über 1 sind, dann ist das Ziel schon erreicht.
        if ( t > 1.0f )
        {
            // Ziel erreicht
            m_isRotatingSmoothly = false;
            t = 1.0;
        }
        if ( t < 0.5 )
            t = t*t*2; // Wie bei der Translation der Kamara, siehe kommentar oben.
        if ( t > 0.5 )
        {
            t = 1-t;
            t = t*t*2;
            t = 1-t;
        }
        // Neue Rotation der Kamera berechnen
        // r = s + t*e      | s = Startwinkel, z = Zielwinkel, t = Skalar
        float new_angle = m_startingAngle + ( m_endingAngle-m_startingAngle ) *t;

        rotateAbsolute ( new_angle, 0.0f ); // Kameraposition ein wenig gegen Zielwinkel drehen
    }
}

void CameraController::moveRelative( const Vector2D& rMove, float timeToArrive )
{
    if ( timeToArrive > 0.0f)
    {
        m_isMovingSmoothly = true;
        m_cameraDeparture = m_position;
        m_cameraDestination = rMove + m_cameraDeparture;
        m_moveTotalTimeToArrive = timeToArrive;
        m_moveTimeElapsed = 0.0;
    }
    else
    {
        m_position += rMove;
    }
}

void CameraController::moveAbsolute( const Vector2D& rPos, float timeToArrive )
{
    if ( timeToArrive > 0.0f)
    {
        m_isMovingSmoothly = true;
        m_cameraDeparture = m_position;
        m_cameraDestination = rPos;
        m_moveTotalTimeToArrive = timeToArrive;
        m_moveTimeElapsed = 0.0;
    }
    else
    {
        m_position = rPos;
    }
}

void CameraController::rotateAbsolute( float angle, float timeToArrive )
{
    if ( timeToArrive > 0.0f)
    {
        m_isRotatingSmoothly = true;
        m_startingAngle = mod2pi(m_rotation);
        m_endingAngle = angle;
        if ( fabs(m_startingAngle - m_endingAngle) > fabs(m_startingAngle-2*cPi - m_endingAngle) )
            m_startingAngle -= 2*cPi;
        m_rotateTotalTimeToArrive = timeToArrive;
        m_rotateTimeElapsed = 0.0;
    }
    else
    {
        m_rotation = mod2pi(angle);
    }
}

void CameraController::rotateRelative( float angle, float timeToArrive )
{
    if ( timeToArrive > 0.0f)
    {
        m_isRotatingSmoothly = true;
        m_startingAngle = mod2pi(m_rotation);
        m_endingAngle = m_startingAngle + angle;
        m_rotateTotalTimeToArrive = timeToArrive;
        m_rotateTimeElapsed = 0.0;
    }
    else
    {
        m_rotation = mod2pi(m_rotation + angle);
    }
}

void CameraController::zoom( float zoom )
{
    setZoom(m_zoom * zoom);
}

void CameraController::setZoom( float zoom )
{
    m_zoom = zoom;
    if ( m_zoom > m_maxZoom )
        m_zoom = m_maxZoom;
    else if ( m_zoom < m_minZoom )
        m_zoom = m_minZoom;
    m_viewWidth = m_refViewWidth * 1.0f/m_zoom;
    m_viewHeight = m_refViewHeight * 1.0f/m_zoom;
}

void CameraController::setZoomRange(float min, float max)
{
    m_minZoom = min;
    m_maxZoom = max;
}

void CameraController::look() const
{
    m_renderSubSystem.setViewPosition(m_position, m_zoom, m_rotation);
}

const Vector2D& CameraController::getCameraPos() const
{
    return m_position;
}

float CameraController::getCameraAngle() const
{
    return mod2pi(m_rotation);
}

Vector2D CameraController::screenToWorld( const Vector2D& screenPos )
{
    Vector2D middle_of_screen_to_screenPos( ( screenPos.x - 0.5f ) * m_viewWidth,
                                           -( screenPos.y - 0.5f ) * m_viewHeight );
    middle_of_screen_to_screenPos.rotate( m_rotation );

    return m_position + middle_of_screen_to_screenPos;
}

Vector2D CameraController::worldToScreen( const Vector2D& worldPos )
{
    Vector2D screenPos ( (worldPos.x - m_position.x),
                         (worldPos.y - m_position.y) );
    screenPos.rotate( -m_rotation );

    screenPos.x /= m_viewWidth;
    screenPos.y /= -m_viewHeight;

    screenPos.x += 0.5f;
    screenPos.y += 0.5f;

    return screenPos;
}
