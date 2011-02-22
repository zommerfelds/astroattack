/*
 * Texture.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "Texture.h"
#include "main.h"       // für Logdatei
#include "Exception.h"  // Exceptions
#include <IL/il.h>      // DevIL für Laden von Bilddateien
#include <IL/ilu.h>     // DevIL (U) bearbeiten von Bilddaten
#include <fstream>
#include <sstream>
#include <cmath>
// cross platform OpenGL include (provided by SDL)
#include "SDL_opengl.h"
#include <boost/make_shared.hpp>

// Konstruktor
TextureManager::TextureManager()
{
    // Version von DevIL überprüfen.
    ILint il_dynamic_library_version = ilGetInteger(IL_VERSION_NUM);
    if ( il_dynamic_library_version < IL_VERSION )
    {
        // falsche Version
        throw Exception( gAaLog.Write ( "DevIL (IL) library is too old!\nFound %i, need > %i.", il_dynamic_library_version, IL_VERSION ) );
    }
    ILint ilu_dynamic_library_version = iluGetInteger(ILU_VERSION_NUM);
    if ( ilu_dynamic_library_version < ILU_VERSION )
    {
        // falsche Version
        throw Exception( gAaLog.Write ( "DevIL (ILU) library is too old!\nFound %i, need > %i.", ilu_dynamic_library_version, ILU_VERSION ) );
    }

    ilInit(); // Initialization von DevIL (IL)
    iluInit(); // Initialization von DevIL (ILU)
}

// Destruktor
TextureManager::~TextureManager()
{
    for ( TextureMap::iterator it = m_textures.begin(); it != m_textures.end(); ++it )
    {
        glDeleteTextures(1, &it->second.texAddress);
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
        gAaLog.Write ( "========= OpenGL Error: %u: %s =========\n", errCode, errString );
    }
}

// Returns the smallest number N so that 2^N >= x
int getNext2PowN( int x )
{
    int r1 = (int)pow(2.0f,(int)(log((float)x)/log(2.0f)+0.0001f));
    int r2 = (int)pow(2.0f,(int)(log((float)x)/log(2.0f)-0.0001f));
    if (r2 != r1 )
        return r1;
    else
        return r2*2;
}

// Textur laden
void TextureManager::LoadTexture( const std::string& name, TextureIdType id, const LoadTextureInfo& loadTexInfo, int quality, int* pW, int* pH )
{
    if ( m_textures.count( id )==1 )
    {
        gAaLog.Write( "*** WARNING: Loading texture: Texture with ID \"%s\" already exists! (new texture was not loaded) ***\n", id.c_str() );
        return;
    }
    try
    {
        //TODO: check for DevIL errors ilGetError()
        gAaLog.Write( "Loading Texture \"%s\"... ", name.c_str() );

        ILuint devIl_tex_id;                              // ID des DevIL Bildes
        ILboolean success;                                // Speichert ob die Funktionen erfolgreich sind
        ilGenImages(1, &devIl_tex_id);                    // neue Bild ID
        ilBindImage(devIl_tex_id);                        // Textur binden (als aktuelle setzen)
        success = ilLoadImage( (ILstring)name.c_str() );  // bild laden
        if (success)                                      // Falls keine Fehler
        {
            if ( pW != NULL )
                *pW = ilGetInteger(IL_IMAGE_WIDTH);
            if ( pH != NULL )
                *pH = ilGetInteger(IL_IMAGE_HEIGHT);
            // Breite und Höhe müssen in der Form 2^n darstellbar sein. (n ist eine natürliche Zahl)
            // Hier erhalten sie den nächsthöheren Wert der 2^n erfüllt.
            int width_2pown = getNext2PowN( ilGetInteger(IL_IMAGE_WIDTH) );
            int height_2pown = getNext2PowN( ilGetInteger(IL_IMAGE_HEIGHT) );
            
            //gAaLog.Write( "GL: w=%i, h=%i\n", width_2pown, height_2pown );

            // TODO: don't shift small numbers
            if ( quality == TEX_QUALITY_BEST )
                iluScale( width_2pown, height_2pown, ilGetInteger(IL_IMAGE_DEPTH));
            // Bild verkleinern, falls man eine schlechtere Qualität wünscht.
            else if ( quality == TEX_QUALITY_GOOD )
                iluScale( width_2pown>>1, height_2pown>>1, ilGetInteger(IL_IMAGE_DEPTH));
            else if ( quality == TEX_QUALITY_MIDDLE )
                iluScale( width_2pown>>2, height_2pown>>2, ilGetInteger(IL_IMAGE_DEPTH));
            else if ( quality == TEX_QUALITY_LOW )
                iluScale( width_2pown>>3, height_2pown>>3, ilGetInteger(IL_IMAGE_DEPTH));
            else if ( quality == TEX_QUALITY_LOWEST )
                iluScale( width_2pown>>4, height_2pown>>4, ilGetInteger(IL_IMAGE_DEPTH));

            success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE); // Farbkomponente in unsigned byte konvertieren
            if (!success) 
                throw 0; // Error

            GLuint openGl_tex_id;

            glGenTextures(1, &openGl_tex_id); // OpenGL Texture generieren
            glBindTexture(GL_TEXTURE_2D, openGl_tex_id); // binden

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, loadTexInfo.textureWrapModeX);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, loadTexInfo.textureWrapModeY);

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

                // TODO: GL_RGBA is not necessarily needed
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
            texInfo.texAddress = openGl_tex_id;
            texInfo.scale = loadTexInfo.scale;
            m_textures.insert( std::make_pair(id,texInfo) ); // Textur in m_textures eintragen
        }
        else
             throw 0; // Error

        ilDeleteImages(1, &devIl_tex_id); // Löschen weil es schon eine kopie in OpenGL gibt
        gAaLog.Write( "Done!\n" );
    }
    catch (...)
    {
        // Error
        throw Exception ( gAaLog.Write ( "Error while loading the texture \"%s\"!\n%s\n", name.c_str(), (const char*)iluErrorString(ilGetError()) ) );
    }
    CheckOpenlGlError();
    //gAaLog.Write ( "IL error: %s\n", (const char*)iluErrorString(ilGetError()) );
}

void TextureManager::FreeTexture( const TextureIdType& id )
{
    TextureMap::iterator c_it = m_textures.find( id );
    if ( c_it != m_textures.end() )
    {
        GLint curTexId = 0;
        glGetIntegerv( GL_TEXTURE_BINDING_2D, &curTexId );

        if ( curTexId == (GLint)c_it->second.texAddress )
            glDisable( GL_TEXTURE_2D );
        glDeleteTextures(1, &c_it->second.texAddress );
        m_textures.erase( c_it ); 
    }
}

void TextureManager::SetTexture( const TextureIdType& id )
{
    TextureMap::const_iterator i ( m_textures.find( id ) );
    if ( i != m_textures.end() )
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_textures[id].texAddress );
        glMatrixMode(GL_TEXTURE);
        glLoadIdentity();
        glScalef(m_textures[id].scale, m_textures[id].scale, 0.0f);
        glMatrixMode(GL_MODELVIEW);
        //m_currentTexture = id;
    }
    else
        gAaLog.Write ( "*** SetTexture(): Texture ID not found! ***\n" );
}

std::vector<TextureIdType> TextureManager::GetTextureList() const
{
    std::vector<TextureIdType> texList (m_textures.size());

    int i = 0;
    for( TextureMap::const_iterator it = m_textures.begin(); it != m_textures.end(); ++it, ++i )
    {
        texList[i] = it->first;
    }

    return texList;
}

void TextureManager::Clear()
{
    //glBindTexture(GL_TEXTURE_2D, 0 );
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glDisable(GL_TEXTURE_2D);
}

AnimationManager::AnimationManager( TextureManager* pTM )
: m_pTM( pTM )
{
}

// Lädt eine Animationsdatei
void AnimationManager::LoadAnimation( const char* name, AnimationIdType id,const LoadTextureInfo& texInfo, int quality )
{
    if ( m_animInfoMap.count( id )==1 )
    {
        gAaLog.Write ( "Warning loading animation: Animation with ID \"%s\" exists already! (new animation was not loaded)\n", id.c_str() );
        return;
    }

    std::ifstream input_stream;
    input_stream.open( name );
	if( input_stream.fail() ) // Fehler beim Öffnen
    {
        throw Exception ( gAaLog.Write ( "Animation file \"%s\" could not be opened.\n", name ) );
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

        std::string str_name ( name );
        std::string path = str_name.substr( 0, str_name.find_last_of('/')+1 );

        for ( int i = 0 ; i < num_frames; ++i )
        {
            std::stringstream digits_str;
            digits_str.fill('0');
            digits_str.width(num_digits);
            digits_str << i;
            std::string file_to_load = path + prefix + digits_str.str() + suffix;
            TextureIdType tex_id = std::string("_")+id+digits_str.str();
            m_pTM->LoadTexture(file_to_load,tex_id,texInfo,quality);
        }
        pAnimInfo->totalFrames = num_frames;
        pAnimInfo->numDigits = num_digits;
        pAnimInfo->name = id;

        StateIdType stateId;
        while ( input_stream >> stateId )
        {
            std::map< StateIdType, boost::shared_ptr<StateInfo> >::const_iterator it = 
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
        throw Exception ( gAaLog.Write ( "Error loading \"%s\". (Bad syntax?)\n", name ) );
    }

    input_stream.close();
}

const AnimInfo* AnimationManager::GetAnimInfo( AnimationIdType animId ) const
{
    AnimInfoMap::const_iterator cit = m_animInfoMap.find( animId );
    if (cit!=m_animInfoMap.end())
        return cit->second.get();
    else
        return NULL;
}

void AnimationManager::FreeAnimation( AnimationIdType id )
{
    AnimInfo* animInfo = m_animInfoMap[id].get();
    for ( int i = 0; i < animInfo->totalFrames; ++i )
    {
        std::stringstream digits_str;
        digits_str.fill('0');
        digits_str.width(animInfo->numDigits);
        digits_str << i;
        m_pTM->FreeTexture( animInfo->name + digits_str.str() );
    }
}
