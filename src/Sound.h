/*
 * Sound.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

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

    bool init();
    void deInit();

    void loadSound( const char* name, SoundIdType id );
    void freeSound( const SoundIdType& id );
    void playSound( const SoundIdType& id );

    void loadMusic( const char* name, MusicIdType id );
    void freeMusic( const MusicIdType& id );
    void playMusic( const MusicIdType& id, bool forever, int fadeInMs );
    void stopMusic( int fadeOutMs );
    void pauseMusic();
    void continueMusic();

private:
    bool m_isInit;

    SoundMap m_sounds;
    MusicMap m_music;

    Mix_Music* m_currentPlayingMusic;
    bool m_deletePlayingMusicAtEnd;

    // does this really has to be static?
    static void musicFinishedCallback();
    static SoundSubSystem* soundSystemToNotifyMusicFinished;
    void onMusicFinished();
};

#endif
