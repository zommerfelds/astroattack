/*----------------------------------------------------------\
|                         Log.cpp                           |
|                         -------                           |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "main.h"
#include "Logger.h"

#include <stdarg.h>
#include <ctime>
// .h

#define INDENT_SPACE "   " // pro indentation level (Texteinrückung) wird dieser Zeichensatz eingefügt

// Destruktor
Logger::~Logger()
{
	if ( m_isOpen )
    {
		CloseFile();
        m_isOpen = false;
    }
}

// Text in Log-Datei schreiben (beleibig viele Parameter nach dem format Parameter, wie printf())
const char* Logger::Write( const char *format, ... )
{
    // Parameterliste
    va_list List;
    va_start ( List,format );

    int num_chars = 0; // Anzahl Zeichen die in m_buf geschrieben werden
    // Text in m_buf speichern
#ifdef USE_SAFE_CRT_FUNCTIONS
    num_chars = vsnprintf_s ( m_buf,sizeof ( m_buf ),_TRUNCATE,format,List );
#else
    num_chars = vsnprintf ( m_buf,LOG_BUF_SIZE,format,List );
#endif

    va_end ( List );

    if ( !m_isOpen )
        return m_buf; // Pointer zum Text zurückgeben

    // Text in Datei schreiben
    // Texteinrückung wird beachtet
    if( num_chars>0 )
    {
        static bool last_char_was_newline = false;
        if (last_char_was_newline) // wenn eine Zeile gerade neu angefangen wird, den Text entsprechend einrücken
            for( unsigned int a = 0; a < m_indentationLevel; ++a)
                m_LogStreamOut << INDENT_SPACE;
        for( int i = 0; i < num_chars; ++i) // für jedes einzelne Zeichen
        {
            m_LogStreamOut << m_buf[i]; // Zeichen in Datei schreiben schreiben
            if ( i+1 < num_chars )
                if ( m_buf[i]=='\n' && m_buf[i+1]!='\n' ) // wenn eine neue Zeile angefangen wird, Text einrücken
                                                          // (Ausnahme wenn gerade einen Zeilenumbruch folgt)
                    for( unsigned int a = 0; a < m_indentationLevel; ++a)
                        m_LogStreamOut << INDENT_SPACE;
        }
        last_char_was_newline = m_buf[num_chars-1]=='\n'; // Speichern ob das letzte Zeichen einen Zeilenumbruch war,
                                                          // damit nächstes mal am anfang eine Texteinrückckung gemacht wird.
                                                          // (wir wissen hier noch nicht wie viel)
    }

    m_LogStreamOut.flush();
    return m_buf; // Pointer zum Text zurückgeben
}

void Logger::WriteCurrentTime()
{
    if ( !m_isOpen )
        return;

    time_t t = time ( NULL );

#ifdef USE_SAFE_CRT_FUNCTIONS
    char time_str[30] = {0};
    ctime_s ( time_str, 30, &t );
    Write ( time_str );
#else
    Write ( ctime ( &t ) );
#endif
}

bool Logger::OpenFile( const char *pFileName )
{
    if (m_isOpen)
        CloseFile();
    m_LogStreamOut.open( pFileName );
	if( m_LogStreamOut.fail() )
    {
        m_isOpen = false;
		return false;
    }
    m_isOpen = true;
	return true;
}

void Logger::CloseFile()
{
    m_LogStreamOut.close();
    m_LogStreamOut.clear();
    m_isOpen = true;
}

//------ Hilfsfunktionen ------//
void Logger::WriteInfoStart()
{
    Write ( "*** " GAME_NAME " %s ***\nStart: ", GAME_VERSION ); // Version in der Log-Datei anzeigen
    WriteCurrentTime(); // Startzeit aufschreiben
    Write ( "-------------------------------\n\n" );
}

void Logger::WriteInfoEnd()
{
    // Endinforamtionen in Log-Datei schreiben
    SetIndentationLevel( 0 ); // Keine Texteinrückung
    Write ( "\n-----------------------------\n" );
    Write ( "End: " );
    WriteCurrentTime();
}

// Astro Attack - Christian Zommerfelds - 2009
