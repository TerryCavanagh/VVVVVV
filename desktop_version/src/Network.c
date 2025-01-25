#include <stdint.h>
#include <stdio.h>

#include "MakeAndPlay.h"
#include "Unused.h"

#ifdef MAKEANDPLAY
    #ifdef STEAM_NETWORK
        #undef STEAM_NETWORK
    #endif
    #ifdef GOG_NETWORK
        #undef GOG_NETWORK
    #endif
    #ifdef DISCORD_NETWORK
        #undef DISCORD_NETWORK
    #endif
#endif

#ifdef STEAM_NETWORK
#define STEAM_NUM 1
#else
#define STEAM_NUM 0
#endif
#ifdef GOG_NETWORK
#define GOG_NUM 1
#else
#define GOG_NUM 0
#endif
#ifdef DISCORD_NETWORK
#define DISCORD_NUM 1
#else
#define DISCORD_NUM 0
#endif

#define NUM_BACKENDS (STEAM_NUM+GOG_NUM+DISCORD_NUM)
#define DECLARE_BACKEND(name) \
    int32_t name##_init(void); \
    void name##_shutdown(void); \
    int32_t name##_update(void); \
    void name##_unlockAchievement(const char *name); \
    void name##_setRPC(const char *area, const char *roomname);
#ifdef STEAM_NETWORK
DECLARE_BACKEND(STEAM)
#endif
#ifdef GOG_NETWORK
DECLARE_BACKEND(GOG)
#endif
#ifdef DISCORD_NETWORK
DECLARE_BACKEND(DISCORD)
#endif
#undef DECLARE_BACKEND

typedef struct NetworkBackend
{
    int32_t IsInit;
    int32_t (*Init)(void);
    void (*Shutdown)(void);
    int32_t (*Update)(void);
    void (*UnlockAchievement)(const char*);
    void (*SetRPC)(const char*, const char*);
} NetworkBackend;

#if NUM_BACKENDS > 0
static NetworkBackend backends[NUM_BACKENDS];
#endif

int NETWORK_init(void)
{
    int32_t i, any = 0;
    #define ASSIGN_BACKEND(name, index) \
        backends[index].Init = name##_init; \
        backends[index].Shutdown = name##_shutdown; \
        backends[index].Update = name##_update; \
        backends[index].UnlockAchievement = name##_unlockAchievement; \
        backends[index].SetRPC = name##_setRPC;

    #ifdef STEAM_NETWORK
    ASSIGN_BACKEND(STEAM, 0)
    #endif
    #ifdef GOG_NETWORK
    ASSIGN_BACKEND(GOG, STEAM_NUM)
    #endif
    #ifdef DISCORD_NETWORK
    ASSIGN_BACKEND(DISCORD, STEAM_NUM+GOG_NUM)
    #endif
    #undef ASSIGN_BACKEND
    #if NUM_BACKENDS > 0
    for (i = 0; i < NUM_BACKENDS; i += 1)
    {
        backends[i].IsInit = backends[i].Init();
        any |= backends[i].IsInit;
    }
    #else
    UNUSED(i);
    #endif
    return any;
}

void NETWORK_shutdown(void)
{
    #if NUM_BACKENDS > 0
    int32_t i;
    for (i = 0; i < NUM_BACKENDS; i += 1)
    if (backends[i].IsInit)
    {
        backends[i].Shutdown();
    }
    #endif
}


int32_t NETWORK_update(void)
{
    int32_t result = 0;
    #if NUM_BACKENDS > 0
    int32_t i;
    for (i = 0; i < NUM_BACKENDS; i += 1)
    if (backends[i].IsInit)
    {
        if ( backends[i].Update() )
        {
            result |= (1 << i);
        }
    }
    #endif
    return result;
}

void NETWORK_unlockAchievement(const char *name)
{
    #if NUM_BACKENDS > 0
    int32_t i;
    for (i = 0; i < NUM_BACKENDS; i += 1)
    if (backends[i].IsInit)
    {
        backends[i].UnlockAchievement(name);
    }
    #else
    UNUSED(name);
    #endif
}

void NETWORK_setRPC(const char *area, const char *roomname)
{
    #if NUM_BACKENDS > 0
    int32_t i;
    for (i = 0; i < NUM_BACKENDS; i += 1)
    if (backends[i].IsInit)
    {
        backends[i].SetRPC(area, roomname);
    }
    #endif
}

