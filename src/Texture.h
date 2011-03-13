/*
 * Texture.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Verwaltung von Texturen und Animationen

#ifndef TEXTURE_H
#define TEXTURE_H

#include <map>
#include <set>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

struct LoadTextureInfo;
struct TexInfo;

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
    void loadTexture( const std::string& fileName, TextureId, const LoadTextureInfo&, int* pW=NULL, int* pH=NULL );
    void freeTexture( const TextureId& );
    void setTexture( const TextureId& );
    std::vector<TextureId> getTextureList() const;
    void clear();
private:
    typedef std::map<TextureId, TexInfo> TextureMap;
    TextureMap m_textures;      // Textur-Addressen für OpenGL
};

typedef std::string AnimStateId;
struct StateInfo;
struct AnimInfo;
typedef std::map< AnimStateId, boost::shared_ptr<StateInfo> > StateInfoMap;

//------------------------------------------//
// Klasse für das Verwalten von Animationen //
//------------------------------------------//
class AnimationManager
{
public:
    AnimationManager( TextureManager& );
    ~AnimationManager() {}
    void loadAnimation( const char* name,AnimationId id,const LoadTextureInfo& texInfo );
    void freeAnimation( AnimationId id );
    const AnimInfo* getAnimInfo( AnimationId animId ) const;

private:
    typedef std::map<AnimationId, boost::shared_ptr<AnimInfo> > AnimInfoMap;
    AnimInfoMap m_animInfoMap;      // Texturen
    TextureManager& m_texManager;
};

// --- STRUKTUREN ---

// Informationen für das Laden einer Textur
struct LoadTextureInfo
{
    enum WrapMode {
        WrapClamp,
        WrapRepeat
    };

    enum Quality {
        QualityBest = 0,
        QualityGood = 1,
        QualityMiddle = 2,
        QualityLow = 3,
        QualityLowest = 4
    };

    bool loadMipmaps;
    WrapMode wrapModeX; // GL_CLAMP oder GL_REPEAT
    WrapMode wrapModeY; // GL_CLAMP oder GL_REPEAT
    Quality quality;
    float scale;
};

struct TexInfo
{
    unsigned int texAddress;
    float scale;
};

// Information eines Animationszustands (eine Aktion wie z.B. Rennen, Springen...)
struct StateInfo
{
    int begin;
    int end;
    int speed;
    std::set< int > stops;
};

// Informationen einer Animation
struct AnimInfo
{
    AnimationId name;
    int totalFrames;
    int numDigits;
    StateInfoMap states;
};

#endif
