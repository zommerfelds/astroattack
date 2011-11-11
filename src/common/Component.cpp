/*
 * Component.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "Component.h"
#include "game/GameEvents.h" // Steuert die Spielerreignisse
#include "ComponentManager.h"

Component::Component(const ComponentIdType& id, GameEvents& gameEvents)
: m_gameEvents (gameEvents),
  m_compManager (NULL),
  m_entityId (),
  m_id (id)
{}


const ComponentIdType Component::DEFAULT_ID = "";
