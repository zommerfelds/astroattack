/*
 * Editor.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "Editor.h"

#include "common/World.h"
#include "common/Vector2D.h"
#include "common/DataLoader.h"

#include <iostream>
#include <boost/property_tree/info_parser.hpp>

Editor::Editor(GameEvents& events)
: m_events (events),
  m_world ()
{
}

void Editor::loadLevel(const std::string& fileName)
{
    m_world.reset(new World(m_events));
    try
    {
        DataLoader::loadToWorld( "data/player.info", *m_world, m_events );
        DataLoader::loadToWorld( fileName, *m_world, m_events );
    }
    catch (DataLoadException& e)
    {
        std::cerr << "Could not read level: " << e.getMsg() << std::endl;
        return;
    }
}

void Editor::onLMouseClick(const Vector2D& worldPos)
{

}
