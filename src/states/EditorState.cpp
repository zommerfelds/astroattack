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

#include <Box2D/Box2D.h>

#include <cmath>
#include <sstream>

StateIdType EditorState::stateId = "EditorState";

// temp!
#include <SDL/SDL_opengl.h>
#include "../Texture.h"

EditorState::EditorState( SubSystems* pSubSystems )
: GameState( pSubSystems ),
  m_pGameWorld ( new GameWorld( GetSubSystems()->eventManager.get() ) ),
  m_pGameCamera ( new GameCamera( GetSubSystems()->input.get(), GetSubSystems()->renderer.get(), m_pGameWorld.get() ) ),
  m_currentPoint ( 0 ),
  m_currentTexture (),
  m_currentTextureNum ( 0 ),
  m_helpTextOn ( false )
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
    loader.LoadXmlToWorld( gAaConfig.GetString("EditorLevel").c_str() /*"data/Levels/level.xml"*/, m_pGameWorld.get(), GetSubSystems() );
}

void EditorState::Cleanup()     // State abbrechen
{
    // Grafiken aus XML-Datei laden
    XmlLoader xml;
    xml.SaveWorldToXml( gAaConfig.GetString("EditorLevel").c_str() /*"data/Levels/level.xml"*/, m_pGameWorld.get() );

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
    static bool pressedMouse = false;
    if ( GetSubSystems()->input->LMouseKeyState() )
    {
        if ( pressedMouse == false && m_currentPoint < 8 )
        {
            *m_pClickedPoints[m_currentPoint] = m_pGameCamera->ScreenToWorld( *GetSubSystems()->input->MousePos() );
            SnapToGrid( m_pClickedPoints[m_currentPoint].get() );
            ++m_currentPoint;
        }
        pressedMouse = true;
    }
    else
        pressedMouse = false;

    if ( GetSubSystems()->input->KeyState( EditorCancelBlock ) )
    {
        m_currentPoint = 0;
    }

    static bool pressedCancelVertex = false;
    if ( GetSubSystems()->input->KeyState( EditorCancelVertex ) )
    {
        if ( pressedCancelVertex == false && m_currentPoint > 0 )
            --m_currentPoint;
        pressedCancelVertex = true;
    }
    else
        pressedCancelVertex = false;

    static bool pressedCreateEntity = false;
    if ( GetSubSystems()->input->KeyState( EditorCreateEntity ) )
    {
        if ( pressedCreateEntity == false && m_currentPoint > 2 )
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
            boost::shared_ptr<Entity> pEntity( new Entity( entityName ) );

            boost::shared_ptr<CompPhysics> compPhysics( new CompPhysics( new b2BodyDef/*, false*/ ) );
            boost::shared_ptr<b2FixtureDef> fixtureDef ( new b2FixtureDef );
            b2PolygonShape* shape = new b2PolygonShape;
            b2Vec2 vertices[b2_maxPolygonVertices];
            for ( int i = 0; i < m_currentPoint; ++i )
            {
                vertices[i].x = m_pClickedPoints[i]->x;
                vertices[i].y = m_pClickedPoints[i]->y;
            }
            shape->Set( vertices, m_currentPoint );
            fixtureDef->shape = shape;
            fixtureDef->friction = 0.3f;
            compPhysics->AddFixtureDef( fixtureDef, "shape1" );
            pEntity->SetComponent( compPhysics );

            TextureIdType textureName = m_currentTexture;
            boost::shared_ptr<CompVisualTexture> compPolyTex( new CompVisualTexture(textureName) );
            pEntity->SetComponent( compPolyTex );

            m_pGameWorld->AddEntity( pEntity );

            m_currentPoint = 0;
        }
        pressedCreateEntity = true;
    }
    else
        pressedCreateEntity = false;

    static bool pressedEditorNextTexture = false;
    if ( GetSubSystems()->input->KeyState( EditorNextTexture ) )
    {
        if ( !pressedEditorNextTexture )
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
        pressedEditorNextTexture = true;
    }
    else
        pressedEditorNextTexture = false;

    static bool pressedEditorPrevTexture = false;
    if ( GetSubSystems()->input->KeyState( EditorPrevTexture ) )
    {
        if ( !pressedEditorPrevTexture )
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
        pressedEditorPrevTexture = true;
    }
    else
        pressedEditorPrevTexture = false;

    static bool pressedHelp = false;
    if ( GetSubSystems()->input->KeyState( EditorToggleHelp ) )
    {
        if ( pressedHelp == false )
            m_helpTextOn = !m_helpTextOn;
        pressedHelp = true;
    }
    else
        pressedHelp = false;
}

