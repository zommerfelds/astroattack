/*
 * DataLoader.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

// Lesen von INFO-Dateien und übersetzung in Entities

#ifndef DATALOADER_H
#define DATALOADER_H

#include "Exception.h"
#include "Texture.h"
#include <set>
#include <string>

class World;
class TextureManager;
class AnimationManager;
class FontManager;
class FontManager;
struct GameEvents;

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
    static void loadToWorld(const std::string& fileName, World& gameWorld, GameEvents& events);
    static ResourceIds loadGraphics(const std::string& fileName, TextureManager*, AnimationManager*, FontManager*, TexQuality);
    static void unLoadGraphics(const ResourceIds& resourcesToUnload, TextureManager*, AnimationManager*, FontManager*);
    static void saveWorld(const std::string& fileName, const World& gameWorld);
private:
};

#endif
