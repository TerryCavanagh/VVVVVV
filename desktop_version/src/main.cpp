#include <SDL.h>
#include <stdio.h>

#include "DeferCallbacks.h"
#include "editor.h"
#include "Enums.h"
#include "Entity.h"
#include "Exit.h"
#include "FileSystemUtils.h"
#include "Game.h"
#include "Graphics.h"
#include "Input.h"
#include "KeyPoll.h"
#include "Logic.h"
#include "Map.h"
#include "Music.h"
#include "Network.h"
#include "preloader.h"
#include "Render.h"
#include "RenderFixed.h"
#include "Screen.h"
#include "Script.h"
#include "SoundSystem.h"
#include "UtilityClass.h"

scriptclass script;

#if !defined(NO_CUSTOM_LEVELS)
std::vector<edentities> edentity;
editorclass ed;
#endif

UtilityClass help;
Graphics graphics;
musicclass music;
Game game;
KeyPoll key;
mapclass map;
entityclass obj;
Screen gameScreen;

static bool startinplaytest = false;
static bool savefileplaytest = false;
static int savex = 0;
static int savey = 0;
static int saverx = 0;
static int savery = 0;
static int savegc = 0;
static int savemusic = 0;
static std::string playassets;

static std::string playtestname;

static volatile Uint32 time_ = 0;
static volatile Uint32 timePrev = 0;
static volatile Uint32 accumulator = 0;
static volatile Uint32 f_time = 0;
static volatile Uint32 f_timePrev = 0;

static inline Uint32 get_framerate(const int slowdown)
{
    switch (slowdown)
    {
    case 30:
        return 34;
    case 24:
        return 41;
    case 18:
        return 55;
    case 12:
        return 83;
    }

    return 34;
}

enum FuncType
{
    Func_null,
    Func_fixed,
    Func_input,
    Func_delta
};

struct ImplFunc
{
    enum FuncType type;
    void (*func)(void);
};

static void runscript(void)
{
    script.run();
}

static void teleportermodeinput(void)
{
    if (game.useteleporter)
    {
        teleporterinput();
    }
    else
    {
        script.run();
        gameinput();
    }
}

/* Only gets used in EDITORMODE. I assume the compiler will optimize this away
 * if this is a NO_CUSTOM_LEVELS or NO_EDITOR build
 */
static void flipmodeoff(void)
{
    graphics.flipmode = false;
}

static void focused_begin(void);
static void focused_end(void);

static const inline struct ImplFunc* get_gamestate_funcs(
    const int gamestate,
    int* num_implfuncs
) {
    switch (gamestate)
    {

#define FUNC_LIST_BEGIN(GAMESTATE) \
    case GAMESTATE: \
    { \
        static const struct ImplFunc implfuncs[] = { \
            {Func_fixed, focused_begin},

#define FUNC_LIST_END \
            {Func_fixed, focused_end} \
        }; \
        *num_implfuncs = SDL_arraysize(implfuncs); \
        return implfuncs; \
    }

    FUNC_LIST_BEGIN(GAMEMODE)
        {Func_fixed, runscript},
        {Func_fixed, gamerenderfixed},
        {Func_delta, gamerender},
        {Func_input, gameinput},
        {Func_fixed, gamelogic},
    FUNC_LIST_END

    FUNC_LIST_BEGIN(TITLEMODE)
        {Func_input, titleinput},
        {Func_fixed, titlerenderfixed},
        {Func_delta, titlerender},
        {Func_fixed, titlelogic},
    FUNC_LIST_END

    FUNC_LIST_BEGIN(MAPMODE)
        {Func_fixed, maprenderfixed},
        {Func_delta, maprender},
        {Func_input, mapinput},
        {Func_fixed, maplogic},
    FUNC_LIST_END

    FUNC_LIST_BEGIN(TELEPORTERMODE)
        {Func_fixed, maprenderfixed},
        {Func_delta, teleporterrender},
        {Func_input, teleportermodeinput},
        {Func_fixed, maplogic},
    FUNC_LIST_END

    FUNC_LIST_BEGIN(GAMECOMPLETE)
        {Func_fixed, gamecompleterenderfixed},
        {Func_delta, gamecompleterender},
        {Func_input, gamecompleteinput},
        {Func_fixed, gamecompletelogic},
    FUNC_LIST_END

    FUNC_LIST_BEGIN(GAMECOMPLETE2)
        {Func_delta, gamecompleterender2},
        {Func_input, gamecompleteinput2},
        {Func_fixed, gamecompletelogic2},
    FUNC_LIST_END

#if !defined(NO_CUSTOM_LEVELS) && !defined(NO_EDITOR)
    FUNC_LIST_BEGIN(EDITORMODE)
        {Func_fixed, flipmodeoff},
        {Func_input, editorinput},
        {Func_fixed, editorrenderfixed},
        {Func_delta, editorrender},
        {Func_fixed, editorlogic},
    FUNC_LIST_END
#endif

    FUNC_LIST_BEGIN(PRELOADER)
        {Func_input, preloaderinput},
        {Func_fixed, preloaderrenderfixed},
        {Func_delta, preloaderrender},
    FUNC_LIST_END

#undef FUNC_LIST_END
#undef FUNC_LIST_BEGIN

    }

    SDL_assert(0 && "Invalid gamestate!");
    return NULL;
}

