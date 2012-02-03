/*
 * Component.cpp
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#include "Component.h"

#include "ComponentManager.h"
#include "common/GameEvents.h"

#include <boost/scoped_ptr.hpp>

Component::Component(const ComponentId& id, GameEvents& gameEvents)
: m_gameEvents (gameEvents),
  m_compManager (NULL),
  m_entityId (),
  m_id (id)
{}

const ComponentType& Component::getTypeIdStatic()
{
    static boost::scoped_ptr<ComponentType> typeId (new ComponentType("Component"));
    return *typeId;
}

const ComponentId Component::DEFAULT_ID = "";
