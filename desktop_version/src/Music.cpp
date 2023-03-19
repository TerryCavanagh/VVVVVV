#define MUSIC_DEFINITION
#include "Music.h"

#include <SDL.h>
#include <FAudio.h>
#include <physfsrwops.h>

#include "Alloc.h"
#include "BinaryBlob.h"
#include "FileSystemUtils.h"
#include "Game.h"
#include "Graphics.h"
#include "Map.h"
#include "Script.h"
#include "Unused.h"
#include "UtilityClass.h"
#include "Vlogging.h"

#include <vector>

/* stb_vorbis */

#define malloc SDL_malloc
#define realloc SDL_realloc
#define free VVV_free
#ifdef memset /* Thanks, Apple! */
#undef memset
#endif
#define memset SDL_memset
#ifdef memcpy /* Thanks, Apple! */
#undef memcpy
#endif
#define memcpy SDL_memcpy
#define memcmp SDL_memcmp

#define pow SDL_pow
#define log(x) SDL_log(x)
#define sin(x) SDL_sin(x)
#define cos(x) SDL_cos(x)
#define floor SDL_floor
#define abs(x) SDL_abs(x)
#define ldexp(v, e) SDL_scalbn((v), (e))
#define exp(x) SDL_exp(x)

#define qsort SDL_qsort

#define assert SDL_assert

#define FILE SDL_RWops
#ifdef SEEK_SET
#undef SEEK_SET
#endif
#ifdef SEEK_CUR
#undef SEEK_CUR
#endif
#ifdef SEEK_END
#undef SEEK_END
#endif
#ifdef EOF
#undef EOF
#endif
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define EOF -1
#define fopen(path, mode) SDL_RWFromFile(path, mode)
#define fopen_s(io, path, mode) (!(*io = fopen(path, mode)))
#define fclose(io) SDL_RWclose(io)
#define fread(dst, size, count, io) SDL_RWread(io, dst, size, count)
#define fseek(io, offset, whence) SDL_RWseek(io, offset, whence)
#define ftell(io) SDL_RWtell(io)

#define FAudio_alloca(x) SDL_stack_alloc(uint8_t, x)
#define FAudio_dealloca(x) SDL_stack_free(x)

#define STB_VORBIS_NO_PUSHDATA_API 1
#define STB_VORBIS_NO_INTEGER_CONVERSION 1
#include <stb_vorbis.h>

/* End stb_vorbis include */

#define VVV_MAX_VOLUME 128
#define VVV_MAX_CHANNELS 8

class SoundTrack;
class MusicTrack;
static std::vector<SoundTrack> soundTracks;
static std::vector<MusicTrack> musicTracks;

static FAudio* faudioctx = NULL;
static FAudioMasteringVoice* masteringvoice = NULL;

class SoundTrack
{
public:
    SoundTrack(const char* fileName)
    {
        unsigned char *mem;
        size_t length;
        SDL_AudioSpec spec;
        SDL_RWops *fileIn;
        SDL_zerop(this);
        FILESYSTEM_loadAssetToMemory(fileName, &mem, &length);
        if (mem == NULL)
        {
            vlog_error("Unable to load WAV file %s", fileName);
            SDL_assert(0 && "WAV file missing!");
            return;
        }
        fileIn = SDL_RWFromConstMem(mem, length);
        if (SDL_LoadWAV_RW(fileIn, 1, &spec, &wav_buffer, &wav_length) == NULL)
        {
            vlog_error("Unable to load WAV file %s", fileName);
            goto end;
        }
        format.nChannels = spec.channels;
        format.nSamplesPerSec = spec.freq;
        format.wFormatTag = FAUDIO_FORMAT_PCM;
        format.wBitsPerSample = SDL_AUDIO_BITSIZE(spec.format);
        format.nBlockAlign = format.nChannels * format.wBitsPerSample;
        format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
        format.cbSize = 0;
        valid = true;
end:
        VVV_free(mem);
    }

    void Dispose(void)
    {
        VVV_free(wav_buffer);
    }

