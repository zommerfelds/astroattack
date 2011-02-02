/*----------------------------------------------------------\
|                        Init.cpp                           |
|                        --------                           |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "main.h" // wichtige Definitionen und Dateien einbinden
// Simple DirectMedia Layer (freie Plattform-übergreifende Multimedia-Programmierschnittstelle)
#include <SDL/SDL.h>
// OpenGL via SDL inkludieren (Plattform-übergreifende Definitionen)
#include <SDL/SDL_opengl.h>
#include "Renderer.h"
#include "Texture.h"

#include "GameApp.h"

// SDL und Fenster initialisieren
bool GameApp::InitSDL()
{
    gAaLog.Write ( "Initializing SDL... " );

    if ( SDL_Init ( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) == -1 )		// SDL initialisieren
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
    if ( gAaConfig.GetInt("FullScreen") )
        flags |= SDL_FULLSCREEN;
    if ( SDL_SetVideoMode ( gAaConfig.GetInt("ScreenWidth"), gAaConfig.GetInt("ScreenHeight"), gAaConfig.GetInt("ScreenBpp"), flags ) == NULL )
    {
        return false;
    }
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

// Astro Attack - Christian Zommerfelds - 2009
