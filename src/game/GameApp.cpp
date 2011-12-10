/*
 * GameApp.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "GameApp.h"
#include "main.h"
#include "Configuration.h"
#include "Input.h"
#include "Gui.h"
#include "GameState.h"

#include "states/MainMenuState.h"
#include "states/PlayingState.h"

#include "common/Logger.h"
#include "common/Physics.h"
#include "common/Sound.h"
#include "common/Renderer.h"
#include "common/Component.h"
#include "common/Texture.h"
#include "common/Exception.h"

#include <string>
#include <iostream>
#include <SDL.h>
#include <SDL_opengl.h>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

// Konstruktor
GameApp::GameApp(const std::vector<std::string>& args) :
        m_isInit (false),
        m_subSystems (),
        m_quit ( false ),
        m_eventConnection (),
        m_fpsMeasureStart ( 0 ),
        m_framesCounter ( 0 ),
        m_fps ( 0 ),
        m_startGame ( true ),
        m_fullScreen ( false ),
        m_overRideFullScreen ( false ),
        m_doRestart (false)
{
    m_eventConnection = m_subSystems.events.quitGame.registerListener( boost::bind( &GameApp::onQuit, this, _1 ) );

    parseArguments(args);
}
SubSystems::SubSystems()
      : stateManager (),
        events (),
        input (),
        physics( events ),
        renderer ( events ),
        sound (),
        gui ( renderer, input ),
        playerController (events, input),
        isLoading ( false )
{
}

SubSystems::~SubSystems()
{
}

// TODO: handle bad inits
bool SubSystems::init()
{
    renderer.init( gConfig.get<int>("ScreenWidth"), gConfig.get<int>("ScreenHeight") );
    sound.init(gConfig.get<float>("VolSound"), gConfig.get<float>("VolMusic"), gConfig.get<float>("VolMaster"));
    return true;
}

void SubSystems::deInit()
{
    stateManager.clear();
    sound.deInit();
    renderer.deInit();
}

// Destruktor
GameApp::~GameApp()
{
    deInit();
}

bool GameApp::doRestart()
{
    return m_doRestart;
}

// Alle Objekte von GameApp initialisieren
void GameApp::init()
{
    log(Info) << "* Started initialization *\n\n";  // In Log-Datei schreiben

    //========================= SDL ============================//
    if ( !initSDL() ) // SDL initialisieren
        throw Exception(std::string() + "Error initializing SDL: " + SDL_GetError() + "\n" );

    //======================== Video ===========================//
    if ( !initVideo() ) // Anzeigegeräte bereitstellen
        throw Exception(std::string() + "Error setting up the screen - " + gConfig.get<std::string>("ScreenWidth") + "x" + gConfig.get<std::string>("ScreenHeight") + " " + gConfig.get<std::string>("ScreenBpp") + " Bit\n" + SDL_GetError() + "\n");

    //===================== Untersysteme =======================//
    log(Info) << "Initializing sub systems... ";
    m_subSystems.init();
    log(Info) << "[ Done ]\n";

    // "Loading..." -> Ladungsanzeige zeichnen
    m_subSystems.renderer.displayLoadingScreen();

    // Texturen laden
    m_subSystems.renderer.loadData((TexQuality) gConfig.get<int>("TexQuality"));

    log(Info) << "\n* Finished initialization *\n";

    boost::shared_ptr<GameState> gameState;
    if (m_levelToLoad.empty())
        gameState = boost::shared_ptr<MainMenuState>( new MainMenuState(m_subSystems) );
    else
        gameState = boost::shared_ptr<PlayingState>( new PlayingState(m_subSystems, m_levelToLoad) );
    m_subSystems.stateManager.changeState( gameState );

    m_isInit = true;
}

// SDL und Fenster initialisieren
bool GameApp::initSDL()
{
    log(Info) << "Initializing SDL... ";

    if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) == -1 )       // SDL initialisieren
        return false;

    SDL_WM_SetCaption( GAME_NAME, NULL ); // Fensterbeschriftung
    SDL_ShowCursor( SDL_DISABLE ); // Standart SDL Mauszeiger ausblenden

    // Events, die ignoriert werden können
    SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
    SDL_EventState(SDL_JOYAXISMOTION, SDL_IGNORE);
    SDL_EventState(SDL_JOYBALLMOTION, SDL_IGNORE);
    SDL_EventState(SDL_JOYHATMOTION, SDL_IGNORE);
    SDL_EventState(SDL_JOYBUTTONDOWN, SDL_IGNORE);
    SDL_EventState(SDL_JOYBUTTONUP, SDL_IGNORE);
    SDL_EventState(SDL_VIDEORESIZE, SDL_IGNORE);
    SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);
    SDL_EventState(SDL_MOUSEBUTTONUP, SDL_IGNORE);

    log(Info) << "[ Done ]\n";

    return true;
}

// Anzeige initialisieren
bool GameApp::initVideo()
{
    log(Info) << "Setting up video... ";

    // Einige OpenGL Flags festlegen, bevor wir SDL_SetVideoMode() aufrufen
    SDL_GL_SetAttribute ( SDL_GL_RED_SIZE, gConfig.get<int>("ScreenBpp")/4 );    // Grösse der Rotkomponente im Framebuffer, in Bits
    SDL_GL_SetAttribute ( SDL_GL_GREEN_SIZE, gConfig.get<int>("ScreenBpp")/4 );  // Grösse der Greenkomponente im Framebuffer, in Bits
    SDL_GL_SetAttribute ( SDL_GL_BLUE_SIZE, gConfig.get<int>("ScreenBpp")/4 );   // Grösse der Blaukomponente im Framebuffer, in Bits
    SDL_GL_SetAttribute ( SDL_GL_ALPHA_SIZE, gConfig.get<int>("ScreenBpp")/4 );  // Grösse der Alphakomponente im Framebuffer, in Bits
    SDL_GL_SetAttribute ( SDL_GL_DOUBLEBUFFER, 1 );                              // Double Buffering aktivieren
    int AA = gConfig.get<int>("AntiAliasing");
    if (AA!=0)
    {
        SDL_GL_SetAttribute ( SDL_GL_MULTISAMPLEBUFFERS, 1 );               // Full Screen Anti-Aliasing aktivieren und
        SDL_GL_SetAttribute ( SDL_GL_MULTISAMPLESAMPLES, AA );              // auf AA mal stellen (Bsp. 4x)
    }
    else
        SDL_GL_SetAttribute ( SDL_GL_MULTISAMPLEBUFFERS, 0 );
    SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, gConfig.get<int>("V-Sync") );

    // Videomodus einrichten. Überprüfen ob es Fehler gab.
    Uint32 flags = SDL_OPENGL;
    if ( m_overRideFullScreen )
    {
        if ( m_fullScreen )
            flags |= SDL_FULLSCREEN;
    }
    else if ( gConfig.get<bool>("FullScreen") )
        flags |= SDL_FULLSCREEN;

    // set up screen
    if ( !SDL_SetVideoMode ( gConfig.get<int>("ScreenWidth"), gConfig.get<int>("ScreenHeight"), gConfig.get<int>("ScreenBpp"), flags ) )
        return false;

    log(Info) << "[ Done ]\n\n";

    log(Info) << "Graphic card: " << glGetString(GL_RENDERER) << "(" << glGetString(GL_VENDOR) << ")\n";
    log(Info) << "OpenGL version: " << glGetString(GL_VERSION) << "\n";

    const SDL_VideoInfo* vidInfo = SDL_GetVideoInfo();

    int value = 0;
    SDL_GL_GetAttribute ( SDL_GL_BUFFER_SIZE, &value );
    gConfig.put("ScreenBpp", value);
    log(Info) << "Resolution: " << vidInfo->current_w << "x" << vidInfo->current_h << "\n";
    //log(Info) << "Widescreen: " << (gConfig.get<bool>("WideScreen")?"on":"off") << "\n";
    log(Info) << "Bits per pixel: " << value << "\n";
    SDL_GL_GetAttribute ( SDL_GL_DOUBLEBUFFER, &value );
    log(Info) << "Double buffer: " << (value?"on":"off") << "\n";
    value = 1;
    SDL_GL_GetAttribute ( SDL_GL_MULTISAMPLESAMPLES, &value );
    gConfig.put("AntiAliasing", value);
    if (value==1)
        log(Info) << "Anti-aliasing: off\n";
    else
        log(Info) << "Anti-aliasing: " << value << "x\n";
    SDL_GL_GetAttribute ( SDL_GL_SWAP_CONTROL, &value );
    gConfig.put("V-Sync", value);
    value = 1;
    log(Info) << "V-Sync: " << (value?"on":"off") << "\n\n";

    return true;
}

// Alles deinitialisieren
void GameApp::deInit()
{
    if (m_isInit)
    {
        log(Info) << "* Started deinitialization *\n\n";      // In Log-Datei schreiben

        log(Info) << "Cleaning up SubSystems...";
        m_subSystems.deInit();
        log(Info) << "[ Done ]\n";

        // SDL beenden
        log(Info) << "Shuting down SDL...";
        SDL_Quit();
        log(Info) << "[ Done ]\n";

        log(Info) << "\n* Finished deinitialization *\n";   // In Log-Datei schreiben
        m_isInit = false;
    }
}

// Nach der Initialisation Spiel m_fpsMeasureStarten (d.h. Hauptschleife m_fpsMeasureStarten)
void GameApp::run()
{
    if (!m_startGame)
        return;
    init();
    mainLoop();
    deInit();
}

// Kleine vereinfachungen der Hauptfunktionen
#define FRAME(dt)   m_subSystems.stateManager.getCurrentState()->frame( dt )    // Pro Frame aufgerufen (PC-Abhängig)
#define UPDATE()    m_subSystems.stateManager.getCurrentState()->update()       // Pro Update aufgerufen (60Hz)
#define DRAW(a)     m_subSystems.stateManager.getCurrentState()->draw( a )      // Pro Frame am Ende aufgerufen zum zeichnen

// Hauptschleife des Spieles
void GameApp::mainLoop()
{
    log(Info) << "Main loop started\n";
    m_fpsMeasureStart = SDL_GetTicks();

    // Variablen um Zeit zu messen
    Uint32 currentTimeMsecs = 0;              // momentane Zeit
    Uint32 lastTimeMsecs = 0;                 // Zeit, wo letzes Frame gerendert wurde
    currentTimeMsecs = lastTimeMsecs = SDL_GetTicks(); // Alle mit aktuellen Zeit initialisieren

    float timeAccumulator = 0.0f; // Time Akkumulator: damit eine PC unabhängige interne schlaufe immer mit 60Hz durchlaufen kann

    float deltaTime = 0.0; // Zeit seit letzter Frame -> um Bewegungen in einer konstanten Rate zu aktualisieren

    SDL_Event sdlWindowEvent; // SDL Eingabe-Ereignisse (nur für den Fall, wann der Benutzer das Fenster schliessen will)
    // Die restlichen Eingaben werden in Input.cpp gemacht.

    ////////////////////////////////////////////////////////
    //                                                    //
    //                     MAIN LOOP                      //
    //                                                    //
    ////////////////////////////////////////////////////////

    // Solange der benutzer noch nicht abgebrochen hat
    while ( m_quit == false )
    {
        currentTimeMsecs = SDL_GetTicks();  // Aktualisieren der Zeit
        calcFPS( currentTimeMsecs );        // Berechnet die Aktualisierungsrate

        // Zeiten berechnen
        deltaTime = ( currentTimeMsecs-lastTimeMsecs ) * 0.001f; // Zeit seit letztem Frame in Sekunden berechnen
        lastTimeMsecs = currentTimeMsecs; // Die momentane Zeit in last_time_msecs speichern
        
        bool hasFocus = ((SDL_GetAppState() & SDL_APPINPUTFOCUS) != 0);
        if (hasFocus)
        {
            FRAME( deltaTime ); // Frameabhängige Arbeiten hier durchführen

            timeAccumulator += deltaTime; // time accumulator

            const float cMaxTimeAccumulator = 0.5f;

            // If the game was in a loading state, the accumulator is too big or the windows hanst focus, reset the accumulator
            if ( m_subSystems.isLoading || deltaTime > cMaxTimeAccumulator )
            {
                if (!m_subSystems.isLoading)
                    std::cerr << "time accumulator too big, skipping updates" << std::endl; // TODO: put in log

                timeAccumulator = 0;
                m_subSystems.isLoading = false;

            }

            const float slowMotionDelay = 0.0f; // mainly for testing purpose

            while ( timeAccumulator >= cPhysicsTimeStep + slowMotionDelay )
            {
                UPDATE(); // Hier wird das gesammte Spiel aktualisiert (Physik und Spiellogik)
                timeAccumulator -= cPhysicsTimeStep + slowMotionDelay;
            }

            if (m_quit)
                break; // don't redraw the screen if we are quitting

            //m_subSystems.renderer.clearScreen();
            DRAW( timeAccumulator/(cPhysicsTimeStep + slowMotionDelay)*cPhysicsTimeStep ); // Spiel zeichnen
            m_subSystems.renderer.drawFPS(m_fps);
            m_subSystems.renderer.flipBuffer();
        }

        handleSdlQuitEvents( sdlWindowEvent ); // Sehen ob der Benutzer das fenster schliessen will.
    }

    ////////////////////////////////////////////////////////
    //                                                    //
    //                  END OF MAIN LOOP                  //
    //                                                    //
    ////////////////////////////////////////////////////////

}

void GameApp::onQuit(bool restart)
{
    m_quit = true;
    m_doRestart = restart;
    log(Info) << "User requested to quit, quitting...\n";
    m_subSystems.renderer.displayTextScreen("Closing AstroAttack...");
}

// SDL Fensterereignisse behandeln (ob man das Fenster schliessen will)
void GameApp::handleSdlQuitEvents(SDL_Event& sdlEvent)
{
    while (SDL_PollEvent(&sdlEvent))
    {
        switch (sdlEvent.type)
        {
        // Wenn eine Taste gedrück wurde
        case SDL_KEYDOWN:
            {
                switch (sdlEvent.key.keysym.sym)
                {
                case SDLK_ESCAPE: // ESC gedrückt, quit -> true
                    if (m_subSystems.stateManager.getCurrentState()->getId() == "MainMenuState")
                    {
                        onQuit();
                    }
                    else
                    {
                        boost::shared_ptr<MainMenuState> menuState = boost::shared_ptr<MainMenuState>(new MainMenuState(m_subSystems));
                        m_subSystems.stateManager.changeState(menuState);
                    }
                    break;
                case SDLK_F4:
                    if (sdlEvent.key.keysym.mod & KMOD_LALT) // ALT-F4
                        onQuit();
                    break;

                default:
                    break;
                }
            }
            break;

            // Quit-Ereignis (zum Beispiel das X Knopf drücken), quit -> true
        case SDL_QUIT:
            onQuit();
            break;

        default:
            break;
        }
    }
}

// Framerate aktualisieren (Frames pro Sekunde)
// Einmal pro frame aufrufen
void GameApp::calcFPS( unsigned int curTime )
{
    if ( m_fpsMeasureStart + 1000 < curTime )
    {
        // m_framesCounter ist jetzt die Anzahl Frames in dieser Sekunde, also die FPS
        m_fps = m_framesCounter;
        log(Info) << "FPS: " << m_fps << "\n";
        m_framesCounter = 0;
        m_fpsMeasureStart = SDL_GetTicks();
    }
    ++m_framesCounter;
}

// Programmargumente verarbeiten
void GameApp::parseArguments( const std::vector<std::string>& args )
{
    std::string outputStr;

    for(size_t i=0; i<args.size(); ++i)
    {
        if ( args[i]=="-f" || args[i]=="--full-screen" )
        {
            m_fullScreen = true;
            m_overRideFullScreen = true;
        }
        else if ( args[i]=="-w" || args[i]=="--windowed" )
        {
            m_fullScreen = false;
            m_overRideFullScreen = true;
        }
        else if ( (args[i]=="-l" || args[i]=="--level") && i+1<args.size())
        {
            m_levelToLoad = args[++i];
        }
        else if ( args[i]=="-v" || args[i]=="--version" )
        {
            outputStr = GAME_NAME " " GAME_VERSION "\n";
            m_startGame = false;
            break;
        }
        else
        {
            if ( !(args[i]=="-h" || args[i]=="--help") )
                outputStr = "Argument not recognized: " + args[i] + "\n";
            outputStr += "Usage: " GAME_NAME " [options]\n"
                        " -v, --version          :  show version\n"
                        " -h, --help             :  show help\n"
                        " -l, --level LEVELFILE  :  directly load a level file at startup\n"
                        " -f, --full-screen      :  start in full screen mode\n"
                        "See README.txt for more info.\n";
            m_startGame = false;
            break;
        }
    }

    if ( outputStr != "" )
    {
        std::cout << outputStr;
        std::cout.flush();
    }
}
