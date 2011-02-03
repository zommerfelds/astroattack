/*----------------------------------------------------------\
|                    EditorState.cpp                        |
|                    ---------------                        |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "EditorState.h"

#include "../GameApp.h"
#include "../Vector2D.h"
#include "../World.h"
#include "../Camera.h"
#include "../Renderer.h"
#include "../Input.h"
#include "../main.h"
#include "../XmlLoader.h"
#include "../Physics.h"

#include "../components/CompVisualTexture.h"
#include "../components/CompPhysics.h"
#include "../components/CompShape.h"
#include "../components/CompPosition.h"

#include <cmath>
#include <sstream>

#include <boost/make_shared.hpp>

const StateIdType EditorState::stateId = "EditorState";

// temp!
#include <SDL/SDL_opengl.h>
#include "../Texture.h"

EditorState::EditorState( SubSystems* pSubSystems )
: GameState( pSubSystems ),
  m_pGameWorld ( new GameWorld( GetSubSystems()->events.get() ) ),
  m_pGameCamera ( new GameCamera( GetSubSystems()->input.get(), GetSubSystems()->renderer.get(), m_pGameWorld.get() ) ),
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
    for ( int i = 0; i < 8; ++i )
    {
        m_pClickedPoints[i].reset( new Vector2D );
    }

    std::vector<std::string> texList ( GetSubSystems()->renderer->GetTextureManager()->GetTextureList() );
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
        gAaLog.Write ( "Warning: No textures found for editor!\n" );
}

EditorState::~EditorState()
{
}

void EditorState::Init()        // State starten
{
    m_pGameCamera->Init();
    m_pGameCamera->SetFollowPlayer ( false );

    XmlLoader loader;
    loader.LoadXmlToWorld( gAaConfig.GetString("EditorLevel").c_str(), m_pGameWorld.get(), GetSubSystems() );
}

void EditorState::Cleanup()     // State abbrechen
{
    // Grafiken aus XML-Datei laden
    XmlLoader xml;
    xml.SaveWorldToXml( gAaConfig.GetString("EditorLevel").c_str(), m_pGameWorld.get() );

    m_pGameWorld.reset();
    m_pGameCamera.reset();
}

void EditorState::Pause()       // State anhalten
{
}

void EditorState::Resume()      // State wiederaufnehmen
{
}

void EditorState::Frame( float deltaTime )       // Pro Frame
{
    GetSubSystems()->input->Update();   // neue Eingaben lesen
    m_pGameCamera->Update( deltaTime ); // Kamera updaten
}

void SnapToGrid( Vector2D* worldCoordinates )
{
    const float cellWidth = 0.5f;

    float cellsX = worldCoordinates->x/cellWidth;
    if ( cellsX > 0 )
        cellsX += 0.5;
    else
        cellsX -= 0.5;
    worldCoordinates->x = ((int)(cellsX))*cellWidth;

    float cellsY = worldCoordinates->y/cellWidth;
    if ( cellsY > 0 )
        cellsY += 0.5;
    else
        cellsY -= 0.5;
    worldCoordinates->y = ((int)(cellsY))*cellWidth;
}

void EditorState::Update()      // Spiel aktualisieren
{
    if ( GetSubSystems()->input->LMouseKeyState() )
    {
        if ( m_mouseButDownOld == false && m_currentPoint < 8 )
        {
            *m_pClickedPoints[m_currentPoint] = m_pGameCamera->ScreenToWorld( *GetSubSystems()->input->MousePos() );
            SnapToGrid( m_pClickedPoints[m_currentPoint].get() );
            ++m_currentPoint;
        }
        m_mouseButDownOld = true;
    }
    else
        m_mouseButDownOld = false;

    if ( GetSubSystems()->input->KeyState( EditorCancelBlock ) )
    {
        m_currentPoint = 0;
    }

    if ( GetSubSystems()->input->KeyState( EditorCancelVertex ) )
    {
        if ( m_cancelVertexKeyDownOld == false && m_currentPoint > 0 )
            --m_currentPoint;
        m_cancelVertexKeyDownOld = true;
    }
    else
        m_cancelVertexKeyDownOld = false;

    if ( GetSubSystems()->input->KeyState( EditorCreateEntity ) )
    {
        if ( m_createEntityKeyDownOld == false && m_currentPoint > 2 )
        {
            std::string entityName;
            for ( int i = 0;; ++i )
            {
                std::stringstream ss;
                ss << "EdBlock";
                ss.fill('0');
                ss.width(5);
                ss << i;
                if ( !m_pGameWorld->GetEntity( ss.str() ) )
                {
                    entityName = ss.str();
                    break;
                }
            }
            boost::shared_ptr<Entity> pEntity = boost::make_shared<Entity>(entityName);

            boost::shared_ptr<CompPhysics> compPhysics = boost::make_shared<CompPhysics>();
            boost::shared_ptr<ShapeDef> shapeDef = boost::make_shared<ShapeDef>();
            shapeDef->friction = 0.3f;
            shapeDef->compName = "shape1";
            compPhysics->AddShapeDef( shapeDef );
            pEntity->AddComponent( compPhysics );

            boost::shared_ptr<CompShapePolygon> compShape = boost::make_shared<CompShapePolygon>();
            compShape->SetName("shape1");
            for ( int i = 0; i < m_currentPoint; ++i )
            {
            	compShape->SetVertex(i, *m_pClickedPoints[i]);
            }
            pEntity->AddComponent( compShape );

            boost::shared_ptr<CompPosition> compPos = boost::make_shared<CompPosition>();
            pEntity->AddComponent( compPos );

            TextureIdType textureName = m_currentTexture;
            boost::shared_ptr<CompVisualTexture> compPolyTex = boost::make_shared<CompVisualTexture>(textureName);
            pEntity->AddComponent( compPolyTex );

            m_pGameWorld->AddEntity( pEntity );

            m_currentPoint = 0;
        }
        m_createEntityKeyDownOld = true;
    }
    else
        m_createEntityKeyDownOld = false;

    if ( GetSubSystems()->input->KeyState( EditorNextTexture ) )
    {
        if ( !m_nextTextureKeyDownOld )
        {
            unsigned int old_currentTextureNum = m_currentTextureNum;
            std::vector<std::string> texList ( GetSubSystems()->renderer->GetTextureManager()->GetTextureList() );
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

    if ( GetSubSystems()->input->KeyState( EditorPrevTexture ) )
    {
        if ( !m_prevTextureKeyDownOld )
        {
            std::vector<std::string> texList ( GetSubSystems()->renderer->GetTextureManager()->GetTextureList() );
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

    if ( GetSubSystems()->input->KeyState( EditorToggleHelp ) )
    {
        if ( m_helpKeyDownOld == false )
            m_helpTextOn = !m_helpTextOn;
        m_helpKeyDownOld = true;
    }
    else
        m_helpKeyDownOld = false;
}

void EditorState::Draw( float accumulator )        // Spiel zeichnen
{
    GetSubSystems()->physics->CalculateSmoothPositions(accumulator);

    RenderSubSystem* pRenderer = GetSubSystems()->renderer.get();

    // Bildschirm leeren
    pRenderer->ClearScreen();
    // Weltmodus
    pRenderer->SetMatrix(RenderSubSystem::World);
    m_pGameCamera->Look();

    glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
    // Texturen zeichnen
    pRenderer->DrawVisualTextureComps();
    // Animationen zeichnen
    pRenderer->DrawVisualAnimationComps();

    pRenderer->GetTextureManager()->Clear();

    glColor4f ( 1.0f, 1.0f, 1.0f, 0.5f );
    for ( int i = 0; i < m_currentPoint-1; ++i )
        pRenderer->DrawVector( *m_pClickedPoints[i+1] - *m_pClickedPoints[i], *m_pClickedPoints[i] );
    if ( m_currentPoint > 1 )
    {
        glColor4f ( 1.0f, 1.0f, 1.0f, 0.25f );
        pRenderer->DrawVector( *m_pClickedPoints[0] - *m_pClickedPoints[m_currentPoint-1], *m_pClickedPoints[m_currentPoint-1] );
    }
    
    // GUI modus (Grafische Benutzeroberfläche)
    pRenderer->SetMatrix(RenderSubSystem::GUI);
    // Texte zeichnen
    pRenderer->DrawVisualMessageComps();
    
    pRenderer->DrawString( "Editor", "FontW_b", 0.02f, 0.02f );

    if ( m_helpTextOn )
    {
        pRenderer->DrawString( "Left click:\n"
                               "Enter:\n"
                               "Backspace:\n"
                               "Delete:\n"
                               "Page Up/Down:\n"
                               "H:", "FontW_s", 3.3, 0.02f, AlignRight );
        pRenderer->DrawString( "new vertex\n"
                               "apply block\n"
                               "delete last vertex\n"
                               "delete all vetices\n"
                               "change texture\n"
                               "hide this help text", "FontW_s", 3.4f, 0.02f, AlignLeft );
        pRenderer->DrawString( "* Important *\n"
                               "Only draw convex polygons!\n"
                               "Only draw in counter-clockwise order!\n", "FontW_s", 3.4f, 0.7f, AlignCenter );
    }
    else
    {
        pRenderer->DrawString( "H for help", "FontW_s", 3.5f, 0.02f );
    }

    // Fadenkreuz zeichnen
    Vector2D mousePos ( *GetSubSystems()->input->MousePos() );

    pRenderer->GetTextureManager()->Clear();

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
        pRenderer->DrawTexturedQuad( texCoord, vertexCoord, m_currentTexture, true );
        pRenderer->DrawString( m_currentTexture, "FontW_s", 0.09f, 2.91f );
    }

    mousePos = m_pGameCamera->ScreenToWorld(mousePos);
    SnapToGrid( &mousePos );
    mousePos = m_pGameCamera->WorldToScreen( mousePos );
    mousePos.x = mousePos.x * 4.0f;
    mousePos.y = mousePos.y * 3.0f;

    pRenderer->DrawEditorCursor( mousePos );

    // Erzeugtes Bild zeigen
    pRenderer->FlipBuffer(); // (vom Backbuffer zum Frontbuffer wechseln)
}

// Astro Attack - Christian Zommerfelds - 2009
