/*
 * PlayerController.h
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */

#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include "common/Event.h"

class Component;
class GameEvents;
class CompPlayerController;
class InputSubSystem;

class PlayerController {

public:
    PlayerController(GameEvents&, InputSubSystem&);
    void update();

private:
    void onRegisterComponent(Component& component);

    EventConnection m_eventConnection1;

    CompPlayerController* m_compPlayerContrl;

    InputSubSystem& m_inputSubSystem;
};


#endif /* PLAYERCONTROLLER_H */
