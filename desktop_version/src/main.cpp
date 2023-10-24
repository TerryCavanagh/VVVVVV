#include <SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include "ButtonGlyphs.h"
#include "CustomLevels.h"
#include "DeferCallbacks.h"
#include "Editor.h"
#include "Enums.h"
#include "Entity.h"
#include "Exit.h"
#include "FileSystemUtils.h"
#include "Font.h"
#include "Game.h"
#include "Graphics.h"
#include "GraphicsUtil.h"
#include "Input.h"
#include "InterimVersion.h"
#include "KeyPoll.h"
#include "Localization.h"
#include "LocalizationStorage.h"
#include "Logic.h"
#include "Map.h"
#include "Music.h"
#include "Network.h"
#include "preloader.h"
#include "ReleaseVersion.h"
#include "Render.h"
#include "RenderFixed.h"
#include "Screen.h"
#include "Script.h"
#include "UtilityClass.h"
#include "Vlogging.h"

scriptclass script;

std::vector<CustomEntity> customentities;
customlevelclass cl;
editorclass ed;

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
static int savemusic = -1;
static std::string playassets;

static std::string playtestname;

static volatile Uint64 time_ = 0;
static volatile Uint64 timePrev = 0;
static volatile Uint32 accumulator = 0;

#ifndef __EMSCRIPTEN__
static volatile Uint64 f_time = 0;
static volatile Uint64 f_timePrev = 0;
#endif

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
        {Func_fixed, teleporterrenderfixed},
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
        {Func_fixed, gamecompleterenderfixed2},
        {Func_delta, gamecompleterender2},
        {Func_input, gamecompleteinput2},
        {Func_fixed, gamecompletelogic2},
    FUNC_LIST_END

    FUNC_LIST_BEGIN(EDITORMODE)
        {Func_fixed, flipmodeoff},
        {Func_input, editorinput},
        {Func_fixed, editorlogic},
        {Func_fixed, editorrenderfixed},
        {Func_delta, editorrender},
    FUNC_LIST_END

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
        NULL
    },
    {
        Func_delta,
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

#ifdef __EMSCRIPTEN__
static void emscriptenloop(void)
{
    timePrev = time_;
    time_ = SDL_GetTicks64();
    deltaloop();
}
#endif

int main(int argc, char *argv[])
{
    char* baseDir = NULL;
    char* assetsPath = NULL;
    char* langDir = NULL;
    char* fontsDir = NULL;
    bool seed_use_sdl_getticks = false;
#ifdef _WIN32
    bool open_console = false;
#endif

    vlog_init();

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
        vlog_error("%s option requires one argument.", argv[i]); \
        VVV_exit(1); \
    }

        if (ARG("-version"))
        {
            /* Just print the version and exit. No vlogging. */
            puts(
                "VVVVVV " RELEASE_VERSION
#ifdef MAKEANDPLAY
                " [M&P]"
#endif
            );
#ifdef INTERIM_VERSION_EXISTS
            puts(COMMIT_DATE);
            puts(INTERIM_COMMIT);
            puts(BRANCH_NAME);
#endif
            VVV_exit(0);
        }
        else if (ARG("-addresses"))
        {
            printf("cl         : %p\n", (void*) &cl);
            printf("ed         : %p\n", (void*) &ed);
            printf("game       : %p\n", (void*) &game);
            printf("gameScreen : %p\n", (void*) &gameScreen);
            printf("graphics   : %p\n", (void*) &graphics);
            printf("help       : %p\n", (void*) &help);
            printf("key        : %p\n", (void*) &key);
            printf("map        : %p\n", (void*) &map);
            printf("music      : %p\n", (void*) &music);
            printf("obj        : %p\n", (void*) &obj);
            printf("script     : %p\n", (void*) &script);

            VVV_exit(0);
        }
        else if (ARG("-renderer"))
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
        else if (ARG("-langdir"))
        {
            ARG_INNER({
                i++;
                langDir = argv[i];
            })
        }
        else if (ARG("-fontsdir"))
        {
            ARG_INNER({
                i++;
                fontsDir = argv[i];
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
                // Even if this is a directory, FILESYSTEM_mountAssets() expects '.vvvvvv' on the end
                playassets = "levels/" + std::string(argv[i]) + ".vvvvvv";
            })
        }
        else if (ARG("-nooutput"))
        {
            vlog_toggle_output(0);
        }
        else if (ARG("-forcecolor") || ARG("-forcecolour"))
        {
            vlog_toggle_color(1);
        }
        else if (ARG("-nocolor") || ARG("-nocolour"))
        {
            vlog_toggle_color(0);
        }
        else if (ARG("-debug"))
        {
            vlog_toggle_debug(1);
        }
        else if (ARG("-noinfo"))
        {
            vlog_toggle_info(0);
        }
        else if (ARG("-nowarn"))
        {
            vlog_toggle_warn(0);
        }
        else if (ARG("-noerror"))
        {
            vlog_toggle_error(0);
        }
        else if (ARG("-translator"))
        {
            loc::show_translator_menu = true;
        }
#ifdef _WIN32
        else if (ARG("-console"))
        {
            open_console = true;
        }
#endif
        else if (ARG("-seed-use-sdl-getticks"))
        {
            seed_use_sdl_getticks = true;
        }
#undef ARG_INNER
#undef ARG
        else
        {
            vlog_error("Error: invalid option: %s", argv[i]);
            VVV_exit(1);
        }
    }

