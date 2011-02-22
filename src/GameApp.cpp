/*
 * GameApp.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "main.h" // wichtige Definitionen und Dateien einbinden
#include "GameApp.h"

#include "Input.h"
#include "Physics.h"
#include "Sound.h"
#include "Renderer.h"
#include "GUI.h"
#include "GameStates.h"
#include "states/MainMenuState.h"
#include "Component.h"
#include "states/PlayingState.h"
#include "Texture.h"
#include "Exception.h" // Ausnahmen im Program (werden in main.cpp eingefangen)

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <string>
#include <iostream>

#include "SDL.h"
// cross platform OpenGL include (provided by SDL)
#include "SDL_opengl.h"

const char* cMainLogFileName = "data/config.xml";

Configuration gAaConfig; // Spieleinstellungen.

// Konstruktor
GameApp::GameApp(const std::vector<std::string>& args) :
        m_pSubSystems ( new SubSystems ),
        m_quit ( false ),
        m_eventConnection (),
        m_fpsMeasureStart ( 0 ),
        m_framesCounter ( 0 ),
        m_fps ( 0 ),
        m_startGame ( true ),
        m_fullScreen ( false ),
        m_overRideFullScreen ( false )
{
    Component::gameEvents = m_pSubSystems->events.get();
    m_eventConnection = m_pSubSystems->events->quitGame.RegisterListener( boost::bind( &GameApp::OnQuit, this ) );

    ParseArguments(args);
}
SubSystems::SubSystems()
      : stateManager ( new StateManager ),
        events ( new GameEvents ),
        input ( new InputSubSystem ),
        physics( new PhysicsSubSystem( events.get() ) ),
        renderer ( new RenderSubSystem( events.get() ) ),
        sound ( new SoundSubSystem() ),
        gui ( new GuiSubSystem( renderer.get(), input.get() ) ),
        isLoading ( false )
{
}

SubSystems::~SubSystems()
{
    stateManager.reset();   // Spielstatus
    input.reset();          // Eingabe
    physics.reset();        // Physik
    renderer.reset();       // Ausgabe
    sound.reset();          // Sound
    gui.reset();            // Grafische Benutzeroberfläche

    // must be deleted at last because other systems may use this system when destructing
    events.reset();         // Spielereignisse
}

// Destruktor
GameApp::~GameApp() {}

// Alle Objekte von GameApp initialisieren
void GameApp::Init()
{
    gAaLog.Write( "* Started initialization *\n\n" );  // In Log-Datei schreiben
    gAaLog.IncreaseIndentationLevel();                  // Text ab jetzt einrücken

    // Einstellung lesen
    gAaConfig.Load(cMainLogFileName);

    //========================= SDL ============================//
    if ( !InitSDL() ) // SDL initialisieren
        throw Exception ( gAaLog.Write ( "Error initializing SDL: %s\n", SDL_GetError() ) );

    //======================== Video ===========================//
    if ( !InitVideo() ) // Anzeigegeräte bereitstellen
        throw Exception ( gAaLog.Write ( "Error setting up the screen - %ix%i %i Bit\n%s\n", gAaConfig.GetInt("ScreenWidth"), gAaConfig.GetInt("ScreenHeight"), gAaConfig.GetInt("ScreenBpp"), SDL_GetError() ) );

    gAaConfig.ApplyConfig();

    //===================== Untersysteme =======================//
    gAaLog.Write ( "Initializing sub systems... " );
    m_pSubSystems->renderer->Init( gAaConfig.GetInt("ScreenWidth"), gAaConfig.GetInt("ScreenHeight") );
    m_pSubSystems->physics->Init();
    m_pSubSystems->sound->Init(); // Sound
    gAaLog.Write ( "[ Done ]\n" );

    // "Loading..." -> Ladungsanzeige zeichnen
    m_pSubSystems->renderer->DisplayLoadingScreen();

    // Texturen laden
    m_pSubSystems->renderer->LoadData();

    gAaLog.DecreaseIndentationLevel(); // Nicht mehr einrücken
    gAaLog.Write ( "\n* Finished initialization *\n" );

    boost::shared_ptr<GameState> gameState;
    if (m_levelToLoad.empty())
        gameState = boost::shared_ptr<MainMenuState>( new MainMenuState(*m_pSubSystems) );
    else
        gameState = boost::shared_ptr<PlayingState>( new PlayingState(*m_pSubSystems, m_levelToLoad) );
    m_pSubSystems->stateManager->ChangeState( gameState );
}

// SDL und Fenster initialisieren
bool GameApp::InitSDL()
{
    gAaLog.Write ( "Initializing SDL... " );

    if ( SDL_Init ( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) == -1 )       // SDL initialisieren
        return false;

    SDL_WM_SetCaption ( GAME_NAME, NULL ); // Fensterbeschriftung
    SDL_ShowCursor ( SDL_DISABLE ); // Standart SDL Mauszeiger ausblenden

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

    gAaLog.Write ( "[ Done ]\n" );

    return true;
}

// Anzeige initialisieren
bool GameApp::InitVideo()
{
    gAaLog.Write ( "Setting up video... " );

    // Einige OpenGL Flags festlegen, bevor wir SDL_SetVideoMode() aufrufen
    SDL_GL_SetAttribute ( SDL_GL_RED_SIZE, gAaConfig.GetInt("ScreenBpp")/4 );    // Grösse der Rotkomponente im Framebuffer, in Bits
    SDL_GL_SetAttribute ( SDL_GL_GREEN_SIZE, gAaConfig.GetInt("ScreenBpp")/4 );  // Grösse der Greenkomponente im Framebuffer, in Bits
    SDL_GL_SetAttribute ( SDL_GL_BLUE_SIZE, gAaConfig.GetInt("ScreenBpp")/4 );   // Grösse der Blaukomponente im Framebuffer, in Bits
    SDL_GL_SetAttribute ( SDL_GL_ALPHA_SIZE, gAaConfig.GetInt("ScreenBpp")/4 );  // Grösse der Alphakomponente im Framebuffer, in Bits
    SDL_GL_SetAttribute ( SDL_GL_DOUBLEBUFFER, 1 );                              // Double Buffering aktivieren
    int AA = gAaConfig.GetInt("AntiAliasing");
    if (AA!=0)
    {
        SDL_GL_SetAttribute ( SDL_GL_MULTISAMPLEBUFFERS, 1 );               // Full Screen Anti-Aliasing aktivieren und
        SDL_GL_SetAttribute ( SDL_GL_MULTISAMPLESAMPLES, AA );              // auf AA mal stellen (Bsp. 4x)
    }
    else
        SDL_GL_SetAttribute ( SDL_GL_MULTISAMPLEBUFFERS, 0 );
    SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, gAaConfig.GetInt("V-Sync") );

    // Videomodus einrichten. Überprüfen ob es Fehler gab.
    Uint32 flags = SDL_OPENGL;
    if ( m_overRideFullScreen )
    {
        if ( m_fullScreen )
            flags |= SDL_FULLSCREEN;
    }
    else if ( gAaConfig.GetInt("FullScreen") )
        flags |= SDL_FULLSCREEN;

    // set up screen
    if ( !SDL_SetVideoMode ( gAaConfig.GetInt("ScreenWidth"), gAaConfig.GetInt("ScreenHeight"), gAaConfig.GetInt("ScreenBpp"), flags ) )
        return false;

    gAaLog.Write ( "[ Done ]\n\n" );

    gAaLog.IncreaseIndentationLevel();
    gAaLog.Write ( "Graphic card: %s (%s)\n", glGetString ( GL_RENDERER ), glGetString ( GL_VENDOR ) );
    gAaLog.Write ( "OpenGL version: %s\n", glGetString ( GL_VERSION ) );

    const SDL_VideoInfo* vidInfo = SDL_GetVideoInfo();

    int value = 0;
    SDL_GL_GetAttribute ( SDL_GL_BUFFER_SIZE, &value );
    gAaConfig.SetInt( "ScreenBpp", value );
    gAaLog.Write ( "Resolution: %dx%d\n", vidInfo->current_w, vidInfo->current_h );
    gAaLog.Write ( "Widescreen: %s\n", (gAaConfig.GetInt("WideScreen")?"on":"off") );
    gAaLog.Write ( "Bits per pixel: %d\n", value );
    SDL_GL_GetAttribute ( SDL_GL_DOUBLEBUFFER, &value );
    gAaLog.Write ( "Double buffer: %s\n", (value?"on":"off") );
    value = 1;
    SDL_GL_GetAttribute ( SDL_GL_MULTISAMPLESAMPLES, &value );
    gAaConfig.SetInt("AntiAliasing",value);
    if (value==1)
        gAaLog.Write ( "Anti-aliasing: off\n" );
    else
        gAaLog.Write ( "Anti-aliasing: %dx\n", value );
    SDL_GL_GetAttribute ( SDL_GL_SWAP_CONTROL, &value );
    gAaConfig.SetInt("V-Sync",value);
    value = 1;
    gAaLog.Write ( "V-Sync: %s\n\n", (value?"on":"off") );
    gAaLog.DecreaseIndentationLevel();

    return true;
}

// Alles deinitialisieren
void GameApp::DeInit()
{
    gAaLog.Write ( "* Started deinitialization *\n\n" );      // In Log-Datei schreiben
    gAaLog.IncreaseIndentationLevel();

    gAaLog.Write ( "Cleaning up SubSystems..." );
    Component::gameEvents = NULL; // set the component's GameEvents pointer to zero,
                                  // so the components know its no longer valid
    // Die Objekten löschen sich eigentlich von selbst. Es wird jedoch gewünscht dass sie jetzt gelöscht werden:
    m_pSubSystems.reset();
    gAaLog.Write ( "[ Done ]\n" );

    // SDL beenden
    gAaLog.Write ( "Shuting down SDL..." );
    SDL_Quit();
    gAaLog.Write ( "[ Done ]\n" );

    gAaConfig.ApplyConfig();
    gAaConfig.Save(cMainLogFileName);

    gAaLog.DecreaseIndentationLevel();
    gAaLog.Write ( "\n* Finished deinitialization *\n" );   // In Log-Datei schreiben
}

// Nach der Initialisation Spiel m_fpsMeasureStarten (d.h. Hauptschleife m_fpsMeasureStarten)
void GameApp::Run()
{
    if (!m_startGame)
        return;
    Init();
    MainLoop();
    DeInit();
}

// Kleine vereinfachungen der Hauptfunktionen
#define FRAME(dt)   m_pSubSystems->stateManager->GetCurrentState()->Frame( dt )    // Pro Frame aufgerufen (PC-Abhängig)
#define UPDATE()    m_pSubSystems->stateManager->GetCurrentState()->Update()       // Pro Update aufgerufen (60Hz)
#define DRAW(a)     m_pSubSystems->stateManager->GetCurrentState()->Draw( a )      // Pro Frame am Ende aufgerufen zum zeichnen

// Hauptschleife des Spieles
void GameApp::MainLoop()
{
    gAaLog.Write ( "Main loop started\n" );
    m_fpsMeasureStart = SDL_GetTicks();

    // Variablen um Zeit zu messen
    Uint32 current_time_msecs = 0;              // momentane Zeit
    Uint32 last_time_msecs = 0;                 // Zeit, wo letzes Frame gerendert wurde
    current_time_msecs = last_time_msecs = SDL_GetTicks(); // Alle mit aktuellen Zeit initialisieren

    float accumulator_secs = 0.0f; // Time Akkumulator: damit eine PC unabhängige interne schlaufe immer mit 60Hz durchlaufen kann

    float delta_time_secs = 0.0; // Zeit seit letzter Frame -> um Bewegungen in einer konstanten Rate zu aktualisieren

    SDL_Event sdl_window_events; // SDL Eingabe-Ereignisse (nur für den Fall, wann der Benutzer das Fenster schliessen will)
    //memset(&sdl_window_events,0,sizeof(SDL_Event));
    // Die restlichen Eingaben werden in Input.cpp gemacht.

    gAaLog.IncreaseIndentationLevel();

    ////////////////////////////////////////////////////////
    //                                                    //
    //            m_fpsMeasureStart DER HAUPTSCHLEIFE                 //
    //                                                    //
    ////////////////////////////////////////////////////////

    // Solange der benutzer noch nicht abgebrochen hat
    while ( m_quit == false )
    {
        current_time_msecs = SDL_GetTicks();  // Aktualisieren der Zeit
        CalcFPS( current_time_msecs );        // Berechnet die Aktualisierungsrate

        // Zeiten berechnen
        delta_time_secs = ( current_time_msecs-last_time_msecs ) * 0.001f; // Zeit seit letztem Frame in Sekunden berechnen
        last_time_msecs = current_time_msecs; // Die momentane Zeit in last_time_msecs speichern

        HandleSdlQuitEvents( sdl_window_events, m_quit ); // Sehen ob der Benutzer das fenster schliessen will.
        
        FRAME( delta_time_secs ); // Frameabhängige Arbeiten hier durchführen

        accumulator_secs += delta_time_secs; // time accumulator

        //gAaLog.Write ( "State: %i\n", (int)SDL_GetAppState() );
        // TEST THIS !!!!!!! (getstate)
        // The update loop stops, but the frame loop does not stop (camera)
        if ( m_pSubSystems->isLoading || delta_time_secs>0.5f || ((SDL_GetAppState()&SDL_APPINPUTFOCUS)==0) ) // falls etwas gerade am laden war, wird der Akkumultor zurückgesetzt,
        {                                                       // damit delta_time_secs nicht ultra gross wird und eine grosse Anzahl von Uptates verlangt wird
            accumulator_secs = 0;
            m_pSubSystems->isLoading = false;
        }
        
        while ( accumulator_secs >= PHYS_DELTA_TIME/*+0.3f*/ )
        {
            UPDATE(); // Hier wird das gesammte Spiel aktualisiert (Physik und Spiellogik)
            accumulator_secs -= PHYS_DELTA_TIME/*+0.3f*/;
        }

        m_pSubSystems->renderer->ClearScreen();
        DRAW( accumulator_secs/*/(PHYS_DELTA_TIME+0.3f)*PHYS_DELTA_TIME*/ ); // Spiel zeichnen
        m_pSubSystems->renderer->DrawFPS(m_fps);
        m_pSubSystems->renderer->FlipBuffer();
    }

    ////////////////////////////////////////////////////////
    //                                                    //
    //             ENDE DER HAUPTSCHLEIFE                 //
    //                                                    //
    ////////////////////////////////////////////////////////

    gAaLog.DecreaseIndentationLevel();

    // Alle offenen States ausschalten, bevor alle SubSysteme gelöscht werden.
    m_pSubSystems->stateManager.reset();
}

