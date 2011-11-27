/*
 * InputCameraController.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "InputCameraController.h"
#include "Input.h"
#include "Configuration.h"

#include "common/components/CompPhysics.h"
#include "common/components/CompVisualAnimation.h"
#include "common/components/CompGravField.h"

#include "common/CameraController.h"
#include "common/ComponentManager.h"

namespace {

// 16:10
const float STDViewWidth_16_10 = 26.4f; // horizontal lenght (units) of the view
const float STDViewHeight_16_10 = 16.5f; // vertical lenght (units) of the view

// 4:3
const float STDViewWidth_4_3 = 24.f; // horizontal lenght (units) of the view
const float STDViewHeight_4_3  = 18.f; // vertical lenght (units) of the view

// Number of frames to wait before player heading can change again
const float cTimeTillAnimSwitchHeadingIsPossible = 0.2f;

const float cMinZoom = 0.001f;
const float cMaxZoom = 90000.0f;

}

// Konstruktor
InputCameraController::InputCameraController(const InputSubSystem& inputSubSystem, RenderSubSystem& renderSubSystem, ComponentManager& compMgr)
 : CameraController(renderSubSystem, (float(gConfig.get<int>("ScreenWidth")))/gConfig.get<int>("ScreenHeight")),
   m_inputSubSystem (inputSubSystem),
   m_compManager (compMgr),
   m_isFollowingPlayer ( false ),
   m_timeSinceLastSwitchHeading ( 0 ),
   m_playerHeading ( 0 )
{
	setZoomRange(cMinZoom, cMaxZoom);
	setZoom(0.08f);
}

// Kamarabewegung aktualisieren und auf Eingabe, die die Kamera beeinflusst, reagieren
void InputCameraController::update( float deltaTime ) // time_span in seconds
{
    // Zoom
    const float cZoomFactor = 4.0f;
    // Wenn Spieler + oder - gedrückt hat
    if ( m_inputSubSystem.getKeyState ( CameraZoomIn ) ) // not frame rate independent (?)
        zoom( pow(1/cZoomFactor, deltaTime) );
        //SetZoom(m_scaleValue - cZoomFactor * deltaTime);
    else if ( m_inputSubSystem.getKeyState ( CameraZoomOut ) )
        zoom( pow(cZoomFactor, deltaTime) );
        //SetZoom(m_scaleValue + cZoomFactor * deltaTime);

    if (!isFollowingPlayer())
    {
        // Wenn die Kamera den Spieler nicht folgt, kann man sie mit den Pfeiltasten bewegen.
        const float cScrollVelMag = 40.0f / getZoom(); // Verschiebungsgrösse
        Vector2D scrollVelocity;
        if ( m_inputSubSystem.getKeyState ( CameraUp ) )
            scrollVelocity.set(0, cScrollVelMag);
        else if ( m_inputSubSystem.getKeyState ( CameraDown ) )
            scrollVelocity.set(0, -cScrollVelMag);

        if ( m_inputSubSystem.getKeyState ( CameraLeft ) )
            scrollVelocity.set(-cScrollVelMag, 0);
        else if ( m_inputSubSystem.getKeyState ( CameraRight ) )
            scrollVelocity.set(cScrollVelMag, 0);

        moveRelative( scrollVelocity.rotated(getCameraAngle()) * deltaTime, 0.0f );

        if ( m_inputSubSystem.getRMouseKeyState() )
        {
            moveAbsolute( screenToWorld(m_inputSubSystem.getMousePos()), 1.0f);
        }
    }

    m_timeSinceLastSwitchHeading += deltaTime;

	// Zuerst checken ob es einen Spielerobjekt überhaupt gibt.
	CompPhysics* playerPhys = m_compManager.getComponent<CompPhysics>("Player");
	if ( playerPhys )
	{
		const Vector2D& playerPos = playerPhys->getSmoothCenterOfMass(); // Position des Spielers

		if ( m_isFollowingPlayer )
		{
			// calculate cursor position relative to the middle of the screen
			Vector2D cursorPos = m_inputSubSystem.getMousePos();
			cursorPos.x = cursorPos.x - 0.5f;
			cursorPos.y = (cursorPos.y - 0.5f) * -1;
			cursorPos.rotate(getCameraAngle());

			const float cRangeOfSightFactor = 6.0f; // how far the mouse can move the camera away from the player
			Vector2D target = playerPos + cursorPos * cRangeOfSightFactor;

			// to integrate over the full deltaTime step (which can be big) we split it into smaller steps
			// this prevents explosive behavior when deltaTime is very big
			// all sub-steps will be the same size except for the last one that will be just the remainder time (smaller than the others)
			const float cCameraTimeStep = 0.015f;
			float timeStep = cCameraTimeStep;
			float remainingTime = deltaTime;
			while (remainingTime > 0.0f)
			{
				if (remainingTime < cCameraTimeStep) // in the last step pick the remaining time
					timeStep = remainingTime;
				remainingTime -= cCameraTimeStep;

				// calculate new camera position
				// the position depends on the player position and on the mouse cursor position
				const float cPosFollowQuicknessFactor = 15.0f; // how fast the camera moves to the new position

				Vector2D velocity = (target - getCameraPos()) * cPosFollowQuicknessFactor;
				moveRelative(velocity * timeStep);

				// calculate new camera zoom
				// the zoom depends on the mouse cursor position and on the players velocity
				const float cZoomFollowQuicknessFactor = 1.5f;
				const float cMaxFollowZoom = 0.13f;
				const float cMinFollowZoom = 0.05f;
				const float cCursorFactor = 0.65f;
				const float cGradientStart = 0.1f;
				const float cGradientEnd = 1.0f;
				const float cPlayerMaxVel = 5.0f;

				float zoomGradient = (cMaxFollowZoom - cMinFollowZoom) / (cGradientEnd - cGradientStart);
				float velFraction = playerPhys->getLinearVelocity().length() / cPlayerMaxVel;
				if (velFraction > 1.0f || playerPhys->getContacts().empty())
					velFraction = 1.0f;
				float zoomParameter = (cursorPos.length() * 2.0f) * (cCursorFactor) + velFraction * (1.0f - cCursorFactor);
				float zoomTarget = 0.0f;
				if (zoomParameter < cGradientStart)
					zoomTarget = cMaxFollowZoom;
				else if (zoomParameter > cGradientEnd)
					zoomTarget = cMinFollowZoom; // cMaxFollowZoom - (cGradientEnd - cGradientStart) * cZoomGradient;
				else
					zoomTarget = cMaxFollowZoom - (zoomParameter - cGradientStart) * zoomGradient;

				float zoomVelocity = (zoomTarget - getZoom()) * cZoomFollowQuicknessFactor;
				setZoom(getZoom() + zoomVelocity * timeStep);
			}
		}

		// flip player's heading according to cursor
		{
			std::vector<CompVisualAnimation*> player_anims = m_compManager.getComponents<CompVisualAnimation>("Player");
			const CompGravField* grav = playerPhys->getActiveGravField();
			Vector2D upVector(0.0f,1.0f);
			if ( grav )
				upVector = grav->getAcceleration( playerPhys->getCenterOfMass() ).getUnitVector()*-1;
			bool right = upVector.isRight( screenToWorld(m_inputSubSystem.getMousePos()) - playerPhys->getCenterOfMass() );
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

    const float cRotVelocity = 2.0f;
    if ( m_inputSubSystem.getKeyState ( CameraRotateCw ) )
        rotateRelative( -cRotVelocity * deltaTime, 0.0f);
    else if ( m_inputSubSystem.getKeyState ( CameraRotateCcw ) )
        rotateRelative( cRotVelocity * deltaTime, 0.0f);

    if ( m_inputSubSystem.getKeyState ( CameraResetAngle ) )
    {
        const float cRotResetVelocity = 2.0f;
        float angle = getCameraAngle();
        float angle2Pi = std::fabs(angle - 2*cPi);
        float time = std::min(angle, angle2Pi);
        time /= cRotResetVelocity;
        rotateAbsolute( 0.0f, time );
    }

    CameraController::update(deltaTime);
}

void InputCameraController::setFollowPlayer(bool follow)
{
    m_isFollowingPlayer = follow;
}
