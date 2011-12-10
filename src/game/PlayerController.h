/*
 * PlayerController.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include "common/Event.h"

class Component;
struct GameEvents;
class CompPlayerController;
class InputSubSystem;

class PlayerController {

public:
    PlayerController(GameEvents&, const InputSubSystem&);
    void update();

private:
    void onRegisterComponent(Component& component);

    EventConnection m_eventConnection1;

    CompPlayerController* m_compPlayerContrl;

    const InputSubSystem& m_inputSubSystem;
};


#endif /* PLAYERCONTROLLER_H */