    void Play(void)
    {
        if (!valid)
        {
            return;
        }

        for (int i = 0; i < VVV_MAX_CHANNELS; i++)
        {
            FAudioVoiceState voicestate;
            FAudioSourceVoice_GetState(voices[i], &voicestate, 0);
            if (voicestate.BuffersQueued == 0)
            {
                FAudioVoiceDetails details;
                FAudioVoice_GetVoiceDetails(voices[i], &details);
                if (details.InputSampleRate != format.nSamplesPerSec ||
                details.InputChannels != format.nChannels)
                {
                    VVV_freefunc(FAudioVoice_DestroyVoice, voices[i]);
                    FAudio_CreateSourceVoice(faudioctx, &voices[i], &format, 0, 2.0f, NULL, NULL, NULL);
                }
                const FAudioBuffer faudio_buffer = {
                    FAUDIO_END_OF_STREAM, /* Flags */
                    wav_length * 8, /* AudioBytes */
                    wav_buffer, /* AudioData */
                    0, /* playbegin */
                    0, /* playlength */
                    0, /* LoopBegin */
                    0, /* LoopLength */
                    0, /* LoopCount */
                    NULL
                };
                if (FAudioSourceVoice_SubmitSourceBuffer(voices[i], &faudio_buffer, NULL))
                {
                    vlog_error("Unable to queue sound buffer");
                    return;
                }
                FAudioVoice_SetVolume(voices[i], volume, FAUDIO_COMMIT_NOW);
                if (FAudioSourceVoice_Start(voices[i], 0, FAUDIO_COMMIT_NOW))
                {
                    vlog_error("Unable to start voice processing");
                }
                return;
            }
        }
    }

    static void Init(int audio_rate)
    {
        if (voices == NULL)
        {
            voices = (FAudioSourceVoice**) SDL_malloc(sizeof(FAudioSourceVoice*) * VVV_MAX_CHANNELS);
            for (int i = 0; i < VVV_MAX_CHANNELS; i++)
            {
                FAudioWaveFormatEx format;
                format.nChannels = 1; /* Assume 1 for SoundTracks. Will be recreated if mismatched during play */
                format.nSamplesPerSec = audio_rate;
                format.wFormatTag = FAUDIO_FORMAT_PCM;
                format.wBitsPerSample = 16;
                format.nBlockAlign = format.nChannels * format.wBitsPerSample;
                format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
                format.cbSize = 0;
                if (FAudio_CreateSourceVoice(faudioctx, &voices[i], &format, 0, 2.0f, NULL, NULL, NULL))
                {
                    vlog_error("Unable to create source voice no. %i", i);
                    return;
                }
            }
        }
    }

    static void Pause(void)
    {
        for (size_t i = 0; i < VVV_MAX_CHANNELS; i++)
        {
            FAudioSourceVoice_Stop(voices[i], 0, FAUDIO_COMMIT_NOW);
        }
    }

    static void Resume(void)
    {
        for (size_t i = 0; i < VVV_MAX_CHANNELS; i++)
        {
            FAudioSourceVoice_Start(voices[i], 0, FAUDIO_COMMIT_NOW);
        }
    }

    static void Destroy(void)
    {
        if (voices != NULL)
        {
            for (int i = 0; i < VVV_MAX_CHANNELS; i++)
            {
                VVV_freefunc(FAudioVoice_DestroyVoice, voices[i]);
            }
            VVV_free(voices);
        }
    }

    static void SetVolume(int soundVolume)
    {
        volume = (float) soundVolume / VVV_MAX_VOLUME;
        for (size_t i = 0; i < VVV_MAX_CHANNELS; i++)
        {
            FAudioVoice_SetVolume(voices[i], volume, FAUDIO_COMMIT_NOW);
        }
    }

    Uint8 *wav_buffer;
    Uint32 wav_length;
    FAudioWaveFormatEx format;
    bool valid;

    static FAudioSourceVoice** voices;
    static float volume;
};
FAudioSourceVoice** SoundTrack::voices = NULL;
float SoundTrack::volume = 0.0f;

