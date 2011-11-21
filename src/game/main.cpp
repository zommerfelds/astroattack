/*
 * main.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Hier startet und beendet die Anwendung!

#include <vector>
#include <string>
#include <boost/property_tree/info_parser.hpp>

#include "main.h" // wichtige Definitionen und Dateien einbinden
#include "GameApp.h"
#include "common/Exception.h" // Ausnahmen im Program
#include "Configuration.h"

#include "common/Logger.h"

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
        gRestart = false;

        //gAaLog.writeInfoStart(); TODO

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

        //gAaLog.writeInfoEnd(); TODO
        if ( gRestart )
        {
            log(Info) << "\n\n============= Restarting " GAME_NAME " =============\n\n";
        }
    } while (gRestart);

    writeConfig(cConfigFileName, gConfig);

    return 0; // Programm beenden!
}
