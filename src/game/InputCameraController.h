/*
 * InputCameraController.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#ifndef INPUTCAMERACONTROLLER_H
#define INPUTCAMERACONTROLLER_H

#include "common/CameraController.h"

class InputSubSystem;
class RenderSystem;
class ComponentManager;

//----------//
//  Camera  //
//----------//
/*
    Dieses Klass bedient die Kamera (OpenGL-Ansicht).
    Die Methoden dieser Klasse werden genutzt um Kameraeinstellungen wie Zoom oder Lage zu ändern.
    Update() muss bei jedem Frame aufgerufen werden. (oder nur Look())
*/
class InputCameraController : public CameraController
{
public:
    InputCameraController(const InputSubSystem& inputSubSystem, RenderSystem& renderSubSystem, ComponentManager& compMgr);

    void update( float deltaTime ); // Kamarabewegung aktualisieren und auf Eingabe, die die Kamera beeinflusst, reagieren

    void setFollowPlayer( bool follow ); // soll die Kamera den Spieles folgen oder nicht?
    bool isFollowingPlayer() const {
        return m_isFollowingPlayer;
    }

private:
    const InputSubSystem& m_inputSubSystem;
    ComponentManager& m_compManager;

    bool m_isFollowingPlayer;
    float m_timeSinceLastSwitchHeading;
    char m_playerHeading; // -1: left, 0: not yet defined, 1: right
};

#endif /* CAMERACONTROLLERINPUT_H_ */
