/*
 * DataLoader.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Lesen von XML-Dateien und übersetzung in Entities

#ifndef DATALOADER_H
#define DATALOADER_H

#include <set>
#include <string>

#include "Exception.h"

class World;
struct SubSystems;
struct SlideShow;
class TextureManager;
class AnimationManager;
class FontManager;

struct ResourceIds
{
    std::set<std::string> textures;
    std::set<std::string> animations;
    std::set<std::string> fonts;
};

class DataLoadException : public Exception {
public:
    DataLoadException(const std::string& msg);
};

class DataLoader
{
public:
    static void loadWorld( const std::string& fileName, World& gameWorld, SubSystems& subSystems );
    static void loadSlideShow( const std::string& fileName, SlideShow* pSlideShow );
    static ResourceIds loadGraphics( const std::string& fileName, TextureManager* pTextureManager, AnimationManager* pAnimationManager, FontManager* pFontManager );
    static void unLoadGraphics( const ResourceIds& resourcesToUnload, TextureManager* pTextureManager, AnimationManager* pAnimationManager, FontManager* pFontManager );
    static void saveWorld( const std::string& fileName, const World& gameWorld );
private:
};

#endif
