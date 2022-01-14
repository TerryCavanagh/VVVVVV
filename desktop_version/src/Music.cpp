#define MUSIC_DEFINITION
#include "Music.h"

#include <SDL.h>
#include <physfsrwops.h>

#include "BinaryBlob.h"
#include "FileSystemUtils.h"
#include "Game.h"
#include "Graphics.h"
#include "Map.h"
#include "Script.h"
#include "UtilityClass.h"
#include "Vlogging.h"

/* Begin SDL_mixer wrapper */

#include <SDL_mixer.h>
#include <vector>

#define VVV_MAX_VOLUME MIX_MAX_VOLUME

class MusicTrack
{
public:
    MusicTrack(SDL_RWops *rw)
    {
        m_music = Mix_LoadMUS_RW(rw, 1);
        m_isValid = true;
        if (m_music == NULL)
        {
            vlog_error("Unable to load Magic Binary Music file: %s", Mix_GetError());
            m_isValid = false;
        }
    }

    void Dispose()
    {
        Mix_FreeMusic(m_music);
    }

    bool Play(bool loop)
    {
        if (Mix_PlayMusic(m_music, loop ? -1 : 0) == -1)
        {
            vlog_error("Mix_PlayMusic: %s", Mix_GetError());
            return false;
        }
        return true;
    }

    static void Init(int audio_rate, int audio_channels)
    {
        const Uint16 audio_format = AUDIO_S16SYS;
        const int audio_buffers = 1024;

        if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0)
        {
            vlog_error("Unable to initialize audio: %s", Mix_GetError());
            SDL_assert(0 && "Unable to initialize audio!");
        }
    }

    static void Halt()
    {
        Mix_HaltMusic();
    }

    static bool IsHalted()
    {
        return Mix_PausedMusic() == 1;
    }

    static void Pause()
    {
        Mix_PauseMusic();
    }

    static void Resume()
    {
        Mix_ResumeMusic();
    }

    static void SetVolume(int musicVolume)
    {
        Mix_VolumeMusic(musicVolume);
    }

private:
    Mix_Music *m_music;
    bool m_isValid;
};

class SoundTrack
{
public:
    SoundTrack(const char* fileName)
    {
        unsigned char *mem;
        size_t length;
        FILESYSTEM_loadAssetToMemory(fileName, &mem, &length, false);
        if (mem == NULL)
        {
            m_sound = NULL;
            vlog_error("Unable to load WAV file %s", fileName);
            SDL_assert(0 && "WAV file missing!");
            return;
        }
        SDL_RWops *fileIn = SDL_RWFromConstMem(mem, length);
        m_sound = Mix_LoadWAV_RW(fileIn, 1);
        FILESYSTEM_freeMemory(&mem);

        if (m_sound == NULL)
        {
            vlog_error("Unable to load WAV file: %s", Mix_GetError());
        }
    }

    void Dispose()
    {
        Mix_FreeChunk(m_sound);
    }

    void Play()
    {
        if (Mix_PlayChannel(-1, m_sound, 0) == -1)
        {
            vlog_error("Unable to play WAV file: %s", Mix_GetError());
        }
    }

    static void Pause()
    {
        Mix_Pause(-1);
    }

    static void Resume()
    {
        Mix_Resume(-1);
    }

    static void SetVolume(int soundVolume)
    {
        Mix_Volume(-1, soundVolume);
    }

private:
    Mix_Chunk *m_sound;
};

static std::vector<MusicTrack> musicTracks;
static std::vector<SoundTrack> soundTracks;

/* End SDL_mixer wrapper */

musicclass::musicclass(void)
{
    MusicTrack::Init(44100, 2);

    safeToProcessMusic= false;
    m_doFadeInVol = false;
    m_doFadeOutVol = false;
    musicVolume = 0;

    user_music_volume = USER_VOLUME_MAX;
    user_sound_volume = USER_VOLUME_MAX;

    currentsong = 0;
    nicechange = -1;
    nicefade = false;
    quick_fade = true;

    usingmmmmmm = false;
}

