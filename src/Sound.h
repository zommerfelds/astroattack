/*----------------------------------------------------------\
|                         Sound.h                           |
|                         -------                           |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// Sound Funktionen

#ifndef SOUND_H
#define SOUND_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include <map>
#include <string>
typedef std::string SoundIdType;
typedef std::string MusicIdType;

struct Mix_Chunk;
struct _Mix_Music;
typedef struct _Mix_Music Mix_Music;

typedef std::map<SoundIdType, Mix_Chunk*> SoundMap;
typedef std::map<SoundIdType, Mix_Music*> MusicMap;

class SoundSubSystem
{
public:
    SoundSubSystem();
    ~SoundSubSystem();

    bool Init();

    void LoadSound( const char* name, SoundIdType id );
    void FreeSound( const SoundIdType& id );
    void PlaySound( const SoundIdType& id );

    void LoadMusic( const char* name, MusicIdType id );
    void FreeMusic( const MusicIdType& id );
    void PlayMusic( const MusicIdType& id, bool forever, int fadeInMs );
    void StopMusic( int fadeOutMs );
    void PauseMusic();
    void ContinueMusic();

private:
    bool m_isInit;

    SoundMap m_sounds;
    MusicMap m_music;

    Mix_Music* m_currentPlayingMusic;
    bool m_deletePlayingMusicAtEnd;

    // does this really has to be static?
    static void MusicFinishedCallback();
    static SoundSubSystem* soundSystemToNotifyMusicFinished;
    void MusicFinished();
};

#endif

// Astro Attack - Christian Zommerfelds - 2009
