/*----------------------------------------------------------\
|                    PlayingState.cpp                       |
|                    ----------------                       |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
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
#include <SDL/SDL.h>

#include "../main.h"

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

#include <sstream>


// TEMP !
//#include "../Vector2D.h"
#include <SDL/SDL_opengl.h>
#include "../components/CompGravField.h"
#include "../components/CompPhysics.h"
std::string levelName = "";

// Nur um Warnungen zu vermeiden (dass x nicht benutzt wird)
#define NOT_USED(x) x

// Alle Entities in der Welt werden in dieser Datei aufgelistet
const char* cWordLogFileName = "world.txt";

const StateIdType PlayingState::stateId = "PlayingState";

PlayingState::PlayingState( SubSystems* pSubSystems, std::string levelFileName )
: GameState( pSubSystems ),
  m_pGameWorld ( new GameWorld( GetSubSystems()->events.get() ) ),
  m_pGameCamera ( new GameCamera( GetSubSystems()->input.get(), GetSubSystems()->renderer.get(), m_pGameWorld.get() ) ),
  m_eventConnection1 (), m_eventConnection2 (),
  m_curentDeleteSet (1), m_wantToEndGame( false ), m_alphaOverlay( 0.0 ),
  m_levelFileName ( levelFileName )
{
    // TEMP!
    levelName = levelFileName;
}

PlayingState::~PlayingState()
{
}

void PlayingState::Init()        // State starten
{
    //GetSubSystems()->renderer->DisplayLoadingScreen();
    
	SDL_WarpMouse((Uint16)(gAaConfig.GetInt("ScreenWidth")/2), (Uint16)(gAaConfig.GetInt("ScreenHeight")/2));

    m_pGameCamera->Init();
    m_pGameCamera->SetFollowPlayer( true );

    gAaLog.Write ( "Loading world...\n\n" );
    gAaLog.IncreaseIndentationLevel();

    m_pGameWorld->SetVariable( "Collected", 0 );
    m_pGameWorld->SetVariable( "JetpackEnergy", 1000 );

    // Welt von XML-Datei laden
    XmlLoader loader;
    loader.LoadXmlToWorld( "data/player.xml", m_pGameWorld.get(), GetSubSystems() );
    loader.LoadXmlToWorld( /*"data/Levels/level.xml"*/ m_levelFileName.c_str(), m_pGameWorld.get(), GetSubSystems() );
    //loader.LoadXmlToWorld( "data/Levels/editorLevel.xml", m_pGameWorld.get(), GetSubSystems() ); // Level des Editors

    GetSubSystems()->sound->LoadMusic( "data/Music/Aerospace.ogg", "music" );
    GetSubSystems()->sound->PlayMusic( "music", true, 0 );

    gAaLog.DecreaseIndentationLevel();
    gAaLog.Write ( "[ Done loading world ]\n\n" );

    // Alle Entities und Komponenten in Text Datei anzeigen
    Logger log ( cWordLogFileName );
    log.Write( "World Entities:\n\n" );
    m_pGameWorld->WriteWorldToLogger( log );
    log.CloseFile();

    m_eventConnection1 = GetSubSystems()->events->wantToDeleteEntity.RegisterListener( boost::bind( &PlayingState::OnEntityDeleted, this, _1 ) );
    m_eventConnection2 = GetSubSystems()->events->levelEnd.RegisterListener( boost::bind( &PlayingState::OnLevelEnd, this, _1, _2 ) );
}

void PlayingState::Cleanup()     // State abbrechen
{
    GetSubSystems()->sound->StopMusic( 500 );
    GetSubSystems()->sound->FreeMusic( "music" );

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
    GetSubSystems()->input->Update();   // neue Eingaben lesen
    m_pGameCamera->Update( deltaTime ); // Kamera updaten
}

void PlayingState::Update()      // Spiel aktualisieren
{
    if ( m_wantToEndGame )
    {
        if ( m_alphaOverlay > 1.0f )
        {
            boost::shared_ptr<GameOverState> gameOverStateState ( boost::make_shared<GameOverState>( GetSubSystems(), m_gameOverMessage ) );
            GetSubSystems()->stateManager->ChangeState( gameOverStateState );
            return;
        }
        m_alphaOverlay += 0.10f;
    }

    GetSubSystems()->physics->Update();                       // Physik aktualisieren
    GetSubSystems()->events->gameUpdate.Fire();

    if ( m_curentDeleteSet == 1 )
    {
        m_curentDeleteSet = 2;
        for ( std::set< std::string >::iterator it = m_entitiesToDelete1.begin(); it != m_entitiesToDelete1.end(); ++it )
        {
            m_pGameWorld->RemoveEntity( *it );
        }

        m_entitiesToDelete1.clear();
    }
    else
    {
        m_curentDeleteSet = 1;
        for ( std::set< std::string >::iterator it = m_entitiesToDelete2.begin(); it != m_entitiesToDelete2.end(); ++it )
        {
            m_pGameWorld->RemoveEntity( *it );
        }

        m_entitiesToDelete2.clear();
    }
}

