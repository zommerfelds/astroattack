/*----------------------------------------------------------\
|                        Camera.h                           |
|                        --------                           |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// Steuert und rechnet Kameraverschiebungen

#ifndef CAMERA_H
#define CAMERA_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

class InputSubSystem;
class RenderSubSystem;
class GameWorld;
class Vector2D;

// 8:5
/*
const float STDViewWidth = 26.4; // horizontal lenght (units) of the view
const float STDViewHeight = 16.5; // vertical lenght (units) of the view
*/
// 4:3
/*
const float STDViewWidth = 24; // horizontal lenght (units) of the view
const float STDViewHeight = 18; // vertical lenght (units) of the view
*/

//----------//
//  Camera  //
//----------//

/*
    Dieses Klass bedient die Kamera (OpenGL-Ansicht).
    Die Methoden dieser Klasse werden genutzt um Kameraeinstellungen wie Zoom oder Lage zu ändern.
    Update() muss bei jedem Frame aufgerufen werden. (oder nur Look())
*/
class GameCamera
{
public:
    GameCamera( const InputSubSystem* pInputSubSystem, RenderSubSystem* pRenderSubSystem, const GameWorld* pWorld );
    ~GameCamera();

    // Initialisierung
    void Init();

    void Update ( float deltaTime ); // Kamarabewegung aktualisieren und auf Eingabe, die die Kamera beeinflusst, reagieren
    void Look() const; // OpenGL-Ansicht aktualisieren (wird von Update() automatisch aufgerufen)

    // Methoden um die Kamera zu bewegen:
    // timeToArrive ist die Zeit, die benötigt werden soll, damit die Kamera den Zielort erreicht
    void MoveRelative ( const Vector2D& rMove, float timeToArrive ); // rMove ist Position des Zielortes relativ zur Lage der Kamera
    void MoveAbsolute ( const Vector2D& rPos, float timeToArrive ); // rPos ist absolute Position des Zielortes

    // Um heran- oder herauszuzoomen
    void Zoom ( float zoom ); // zoom ist der Zoomfaktor ( < 1 bedeutet herauszoomen; > 1 bedeutet herazoomen )
    bool SetFollowPlayer ( bool follow ); // soll die Kamera den Spieles folgen oder nicht?

    void RotateAbsolute ( float angleDeg, float timeToArrive ); // Den Winkel der Kamera setzen. "angleDeg" als neuer Winkel nehmen.
    void RotateRelative ( float angleDeg, float timeToArrive ); // Den Winkel der Kamera verändern. Um "angleDeg" drehen. (+:Gegenuhrzegersinn, -:Uhrzeigersinn)

    Vector2D ScreenToWorld( const Vector2D& screenPos );
    Vector2D WorldToScreen( const Vector2D& worldPos );

    // ========= Zugriff ==========
    boost::shared_ptr<Vector2D> GetCameraPos () const;
    float GetCameraAngle () const;
    float GetViewWidth() const // wie weit ist die Strecke vom linken Bildschirmrand zum Rechten?
    {
        return m_viewWidth;
    }
    float GetViewHeight() const // oben - unten
    {
        return m_viewHeight;
    }
    const Vector2D* GetCursorPosInWorld() const
    {
        return m_pCursorPosInworld.get();    // Mauszeigerkoordinaten
    }
    // (nicht relativ zum Display, sondern Koordinaten in der Welt)

private:
	void UpdateCurPosInWorld();

    boost::scoped_ptr<Vector2D> m_pTranslation;
    float m_scaleValue;
    float m_rotationAngleDeg;

    float m_viewWidth;
    float m_viewHeight;

    const InputSubSystem* m_pInputSubSystem;
    RenderSubSystem* m_pRenderSubSystem;
    const GameWorld* m_pWorld;

    bool m_isFollowingPlayer;
	int m_framesTillAnimSwitchHeadingIsPossible;
    char m_playerOldHeading; // -1: left, 0: not yet defined, 1: right

    boost::scoped_ptr<Vector2D> m_pCursorPosInworld;

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

// Astro Attack - Christian Zommerfelds - 2009
