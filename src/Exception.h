/*----------------------------------------------------------\
|                       Exception.h                         |
|                       -----------                         |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include <string>

/*
   - Ausnahmeklasse -

   Diese Klasse speichert eine Zeichenkette wenn fehler geschehen.

   So wird mit fatalen Fehlern umgegengen:
   throw Exception ( "Fehlertext" );
   Oder wenn man auch in der Log-Datei schreiben möchte:
   throw Exception ( gAaLog.Write( "Fehlertext" ) );
*/
class Exception
{
public:
    Exception ( const std::string &iMsg ) // std::string als Parameter
    {
        setMsg ( iMsg );
    }

    Exception ( const char *pc )
    {
        setMsg ( std::string ( pc ) ); // normale char* Zeichenkette als Parameter
    }

    const std::string &getMsg() const // Abfrage nach dem Errorstring
    {
        return m_msg;
    }

private:
    void setMsg ( std::string message )
    {
        m_msg = message;
    }
    std::string m_msg;
};

//-------------------------------- Funktionen --------------------------------//

// Plattform-übergreifende Dialog-Box Funktion
// Nur benutzen in schweren Fällen wann die GUI noch nicht bereit ist.
void OsMsgBox ( const std::string& message, const std::string& title ); // in OsMsgBox.cpp

// Fehlermeldung bei Ausnahmebehandlung anzeigen
void DispError( const std::string& msg ); // in OsMsgBox.cpp

#endif

// Astro Attack - Christian Zommerfelds - 2009