void musicclass::init(void)
{
    soundTracks.push_back(SoundTrack( "sounds/jump.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/jump2.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/hurt.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/souleyeminijingle.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/coin.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/save.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/crumble.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/vanish.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/blip.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/preteleport.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/teleport.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/crew1.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/crew2.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/crew3.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/crew4.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/crew5.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/crew6.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/terminal.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/gamesaved.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/crashing.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/blip2.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/countdown.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/go.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/crash.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/combine.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/newrecord.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/trophy.wav" ));
    soundTracks.push_back(SoundTrack( "sounds/rescue.wav" ));

#ifdef VVV_COMPILEMUSIC
    binaryBlob musicWriteBlob;
#define FOREACH_TRACK(blob, track_name) blob.AddFileToBinaryBlob("data/" track_name);
    TRACK_NAMES(musicWriteBlob)
#undef FOREACH_TRACK

    musicWriteBlob.writeBinaryBlob("data/BinaryMusic.vvv");
    musicWriteBlob.clear();
#endif

    num_mmmmmm_tracks = 0;
    num_pppppp_tracks = 0;

    if (!mmmmmm_blob.unPackBinary("mmmmmm.vvv"))
    {
        if (pppppp_blob.unPackBinary("vvvvvvmusic.vvv"))
        {
            vlog_info("Loading music from PPPPPP blob...");

            mmmmmm = false;
            usingmmmmmm=false;

            int index;
            SDL_RWops* rw;

#define FOREACH_TRACK(blob, track_name) \
    index = blob.getIndex("data/" track_name); \
    rw = SDL_RWFromMem(blob.getAddress(index), blob.getSize(index)); \
    musicTracks.push_back(MusicTrack( rw ));

            TRACK_NAMES(pppppp_blob)

#undef FOREACH_TRACK
        }
        else
        {
            vlog_info("Loading music from loose files...");

            SDL_RWops* rw;
#define FOREACH_TRACK(_, track_name) \
    rw = PHYSFSRWOPS_openRead(track_name); \
    musicTracks.push_back(MusicTrack( rw ));

            TRACK_NAMES(_)

#undef FOREACH_TRACK
        }
    }
    else
    {
        vlog_info("Loading PPPPPP and MMMMMM blobs...");

        mmmmmm = true;
        int index;
        SDL_RWops *rw;

#define FOREACH_TRACK(blob, track_name) \
    index = blob.getIndex("data/" track_name); \
    if (index >= 0 && index < blob.max_headers) \
    { \
        rw = SDL_RWFromConstMem(blob.getAddress(index), blob.getSize(index)); \
        if (rw == NULL) \
        { \
            vlog_error("Unable to read music file header: %s", SDL_GetError()); \
        } \
        else \
        { \
            musicTracks.push_back(MusicTrack( rw )); \
        } \
    }

        TRACK_NAMES(mmmmmm_blob)

        num_mmmmmm_tracks += musicTracks.size();

        size_t index_ = 0;
        while (mmmmmm_blob.nextExtra(&index_))
        {
            rw = SDL_RWFromConstMem(mmmmmm_blob.getAddress(index_), mmmmmm_blob.getSize(index_));
            musicTracks.push_back(MusicTrack( rw ));

            num_mmmmmm_tracks++;
            index_++;
        }

        bool ohCrap = pppppp_blob.unPackBinary("vvvvvvmusic.vvv");
        SDL_assert(ohCrap && "Music not found!");

    TRACK_NAMES(pppppp_blob)

#undef FOREACH_TRACK
    }

    num_pppppp_tracks += musicTracks.size() - num_mmmmmm_tracks;

    SDL_RWops* rw;
    size_t index_ = 0;
    while (pppppp_blob.nextExtra(&index_))
    {
        rw = SDL_RWFromConstMem(pppppp_blob.getAddress(index_), pppppp_blob.getSize(index_));
        musicTracks.push_back(MusicTrack( rw ));

        num_pppppp_tracks++;
        index_++;
    }
}

void musicclass::destroy(void)
{
    for (size_t i = 0; i < soundTracks.size(); ++i)
    {
        soundTracks[i].Dispose();
    }
    soundTracks.clear();

    // Before we free all the music: stop playing music, else SDL2_mixer
    // will call SDL_Delay() if we are fading, resulting in no-draw frames
    MusicTrack::Halt();

    for (size_t i = 0; i < musicTracks.size(); ++i)
    {
        musicTracks[i].Dispose();
    }
    musicTracks.clear();

    pppppp_blob.clear();
    mmmmmm_blob.clear();
}

