/*
 * PlayingState.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// PlayingState.h für mehr Informationen

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "PlayingState.h"
#include "GameOverState.h"
#include "../World.h"
#include "../Camera.h"
#include "../GameApp.h"
#include "../Renderer.h"
#include "../Physics.h"
#include "../Input.h"
#include "../Entity.h"
#include "../Logger.h"
#include "../GameEvents.h"
#include "../XmlLoader.h"
#include "../Sound.h"

#include "../main.h"

// define this to draw gravitation vector and other
//#define DRAW_DEBUG

#ifdef DRAW_DEBUG
#include "../components/CompPhysics.h"
#include "../components/CompGravField.h"
#endif

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <sstream>

// Alle Entities in der Welt werden in dieser Datei aufgelistet
const char* cWordLogFileName = "world.txt";

const StateIdType PlayingState::stateId = "PlayingState";

PlayingState::PlayingState( SubSystems& subSystems, std::string levelFileName )
: GameState( subSystems ),
  m_pGameWorld ( new GameWorld( GetSubSystems().events.get() ) ),
  m_pGameCamera ( new GameCamera( GetSubSystems().input.get(), GetSubSystems().renderer.get(), m_pGameWorld.get() ) ),
  m_eventConnection1 (), m_eventConnection2 (),
  m_curentDeleteSet (1), m_wantToEndGame( false ), m_alphaOverlay( 0.0 ),
  m_levelFileName ( levelFileName ),
  m_showLoadingScreenAtCleanUp ( true )
{}

PlayingState::~PlayingState() {}

void PlayingState::Init()        // State starten
{
    //GetSubSystems().renderer->DisplayLoadingScreen();
    
	GetSubSystems().input->PutMouseOnCenter();

    m_pGameCamera->Init();
    m_pGameCamera->SetFollowPlayer( true );

    gAaLog.Write ( "Loading world...\n\n" );
    gAaLog.IncreaseIndentationLevel();

    m_pGameWorld->SetVariable( "Collected", 0 );
    m_pGameWorld->SetVariable( "JetpackEnergy", 1000 );
    m_pGameWorld->SetVariable( "Health", 1000 );

    // Welt von XML-Datei laden
    XmlLoader loader;
    loader.LoadXmlToWorld( "data/player.xml", m_pGameWorld.get(), GetSubSystems() );
    loader.LoadXmlToWorld( m_levelFileName.c_str(), m_pGameWorld.get(), GetSubSystems() );

    GetSubSystems().sound->LoadMusic( "data/Music/Aerospace.ogg", "music" );
    GetSubSystems().sound->PlayMusic( "music", true, 0 );

    gAaLog.DecreaseIndentationLevel();
    gAaLog.Write ( "[ Done loading world ]\n\n" );

    // Alle Entities und Komponenten in Text Datei anzeigen
    Logger log ( cWordLogFileName );
    log.Write( "World Entities:\n\n" );
    m_pGameWorld->WriteWorldToLogger( log );
    log.CloseFile();

    m_eventConnection1 = GetSubSystems().events->wantToDeleteEntity.RegisterListener( boost::bind( &PlayingState::OnEntityDeleted, this, _1 ) );
    m_eventConnection2 = GetSubSystems().events->levelEnd.RegisterListener( boost::bind( &PlayingState::OnLevelEnd, this, _1, _2 ) );
}

void PlayingState::Cleanup()     // State abbrechen
{
    // loading screen
    if ( m_showLoadingScreenAtCleanUp )
        GetSubSystems().renderer->DisplayTextScreen("p l e a s e    w a i t");

    GetSubSystems().sound->StopMusic( 500 );
    GetSubSystems().sound->FreeMusic( "music" );

    m_pGameWorld.reset();
    m_pGameCamera.reset();
}

void PlayingState::Pause()       // State anhalten
{
}

void PlayingState::Resume()      // State wiederaufnehmen
{
}

void PlayingState::Frame( float deltaTime )
{
    GetSubSystems().input->Update();   // neue Eingaben lesen
    m_pGameCamera->Update( deltaTime ); // Kamera updaten
}

void PlayingState::Update()      // Spiel aktualisieren
{
    if ( m_wantToEndGame )
    {
        if ( m_alphaOverlay > 1.0f )
        {
            boost::shared_ptr<GameOverState> gameOverStateState( new GameOverState(GetSubSystems(), m_gameOverMessage, m_levelFileName) );
            GetSubSystems().stateManager->ChangeState( gameOverStateState );
            return;
        }
        m_alphaOverlay += 0.10f;
    }

    GetSubSystems().physics->Update();                       // Physik aktualisieren
    GetSubSystems().events->gameUpdate.Fire();

    if ( m_curentDeleteSet == 1 )
    {
        m_curentDeleteSet = 2;

        BOOST_FOREACH(const EntityIdType& id, m_entitiesToDelete1)
            m_pGameWorld->RemoveEntity(id);

        m_entitiesToDelete1.clear();
    }
    else
    {
        m_curentDeleteSet = 1;

        BOOST_FOREACH(const EntityIdType& id, m_entitiesToDelete2)
            m_pGameWorld->RemoveEntity(id);

        m_entitiesToDelete2.clear();
    }
}

void PlayingState::Draw( float accumulator )        // Spiel zeichnen
{
	// maybe put this in PhysicsSubSystem::Update
	// (then the physics subsystem would need an accumulator for itself)
    GetSubSystems().physics->CalculateSmoothPositions(accumulator);

    RenderSubSystem* pRenderer = GetSubSystems().renderer.get();

    // Bildschirm leeren
    pRenderer->ClearScreen();
    // GUI modus (Grafische Benutzeroberfläche)
    pRenderer->SetMatrix(RenderSubSystem::GUI);
    // Hintergrundbild zeichnen
    {
        float texCoord[8] = { 0.0f, 0.0f,
                             0.0f, 1.0f,
                             1.0f, 1.0f,
                             1.0f, 0.0f };
        float vertexCoord[8] = { 0.0f, 0.0f,
                                 0.0f, 3.0f,
                                 4.0f, 3.0f,
                                 4.0f, 0.0f };
        pRenderer->DrawTexturedQuad( texCoord, vertexCoord, "_starfield" );
    }
    // Weltmodus
    pRenderer->SetMatrix(RenderSubSystem::World);
    m_pGameCamera->Look();
    // Animationen zeichnen
    pRenderer->DrawVisualAnimationComps();
    // Texturen zeichnen
    pRenderer->DrawVisualTextureComps();

    // Draw debug info
#ifdef DRAW_DEBUG
    Entity* player = m_pGameWorld->GetEntity("Player").get();
    if ( player )
    {
        CompPhysics* player_phys = player->GetComponent<CompPhysics>();
        if ( player_phys )
        {
            const CompGravField* grav = player_phys->GetActiveGravField();
            Vector2D gravPoint = player_phys->GetLocalGravitationPoint().Rotated( player_phys->GetSmoothAngle() ) + player_phys->GetPosition();
            Vector2D smoothGravPoint = player_phys->GetLocalGravitationPoint().Rotated( player_phys->GetSmoothAngle() ) + player_phys->GetSmoothPosition();
            Vector2D smoothRotPoint = player_phys->GetLocalRotationPoint().Rotated( player_phys->GetSmoothAngle() ) + player_phys->GetSmoothPosition();
            if ( grav )
            {
                Vector2D vec = grav->GetAcceleration( gravPoint );
                pRenderer->DrawVector( vec*0.1f, smoothGravPoint );
            }
            pRenderer->DrawPoint( smoothGravPoint );
            pRenderer->DrawPoint( smoothRotPoint );
        }
    }
#endif
    
    // Fadenkreuz zeichnen
    pRenderer->DrawCrosshairs ( *m_pGameCamera->GetCursorPosInWorld() );
    // GUI modus (Grafische Benutzeroberfläche)
    pRenderer->SetMatrix(RenderSubSystem::GUI);

    // Texte zeichnen
    pRenderer->DrawVisualMessageComps();


    // Jetpack %-display
    {
        float x = 0.45f, y = 0.14;
        float r = 0.2f, g = 0.9f, b = 0.3f;
        float jetpackEnergy = m_pGameWorld->GetVariable("JetpackEnergy")/1000.0f;
        float vertexCoord[8] = { x+0.05f, y-0.06f,
                                 x+0.05f, y+0.06f,
                                 x+0.05f + jetpackEnergy, y+0.06f,
                                 x+0.05f + jetpackEnergy, y-0.06f };
        pRenderer->DrawColorQuad( vertexCoord, r, g, b, 0.6f, true );

        std::stringstream ss;
        ss.precision( 1 );
        ss.setf(std::ios::fixed, std::ios::floatfield);
        ss << jetpackEnergy*100 << "%";
        pRenderer->DrawString( "Jetpack", "FontW_m", x, y, AlignRight, AlignCenter, 1.0f, 1.0f, 1.0f, 1.0f );
        pRenderer->DrawString( ss.str().c_str(), "FontW_m", x+0.1f, y, AlignLeft, AlignCenter, 1.0f, 1.0f, 1.0f, 1.0f );
    }

    // Health %-display
    {
        float x = 0.45f, y = 0.35;
        float r = 0.8f, g = 0.2f, b = 0.3f;
        float health = m_pGameWorld->GetVariable("Health")/1000.0f;
        float vertexCoord[8] = { x+0.05f, y-0.06f,
                                 x+0.05f, y+0.06f,
                                 x+0.05f + health, y+0.06f,
                                 x+0.05f + health, y-0.06f };
        pRenderer->DrawColorQuad( vertexCoord, r, g, b, 0.6f, true );

        std::stringstream ss;
        ss.precision( 1 );
        ss.setf(std::ios::fixed, std::ios::floatfield);
        ss << health*100 << "%";
        pRenderer->DrawString( "Health", "FontW_m", x, y, AlignRight, AlignCenter, 1.0f, 1.0f, 1.0f, 1.0f );
        pRenderer->DrawString( ss.str().c_str(), "FontW_m", x+0.1f, y, AlignLeft, AlignCenter, 1.0f, 1.0f, 1.0f, 1.0f );
    }

    if ( m_alphaOverlay != 0.0f )
    {
        pRenderer->DrawOverlay( cBackgroundColor[0], cBackgroundColor[1], cBackgroundColor[2], m_alphaOverlay );
    }

    
    // Erzeugtes Bild zeigen
    pRenderer->FlipBuffer(); // (vom Backbuffer zum Frontbuffer wechseln)

    pRenderer->SetMatrix(RenderSubSystem::World);
    pRenderer->SetMatrix(RenderSubSystem::GUI);
    
}

void PlayingState::OnEntityDeleted( Entity& entity )
{
    if ( m_curentDeleteSet == 1 )
        m_entitiesToDelete1.insert( entity.GetId() );
    else
        m_entitiesToDelete2.insert( entity.GetId() );
}

void PlayingState::OnLevelEnd(bool /*win*/, const std::string& msg)
{
    m_showLoadingScreenAtCleanUp = false;
    m_wantToEndGame = true;
    m_gameOverMessage = msg;
}
