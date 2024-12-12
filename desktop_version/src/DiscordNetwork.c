#include "MakeAndPlay.h"

#ifndef MAKEANDPLAY

#include <stdint.h>
#include <stdio.h>
#include <SDL.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "Vlogging.h"

// Library includes
#include "discord_game_sdk.h"

#if defined(_WIN32)
#define DISCORD_LIBRARY "discord_game_sdk.dll"
#elif defined(__APPLE__)
#define DISCORD_LIBRARY "libdiscord_game_sdk.dylib"
#elif defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__HAIKU__) || defined(__DragonFly__)
#define DISCORD_LIBRARY "libdiscord_game_sdk.so"
#else
#error DISCORD_LIBRARY: Unrecognized platform!
#endif


#define DISCORD_CRASH_LIMIT 10
#define DISCORD_CLIENT_ID 1315544357532729447 // TO TERRY/FLIBIT: You can create your own Discord instance at the Discord Developer Portal. This ID belongs to me, so just be aware that if my account was to get hacked, VVVVVV RPC would too. Use your own!


struct DISCORD_application {
    struct IDiscordCore* core;
    struct IDiscordActivityManager* activityMan;
} app;

struct DiscordActivity activity;

int discordCrashes = 0; // This is here to ensure we do not get stuck in a theoratical softlock of opening and crashing Discord instances.

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

void DISCORD_shutdown(void)
{
    if (libHandle)
    {
        ClearPointers();
    }
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
    params.client_id = DISCORD_CLIENT_ID;
    params.flags = DiscordCreateFlags_Default;

    if(!DiscordCreate(DISCORD_VERSION, &params, &app.core))
    {
        return 1;
    }


    SDL_strlcpy(activity.assets.large_image, "vvvvvv", sizeof(activity.assets.large_image));
    SDL_strlcpy(activity.assets.large_text, "Outside Dimension VVVVVV", sizeof(activity.assets.large_text));

    app.activityMan = app.core->get_activity_manager(app.core);
    app.activityMan->update_activity(app.activityMan, &activity, NULL, NULL);



    return 0;
}

void DISCORD_REQUIRE(int x) {
    if(discordCrashes > DISCORD_CRASH_LIMIT)
    {
        DISCORD_shutdown();
        return;
    }
    if(x != DiscordResult_Ok)
    {
        ++discordCrashes;
        DISCORD_shutdown();
        DISCORD_init();
        return;
    }
    discordCrashes = 0;
}

void DISCORD_update(const char *level, const char *name)
{
    if(discordCrashes > DISCORD_CRASH_LIMIT) {
        return;
    }
    if(app.activityMan == NULL)
    {
        app.activityMan = app.core->get_activity_manager(app.core);
    }
    SDL_strlcpy(activity.state, name, sizeof(activity.state));
    SDL_strlcpy(activity.assets.large_image, "vvvvvv", sizeof(activity.assets.large_image));
    SDL_strlcpy(activity.assets.large_text, level, sizeof(activity.assets.large_text));

    app.activityMan->update_activity(app.activityMan, &activity, NULL, NULL);

    DISCORD_REQUIRE(app.core->run_callbacks(app.core));
}
void DISCORD_unlockAchievement(const char *name)
{
    // No "achievements" in Discord
}



#endif // MakeAndPlay
