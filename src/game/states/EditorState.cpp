/*
 * EditorState.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */


#include <cmath>
#include <sstream>
#include <boost/make_shared.hpp>

#include "EditorState.h"
#include "game/GameApp.h"
#include "common/Renderer.h"
#include "game/Input.h"
#include "game/Logger.h"
#include "game/Configuration.h"
#include "common/DataLoader.h"
#include "common/Physics.h"

#include "common/components/CompVisualTexture.h"
#include "common/components/CompPhysics.h"
#include "common/components/CompShape.h"
#include "common/components/CompPosition.h"

const GameStateId EditorState::STATE_ID = "EditorState";

#include "common/Texture.h"

EditorState::EditorState( SubSystems& subSystems )
: GameState( subSystems ),
  m_gameWorld ( getSubSystems().events ),
  m_cameraController ( getSubSystems().input, getSubSystems().renderer, m_gameWorld.getCompManager() ),
  m_currentPoint ( 0 ),
  m_currentTexture (),
  m_currentTextureNum ( 0 ),
  m_helpTextOn ( false ),
  m_mouseButDownOld ( false ),
  m_cancelVertexKeyDownOld ( false ),
  m_createEntityKeyDownOld ( false ),
  m_nextTextureKeyDownOld ( false ),
  m_prevTextureKeyDownOld ( false ),
  m_helpKeyDownOld ( false )
{
    std::vector<std::string> texList = getSubSystems().renderer.getTextureManager().getTextureList();
    unsigned int i = 0;
    for ( ; i < texList.size(); ++i )
    {                
        m_currentTexture = texList[i];
        if ( m_currentTexture[0] != '_' )
        {
            m_currentTextureNum = i;
            break;
        }        
    }
    if ( i == texList.size() )
        gAaLog.write ( "Warning: No textures found for editor!\n" );
}

void EditorState::init()        // State starten
{
    m_cameraController.setFollowPlayer ( false );

    DataLoader::loadWorld( gConfig.get<std::string>("EditorLevel"), m_gameWorld, getSubSystems().events );
}

void EditorState::cleanup()     // State abbrechen
{
    // Grafiken aus XML-Datei laden
    DataLoader::saveWorld( gConfig.get<std::string>("EditorLevel"), m_gameWorld );
}

void EditorState::pause()       // State anhalten
{
}

void EditorState::resume()      // State wiederaufnehmen
{
}

void EditorState::frame( float deltaTime )       // Pro Frame
{
    getSubSystems().input.update();   // neue Eingaben lesen
    m_cameraController.update( deltaTime ); // Kamera updaten
}

namespace
{

void SnapToGrid( Vector2D& worldCoordinates )
{
    const float cellWidth = 0.5f;

    float cellsX = worldCoordinates.x/cellWidth;
    if ( cellsX > 0 )
        cellsX += 0.5;
    else
        cellsX -= 0.5;
    worldCoordinates.x = ((int)(cellsX))*cellWidth;

    float cellsY = worldCoordinates.y/cellWidth;
    if ( cellsY > 0 )
        cellsY += 0.5;
    else
        cellsY -= 0.5;
    worldCoordinates.y = ((int)(cellsY))*cellWidth;
}

}

void EditorState::update()      // Spiel aktualisieren
{
    if ( getSubSystems().input.getLMouseKeyState() )
    {
        if ( m_mouseButDownOld == false && m_currentPoint < 8 )
        {
            m_clickedPoints[m_currentPoint] = m_cameraController.screenToWorld( getSubSystems().input.getMousePos() );
            SnapToGrid( m_clickedPoints[m_currentPoint] );
            ++m_currentPoint;
        }
        m_mouseButDownOld = true;
    }
    else
        m_mouseButDownOld = false;

    if ( getSubSystems().input.getKeyState( EditorCancelBlock ) )
    {
        m_currentPoint = 0;
    }

    if ( getSubSystems().input.getKeyState( EditorCancelVertex ) )
    {
        if ( m_cancelVertexKeyDownOld == false && m_currentPoint > 0 )
            --m_currentPoint;
        m_cancelVertexKeyDownOld = true;
    }
    else
        m_cancelVertexKeyDownOld = false;

    if ( getSubSystems().input.getKeyState( EditorCreateEntity ) )
    {
        if ( m_createEntityKeyDownOld == false && m_currentPoint > 2 )
        {
            std::string entityId;
            for ( int i = 0;; ++i )
            {
                std::stringstream ss;
                ss << "Block";
                ss.fill('0');
                ss.width(5);
                ss << i;
                if ( m_gameWorld.getCompManager().getAllEntities().count(ss.str()) == 0 )
                {
                    entityId = ss.str();
                    break;
                }
            }
            ComponentList entity;

            boost::shared_ptr<CompPhysics> compPhysics = boost::shared_ptr<CompPhysics>(new CompPhysics(Component::DEFAULT_ID, getSubSystems().events));
            boost::shared_ptr<ShapeDef> shapeDef = boost::make_shared<ShapeDef>();
            shapeDef->friction = 0.3f;
            shapeDef->compId = "shape";
            compPhysics->addShapeDef( shapeDef );
            entity.push_back(compPhysics);

            boost::shared_ptr<CompShapePolygon> compShape = boost::shared_ptr<CompShapePolygon>(new CompShapePolygon("shape", getSubSystems().events));
            for ( int i = 0; i < m_currentPoint; ++i )
            {
            	compShape->setVertex(i, m_clickedPoints[i]);
            }
            entity.push_back(compShape);

            boost::shared_ptr<CompPosition> compPos = boost::shared_ptr<CompPosition>(new CompPosition(Component::DEFAULT_ID, getSubSystems().events));
            entity.push_back(compPos);

            TextureId textureName = m_currentTexture;
            boost::shared_ptr<CompVisualTexture> compPolyTex = boost::shared_ptr<CompVisualTexture>(new CompVisualTexture(Component::DEFAULT_ID, getSubSystems().events, textureName));
            entity.push_back(compPolyTex);

            m_gameWorld.getCompManager().addEntity(entityId, entity);

            m_currentPoint = 0;
        }
        m_createEntityKeyDownOld = true;
    }
    else
        m_createEntityKeyDownOld = false;

    if ( getSubSystems().input.getKeyState( EditorNextTexture ) )
    {
        if ( !m_nextTextureKeyDownOld )
        {
            unsigned int old_currentTextureNum = m_currentTextureNum;
            std::vector<std::string> texList ( getSubSystems().renderer.getTextureManager().getTextureList() );
            do
            {
                ++m_currentTextureNum;
                if ( texList.size() <= m_currentTextureNum )
                    m_currentTextureNum = 0;
                m_currentTexture = texList[m_currentTextureNum];
                if ( m_currentTexture[0] != '_' )
                    break;
            } while ( old_currentTextureNum != m_currentTextureNum );
        }
        m_nextTextureKeyDownOld = true;
    }
    else
        m_nextTextureKeyDownOld = false;

    if ( getSubSystems().input.getKeyState( EditorPrevTexture ) )
    {
        if ( !m_prevTextureKeyDownOld )
        {
            std::vector<std::string> texList = getSubSystems().renderer.getTextureManager().getTextureList();
            unsigned int old_currentTextureNum = m_currentTextureNum;
            do
            {                
                if ( m_currentTextureNum != 0 )
                    --m_currentTextureNum;
                else
                    m_currentTextureNum = texList.size()-1;
                m_currentTexture = texList[m_currentTextureNum];
                if ( m_currentTexture[0] != '_' )
                    break;
            } while ( old_currentTextureNum != m_currentTextureNum );
        }
        m_prevTextureKeyDownOld = true;
    }
    else
        m_prevTextureKeyDownOld = false;

    if ( getSubSystems().input.getKeyState( EditorToggleHelp ) )
    {
        if ( m_helpKeyDownOld == false )
            m_helpTextOn = !m_helpTextOn;
        m_helpKeyDownOld = true;
    }
    else
        m_helpKeyDownOld = false;
}

