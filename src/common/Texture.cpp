/*
 * Texture.cpp
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#include "Texture.h"

#include "Logger.h"
#include "Exception.h"
#include "Foreach.h"
#include "DataLoader.h"

#include <cmath>
#include <fstream>
#include <sstream>
#include <IL/il.h>
#include <IL/ilu.h>
#include <SDL_opengl.h> // cross platform OpenGL include (provided by SDL)
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>

// Konstruktor
TextureManager::TextureManager()
{
    // Version von DevIL überprüfen.
    ILint il_dynamic_library_version = ilGetInteger(IL_VERSION_NUM);
    if ( il_dynamic_library_version < IL_VERSION )
    {
        // falsche Version
        throw Exception("DevIL (IL) library is too old!\nFound " + boost::lexical_cast<std::string>(il_dynamic_library_version) + ", need > " + boost::lexical_cast<std::string>(IL_VERSION) + ".");
    }
    ILint ilu_dynamic_library_version = iluGetInteger(ILU_VERSION_NUM);
    if ( ilu_dynamic_library_version < ILU_VERSION )
    {
        // falsche Version
        throw Exception("DevIL (ILU) library is too old!\nFound " + boost::lexical_cast<std::string>(ilu_dynamic_library_version) + ", need > " + boost::lexical_cast<std::string>(ILU_VERSION) + ".");
    }

    ilInit(); // Initialization von DevIL (IL)
    iluInit(); // Initialization von DevIL (ILU)
}

// Destruktor
TextureManager::~TextureManager()
{
    foreach (const TextureMap::value_type& t, m_textures)
    {
        freeTextureMemory(t.second);
    }
}

// Überprüfen ob es einen Fehler in OpenGL gibt
void CheckOpenlGlError()
{
    GLenum errCode;
    const GLubyte *errString;

    if ((errCode = glGetError()) != GL_NO_ERROR)
    {
        errString = gluErrorString(errCode);
        log(Error) << "========= OpenGL Error: " << errCode << ": " << errString << " =========\n";
    }
}

// Returns the smallest N such that 2^N >= x
int getNext2PowN(int x)
{
    int p = 1;
    while (p < x) p <<= 1;
    return p;
}

// Textur laden
void TextureManager::loadTexture(const std::string& fileName, TextureId id, const LoadTextureInfo& loadTexInfo, int* w, int* pH)
{
    if ( m_textures.count( id )==1 )
    {
        log(Warning) << "Loading texture: ID \"" << id << "\" already exists, new texture was not loaded\n";
        return;
    }
    try
    {
        log(Info) << "Loading texture \"" << fileName << "\"... ";

        ILuint devIl_tex_id;                              // ID des DevIL Bildes
        ILboolean success;                                // Speichert ob die Funktionen erfolgreich sind
        ilGenImages(1, &devIl_tex_id);                    // neue Bild ID
        ilBindImage(devIl_tex_id);                        // Textur binden (als aktuelle setzen)
        success = ilLoadImage( (ILstring)fileName.c_str() );  // bild laden
        if (success)                                      // Falls keine Fehler
        {
            if ( w != NULL )
                *w = ilGetInteger(IL_IMAGE_WIDTH);
            if ( pH != NULL )
                *pH = ilGetInteger(IL_IMAGE_HEIGHT);
            // Breite und Höhe müssen in der Form 2^n darstellbar sein. (n ist eine natürliche Zahl)
            // Hier erhalten sie den nächsthöheren Wert der 2^n erfüllt.
            int width = ilGetInteger(IL_IMAGE_WIDTH);
            int height = ilGetInteger(IL_IMAGE_HEIGHT);
            int depht = ilGetInteger(IL_IMAGE_DEPTH);
            int width_2pown = getNext2PowN(width);
            int height_2pown = getNext2PowN(height);

            //log() << "GL: w=%i, h=%i\n", width_2pown, height_2pown );

            // TODO: don't shift small numbers
            switch (loadTexInfo.quality)
            {
            case QualityBest:
                if (width != width_2pown || height != height_2pown)
                    iluScale( width_2pown, height_2pown, depht);
                break;
            case QualityGood:
                iluScale( width_2pown>>1, height_2pown>>1, depht);
                break;
            case QualityMiddle:
                iluScale( width_2pown>>2, height_2pown>>2, depht);
                break;
            case QualityLow:
                iluScale( width_2pown>>3, height_2pown>>3, depht);
                break;
            case QualityLowest:
                iluScale( width_2pown>>4, height_2pown>>4, depht);
                break;
            }

            success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE); // Farbkomponente in unsigned byte konvertieren
            if (!success)
                throw 0; // Error

            GLuint openGl_tex_id;

            glGenTextures(1, &openGl_tex_id); // OpenGL Texture generieren
            glBindTexture(GL_TEXTURE_2D, openGl_tex_id); // binden

            log(Detail) << "OpenGL ID: " << openGl_tex_id << " ";

            GLint texWrapModeX = (loadTexInfo.wrapModeX == WrapClamp) ? GL_CLAMP : GL_REPEAT;
            GLint texWrapModeY = (loadTexInfo.wrapModeY == WrapClamp) ? GL_CLAMP : GL_REPEAT;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texWrapModeX);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texWrapModeY);

            if (loadTexInfo.loadMipmaps)
            {
                // Textur-Parameter setzen -> bestimmt die Qualität
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

                // Textur wird hier in die Grafikkarte geladen! Dabei werden Mipmaps generiert.
                gluBuild2DMipmaps(GL_TEXTURE_2D,                    // 2D Textur wird geladen
                                  GL_RGBA,                          // Farbiges Bild mit Alpha-Kanal
                                  ilGetInteger(IL_IMAGE_WIDTH),     // Breite des Bildes
                                  ilGetInteger(IL_IMAGE_HEIGHT),    // Höhe des Bildes
                                  GL_RGBA,                          // Format
                                  GL_UNSIGNED_BYTE,                 // Wie Daten aufgeschriben sind
                                  ilGetData());                     // Die Bilddaten überhaupt (Pixels)
                // TODO: GL_RGBA is not necessarily
            }
            else
            {
                // Textur-Parameter setzen -> bestimmt die Qualität
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                // Das ist die Alternative ohne Mipmaps
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH),
                    ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                    ilGetData());
            }
            TexInfo texInfo;
            texInfo.glTexId = openGl_tex_id;
            texInfo.scale = loadTexInfo.scale;
            m_textures.insert( std::make_pair(id,texInfo) ); // Textur in m_textures eintragen
        }
        else
             throw 0; // Error

        ilDeleteImages(1, &devIl_tex_id); // Löschen weil es schon eine kopie in OpenGL gibt
        log(Info) << "Done!\n";
    }
    catch (...)
    {
        // Error
        throw DataLoadException("Error while loading the texture \"" + fileName + "\"!\n" + iluErrorString(ilGetError()) + "\n");
    }
    CheckOpenlGlError();
    if (ilGetError())
        log(Warning) << "DevIL error: " << iluErrorString(ilGetError()) << "\n";
}

void TextureManager::freeTexture(const TextureId& id)
{
    TextureMap::iterator it = m_textures.find(id);
    assert (it != m_textures.end());

    freeTextureMemory(it->second);
    m_textures.erase(it);
}

void TextureManager::freeTextureMemory(const TexInfo& tex)
{
    GLint curTexId = 0;
    glGetIntegerv( GL_TEXTURE_BINDING_2D, &curTexId );

    if ( (unsigned)curTexId == tex.glTexId )
        glDisable( GL_TEXTURE_2D );
    log(Detail) << "Deleting OpenGL texture " << tex.glTexId << "\n";
    glDeleteTextures(1, &tex.glTexId);
}

void TextureManager::setTexture( const TextureId& id )
{
    TextureMap::const_iterator i ( m_textures.find( id ) );
    if ( i != m_textures.end() )
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_textures[id].glTexId );
        glMatrixMode(GL_TEXTURE);
        glLoadIdentity();
        glScalef(m_textures[id].scale, m_textures[id].scale, 0.0f);
        glMatrixMode(GL_MODELVIEW);
    }
    else
        log(Warning) << "*** SetTexture(): Texture ID '" << id << "' not found! ***\n";
}

std::vector<TextureId> TextureManager::getTextureList() const
{
    std::vector<TextureId> texList (m_textures.size());

    int i = 0;
    for( TextureMap::const_iterator it = m_textures.begin(); it != m_textures.end(); ++it, ++i )
    {
        texList[i] = it->first;
    }

    return texList;
}

void TextureManager::clear()
{
    //glBindTexture(GL_TEXTURE_2D, 0 );
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glDisable(GL_TEXTURE_2D);
}

AnimationManager::AnimationManager(TextureManager& tm)
: m_texManager( tm )
{}

// Destruktor
AnimationManager::~AnimationManager()
{
    foreach (const AnimInfoMap::value_type& a, m_animInfoMap)
    {
        freeAnimationMemory(*a.second);
    }
}

// Lädt eine Animationsdatei
void AnimationManager::loadAnimation(const std::string& fileName, AnimationId id, const LoadTextureInfo& texInfo)
{
    if ( m_animInfoMap.count( id )==1 )
    {
        log(Warning) << "Loading animation: ID '" << id << "' exists already, new animation was not loaded\n";
        return;
    }

    std::ifstream input_stream;
    input_stream.open(fileName.c_str());
    if( input_stream.fail() ) // Fehler beim Öffnen
    {
        throw DataLoadException("Animation file '" + fileName + "' could not be opened.\n");
    }

    boost::shared_ptr<AnimInfo> pAnimInfo = boost::make_shared<AnimInfo>();

    try
    {
        std::string prefix;
        std::string suffix;
        int num_digits;
        int num_frames;

        if ( !(input_stream >> num_frames) )
            throw 0;
        if ( !(input_stream >> prefix) )
            throw 0;
        if ( !(input_stream >> num_digits) )
            throw 0;
        if ( !(input_stream >> suffix ) )
            throw 0;

        std::string str_name ( fileName );
        std::string path = str_name.substr( 0, str_name.find_last_of('/')+1 );

        for ( int i = 0 ; i < num_frames; ++i )
        {
            std::stringstream digits_str;
            digits_str.fill('0');
            digits_str.width(num_digits);
            digits_str << i;
            std::string file_to_load = path + prefix + digits_str.str() + suffix;
            TextureId tex_id = std::string("_")+id+digits_str.str();
            m_texManager.loadTexture(file_to_load, tex_id, texInfo);
        }
        pAnimInfo->totalFrames = num_frames;
        pAnimInfo->numDigits = num_digits;
        pAnimInfo->name = id;

        AnimStateId stateId;
        while ( input_stream >> stateId )
        {
            std::map< AnimStateId, boost::shared_ptr<StateInfo> >::const_iterator it = 
                pAnimInfo->states.insert( std::make_pair( stateId, boost::make_shared<StateInfo>() ) ).first;
            if ( !(input_stream >> it->second->begin ) )
                throw 0;
            if ( !(input_stream >> it->second->end ) )
                throw 0;
            if ( !(input_stream >> it->second->speed ) )
                throw 0;
            int num_stops;
            if ( !(input_stream >> num_stops ) )
                throw 0;
            for ( int i = 0; i < num_stops; ++i )
            {
                int stop;
                if ( !(input_stream >> stop ) )
                    throw 0;
                it->second->stops.insert( stop );
            }
            m_animInfoMap.insert( std::make_pair( id, pAnimInfo ) );
        }
    }
    catch (...)
    {
        throw DataLoadException("Error loading '" + fileName + "'. (Bad syntax?)\n");
    }

    input_stream.close();
}

const AnimInfo* AnimationManager::getAnimInfo(AnimationId animId) const
{
    AnimInfoMap::const_iterator cit = m_animInfoMap.find( animId );
    if (cit!=m_animInfoMap.end())
        return cit->second.get();
    else
        return NULL;
}

void AnimationManager::freeAnimation(AnimationId id)
{
    AnimInfoMap::iterator it = m_animInfoMap.find(id);
    assert (it != m_animInfoMap.end());

    freeAnimationMemory(*it->second);
    m_animInfoMap.erase(it);
}

void AnimationManager::freeAnimationMemory(const AnimInfo& animInfo)
{
    for (int i = 0; i < animInfo.totalFrames; ++i)
    {
        std::stringstream frameNum;
        frameNum.fill('0');
        frameNum.width(animInfo.numDigits);
        frameNum << i;
        m_texManager.freeTexture(std::string("_") + animInfo.name + frameNum.str());
    }
}