void EditorState::Draw( float accumulator )        // Spiel zeichnen
{
    RenderSubSystem* pRenderer = GetSubSystems()->renderer.get();

    // Bildschirm leeren
    pRenderer->ClearScreen();
    // Weltmodus
    pRenderer->MatrixWorld();        
    // Texturen zeichnen
    pRenderer->DrawVisualTextureComps( accumulator );
    // Animationen zeichnen
    pRenderer->DrawVisualAnimationComps( accumulator );

    pRenderer->GetTextureManager()->Clear();

    glColor4f ( 1.0f, 1.0f, 1.0f, 0.5f );
    for ( int i = 0; i < m_currentPoint-1; ++i )
        pRenderer->DrawVector( *m_pClickedPoints[i+1] - *m_pClickedPoints[i], *m_pClickedPoints[i] );
    if ( m_currentPoint > 1 )
    {
        glColor4f ( 1.0f, 1.0f, 1.0f, 0.25f );
        pRenderer->DrawVector( *m_pClickedPoints[0] - *m_pClickedPoints[m_currentPoint-1], *m_pClickedPoints[m_currentPoint-1] );
    }
    
    /*{
        const b2AABB* bound = GetSubSystems()->physics->GetWorldBound();
        float vertexCoord[8] = { bound->lowerBound.x, bound->lowerBound.y,
                                 bound->lowerBound.x, bound->upperBound.y,
                                 bound->upperBound.x, bound->upperBound.y,
                                 bound->upperBound.x, bound->lowerBound.y };
        pRenderer->DrawColorQuad( vertexCoord, 0.0f, 0.0f, 0.0f, 0.0f, true );
    }*/
    
    // GUI modus (Grafische Benutzeroberfläche)
    pRenderer->MatrixGUI();
    // Texte zeichnen
    pRenderer->DrawVisualMessageComps();
    
    pRenderer->DrawString( "Editor", 0.02f, 0.02f, "FontW_b" );

    if ( m_helpTextOn )
    {
        pRenderer->DrawString( "Linksklick:   Eckpunkt erzeugen", 2.6f, 0.02f, "FontW_s" );
        pRenderer->DrawString( "Enter:   Aus Eckpunkte ein Polygon erzeugen", 2.6f, 0.12f, "FontW_s" );
        pRenderer->DrawString( "Backspace:   Letzter Eckpunkt löschen", 2.6f, 0.22f, "FontW_s" );
        pRenderer->DrawString( "Delete:   Alle aktuelle Eckpunkte löschen", 2.6f, 0.32f, "FontW_s" );
        pRenderer->DrawString( "Page Up/Down:   Textur wählen", 2.6f, 0.42f, "FontW_s" );
        pRenderer->DrawString( "H:   Hilfe ausblenden", 2.6f, 0.52f, "FontW_s" );

        pRenderer->DrawString( "* Wichtig *", 2.6f, 0.72f, "FontW_s" );
        pRenderer->DrawString( "Nur konvexe Polygone!", 2.6f, 0.82f, "FontW_s" );
        pRenderer->DrawString( "Eckpunkte im Gegenuhrzeigersinn erstellen!", 2.6f, 0.92f, "FontW_s" );
    }
    else
    {
        pRenderer->DrawString( "H für Hilfe", 3.5f, 0.02f, "FontW_s" );
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
        pRenderer->DrawString( m_currentTexture, 0.1f, 2.91f, "FontW_s" );
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
