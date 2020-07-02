#include <SDL.h>
#include "SoundSystem.h"

#include "UtilityClass.h"
#include "Game.h"
#include "Graphics.h"
#include "KeyPoll.h"
#include "Render.h"

#include "Tower.h"
#include "WarpClass.h"
#include "Labclass.h"
#include "Finalclass.h"
#include "Map.h"

#include "Screen.h"

#include "Script.h"

#include "Logic.h"

#include "Input.h"
#include "editor.h"
#include "preloader.h"

#include "FileSystemUtils.h"
#include "Network.h"

#include <stdio.h>
#include <string.h>

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

bool startinplaytest = false;
bool savefileplaytest = false;
int savex = 0;
int savey = 0;
int saverx = 0;
int savery = 0;
int savegc = 0;
int savemusic = 0;
std::string playassets;

std::string playtestname;

volatile Uint32 time_ = 0;
volatile Uint32 timePrev = 0;
volatile Uint32 accumulator = 0;
volatile Uint32 f_time = 0;
volatile Uint32 f_timePrev = 0;
volatile Uint32 f_accumulator = 0;

void inline gameloop();
void inline deltaloop();
void inline fixedloop();

int main(int argc, char *argv[])
{
    char* baseDir = NULL;
    char* assetsPath = NULL;

    for (int i = 1; i < argc; ++i)
    {
#define ARG(name) (strcmp(argv[i], name) == 0)
#define ARG_INNER(code) \
    if (i + 1 < argc) \
    { \
        code \
    } \
    else \
    { \
        printf("%s option requires one argument.\n", argv[i]); \
        return 1; \
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
                int v = std::atoi(argv[i+1]);
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
#undef ARG_IS
    }

    if(!FILESYSTEM_init(argv[0], baseDir, assetsPath))
    {
        puts("Unable to initialize filesystem!");
        return 1;
    }

    SDL_Init(
        SDL_INIT_VIDEO |
        SDL_INIT_AUDIO |
        SDL_INIT_JOYSTICK |
        SDL_INIT_GAMECONTROLLER
    );

    NETWORK_init();

    gameScreen.init();

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

    const SDL_PixelFormat* fmt = gameScreen.GetFormat();
    graphics.backBuffer = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 240, fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
    SDL_SetSurfaceBlendMode(graphics.backBuffer, SDL_BLENDMODE_NONE);
    graphics.footerbuffer = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 10, fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
    SDL_SetSurfaceBlendMode(graphics.footerbuffer, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(graphics.footerbuffer, 127);
    FillRect(graphics.footerbuffer, SDL_MapRGB(fmt, 0, 0, 0));

    graphics.ghostbuffer = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 240, fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
    SDL_SetSurfaceBlendMode(graphics.ghostbuffer, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(graphics.ghostbuffer, 127);

    graphics.Makebfont();

    graphics.foregroundBuffer =  SDL_CreateRGBSurface(SDL_SWSURFACE ,320 ,240 ,fmt->BitsPerPixel,fmt->Rmask,fmt->Gmask,fmt->Bmask,fmt->Amask  );
    SDL_SetSurfaceBlendMode(graphics.foregroundBuffer, SDL_BLENDMODE_NONE);

    graphics.screenbuffer = &gameScreen;

    graphics.menubuffer = SDL_CreateRGBSurface(SDL_SWSURFACE ,320 ,240 ,fmt->BitsPerPixel,fmt->Rmask,fmt->Gmask,fmt->Bmask,fmt->Amask );
    SDL_SetSurfaceBlendMode(graphics.menubuffer, SDL_BLENDMODE_NONE);

    graphics.towerbuffer =  SDL_CreateRGBSurface(SDL_SWSURFACE ,320+16 ,240+16 ,fmt->BitsPerPixel,fmt->Rmask,fmt->Gmask,fmt->Bmask,fmt->Amask  );
    SDL_SetSurfaceBlendMode(graphics.towerbuffer, SDL_BLENDMODE_NONE);
    graphics.towerbuffer_lerp = SDL_CreateRGBSurface(SDL_SWSURFACE, 320+16, 240+16, fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
    SDL_SetSurfaceBlendMode(graphics.towerbuffer, SDL_BLENDMODE_NONE);

    graphics.tempBuffer = SDL_CreateRGBSurface(SDL_SWSURFACE ,320 ,240 ,fmt->BitsPerPixel,fmt->Rmask,fmt->Gmask,fmt->Bmask,fmt->Amask  );
    SDL_SetSurfaceBlendMode(graphics.tempBuffer, SDL_BLENDMODE_NONE);

    game.gamestate = PRELOADER;

    game.menustart = false;
    game.mainmenu = 0;

    map.ypos = (700-29) * 8;
    map.bypos = map.ypos / 2;

    //Moved screensetting init here from main menu V2.1
    game.loadstats();

    // FIXME: Thanks to having to work around an SDL2 bug, this destroys the
    // renderer created by Screen::init(), which is a bit wasteful!
    // This is annoying to fix because we'd have to call gameScreen.init() after
    // game.loadstats(), but game.loadstats() assumes gameScreen.init() is already called!
    gameScreen.resetRendererWorkaround();

    if (game.skipfakeload)
        game.gamestate = TITLEMODE;
    if(game.usingmmmmmm==0) music.usingmmmmmm=false;
    if(game.usingmmmmmm==1) music.usingmmmmmm=true;
    if (game.slowdown == 0) game.slowdown = 30;

    switch(game.slowdown){
        case 30: game.gameframerate=34; break;
        case 24: game.gameframerate=41; break;
        case 18: game.gameframerate=55; break;
        case 12: game.gameframerate=83; break;
        default: game.gameframerate=34; break;
    }

    //Check to see if you've already unlocked some achievements here from before the update
    if (game.swnbestrank > 0){
        if(game.swnbestrank >= 1) NETWORK_unlockAchievement("vvvvvvsupgrav5");
        if(game.swnbestrank >= 2) NETWORK_unlockAchievement("vvvvvvsupgrav10");
        if(game.swnbestrank >= 3) NETWORK_unlockAchievement("vvvvvvsupgrav15");
        if(game.swnbestrank >= 4) NETWORK_unlockAchievement("vvvvvvsupgrav20");
        if(game.swnbestrank >= 5) NETWORK_unlockAchievement("vvvvvvsupgrav30");
        if(game.swnbestrank >= 6) NETWORK_unlockAchievement("vvvvvvsupgrav60");
    }

    if(game.unlock[5]) NETWORK_unlockAchievement("vvvvvvgamecomplete");
    if(game.unlock[19]) NETWORK_unlockAchievement("vvvvvvgamecompleteflip");
    if(game.unlock[20]) NETWORK_unlockAchievement("vvvvvvmaster");

    if (game.bestgamedeaths > -1) {
        if (game.bestgamedeaths <= 500) {
            NETWORK_unlockAchievement("vvvvvvcomplete500");
        }
        if (game.bestgamedeaths <= 250) {
            NETWORK_unlockAchievement("vvvvvvcomplete250");
        }
        if (game.bestgamedeaths <= 100) {
            NETWORK_unlockAchievement("vvvvvvcomplete100");
        }
        if (game.bestgamedeaths <= 50) {
            NETWORK_unlockAchievement("vvvvvvcomplete50");
        }
    }

    if(game.bestrank[0]>=3) NETWORK_unlockAchievement("vvvvvvtimetrial_station1_fixed");
    if(game.bestrank[1]>=3) NETWORK_unlockAchievement("vvvvvvtimetrial_lab_fixed");
    if(game.bestrank[2]>=3) NETWORK_unlockAchievement("vvvvvvtimetrial_tower_fixed");
    if(game.bestrank[3]>=3) NETWORK_unlockAchievement("vvvvvvtimetrial_station2_fixed");
    if(game.bestrank[4]>=3) NETWORK_unlockAchievement("vvvvvvtimetrial_warp_fixed");
    if(game.bestrank[5]>=3) NETWORK_unlockAchievement("vvvvvvtimetrial_final_fixed");

    obj.init();

#if !defined(NO_CUSTOM_LEVELS)
    if (startinplaytest) {
        game.levelpage = 0;
        game.playcustomlevel = 0;
        game.playassets = playassets;

        ed.directoryList.clear();
        ed.directoryList.push_back(playtestname);

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
                return 1;
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
            game.cliplaytest = true;
            music.play(savemusic);
            script.startgamemode(23);
        } else {
            script.startgamemode(22);
        }

        graphics.fademode = 0;
    }
#endif

    key.isActive = true;
    game.gametimer = 0;

    while(!key.quitProgram)
    {
        f_timePrev = f_time;
        f_time = SDL_GetTicks();
        const float f_rawdeltatime = static_cast<float>(f_time - f_timePrev);
        if (!game.over30mode)
        {
            f_accumulator += f_rawdeltatime;
        }

        gameloop();
    }

    game.savestats();
    NETWORK_shutdown();
    SDL_Quit();
    FILESYSTEM_deinit();

    return 0;
}

void inline gameloop()
{
    while ((game.over30mode || f_accumulator >= 34) && !key.quitProgram)
    {
        if (game.over30mode)
        {
            f_accumulator = 0;
        }
        else
        {
            f_accumulator = fmodf(f_accumulator, 34);
        }

        timePrev = time_;
        time_ = SDL_GetTicks();

        deltaloop();
    }
}

void inline deltaloop()
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
        timesteplimit = game.gameframerate;
    }
    else
    {
        timesteplimit = 34;
    }

    while (accumulator >= timesteplimit)
    {
        accumulator = fmodf(accumulator, timesteplimit);

        fixedloop();
    }
    const float alpha = game.over30mode ? static_cast<float>(accumulator) / timesteplimit : 1.0f;
    graphics.alpha = alpha;

    if (key.isActive)
    {
        switch (game.gamestate)
        {
        case PRELOADER:
            preloaderrender();
            break;
#if !defined(NO_CUSTOM_LEVELS) && !defined(NO_EDITOR)
        case EDITORMODE:
            graphics.flipmode = false;
            editorrender();
            break;
#endif
        case TITLEMODE:
            titlerender();
            break;
        case GAMEMODE:
            gamerender();
            break;
        case MAPMODE:
            maprender();
            break;
        case TELEPORTERMODE:
            teleporterrender();
            break;
        case GAMECOMPLETE:
            gamecompleterender();
            break;
        case GAMECOMPLETE2:
            gamecompleterender2();
            break;
        case CLICKTOSTART:
            help.updateglow();
            break;
        }
        gameScreen.FlipScreen();
    }
}

void inline fixedloop()
{
    // Update network per frame.
    NETWORK_update();

    key.Poll();
    if(key.toggleFullscreen)
    {
        if(!gameScreen.isWindowed)
        {
            SDL_ShowCursor(SDL_DISABLE);
            SDL_ShowCursor(SDL_ENABLE);
        }
        else
        {
            SDL_ShowCursor(SDL_ENABLE);
        }


        if(game.gamestate == EDITORMODE)
        {
            SDL_ShowCursor(SDL_ENABLE);
        }

        gameScreen.toggleFullScreen();
        game.fullscreen = !game.fullscreen;
        key.toggleFullscreen = false;

        key.keymap.clear(); //we lost the input due to a new window.
        if (game.glitchrunnermode)
        {
            game.press_left = false;
            game.press_right = false;
            game.press_action = true;
            game.press_map = false;
        }
    }

    if(!key.isActive)
    {
        Mix_Pause(-1);
        Mix_PauseMusic();

        if (!game.blackout)
        {
            FillRect(graphics.backBuffer, 0x00000000);
            graphics.bprint(5, 110, "Game paused", 196 - help.glow, 255 - help.glow, 196 - help.glow, true);
            graphics.bprint(5, 120, "[click to resume]", 196 - help.glow, 255 - help.glow, 196 - help.glow, true);
            graphics.bprint(5, 220, "Press M to mute in game", 164 - help.glow, 196 - help.glow, 164 - help.glow, true);
            graphics.bprint(5, 230, "Press N to mute music only", 164 - help.glow, 196 - help.glow, 164 - help.glow, true);
        }
        graphics.render();
        gameScreen.FlipScreen();
        //We are minimised, so lets put a bit of a delay to save CPU
        SDL_Delay(100);
    }
    else
    {
        Mix_Resume(-1);
        Mix_ResumeMusic();
        game.gametimer++;
        graphics.cutscenebarstimer();

        switch(game.gamestate)
        {
        case PRELOADER:
            preloaderlogic();
            break;
#if !defined(NO_CUSTOM_LEVELS) && !defined(NO_EDITOR)
        case EDITORMODE:
            //Input
            editorinput();
            ////Logic
            editorlogic();
            break;
#endif
        case TITLEMODE:
            //Input
            titleinput();
            ////Logic
            titlelogic();
            break;
        case GAMEMODE:
            // WARNING: If updating this code, don't forget to update Map.cpp mapclass::twoframedelayfix()

            // Ugh, I hate this kludge variable but it's the only way to do it
            if (script.dontrunnextframe)
            {
                script.dontrunnextframe = false;
            }
            else if (script.running)
            {
                script.run();
            }

            //Update old positions of entities - has to be done BEFORE gameinput!
            for (size_t i = 0; i < obj.entities.size(); i++)
            {
                obj.entities[i].oldxp = obj.entities[i].xp;
                obj.entities[i].oldyp = obj.entities[i].yp;
            }

            gameinput();
            gamelogic();


            break;
        case MAPMODE:
            mapinput();
            maplogic();
            break;
        case TELEPORTERMODE:
            if(game.useteleporter)
            {
                teleporterinput();
            }
            else
            {
                if (script.running)
                {
                    script.run();
                }
                gameinput();
            }
            maplogic();
            break;
        case GAMECOMPLETE:
            //Input
            gamecompleteinput();
            //Logic
            gamecompletelogic();
            break;
        case GAMECOMPLETE2:
            //Input
            gamecompleteinput2();
            //Logic
            gamecompletelogic2();
            break;
        case CLICKTOSTART:
            break;
        default:

            break;

        }

    }

    //Screen effects timers
    if (key.isActive && game.flashlight > 0)
    {
        game.flashlight--;
    }
    if (key.isActive && game.screenshake > 0)
    {
        game.screenshake--;
        graphics.updatescreenshake();
    }

    if (graphics.screenbuffer->badSignalEffect)
    {
        UpdateFilter();
    }

    //We did editorinput, now it's safe to turn this off
    key.linealreadyemptykludge = false;

    if (game.savemystats)
    {
        game.savemystats = false;
        game.savestats();
    }

    //Mute button
#if !defined(NO_CUSTOM_LEVELS) && !defined(NO_EDITOR)
    bool inEditor = ed.textentry || ed.scripthelppage == 1;
#else
    bool inEditor = false;
#endif
    if (key.isDown(KEYBOARD_m) && game.mutebutton<=0 && !inEditor)
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

    if (key.isDown(KEYBOARD_n) && game.musicmutebutton <= 0 && !inEditor)
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

        if (game.musicmuted || game.completestop)
        {
            Mix_VolumeMusic(0);
        }
        else
        {
            Mix_VolumeMusic(MIX_MAX_VOLUME);
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
}