void musicclass::play(int t)
{
    if (mmmmmm && usingmmmmmm)
    {
        // Don't conjoin this if-statement with the above one...
        if (num_mmmmmm_tracks > 0)
        {
            t %= num_mmmmmm_tracks;
        }
    }
    else if (num_pppppp_tracks > 0)
    {
        t %= num_pppppp_tracks;
    }

    if (mmmmmm && !usingmmmmmm)
    {
        t += num_mmmmmm_tracks;
    }

    safeToProcessMusic = true;

    if (currentsong == t && !m_doFadeOutVol)
    {
        return;
    }

    currentsong = t;

    if (t == -1)
    {
        return;
    }

    if (!INBOUNDS_VEC(t, musicTracks))
    {
        vlog_error("play() out-of-bounds!");
        currentsong = -1;
        return;
    }

    if (currentsong == 0 || currentsong == 7 || (!map.custommode && (currentsong == 0+num_mmmmmm_tracks || currentsong == 7+num_mmmmmm_tracks)))
    {
        // Level Complete theme, no fade in or repeat
        if (musicTracks[t].Play(false))
        {
            m_doFadeInVol = false;
            m_doFadeOutVol = false;
            musicVolume = VVV_MAX_VOLUME;
            MusicTrack::SetVolume(musicVolume);
        }
    }
    else
    {
        if (m_doFadeOutVol)
        {
            // We're already fading out
            nicechange = t;
            nicefade = true;
            currentsong = -1;

            if (quick_fade)
            {
                fadeMusicVolumeOut(500); // fade out quicker
            }
            else
            {
                quick_fade = true;
            }
        }
        else if (musicTracks[t].Play(true))
        {
            m_doFadeInVol = false;
            m_doFadeOutVol = false;
            fadeMusicVolumeIn(3000);
        }
    }
}

void musicclass::resume()
{
    MusicTrack::Resume();
}

void musicclass::resumefade(const int fadein_ms)
{
    resume();
    fadeMusicVolumeIn(fadein_ms);
}

void musicclass::fadein(void)
{
    resumefade(3000); // 3000 ms fadein
}

void musicclass::pause(void)
{
    MusicTrack::Pause();
}

void musicclass::haltdasmusik(void)
{
    /* Just pauses music. This is intended. */
    pause();
    currentsong = -1;
    m_doFadeInVol = false;
    m_doFadeOutVol = false;
}

void musicclass::silencedasmusik(void)
{
    musicVolume = 0;
    m_doFadeInVol = false;
    m_doFadeOutVol = false;
}

struct FadeState
{
    int start_volume;
    int end_volume;
    int duration_ms;
    int step_ms;
};

static struct FadeState fade;

enum FadeCode
{
    Fade_continue,
    Fade_finished
};

static enum FadeCode processmusicfade(struct FadeState* state, int* volume)
{
    int range;
    int new_volume;

    if (state->duration_ms == 0 /* Fast path. */
    || state->start_volume == state->end_volume /* Fast path. */
    || state->step_ms >= state->duration_ms /* We're finished. */)
    {
        *volume = state->end_volume;
        state->step_ms = 0;
        return Fade_finished;
    }

    range = state->end_volume - state->start_volume;
    new_volume = range * state->step_ms / state->duration_ms;
    new_volume += state->start_volume;

    *volume = new_volume;

    state->step_ms += game.get_timestep();

    return Fade_continue;
}

void musicclass::fadeMusicVolumeIn(int ms)
{
    if (halted())
    {
        return;
    }

    m_doFadeInVol = true;
    m_doFadeOutVol = false;

    /* Ensure it starts at 0 */
    musicVolume = 0;

    /* Fix 1-frame glitch */
    MusicTrack::SetVolume(0);

    fade.step_ms = 0;
    fade.duration_ms = ms;
    fade.start_volume = 0;
    fade.end_volume = VVV_MAX_VOLUME;
}

void musicclass::fadeMusicVolumeOut(const int fadeout_ms)
{
    if (halted())
    {
        return;
    }

    m_doFadeInVol = false;
    m_doFadeOutVol = true;

    fade.step_ms = 0;
    /* Duration is proportional to current volume. */
    fade.duration_ms = fadeout_ms * musicVolume / VVV_MAX_VOLUME;
    fade.start_volume = musicVolume;
    fade.end_volume = 0;
}

void musicclass::fadeout(const bool quick_fade_ /*= true*/)
{
    fadeMusicVolumeOut(quick_fade_ ? 500 : 2000);
    quick_fade = quick_fade_;
}

void musicclass::processmusicfadein(void)
{
    enum FadeCode fade_code = processmusicfade(&fade, &musicVolume);
    if (fade_code == Fade_finished)
    {
        m_doFadeInVol = false;
    }
}

void musicclass::processmusicfadeout(void)
{
    enum FadeCode fade_code = processmusicfade(&fade, &musicVolume);
    if (fade_code == Fade_finished)
    {
        musicVolume = 0;
        m_doFadeOutVol = false;
        haltdasmusik();
    }
}

void musicclass::processmusic(void)
{
    if(!safeToProcessMusic)
    {
        return;
    }

    if(m_doFadeInVol)
    {
        processmusicfadein();
    }

    if (m_doFadeOutVol)
    {
        processmusicfadeout();
    }

    /* This needs to come after processing fades */
    if (nicefade && halted())
    {
        play(nicechange);
        nicechange = -1;
        nicefade = false;
    }
}


