#include "MakeAndPlay.h"

#ifndef MAKEANDPLAY

#include <stdint.h>
#include <SDL.h>

#include "Vlogging.h"

/* Steamworks interface versions */

#define VVVVVV_STEAMCLIENT "SteamClient017"
#define VVVVVV_STEAMUSERSTATS "STEAMUSERSTATS_INTERFACE_VERSION011"

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
    ))

static void *libHandle = NULL;
static struct ISteamUserStats *steamUserStats = NULL;

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

/* NETWORK API Implementation */

int32_t STEAM_init(void)
{
#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__HAIKU__) || defined(__DragonFly__)
    return 0;
#endif
    struct ISteamClient *steamClient;
    int32_t steamUser, steamPipe;

    libHandle = SDL_LoadObject(STEAM_LIBRARY);
    if (!libHandle)
    {
        vlog_info(STEAM_LIBRARY " not found!");
        return 0;
    }

#define FOREACH_FUNC(rettype, name, params) \
    name = (rettype (*) params) SDL_LoadFunction(libHandle, #name); \
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
    if (libHandle)
    {
        SteamAPI_RunCallbacks();
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
