/*
 * main.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Hier startet und beendet die Anwendung!

#include "main.h"
#include "GameApp.h"
#include "Configuration.h"

#include "common/Exception.h"
#include "common/Logger.h"

#include <vector>
#include <string>
#include <boost/property_tree/info_parser.hpp>

namespace {
const std::string cConfigFileName = "data/config.info";
}

// Programmstart!
int main ( int argc, char* argv[] )
{
    //_CrtSetDbgFlag(_CrtSetDbgFlag(0)|_CRTDBG_CHECK_ALWAYS_DF);
    //_crtBreakAlloc = 0x0108D008;
    //_ASSERTE( _CrtCheckMemory( ) );

    std::vector<std::string> args (argv+1, argv+argc);

    // TODO
    /*if ( gAaLog.isOpen()==false ) // Fehler beim Öffnen?
	{
        OsMsgBox( "Log file \"" LOG_FILE_NAME "\" could not be opened!\n", "Error" );
	}*/

    // Einstellung lesen
    boost::property_tree::info_parser::read_info(cConfigFileName, gConfig); // TODO: handle fail
    setUpLoggerFromPropTree(gConfig);

    do
    {
        gDoRestart = false;

        log(Info).writeHeader(GAME_NAME " " GAME_VERSION);

        //try
        {
            GameApp aaApp(args);   // create application object
            aaApp.run();           // run the game!
        }
        /*catch ( Exception& e ) // falls es einen Fehler gab (Ausnahmebehandlung)
        {
            OsMsgBox( e.getMsg(), "Exception" ); // Diesen anzeigen
        }
        catch ( std::bad_alloc& ) // Falls nicht genügend Speicherplatz für alle Objekte gefunden wurde wird diese Ausnahme aufgerufen
        {
            OsMsgBox( log() << "Error: Memory could not be allocated!\n" ), "Exception" );
        }
        catch ( std::exception& e ) // Falls eine andere Standart-Ausnahme
        {
            std::string error_msg = std::string("Error: ") + e.what();
            OsMsgBox( log() << error_msg ), "Exception" ); // Fehler ausgeben
        }
        catch (...) // Falls eine unbekannte Ausnahme
        {
            char std_err_msg[] = "AstroAttack has encountered an unrecoverable error.\n";
            char std_err_msg2[] = "See the log file \"" LOG_FILE_NAME "\" for more information.";
            OsMsgBox( std::string( log() << std_err_msg ) ) + std_err_msg2, "Exception" );
        }*/

        if ( gDoRestart )
        {
            log(Info) << "\n\n============= Restarting " GAME_NAME " =============\n\n";
        }
    } while (gDoRestart);

    writeConfig(cConfigFileName, gConfig);

    return 0; // Programm beenden!
}
