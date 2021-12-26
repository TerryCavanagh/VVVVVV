#ifndef MUSIC_H
#define MUSIC_H

#include <vector>

#include "BinaryBlob.h"
#include <SDL_mixer.h>

/* SDL_mixer object wrappers, do NOT reference these!!! */

class MusicTrack
{
public:
    MusicTrack(SDL_RWops *rw);
    Mix_Music *m_music;
    bool m_isValid;
};

class SoundTrack
{
public:
    SoundTrack(const char* fileName);
    Mix_Chunk *sound;
};

/* End SDL_mixer object wrappers */

#define musicroom(rx, ry) ((rx) + ((ry) * 20))

/* The amount of "space" for the scale of the user-set volume. */
#define USER_VOLUME_MAX 256

/* It is advised that USER_VOLUME_MAX be divisible by this. */
#define USER_VOLUME_STEP 32

class musicclass
{
public:
    musicclass(void);
    void init(void);
    void destroy(void);

    void play(int t);
    void resume();
    void resumefade(const int fadein_ms);
    void pause(void);
    void haltdasmusik(void);
    void silencedasmusik(void);
    void fadeMusicVolumeIn(int ms);
    void fadeMusicVolumeOut(const int fadeout_ms);
    void fadeout(const bool quick_fade_ = true);
    void fadein(void);
    void processmusicfadein(void);
    void processmusicfadeout(void);
    void processmusic(void);
    void niceplay(int t);

    void changemusicarea(int x, int y);

    int currentsong;

    void playef(int t);
    void pauseef(void);
    void resumeef(void);

    bool halted(void);
    void updatemutestate(void);

    std::vector<SoundTrack> soundTracks;
    std::vector<MusicTrack> musicTracks;
    bool safeToProcessMusic;

    int nicechange; // -1 if no song queued
    bool nicefade;

    bool m_doFadeInVol;
    bool m_doFadeOutVol;
    int musicVolume;

    /* 0..USER_VOLUME_MAX */
    int user_music_volume;
    int user_sound_volume;

    bool quick_fade;

    // MMMMMM mod settings
    bool mmmmmm;
    bool usingmmmmmm;

    binaryBlob pppppp_blob;
    binaryBlob mmmmmm_blob;
    int num_pppppp_tracks;
    int num_mmmmmm_tracks;
};

#ifndef MUSIC_DEFINITION
extern musicclass music;
#endif

#endif /* MUSIC_H */
