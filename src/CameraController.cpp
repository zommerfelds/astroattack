/*
 * Camera.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)


#include "CameraController.h"
#include "Input.h"
#include "World.h"
#include "Vector2D.h"
#include "Entity.h"
#include "Renderer.h"
#include "main.h"

//#include "components/CompPosition.h"
#include "components/CompPhysics.h"
#include "components/CompVisualAnimation.h"
#include "components/CompGravField.h"

#include <cmath>
#include <boost/make_shared.hpp>

const float cMinScale = 0.7f;
const float cMaxScale = 10.0f;

namespace
{
    float mod360(float x) // Modulo 360 -> immer positiv
    {
        float result = fmod(x,360);
        if ( result < 0 )
            result += 360;
        return result;
    }
}

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
  : m_pPosition ( new Vector2D ),
    m_scaleValue ( 1.0f ),
    m_rotationAngleDeg ( 0.0f ),
    m_viewWidth ( gAaConfig.GetInt("WideScreen")?STDViewWidth_8_5:STDViewWidth_4_3 ),
    m_viewHeight ( gAaConfig.GetInt("WideScreen")?STDViewHeight_8_5:STDViewHeight_4_3 ),
    m_inputSubSystem ( inputSubSystem ),
    m_renderSubSystem ( renderSubSystem ),
    m_world ( world ),
    m_isFollowingPlayer ( false ),
    m_timeSinceLastSwitchHeading ( 0 ),
    m_playerHeading ( 0 ),
    m_isMovingSmoothly ( false ),
    m_pCameraDeparture ( new Vector2D ),
    m_pCameraDestination ( new Vector2D ),
    m_moveTimeElapsed ( 0.0f ),
    m_moveTotalTimeToArrive ( 0.0f ),
    m_isRotatingSmoothly ( false ),
    m_startingAngle ( 0.0f ),
    m_endingAngle ( 0.0f ),
    m_rotateTimeElapsed ( 0.0f ),
    m_rotateTotalTimeToArrive ( 0.0f )
{}

CameraController::~CameraController() {}

// Initialisierung der 2D Kamera (ganz am Anfang aufrufen)
void CameraController::Init ()
{
    m_renderSubSystem.SetViewSize( m_viewWidth, m_viewHeight );
    /*m_renderSubSystem.SetMatrix(RenderSubSystem::World);
    glMatrixMode ( GL_PROJECTION );
    glLoadIdentity(); //Reset projection matrix

    // orthogonalen 2D-Rendermodus
    gluOrtho2D( -m_viewWidth/2, m_viewWidth/2, -m_viewHeight/2, m_viewHeight/2 ); // (z ist nicht wichtig)
    glMatrixMode ( GL_MODELVIEW );*/

    m_isFollowingPlayer = false;
    //Zoom( 1.25f );
    Zoom( 1.5f );
}

