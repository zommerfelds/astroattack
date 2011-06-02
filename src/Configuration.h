/*
 * Configuration.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

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

class Configuration;

extern Configuration gAaConfig; // Spieleinstellungen.

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
    Configuration() { loadDefault(); }

    bool load( const char *pFileName ); // Konfiguration aus Datei laden
    bool save( const char *pFileName ) const; // Konfiguration in Datei speichern

    void loadDefault(); // Standartkonfiguration laden

    int getInt(const std::string& id) const;
    float getFloat(const std::string& id) const;
    std::string getString(const std::string& id) const;

    void setInt(const std::string& id, int val );
    void setFloat(const std::string& id, float val );
    void setString(const std::string& id, std::string val );

    void applyConfig() { m_appliedConfigEntries = m_unappliedConfigEntries; }
    void discardConfig() { m_unappliedConfigEntries = m_appliedConfigEntries; }

    //int get(ConfigIndex index) const;
    //void set(ConfigIndex index, int value);
    //void getScreenConfig(int* pWidth, int* pWeight, int* pBpp, bool* pFullScreen);
    //void setScreenConfig(const int* pWidth, const int* pWeight, const int* pBpp, const bool* pFullScreen);
    
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
