#include "MakeAndPlay.h"

#ifndef MAKEANDPLAY

#include <stdint.h>
#include <SDL.h>

#include "CWrappers.h"
#include "Vlogging.h"

/* Steamworks interface versions */

#define VVVVVV_STEAMCLIENT "SteamClient017"
#define VVVVVV_STEAMUSERSTATS "STEAMUSERSTATS_INTERFACE_VERSION011"
#define VVVVVV_STEAMSCREENSHOTS "STEAMSCREENSHOTS_INTERFACE_VERSION003"

/* Shared object file name */

#if defined(_WIN32)
#define STEAM_LIBRARY "steam_api.dll"
#elif defined(__APPLE__)
#define STEAM_LIBRARY "libsteam_api.dylib"
#elif defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__HAIKU__) || defined(__DragonFly__)
#define STEAM_LIBRARY "libsteam_api.so"
#else
#error STEAM_LIBRARY: Unrecognized platform!
#endif

/* DLL, Entry Points */

struct ISteamClient;
struct ISteamUserStats;
struct ISteamScreenshots;
struct CallbackMsg_t
{
    int32_t m_hSteamUser;
    int32_t m_iCallback;
    uint8_t* m_pubParam;
    int32_t m_cubParam;
};
struct SteamAPICallCompleted_t
{
    uint64_t m_hAsyncCall;
    int32_t m_iCallback;
    uint32_t m_cubParam;
};

#define FUNC_LIST \
    FOREACH_FUNC(uint8_t, SteamAPI_Init, (void)) \
    FOREACH_FUNC(void, SteamAPI_Shutdown, (void)) \
    FOREACH_FUNC(void, SteamAPI_RunCallbacks, (void)) \
    FOREACH_FUNC(struct ISteamClient*, SteamInternal_CreateInterface, (const char*)) \
    FOREACH_FUNC(int32_t, SteamAPI_GetHSteamUser, (void)) \
    FOREACH_FUNC(int32_t, SteamAPI_GetHSteamPipe, (void)) \
    FOREACH_FUNC(struct ISteamUserStats*, SteamAPI_ISteamClient_GetISteamUserStats, ( \
        struct ISteamClient*, \
        int32_t, \
        int32_t, \
        const char* \
    )) \
    FOREACH_FUNC(uint8_t, SteamAPI_ISteamUserStats_RequestCurrentStats, (struct ISteamUserStats*)) \
    FOREACH_FUNC(uint8_t, SteamAPI_ISteamUserStats_StoreStats, (struct ISteamUserStats*)) \
    FOREACH_FUNC(uint8_t, SteamAPI_ISteamUserStats_SetAchievement, ( \
        struct ISteamUserStats*, \
        const char* \
    )) \
    FOREACH_FUNC(struct ISteamScreenshots*, SteamAPI_ISteamClient_GetISteamScreenshots, ( \
        struct ISteamClient*, \
        int32_t, \
        int32_t, \
        const char* \
    )) \
    FOREACH_FUNC(void, SteamAPI_ISteamScreenshots_HookScreenshots, (\
        struct ISteamScreenshots*, \
        uint8_t \
    )) \
    FOREACH_FUNC(uint32_t, SteamAPI_ISteamScreenshots_WriteScreenshot, ( \
        struct ISteamScreenshots*, \
        void*, \
        uint32_t, \
        int32_t, \
        int32_t \
    )) \
    FOREACH_FUNC(void, SteamAPI_ManualDispatch_Init, (void)) \
    FOREACH_FUNC(void, SteamAPI_ManualDispatch_RunFrame, (int32_t)) \
    FOREACH_FUNC(uint8_t, SteamAPI_ManualDispatch_GetNextCallback, (int32_t, struct CallbackMsg_t*)) \
    FOREACH_FUNC(void, SteamAPI_ManualDispatch_FreeLastCallback, (int32_t)) \
    FOREACH_FUNC(uint8_t, SteamAPI_ManualDispatch_GetAPICallResult, ( \
        int32_t, \
        uint64_t, \
        void*, \
        int32_t, \
        int32_t, \
        uint8_t* \
    ))

#define iScreenshotRequested 2302

static void* libHandle = NULL;
static struct ISteamUserStats* steamUserStats = NULL;
static struct ISteamScreenshots* steamScreenshots = NULL;

#define FOREACH_FUNC(rettype, name, params) static rettype (*name) params = NULL;
FUNC_LIST
#undef FOREACH_FUNC

/* Clean up after ourselves... */

