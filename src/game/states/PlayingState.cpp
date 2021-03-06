/*
 * PlayingState.cpp
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */

#include "PlayingState.h"

#include "GameOverState.h"
#include "MainMenuState.h"

#include "game/GameApp.h"
#include "game/Input.h"
#include "game/main.h"

#include "common/components/CompVariable.h"
#include "common/Logger.h"
#include "common/GameEvents.h"
#include "common/DataLoader.h"
#include "common/Sound.h"
#include "common/Vector2D.h"
#include "common/Renderer.h"
#include "common/Physics.h"
#include "common/Foreach.h"

#include <sstream>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

// define this to draw gravitation vector and other
//#define DRAW_DEBUG

#ifdef DRAW_DEBUG
#include "common/components/CompPhysics.h"
#include "common/components/CompGravField.h"
#include "common/components/CompPosition.h"
#endif

// Alle Entities in der Welt werden in dieser Datei aufgelistet
const std::string cWordLogFileName = "world.txt";

const GameStateId PlayingState::STATE_ID = "PlayingState";

PlayingState::PlayingState( SubSystems& subSystems, const std::string& levelFileName )
: GameState( subSystems ),
  m_compMgr ( getSubSystems().events ),
  m_cameraController ( getSubSystems().input, getSubSystems().renderer, m_compMgr ),
  m_eventConnection1 (), m_eventConnection2 (),
  m_curentDeleteSet (1), m_wantToEndGame( false ), m_alphaOverlay( 0.0 ),
  m_levelFileName ( levelFileName )
{}

void PlayingState::init()        // State starten
{
    //getSubSystems().renderer.displayLoadingScreen();

    log(Info) << "Loading world...\n\n";

    try
    {
        // Welt von XML-Datei laden
        DataLoader::loadToWorld( "data/player.info", m_compMgr, getSubSystems().events );
        DataLoader::loadToWorld( m_levelFileName, m_compMgr, getSubSystems().events );
    }
    catch (DataLoadException& e)
    {
        // TODO show error in game
        log(Error) << e.getMsg() << "\n";
        boost::shared_ptr<MainMenuState> menuState (new MainMenuState(getSubSystems()));
        getSubSystems().stateManager.changeState(menuState);
        return;
    }

    CompVariable* varJetpackEnergy = m_compMgr.getComponent<CompVariable>("Player", "JetpackEnergy");
    CompVariable* varHealth = m_compMgr.getComponent<CompVariable>("Player", "Health");
    if (varJetpackEnergy && varHealth) {
        varJetpackEnergy->setValue(1000);
        varHealth->setValue(1000);
    }
    else {
        log(Error) << "Missing 'JetpackEnergy' or 'Health' CompVariable in Player" << "\n";
        // TODO: show error
        boost::shared_ptr<MainMenuState> menuState (new MainMenuState(getSubSystems()));
        getSubSystems().stateManager.changeState(menuState);
        return;
    }

    getSubSystems().sound.loadMusic( "data/Music/Aerospace.ogg", "music" );
    getSubSystems().sound.playMusic( "music", true, 0 );

    log(Info) << "[ Done loading world ]\n\n";

    // Alle Entities und Komponenten in Text Datei anzeigen
    Logger logger(true);
    boost::shared_ptr<FileHandler> fileHandler = boost::make_shared<FileHandler>(cWordLogFileName);
    logger.addHandler(fileHandler);
    logger << "World Entities:\n\n";
    m_compMgr.writeEntitiesToLogger(logger, Info);

    getSubSystems().input.warpMouseToCenter();

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
    getSubSystems().renderer.update();
    getSubSystems().playerController.update();

    //getSubSystems().events.gameUpdate.fire();

    if ( m_curentDeleteSet == 1 )
    {
        m_curentDeleteSet = 2;

        foreach(const EntityId& id, m_entitiesToDelete1)
            m_compMgr.removeEntity(id);

        m_entitiesToDelete1.clear();
    }
    else
    {
        m_curentDeleteSet = 1;

        foreach(const EntityId& id, m_entitiesToDelete2)
            m_compMgr.removeEntity(id);

        m_entitiesToDelete2.clear();
    }
}

