/*
 * main.cpp
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#include "main.h"
#include "GameApp.h"
#include "Configuration.h"

#include "common/Exception.h"
#include "common/Logger.h"

#include <vector>
#include <string>

namespace {
const std::string cConfigFileName = "data/config.info";
}

#define DONT_CATCH_EXCEPTION

// Programmstart!
int main ( int argc, char* argv[] )
{
    std::vector<std::string> args (argv+1, argv+argc);

    loadConfig(cConfigFileName, gConfig);
    setUpLoggerFromPropTree(gConfig);

    char centeredStr[] = "SDL_VIDEO_CENTERED=1";
    putenv(centeredStr);

    try
    {
        bool restart;
        do
        {
            restart = false;
            log(Info).writeHeader(GAME_NAME " " GAME_VERSION);

            GameApp aaApp(args);   // create application object
            aaApp.run();           // run the game!

            if ( aaApp.doRestart() )
            {
                log(Info) << "\n\n============= Restarting " GAME_NAME " =============\n\n";
                restart = true;
            }

            // args.clear(); ??
        }
        while (restart);
    }
#ifndef DONT_CATCH_EXCEPTION
    catch ( Exception& e )
    {
        log(Fatal) << e.getMsg() << "\n"; // Diesen anzeigen
    }
    catch ( std::bad_alloc& )
    {
        log(Fatal) << "Error: Memory could not be allocated!\n";
    }
    catch ( std::exception& e )
    {
        log(Fatal) << e.what() << "\n"; // Fehler ausgeben
    }
#endif
    catch (...)
    {
        assert(false);
    }

    writeConfig(cConfigFileName, gConfig);

    return 0; // Programm beenden!
}
