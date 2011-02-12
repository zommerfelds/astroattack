/*----------------------------------------------------------\
|                        Sound.cpp                          |
|                        ---------                          |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// Sound Funktionen

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "Sound.h"
#include <SDL/SDL_mixer.h>
#include "main.h" // wichtige Definitionen und Dateien einbinden
#include "Exception.h" // Ausnahmen im Program (werden in main.cpp eingefangen)
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

SoundSubSystem::SoundSubSystem() : m_isInit ( false ), m_deletePlayingMusicAtEnd ( false )
{
}

SoundSubSystem::~SoundSubSystem()
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
    if ( m_isInit )
        Mix_CloseAudio();
}

bool SoundSubSystem::Init()
{
    if ( m_isInit )
    {
        gAaLog.Write ( "Warning in SoundSubSystem::Init(): Sound SubSystem was already initialized!\n" );
        return true;
    }
    if( Mix_OpenAudio( MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024 ) == -1 )
    {
        //throw Exception ( gAaLog.Write ( "Error in Mix_OpenAudio: %s\n", Mix_GetError() ) );
        gAaLog.Write ( "Error in Mix_OpenAudio: %s\n", Mix_GetError() );
        return false;
    }
    Mix_AllocateChannels(64);
    Mix_Volume(-1, (int)(MIX_MAX_VOLUME*gAaConfig.GetFloat("VolSound")*gAaConfig.GetFloat("VolMaster")) );
    Mix_VolumeMusic( (int)(MIX_MAX_VOLUME*gAaConfig.GetFloat("VolMusic")*gAaConfig.GetFloat("VolMaster")) );

    SoundSubSystem::soundSystemToNotifyMusicFinished = this;
    Mix_HookMusicFinished( &SoundSubSystem::MusicFinishedCallback );

    /*if(!Mix_RegisterEffect(MIX_CHANNEL_POST, Vol, NULL, NULL)) {
        gAaLog.Write("Mix_RegisterEffect: %s\n", Mix_GetError());
    }*/

    m_isInit = true;
    return true;
}

void SoundSubSystem::MusicFinishedCallback()
{
    soundSystemToNotifyMusicFinished->MusicFinished();
}

void SoundSubSystem::LoadSound( const char *name, SoundIdType id )
{
    if ( !m_isInit )
    {
        gAaLog.Write ( "Error in SoundSubSystem::LoadSound(): Sound SubSystem is not initialized!\n" );
        return;
    }
    if ( m_sounds.count( id )==1 )
    {
        gAaLog.Write ( "Warning loading sound: Sound with ID \"%s\" exists already! (new sound was not loaded)\n", id.c_str() );
        return;
    }
    Mix_Chunk *sample = Mix_LoadWAV( name );
    if( !sample )
    {
        gAaLog.Write ( "Error loading sound: %s\n", Mix_GetError() );
        return;
    }

    m_sounds.insert( std::make_pair(id,sample) );
}

void SoundSubSystem::FreeSound(const SoundIdType &id)
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

void SoundSubSystem::PlaySound(const SoundIdType &id)
{
    SoundMap::iterator c_it = m_sounds.find( id );
    if ( c_it != m_sounds.end() )
    {
        if( Mix_PlayChannel( -1, c_it->second, 0 ) == -1 )
        {
            gAaLog.Write ( "Error in Mix_PlayChannel: %s\n", Mix_GetError() );
        }
    }
}

void SoundSubSystem::LoadMusic(const char *name, MusicIdType id)
{
    if ( !m_isInit )
    {
        gAaLog.Write ( "Error in SoundSubSystem::LoadMusic(): Sound SubSystem is not initialized!\n" );
        return;
    }
    if ( m_music.count( id )==1 )
    {
        gAaLog.Write ( "Warning loading music: Music with ID \"%s\" exists already! (new music was not loaded)\n", id.c_str() );
        return;
    }
    Mix_Music *music;
    music=Mix_LoadMUS( name );
    if( !music )
    {
        gAaLog.Write ( "Error loading music: %s\n", Mix_GetError() );
        return;
    }

    m_music.insert( std::make_pair(id,music) );
}

void SoundSubSystem::FreeMusic(const MusicIdType &id)
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

void SoundSubSystem::PlayMusic(const MusicIdType &id, bool forever, int fadeInMs)
{
    MusicMap::iterator c_it = m_music.find( id );
    if ( c_it != m_music.end() )
    {
        if( Mix_FadeInMusic(c_it->second, forever?-1:0, fadeInMs) == -1 )
        {
            gAaLog.Write ( "Error in Mix_FadeInMusic: %s\n", Mix_GetError() );
        }
        m_currentPlayingMusic = c_it->second;
    }
}

void SoundSubSystem::StopMusic(int fadeOutMs)
{
    Mix_FadeOutMusic( fadeOutMs );
}

void SoundSubSystem::PauseMusic()
{
    if ( Mix_PlayingMusic() )
        Mix_PauseMusic();
}

void SoundSubSystem::ContinueMusic()
{
    Mix_ResumeMusic();
}

void SoundSubSystem::MusicFinished()
{
    m_currentPlayingMusic = NULL;
    //if ( m_deletePlayingMusicAtEnd )
}

// Astro Attack - Christian Zommerfelds - 2009