class MusicTrack
{
public:
    MusicTrack(SDL_RWops *rw)
    {
        SDL_zerop(this);
        read_buf = (Uint8*) SDL_malloc(rw->size(rw));
        SDL_RWread(rw, read_buf, rw->size(rw), 1);
        int err;
        stb_vorbis_info vorbis_info;
        stb_vorbis_comment vorbis_comment;
        vorbis = stb_vorbis_open_memory(read_buf, rw->size(rw), &err, NULL);
        if (vorbis == NULL)
        {
            vlog_error("Unable to create Vorbis handle, error %d", err);
            VVV_free(read_buf);
            goto end;
        }
        vorbis_info = stb_vorbis_get_info(vorbis);
        format.wFormatTag = FAUDIO_FORMAT_IEEE_FLOAT;
        format.wBitsPerSample = sizeof(float) * 8;
        format.nChannels = vorbis_info.channels;
        format.nSamplesPerSec = vorbis_info.sample_rate;
        format.nBlockAlign = format.nChannels * format.wBitsPerSample;
        format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
        format.cbSize = 0;

        channels = format.nChannels;
        size = format.nAvgBytesPerSec / 20;

        decoded_buf_playing = (Uint8*) SDL_malloc(size);
        decoded_buf_reserve = (Uint8*) SDL_malloc(size);

        loopbegin = 0;
        looplength = 0;
        vorbis_comment = stb_vorbis_get_comment(vorbis);
        parseComments(this, vorbis_comment.comment_list, vorbis_comment.comment_list_length);
        valid = true;

end:
        SDL_RWclose(rw);
    }

    void Dispose(void)
    {
        VVV_freefunc(stb_vorbis_close, vorbis);
        VVV_free(read_buf);
        VVV_free(decoded_buf_playing);
        VVV_free(decoded_buf_reserve);
        if (!IsHalted())
        {
            VVV_freefunc(FAudioVoice_DestroyVoice, musicVoice);
        }
    }

    bool Play(bool loop)
    {
        if (!valid)
        {
            return false;
        }

        shouldloop = loop;
        sample_pos = 0;
        stb_vorbis_seek_start(vorbis);

        if (!IsHalted())
        {
            FAudioVoiceDetails details;
            FAudioVoice_GetVoiceDetails(musicVoice, &details);
            if (details.InputSampleRate != format.nSamplesPerSec ||
            details.InputChannels != format.nChannels)
            {
                Halt();
            }
        }

        if (IsHalted())
        {
            SDL_zero(callbacks);
            callbacks.OnBufferStart = &MusicTrack::refillReserve;
            callbacks.OnBufferEnd = &MusicTrack::swapBuffers;
            FAudio_CreateSourceVoice(faudioctx, &musicVoice, &format, 0, 2.0f, &callbacks, NULL, NULL);
        }
        else
        {
            Pause();
            FAudioSourceVoice_FlushSourceBuffers(musicVoice);
        }

        FAudioBuffer faudio_buffer;
        SDL_zero(faudio_buffer);
        if (looplength == 0)
        {
            faudio_buffer.PlayLength = stb_vorbis_get_samples_float_interleaved(vorbis, channels, (float*) decoded_buf_playing, size / sizeof(float));
        }
        else
        {
            int samples_read = stb_vorbis_get_samples_float_interleaved(vorbis, channels, (float*) decoded_buf_playing, size / sizeof(float));
            faudio_buffer.PlayLength = SDL_min(samples_read, (loopbegin + looplength) - sample_pos);
        }
        faudio_buffer.AudioBytes = size;
        faudio_buffer.pAudioData = decoded_buf_playing;
        faudio_buffer.pContext = this;
        sample_pos += faudio_buffer.PlayLength;
        if (FAudioSourceVoice_SubmitSourceBuffer(musicVoice, &faudio_buffer, NULL))
        {
            vlog_error("Unable to queue sound buffer");
            return false;
        }
        Resume();
        return true;
    }

    static void Halt(void)
    {
        if (!IsHalted())
        {
            FAudioSourceVoice_FlushSourceBuffers(musicVoice);
            VVV_freefunc(FAudioVoice_DestroyVoice, musicVoice);
            paused = true;
        }
    }

    static bool IsHalted(void)
    {
        return musicVoice == NULL;
    }

    static void Pause(void)
    {
        if (!IsHalted())
        {
            FAudioSourceVoice_Stop(musicVoice, 0, FAUDIO_COMMIT_NOW);
            paused = true;
        }
    }

    static bool IsPaused(void)
    {
        return paused || IsHalted();
    }

    static void Resume(void)
    {
        if (!IsHalted())
        {
            FAudioSourceVoice_Start(musicVoice, 0, FAUDIO_COMMIT_NOW);
            paused = false;
        }
    }

