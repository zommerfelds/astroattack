/*
 * Camera.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)


#include "CameraController.h"
#include "Input.h"
#include "World.h"
#include "Entity.h"
#include "Renderer.h"
#include "main.h"

//#include "components/CompPosition.h"
#include "components/CompPhysics.h"
#include "components/CompVisualAnimation.h"
#include "components/CompGravField.h"

#include <cmath>
#include <algorithm>
#include <boost/make_shared.hpp>

const float cMinZoom = 1.0f;
const float cMaxZoom = 9.0f;

// 16:10
const float STDViewWidth_8_5 = 26.4f; // horizontal lenght (units) of the view
const float STDViewHeight_8_5 = 16.5f; // vertical lenght (units) of the view

// 4:3
const float STDViewWidth_4_3 = 24.f; // horizontal lenght (units) of the view
const float STDViewHeight_4_3  = 18.f; // vertical lenght (units) of the view

// Number of frames to wait before player heading can change again
const float cTimeTillAnimSwitchHeadingIsPossible = 0.2f;

// Konstruktor
CameraController::CameraController( const InputSubSystem& inputSubSystem, RenderSubSystem& renderSubSystem, const GameWorld& world )
  : m_position (),
    m_zoom ( 1.0f ),
    m_rotation ( 0.0f ),
    m_viewWidth ( gAaConfig.getInt("WideScreen")?STDViewWidth_8_5:STDViewWidth_4_3 ),
    m_viewHeight ( gAaConfig.getInt("WideScreen")?STDViewHeight_8_5:STDViewHeight_4_3 ),
    m_inputSubSystem ( inputSubSystem ),
    m_renderSubSystem ( renderSubSystem ),
    m_world ( world ),
    m_isFollowingPlayer ( false ),
    m_timeSinceLastSwitchHeading ( 0 ),
    m_playerHeading ( 0 ),
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
{}

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

// Initialisierung der 2D Kamera (ganz am Anfang aufrufen)
void CameraController::init ()
{
    m_renderSubSystem.setViewSize( m_viewWidth, m_viewHeight );

    setZoom( 1.5f );
}

// Kamarabewegung aktualisieren und auf Eingabe, die die Kamera beeinflusst, reagieren
void CameraController::update ( float deltaTime ) // time_span in seconds
{
    // Zoom
    const float cZoomFactor = 4.0f;
    // Wenn Spieler + oder - gedrückt hat
    if ( m_inputSubSystem.getKeyState ( CameraZoomIn ) ) // not frame rate independent!
        zoom( pow(1/cZoomFactor, deltaTime) );
        //SetZoom(m_scaleValue - cZoomFactor * deltaTime);
    else if ( m_inputSubSystem.getKeyState ( CameraZoomOut ) )
        zoom( pow(cZoomFactor, deltaTime) );
        //SetZoom(m_scaleValue + cZoomFactor * deltaTime);

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

    m_timeSinceLastSwitchHeading += deltaTime;

    // Zuerst checken ob es einen Spielerobjekt überhaupt gibt.
    Entity* player = m_world.getEntity("Player");
    if ( player )
    {
        CompPhysics* playerPhys = player->getComponent<CompPhysics>();
        if ( playerPhys )
        {
            const Vector2D& playerPos = playerPhys->getSmoothCenterOfMassPosition(); // Position des Spielers

            if ( m_isFollowingPlayer )
            {
                // calculate cursor position relative to the middle of the screen
                Vector2D cursorPos = m_inputSubSystem.getMousePos();
                cursorPos.x = cursorPos.x - 0.5f;
                cursorPos.y = (cursorPos.y - 0.5f) * -1;
                cursorPos.rotate(m_rotation);

                // calculate new camera position
                // the position depends on the player position and on the mouse cursor position
                const float cPosFollowQuicknessFactor = 15.0f; // how fast the camera moves to the new position
                const float cRangeOfSightFactor = 6.0f; // how far the mouse can move the camera away from the player

                Vector2D posTargetDiff = playerPos + cursorPos * cRangeOfSightFactor - m_position;
                Vector2D posVelocity = posTargetDiff * cPosFollowQuicknessFactor;
                m_position += posVelocity * deltaTime;

                // calculate new camera zoom
                // the zoom depends on the mouse cursor position and on the players velocity
                const float cZoomFollowQuicknessFactor = 1.5f;
                const float cMaxFollowZoom = 2.5f;
                const float cMinFollowZoom = 1.34f;
                const float cGradientStart = 0.1f;
                const float cGradientEnd = 1.0f;
                const float cPlayerMaxVel = 5.0f;

                float zoomGradient = (cMaxFollowZoom - cMinFollowZoom) / (cGradientEnd - cGradientStart);
                float velFraction = playerPhys->getLinearVelocity().length() / cPlayerMaxVel;
                if (velFraction > 1.0f || playerPhys->getContacts().empty())
                    velFraction = 1.0f;
                float zoomParameter = cursorPos.length() + velFraction * 0.5f;
                float zoomTarget = 0.0f;
                if (zoomParameter < cGradientStart)
                    zoomTarget = cMaxFollowZoom;
                else if (zoomParameter > cGradientEnd)
                    zoomTarget = cMinFollowZoom;//cMaxFollowZoom - (cGradientEnd - cGradientStart) * cZoomGradient;
                else
                    zoomTarget = cMaxFollowZoom - (zoomParameter - cGradientStart) * zoomGradient;

                float zoomVelocity = (zoomTarget - m_zoom) * cZoomFollowQuicknessFactor;
                setZoom(m_zoom + zoomVelocity * deltaTime);
            }
            // flip player's heading according to cursor
            {
                std::vector<CompVisualAnimation*> player_anims = player->getComponents<CompVisualAnimation>();
                const CompGravField* grav = playerPhys->getActiveGravField();
                Vector2D upVector(0.0f,1.0f);
                if ( grav )
                    upVector = grav->getAcceleration( playerPhys->getCenterOfMassPosition() ).getUnitVector()*-1;
                bool right = upVector.isRight( screenToWorld(m_inputSubSystem.getMousePos()) - playerPhys->getCenterOfMassPosition() );
                if ( m_playerHeading == 0 || right != (m_playerHeading==1) )
                {
                    if ( m_timeSinceLastSwitchHeading > cTimeTillAnimSwitchHeadingIsPossible )
                    {
                        m_playerHeading = right ? 1 : -1;
                        m_timeSinceLastSwitchHeading = 0;

                        for ( size_t i = 0; i < player_anims.size(); ++i )
                        {
                            CompVisualAnimation* anim = player_anims[i];
                            anim->setFlip( !right );
                        }
                    }
                }
            }
        }
    }

    if (!m_isFollowingPlayer)
    {
        // Wenn die Kamera den Spieler nicht folgt, kann man sie mit den Pfeiltasten bewegen.
        const float cScrollVelMag = 40.0f / m_zoom; // Verschiebungsgrösse
        Vector2D scrollVelocity;
        if ( m_inputSubSystem.getKeyState ( CameraUp ) )
            scrollVelocity.set(0, cScrollVelMag);
        else if ( m_inputSubSystem.getKeyState ( CameraDown ) )
            scrollVelocity.set(0, -cScrollVelMag);

        if ( m_inputSubSystem.getKeyState ( CameraLeft ) )
            scrollVelocity.set(-cScrollVelMag, 0);
        else if ( m_inputSubSystem.getKeyState ( CameraRight ) )
            scrollVelocity.set(cScrollVelMag, 0);

        moveRelative( scrollVelocity.rotated(m_rotation) * deltaTime, 0.0f );

        if ( m_inputSubSystem.getRMouseKeyState() )
        {
            moveAbsolute( screenToWorld(m_inputSubSystem.getMousePos()), 1.0f);
        }
    }

    const float cRotVelocity = 2.0f;
    if ( m_inputSubSystem.getKeyState ( CameraRotateCw ) )
    rotateRelative( -cRotVelocity * deltaTime, 0.0f);
    else if ( m_inputSubSystem.getKeyState ( CameraRotateCcw ) )
        rotateRelative( cRotVelocity * deltaTime, 0.0f);

    if ( m_inputSubSystem.getKeyState ( CameraResetAngle ) )
    {
        const float cRotResetVelocity = 2.0f;
        float angle = getCameraAngle();
        float angle2Pi = fabs(angle - 2*cPi);
        float time = std::min(angle, angle2Pi);
        time /= cRotResetVelocity;
        rotateAbsolute( 0.0f, time );
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
    if ( m_zoom > cMaxZoom )
        m_zoom = cMaxZoom;
    else if ( m_zoom < cMinZoom )
        m_zoom = cMinZoom;
    m_viewWidth = (gAaConfig.getInt("WideScreen")?STDViewWidth_8_5:STDViewWidth_4_3) * 1.0f/m_zoom;
    m_viewHeight = (gAaConfig.getInt("WideScreen")?STDViewHeight_8_5:STDViewHeight_4_3) * 1.0f/m_zoom;
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

bool CameraController::setFollowPlayer( bool follow )
{
    bool oldIsFollowingPlayer = m_isFollowingPlayer;
    m_isFollowingPlayer = follow;
    return oldIsFollowingPlayer; // alte Einstellung zurückgeben
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
