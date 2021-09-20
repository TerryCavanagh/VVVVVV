#include <stdint.h>

#include "MakeAndPlay.h"
#include "Unused.h"

#ifdef MAKEANDPLAY
    #ifdef STEAM_NETWORK
        #undef STEAM_NETWORK
    #endif
    #ifdef GOG_NETWORK
        #undef GOG_NETWORK
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

#define NUM_BACKENDS (STEAM_NUM+GOG_NUM)
#define DECLARE_BACKEND(name) \
    int32_t name##_init(void); \
    void name##_shutdown(void); \
    void name##_update(void); \
    void name##_unlockAchievement(const char *name);
#ifdef STEAM_NETWORK
DECLARE_BACKEND(STEAM)
#endif
#ifdef GOG_NETWORK
DECLARE_BACKEND(GOG)
#endif
#undef DECLARE_BACKEND

typedef struct NetworkBackend
{
    int32_t IsInit;
    int32_t (*Init)(void);
    void (*Shutdown)(void);
    void (*Update)(void);
    void (*UnlockAchievement)(const char*);
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
        backends[index].UnlockAchievement = name##_unlockAchievement;
    #ifdef STEAM_NETWORK
    ASSIGN_BACKEND(STEAM, 0)
    #endif
    #ifdef GOG_NETWORK
    ASSIGN_BACKEND(GOG, STEAM_NUM)
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

void NETWORK_update(void)
{
    #if NUM_BACKENDS > 0
    int32_t i;
    for (i = 0; i < NUM_BACKENDS; i += 1)
    if (backends[i].IsInit)
    {
        backends[i].Update();
    }
    #endif
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
