/*
 * Logger.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Für Informationen in einer Log-Datei zu speichern

#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>

class Logger;

// global logger
extern Logger gAaLog;

/*
    Diese Klasse kann Texte in einer Log-Datei speichern.
*/

class Logger
{
public:
    Logger(const std::string& fileName); // Konstruktor mit Parameter

    bool openFile(const std::string& fileName); // Neue Datei öffnen
	void closeFile(); // Datei schliessen
    bool isOpen() const { return m_isOpen; } // Is eine Datei offen?
    const char* write(const char *format, ...); // Text in Datei schreiben
    void writeCurrentTime(); // Zeit aufschreiben
    void setIndentationLevel( int level ) { m_indentationLevel = level; } // Texteinrückung bestimmen
    void increaseIndentationLevel() { ++m_indentationLevel; } // Texteinrückung erhöhen
    void decreaseIndentationLevel() { if (m_indentationLevel>0) --m_indentationLevel; } // Texteinrückung reduzieren

    void writeInfoStart();  // Gibt einige Informationen in der Logdatei aus -> Titel und Startzeit (für den Anfang des Dokumentes)
    void writeInfoEnd();    // Gibt einige Informationen in der Logdatei aus -> Endzeit (für den Schluss des Dokumentes)
private:

    std::ofstream m_LogStreamOut;
    static const int cLogBufSize = 8192;
    char m_buf[cLogBufSize];
    bool m_isOpen;
    unsigned int m_indentationLevel;

    bool m_lastCharWasNewline;

};

#endif
