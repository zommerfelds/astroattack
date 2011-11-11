/*
 * main.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Hier startet und beendet die Anwendung!

#include <vector>
#include <string>

#include "main.h" // wichtige Definitionen und Dateien einbinden
#include "GameApp.h"
#include "common/Exception.h" // Ausnahmen im Program

#include "Logger.h"

// Programmstart!
int main ( int argc, char* argv[] )
{
    //_CrtSetDbgFlag(_CrtSetDbgFlag(0)|_CRTDBG_CHECK_ALWAYS_DF);
    //_crtBreakAlloc = 0x0108D008;
    //_ASSERTE( _CrtCheckMemory( ) );

    std::vector<std::string> args (argv+1, argv+argc);

    if ( gAaLog.isOpen()==false ) // Fehler beim Öffnen?
	{
        OsMsgBox( "Log file \"" LOG_FILE_NAME "\" could not be opened!\n", "Error" );
	}

    do
    {
        gRestart = false;

        gAaLog.writeInfoStart();

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
            OsMsgBox( gAaLog.write( "Error: Memory could not be allocated!\n" ), "Exception" );
        }
        catch ( std::exception& e ) // Falls eine andere Standart-Ausnahme
        {
            std::string error_msg = std::string("Error: ") + e.what();
            OsMsgBox( gAaLog.write( error_msg.c_str() ), "Exception" ); // Fehler ausgeben
        }
        catch (...) // Falls eine unbekannte Ausnahme
        {
            char std_err_msg[] = "AstroAttack has encountered an unrecoverable error.\n";
            char std_err_msg2[] = "See the log file \"" LOG_FILE_NAME "\" for more information.";
            OsMsgBox( std::string( gAaLog.write( std_err_msg ) ) + std_err_msg2, "Exception" );
        }*/

        gAaLog.writeInfoEnd();
        if ( gRestart )
        {
            gAaLog.write( "\n\n============= Restarting " GAME_NAME " =============\n\n" );
        }
    } while (gRestart);

    return 0; // Programm beenden!
}
