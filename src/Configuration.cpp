/*----------------------------------------------------------\
|                    Configuration.cpp                      |
|                    -----------------                      |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "main.h" // wichtige Definitionen und Dateien einbinden
#include "Configuration.h"
#include "Texture.h"
#include <fstream>
#include <string>
#include <sstream>
#include <tinyxml.h>

// Standarteinstellungen laden
void Configuration::LoadDefault()
{
    // Standarteinstellungen
    /*const int defScreenWidth = 1024;
    const int defScreenHeight = 768;
    const int defScreenBpp = 32;
    const int defFullScreen = 0;
    const int defAntiAliasing = 2;
    const int defVSync = 1;
    const int defTexQuality = TEX_QUALITY_BEST;

    m_screenWidth = defScreenWidth;      // Breite der Anzeigefläche
    m_screenHeight = defScreenHeight;    // Höhe der Anzeigefläche
    m_screenBpp = defScreenBpp;          // Bits pro Pixel der Anzeige
    m_fullScreen = defFullScreen;        // ganzer Bildschirm? ja=1 nein=0
    m_antiAliasing = defAntiAliasing;    // Antialiasing 0->deaktiviert 2->2x 4->4x usw.
    m_vSync = defVSync;                  // Synchronisation mit Bildschirm
    m_texQuality = defTexQuality;*/

    m_hasLoadedDefault = true;
}

// Konfiguration aus Datei laden
bool Configuration::Load( const char *pFileName )
{
#if 0
    std::ifstream config_in_stream;
    config_in_stream.open( pFileName );
	if( config_in_stream.fail() ) // Fehler beim Öffnen
    {
        LoadDefault(); // Standarteinstellungen laden
        gAaLog.Write ( "Warning: Config file \"%s\" could not be opened. Default settings were loaded.\n", pFileName );
		return false;
    }

    try
    {
        std::string temp = "";

        // --- Fensterbreite --- //
        config_in_stream >> temp; // zuerst einen Wort überspringen,
        if ( !(config_in_stream >> m_screenWidth) ) // dann die Einstellung einlesen
            throw 0;
        // --- Fensterhöhe --- //
        config_in_stream >> temp;
        if ( !(config_in_stream >> m_screenHeight) )
            throw 0;
        // --- Farbqualität --- //
        config_in_stream >> temp;
        if ( !(config_in_stream >> m_screenBpp) )
            throw 0;
        // --- Vollbild? --- //
        config_in_stream >> temp;
        if ( config_in_stream >> m_fullScreen )
        {
            if ( m_fullScreen )
                m_fullScreen = 1;
            else
                m_fullScreen = 0;
        }
        else throw 0;
        // --- Antialising --- //
        config_in_stream >> temp;
        if ( config_in_stream >> m_antiAliasing )
        {
            if ( m_antiAliasing==1 )
                m_antiAliasing = 0;
        }
        else throw 0;
        // --- V-Sync --- //
        config_in_stream >> temp;
        if ( config_in_stream >> m_vSync )
        {
            if ( m_vSync )
                m_vSync = 1;
            else
                m_vSync = 0;
        }
        else throw 0;
        // --- Texturqualität --- //
        config_in_stream >> temp;
        if ( !(config_in_stream >> m_texQuality) )
            throw 0;

        // ist alles gut gegangen?
        if (m_screenWidth==0 || m_screenHeight==0 || m_screenBpp==0)
            throw 0;

        m_hasLoadedDefault = false;
    }
    catch (...)
    {
        LoadDefault();
        gAaLog.Write ( "Error reading config file \"%s\" (bad syntax?). Default settings loaded.\n", pFileName );
        gAaLog.Write ( "Please delete or backup the file \"%s\", so that " GAME_NAME " can write a new one.\n", pFileName );
    }

    config_in_stream.close();

    return true;
#endif
    TiXmlDocument doc(pFileName);
	if (!doc.LoadFile()) return false;

	TiXmlHandle hRoot(0);

    {
        TiXmlElement* pElem = doc.FirstChildElement();
	    if (!pElem) return false;
	    hRoot=TiXmlHandle(pElem);
    }    

    for ( TiXmlElement* entryElement = hRoot.FirstChildElement().ToElement(); entryElement; entryElement = entryElement->NextSiblingElement() )
    {
        std::string id = entryElement->Value();
        std::string type = entryElement->Attribute("type");
        ConfigEntry cfgEntry;
        if ( type == "int" )
        {
            cfgEntry.type = 0;
            cfgEntry.valInt = 0;
            entryElement->QueryIntAttribute( "val", &cfgEntry.valInt );
        }
        else if ( type == "float" )
        {
            cfgEntry.type = 1;
            cfgEntry.valFloat = 0.0f;
            entryElement->QueryFloatAttribute( "val", &cfgEntry.valFloat );
        }
        else if ( type == "string" )
        {
            cfgEntry.type = 2;
            cfgEntry.valString = "";
            cfgEntry.valString = entryElement->Attribute( "val" );
        }
        m_appliedConfigEntries.insert( std::make_pair(id,cfgEntry) );
    }
    m_unappliedConfigEntries = m_appliedConfigEntries;
    return true;
}