void PlayingState::draw( float accumulator )        // Spiel zeichnen
{
    // maybe put this in PhysicsSubSystem::Update
    // (then the physics subsystem would need an accumulator for itself)
    getSubSystems().physics.calculateSmoothPositions(accumulator);

    RenderSystem& renderer = getSubSystems().renderer;

    // Bildschirm leeren
    //renderer.ClearScreen();
    // GUI modus (Grafische Benutzeroberfläche)
    renderer.setMatrix(RenderSystem::GUI);
    // Hintergrundbild zeichnen
    {
        float texCoord[8] = { 0.0f, 0.0f,
                              0.0f, 1.0f,
                              1.0f, 1.0f,
                              1.0f, 0.0f };
        float* const & vertexCoord = texCoord;
        renderer.drawTexturedQuad( texCoord, vertexCoord, "_starfield" ); // sky
    }
    // Weltmodus
    renderer.setMatrix(RenderSystem::World);
    m_cameraController.look();
    // Animationen zeichnen
    renderer.drawVisualAnimationComps();
    // Texturen zeichnen
    renderer.drawVisualTextureComps();

    // Draw debug info
#ifdef DRAW_DEBUG
    CompPhysics* playerPhys = m_compMgr.getComponent<CompPhysics>("Player");
    CompPosition* playerPos = m_compMgr.getComponent<CompPosition>("Player");
    if ( playerPhys && playerPos )
    {
        const CompGravField* grav = playerPhys->getActiveGravField();
        Vector2D gravPoint = playerPhys->getLocalGravitationPoint().rotated( playerPos->getOrientation() ) + playerPos->getPosition();
        Vector2D smoothGravPoint = playerPhys->getLocalGravitationPoint().rotated( playerPos->getDrawingOrientation() ) + playerPos->getDrawingPosition();
        if ( grav )
        {
            Vector2D vec = grav->getAcceleration( gravPoint );
            renderer.drawVector( vec*0.1f, smoothGravPoint );
        }
        renderer.drawPoint( smoothGravPoint );

        // draw contacts
        ContactVector contacts = playerPhys->getContacts();
        foreach (boost::shared_ptr<ContactInfo> contact, contacts)
        {
            renderer.drawPoint( contact->point );
        }
    }
#endif
    
    // Fadenkreuz zeichnen
    renderer.drawCrosshairs( m_cameraController.screenToWorld(getSubSystems().input.getMousePos()) );
    // GUI modus (Grafische Benutzeroberfläche)
    renderer.setMatrix(RenderSystem::GUI);

    // Texte zeichnen
    renderer.drawVisualMessageComps();

    // Jetpack %-display
    {
        float x = 0.11f, y = 0.04f;
        float r = 0.2f, g = 0.9f, b = 0.3f, a = 0.6f;
        float jetpackEnergy = m_compMgr.getComponent<CompVariable>("Player", "JetpackEnergy")->getValue()/1000.0f;
        float vertexCoord[8] = { x+0.025f, y-0.02f,
                                 x+0.025f, y+0.02f,
                                 x+0.025f + jetpackEnergy * 0.3f, y+0.02f,
                                 x+0.025f + jetpackEnergy * 0.3f, y-0.02f };
        renderer.drawColorQuad(vertexCoord, r, g, b, a, true);

        std::stringstream ss;
        ss.precision( 1 );
        ss.setf(std::ios::fixed, std::ios::floatfield);
        ss << jetpackEnergy*100 << "%";
        renderer.drawString( "Jetpack", "FontW_m", x, y, AlignRight, AlignCenter, 1.0f, 1.0f, 1.0f, 1.0f );
        renderer.drawString( ss.str(), "FontW_m", x+0.035f, y, AlignLeft, AlignCenter, 1.0f, 1.0f, 1.0f, 1.0f );
    }

    // Health %-display
    {
        float x = 0.11f, y = 0.09f;
        float r = 0.8f, g = 0.2f, b = 0.3f, a = 0.6f;
        float health = m_compMgr.getComponent<CompVariable>("Player", "Health")->getValue()/1000.0f;
        float vertexCoord[8] = { x+0.025f, y-0.02f,
                                 x+0.025f, y+0.02f,
                                 x+0.025f + health * 0.3f, y+0.02f,
                                 x+0.025f + health * 0.3f, y-0.02f };
        renderer.drawColorQuad( vertexCoord, r, g, b, a, true );

        std::stringstream ss;
        ss.precision( 1 );
        ss.setf(std::ios::fixed, std::ios::floatfield);
        ss << health*100 << "%";
        renderer.drawString( "Health", "FontW_m", x, y, AlignRight, AlignCenter, 1.0f, 1.0f, 1.0f, 1.0f );
        renderer.drawString( ss.str(), "FontW_m", x+0.037f, y, AlignLeft, AlignCenter, 1.0f, 1.0f, 1.0f, 1.0f );
    }

    if ( m_alphaOverlay != 0.0f )
    {
        renderer.drawOverlay( cBackgroundColor[0], cBackgroundColor[1], cBackgroundColor[2], m_alphaOverlay );
    }

    // Erzeugtes Bild zeigen
    //renderer.FlipBuffer(); // (vom Backbuffer zum Frontbuffer wechseln)
}

void PlayingState::onEntityDeleted( const EntityId& entityId )
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