void musicclass::niceplay(int t)
{
    /* important: do nothing if the correct song is playing! */
    if ((!mmmmmm && currentsong != t)
    || (mmmmmm && usingmmmmmm && currentsong != t)
    || (mmmmmm && !usingmmmmmm && currentsong != t + num_mmmmmm_tracks))
    {
        if (currentsong != -1)
        {
            fadeout(false);
        }
        nicefade = true;
    }
    nicechange = t;
}

static const int areamap[] = {
    4, 3, 3, 3, 3, 3, 3, 3, 4,-2, 4, 4, 4,12,12,12,12,12,12,12,
    4, 3, 3, 3, 3, 3, 3, 4, 4,-2, 4, 4, 4, 4,12,12,12,12,12,12,
    4, 4, 4, 4, 3, 4, 4, 4, 4,-2, 4, 4, 4, 4,12,12,12,12,12,12,
    4, 4, 4, 4, 3, 4, 4, 4, 4,-2, 4, 4, 1, 1, 1, 1,12,12,12,12,
    4, 4, 3, 3, 3, 4, 4, 4, 4,-2,-2,-2, 1, 1, 1, 1, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4,-2, 1, 1, 1, 1, 1, 1,11,11,-1, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4,-2, 1, 1, 1, 1, 1, 1, 1,11,11,11,
    4, 4, 4, 4, 4, 4, 4, 4, 4,-2, 1, 1, 1, 1, 1, 1, 1, 1, 1,11,
    4, 4, 4, 4, 4, 4, 4, 4, 4,-2, 4, 4, 4, 1, 1, 1, 1, 1, 1, 3,
    4, 4, 4, 4, 4, 4, 4, 4,-2,-2, 4, 4, 4, 1, 1, 1, 1, 1, 1, 4,
    4, 4,-1,-1,-1, 4, 4, 4, 4,-2, 4, 4, 4, 1, 1, 1, 1, 1, 1, 4,
    4, 4,-1,-1,-1, 4, 4, 4, 4,-2, 4, 1, 1, 1, 1, 1, 1, 1, 1, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4,-2, 4, 1, 1, 1, 1, 1, 1, 4, 1, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4,-2, 4, 1, 1, 1, 1, 1, 1, 4, 1, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4,-2, 4,-1,-3, 4, 4, 4, 4, 4, 1, 4,
    4, 4, 4, 4, 4, 3, 3, 3, 4,-2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 3, 3, 3, 3, 3, 3, 4,-2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 3, 3, 3, 3, 3, 3, 3, 4,-2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    3, 3, 3, 3, 3, 4, 4, 3, 4,-2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    3, 3, 3, 3, 3, 4, 4, 3, 4,-2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4
};

SDL_COMPILE_TIME_ASSERT(areamap, SDL_arraysize(areamap) == 20 * 20);

void musicclass::changemusicarea(int x, int y)
{
    int room;
    int track;

    if (script.running)
    {
        return;
    }

    room = musicroom(x, y);

    if (!INBOUNDS_ARR(room, areamap))
    {
        SDL_assert(0 && "Music map index out-of-bounds!");
        return;
    }

    track = areamap[room];

    switch (track)
    {
    case -1:
        /* Don't change music. */
        return;
    case -2:
        /* Special case: Tower music, changes with Flip Mode. */
        if (graphics.setflipmode)
        {
            track = 9; /* ecroF evitisoP */
        }
        else
        {
            track = 2; /* Positive Force */
        }
        break;
    case -3:
        /* Special case: start of Space Station 2. */
        if (game.intimetrial)
        {
            track = 1; /* Pushing Onwards */
        }
        else
        {
            track = 4; /* Passion for Exploring */
        }
        break;
    }

    niceplay(track);
}

void musicclass::playef(int t)
{
    if (!INBOUNDS_VEC(t, soundTracks))
    {
        return;
    }
    soundTracks[t].Play();
}

void musicclass::pauseef(void)
{
    SoundTrack::Pause();
}

void musicclass::resumeef(void)
{
    SoundTrack::Resume();
}

bool musicclass::halted(void)
{
    return MusicTrack::IsHalted();
}

void musicclass::updatemutestate(void)
{
    if (game.muted)
    {
        MusicTrack::SetVolume(0);
        SoundTrack::SetVolume(0);
    }
    else
    {
        SoundTrack::SetVolume(VVV_MAX_VOLUME * user_sound_volume / USER_VOLUME_MAX);

        if (game.musicmuted)
        {
            MusicTrack::SetVolume(0);
        }
        else
        {
            MusicTrack::SetVolume(musicVolume * user_music_volume / USER_VOLUME_MAX);
        }
    }
}
