/*
 * Component.cpp
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#include "Component.h"

#include "ComponentManager.h"
#include "common/GameEvents.h"

Component::Component(const ComponentIdType& id, GameEvents& gameEvents)
: m_gameEvents (gameEvents),
  m_compManager (NULL),
  m_entityId (),
  m_id (id)
{}

const ComponentIdType Component::DEFAULT_ID = "";