void EditorState::draw( float accumulator )        // Spiel zeichnen
{
    getSubSystems().physics.calculateSmoothPositions(accumulator);

    RenderSubSystem& renderer = getSubSystems().renderer;

    // Weltmodus
    renderer.setMatrix(RenderSubSystem::World);
    m_cameraController.look();

    // Texturen zeichnen
    renderer.drawVisualTextureComps();
    // Animationen zeichnen
    renderer.drawVisualAnimationComps();

    renderer.getTextureManager().clear();

    for ( int i = 0; i < m_currentPoint-1; ++i )
        renderer.drawVector( m_clickedPoints[i+1] - m_clickedPoints[i], m_clickedPoints[i] );
    if ( m_currentPoint > 1 )
    {
        renderer.drawVector( m_clickedPoints[0] - m_clickedPoints[m_currentPoint-1], m_clickedPoints[m_currentPoint-1] );
    }
    
    // GUI modus (Grafische Benutzeroberfläche)
    renderer.setMatrix(RenderSubSystem::GUI);
    // Texte zeichnen
    renderer.drawVisualMessageComps();
    
    renderer.drawString( "Editor", "FontW_b", 0.02f, 0.02f );

    if ( m_helpTextOn )
    {
        renderer.drawString( "Left click:\n"
                             "Enter:\n"
                             "Backspace:\n"
                             "Delete:\n"
                             "Page Up/Down:\n"
                             "Arrow keys:\n"
                             "H:",
                             "FontW_s", 3.2f, 0.02f, AlignRight );
        renderer.drawString( "new vertex\n"
                             "apply block\n"
                             "delete last vertex\n"
                             "delete all vetices\n"
                             "change texture\n"
                             "move camera\n"
                             "hide this help text", "FontW_s", 3.3f, 0.02f, AlignLeft );
        renderer.drawString( "* Important *\n"
                             "Only draw convex polygons!\n"
                             "Only draw in counter-clockwise order!\n", "FontW_s", 3.3f, 0.7f, AlignCenter );
    }
    else
    {
        renderer.drawString( "H for help", "FontW_s", 3.2f, 0.02f );
    }

    // Fadenkreuz zeichnen
    Vector2D mousePos = getSubSystems().input.getMousePos();

    renderer.getTextureManager().clear();

    // Aktuelle Textur anzeigen
    {
        float texCoord[8] = { 0.0f, 0.0f,
                             0.0f, 1.0f,
                             1.0f, 1.0f,
                             1.0f, 0.0f };
        float vertexCoord[8] = { 0.1f, 2.5f,
                                 0.1f, 2.9f,
                                 0.5f, 2.9f,
                                 0.5f, 2.5f };
        renderer.drawTexturedQuad( texCoord, vertexCoord, m_currentTexture, true );
        renderer.drawString( m_currentTexture, "FontW_s", 0.09f, 2.91f );
    }

    mousePos = m_cameraController.screenToWorld(mousePos);
    SnapToGrid( mousePos );
    mousePos = m_cameraController.worldToScreen(mousePos);
    mousePos.x = mousePos.x * 4.0f;
    mousePos.y = mousePos.y * 3.0f;

    renderer.drawEditorCursor(mousePos);
}
