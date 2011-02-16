/*
 * Camera.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

// OpenGL via SDL inkludieren (Plattform-übergreifende Definitionen)
#include "SDL_opengl.h"
#include "Camera.h"
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

#include <math.h>
#include <boost/make_shared.hpp>

#define CAMERA_POS (*m_pTranslation * -1)

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

// 8:5
const float STDViewWidth_8_5 = 26.4f; // horizontal lenght (units) of the view
const float STDViewHeight_8_5 = 16.5f; // vertical lenght (units) of the view

// 4:3
const float STDViewWidth_4_3 = 24.f; // horizontal lenght (units) of the view
const float STDViewHeight_4_3  = 18.f; // vertical lenght (units) of the view

// Number of frames to wait before player heading can change again
const int cFramesTillAnimSwitchHeadingIsPossible = 15;

// Konstruktor
GameCamera::GameCamera( const InputSubSystem* pInputSubSystem, RenderSubSystem* pRenderSubSystem, const GameWorld* pWorld )
  : m_pTranslation ( new Vector2D ),
    m_scaleValue ( 1.0f ),
    m_rotationAngleDeg ( 0.0f ),
    m_viewWidth ( gAaConfig.GetInt("WideScreen")?STDViewWidth_8_5:STDViewWidth_4_3 ),
    m_viewHeight ( gAaConfig.GetInt("WideScreen")?STDViewHeight_8_5:STDViewHeight_4_3 ),
    m_pInputSubSystem ( pInputSubSystem ),
    m_pRenderSubSystem ( pRenderSubSystem ),
    m_pWorld ( pWorld ),
    m_isFollowingPlayer ( false ),
	m_framesTillAnimSwitchHeadingIsPossible ( 0 ),
    m_playerOldHeading ( 0 ),
    m_pCursorPosInworld ( new Vector2D ),
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

GameCamera::~GameCamera() {}

// Initialisierung der 2D Kamera (ganz am Anfang aufrufen)
void GameCamera::Init ()
{
    m_pRenderSubSystem->SetMatrix(RenderSubSystem::World);
    glMatrixMode ( GL_PROJECTION );
    glLoadIdentity(); //Reset projection matrix

    // orthogonalen 2D-Rendermodus
    gluOrtho2D( -m_viewWidth/2, m_viewWidth/2, -m_viewHeight/2, m_viewHeight/2 ); // (z ist nicht wichtig)
    glMatrixMode ( GL_MODELVIEW );

    m_isFollowingPlayer = false;
    //Zoom( 1.25f );
    Zoom( 1.5f );
}

// Kamarabewegung aktualisieren und auf Eingabe, die die Kamera beeinflusst, reagieren
void GameCamera::Update ( float deltaTime ) // time_span in seconds
{
    // Zoom
    const float Zoom_Amount = 3.0;
    // Wenn Spieler + oder - gedrückt hat
    if ( m_pInputSubSystem->KeyState ( CameraZoomIn ) )
        Zoom ( 1.0f + Zoom_Amount * deltaTime );
    else if ( m_pInputSubSystem->KeyState ( CameraZoomOut ) )
        Zoom ( 1.0f - Zoom_Amount * deltaTime );

    // Cursorposition in der Welt ausrechnen (nicht relativ zum Display, sondern Koordinaten in der Welt)
    UpdateCurPosInWorld();

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
        Vector2D new_pos ( *m_pCameraDeparture + ( *m_pCameraDestination-*m_pCameraDeparture ) *t );

        MoveAbsolute ( new_pos, 0.0f ); // Kameraposition ein wenig gegen richtung Ziel verschieben
        UpdateCurPosInWorld();
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
        UpdateCurPosInWorld();
    }

	if ( m_framesTillAnimSwitchHeadingIsPossible > 0 )
		m_framesTillAnimSwitchHeadingIsPossible--;

    if ( m_isFollowingPlayer )
    {
        // Zuerst checken ob es einen Spielerobjekt überhaupt gibt.
        boost::shared_ptr<Entity> player = m_pWorld->GetEntity("Player");
        if ( player )
        {
            CompPhysics* player_phys = player->GetComponent<CompPhysics>();
            if ( player_phys )
            {
                // Blickpunkt der Kamera festsetzten (in der Mitte zwischen Spieler und Cursor)

                Vector2D camera_focus ( player_phys->GetSmoothPosition() ); // Position des Spielers

                Vector2D dist ( camera_focus.Distance ( *m_pCursorPosInworld ) ); // Vektor vom Spieler zum Cursor
                dist *= 0.45f; // nicht ganz in der Mitte, sondern eher zum Spieler gerichtet
                camera_focus += dist;

#define FOLLOW_QUICKNESS 25
                MoveRelative ( ( camera_focus - CAMERA_POS ) * deltaTime * FOLLOW_QUICKNESS, 0.0f );
                UpdateCurPosInWorld();

                std::vector<CompVisualAnimation*> player_anims = player->GetComponents<CompVisualAnimation>();
                // ACHTUNG
				const CompGravField* grav = player_phys->GetActiveGravField();
				Vector2D upVector(0.0f,1.0f);
				if ( grav!=NULL )
					upVector = grav->GetAcceleration( player_phys->GetCenterOfMassPosition() ).GetUnitVector()*-1;
				bool right = upVector.IsRight( *m_pCursorPosInworld - player_phys->GetCenterOfMassPosition() );
				if ( m_playerOldHeading == 0 || right != (m_playerOldHeading==1) ) {
					if ( m_framesTillAnimSwitchHeadingIsPossible == 0 ) {
						if (right)
                            m_playerOldHeading = 1;
                        else
                            m_playerOldHeading = -1;
						m_framesTillAnimSwitchHeadingIsPossible = cFramesTillAnimSwitchHeadingIsPossible;

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
    else
    {
        // Wenn die Kamera den Spieler nicht folgt, kann man sie mit den Pfeiltasten bewegen.
        const float scroll_Amount = 40.0f/m_scaleValue; // Verschiebungsgrösse
        Vector2D temp_vector;
        if ( m_pInputSubSystem->KeyState ( CameraUp ) )
        {
            temp_vector = Vector2D( 0,scroll_Amount );
            temp_vector.Rotate( degToRad(m_rotationAngleDeg) );
            MoveRelative ( temp_vector * deltaTime, 0.0f );
        }
        else if ( m_pInputSubSystem->KeyState ( CameraDown ) )
        {
            temp_vector = Vector2D( 0,-scroll_Amount );
            temp_vector.Rotate( degToRad(m_rotationAngleDeg) );
            MoveRelative ( temp_vector * deltaTime, 0.0f);
        }
        if ( m_pInputSubSystem->KeyState ( CameraLeft ) )
        {
            temp_vector = Vector2D( -scroll_Amount,0 );
            temp_vector.Rotate( degToRad(m_rotationAngleDeg) );
            MoveRelative ( temp_vector * deltaTime, 0.0f);
        }
        else if ( m_pInputSubSystem->KeyState ( CameraRight ) )
        {
            temp_vector = Vector2D( scroll_Amount,0 );
            temp_vector.Rotate( degToRad(m_rotationAngleDeg) );
            MoveRelative ( temp_vector * deltaTime, 0.0f);
        }
        UpdateCurPosInWorld();

        if ( m_pInputSubSystem->RMouseKeyState() )
        {
            MoveAbsolute ( *m_pCursorPosInworld, 1.0f);
            UpdateCurPosInWorld();
        }
    }

    const float rotate_Amount = 130.0; // Drehstärke
    if ( m_pInputSubSystem->KeyState ( CameraRotateCw ) )
        RotateRelative( -rotate_Amount * deltaTime, 0.0f);
    else if ( m_pInputSubSystem->KeyState ( CameraRotateCcw ) )
        RotateRelative( rotate_Amount * deltaTime, 0.0f);
    if ( m_pInputSubSystem->KeyState ( CameraResetAngle ) )
    {
        float time = GetCameraAngle();
        float x = fabs(time-360);
        if ( time > x )
            time = x;
        time *= 0.01f;
        RotateAbsolute( 0.0f, time );
    }
    UpdateCurPosInWorld();

    //gAaLog.Write( "Cx = %f Cy = %f r = %f\n",CAMERA_POS.x,CAMERA_POS.y,m_rotationAngleDeg);
}

void GameCamera::MoveRelative ( const Vector2D& rMove, float timeToArrive )
{
    if ( timeToArrive > 0.0f)
    {
        m_isMovingSmoothly = true;
        *m_pCameraDeparture = CAMERA_POS;
        *m_pCameraDestination = rMove + *m_pCameraDeparture;
        m_moveTotalTimeToArrive = timeToArrive;
        m_moveTimeElapsed = 0.0;
    }
    else
	{
		//UpdateCurPosInWorld();
        *m_pTranslation += rMove * -1.0;
	}
}

void GameCamera::MoveAbsolute ( const Vector2D& rPos, float timeToArrive )
{
    if ( timeToArrive > 0.0f)
    {
        m_isMovingSmoothly = true;
        *m_pCameraDeparture = CAMERA_POS;
        *m_pCameraDestination = rPos;
        m_moveTotalTimeToArrive = timeToArrive;
        m_moveTimeElapsed = 0.0;
    }
    else
	{
		//UpdateCurPosInWorld();
        *m_pTranslation = rPos * -1.0;
	}
}

void GameCamera::Zoom ( float zoom )
{
    m_scaleValue *= zoom;
    if ( m_scaleValue > cMaxScale )
        m_scaleValue = cMaxScale;
    else if ( m_scaleValue < cMinScale )
        m_scaleValue = cMinScale;
    m_viewWidth = (gAaConfig.GetInt("WideScreen")?STDViewWidth_8_5:STDViewWidth_4_3) * 1.0f/m_scaleValue;
    m_viewHeight = (gAaConfig.GetInt("WideScreen")?STDViewHeight_8_5:STDViewHeight_4_3) * 1.0f/m_scaleValue;
}

void GameCamera::Look() const
{
    glLoadIdentity();
    
    glRotatef ( m_rotationAngleDeg, 0.0, 0.0, -1.0f);
    glScalef ( m_scaleValue, m_scaleValue, 1 ); // x und y zoomen
    glTranslatef ( m_pTranslation->x * 1, m_pTranslation->y * 1, 0.0f);
}

boost::shared_ptr<Vector2D> GameCamera::GetCameraPos () const
{
    boost::shared_ptr<Vector2D> pos( boost::make_shared<Vector2D>() );
    *pos = CAMERA_POS; // = *m_pTranslation * (-1) , siehe #define zu oberst
    return pos;
    // Translation is der Kehrwert der Position weil
    // wenn man die Welt nach links verschiebt (Translation) ist es das gleiche wie
    // wenn man die Kamera (Pos) nach rechts verschiebt.
}

float GameCamera::GetCameraAngle () const
{
    return mod360( m_rotationAngleDeg );
}

bool GameCamera::SetFollowPlayer ( bool follow )
{
    bool oldIsFollowingPlayer = m_isFollowingPlayer;
    m_isFollowingPlayer = follow;
    return oldIsFollowingPlayer; // alte Einstellung zurückgeben
}

// Cursorposition in der Welt neu ausrechnen (nicht relativ zum Display, sondern Koordinaten in der Welt)
void GameCamera::UpdateCurPosInWorld ()
{
    Vector2D middle_of_screen_to_cursor( ( m_pInputSubSystem->MousePos()->x - 0.5f ) * m_viewWidth,
                                        -( m_pInputSubSystem->MousePos()->y - 0.5f ) * m_viewHeight );
    middle_of_screen_to_cursor.Rotate( degToRad(m_rotationAngleDeg) );
    //                       Position der Kamera      Position des Kursors relativ zur Bildschirmmitte
    //m_pCursorPosInworld->x = CAMERA_POS.x + ( ( m_pInputSubSystem->MousePos()->x - 0.5 ) * m_viewWidth );
    //m_pCursorPosInworld->y = CAMERA_POS.y - ( ( m_pInputSubSystem->MousePos()->y - 0.5 ) * m_viewHeight );
    *m_pCursorPosInworld = CAMERA_POS + middle_of_screen_to_cursor;
}

void GameCamera::RotateAbsolute ( float angleDeg, float timeToArrive )
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

void GameCamera::RotateRelative ( float angleDeg, float timeToArrive )
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

Vector2D GameCamera::ScreenToWorld( const Vector2D& screenPos )
{
    Vector2D middle_of_screen_to_screenPos( ( screenPos.x - 0.5f ) * m_viewWidth,
                                           -( screenPos.y - 0.5f ) * m_viewHeight );
    middle_of_screen_to_screenPos.Rotate( degToRad(m_rotationAngleDeg) );

    return CAMERA_POS + middle_of_screen_to_screenPos;
}

Vector2D GameCamera::WorldToScreen( const Vector2D& worldPos )
{
    /*Vector2D m (0.5f,0.5f);
    m.Rotate( -degToRad(m_rotationAngleDeg) );
    Vector2D screenPos ( (worldPos.x - CAMERA_POS.x)/m_viewWidth + m.x,
                       - (worldPos.y - CAMERA_POS.y)/m_viewHeight + m.y);
    return screenPos;*/

    Vector2D screenPos ( (worldPos.x - CAMERA_POS.x),
                         (worldPos.y - CAMERA_POS.y) );
    screenPos.Rotate( -degToRad(m_rotationAngleDeg) );

    screenPos.x /= m_viewWidth;
    screenPos.y /= -m_viewHeight;

    screenPos.x += 0.5f;
    screenPos.y += 0.5f;

    return screenPos;
}