// Kamarabewegung aktualisieren und auf Eingabe, die die Kamera beeinflusst, reagieren
void CameraController::Update ( float deltaTime ) // time_span in seconds
{
    // Zoom
    const float Zoom_Amount = 3.0;
    // Wenn Spieler + oder - gedrückt hat
    if ( m_inputSubSystem.KeyState ( CameraZoomIn ) )
        Zoom ( 1.0f + Zoom_Amount * deltaTime );
    else if ( m_inputSubSystem.KeyState ( CameraZoomOut ) )
        Zoom ( 1.0f - Zoom_Amount * deltaTime );

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
        Vector2D new_pos = *m_pCameraDeparture + ( *m_pCameraDestination-*m_pCameraDeparture ) *t;

        MoveAbsolute( new_pos, 0.0f ); // Kameraposition ein wenig gegen richtung Ziel verschieben
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

        RotateAbsolute ( new_angle, 0.0f ); // Kameraposition ein wenig gegen Zielwinkel drehen
    }

    m_timeSinceLastSwitchHeading += deltaTime;

    // Zuerst checken ob es einen Spielerobjekt überhaupt gibt.
    Entity* player = m_world.GetEntity("Player");
    if ( player )
    {
        CompPhysics* playerPhys = player->GetComponent<CompPhysics>();
        if ( playerPhys )
        {
            const Vector2D& playerPos = playerPhys->GetSmoothPosition(); // Position des Spielers

            if ( m_isFollowingPlayer )
            {
                // Blickpunkt der Kamera festsetzten (in der Mitte zwischen Spieler und Cursor)

                const float cFollowQuicknessFactor = 15.0f;
                const float cRangeOfSightFactor = 6.0f;
                Vector2D cursorPos = m_inputSubSystem.GetMousePos();
                cursorPos.x = cursorPos.x - 0.5f;
                cursorPos.y = (cursorPos.y - 0.5f) * -1;
                Vector2D targetPos = playerPos + cursorPos * cRangeOfSightFactor;
                Vector2D velocity = (targetPos - *m_pPosition) * cFollowQuicknessFactor;
                *m_pPosition += velocity * deltaTime;
            }
            // flip player's heading according to cursor
            {
                std::vector<CompVisualAnimation*> player_anims = player->GetComponents<CompVisualAnimation>();
                const CompGravField* grav = playerPhys->GetActiveGravField();
                Vector2D upVector(0.0f,1.0f);
                if ( grav )
                    upVector = grav->GetAcceleration( playerPhys->GetCenterOfMassPosition() ).GetUnitVector()*-1;
                bool right = upVector.IsRight( ScreenToWorld(m_inputSubSystem.GetMousePos()) - playerPhys->GetCenterOfMassPosition() );
                if ( m_playerHeading == 0 || right != (m_playerHeading==1) )
                {
                    if ( m_timeSinceLastSwitchHeading > cTimeTillAnimSwitchHeadingIsPossible )
                    {
                        m_playerHeading = right ? 1 : -1;
                        m_timeSinceLastSwitchHeading = 0;

                        for ( size_t i = 0; i < player_anims.size(); ++i )
                        {
                            CompVisualAnimation* anim = player_anims[i];
                            anim->SetFlip( !right );
                        }
                    }
                }
            }
        }
    }

    if (!m_isFollowingPlayer)
    {
        // Wenn die Kamera den Spieler nicht folgt, kann man sie mit den Pfeiltasten bewegen.
        const float scroll_Amount = 40.0f/m_scaleValue; // Verschiebungsgrösse
        Vector2D temp_vector;
        if ( m_inputSubSystem.KeyState ( CameraUp ) )
        {
            temp_vector = Vector2D( 0,scroll_Amount );
            temp_vector.Rotate( degToRad(m_rotationAngleDeg) );
            MoveRelative ( temp_vector * deltaTime, 0.0f );
        }
        else if ( m_inputSubSystem.KeyState ( CameraDown ) )
        {
            temp_vector = Vector2D( 0,-scroll_Amount );
            temp_vector.Rotate( degToRad(m_rotationAngleDeg) );
            MoveRelative ( temp_vector * deltaTime, 0.0f);
        }
        if ( m_inputSubSystem.KeyState ( CameraLeft ) )
        {
            temp_vector = Vector2D( -scroll_Amount,0 );
            temp_vector.Rotate( degToRad(m_rotationAngleDeg) );
            MoveRelative ( temp_vector * deltaTime, 0.0f);
        }
        else if ( m_inputSubSystem.KeyState ( CameraRight ) )
        {
            temp_vector = Vector2D( scroll_Amount,0 );
            temp_vector.Rotate( degToRad(m_rotationAngleDeg) );
            MoveRelative ( temp_vector * deltaTime, 0.0f);
        }

        if ( m_inputSubSystem.RMouseKeyState() )
        {
            MoveAbsolute ( ScreenToWorld(m_inputSubSystem.GetMousePos()), 1.0f);
        }
    }

    const float rotate_Amount = 130.0; // Drehstärke
    if ( m_inputSubSystem.KeyState ( CameraRotateCw ) )
        RotateRelative( -rotate_Amount * deltaTime, 0.0f);
    else if ( m_inputSubSystem.KeyState ( CameraRotateCcw ) )
        RotateRelative( rotate_Amount * deltaTime, 0.0f);
    if ( m_inputSubSystem.KeyState ( CameraResetAngle ) )
    {
        float time = GetCameraAngle();
        float x = fabs(time-360);
        if ( time > x )
            time = x;
        time *= 0.01f;
        RotateAbsolute( 0.0f, time );
    }
}

