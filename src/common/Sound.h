/*
 * Sound.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Sound Funktionen

#ifndef SOUND_H
#define SOUND_H

#include <map>
#include <string>

// forward declaration of Mix_Music
struct Mix_Chunk;
struct _Mix_Music;
typedef struct _Mix_Music Mix_Music;

typedef std::string SoundId;
typedef std::string MusicId;

class SoundSubSystem
{
public:
    SoundSubSystem();
    ~SoundSubSystem();

    bool init(float volSound, float volMusic, float volMaster);
    void deInit();

    void loadSound( const std::string& name, SoundId id );
    void freeSound( const SoundId& id );
    void playSound( const SoundId& id );

    void loadMusic( const std::string& name, MusicId id );
    void freeMusic( const MusicId& id );
    void playMusic( const MusicId& id, bool forever, int fadeInMs );
    void stopMusic( int fadeOutMs );
    void pauseMusic();
    void continueMusic();

private:

    typedef std::map<SoundId, Mix_Chunk*> SoundMap;
    typedef std::map<SoundId, Mix_Music*> MusicMap;

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