#if defined(ALWAYS_SHOW_TRANSLATOR_MENU)
    loc::show_translator_menu = true;
#endif

#ifdef _WIN32
    if (open_console)
    {
        vlog_open_console();
    }
#endif

    SDL_SetHintWithPriority(SDL_HINT_IME_SHOW_UI, "1", SDL_HINT_OVERRIDE);

    if(!FILESYSTEM_init(argv[0], baseDir, assetsPath, langDir, fontsDir))
    {
        vlog_error("Unable to initialize filesystem!");
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

    vlog_info("\t\t");
    vlog_info("\t\t");
    vlog_info("\t\t       VVVVVV");
    vlog_info("\t\t");
    vlog_info("\t\t");
    vlog_info("\t\t  8888888888888888  ");
    vlog_info("\t\t88888888888888888888");
    vlog_info("\t\t888888    8888    88");
    vlog_info("\t\t888888    8888    88");
    vlog_info("\t\t88888888888888888888");
    vlog_info("\t\t88888888888888888888");
    vlog_info("\t\t888888            88");
    vlog_info("\t\t88888888        8888");
    vlog_info("\t\t  8888888888888888  ");
    vlog_info("\t\t      88888888      ");
    vlog_info("\t\t  8888888888888888  ");
    vlog_info("\t\t88888888888888888888");
    vlog_info("\t\t88888888888888888888");
    vlog_info("\t\t88888888888888888888");
    vlog_info("\t\t8888  88888888  8888");
    vlog_info("\t\t8888  88888888  8888");
    vlog_info("\t\t    888888888888    ");
    vlog_info("\t\t    8888    8888    ");
    vlog_info("\t\t  888888    888888  ");
    vlog_info("\t\t  888888    888888  ");
    vlog_info("\t\t  888888    888888  ");
    vlog_info("\t\t");
    vlog_info("\t\t");

    // Set up screen
    graphics.init();

    game.init();
    game.seed_use_sdl_getticks = seed_use_sdl_getticks;

    game.gamestate = PRELOADER;

    game.menustart = false;

    // Initialize title screen to cyan
    graphics.titlebg.colstate = 10;
    map.nexttowercolour();

    map.ypos = (700-29) * 8;
    map.oldypos = map.ypos;
    map.setbgobjlerp(graphics.towerbg);
    map.setbgobjlerp(graphics.titlebg);

    {
        // Prioritize unlock.vvv first (2.2 and below),
        // but settings have been migrated to settings.vvv (2.3 and up)
        struct ScreenSettings screen_settings;
        SDL_zero(screen_settings);
        ScreenSettings_default(&screen_settings);
        game.loadstats(&screen_settings);
        game.loadsettings(&screen_settings);
        gameScreen.init(&screen_settings);
    }

    BUTTONGLYPHS_init();
    font::load_main();

    // This loads music too...
    if (!graphics.reloadresources())
    {
        /* Something wrong with the default assets? We can't use them to
         * display the error message, and we have to bail. */
        const char* message;
        if (FILESYSTEM_levelDirHasError())
        {
            message = FILESYSTEM_getLevelDirError();
        }
        else
        {
            message = loc::gettext("Something went wrong, but we forgot the error message.");
        }
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message, NULL);

        VVV_exit(1);
    }

    loc::loadtext(false);
    loc::loadlanguagelist();
    game.createmenu(Menu::mainmenu);

    graphics.create_buffers();

    if (game.skipfakeload)
        game.gamestate = TITLEMODE;
    if (game.slowdown == 0) game.slowdown = 30;

    if (!loc::lang_set && !loc::languagelist.empty())
    {
        loc::pre_title_lang_menu = true;
        game.gamestate = TITLEMODE;
        game.menustart = true;
        game.createmenu(Menu::language);
        game.currentmenuoption = loc::languagelist_curlang;
    }

    //Check to see if you've already unlocked some achievements here from before the update
    if (game.swnbestrank > 0){
        if(game.swnbestrank >= 1) game.unlockAchievement("vvvvvvsupgrav5");
        if(game.swnbestrank >= 2) game.unlockAchievement("vvvvvvsupgrav10");
        if(game.swnbestrank >= 3) game.unlockAchievement("vvvvvvsupgrav15");
        if(game.swnbestrank >= 4) game.unlockAchievement("vvvvvvsupgrav20");
        if(game.swnbestrank >= 5) game.unlockAchievement("vvvvvvsupgrav30");
        if(game.swnbestrank >= 6) game.unlockAchievement("vvvvvvsupgrav60");
    }

    if (game.unlock[UnlockTrophy_GAME_COMPLETE])
    {
        game.unlockAchievement("vvvvvvgamecomplete");
    }
    if (game.unlock[UnlockTrophy_FLIPMODE_COMPLETE])
    {
        game.unlockAchievement("vvvvvvgamecompleteflip");
    }
    if (game.unlock[UnlockTrophy_NODEATHMODE_COMPLETE])
    {
        game.unlockAchievement("vvvvvvmaster");
    }

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

    if (game.bestrank[TimeTrial_SPACESTATION1] >= 3)
    {
        game.unlockAchievement("vvvvvvtimetrial_station1_fixed");
    }
    if (game.bestrank[TimeTrial_LABORATORY] >= 3)
    {
        game.unlockAchievement("vvvvvvtimetrial_lab_fixed");
    }
    if (game.bestrank[TimeTrial_TOWER] >= 3)
    {
        game.unlockAchievement("vvvvvvtimetrial_tower_fixed");
    }
    if (game.bestrank[TimeTrial_SPACESTATION2] >= 3)
    {
        game.unlockAchievement("vvvvvvtimetrial_station2_fixed");
    }
    if (game.bestrank[TimeTrial_WARPZONE] >= 3)
    {
        game.unlockAchievement("vvvvvvtimetrial_warp_fixed");
    }
    if (game.bestrank[TimeTrial_FINALLEVEL] >= 3)
    {
        game.unlockAchievement("vvvvvvtimetrial_final_fixed");
    }

    obj.init();

    if (startinplaytest) {
        game.levelpage = 0;
        game.playcustomlevel = 0;
        game.playassets = playassets;
        game.menustart = true;

        LevelMetaData meta;
        CliPlaytestArgs pt_args;
        if (cl.getLevelMetaDataAndPlaytestArgs(playtestname, meta, &pt_args)) {
            cl.ListOfMetaData.clear();
            cl.ListOfMetaData.push_back(meta);
        } else {
            cl.loadZips();
            if (cl.getLevelMetaDataAndPlaytestArgs(playtestname, meta, &pt_args)) {
                cl.ListOfMetaData.clear();
                cl.ListOfMetaData.push_back(meta);
            } else {
                vlog_error("Level not found");
                VVV_exit(1);
            }
        }

        if (pt_args.valid)
        {
            savefileplaytest = true;
            savex = pt_args.x;
            savey = pt_args.y;
            saverx = pt_args.rx;
            savery = pt_args.ry;
            savegc = pt_args.gc;
            savemusic = pt_args.music;
        }

        game.loadcustomlevelstats();
        game.customleveltitle=cl.ListOfMetaData[game.playcustomlevel].title;
        game.customlevelfilename=cl.ListOfMetaData[game.playcustomlevel].filename;
        if (savefileplaytest) {
            game.playx = savex;
            game.playy = savey;
            game.playrx = saverx;
            game.playry = savery;
            game.playgc = savegc;
            game.playmusic = savemusic;
            game.cliplaytest = true;
            script.startgamemode(Start_CUSTOM_QUICKSAVE);
        } else {
            script.startgamemode(Start_CUSTOM);
        }

        graphics.fademode = FADE_NONE;
    }

    /* Only create the window after we have loaded all the assets. */
    SDL_ShowWindow(gameScreen.m_window);

    key.isActive = true;

    gamestate_funcs = get_gamestate_funcs(game.gamestate, &num_gamestate_funcs);
    loop_assign_active_funcs();

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(emscriptenloop, 0, 0);
#else
    while (true)
    {
        f_time = SDL_GetTicks64();

        const Uint64 f_timetaken = f_time - f_timePrev;
        if (!game.over30mode && f_timetaken < 34)
        {
            const volatile Uint64 f_delay = 34 - f_timetaken;
            SDL_Delay((Uint32) f_delay);
            f_time = SDL_GetTicks64();
        }

        f_timePrev = f_time;

        timePrev = time_;
        time_ = SDL_GetTicks64();

        deltaloop();
    }

    cleanup();
