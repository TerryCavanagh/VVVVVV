#ifndef MUSIC_H
#define MUSIC_H

#include "BinaryBlob.h"

#define musicroom(rx, ry) ((rx) + ((ry) * 20))

/* The amount of "space" for the scale of the user-set volume. */
#define USER_VOLUME_MAX 256

/* It is advised that USER_VOLUME_MAX be divisible by this. */
#define USER_VOLUME_STEP 32

enum
{
    Music_PATHCOMPLETE = 0,
    Music_PUSHINGONWARDS = 1,
    Music_POSITIVEFORCE = 2,
    Music_POTENTIALFORANYTHING = 3,
    Music_PASSIONFOREXPLORING = 4,
    Music_PAUSE = 5,
    Music_PRESENTINGVVVVVV = 6,
    Music_PLENARY = 7,
    Music_PREDESTINEDFATE = 8,
    Music_POSITIVEFORCEREVERSED = 9,
    Music_POPULARPOTPOURRI = 10,
    Music_PIPEDREAM = 11,
    Music_PRESSURECOOKER = 12,
    Music_PACEDENERGY = 13,
    Music_PIERCINGTHESKY = 14,
    Music_PREDESTINEDFATEREMIX = 15
};

enum
{
    Sound_FLIP = 0,
    Sound_UNFLIP = 1,
    Sound_CRY = 2,
    Sound_TRINKET = 3,
    Sound_COIN = 4,
    Sound_CHECKPOINT = 5,
    Sound_CRUMBLE = 6,
    Sound_DISAPPEAR = 7,
    Sound_GRAVITYLINE = 8,
    Sound_FLASH = 9,
    Sound_TELEPORT = 10,
    Sound_VIRIDIAN = 11,
    Sound_VERDIGRIS = 12,
    Sound_VICTORIA = 13,
    Sound_VITELLARY = 14,
    Sound_VIOLET = 15,
    Sound_VERMILION = 16,
    Sound_TERMINALTOUCH = 17,
    Sound_GAMESAVED = 18,
    Sound_ALARM = 19,
    Sound_TERMINALTEXT = 20,
    Sound_COUNTDOWN = 21,
    Sound_GO = 22,
    Sound_DESTROY = 23,
    Sound_COMBINE = 24,
    Sound_NEWRECORD = 25,
    Sound_TROPHY = 26,
    Sound_RESCUE = 27
};

class musicclass
{
public:
    musicclass(void);
    void init(void);
    void destroy(void);

    void play(int t);
    void playid(const char* id);
    bool idexists(const char* id);
    bool isextra(int t);
    const char* getid(int t);
    void resume(void);
    void resumefade(const int fadein_ms);
    void pause(void);
    void haltdasmusik(void);
    void haltdasmusik(bool from_fade);
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
    int haltedsong;

    void playef(int t);
    void pauseef(void);
    void resumeef(void);

    bool halted(void);
    void updatemutestate(void);

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