void CameraController::MoveRelative ( const Vector2D& rMove, float timeToArrive )
{
    if ( timeToArrive > 0.0f)
    {
        m_isMovingSmoothly = true;
        *m_pCameraDeparture = *m_pPosition;
        *m_pCameraDestination = rMove + *m_pCameraDeparture;
        m_moveTotalTimeToArrive = timeToArrive;
        m_moveTimeElapsed = 0.0;
    }
    else
	{
		//UpdateCurPosInWorld();
        *m_pPosition += rMove;
	}
}

void CameraController::MoveAbsolute ( const Vector2D& rPos, float timeToArrive )
{
    if ( timeToArrive > 0.0f)
    {
        m_isMovingSmoothly = true;
        *m_pCameraDeparture = *m_pPosition;
        *m_pCameraDestination = rPos;
        m_moveTotalTimeToArrive = timeToArrive;
        m_moveTimeElapsed = 0.0;
    }
    else
	{
		//UpdateCurPosInWorld();
        *m_pPosition = rPos;
	}
}

void CameraController::Zoom ( float zoom )
{
    m_scaleValue *= zoom;
    if ( m_scaleValue > cMaxScale )
        m_scaleValue = cMaxScale;
    else if ( m_scaleValue < cMinScale )
        m_scaleValue = cMinScale;
    m_viewWidth = (gAaConfig.GetInt("WideScreen")?STDViewWidth_8_5:STDViewWidth_4_3) * 1.0f/m_scaleValue;
    m_viewHeight = (gAaConfig.GetInt("WideScreen")?STDViewHeight_8_5:STDViewHeight_4_3) * 1.0f/m_scaleValue;
}

void CameraController::Look() const
{
    m_renderSubSystem.SetViewPosition(*m_pPosition, m_scaleValue, m_rotationAngleDeg);
}

const Vector2D& CameraController::GetCameraPos() const
{
    return *m_pPosition;
}

float CameraController::GetCameraAngle () const
{
    return mod360( m_rotationAngleDeg );
}

bool CameraController::SetFollowPlayer ( bool follow )
{
    bool oldIsFollowingPlayer = m_isFollowingPlayer;
    m_isFollowingPlayer = follow;
    return oldIsFollowingPlayer; // alte Einstellung zurückgeben
}

void CameraController::RotateAbsolute ( float angleDeg, float timeToArrive )
{
    if ( timeToArrive > 0.0f)
    {
        m_isRotatingSmoothly = true;
        m_startingAngle = mod360(m_rotationAngleDeg);
        m_endingAngle = angleDeg;
        if ( fabs(m_startingAngle - m_endingAngle) > fabs(m_startingAngle-360 - m_endingAngle) )
            m_startingAngle -= 360;
        m_rotateTotalTimeToArrive = timeToArrive;
        m_rotateTimeElapsed = 0.0;
    }
    else
	{
		m_rotationAngleDeg = mod360(angleDeg);
	}
}

void CameraController::RotateRelative ( float angleDeg, float timeToArrive )
{
    if ( timeToArrive > 0.0f)
    {
        m_isRotatingSmoothly = true;
        m_startingAngle = mod360(m_rotationAngleDeg);
        m_endingAngle = m_startingAngle + angleDeg;
        m_rotateTotalTimeToArrive = timeToArrive;
        m_rotateTimeElapsed = 0.0;
    }
    else
	{
		m_rotationAngleDeg += mod360(angleDeg);
        m_rotationAngleDeg = mod360(m_rotationAngleDeg);
	}
}

Vector2D CameraController::ScreenToWorld( const Vector2D& screenPos )
{
    Vector2D middle_of_screen_to_screenPos( ( screenPos.x - 0.5f ) * m_viewWidth,
                                           -( screenPos.y - 0.5f ) * m_viewHeight );
    middle_of_screen_to_screenPos.Rotate( degToRad(m_rotationAngleDeg) );

    return *m_pPosition + middle_of_screen_to_screenPos;
}

Vector2D CameraController::WorldToScreen( const Vector2D& worldPos )
{
    Vector2D screenPos ( (worldPos.x - m_pPosition->x),
                         (worldPos.y - m_pPosition->y) );
    screenPos.Rotate( -degToRad(m_rotationAngleDeg) );

    screenPos.x /= m_viewWidth;
    screenPos.y /= -m_viewHeight;

    screenPos.x += 0.5f;
    screenPos.y += 0.5f;

    return screenPos;
}
