#include <stdio.h>
#include <stdint.h>
#include <SDL.h>

/* Steamworks interface versions */

#define VVVVVV_STEAMCLIENT "SteamClient017"
#define VVVVVV_STEAMUSERSTATS "STEAMUSERSTATS_INTERFACE_VERSION011"

/* Shared object file name */

#if defined(_WIN32)
#define STEAM_LIBRARY "steam_api.dll"
#elif defined(__APPLE__)
#define STEAM_LIBRARY "libsteam_api.dylib"
#elif defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__HAIKU__)
#define STEAM_LIBRARY "libsteam_api.so"
#else
#error STEAM_LIBRARY: Unrecognized platform!
#endif

/* Function Pointer Types */

typedef uint8_t (*SteamAPI_InitFunc)();
typedef void (*SteamAPI_ShutdownFunc)();
typedef void (*SteamAPI_RunCallbacksFunc)();
typedef intptr_t (*SteamInternal_CreateInterfaceFunc)(const char*);
typedef int32_t (*SteamAPI_GetHSteamUserFunc)();
typedef int32_t (*SteamAPI_GetHSteamPipeFunc)();
typedef intptr_t (*SteamAPI_ISteamClient_GetISteamUserStatsFunc)(
	intptr_t,
	int32_t,
	int32_t,
	const char*
);
typedef uint8_t (*SteamAPI_ISteamUserStats_RequestCurrentStatsFunc)(intptr_t);
typedef uint8_t (*SteamAPI_ISteamUserStats_StoreStatsFunc)(intptr_t);
typedef uint8_t (*SteamAPI_ISteamUserStats_GetStatFunc)(
	intptr_t,
	const char*,
	int32_t*
);
typedef uint8_t (*SteamAPI_ISteamUserStats_SetStatFunc)(
	intptr_t,
	const char*,
	int32_t
);
typedef uint8_t (*SteamAPI_ISteamUserStats_SetAchievementFunc)(
	intptr_t,
	const char*
);

/* DLL, Entry Points */

static void *libHandle = NULL;
static intptr_t steamUserStats = (intptr_t) NULL;

#define DEFINE_FUNC(name) static name##Func name = NULL;
DEFINE_FUNC(SteamAPI_Init)
DEFINE_FUNC(SteamAPI_Shutdown)
DEFINE_FUNC(SteamAPI_RunCallbacks)
DEFINE_FUNC(SteamInternal_CreateInterface)
DEFINE_FUNC(SteamAPI_GetHSteamUser)
DEFINE_FUNC(SteamAPI_GetHSteamPipe)
DEFINE_FUNC(SteamAPI_ISteamClient_GetISteamUserStats)
DEFINE_FUNC(SteamAPI_ISteamUserStats_RequestCurrentStats)
DEFINE_FUNC(SteamAPI_ISteamUserStats_StoreStats)
DEFINE_FUNC(SteamAPI_ISteamUserStats_GetStat)
DEFINE_FUNC(SteamAPI_ISteamUserStats_SetStat)
DEFINE_FUNC(SteamAPI_ISteamUserStats_SetAchievement)
#undef DEFINE_FUNC

/* Clean up after ourselves... */

static void ClearPointers()
{
	SDL_UnloadObject(libHandle);
	libHandle = NULL;
	steamUserStats = (intptr_t) NULL;
	SteamAPI_Init = NULL;
	SteamAPI_Shutdown = NULL;
	SteamAPI_RunCallbacks = NULL;
	SteamInternal_CreateInterface = NULL;
	SteamAPI_GetHSteamUser = NULL;
	SteamAPI_GetHSteamPipe = NULL;
	SteamAPI_ISteamClient_GetISteamUserStats = NULL;
	SteamAPI_ISteamUserStats_RequestCurrentStats = NULL;
	SteamAPI_ISteamUserStats_StoreStats = NULL;
	SteamAPI_ISteamUserStats_GetStat = NULL;
	SteamAPI_ISteamUserStats_SetStat = NULL;
	SteamAPI_ISteamUserStats_SetAchievement = NULL;
}

/* NETWORK API Implementation */

int32_t STEAM_init()
{
#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__HAIKU__)
	return 0;
#endif
	intptr_t steamClient;
	int32_t steamUser, steamPipe;
	union typepun 
	{
		void (*funcname)(void);
		void* dataptr;
	};

	union typepun punner;

	libHandle = SDL_LoadObject(STEAM_LIBRARY);
	if (!libHandle)
	{
		printf("%s not found!\n", STEAM_LIBRARY);
		return 0;
	}


	#define LOAD_FUNC(name) \
		punner.dataptr = SDL_LoadFunction(libHandle, #name); \
		name = (name##Func) punner.funcname; \
		if (!name) \
		{ \
			printf("%s symbol %s not found!\n", STEAM_LIBRARY, #name); \
			ClearPointers(); \
			return 0; \
		}
	LOAD_FUNC(SteamAPI_Init)
	LOAD_FUNC(SteamAPI_Shutdown)
	LOAD_FUNC(SteamAPI_RunCallbacks)
	LOAD_FUNC(SteamInternal_CreateInterface)
	LOAD_FUNC(SteamAPI_GetHSteamUser)
	LOAD_FUNC(SteamAPI_GetHSteamPipe)
	LOAD_FUNC(SteamAPI_ISteamClient_GetISteamUserStats)
	LOAD_FUNC(SteamAPI_ISteamUserStats_RequestCurrentStats)
	LOAD_FUNC(SteamAPI_ISteamUserStats_StoreStats)
	LOAD_FUNC(SteamAPI_ISteamUserStats_GetStat)
	LOAD_FUNC(SteamAPI_ISteamUserStats_SetStat)
	LOAD_FUNC(SteamAPI_ISteamUserStats_SetAchievement)
	#undef LOAD_FUNC

	if (!SteamAPI_Init())
	{
		printf("Steamworks not initialized!\n");
		ClearPointers();
		return 0;
	}
	steamClient = SteamInternal_CreateInterface(VVVVVV_STEAMCLIENT);
	steamUser = SteamAPI_GetHSteamUser();
	steamPipe = SteamAPI_GetHSteamPipe();
	if (!steamClient || !steamUser || !steamPipe)
	{
		SteamAPI_Shutdown();
		printf(VVVVVV_STEAMCLIENT " not created!\n");
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
		printf(VVVVVV_STEAMUSERSTATS " not created!\n");
		ClearPointers();
		return 0;
	}
	SteamAPI_ISteamUserStats_RequestCurrentStats(steamUserStats);
	return 1;
}

void STEAM_shutdown()
{
	if (libHandle)
	{
		SteamAPI_Shutdown();
		ClearPointers();
	}
}

void STEAM_update()
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

int32_t STEAM_getAchievementProgress(const char *name)
{
	int32_t result = -1;
	if (libHandle)
	{
		SteamAPI_ISteamUserStats_GetStat(
			steamUserStats,
			name,
			&result
		);
	}
	return result;
}

void STEAM_setAchievementProgress(const char *name, int32_t stat)
{
	if (libHandle)
	{
		SteamAPI_ISteamUserStats_SetStat(
			steamUserStats,
			name,
			stat
		);
		SteamAPI_ISteamUserStats_StoreStats(steamUserStats);
	}
}
