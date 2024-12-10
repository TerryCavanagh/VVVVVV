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
#include <SDL.h>
#include "discord_game_sdk.h" // DISCORD HEADERS REQUIRE LINKING AT RUNTIME!!!!!

#define DISCORD_REQUIRE(x) assert(x == DiscordResult_Ok)

struct Application {
    struct IDiscordCore* core;
    struct IDiscordActivityManager* activityMan;
};

struct DiscordActivity activity;
struct Application app;



int32_t DISCORD_init(void)
{
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