enum IndexCode
{
    Index_none,
    Index_end
};

static const struct ImplFunc* gamestate_funcs = NULL;
static int num_gamestate_funcs = 0;
static int gamestate_func_index = -1;

static enum IndexCode increment_gamestate_func_index(void)
{
    gamestate_func_index++;

    if (gamestate_func_index == num_gamestate_funcs)
    {
        /* Reached the end of current gamestate order.
         * Re-fetch for new order if gamestate changed.
         */
        gamestate_funcs = get_gamestate_funcs(
            game.gamestate,
            &num_gamestate_funcs
        );

        /* Also run callbacks that were deferred to end of func sequence. */
        DEFER_execute_callbacks();

        gamestate_func_index = 0;

        return Index_end;
    }

    return Index_none;
}

static void unfocused_run(void);

static const struct ImplFunc unfocused_func_list[] = {
    {
        Func_input, /* we still need polling when unfocused */
        unfocused_run
    }
};
static const struct ImplFunc* unfocused_funcs = unfocused_func_list;
static int num_unfocused_funcs = SDL_arraysize(unfocused_func_list);
static int unfocused_func_index = 0; // This does not get incremented on start, do NOT use -1!

static enum IndexCode increment_unfocused_func_index(void)
{
    unfocused_func_index++;

    if (unfocused_func_index == num_unfocused_funcs)
    {
        unfocused_func_index = 0;

        return Index_end;
    }

    return Index_none;
}

static const struct ImplFunc** active_funcs = NULL;
static int* num_active_funcs = NULL;
static int* active_func_index = NULL;
static enum IndexCode (*increment_func_index)(void) = NULL;

enum LoopCode
{
    Loop_continue,
    Loop_stop
};

static enum LoopCode loop_assign_active_funcs(void)
{
    if (key.isActive)
    {
        active_funcs = &gamestate_funcs;
        num_active_funcs = &num_gamestate_funcs;
        active_func_index = &gamestate_func_index;
        increment_func_index = &increment_gamestate_func_index;
    }
    else
    {
        active_funcs = &unfocused_funcs;
        num_active_funcs = &num_unfocused_funcs;
        active_func_index = &unfocused_func_index;
        increment_func_index = &increment_unfocused_func_index;
    }
    return Loop_continue;
}

static enum LoopCode loop_run_active_funcs(void)
{
    while ((*active_funcs)[*active_func_index].type != Func_delta)
    {
        const struct ImplFunc* implfunc = &(*active_funcs)[*active_func_index];
        enum IndexCode index_code;

        if (implfunc->type == Func_input && !game.inputdelay)
        {
            key.Poll();
        }

        if (implfunc->type != Func_null && implfunc->func != NULL)
        {
            implfunc->func();
        }

        index_code = increment_func_index();

        if (index_code == Index_end)
        {
            return Loop_continue;
        }
    }

    /* About to switch over to rendering... but call this first. */
    graphics.renderfixedpre();

    return Loop_stop;
}

static enum LoopCode loop_begin(void);
static enum LoopCode loop_end(void);

static enum LoopCode (*const meta_funcs[])(void) = {
    loop_begin,
    loop_assign_active_funcs,
    loop_run_active_funcs,
    loop_end
};
static int meta_func_index = 0;

static void inline fixedloop(void)
{
    while (true)
    {
        enum LoopCode loop_code = meta_funcs[meta_func_index]();

        if (loop_code == Loop_stop)
        {
            break;
        }

        meta_func_index = (meta_func_index + 1) % SDL_arraysize(meta_funcs);
    }
}

static void inline deltaloop(void);

static void cleanup(void);

