/*
 * main.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// -> Hauptablauf <-
// Hier startet und beendet die Anwendung!

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "main.h" // wichtige Definitionen und Dateien einbinden
#include "GameApp.h"
#include "Exception.h" // Ausnahmen im Program

#include <vector>
#include <string>

Logger gAaLog( LOG_FILE_NAME ); // Haupt-Log-Datei erstellen, wo wichtige Ereignisse aufgeschrieben werden.
bool gRestart = false; // TODO: dont use globals

// Programmstart!
int main ( int argc, char* argv[] )
{
    //_CrtSetDbgFlag(_CrtSetDbgFlag(0)|_CRTDBG_CHECK_ALWAYS_DF);
    //_crtBreakAlloc = 0x0108D008;
    //_ASSERTE( _CrtCheckMemory( ) );

    std::vector<std::string> args (argv+1, argv+argc);

    if ( gAaLog.isOpen()==false ) // Fehler beim Öffnen?
	{
        DispError( "ERROR: Log file \"" LOG_FILE_NAME "\" could not be opened!\n" );
		return 1; // Programm beenden
	}

    for(;;)
    {
        gAaLog.writeInfoStart();

        try
        {
            GameApp aaApp(args);   // create application object
            aaApp.run();           // run the game!
        }
        catch ( Exception &e ) // falls es einen Fehler gab (Ausnahmebehandlung)
        {
            DispError( e.getMsg() ); // Diesen anzeigen
        }
        catch ( std::bad_alloc& ) // Falls nicht genügend Speicherplatz für alle Objekte gefunden wurde wird diese Ausnahme aufgerufen
        {
            DispError( gAaLog.write( "Error: Memory could not be allocated!\n" ) );
        }
        catch ( std::exception& e ) // Falls eine andere Standart-Ausnahme
        {
            std::string error_msg = std::string("Error: ") + e.what();
            DispError( gAaLog.write( error_msg.c_str() ) ); // Fehler ausgeben
        }
        catch (...) // Falls eine unbekannte Ausnahme
        {
            char std_err_msg[] = "AstroAttack has encountered an unrecoverable error.\n";
            char std_err_msg2[] = "See the log file \"" LOG_FILE_NAME "\" for more information.";
            DispError( std::string( gAaLog.write( std_err_msg ) ) + std_err_msg2 );
        }

        gAaLog.writeInfoEnd();
        if ( gRestart )
        {
            gAaLog.write( "\n\n============= Restarting " GAME_NAME " =============\n\n" );
            gRestart = false;
        }
        else
            break;
    }

    return 0; // Programm beenden!
}
