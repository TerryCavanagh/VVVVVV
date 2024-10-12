#include <libgen.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include "SDL_libretro.h"

static void fallback_log(enum retro_log_level level, const char* fmt, ...);
static retro_log_printf_t log_cb = fallback_log;
static retro_environment_t environ_cb;
static retro_video_refresh_t video_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;
static retro_audio_sample_batch_t audio_batch_cb;

static SDL_Thread* game_thread;


static void
fallback_log(enum retro_log_level level, const char* fmt, ...)
{
    (void)level;
    va_list va;
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
}

unsigned
retro_api_version(void)
{
    return RETRO_API_VERSION;
}

void
retro_set_environment(retro_environment_t cb)
{
    static struct retro_log_callback log;

    environ_cb = cb;
    if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log))
        log_cb = log.log;
}

void
retro_set_video_refresh(retro_video_refresh_t cb)
{
    video_cb = cb;
}

void
retro_set_audio_sample(retro_audio_sample_t cb)
{
}

void
retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
    audio_batch_cb = cb;
}

void
retro_set_input_poll(retro_input_poll_t cb)
{
    input_poll_cb = cb;
}

void
retro_set_input_state(retro_input_state_t cb)
{
    input_state_cb = cb;
}

void
retro_get_system_info(struct retro_system_info* info)
{
    info->need_fullpath = true;
    info->valid_extensions = "zip";
    info->library_version = "0.1";
    info->library_name = "vvvvvv";
    info->block_extract = true;
}

void
retro_get_system_av_info(struct retro_system_av_info* info)
{
    int width = 320;
    int height = 240;
    info->geometry.base_width = width;
    info->geometry.base_height = height;
    info->geometry.max_width = width;
    info->geometry.max_height = height;
    info->geometry.aspect_ratio = 0.0;
    info->timing.fps = 60.0;
    info->timing.sample_rate = 44100.0;
}

void
retro_init(void)
{
    bool yes = true;
    enum retro_pixel_format pixfmt = RETRO_PIXEL_FORMAT_XRGB8888;
    environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &pixfmt);
    environ_cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &yes);
}


int main(int argc, char *argv[]);

static int
game_main(void* data)
{
    char buf[1024];
    char *pwd = SDL_strdup(getcwd(buf, 1024));
    char *argv[] = {
        SDL_strdup(""),
        SDL_strdup("-basedir"),
        SDL_strdup(pwd),
        SDL_strdup("-langdir"),
        SDL_strdup((SDL_snprintf(buf, 1024, "%s/lang", pwd), buf)),
        SDL_strdup("-fontsdir"),
        SDL_strdup((SDL_snprintf(buf, 1024, "%s/fonts", pwd), buf)),
        SDL_strdup("-assets"),
        SDL_strdup((SDL_snprintf(buf, 1024, "%s/data.zip", pwd), buf)),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    return main(argc, argv);
}

bool
retro_load_game(const struct retro_game_info* game)
{
    char *systemdir;
    environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &systemdir);
    if (game == NULL) {
        chdir(systemdir);
        chdir("vvvvvv");
    } else {
        chdir(dirname(SDL_strdup(game->path)));
    }

    game_thread = SDL_CreateThread(game_main, "VVVVVV", NULL);

    if (game_thread == NULL)
        return false;

    struct retro_keyboard_callback keyboard = {
        .callback = SDL_libretro_KeyboardCallback,
    };
    environ_cb(RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK, &keyboard);

    return true;
}

void
retro_set_controller_port_device(unsigned port, unsigned device)
{
}

void
retro_deinit(void)
{
}

void
retro_reset(void)
{
}

static void
PumpJoypadEvents(void)
{
    static int16_t buttons[16] = { 0 };
    static const int bkeys[16] = {
        [RETRO_DEVICE_ID_JOYPAD_B] = SDLK_v,
        [RETRO_DEVICE_ID_JOYPAD_Y] = SDLK_v,
        [RETRO_DEVICE_ID_JOYPAD_SELECT] = SDLK_ESCAPE,
        [RETRO_DEVICE_ID_JOYPAD_START] = SDLK_RETURN,
        [RETRO_DEVICE_ID_JOYPAD_UP] = SDLK_UP,
        [RETRO_DEVICE_ID_JOYPAD_DOWN] = SDLK_DOWN,
        [RETRO_DEVICE_ID_JOYPAD_LEFT] = SDLK_LEFT,
        [RETRO_DEVICE_ID_JOYPAD_RIGHT] = SDLK_RIGHT,
        [RETRO_DEVICE_ID_JOYPAD_A] = SDLK_v,
        [RETRO_DEVICE_ID_JOYPAD_X] = SDLK_v,
        [RETRO_DEVICE_ID_JOYPAD_L] = SDLK_BACKSPACE,
        [RETRO_DEVICE_ID_JOYPAD_R] = SDLK_RETURN,
        [RETRO_DEVICE_ID_JOYPAD_L2] = SDLK_v,
        [RETRO_DEVICE_ID_JOYPAD_R2] = SDLK_v,
        [RETRO_DEVICE_ID_JOYPAD_L3] = SDLK_v,
        [RETRO_DEVICE_ID_JOYPAD_R3] = SDLK_v,
    };
    for (int i = 0; i < 16; ++i) {
        int16_t state = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i);
        int k = bkeys[i];
        if (buttons[i] != state) {
            buttons[i] = state;
            SDL_SendKeyboardKey(state ? SDL_PRESSED : SDL_RELEASED,
                                SDL_GetScancodeFromKey(k));
        }
    }
}

void
retro_run(void)
{
    input_poll_cb();
    PumpJoypadEvents();
    SDL_libretro_RefreshVideo(video_cb);
    SDL_libretro_ProduceAudio(audio_batch_cb);
}

size_t
retro_serialize_size(void)
{
    return 0;
}

bool
retro_serialize(void* data, size_t size)
{
    return false;
}

bool
retro_unserialize(const void* data, size_t size)
{
    return false;
}

void
retro_cheat_reset(void)
{
}
void
retro_cheat_set(unsigned index, bool enabled, const char* code)
{
}

bool
retro_load_game_special(unsigned game_type,
                        const struct retro_game_info* info,
                        size_t num_info)
{
    return false;
}

void
retro_unload_game(void)
{
    SDL_Event event = { SDL_QUIT };
    SDL_PushEvent(&event);
    SDL_WaitThread(game_thread, NULL);
}

unsigned
retro_get_region(void)
{
    return RETRO_REGION_NTSC;
}

void*
retro_get_memory_data(unsigned id)
{
    return NULL;
}

size_t
retro_get_memory_size(unsigned id)
{
    return 0;
}
