/*
 * PlayingState.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */


#include <sstream>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include "PlayingState.h"
#include "GameOverState.h"
#include "MainMenuState.h"
#include "game/GameApp.h"
#include "common/Renderer.h"
#include "common/Physics.h"
#include "game/Input.h"
#include "game/Logger.h"
#include "game/GameEvents.h"
#include "common/DataLoader.h"
#include "common/Sound.h"
#include "common/Vector2D.h"
#include "game/main.h"

// define this to draw gravitation vector and other
//#define DRAW_DEBUG

#ifdef DRAW_DEBUG
#include "common/components/CompPhysics.h"
#include "common/components/CompGravField.h"
#endif

// Alle Entities in der Welt werden in dieser Datei aufgelistet
const std::string cWordLogFileName = "world.txt";

const GameStateId PlayingState::STATE_ID = "PlayingState";

PlayingState::PlayingState( SubSystems& subSystems, const std::string& levelFileName )
: GameState( subSystems ),
  m_gameWorld ( getSubSystems().events ),
  m_cameraController ( getSubSystems().input, getSubSystems().renderer, m_gameWorld.getCompManager() ),
  m_eventConnection1 (), m_eventConnection2 (),
  m_curentDeleteSet (1), m_wantToEndGame( false ), m_alphaOverlay( 0.0 ),
  m_levelFileName ( levelFileName )
{}

void PlayingState::init()        // State starten
{
    //GetSubSystems().renderer.displayLoadingScreen();

    gAaLog.write ( "Loading world...\n\n" );
    gAaLog.increaseIndentationLevel();

    try
    {
        // Welt von XML-Datei laden
        DataLoader::loadWorld( "data/player.info", m_gameWorld, getSubSystems() );
        DataLoader::loadWorld( m_levelFileName, m_gameWorld, getSubSystems() );
    }
    catch (DataLoadException& e)
    {
        // TODO is this a good thing to do?
        // TODO show error
        gAaLog.write("Error loading file : %s", e.getMsg().c_str());
        boost::shared_ptr<MainMenuState> menuState (new MainMenuState(getSubSystems()));
        getSubSystems().stateManager.changeState(menuState);
        return;
    }

    m_gameWorld.setVariable( "Collected", 0 );
    m_gameWorld.setVariable( "JetpackEnergy", 1000 );
    m_gameWorld.setVariable( "Health", 1000 );

    getSubSystems().sound.loadMusic( "data/Music/Aerospace.ogg", "music" );
    getSubSystems().sound.playMusic( "music", true, 0 );

    gAaLog.decreaseIndentationLevel();
    gAaLog.write ( "[ Done loading world ]\n\n" );

    // Alle Entities und Komponenten in Text Datei anzeigen
    Logger log ( cWordLogFileName );
    log.write( "World Entities:\n\n" );
    m_gameWorld.getCompManager().writeEntitiesToLogger( log );
    log.closeFile();

    getSubSystems().input.putMouseOnCenter();

    m_cameraController.init();
    m_cameraController.setFollowPlayer( true );

    m_eventConnection1 = getSubSystems().events.wantToDeleteEntity.registerListener( boost::bind( &PlayingState::onEntityDeleted, this, _1 ) );
    m_eventConnection2 = getSubSystems().events.levelEnd.registerListener( boost::bind( &PlayingState::onLevelEnd, this, _1, _2 ) );
}

void PlayingState::cleanup()     // State abbrechen
{

    getSubSystems().sound.stopMusic( 500 );
    getSubSystems().sound.freeMusic( "music" );
}

void PlayingState::pause()       // State anhalten
{
}

void PlayingState::resume()      // State wiederaufnehmen
{
}

void PlayingState::frame( float deltaTime )
{
    getSubSystems().input.update();   // neue Eingaben lesen
    m_cameraController.update( deltaTime ); // Kamera updaten
}

void PlayingState::update()      // Spiel aktualisieren
{
    if ( m_wantToEndGame )
    {
        if ( m_alphaOverlay > 1.0f )
        {
            boost::shared_ptr<GameOverState> gameOverStateState( new GameOverState(getSubSystems(), m_gameOverMessage, m_levelFileName) );
            getSubSystems().stateManager.changeState( gameOverStateState );
            return;
        }
        m_alphaOverlay += 0.10f;
    }

    getSubSystems().physics.update();                       // Physik aktualisieren

    getSubSystems().events.gameUpdate.fire();

    if ( m_curentDeleteSet == 1 )
    {
        m_curentDeleteSet = 2;

        BOOST_FOREACH(const EntityIdType& id, m_entitiesToDelete1)
            m_gameWorld.getCompManager().removeEntity(id);

        m_entitiesToDelete1.clear();
    }
    else
    {
        m_curentDeleteSet = 1;

        BOOST_FOREACH(const EntityIdType& id, m_entitiesToDelete2)
            m_gameWorld.getCompManager().removeEntity(id);

        m_entitiesToDelete2.clear();
    }
}

