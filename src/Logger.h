/*----------------------------------------------------------\
|                          Log.h                            |
|                          -----                            |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
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
    Logger( const char *pFileName ) : m_isOpen ( false ), m_indentationLevel ( 0 ) { OpenFile( pFileName ); } // Konstruktor mit Parameter
    ~Logger();
    bool OpenFile( const char *pFileName ); // Neue Datei öffnen
	void CloseFile(); // Datei schliessen
    bool IsOpen() { return m_isOpen; } // Is eine Datei offen?
    const char* Write( const char *format, ... ); // Text in Datei schreiben
    void WriteCurrentTime(); // Zeit aufschreiben
    void SetIndentationLevel( int level ) { m_indentationLevel = level; } // Texteinrückung bestimmen
    void IncreaseIndentationLevel() { ++m_indentationLevel; } // Texteinrückung erhöhen
    void DecreaseIndentationLevel() { if (m_indentationLevel>0) --m_indentationLevel; } // Texteinrückung reduzieren

    void WriteInfoStart();  // Gibt einige Informationen in der Logdatei aus -> Titel und Startzeit (für den Anfang des Dokumentes)
    void WriteInfoEnd();    // Gibt einige Informationen in der Logdatei aus -> Endzeit (für den Schluss des Dokumentes)
private:
    Logger() : m_isOpen ( false ), m_indentationLevel ( 0 ) {} // Default Konstruktor ist privat, weil man das andere Konstruktor
                                                           // verwenden muss: Logger( const char *pFileName )
                                                           // So wird versichert, dass schon an Anfang eine Datei offen ist.
    std::ofstream m_LogStreamOut;
    char m_buf[LOG_BUF_SIZE];
    bool m_isOpen;
    unsigned int m_indentationLevel;
};

#endif

// Astro Attack - Christian Zommerfelds - 2009
