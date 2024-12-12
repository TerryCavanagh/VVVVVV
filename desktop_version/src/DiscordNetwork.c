#include "MakeAndPlay.h"

#ifndef MAKEANDPLAY

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <string.h>
#endif

// Library includes
#include "discord_game_sdk.h"
#include <SDL.h>
#if defined(_WIN32)
#define DISCORD_LIBRARY "discord_game_sdk.dll"
#elif defined(__APPLE__)
#define DISCORD_LIBRARY "libdiscord_game_sdk.dylib"
#elif defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__HAIKU__) || defined(__DragonFly__)
#define DISCORD_LIBRARY "libdiscord_game_sdk.so"
#else
#error DISCORD_LIBRARY: Unrecognized platform!
#endif

#include "Vlogging.h"


#define DISCORD_REQUIRE(x) assert(x == DiscordResult_Ok)

struct Application {
    struct IDiscordCore* core;
    struct IDiscordActivityManager* activityMan;
};

struct DiscordActivity activity;
struct Application app;

static void* libHandle = NULL;


#define FUNC_LIST \
    FOREACH_FUNC(enum EDiscordResult, DiscordCreate, (DiscordVersion, struct DiscordCreateParams*, struct IDiscordCore**))



#define FOREACH_FUNC(rettype, name, params) static rettype (*name) params = NULL;
FUNC_LIST
#undef FOREACH_FUNC

static void ClearPointers(void)
{
    SDL_UnloadObject(libHandle);
    libHandle = NULL;
#define FOREACH_FUNC(rettype, name, params) name = NULL;
    FUNC_LIST
#undef FOREACH_FUNC
}


int32_t DISCORD_init(void)
{
#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__HAIKU__) || defined(__DragonFly__)
    return 0;
#endif
    libHandle = SDL_LoadObject(DISCORD_LIBRARY);
    if (!libHandle)
    {
        vlog_info(DISCORD_LIBRARY " not found!");
        printf("Can't load object %s : %s\n", DISCORD_LIBRARY, SDL_GetError());
        return 0;
    }

#define FOREACH_FUNC(rettype, name, params) \
    name = (rettype (*) params) (intptr_t) SDL_LoadFunction(libHandle, #name); \
    if (!name) \
    { \
        vlog_error(DISCORD_LIBRARY " symbol " #name " not found!"); \
        ClearPointers(); \
        return 0; \
    }
    FUNC_LIST
#undef FOREACH_FUNC
    memset(&app, 0, sizeof(app));

    struct DiscordCreateParams params;
    params.client_id = 1315544357532729447;
    params.flags = DiscordCreateFlags_Default;

    if(!DiscordCreate(DISCORD_VERSION, &params, &app.core))
    {
        return 1;
    }


    strncpy(activity.state, "On the Main Menu", 128);
    //strncpy(activity.details, "I'm Sorry", 128);
    strncpy(activity.assets.large_image, "vvvvvv", 128);
    strncpy(activity.assets.large_text, "Dimension  VVVVVV", 128);

    app.activityMan = app.core->get_activity_manager(app.core);
    app.activityMan->update_activity(app.activityMan, &activity, NULL, NULL);



    return 0;
}

void DISCORD_update(const char *level, const char *name)
{
    if(app.activityMan == NULL)
    {
        app.activityMan = app.core->get_activity_manager(app.core);
    }
    strncpy(activity.state, name, 127);
    strncpy(activity.assets.large_image, "vvvvvv", 127);
    strncpy(activity.assets.large_text, level, 127);
    activity.state[127] = '\0';
    activity.assets.large_image[127] = '\0';
    activity.assets.large_text[127] = '\0';

    app.activityMan->update_activity(app.activityMan, &activity, NULL, NULL);

    DISCORD_REQUIRE(app.core->run_callbacks(app.core));
}
void DISCORD_unlockAchievement(const char *name)
{
    // No "achivements" in Discord
}

void DISCORD_shutdown(void)
{
    // ???
}

#endif // MakeAndPlay