void PlayingState::draw( float accumulator )        // Spiel zeichnen
{
	// maybe put this in PhysicsSubSystem::Update
	// (then the physics subsystem would need an accumulator for itself)
    getSubSystems().physics.calculateSmoothPositions(accumulator);

    RenderSubSystem& renderer = getSubSystems().renderer;

    // Bildschirm leeren
    //renderer.ClearScreen();
    // GUI modus (Grafische Benutzeroberfläche)
    renderer.setMatrix(RenderSubSystem::GUI);
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
        renderer.drawTexturedQuad( texCoord, vertexCoord, "_starfield" ); // sky
    }
    // Weltmodus
    renderer.setMatrix(RenderSubSystem::World);
    m_cameraController.look();
    // Animationen zeichnen
    renderer.drawVisualAnimationComps();
    // Texturen zeichnen
    renderer.drawVisualTextureComps();

    // Draw debug info
#ifdef DRAW_DEBUG
    // TODO use CompPosition
    CompPhysics* player_phys = m_gameWorld.getCompManager().getComponent<CompPhysics>("Player");
    if ( player_phys )
    {
        const CompGravField* grav = player_phys->getActiveGravField();
        Vector2D gravPoint = player_phys->getLocalGravitationPoint().rotated( player_phys->getSmoothAngle() ) + player_phys->getSmoothPosition();
        Vector2D smoothGravPoint = player_phys->getLocalGravitationPoint().rotated( player_phys->getSmoothAngle() ) + player_phys->getSmoothPosition();
        if ( grav )
        {
            Vector2D vec = grav->getAcceleration( gravPoint );
            renderer.drawVector( vec*0.1f, smoothGravPoint );
        }
        renderer.drawPoint( smoothGravPoint );

        // draw contacts
        //ContactVector contacts = player_phys->getContacts();
        /*BOOST_FOREACH (boost::shared_ptr<ContactInfo> contact, contacts)
        {
            renderer.drawPoint( contact->point );
        }*/
    }
#endif
    
    // Fadenkreuz zeichnen
    renderer.drawCrosshairs ( m_cameraController.screenToWorld(getSubSystems().input.getMousePos()) );
    // GUI modus (Grafische Benutzeroberfläche)
    renderer.setMatrix(RenderSubSystem::GUI);

    // Texte zeichnen
    renderer.drawVisualMessageComps();

    // Jetpack %-display
    {
        float x = 0.45f, y = 0.14;
        float r = 0.2f, g = 0.9f, b = 0.3f;
        float jetpackEnergy = m_gameWorld.getVariable("JetpackEnergy")/1000.0f;
        float vertexCoord[8] = { x+0.05f, y-0.06f,
                                 x+0.05f, y+0.06f,
                                 x+0.05f + jetpackEnergy, y+0.06f,
                                 x+0.05f + jetpackEnergy, y-0.06f };
        renderer.drawColorQuad( vertexCoord, r, g, b, 0.6f, true );

        std::stringstream ss;
        ss.precision( 1 );
        ss.setf(std::ios::fixed, std::ios::floatfield);
        ss << jetpackEnergy*100 << "%";
        renderer.drawString( "Jetpack", "FontW_m", x, y, AlignRight, AlignCenter, 1.0f, 1.0f, 1.0f, 1.0f );
        renderer.drawString( ss.str(), "FontW_m", x+0.1f, y, AlignLeft, AlignCenter, 1.0f, 1.0f, 1.0f, 1.0f );
    }

    // Health %-display
    {
        float x = 0.45f, y = 0.35;
        float r = 0.8f, g = 0.2f, b = 0.3f;
        float health = m_gameWorld.getVariable("Health")/1000.0f;
        float vertexCoord[8] = { x+0.05f, y-0.06f,
                                 x+0.05f, y+0.06f,
                                 x+0.05f + health, y+0.06f,
                                 x+0.05f + health, y-0.06f };
        renderer.drawColorQuad( vertexCoord, r, g, b, 0.6f, true );

        std::stringstream ss;
        ss.precision( 1 );
        ss.setf(std::ios::fixed, std::ios::floatfield);
        ss << health*100 << "%";
        renderer.drawString( "Health", "FontW_m", x, y, AlignRight, AlignCenter, 1.0f, 1.0f, 1.0f, 1.0f );
        renderer.drawString( ss.str(), "FontW_m", x+0.1f, y, AlignLeft, AlignCenter, 1.0f, 1.0f, 1.0f, 1.0f );
    }

    if ( m_alphaOverlay != 0.0f )
    {
        renderer.drawOverlay( cBackgroundColor[0], cBackgroundColor[1], cBackgroundColor[2], m_alphaOverlay );
    }

    // Erzeugtes Bild zeigen
    //renderer.FlipBuffer(); // (vom Backbuffer zum Frontbuffer wechseln)
}

void PlayingState::onEntityDeleted( const EntityIdType& entityId )
{
    if ( m_curentDeleteSet == 1 )
        m_entitiesToDelete1.insert( entityId );
    else
        m_entitiesToDelete2.insert( entityId );
}

void PlayingState::onLevelEnd(bool /*win*/, const std::string& msg)
{
    m_wantToEndGame = true;
    m_gameOverMessage = msg;
}
