/*
 * Editor.cpp
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#include "Editor.h"

#include "common/components/CompVisualTexture.h"
#include "common/components/CompPhysics.h"
#include "common/components/CompShape.h"
#include "common/components/CompPosition.h"

#include "common/World.h"
#include "common/Vector2D.h"
#include "common/DataLoader.h"
#include "common/Foreach.h"

#include <iostream>
#include <boost/property_tree/info_parser.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

Editor::Editor(GameEvents& events)
: m_events (events),
  m_world (new World(m_events)),
  m_textureList (),
  m_currentTextureIt ()
{
}

Editor::~Editor() {}

void Editor::setTextureList(const std::vector<std::string>& textureList)
{
    foreach(const std::string& tex, textureList)
    {
        if (!tex.empty() && tex[0] != '_')
            m_textureList.push_back(tex);
    }
    m_currentTextureIt = m_textureList.begin();
    m_guiData.currentTexture = *m_currentTextureIt;
    if (m_textureList.empty())
        log(Warning) << "No textures found for editor!\n";
}


void Editor::clearLevel()
{
    m_guiData.indexCurVertex = 0;
    m_world.reset(new World(m_events));
}

void Editor::loadLevel(const std::string& fileName)
{
    clearLevel();
    try
    {
        //DataLoader::loadToWorld( "data/player.info", *m_world, m_events );
        DataLoader::loadToWorld( fileName, *m_world, m_events );
    }
    catch (DataLoadException& e)
    {
        std::cerr << "Could not read level: " << e.getMsg() << std::endl;
        return;
    }
}

void Editor::saveLevel(const std::string& fileName)
{
    DataLoader::saveWorld(fileName, *m_world);
}

void Editor::cmdAddVertex(const Vector2D& worldPos)
{
    std::cerr << "worldPos: " << worldPos.x << " " << worldPos.y << std::endl;

    if (m_guiData.indexCurVertex < 8)
    {
        m_guiData.createdVertices[m_guiData.indexCurVertex] = worldPos;
        ++m_guiData.indexCurVertex;
    }
}

void Editor::cmdCreateBlock()
{
    std::cerr << "Creating block" << std::endl;
    if (m_guiData.indexCurVertex > 2)
    {
        std::string entityId;
        for ( int i = 0;; ++i )
        {
            std::stringstream ss;
            ss << "Block";
            ss.fill('0');
            ss.width(5);
            ss << i;
            if ( m_world->getCompManager().getAllEntities().count(ss.str()) == 0 )
            {
                entityId = ss.str();
                break;
            }
        }
        ComponentList entity;

        boost::shared_ptr<CompPhysics> compPhysics = boost::shared_ptr<CompPhysics>(new CompPhysics(Component::DEFAULT_ID, m_events));
        boost::shared_ptr<ShapeDef> shapeDef = boost::make_shared<ShapeDef>();
        shapeDef->friction = 0.3f;
        shapeDef->compId = "shape";
        compPhysics->addShapeDef( shapeDef );
        entity.push_back(compPhysics);

        boost::shared_ptr<CompShapePolygon> compShape = boost::shared_ptr<CompShapePolygon>(new CompShapePolygon("shape", m_events));
        for ( int i = 0; i < m_guiData.indexCurVertex; ++i )
        {
            compShape->setVertex(i, m_guiData.createdVertices[i]);
        }
        entity.push_back(compShape);

        boost::shared_ptr<CompPosition> compPos = boost::shared_ptr<CompPosition>(new CompPosition(Component::DEFAULT_ID, m_events));
        entity.push_back(compPos);

        TextureId textureName = m_guiData.currentTexture;
        boost::shared_ptr<CompVisualTexture> compPolyTex = boost::shared_ptr<CompVisualTexture>(new CompVisualTexture(Component::DEFAULT_ID, m_events, textureName));
        entity.push_back(compPolyTex);

        m_world->getCompManager().addEntity(entityId, entity);

        m_guiData.indexCurVertex = 0;
    }
}

void Editor::cmdCancelBlock()
{
    m_guiData.indexCurVertex = 0;
}

void Editor::cmdCancelVertex()
{
    if (m_guiData.indexCurVertex > 0)
        --m_guiData.indexCurVertex;
}

void Editor::cmdNextTexture()
{
    m_currentTextureIt++;
    if (m_currentTextureIt == m_textureList.end())
        m_currentTextureIt = m_textureList.begin();
    m_guiData.currentTexture = *m_currentTextureIt;
}

void Editor::cmdPrevTexture()
{
    m_currentTextureIt--;
    if (m_currentTextureIt == m_textureList.begin())
        m_currentTextureIt = --m_textureList.end();
    m_guiData.currentTexture = *m_currentTextureIt;
}

const EditorGuiData& Editor::getGuiData()
{
    return m_guiData;
}

