/*----------------------------------------------------------\
|                       GameApp.cpp                         |
|                       -----------                         |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "main.h" // wichtige Definitionen und Dateien einbinden
#include "GameApp.h"

#include "Input.h"
#include "Physics.h"
#include "World.h"
#include "Camera.h"
#include "Sound.h"
#include "Renderer.h"
#include "GUI.h"
#include "GameStates.h"
#include "states/MainMenuState.h"
#include "Component.h"

// TEMP
#include "states/PlayingState.h"

#include "Exception.h" // Ausnahmen im Program (werden in main.cpp eingefangen)

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <string>

const char* cMainLogFileName = "data/config.xml";

Configuration gAaConfig; // Spieleinstellungen.

// Konstruktor
GameApp::GameApp() : m_isInit ( false ),
        m_pSubSystems ( new SubSystems ),
        m_quit ( false ),
        m_eventConnection (),
        m_fpsMeasureStart ( 0 ),
        m_framesCounter ( 0 )
{
    Component::gameEvents = m_pSubSystems->events.get();
    m_eventConnection = m_pSubSystems->events->quitGame.RegisterListener( boost::bind( &GameApp::OnQuit, this ) );
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
GameApp::~GameApp()
{
    if ( m_isInit )
        DeInit();
}

// Alle Objekte von GameApp initialisieren
void GameApp::Init( int argc, char* args[] )
{
    gAaLog.Write ( "* Started initialization *\n\n" );  // In Log-Datei schreiben
    gAaLog.IncreaseIndentationLevel();                  // Text ab jetzt einrücken

    // Einstellung lesen
    gAaConfig.Load(cMainLogFileName);

    ParseArguments( argc, args );

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

    boost::shared_ptr<MainMenuState> menuState ( boost::make_shared<MainMenuState>( m_pSubSystems.get() ) );
    m_pSubSystems->stateManager->ChangeState( menuState );
    //boost::shared_ptr<PlayingState> playState ( new PlayingState( m_pSubSystems.get() ) );
    //m_pSubSystems->stateManager->ChangeState( playState );

    // Ja, initialisation war erfolgreich!
    m_isInit = true;
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
    m_isInit = false;
}

// Nach der Initialisation Spiel m_fpsMeasureStarten (d.h. Hauptschleife m_fpsMeasureStarten)
void GameApp::Run()
{
    if ( m_isInit == false )
        throw Exception ( gAaLog.Write ( "Error in GameApp::Run(): Game is not initialized!" ) );
    MainLoop();
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

        DRAW( accumulator_secs/*/(PHYS_DELTA_TIME+0.3f)*PHYS_DELTA_TIME*/ ); // Spiel zeichnen
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
                    boost::shared_ptr<MainMenuState> menuState ( boost::make_shared<MainMenuState>( m_pSubSystems.get() ) );
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
void GameApp::CalcFPS( Uint32 curTime )
{
    if ( (signed)m_fpsMeasureStart + 1000 < (signed long long)curTime )
    {
        // m_framesCounter ist jetzt die Anzahl Frames in dieser Sekunde, also die FPS

        gAaLog.Write ( "FPS: %i\n", m_framesCounter );
        //cerr << "FPS: " << m_framesCounter << endl;
        m_framesCounter = 0;
        m_fpsMeasureStart = SDL_GetTicks();
    }
    ++m_framesCounter;
}

// Programmargumente verarbeiten
void GameApp::ParseArguments( int argc, char* args[] )
{
    if (argc > 1)
    {
        std::string strArg = args[1];
        if ( strArg=="-fs" )
            gAaConfig.SetInt( "FullScreen", 1 );
    }
}

// Astro Attack - Christian Zommerfelds - 2009
