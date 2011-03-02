/*
 * Configuration.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "main.h" // wichtige Definitionen und Dateien einbinden
#include "Configuration.h"
#include "Texture.h"
#include <fstream>
#include <string>
#include <sstream>
#include "contrib/pugixml/pugixml.hpp"
#include <boost/foreach.hpp>
#include "contrib/pugixml/foreach.hpp"

// Standarteinstellungen laden
void Configuration::loadDefault()
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
bool Configuration::load( const char *pFileName )
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(pFileName);
    if (!result)
    {
        gAaLog.write( "[ Error parsing file '%s' at offset %d!\nError description: %s ]\n\n", pFileName, result.offset, result.description() );
        return false;
    }

    BOOST_FOREACH(pugi::xml_node node, doc.first_child())
    {
        std::string id = node.name();
        std::string type = node.attribute("type").value();
        ConfigEntry cfgEntry;
        if ( type == "int" )
        {
            cfgEntry.type = 0;
            cfgEntry.valInt = node.attribute("val").as_int();
        }
        else if ( type == "float" )
        {
            cfgEntry.type = 1;
            cfgEntry.valFloat = node.attribute("val").as_float();
        }
        else if ( type == "string" )
        {
            cfgEntry.type = 2;
            cfgEntry.valString = node.attribute("val").value();
        }
        m_appliedConfigEntries.insert( std::make_pair(id,cfgEntry) );
    }
    m_unappliedConfigEntries = m_appliedConfigEntries;
    return true;
}

// Konfiguration in Datei speichern
bool Configuration::save( const char *pFileName ) const
{
    pugi::xml_document doc;

    pugi::xml_node configNode = doc.append_child();
    configNode.set_name("config");

    for ( std::map< std::string, ConfigEntry >::const_iterator it = m_appliedConfigEntries.begin(); it != m_appliedConfigEntries.end(); ++it )
    {
        pugi::xml_node entryNode = configNode.append_child(it->first.c_str());
        std::string typeStr;
        switch ( it->second.type )
        {
        case 0:
            typeStr = "int";
            entryNode.append_attribute("val").set_value(it->second.valInt);
            break;
        case 1:
            typeStr = "float";
            entryNode.append_attribute("val").set_value(it->second.valFloat);
            break;
        case 2:
            typeStr = "string";
            entryNode.append_attribute("val").set_value(it->second.valString.c_str());
            break;
        default:
            break;
        }
        entryNode.append_attribute("type").set_value(typeStr.c_str());
    }

    return doc.save_file(pFileName);
}

int Configuration::getInt(const std::string& id) const
{
    std::map< std::string, ConfigEntry >::const_iterator it = m_appliedConfigEntries.find( id );
    if ( it == m_appliedConfigEntries.end() )
        return 0;
    else
        return it->second.valInt;
}

float Configuration::getFloat(const std::string& id) const
{
    std::map< std::string, ConfigEntry >::const_iterator it = m_appliedConfigEntries.find( id );
    if ( it == m_appliedConfigEntries.end() )
        return 0.0f;
    else
        return it->second.valFloat;
}

std::string Configuration::getString(const std::string& id) const
{
    std::map< std::string, ConfigEntry >::const_iterator it = m_appliedConfigEntries.find( id );
    if ( it == m_appliedConfigEntries.end() )
        return "";
    else
        return it->second.valString;
}

void Configuration::setInt(const std::string& id, int val )
{
    ConfigEntry& entry = m_unappliedConfigEntries[id];
    entry.type = 0;
    entry.valInt = val;
}

void Configuration::setFloat(const std::string& id, float val )
{
    ConfigEntry& entry = m_unappliedConfigEntries[id];
    entry.type = 1;
    entry.valFloat = val;
}

void Configuration::setString(const std::string& id, std::string val )
{
    ConfigEntry& entry = m_unappliedConfigEntries[id];
    entry.type = 2;
    entry.valString = val;
}
