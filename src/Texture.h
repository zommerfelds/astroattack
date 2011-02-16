/*
 * Texture.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Verwaltung von Texturen und Animationen

#ifndef TEXTURE_H
#define TEXTURE_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include <map>
#include <set>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

typedef std::string TextureIdType;
typedef std::string AnimationIdType;

#define TEX_CLAMP   0x2900
#define TEX_REPEAT  0x2901

#define TEX_QUALITY_BEST     0
#define TEX_QUALITY_GOOD     1
#define TEX_QUALITY_MIDDLE   2
#define TEX_QUALITY_LOW      3
#define TEX_QUALITY_LOWEST   4

struct LoadTextureInfo;
struct TexInfo;

//---------------------------------------//
// Klasse für das Verwalten von Texturen //
//---------------------------------------//
class TextureManager
{
public:
    TextureManager();
    ~TextureManager();
    void LoadTexture( const std::string& name, TextureIdType id, const LoadTextureInfo& loadTexInfo, int quality, int* pW=NULL, int* pH=NULL );
    void FreeTexture( const TextureIdType& id );
    void SetTexture( const TextureIdType& id );
    std::vector<TextureIdType> GetTextureList() const;
    void Clear();
private:
    typedef std::map<TextureIdType, TexInfo> TextureMap;
    TextureMap m_textures;      // Textur-Addressen für OpenGL
};

typedef std::string StateIdType;
struct StateInfo;
struct AnimInfo;
typedef std::map< StateIdType, boost::shared_ptr<StateInfo> > StateInfoMap;

//------------------------------------------//
// Klasse für das Verwalten von Animationen //
//------------------------------------------//
class AnimationManager
{
public:
    AnimationManager( TextureManager* pTM );
    ~AnimationManager() {}
    void LoadAnimation( const char* name,AnimationIdType id,const LoadTextureInfo& texInfo, int quality );
    void FreeAnimation( AnimationIdType id );
    const AnimInfo* GetAnimInfo( AnimationIdType animId ) const;

private:
    typedef std::map<AnimationIdType, boost::shared_ptr<AnimInfo> > AnimInfoMap;
    AnimInfoMap m_animInfoMap;      // Texturen
    TextureManager* m_pTM;
};

// --- STRUKTUREN ---

// Informationen für das Laden einer Textur
struct LoadTextureInfo
{
    bool loadMipmaps;
    int textureWrapModeX; // GL_CLAMP oder GL_REPEAT
    int textureWrapModeY; // GL_CLAMP oder GL_REPEAT
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
    AnimationIdType name;
    int totalFrames;
    int numDigits;
    StateInfoMap states;
};

#endif
