/*
 * main.cpp
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
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

#define CATCH_EXCEPTION

int main ( int argc, char* argv[] )
{
    std::vector<std::string> args (argv+1, argv+argc);

    loadConfig(cConfigFileName, gConfig);
    setUpLoggerFromPropTree(gConfig);

    putenv(const_cast<char*>("SDL_VIDEO_CENTERED=1"));

#ifdef CATCH_EXCEPTION
    try
#endif
    {
        bool restart;
        do
        {
            restart = false;
            log(Info).writeHeader(GAME_NAME " " GAME_VERSION);

            GameApp aaApp(args);
            aaApp.run();

            if ( aaApp.doRestart() )
            {
                log(Info) << "\n\n============= Restarting " GAME_NAME " =============\n\n";
                restart = true;
            }

            args.clear();
        }
        while (restart);
    }
#ifdef CATCH_EXCEPTION
    catch ( Exception& e )
    {
        log(Fatal) << e.getMsg() << "\n";
        OsMsgBox(e.getMsg(), "Exception");
    }
    catch ( std::bad_alloc& )
    {
        log(Fatal) << "Error: Memory could not be allocated!\n";
    }
    catch ( std::exception& e )
    {
        log(Fatal) << e.what() << "\n";
    }
    catch (...)
    {
        assert(false);
    }
#endif

    writeConfig(cConfigFileName, gConfig);

    return 0;
}