int main(int argc, char *argv[])
{
    char* baseDir = NULL;
    char* assetsPath = NULL;

    for (int i = 1; i < argc; ++i)
    {
#define ARG(name) (SDL_strcmp(argv[i], name) == 0)
#define ARG_INNER(code) \
    if (i + 1 < argc) \
    { \
        code \
    } \
    else \
    { \
        printf("%s option requires one argument.\n", argv[i]); \
        VVV_exit(1); \
    }

        if (ARG("-renderer"))
        {
            ARG_INNER({
                i++;
                SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, argv[i], SDL_HINT_OVERRIDE);
            })
        }
        else if (ARG("-basedir"))
        {
            ARG_INNER({
                i++;
                baseDir = argv[i];
            })
        }
        else if (ARG("-assets"))
        {
            ARG_INNER({
                i++;
                assetsPath = argv[i];
            })
        }
        else if (ARG("-playing") || ARG("-p"))
        {
            ARG_INNER({
                i++;
                startinplaytest = true;
                playtestname = std::string("levels/");
                playtestname.append(argv[i]);
                playtestname.append(std::string(".vvvvvv"));
            })
        }
        else if (ARG("-playx") || ARG("-playy") ||
        ARG("-playrx") || ARG("-playry") ||
        ARG("-playgc") || ARG("-playmusic"))
        {
            ARG_INNER({
                savefileplaytest = true;
                int v = help.Int(argv[i+1]);
                if (ARG("-playx")) savex = v;
                else if (ARG("-playy")) savey = v;
                else if (ARG("-playrx")) saverx = v;
                else if (ARG("-playry")) savery = v;
                else if (ARG("-playgc")) savegc = v;
                else if (ARG("-playmusic")) savemusic = v;
                i++;
            })
        }
        else if (ARG("-playassets"))
        {
            ARG_INNER({
                i++;
                // Even if this is a directory, FILESYSTEM_mountassets() expects '.vvvvvv' on the end
                playassets = "levels/" + std::string(argv[i]) + ".vvvvvv";
            })
        }
#undef ARG_INNER
#undef ARG
        else
        {
            printf("Error: invalid option: %s\n", argv[i]);
            VVV_exit(1);
        }
    }

    if(!FILESYSTEM_init(argv[0], baseDir, assetsPath))
    {
        puts("Unable to initialize filesystem!");
        VVV_exit(1);
    }

    SDL_Init(
        SDL_INIT_VIDEO |
        SDL_INIT_AUDIO |
        SDL_INIT_JOYSTICK |
        SDL_INIT_GAMECONTROLLER
    );
    if (SDL_IsTextInputActive() == SDL_TRUE)
    {
        SDL_StopTextInput();
    }

    NETWORK_init();

    printf("\t\t\n");
    printf("\t\t\n");
    printf("\t\t       VVVVVV\n");
    printf("\t\t\n");
    printf("\t\t\n");
    printf("\t\t  8888888888888888  \n");
    printf("\t\t88888888888888888888\n");
    printf("\t\t888888    8888    88\n");
    printf("\t\t888888    8888    88\n");
    printf("\t\t88888888888888888888\n");
    printf("\t\t88888888888888888888\n");
    printf("\t\t888888            88\n");
    printf("\t\t88888888        8888\n");
    printf("\t\t  8888888888888888  \n");
    printf("\t\t      88888888      \n");
    printf("\t\t  8888888888888888  \n");
    printf("\t\t88888888888888888888\n");
    printf("\t\t88888888888888888888\n");
    printf("\t\t88888888888888888888\n");
    printf("\t\t8888  88888888  8888\n");
    printf("\t\t8888  88888888  8888\n");
    printf("\t\t    888888888888    \n");
    printf("\t\t    8888    8888    \n");
    printf("\t\t  888888    888888  \n");
    printf("\t\t  888888    888888  \n");
    printf("\t\t  888888    888888  \n");
    printf("\t\t\n");
    printf("\t\t\n");

    //Set up screen




    // Load Ini


    graphics.init();

    game.init();

    // This loads music too...
    graphics.reloadresources();

    game.gamestate = PRELOADER;

    game.menustart = false;

    // Initialize title screen to cyan
    graphics.titlebg.colstate = 10;
    map.nexttowercolour();

    map.ypos = (700-29) * 8;
    graphics.towerbg.bypos = map.ypos / 2;
    graphics.titlebg.bypos = map.ypos / 2;

    {
        // Prioritize unlock.vvv first (2.2 and below),
        // but settings have been migrated to settings.vvv (2.3 and up)
        ScreenSettings screen_settings;
        game.loadstats(&screen_settings);
        game.loadsettings(&screen_settings);
        gameScreen.init(screen_settings);
    }
    graphics.screenbuffer = &gameScreen;

    graphics.create_buffers(gameScreen.GetFormat());

    if (game.skipfakeload)
        game.gamestate = TITLEMODE;
    if (game.slowdown == 0) game.slowdown = 30;

    //Check to see if you've already unlocked some achievements here from before the update
    if (game.swnbestrank > 0){
        if(game.swnbestrank >= 1) game.unlockAchievement("vvvvvvsupgrav5");
        if(game.swnbestrank >= 2) game.unlockAchievement("vvvvvvsupgrav10");
        if(game.swnbestrank >= 3) game.unlockAchievement("vvvvvvsupgrav15");
        if(game.swnbestrank >= 4) game.unlockAchievement("vvvvvvsupgrav20");
        if(game.swnbestrank >= 5) game.unlockAchievement("vvvvvvsupgrav30");
        if(game.swnbestrank >= 6) game.unlockAchievement("vvvvvvsupgrav60");
    }

    if(game.unlock[5]) game.unlockAchievement("vvvvvvgamecomplete");
    if(game.unlock[19]) game.unlockAchievement("vvvvvvgamecompleteflip");
    if(game.unlock[20]) game.unlockAchievement("vvvvvvmaster");

    if (game.bestgamedeaths > -1) {
        if (game.bestgamedeaths <= 500) {
            game.unlockAchievement("vvvvvvcomplete500");
        }
        if (game.bestgamedeaths <= 250) {
            game.unlockAchievement("vvvvvvcomplete250");
        }
        if (game.bestgamedeaths <= 100) {
            game.unlockAchievement("vvvvvvcomplete100");
        }
        if (game.bestgamedeaths <= 50) {
            game.unlockAchievement("vvvvvvcomplete50");
        }
    }

    if(game.bestrank[0]>=3) game.unlockAchievement("vvvvvvtimetrial_station1_fixed");
    if(game.bestrank[1]>=3) game.unlockAchievement("vvvvvvtimetrial_lab_fixed");
    if(game.bestrank[2]>=3) game.unlockAchievement("vvvvvvtimetrial_tower_fixed");
    if(game.bestrank[3]>=3) game.unlockAchievement("vvvvvvtimetrial_station2_fixed");
    if(game.bestrank[4]>=3) game.unlockAchievement("vvvvvvtimetrial_warp_fixed");
    if(game.bestrank[5]>=3) game.unlockAchievement("vvvvvvtimetrial_final_fixed");

    obj.init();

