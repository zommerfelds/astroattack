/*
 * Exception.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

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
    : m_msg ( iMsg ) {}

    const std::string& getMsg() const // Abfrage nach dem Errorstring
    {
        return m_msg;
    }

private:
    std::string m_msg;
};

//-------------------------------- Funktionen --------------------------------//
// TODO: restructure place

// Plattform-übergreifende Dialog-Box Funktion
// Nur benutzen in schweren Fällen wann die GUI noch nicht bereit ist.
void OsMsgBox ( const std::string& message, const std::string& title ); // in OsMsgBox.cpp

// Fehlermeldung bei Ausnahmebehandlung anzeigen
void DispError( const std::string& msg ); // in OsMsgBox.cpp

#endif