// Konfiguration in Datei speichern
bool Configuration::Save( const char *pFileName ) const
{
#if 0
    // Wurden die Standarteinstellungen geladen? Wenn ja, alte Konfigurationsdatei nicht überschreiben.
    if( !m_hasLoadedDefault )
    {
        std::ofstream config_out_stream;
        config_out_stream.open( pFileName );
	    if( config_out_stream.fail() )
		    return false;

        config_out_stream << "ScreenWidth                          " << m_screenWidth << "\n"
                          << "ScreenHeight                         " << m_screenHeight << "\n"
                          << "ScreenBpp                            " << m_screenBpp << "\n"
                          << "FullScreen(0=off;1=on)               " << m_fullScreen << "\n"
                          << "AntiAliasing(1=off,2=2x,4=4x,...)    " << m_antiAliasing << "\n"
                          << "V-Sync(0=off;1=on)                   " << m_vSync << "\n"
                          << "TexQuality(0=best,...,4=lowest)      " << m_texQuality << "\n";

        config_out_stream.close();
    }

    return true;
#endif
    TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );

    TiXmlElement* rootElement = new TiXmlElement( "config" );
	doc.LinkEndChild( rootElement );

    for ( std::map< std::string, ConfigEntry >::const_iterator it = m_appliedConfigEntries.begin(); it != m_appliedConfigEntries.end(); ++it )
    {
        TiXmlElement* entityElement = new TiXmlElement( it->first.c_str() );
        std::string typeStr;
        std::stringstream ss;
        switch ( it->second.type )
        {
        case 0:
            typeStr = "int";
            ss << it->second.valInt;
            entityElement->SetAttribute( "val", ss.str().c_str() );
            break;
        case 1:
            typeStr = "float";
            ss << it->second.valFloat;
            entityElement->SetAttribute( "val", ss.str().c_str() );
            break;
        case 2:
            typeStr = "string";
            entityElement->SetAttribute( "val", it->second.valString.c_str() );
            break;
        default:
            break;
        }
        entityElement->SetAttribute( "type", typeStr.c_str() );
        
        rootElement->LinkEndChild( entityElement );
    }

    return doc.SaveFile( pFileName );
}

int Configuration::GetInt(const std::string& id) const
{
    std::map< std::string, ConfigEntry >::const_iterator it = m_appliedConfigEntries.find( id );
    if ( it == m_appliedConfigEntries.end() )
        return 0;
    else
        return it->second.valInt;
}

float Configuration::GetFloat(const std::string& id) const
{
    std::map< std::string, ConfigEntry >::const_iterator it = m_appliedConfigEntries.find( id );
    if ( it == m_appliedConfigEntries.end() )
        return 0.0f;
    else
        return it->second.valFloat;
}

std::string Configuration::GetString(const std::string& id) const
{
    std::map< std::string, ConfigEntry >::const_iterator it = m_appliedConfigEntries.find( id );
    if ( it == m_appliedConfigEntries.end() )
        return "";
    else
        return it->second.valString;
}

void Configuration::SetInt(const std::string& id, int val )
{
    ConfigEntry& entry = m_unappliedConfigEntries[id];
    entry.type = 0;
    entry.valInt = val;
}

void Configuration::SetFloat(const std::string& id, float val )
{
    ConfigEntry& entry = m_unappliedConfigEntries[id];
    entry.type = 1;
    entry.valFloat = val;
}

void Configuration::SetString(const std::string& id, std::string val )
{
    ConfigEntry& entry = m_unappliedConfigEntries[id];
    entry.type = 2;
    entry.valString = val;
}

// Astro Attack - Christian Zommerfelds - 2009