void PlayingState::Draw( float accumulator )        // Spiel zeichnen
{
	// maybe put this in PhysicsSubSystem::Update
	// (then the physics subsystem would need an accumulator for itself)
    GetSubSystems()->physics->CalculateSmoothPositions(accumulator);

    glColor4ub( 243, 255, 255, 230 );
    RenderSubSystem* pRenderer = GetSubSystems()->renderer.get();

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
#if 0
    Entity* player = m_pGameWorld->GetEntity("Player").get();
    if ( player )
    {
        CompPhysics* player_phys = static_cast<CompPhysics*>(player->GetFirstComponent("CompPhysics"));
        if ( player_phys )
        {
            const CompGravField* grav = player_phys->GetGravField();
            if ( grav )
            {
                Vector2D vec = grav->GetAcceleration( player_phys->GetBody()->GetWorldCenter() );
                pRenderer->DrawVector( vec*0.1f, player_phys->GetBody()->GetWorldCenter() );
            }
            pRenderer->DrawPoint( player_phys->GetLocalGravitationPoint().Rotated( player_phys->GetBody()->GetAngle() ) + player_phys->GetBody()->GetPosition() );
            pRenderer->DrawPoint( player_phys->GetLocalRotationPoint().Rotated( player_phys->GetBody()->GetAngle() ) + player_phys->GetBody()->GetPosition() );
        }
    }
#endif
    
    // Weltgrenze zeichnen
    /*{
        const b2AABB* bound = GetSubSystems()->physics->GetWorldBound();
        float vertexCoord[8] = { bound->lowerBound.x, bound->lowerBound.y,
                                 bound->lowerBound.x, bound->upperBound.y,
                                 bound->upperBound.x, bound->upperBound.y,
                                 bound->upperBound.x, bound->lowerBound.y };
        pRenderer->DrawColorQuad( vertexCoord, 0.0f, 0.0f, 0.0f, 0.0f, true );
    }*/
    // Fadenkreuz zeichnen
    pRenderer->DrawCrosshairs ( *m_pGameCamera->GetCursorPosInWorld() );
    // GUI modus (Grafische Benutzeroberfläche)
    pRenderer->SetMatrix(RenderSubSystem::GUI);
    // Jetpack %-Anzeige
    {
        float vertexCoord[8] = { 0.5f, 0.08f,
                                0.5f, 0.2f,
                                0.5f + (m_pGameWorld->GetVariable("JetpackEnergy"))/1000.f*1.0f, 0.2f,
                                0.5f + (m_pGameWorld->GetVariable("JetpackEnergy"))/1000.f*1.0f, 0.08f };
        pRenderer->DrawColorQuad( vertexCoord, 0.2f, 0.9f, 0.3f, 0.6f, true );
    }
    // Texte zeichnen
    pRenderer->DrawVisualMessageComps();

    std::stringstream ss;
    ss.precision( 1 );
    ss.setf(std::ios::fixed,std::ios::floatfield);
    ss << m_pGameWorld->GetVariable("JetpackEnergy")/10.0f << "%";
    pRenderer->DrawString( ss.str().c_str(), "FontW_m", 0.55f, 0.133f, AlignLeft, AlignCenter, 1.0f, 1.0f, 1.0f, 1.0f );
    pRenderer->DrawString( "Jetpack", "FontW_m", 0.45f, 0.133f, AlignRight, AlignCenter, 1.0f, 1.0f, 1.0f, 1.0f );

    if ( m_alphaOverlay != 0.0f )
    {
        pRenderer->DrawOverlay( 0.3f, 0.0f, 0.5f, m_alphaOverlay );
    }

    
    // Erzeugtes Bild zeigen
    pRenderer->FlipBuffer(); // (vom Backbuffer zum Frontbuffer wechseln)

    pRenderer->SetMatrix(RenderSubSystem::World);
    pRenderer->SetMatrix(RenderSubSystem::GUI);
    glColor4ub( 244, 255, 255, 230 );
    
}

void PlayingState::OnEntityDeleted( Entity* pEntity )
{
    if ( pEntity )
    {
        std::string id = pEntity->GetId();
        if ( m_curentDeleteSet == 1 )
        {
            m_entitiesToDelete1.insert( id );
        }
        else
        {
            m_entitiesToDelete2.insert( id );
        }
    }
}

void PlayingState::OnLevelEnd(bool /*win*/, std::string msg)
{
    m_wantToEndGame = true;
    m_gameOverMessage = msg;
}

// Astro Attack - Christian Zommerfelds - 2009