#if !defined(NO_CUSTOM_LEVELS)
    if (startinplaytest) {
        game.levelpage = 0;
        game.playcustomlevel = 0;
        game.playassets = playassets;
        game.menustart = true;

        LevelMetaData meta;
        if (ed.getLevelMetaData(playtestname, meta)) {
            ed.ListOfMetaData.clear();
            ed.ListOfMetaData.push_back(meta);
        } else {
            ed.loadZips();
            if (ed.getLevelMetaData(playtestname, meta)) {
                ed.ListOfMetaData.clear();
                ed.ListOfMetaData.push_back(meta);
            } else {
                printf("Level not found\n");
                VVV_exit(1);
            }
        }

        game.loadcustomlevelstats();
        game.customleveltitle=ed.ListOfMetaData[game.playcustomlevel].title;
        game.customlevelfilename=ed.ListOfMetaData[game.playcustomlevel].filename;
        if (savefileplaytest) {
            game.playx = savex;
            game.playy = savey;
            game.playrx = saverx;
            game.playry = savery;
            game.playgc = savegc;
            game.playmusic = savemusic;
            game.cliplaytest = true;
            script.startgamemode(23);
        } else {
            script.startgamemode(22);
        }

        graphics.fademode = 0;
    }
#endif

    key.isActive = true;

    gamestate_funcs = get_gamestate_funcs(game.gamestate, &num_gamestate_funcs);
    loop_assign_active_funcs();

    while(!key.quitProgram)
    {
        f_time = SDL_GetTicks();

        const Uint32 f_timetaken = f_time - f_timePrev;
        if (!game.over30mode && f_timetaken < 34)
        {
            const volatile Uint32 f_delay = 34 - f_timetaken;
            SDL_Delay(f_delay);
            f_time = SDL_GetTicks();
        }

        f_timePrev = f_time;

        timePrev = time_;
        time_ = SDL_GetTicks();

        deltaloop();
    }

    cleanup();
    return 0;
}

