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

bool startinplaytest = false;
bool savefileplaytest = false;
int savex = 0;
int savey = 0;
int saverx = 0;
int savery = 0;
int savegc = 0;
int savemusic = 0;

std::string playtestname;

int main(int argc, char *argv[])
{
    char* baseDir = NULL;
    char* assetsPath = NULL;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-renderer") == 0) {
            ++i;
            SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, argv[i], SDL_HINT_OVERRIDE);
        } else if (strcmp(argv[i], "-basedir") == 0) {
            ++i;
            baseDir = argv[i];
        } else if (strcmp(argv[i], "-assets") == 0) {
            ++i;
            assetsPath = argv[i];
        } else if (strcmp(argv[i], "--playing") == 0 || strcmp(argv[i], "-p") == 0) {
            if (i + 1 < argc) {
                startinplaytest = true;
                i++;
                playtestname = std::string("levels/");
                playtestname.append(argv[i]);
                playtestname.append(std::string(".vvvvvv"));
            } else {
                printf("--playing option requires one argument.\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--playx") == 0 ||
                strcmp(argv[i], "--playy") == 0 ||
                strcmp(argv[i], "--playrx") == 0 ||
                strcmp(argv[i], "--playry") == 0 ||
                strcmp(argv[i], "--playgc") == 0 ||
                strcmp(argv[i], "--playmusic") == 0) {
            if (i + 1 < argc) {
                savefileplaytest = true;
                auto v = std::atoi(argv[i+1]);
                if (strcmp(argv[i], "--playx") == 0) savex = v;
                else if (strcmp(argv[i], "--playy") == 0) savey = v;
                else if (strcmp(argv[i], "--playrx") == 0) saverx = v;
                else if (strcmp(argv[i], "--playry") == 0) savery = v;
                else if (strcmp(argv[i], "--playgc") == 0) savegc = v;
                else if (strcmp(argv[i], "--playmusic") == 0) savemusic = v;
                i++;
            } else {
                printf("--playing option requires one argument.\n");
                return 1;
            }
        }
        if (std::string(argv[i]) == "-renderer") {
            SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, argv[2], SDL_HINT_OVERRIDE);
        }
    }

    if(!FILESYSTEM_init(argv[0], baseDir, assetsPath))
    {
        return 1;
    }

    SDL_Init(
        SDL_INIT_VIDEO |
        SDL_INIT_AUDIO |
        SDL_INIT_JOYSTICK |
        SDL_INIT_GAMECONTROLLER
    );

    NETWORK_init();

    Screen gameScreen;

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



    music.init();
    game.init();
    game.infocus = true;

    graphics.MakeTileArray();
    graphics.MakeSpriteArray();
    graphics.maketelearray();


    graphics.images.push_back(graphics.grphx.im_image0);
    graphics.images.push_back(graphics.grphx.im_image1);
    graphics.images.push_back(graphics.grphx.im_image2);
    graphics.images.push_back(graphics.grphx.im_image3);
    graphics.images.push_back(graphics.grphx.im_image4);
    graphics.images.push_back(graphics.grphx.im_image5);
    graphics.images.push_back(graphics.grphx.im_image6);

    graphics.images.push_back(graphics.grphx.im_image7);
    graphics.images.push_back(graphics.grphx.im_image8);
    graphics.images.push_back(graphics.grphx.im_image9);
    graphics.images.push_back(graphics.grphx.im_image10);
    graphics.images.push_back(graphics.grphx.im_image11);
    graphics.images.push_back(graphics.grphx.im_image12);

    const SDL_PixelFormat* fmt = gameScreen.GetFormat();
    graphics.backBuffer = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 240, fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
    SDL_SetSurfaceBlendMode(graphics.backBuffer, SDL_BLENDMODE_NONE);
    graphics.footerbuffer = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 10, fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
    SDL_SetSurfaceBlendMode(graphics.footerbuffer, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(graphics.footerbuffer, 127);
    FillRect(graphics.footerbuffer, SDL_MapRGB(fmt, 0, 0, 0));
    graphics.Makebfont();


    graphics.foregroundBuffer =  SDL_CreateRGBSurface(SDL_SWSURFACE ,320 ,240 ,fmt->BitsPerPixel,fmt->Rmask,fmt->Gmask,fmt->Bmask,fmt->Amask  );
    SDL_SetSurfaceBlendMode(graphics.foregroundBuffer, SDL_BLENDMODE_NONE);

    graphics.screenbuffer = &gameScreen;

    graphics.menubuffer = SDL_CreateRGBSurface(SDL_SWSURFACE ,320 ,240 ,fmt->BitsPerPixel,fmt->Rmask,fmt->Gmask,fmt->Bmask,fmt->Amask );
    SDL_SetSurfaceBlendMode(graphics.menubuffer, SDL_BLENDMODE_NONE);

    graphics.towerbuffer =  SDL_CreateRGBSurface(SDL_SWSURFACE ,320 ,240 ,fmt->BitsPerPixel,fmt->Rmask,fmt->Gmask,fmt->Bmask,fmt->Amask  );
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

    if (startinplaytest) {
        game.levelpage=0;
        ed.getDirectoryData();
        game.loadcustomlevelstats();

        bool found = false;

        // search for the file in the vector
        for(size_t i = 0; i < ed.ListOfMetaData.size(); i++) {
            LevelMetaData currentmeta = ed.ListOfMetaData[i];
            if (currentmeta.filename == playtestname) {
                game.playcustomlevel = (int)i;
                found = true;
                break;
            }
        }
        if (!found) {
            printf("Level not found\n");
            return 1;
        }
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

    volatile Uint32 time, timePrev = 0;
    game.infocus = true;
    key.isActive = true;

    while(!key.quitProgram)
    {
        time = SDL_GetTicks();

        // Update network per frame.
        NETWORK_update();

        //framerate limit to 30
        Uint32 timetaken = time - timePrev;
        if(game.gamestate==EDITORMODE)
        {
            if (timetaken < 24)
            {
                volatile Uint32 delay = 24 - timetaken;
                SDL_Delay( delay );
                time = SDL_GetTicks();
            }
            timePrev = time;

        }else{
            if (timetaken < game.gameframerate)
            {
                volatile Uint32 delay = game.gameframerate - timetaken;
                SDL_Delay( delay );
                time = SDL_GetTicks();
            }
            timePrev = time;

        }



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
            game.press_left = false;
            game.press_right = false;
            game.press_action = true;
            game.press_map = false;
        }

        game.infocus = key.isActive;
        if(!game.infocus)
        {
            if(game.getGlobalSoundVol()> 0)
            {
                game.setGlobalSoundVol(0);
            }
            FillRect(graphics.backBuffer, 0x00000000);
            graphics.bprint(5, 110, "Game paused", 196 - help.glow, 255 - help.glow, 196 - help.glow, true);
            graphics.bprint(5, 120, "[click to resume]", 196 - help.glow, 255 - help.glow, 196 - help.glow, true);
            graphics.bprint(5, 230, "Press M to mute in game", 164 - help.glow, 196 - help.glow, 164 - help.glow, true);
            graphics.render();
            //We are minimised, so lets put a bit of a delay to save CPU
            SDL_Delay(100);
        }
        else
        {
            switch(game.gamestate)
            {
            case PRELOADER:
                //Render
                preloaderrender();
                break;
#if !defined(NO_CUSTOM_LEVELS)
            case EDITORMODE:
                graphics.flipmode = false;
                //Input
                editorinput();
                //Render
                editorrender();
                ////Logic
                editorlogic();
                break;
#endif
            case TITLEMODE:
                //Input
                titleinput();
                //Render
                titlerender();
                ////Logic
                titlelogic();
                break;
            case GAMEMODE:
                if (map.towermode)
                {
                    gameinput();
                    towerrender();
                    towerlogic();

                }
                else
                {

                    if (script.running)
                    {
                        script.run();
                    }

                    gameinput();
                    gamerender();
                    gamelogic();


                    break;
                case MAPMODE:
                    maprender();
                    mapinput();
                    maplogic();
                    break;
                case TELEPORTERMODE:
                    teleporterrender();
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
                    gamecompleterender();
                    //Input
                    gamecompleteinput();
                    //Logic
                    gamecompletelogic();
                    break;
                case GAMECOMPLETE2:
                    gamecompleterender2();
                    //Input
                    gamecompleteinput2();
                    //Logic
                    gamecompletelogic2();
                    break;
                case CLICKTOSTART:
                    help.updateglow();
                    break;
                default:

                break;
                }

            }

        }

        //We did editorinput, now it's safe to turn this off
        key.linealreadyemptykludge = false;

        if (game.savemystats)
        {
            game.savemystats = false;
            game.savestats();
        }

        //Mute button
#if !defined(NO_CUSTOM_LEVELS)
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

        if (game.muted)
        {
            game.globalsound = 0;
            Mix_VolumeMusic(0) ;
            Mix_Volume(-1,0);
        }

        if (!game.muted && game.globalsound == 0)
        {
            game.globalsound = 1;
            Mix_VolumeMusic(MIX_MAX_VOLUME) ;
            Mix_Volume(-1,MIX_MAX_VOLUME);
        }

        if (key.resetWindow)
        {
            key.resetWindow = false;
            gameScreen.ResizeScreen(-1, -1);
        }

        music.processmusic();
        graphics.processfade();
        game.gameclock();
        gameScreen.FlipScreen();
    }

    game.savestats();
    NETWORK_shutdown();
    SDL_Quit();
    FILESYSTEM_deinit();

    return 0;
}
