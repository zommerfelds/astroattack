/*
 * XmlLoader.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Lesen von XML-Dateien und übersetzung in Entities

#ifndef XMLLOADER_H
#define XMLLOADER_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

class GameWorld;
struct SubSystems;
struct SlideShow;
class TextureManager;
class AnimationManager;
class FontManager;

#include <set>
#include <string>

struct ResourceIds
{
    std::set<std::string> textures;
    std::set<std::string> animations;
    std::set<std::string> fonts;
};

class XmlLoader
{
public:
    static void loadXmlToWorld( const char* pFileName, GameWorld& gameWorld, SubSystems& subSystems );
    static void loadSlideShow( const char* pFileName, SlideShow* pSlideShow );
    static ResourceIds loadGraphics( const char* pFileName, TextureManager* pTextureManager, AnimationManager* pAnimationManager, FontManager* pFontManager );
    static void unLoadGraphics( const ResourceIds& resourcesToUnload, TextureManager* pTextureManager, AnimationManager* pAnimationManager, FontManager* pFontManager );
    static void saveWorldToXml( const char* pFileName, const GameWorld& gameWorld );
private:
};

#endif
