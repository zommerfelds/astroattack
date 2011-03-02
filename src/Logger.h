/*
 * Logger.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Für Informationen in einer Log-Datei zu speichern

#ifndef LOGGER_H
#define LOGGER_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include <fstream>
#define LOG_BUF_SIZE 8192

/*
    Diese Klasse kann Texte in einer Log-Datei speichern.
*/

class Logger
{
public:
    Logger( const char *pFileName ); // Konstruktor mit Parameter

    bool openFile( const char *pFileName ); // Neue Datei öffnen
	void closeFile(); // Datei schliessen
    bool isOpen() { return m_isOpen; } // Is eine Datei offen?
    const char* write( const char *format, ... ); // Text in Datei schreiben
    void writeCurrentTime(); // Zeit aufschreiben
    void setIndentationLevel( int level ) { m_indentationLevel = level; } // Texteinrückung bestimmen
    void increaseIndentationLevel() { ++m_indentationLevel; } // Texteinrückung erhöhen
    void decreaseIndentationLevel() { if (m_indentationLevel>0) --m_indentationLevel; } // Texteinrückung reduzieren

    void writeInfoStart();  // Gibt einige Informationen in der Logdatei aus -> Titel und Startzeit (für den Anfang des Dokumentes)
    void writeInfoEnd();    // Gibt einige Informationen in der Logdatei aus -> Endzeit (für den Schluss des Dokumentes)
private:
    Logger() {} // Default Konstruktor ist privat, weil man das andere Konstruktor
                                                           // verwenden muss: Logger( const char *pFileName )
                                                           // So wird versichert, dass schon an Anfang eine Datei offen ist.
    std::ofstream m_LogStreamOut;
    char m_buf[LOG_BUF_SIZE];
    bool m_isOpen;
    unsigned int m_indentationLevel;

    bool m_lastCharWasNewline;

};

#endif