    static void SetVolume(int musicVolume)
    {
        float adj_vol = (float) musicVolume / VVV_MAX_VOLUME;
        if (!IsHalted())
        {
            FAudioVoice_SetVolume(musicVoice, adj_vol, FAUDIO_COMMIT_NOW);
        }
    }

    stb_vorbis* vorbis;
    int channels;
    Uint32 size;
    int loopbegin;
    int looplength;
    int sample_pos; //stb_vorbis offset not yet functional on pulldata API. TODO Replace when fixed

    FAudioVoiceCallback callbacks;
    FAudioWaveFormatEx format;

    Uint8* decoded_buf_playing;
    Uint8* decoded_buf_reserve;
    Uint8* read_buf;
    bool shouldloop;
    bool valid;

    static bool paused;
    static FAudioSourceVoice* musicVoice;

    static void refillReserve(FAudioVoiceCallback* callback, void* ctx)
    {
        MusicTrack* t = (MusicTrack*) ctx;
        FAudioBuffer faudio_buffer;
        SDL_zero(faudio_buffer);
        UNUSED(callback);
        if (t->looplength == 0)
        {
            faudio_buffer.PlayLength = stb_vorbis_get_samples_float_interleaved(t->vorbis, t->channels, (float*) t->decoded_buf_reserve, t->size / sizeof(float));
        }
        else
        {
            int samples_read = stb_vorbis_get_samples_float_interleaved(t->vorbis, t->channels, (float*) t->decoded_buf_reserve, t->size / sizeof(float));
            faudio_buffer.PlayLength = SDL_min(samples_read, (t->loopbegin + t->looplength) - t->sample_pos);
        }
        faudio_buffer.AudioBytes = t->size;
        faudio_buffer.pAudioData = t->decoded_buf_reserve;
        faudio_buffer.pContext = t;
        if (faudio_buffer.PlayLength == 0)
        {
            if (t->shouldloop)
            {
                stb_vorbis_seek(t->vorbis, t->loopbegin);
                t->sample_pos = t->loopbegin;
                if (t->looplength != 0)
                {
                    int samples_read = stb_vorbis_get_samples_float_interleaved(t->vorbis, t->channels, (float*) t->decoded_buf_reserve, t->size / sizeof(float));
                    faudio_buffer.PlayLength = SDL_min(samples_read, (t->loopbegin + t->looplength) - t->sample_pos);
                }
                else
                {
                    faudio_buffer.PlayLength = stb_vorbis_get_samples_float_interleaved(t->vorbis, t->channels, (float*) t->decoded_buf_reserve, t->size / sizeof(float));
                }
                if (faudio_buffer.PlayLength == 0)
                {
                    return;
                }
            }
            else
            {
                return;
            }
        }
        t->sample_pos += faudio_buffer.PlayLength;
        FAudioSourceVoice_SubmitSourceBuffer(musicVoice, &faudio_buffer, NULL);
    }

    static void swapBuffers(FAudioVoiceCallback* callback, void* ctx)
    {
        MusicTrack* t = (MusicTrack*) ctx;
        Uint8* tmp = t->decoded_buf_playing;
        UNUSED(callback);
        t->decoded_buf_playing = t->decoded_buf_reserve;
        t->decoded_buf_reserve = tmp;
    }

    /* Lifted from SDL_mixer, we used it in 2.3 and previous */
    static void parseComments(
        MusicTrack* t, char** comments, const int comment_list_length
    ) {
        int loopend = 0;
        for (int i = 0; i < comment_list_length; i++)
        {
            char* param = SDL_strdup(comments[i]);
            if (param == NULL)
            {
                vlog_error(
                    "Could not allocate memory to parse '%s'. Ignoring comments.",
                    comments[i]
                );
                break;
            }
            char* argument = param;
            char* value = SDL_strchr(param, '=');
            if (value == NULL)
            {
                value = param + SDL_strlen(param);
            }
            else
            {
                *(value++) = '\0';
            }

            /* Want to match LOOP-START, LOOP_START, etc. Remove - or _ from
            * string if it is present at position 4. */
            char buf[5];
            SDL_strlcpy(buf, argument, sizeof(buf));
            if (SDL_strcasecmp(buf, "LOOP") == 0
            && ((argument[4] == '_') || (argument[4] == '-')))
            {
                SDL_memmove(argument + 4, argument + 5, SDL_strlen(argument) - 4);
            }

            if (SDL_strcasecmp(argument, "LOOPSTART") == 0)
            {
                t->loopbegin = _Mix_ParseTime(value, t->format.nSamplesPerSec);
            }
            else if (SDL_strcasecmp(argument, "LOOPLENGTH") == 0)
            {
                t->looplength = SDL_strtoll(value, NULL, 10);
            }
            else if (SDL_strcasecmp(argument, "LOOPEND") == 0)
            {
                loopend = _Mix_ParseTime(value, t->format.nSamplesPerSec);
            }

            if (t->loopbegin < 0 || t->looplength < 0 || loopend < 0)
            {
                vlog_warn(
                    "A track loop comment had a negative value. "
                    "Ignoring all comments for the track."
                );
                t->loopbegin = 0;
                t->looplength = 0;
                loopend = 0;
                VVV_free(param);
                break;
            }

            VVV_free(param);
        }
        if (loopend != 0)
        {
            t->looplength = loopend - t->loopbegin;
        }
    }