static void ClearPointers(void)
{
    SDL_UnloadObject(libHandle);
    libHandle = NULL;
    steamUserStats = NULL;
#define FOREACH_FUNC(rettype, name, params) name = NULL;
    FUNC_LIST
#undef FOREACH_FUNC
}

static void run_screenshot()
{
    if (!libHandle)
    {
        return;
    }

    vlog_info("taking a screenshot");

    SDL_Surface* surface = GRAPHICS_tempScreenshot();
    uint8_t success = UTIL_TakeScreenshot(&surface);
    if (!success)
    {
        return;
    }
    SDL_Surface* surface2x = GRAPHICS_tempScreenshot2x();
    success = UTIL_UpscaleScreenshot2x(surface, &surface2x);
    if (!success)
    {
        return;
    }

    SteamAPI_ISteamScreenshots_WriteScreenshot(
        steamScreenshots,
        surface2x->pixels,
        surface2x->w * surface2x->h * surface2x->format->BytesPerPixel,
        surface2x->w,
        surface2x->h
    );
}

/* NETWORK API Implementation */

static int32_t steamPipe = 0;

int32_t STEAM_init(void)
{
#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__HAIKU__) || defined(__DragonFly__)
    return 0;
#endif
    struct ISteamClient *steamClient;
    int32_t steamUser;

    libHandle = SDL_LoadObject(STEAM_LIBRARY);
    if (!libHandle)
    {
        vlog_info(STEAM_LIBRARY " not found!");
        return 0;
    }

#define FOREACH_FUNC(rettype, name, params) \
    name = (rettype (*) params) (intptr_t) SDL_LoadFunction(libHandle, #name); \
    if (!name) \
    { \
        vlog_error(STEAM_LIBRARY " symbol " #name " not found!"); \
        ClearPointers(); \
        return 0; \
    }
    FUNC_LIST
#undef FOREACH_FUNC

    if (!SteamAPI_Init())
    {
        vlog_error("Steamworks not initialized!");
        ClearPointers();
        return 0;
    }
    SteamAPI_ManualDispatch_Init();
    steamClient = SteamInternal_CreateInterface(VVVVVV_STEAMCLIENT);
    steamUser = SteamAPI_GetHSteamUser();
    steamPipe = SteamAPI_GetHSteamPipe();
    if (!steamClient || !steamUser || !steamPipe)
    {
        SteamAPI_Shutdown();
        vlog_error(VVVVVV_STEAMCLIENT " not created!");
        ClearPointers();
        return 0;
    }
    steamUserStats = SteamAPI_ISteamClient_GetISteamUserStats(
        steamClient,
        steamUser,
        steamPipe,
        VVVVVV_STEAMUSERSTATS
    );
    if (!steamUserStats)
    {
        SteamAPI_Shutdown();
        vlog_error(VVVVVV_STEAMUSERSTATS " not created!");
        ClearPointers();
        return 0;
    }
    SteamAPI_ISteamUserStats_RequestCurrentStats(steamUserStats);
    steamScreenshots = SteamAPI_ISteamClient_GetISteamScreenshots(
        steamClient,
        steamUser,
        steamPipe,
        VVVVVV_STEAMSCREENSHOTS
    );
    if (!steamScreenshots)
    {
        SteamAPI_Shutdown();
        vlog_error(VVVVVV_STEAMSCREENSHOTS " not created!");
        ClearPointers();
        return 0;
    }
    SteamAPI_ISteamScreenshots_HookScreenshots(steamScreenshots, 1);
    return 1;
}

void STEAM_shutdown(void)
{
    if (libHandle)
    {
        SteamAPI_Shutdown();
        ClearPointers();
    }
}

void STEAM_update(void)
{
    if (!libHandle)
    {
        return;
    }

    SteamAPI_ManualDispatch_RunFrame(steamPipe);
    struct CallbackMsg_t callback;
    SDL_zero(callback);
    while (SteamAPI_ManualDispatch_GetNextCallback(steamPipe, &callback))
    {
        if (callback.m_iCallback == iScreenshotRequested)
        {
            run_screenshot();
        }
        SteamAPI_ManualDispatch_FreeLastCallback(steamPipe);
    }
}

void STEAM_unlockAchievement(const char *name)
{
    if (libHandle)
    {
        SteamAPI_ISteamUserStats_SetAchievement(
            steamUserStats,
            name
        );
        SteamAPI_ISteamUserStats_StoreStats(steamUserStats);
    }
}

#endif /* MAKEANDPLAY */
