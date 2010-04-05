/*----------------------------------------------------------\
|                     Configuration.h                       |
|                     ---------------                       |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include <string>
#include <map>

// Einstellungsvariablen
/*enum ConfigIndex
{
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    SCREEN_BPP,
    FULL_SCREEN,
    ANTI_ALIASING,
    V_SYNC,
    TEX_QUALITY
};*/

struct ConfigEntry
{
    int type;           // 0 bedeutet eine ganze Zahl, 1 ein float, 2 ein string
    int valInt;
    float valFloat;
    std::string valString;
};

/*
    Diese Klasse liest und speichert Programmeinstellungen aus/in eine Datei.
*/
class Configuration
{
public:
    Configuration() { LoadDefault(); }

    bool Load( const char *pFileName ); // Konfiguration aus Datei laden
    bool Save( const char *pFileName ) const; // Konfiguration in Datei speichern

    void LoadDefault(); // Standartkonfiguration laden

    int GetInt(const std::string& id) const;
    float GetFloat(const std::string& id) const;
    std::string GetString(const std::string& id) const;

    void SetInt(const std::string& id, int val );
    void SetFloat(const std::string& id, float val );
    void SetString(const std::string& id, std::string val );

    void ApplyConfig() { m_appliedConfigEntries = m_unappliedConfigEntries; }
    void DiscardConfig() { m_unappliedConfigEntries = m_appliedConfigEntries; }

    //int Get(ConfigIndex index) const;
    //void Set(ConfigIndex index, int value);
    //void GetScreenConfig(int* pWidth, int* pWeight, int* pBpp, bool* pFullScreen);
    //void SetScreenConfig(const int* pWidth, const int* pWeight, const int* pBpp, const bool* pFullScreen);
    
private:
    /*
    int m_screenWidth;    // Breite der Anzeigefläche
    int m_screenHeight;   // Höhe der Anzeigefläche
    int m_screenBpp;      // Bits pro Pixel der Anzeige
    int m_fullScreen;     // ganzer Bildschirm? ja=1 nein=0
    int m_antiAliasing;   // Antialiasing 0->deaktiviert 2->2x 4->4x usw.
    int m_vSync;          // Vertikale Synchronisation (Aktualisierungsrate wird mit Bildschirm Syhchronisiert)
    int m_texQuality;     // Qualität der Texturen (Texture.h definiert welche Zahlen für was stehen)
    */
    bool m_hasLoadedDefault; // wurden die Standarteinstellungen geladen? Wenn ja, alte Konfigurationsdatei nicht überschreiben.
    std::map< std::string, ConfigEntry > m_appliedConfigEntries;
    std::map< std::string, ConfigEntry > m_unappliedConfigEntries;
};

#endif

// Astro Attack - Christian Zommerfelds - 2009