#endif

    return 0;
}

static void cleanup(void)
{
    /* Order matters! */
    if (FILESYSTEM_isInit()) /* not necessary but silences logs */
    {
        game.savestatsandsettings();
    }

    graphics.grphx.destroy();
    graphics.destroy_buffers();
    graphics.destroy();
    font::destroy();
    gameScreen.destroy();
    music.destroy();
    map.destroy();
    NETWORK_shutdown();
    loc::resettext(true);
    SDL_Quit();
    FILESYSTEM_deinit();
}

SDL_NORETURN void VVV_exit(const int exit_code)
{
    cleanup();
    exit(exit_code);
}

static void inline deltaloop(void)
{
    //timestep limit to 30
    const float rawdeltatime = static_cast<float>(time_ - timePrev);
    accumulator += rawdeltatime;

    Uint32 timesteplimit = game.get_timestep();

    while (accumulator >= timesteplimit)
    {
        enum IndexCode index_code = increment_func_index();

        if (index_code == Index_end)
        {
            loop_assign_active_funcs();
        }

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
            graphics.clear();

            graphics.set_render_target(graphics.gameTexture);

            implfunc->func();

            gameScreen.RenderPresent();
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
    if (!game.blackout)
    {
        graphics.fill_rect(0, 0, 0);
#define FLIP(YPOS) graphics.flipmode ? 232 - YPOS : YPOS
#define FLIP_PR_CJK_LOW (graphics.flipmode ? PR_CJK_HIGH : PR_CJK_LOW)
#define FLIP_PR_CJK_HIGH (graphics.flipmode ? PR_CJK_LOW : PR_CJK_HIGH)
        /* The pause screen can also appear on the language screen, where highlighting
         * a language changes the used language metadata but not the loaded strings... */
        uint32_t flags = PR_CEN | PR_BOR | PR_FONT_IDX(loc::langmeta.font_idx);
        font::print(flags | FLIP_PR_CJK_HIGH, -1, FLIP(110), loc::gettext("Game paused"), 196 - help.glow, 255 - help.glow, 196 - help.glow);

        if (BUTTONGLYPHS_keyboard_is_available())
        {
            font::print(flags | FLIP_PR_CJK_LOW, -1, FLIP(120), loc::gettext("[click to resume]"), 196 - help.glow, 255 - help.glow, 196 - help.glow);

            font::print(flags | FLIP_PR_CJK_HIGH, -1, FLIP(220), loc::gettext("Press M to mute in game"), 164 - help.glow, 196 - help.glow, 164 - help.glow);
            font::print(flags, -1, FLIP(230), loc::gettext("Press N to mute music only"), 164 - help.glow, 196 - help.glow, 164 - help.glow);
        }
#undef FLIP_PR_CJK_HIGH
#undef FLIP_PR_CJK_LOW
#undef FLIP
    }
    graphics.render();
    //We are minimised, so lets put a bit of a delay to save CPU
#ifndef __EMSCRIPTEN__
    SDL_Delay(100);
#endif
}

static void focused_begin(void)
{
    map.nexttowercolour_set = false;
}

static void focused_end(void)
{
    game.gameclock();
    music.processmusic();
    graphics.processfade();
}

static enum LoopCode loop_end(void)
{
    ++game.framecounter;

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

    music.updatemutestate();

    if (key.resetWindow)
    {
        key.resetWindow = false;
        gameScreen.ResizeScreen(-1, -1);
    }

    return Loop_continue;
}
