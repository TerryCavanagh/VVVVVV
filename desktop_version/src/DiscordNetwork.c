#include "MakeAndPlay.h"

#ifndef MAKEANDPLAY

#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

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


#define DISCORD_CLIENT_ID 1315544357532729447

// TO TERRY/FLIBIT: You can create your own Discord instance at the Discord Developer Portal. This ID belongs to me, so just be aware that if my account was to get hacked, VVVVVV RPC would too. Use your own!


struct DISCORD_application {
    struct IDiscordCore* core;
    struct IDiscordActivityManager* activityMan;
} app;

struct DiscordActivity activity;

bool discordDetected = false;
bool discordPostInit = false;


static void* libHandle = NULL;



#define FUNC_LIST \
    FOREACH_FUNC(enum EDiscordResult, DiscordCreate, (DiscordVersion, struct DiscordCreateParams*, struct IDiscordCore**))



#define FOREACH_FUNC(rettype, name, params) static rettype (*name) params = NULL;
FUNC_LIST
#undef FOREACH_FUNC

bool DISCORD_REQUIRE(int x) {
    if (!discordDetected && discordPostInit)
    {
        return false;
    }
    if (x != DiscordResult_Ok)
    {
        return false;
    }
    return true;
}



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
    if (libHandle != NULL)
    {
        app.core->destroy(app.core);
        ClearPointers();
    }
}

int32_t DISCORD_init(void)
{
#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__HAIKU__) || defined(__DragonFly__)
    return 0;
#endif
    libHandle = SDL_LoadObject(DISCORD_LIBRARY);
    if (libHandle == NULL)
    {
        vlog_info(DISCORD_LIBRARY " not found!");
        vlog_debug("Can't load object %s : %s\n", DISCORD_LIBRARY, SDL_GetError());
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
    SDL_memset(&app, 0, sizeof(app));

    struct DiscordCreateParams params;
    params.client_id = DISCORD_CLIENT_ID;
    params.flags = DiscordCreateFlags_NoRequireDiscord;

    if(!DISCORD_REQUIRE(DiscordCreate(DISCORD_VERSION, &params, &app.core)))
    { // Discord's API couldn't initialise, so just ignore it
        discordPostInit = true; // even if it fails, set post init to true.
        vlog_debug("Discord API failed to initialise!");
        discordDetected = false;
        return 0;
    }

    if(app.core == NULL)
    {
        discordPostInit = true;
        vlog_debug("app.core == null. DiscordCreate failed with a positive result?\nCheck DISCORD_REQUIRE  for bugs.");
        discordDetected = false;
        return 0;
    }
    discordPostInit = true;

    // Placeholder, is this really nesaccary
    SDL_strlcpy(activity.assets.large_image, "vvvvvv", sizeof(activity.assets.large_image));
    SDL_strlcpy(activity.assets.large_text, "Outside Dimension VVVVVV", sizeof(activity.assets.large_text));

    app.activityMan = app.core->get_activity_manager(app.core);
    app.activityMan->update_activity(app.activityMan, &activity, NULL, NULL);


    discordDetected = true;
    return 1;
}

void DISCORD_update(const char* area, const char* roomname)
{
    if(libHandle == NULL) {
        // no discord or just shutdown
        return;
    }
    if (app.core == NULL || !discordDetected)
    {
        // No Discord
        return;
    }
    if (!DISCORD_REQUIRE(app.core->run_callbacks(app.core)))
    {
        // Something  or other is wrong, but do we care?
        return;
    }
    if (app.activityMan == NULL)
    {
        vlog_debug("No activityMan!  - it\'s fine, we can recreate this" );
        app.activityMan = app.core->get_activity_manager(app.core);
    }
    if (SDL_strcmp(activity.state, roomname) != 0 || SDL_strcmp(activity.assets.large_text, area) != 0)
    {
        SDL_strlcpy(activity.state, roomname, sizeof(activity.state));
        SDL_strlcpy(activity.assets.large_image, "vvvvvv", sizeof(activity.assets.large_image));
        SDL_strlcpy(activity.assets.large_text, area, sizeof(activity.assets.large_text));

        app.activityMan->update_activity(app.activityMan, &activity, NULL, NULL);
    }
}
void DISCORD_unlockAchievement(const char *name)
{
    // No "achievements" in Discord
}



#endif // MakeAndPlay
