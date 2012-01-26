/*
 * Sound.cpp
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#include "Sound.h"

#include "Logger.h"
#include "Exception.h"

#include <SDL_mixer.h>
#include <boost/bind.hpp>

/*void Vol(int chan, void *stream, int len, void *udata)
{
    // 16 signed bits (lsb) * 2 channels.
    Sint16 *ptr = (Sint16 *) stream;

    for (int i = 0; i < len; i += sizeof (Sint16) * 2) {
        *(ptr+0) = *(ptr+0);
        *(ptr+1) = *(ptr+1);
        ptr+=2;
    }
}*/

SoundSubSystem* SoundSubSystem::soundSystemToNotifyMusicFinished = NULL;

SoundSubSystem::SoundSubSystem() : m_isInit ( false ), m_currentPlayingMusic (), m_deletePlayingMusicAtEnd ( false )
{
}

SoundSubSystem::~SoundSubSystem()
{
    deInit();
}

bool SoundSubSystem::init(float volSound, float volMusic, float volMaster)
{
    if ( m_isInit )
    {
        log(Warning) << "SoundSubSystem::Init(): Sound SubSystem was already initialized!\n";
        return true;
    }
    if( Mix_OpenAudio( MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024 ) == -1 )
    {
        //throw Exception ( log() << "Error in Mix_OpenAudio: %s\n", Mix_GetError() ) );
        log(Error) << "Error in Mix_OpenAudio: " << Mix_GetError() << "\n";
        return false;
    }
    Mix_AllocateChannels(64);
    Mix_Volume(-1, (int)(MIX_MAX_VOLUME * volSound * volMaster));
    Mix_VolumeMusic((int)(MIX_MAX_VOLUME * volMusic * volMaster));

    SoundSubSystem::soundSystemToNotifyMusicFinished = this;
    Mix_HookMusicFinished( &SoundSubSystem::musicFinishedCallback );

    /*if(!Mix_RegisterEffect(MIX_CHANNEL_POST, Vol, NULL, NULL)) {
        log() <<"Mix_RegisterEffect: %s\n", Mix_GetError());
    }*/

    m_isInit = true;
    return true;
}

void SoundSubSystem::deInit()
{
    if (m_isInit)
    {
        Mix_HookMusicFinished( NULL );
        Mix_HaltChannel(-1);
        for ( SoundMap::iterator it = m_sounds.begin(); it != m_sounds.end(); ++it )
        {
            Mix_FreeChunk( it->second );
        }
        for ( MusicMap::iterator it = m_music.begin(); it != m_music.end(); ++it )
        {
            Mix_FreeMusic( it->second );
        }
        Mix_CloseAudio();

        m_isInit = false;
    }
}

void SoundSubSystem::musicFinishedCallback()
{
    soundSystemToNotifyMusicFinished->onMusicFinished();
}

void SoundSubSystem::loadSound( const std::string& name, SoundId id )
{
    if ( !m_isInit )
    {
        log(Error) << "SoundSubSystem::LoadSound(): Sound SubSystem is not initialized!\n";
        return;
    }
    if ( m_sounds.count( id )==1 )
    {
        log(Error) << "Loading sound: Sound with ID \"" << id << "\" exists already! (new sound was not loaded)\n";
        return;
    }
    Mix_Chunk *sample = Mix_LoadWAV( name.c_str() );
    if( !sample )
    {
        log(Error) << "Loading sound: " << Mix_GetError() << "\n";
        return;
    }

    m_sounds.insert( std::make_pair(id,sample) );
}

void SoundSubSystem::freeSound(const SoundId &id)
{
    SoundMap::iterator c_it = m_sounds.find( id );
    if ( c_it != m_sounds.end() )
    {
        if ( Mix_Playing( -1 ) )
        {
            int numChannels = Mix_AllocateChannels( -1 );
            for ( int i = 0; i < numChannels; ++i )
            {
                if ( Mix_GetChunk( i ) == c_it->second )
                    Mix_HaltChannel( i );
            }
        }
        Mix_FreeChunk( c_it->second );
        m_sounds.erase( c_it ); 
    }
}

void SoundSubSystem::playSound(const SoundId &id)
{
    SoundMap::iterator c_it = m_sounds.find( id );
    if ( c_it != m_sounds.end() )
    {
        if( Mix_PlayChannel( -1, c_it->second, 0 ) == -1 )
        {
            log(Error) << "Error in Mix_PlayChannel: " << Mix_GetError() << "\n";
        }
    }
}

void SoundSubSystem::loadMusic(const std::string& name, MusicId id)
{
    if ( !m_isInit )
    {
        log(Error) << "SoundSubSystem::LoadMusic(): Sound SubSystem is not initialized!\n";
        return;
    }
    if ( m_music.count( id )==1 )
    {
        log(Warning) << "Loading music: Music with ID \"" << id << "\" exists already! (new music was not loaded)\n";
        return;
    }
    Mix_Music *music;
    music=Mix_LoadMUS( name.c_str() );
    if( !music )
    {
        log(Error) << "Loading music: " << Mix_GetError() << "\n";
        return;
    }

    m_music.insert( std::make_pair(id,music) );
}

void SoundSubSystem::freeMusic(const MusicId &id)
{
    MusicMap::iterator c_it = m_music.find( id );
    if ( c_it != m_music.end() )
    {
        //if ( m_currentPlayingMusic == c_it->second )
            //StopMusic( 0 );
        Mix_FreeMusic( c_it->second );
        m_music.erase( c_it ); 
    }
}

void SoundSubSystem::playMusic(const MusicId &id, bool forever, int fadeInMs)
{
    if (Mix_VolumeMusic(-1) == 0)
        return; // dont do anything if volume is 0, since you cant change the volume while playing. (tmp for saving fading out time)
    MusicMap::iterator c_it = m_music.find( id );
    if ( c_it != m_music.end() )
    {
        if( Mix_FadeInMusic(c_it->second, forever?-1:0, fadeInMs) == -1 )
        {
            log(Error) << "Mix_FadeInMusic: " << Mix_GetError() << "\n";
        }
        m_currentPlayingMusic = c_it->second;
    }
}

void SoundSubSystem::stopMusic(int fadeOutMs)
{
    Mix_FadeOutMusic( fadeOutMs );
}

void SoundSubSystem::pauseMusic()
{
    if ( Mix_PlayingMusic() )
        Mix_PauseMusic();
}

void SoundSubSystem::continueMusic()
{
    Mix_ResumeMusic();
}

void SoundSubSystem::onMusicFinished()
{
    m_currentPlayingMusic = NULL;
    //if ( m_deletePlayingMusicAtEnd )
}