static void cleanup(void)
{
    /* Order matters! */
    game.savestatsandsettings();
    gameScreen.destroy();
    graphics.grphx.destroy();
    graphics.destroy_buffers();
    graphics.destroy();
    music.destroy();
    NETWORK_shutdown();
    SDL_Quit();
    FILESYSTEM_deinit();
}

void VVV_exit(const int exit_code)
{
    cleanup();
    exit(exit_code);
}

static void inline deltaloop(void)
{
    //timestep limit to 30
    const float rawdeltatime = static_cast<float>(time_ - timePrev);
    accumulator += rawdeltatime;

    Uint32 timesteplimit;
    if (game.gamestate == EDITORMODE)
    {
        timesteplimit = 24;
    }
    else if (game.gamestate == GAMEMODE)
    {
        timesteplimit = get_framerate(game.slowdown);
    }
    else
    {
        timesteplimit = 34;
    }

    while (accumulator >= timesteplimit)
    {
        increment_func_index();

        accumulator = SDL_fmodf(accumulator, timesteplimit);

        /* We are done rendering. */
        graphics.renderfixedpost();

        fixedloop();
    }
    const float alpha = game.over30mode ? static_cast<float>(accumulator) / timesteplimit : 1.0f;
    graphics.alpha = alpha;

    if (active_func_index == NULL
    || *active_func_index == -1
    || active_funcs == NULL)
    {
        /* Somehow the first deltatime has been too small and things haven't
         * initialized. We'll just no-op for now.
         */
    }
    else
    {
        const struct ImplFunc* implfunc = &(*active_funcs)[*active_func_index];

        if (implfunc->type == Func_delta && implfunc->func != NULL)
        {
            implfunc->func();

            gameScreen.FlipScreen();
        }
    }
}

static enum LoopCode loop_begin(void)
{
    if (game.inputdelay)
    {
        key.Poll();
    }

    // Update network per frame.
    NETWORK_update();

    return Loop_continue;
}

static void unfocused_run(void)
{
    Mix_Pause(-1);
    Mix_PauseMusic();

    if (!game.blackout)
    {
        ClearSurface(graphics.backBuffer);
#define FLIP(YPOS) graphics.flipmode ? 232 - YPOS : YPOS
        graphics.bprint(5, FLIP(110), "Game paused", 196 - help.glow, 255 - help.glow, 196 - help.glow, true);
        graphics.bprint(5, FLIP(120), "[click to resume]", 196 - help.glow, 255 - help.glow, 196 - help.glow, true);
        graphics.bprint(5, FLIP(220), "Press M to mute in game", 164 - help.glow, 196 - help.glow, 164 - help.glow, true);
        graphics.bprint(5, FLIP(230), "Press N to mute music only", 164 - help.glow, 196 - help.glow, 164 - help.glow, true);
#undef FLIP
    }
    graphics.render();
    gameScreen.FlipScreen();
    //We are minimised, so lets put a bit of a delay to save CPU
    SDL_Delay(100);
}

static void focused_begin(void)
{
    Mix_Resume(-1);
    Mix_ResumeMusic();
}

static void focused_end(void)
{
    /* no-op. */
}

static enum LoopCode loop_end(void)
{
    //We did editorinput, now it's safe to turn this off
    key.linealreadyemptykludge = false;

    //Mute button
    if (key.isDown(KEYBOARD_m) && game.mutebutton<=0 && !key.textentry())
    {
        game.mutebutton = 8;
        if (game.muted)
        {
            game.muted = false;
        }
        else
        {
            game.muted = true;
        }
    }
    if(game.mutebutton>0)
    {
        game.mutebutton--;
    }

    if (key.isDown(KEYBOARD_n) && game.musicmutebutton <= 0 && !key.textentry())
    {
        game.musicmutebutton = 8;
        game.musicmuted = !game.musicmuted;
    }
    if (game.musicmutebutton > 0)
    {
        game.musicmutebutton--;
    }

    if (game.muted)
    {
        Mix_VolumeMusic(0) ;
        Mix_Volume(-1,0);
    }
    else
    {
        Mix_Volume(-1,MIX_MAX_VOLUME);

        if (game.musicmuted)
        {
            Mix_VolumeMusic(0);
        }
        else
        {
            Mix_VolumeMusic(music.musicVolume);
        }
    }

    if (key.resetWindow)
    {
        key.resetWindow = false;
        gameScreen.ResizeScreen(-1, -1);
    }

    music.processmusic();
    graphics.processfade();
    game.gameclock();

    return Loop_continue;
}
