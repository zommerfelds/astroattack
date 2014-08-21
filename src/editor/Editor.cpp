/*
 * Editor.cpp
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */

#include "Editor.h"

#include "common/components/CompShape.h"
#include "common/components/CompPhysics.h"
#include "common/components/CompPosition.h"
#include "common/components/CompVisualTexture.h"

#include "common/ComponentManager.h"
#include "common/Vector2D.h"
#include "common/DataLoader.h"
#include "common/Foreach.h"
#include "common/Physics.h"
#include "common/GameEvents.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/property_tree/info_parser.hpp>

Editor::Editor(GameEvents& events, PhysicsSystem& physics)
: m_events (events),
  m_physics (physics),
  m_compMgr (new ComponentManager(m_events)),
  m_textureList (),
  m_currentTextureIt ()
{
    //m_eventConnection1 = m_events.newEntity.registerListener( boost::bind( &Editor::onNewEntity, this, _1 ) );
    //m_eventConnection2 = m_events.deleteEntity.registerListener( boost::bind( &Editor::onRemoveEntity, this, _1 ) );
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
    m_compMgr.reset(new ComponentManager(m_events));
    m_guiData.selectedEntity = OptEntity();
}

void Editor::loadLevel(const std::string& fileName)
{
    clearLevel();
    try
    {
        //DataLoader::loadToWorld( "data/player.info", *m_world, m_events );
        DataLoader::loadToWorld( fileName, *m_compMgr, m_events );
        //m_guiData.world = m_world.get();
    }
    catch (DataLoadException& e)
    {
        log(Error) << "Could not read level: " << e.getMsg() << "\n";
        return;
    }
}

void Editor::saveLevel(const std::string& fileName)
{
    DataLoader::saveWorld(fileName, *m_compMgr);
}

void Editor::cmdAddVertex(const Vector2D& worldPos)
{
    if (m_guiData.indexCurVertex < 8)
    {
        m_guiData.createdVertices[m_guiData.indexCurVertex] = worldPos;
        ++m_guiData.indexCurVertex;
    }
}

void Editor::cmdCreateBlock()
{
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
            if ( m_compMgr->getAllEntities().count(ss.str()) == 0 )
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
            compShape->setVertex(i, m_guiData.createdVertices[i] - m_guiData.createdVertices[0]);
        }
        entity.push_back(compShape);

        boost::shared_ptr<CompPosition> compPos = boost::shared_ptr<CompPosition>(new CompPosition(Component::DEFAULT_ID, m_events, m_guiData.createdVertices[0]));
        entity.push_back(compPos);

        TextureId textureName = m_guiData.currentTexture;
        boost::shared_ptr<CompVisualTexture> compPolyTex = boost::shared_ptr<CompVisualTexture>(new CompVisualTexture(Component::DEFAULT_ID, m_events, textureName));
        entity.push_back(compPolyTex);

        m_compMgr->addEntity(entityId, entity);

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

void Editor::cmdSelect(const Vector2D& pos)
{
    m_guiData.selectedEntity = m_physics.selectEntity(pos);
}

bool Editor::cmdSetNewEntityId(const EntityId& id)
{
    if (!m_guiData.selectedEntity || id == m_guiData.selectedEntity->first)
        return true;
    // TODO: test if name is valid
    bool success = m_compMgr->renameEntity(m_guiData.selectedEntity->first, id);
    if (success)
        m_guiData.selectedEntity->first = id;
    return success;
}

void Editor::cmdSetNewCompId(const ComponentId& id, const Component* comp)
{
    if (!m_guiData.selectedEntity) return;
    foreach (Component* c, m_compMgr->getComponents<Component>(m_guiData.selectedEntity->first))
    {
        if (c == comp)
        {
            c->setId(id);
            break;
        }
    }
}

const EditorGuiData& Editor::getGuiData()
{
    return m_guiData;
}

/*void Editor::onNewEntity(const EntityIdType& id)
{
    CompPhysics* compPhys = m_world->getCompManager().getComponent<CompPhysics>(id);
    CompShape* compShape = m_world->getCompManager().getComponent<CompShape>(id);
    if (!compPhys && compShape)
    {
    }
}

void Editor::onRemoveEntity(const EntityIdType& id)
{
}
*/
