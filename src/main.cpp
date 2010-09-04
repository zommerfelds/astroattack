/*----------------------------------------------------------\
|                        main.cpp                           |
|                        --------                           |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// -> Hauptablauf <-
// Hier startet und beendet die Anwendung!

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "main.h" // wichtige Definitionen und Dateien einbinden
#include "GameApp.h"
#include "Exception.h" // Ausnahmen im Program

Logger gAaLog( LOG_FILE_NAME ); // Haupt-Log-Datei erstellen, wo wichtige Ereignisse aufgeschrieben werden.
bool gRestart = false;

void ParseArguments( int argc, char* args[] ); // Programmargumente verarbeiten

// Programmstart!
int main ( int argc, char* args[] )
{
    //_CrtSetDbgFlag(_CrtSetDbgFlag(0)|_CRTDBG_CHECK_ALWAYS_DF);
    //_crtBreakAlloc = 0x0108D008;
    //_ASSERTE( _CrtCheckMemory( ) );
    ParseArguments( argc, args );

    if ( gAaLog.IsOpen()==false ) // Fehler beim Öffnen?
	{
        DispError( "ERROR: Log file \"" LOG_FILE_NAME "\" could not be opened!\n" );
		return 1; // Programm beenden
	}

    for(;;)
    {
        gAaLog.WriteInfoStart();

        try
        {
            GameApp AaApp;         // AstroAttack Anwendung
            AaApp.Init(argc,args); // Programm initialisieren
            AaApp.Run();           // Hauptschleife starten
            AaApp.DeInit();        // Programm deinitialisieren
        }
        catch ( Exception &e ) // falls es einen Fehler gab (Ausnahmebehandlung)
        {
            DispError( e.getMsg() ); // Diesen anzeigen
        }
        catch ( std::bad_alloc& ) // Falls nicht genügend Speicherplatz für alle Objekte gefunden wurde wird diese Ausnahme aufgerufen
        {
            DispError( gAaLog.Write( "Error: Memory could not be allocated!\n" ) );
        }
        catch ( std::exception& e ) // Falls eine andere Standart-Ausnahme
        {
            std::string error_msg = std::string("Error: ") + e.what();
            DispError( gAaLog.Write( error_msg.c_str() ) ); // Fehler ausgeben
        }
        catch (...) // Falls eine unbekannte Ausnahme
        {
            char std_err_msg[] = "AstroAttack has encountered an unrecoverable error.\n";
            char std_err_msg2[] = "See the log file \"" LOG_FILE_NAME "\" for more information.";
            DispError( std::string( gAaLog.Write( std_err_msg ) ) + std_err_msg2 );
        }

        gAaLog.WriteInfoEnd();
        if ( gRestart )
        {
            gAaLog.Write( "\n\n============= Restarting " GAME_NAME " =============\n\n" );
            gRestart = false;
        }
        else
            break;
    }

    return 0; // Programm beenden!
}

// Programmargumente verarbeiten
void ParseArguments( int argc, char* args[] )
{
    // TODO: add option to load directly in a level
    if (argc > 1)
    {
        bool quit = false;
        std::string strArg = args[1];
        std::string titleStr = "";
        std::string outputStr = "";
        if ( strArg=="-v" || strArg=="--version" )
        {
            outputStr = GAME_NAME " " GAME_VERSION "\n";
            titleStr = "Version";
            quit = true;
        }
        else if ( strArg=="-h" || strArg=="--help" )
        {
            outputStr = "Usage: " GAME_NAME " [options]\n"
                        "-v, --version = show version\n"
                        "-h, --help = show help\n"
                        "See readme.txt for more info.\n";
            titleStr = "Help";
            quit = true;
        }
        /*else
            OsMsgBox( "Usage: " GAME_NAME " [option]\n -v, -version = show version\n-h, -hlep = show help\n", "Unknown argument" );*/
        if ( outputStr != "" )
            OsMsgBox( outputStr.c_str(), titleStr.c_str() );
        if ( quit )
            exit(0);
    }
}

// Astro Attack - Christian Zommerfelds - 2009