    static int _Mix_ParseTime(char* time, const long samplerate_hz)
    {
        char* num_start = time;
        char* p;
        Sint64 result = 0;
        int val;

        /* Time is directly expressed as a sample position */
        if (SDL_strchr(time, ':') == NULL)
        {
            return SDL_strtoll(time, NULL, 10);
        }

        for (p = time; *p != '\0'; ++p)
        {
            if (*p == '.' || *p == ':')
            {
                const char c = *p;
                *p = '\0';
                val = SDL_atoi(num_start);
                if (val < 0)
                {
                    return -1;
                }
                result = result * 60 + val;
                num_start = p + 1;
                *p = c;
            }

            if (*p == '.')
            {
                const double val_f = SDL_atof(p);
                if (val_f < 0)
                {
                    return -1;
                }
                return result * samplerate_hz + (Sint64) (val_f * samplerate_hz);
            }
        }

        val = SDL_atoi(num_start);
        if (val < 0)
        {
            return -1;
        }
        return (result * 60 + val) * samplerate_hz;
    }
};
bool MusicTrack::paused = false;
FAudioSourceVoice* MusicTrack::musicVoice = NULL;

musicclass::musicclass(void)
{
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
    if (FAudioCreate(&faudioctx, 0, FAUDIO_DEFAULT_PROCESSOR))
    {
        vlog_error("Unable to initialize FAudio");
        return;
    }
    if (FAudio_CreateMasteringVoice(faudioctx, &masteringvoice, 2, 44100, 0, 0, NULL))
    {
        vlog_error("Unable to create mastering voice");
        return;
    }

    SoundTrack::Init(44100);

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

#define TRACK_LOAD_BLOB(blob, track_name) \
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
            musicTracks.push_back(MusicTrack(rw)); \
        } \
    }

#define FOREACH_TRACK(blob, track_name) TRACK_LOAD_BLOB(blob, track_name)

            TRACK_NAMES(pppppp_blob)

#undef FOREACH_TRACK
        }
        else
        {
            vlog_info("Loading music from loose files...");

            SDL_RWops* rw;

#define FOREACH_TRACK(_, track_name) \
    rw = PHYSFSRWOPS_openRead(track_name); \
    if (rw == NULL) \
    { \
        vlog_error("Unable to read loose music file: %s", SDL_GetError()); \
    } \
    else \
    { \
        musicTracks.push_back(MusicTrack(rw)); \
    }

            TRACK_NAMES(_)

#undef FOREACH_TRACK
        }
    }
    else
    {
        vlog_info("Loading PPPPPP and MMMMMM blobs...");

        mmmmmm = true;
        int index;
        SDL_RWops* rw;

#define FOREACH_TRACK(blob, track_name) TRACK_LOAD_BLOB(blob, track_name)

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
#undef TRACK_LOAD_BLOB
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
    SoundTrack::Destroy();

    for (size_t i = 0; i < musicTracks.size(); ++i)
    {
        musicTracks[i].Dispose();
    }
    musicTracks.clear();

    pppppp_blob.clear();
    mmmmmm_blob.clear();
    VVV_freefunc(FAudioVoice_DestroyVoice, masteringvoice);
    VVV_freefunc(FAudio_Release, faudioctx);
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

void musicclass::resume(void)
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
    return MusicTrack::IsPaused();
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
