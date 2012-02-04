/*
 * Texture.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

// Verwaltung von Texturen und Animationen

#ifndef TEXTURE_H
#define TEXTURE_H

#include <map>
#include <set>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

class LoadTextureInfo;
class TexInfo;

typedef std::string TextureId;
typedef std::string AnimationId;

//---------------------------------------//
// Klasse für das Verwalten von Texturen //
//---------------------------------------//
class TextureManager
{
public:
    TextureManager();
    ~TextureManager();
    void loadTexture(const std::string& fileName, TextureId, const LoadTextureInfo&, int* w=NULL, int* h=NULL);
    void freeTexture(const TextureId&);
    void setTexture(const TextureId&);
    std::vector<TextureId> getTextureList() const;
    void clear();
private:
    void freeTextureMemory(const TexInfo& tex);
    typedef std::map<TextureId, TexInfo> TextureMap;
    TextureMap m_textures;      // Textur-Addressen für OpenGL
};

typedef std::string AnimStateId;
class StateInfo;
class AnimInfo;
typedef std::map< AnimStateId, boost::shared_ptr<StateInfo> > StateInfoMap;

//------------------------------------------//
// Klasse für das Verwalten von Animationen //
//------------------------------------------//
class AnimationManager
{
public:
    AnimationManager(TextureManager&);
    ~AnimationManager();
    void loadAnimation(const std::string& fileName, AnimationId id, const LoadTextureInfo& texInfo);
    void freeAnimation(AnimationId id);
    const AnimInfo* getAnimInfo(AnimationId animId) const;

private:
    void freeAnimationMemory(const AnimInfo&);
    typedef std::map<AnimationId, boost::shared_ptr<AnimInfo> > AnimInfoMap;
    AnimInfoMap m_animInfoMap;      // Texturen
    TextureManager& m_texManager;
};

// --- STRUKTUREN ---

// Informationen für das Laden einer Textur
enum TexWrapMode {
    WrapClamp,
    WrapRepeat
};

enum TexQuality {
    QualityBest = 0,
    QualityGood = 1,
    QualityMiddle = 2,
    QualityLow = 3,
    QualityLowest = 4
};

class LoadTextureInfo
{
public:
    bool loadMipmaps;
    TexWrapMode wrapModeX; // GL_CLAMP oder GL_REPEAT
    TexWrapMode wrapModeY; // GL_CLAMP oder GL_REPEAT
    TexQuality quality;
    float scale;
};

class TexInfo
{
public:
    unsigned int glTexId;
    float scale;
};

// Information eines Animationszustands (eine Aktion wie z.B. Rennen, Springen...)
class StateInfo
{
public:
    int begin;
    int end;
    int speed;
    std::set< int > stops;
};

// Informationen einer Animation
class AnimInfo
{
public:
    AnimationId name;
    int totalFrames;
    int numDigits;
    StateInfoMap states;
};

#endif