// SDL Fensterereignisse behandeln (ob man das Fenster schliessen will)
void GameApp::HandleSdlQuitEvents( SDL_Event& rSdlEvent, bool& rQuit )
{
    while ( SDL_PollEvent( &rSdlEvent ) )
    {
        switch ( rSdlEvent.type )
        {
            // Wenn eine Taste gedrück wurde
        case SDL_KEYDOWN:
        {
            switch ( rSdlEvent.key.keysym.sym )
            {
            case SDLK_ESCAPE: // ESC gedrückt, quit -> true
                if ( m_pSubSystems->stateManager->GetCurrentState()->StateID() == "MainMenuState" )
                {
                    gAaLog.Write ( "User requested to quit, quitting...\n" );
                    rQuit = true;
                }
                else
                {
                    boost::shared_ptr<MainMenuState> menuState = boost::shared_ptr<MainMenuState>( new MainMenuState(*m_pSubSystems) );
                    m_pSubSystems->stateManager->ChangeState( menuState );
                }
                break;
            case SDLK_F4:
                if ( rSdlEvent.key.keysym.mod & KMOD_LALT ) // ALT-F4
                {
                    gAaLog.Write ( "User requested to quit, quitting...\n" );
                    rQuit = true;
                }
                break;

            default:
                break;
            }
        }
        break;

        // Quit-Ereignis (zum Beispiel das X Knopf drücken), quit -> true
        case SDL_QUIT:
        {
            gAaLog.Write ( "User requested to quit, quitting...\n" );
            rQuit = true;
        }
        break;

        default:
            break;
        }
    }
}

// Framerate aktualisieren (Frames pro Sekunde)
// Einmal pro frame aufrufen
void GameApp::CalcFPS( unsigned int curTime )
{
    if ( m_fpsMeasureStart + 1000 < curTime )
    {
        // m_framesCounter ist jetzt die Anzahl Frames in dieser Sekunde, also die FPS
        m_fps = m_framesCounter;
        gAaLog.Write ( "FPS: %i\n", m_fps );
        m_framesCounter = 0;
        m_fpsMeasureStart = SDL_GetTicks();
    }
    ++m_framesCounter;
}

// Programmargumente verarbeiten
void GameApp::ParseArguments( const std::vector<std::string>& args )
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
