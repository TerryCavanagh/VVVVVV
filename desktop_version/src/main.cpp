#include <SDL.h>
#include "SoundSystem.h"

#include "UtilityClass.h"
#include "Game.h"
#include "Graphics.h"
#include "KeyPoll.h"
#include "titlerender.h"

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

#if !defined(NO_EDITOR)
	edentities edentity[3000];
	editorclass ed;
#endif

UtilityClass help;
Graphics graphics;
musicclass music;
Game game;
KeyPoll key;
mapclass map;
entityclass obj;

int main(int argc, char *argv[])
{
    SDL_Init(
        SDL_INIT_VIDEO |
        SDL_INIT_AUDIO |
        SDL_INIT_JOYSTICK |
        SDL_INIT_GAMECONTROLLER
    );

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
        }
    }

    if(!FILESYSTEM_init(argv[0], baseDir, assetsPath))
    {
        return 1;
    }

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




    //UtilityClass help;
    // Load Ini


    //Graphics graphics;
    graphics.init();



    //musicclass music;
    music.init();
    //Game game;
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
    graphics.backBuffer = SDL_CreateRGBSurface(SDL_SWSURFACE ,320 ,240 ,32,fmt->Rmask,fmt->Gmask,fmt->Bmask,fmt->Amask ) ;
    SDL_SetSurfaceBlendMode(graphics.backBuffer, SDL_BLENDMODE_NONE);
    graphics.footerbuffer = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 10, 32, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
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

    //Make a temporary rectangle to hold the offsets
    // SDL_Rect offset;
    //Give the offsets to the rectangle
    // offset.x = 60;
    // offset.y = 80;

    //game.gamestate = TITLEMODE;
    //game.gamestate=EDITORMODE;
    game.gamestate = PRELOADER; //Remember to uncomment this later!

    game.menustart = false;
    game.mainmenu = 0;

    //KeyPoll key;
    //mapclass map;

    map.ypos = (700-29) * 8;
    map.bypos = map.ypos / 2;

    //Moved screensetting init here from main menu V2.1
    game.loadstats(map, graphics);
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

    //entityclass obj;
    obj.init();

    //Quick hack to start in final level ---- //Might be useful to leave this commented in for testing
    /*
    //game.gamestate=GAMEMODE;
		//game.start(obj,music);
		//script.startgamemode(8, key, graphics, game, map, obj, help, music);
   // map.finalmode = true; //Enable final level mode
		//map.finalx = 41; map.finaly = 52; //Midpoint
		//map.finalstretch = true;
		//map.final_colormode = true;
		//map.final_mapcol = 0;
		//map.final_colorframe = 0;

		//game.starttest(obj, music);

    game.savex = 5 * 8; game.savey = 15 * 8; game.saverx = 41; game.savery = 52;
    game.savegc = 0; game.savedir = 1;
    game.state = 0; game.deathseq = -1; game.lifeseq = 10;
		//obj.createentity(game, game.savex, game.savey, 0);
		map.gotoroom(game.saverx, game.savery, graphics, game, obj, music);
		//music.play(1);
		*/
    //End hack here ----

    volatile Uint32 time, timePrev = 0;
    game.infocus = true;
    key.isActive = true;

    while(!key.quitProgram)
    {
		//gameScreen.ClearScreen(0x00);

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
				//SDL_WM_GrabInput(SDL_GRAB_ON);
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
			printf("Error: failed: %s\n", SDL_GetError());




		}
		/*if(key.quitProgram)
		{
			music.playef(2);
		}*/

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
                preloaderrender(graphics, game, help);
                break;
        #if !defined(NO_EDITOR)
            case EDITORMODE:
				graphics.flipmode = false;
                //Input
                editorinput(key, graphics, game, map, obj, help, music);
                //Render
                editorrender(key, graphics, game, map, obj, help);
                ////Logic
                editorlogic(key, graphics, game, obj, music, map, help);
                break;
        #endif
            case TITLEMODE:
                //Input
                titleinput(key, graphics, map, game, obj, help, music);
                //Render
                titlerender(graphics, map, game, obj, help, music);
                ////Logic
                titlelogic(graphics, game, obj, help, music, map);
                break;
            case GAMEMODE:
                if (map.towermode)
                {
					gameinput(key, graphics, game, map, obj, help, music);

                    //if(game.recording==1)
                    //{
                    // ///recordinput(key, graphics, game, map, obj, help, music);
                    //}
                    //else
                    //{
                    //}
                    towerrender(graphics, game, map, obj, help);
                    towerlogic(graphics, game,  obj,  music, map, help);

                }
                else
                {

                    if (game.recording == 1)
                    {
                        //recordinput(key, dwgfx, game, map, obj, help, music);
                    }
                    else
                    {
                        if (script.running)
                        {
                            script.run(key, graphics, game, map, obj, help, music);
                        }

                        gameinput(key, graphics, game, map, obj, help, music);
                        //}
                        gamerender(graphics,map, game,  obj, help);
                        gamelogic(graphics, game,obj, music, map,  help);


                    }
                    break;
                case MAPMODE:
                    maprender(graphics, game, map, obj, help);
                    if (game.recording == 1)
                    {
                        //recordinput(key, dwgfx, game, map, obj, help, music); //will implement this later if it's actually needed
                    }
                    else
                    {
                        mapinput(key, graphics, game, map, obj, help, music);
                    }
                    maplogic(graphics, game, obj ,music , map, help );
                    break;
                case TELEPORTERMODE:
                    teleporterrender(graphics, game, map, obj, help);
                    if (game.recording == 1)
                    {
                        //recordinput(key, graphics, game, map, obj, help, music);
                    }
                    else
                    {
                        if(game.useteleporter)
                        {
                            teleporterinput(key, graphics, game, map, obj, help, music);
                        }
                        else
                        {
                            if (script.running)
                            {
                                script.run(key, graphics, game, map, obj, help, music);
                            }
                            gameinput(key, graphics, game, map, obj, help, music);
                        }
                    }
                    maplogic(graphics, game,  obj, music, map, help);
                    break;
                case GAMECOMPLETE:
                    gamecompleterender(graphics, game, obj, help, map);
                    //Input
                    gamecompleteinput(key, graphics, game, map, obj, help, music);
                    //Logic
                    gamecompletelogic(graphics, game,  obj, music, map, help);
                    break;
                case GAMECOMPLETE2:
                    gamecompleterender2(graphics, game, obj, help);
                    //Input
                    gamecompleteinput2(key, graphics, game, map, obj, help, music);
                    //Logic
                    gamecompletelogic2(graphics, game,  obj, music, map, help);
                    break;
                case CLICKTOSTART:

                    //dwgfx.bprint(5, 115, "[Click to start]", 196 - help.glow, 196 - help.glow, 255 - help.glow, true);
                    //dwgfx.drawgui(help);
                    //dwgfx.render();
                    //dwgfx.backbuffer.unlock();

                    help.updateglow();
                    // if (key.click) {
                    //  dwgfx.textboxremove();
                    // }
                    // if (dwgfx.ntextbox == 0) {
                    //  //music.play(6);
                    //  map.ypos = (700-29) * 8;
                    //  map.bypos = map.ypos / 2;
                    //  map.cameramode = 0;

                    //  game.gamestate = TITLEMODE;
                    // }
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
            game.savestats(map, graphics);
        }

        //Mute button
    #if !defined(NO_EDITOR)
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
            //if (game.globalsound == 1)
            //{
                game.globalsound = 0;
                Mix_VolumeMusic(0) ;
                Mix_Volume(-1,0);
            //}
        }

        if (!game.muted && game.globalsound == 0)
        {
            game.globalsound = 1;
            Mix_VolumeMusic(MIX_MAX_VOLUME) ;
            Mix_Volume(-1,MIX_MAX_VOLUME);
        }

		if(key.resetWindow)
		{
			key.resetWindow = false;
			gameScreen.ResizeScreen(-1, -1);
		}

        music.processmusic();
        graphics.processfade();
        game.gameclock();
        gameScreen.FlipScreen();

        //SDL_FillRect( SDL_GetVideoSurface(), NULL, 0 );
    }


	  //SDL_Delay(300);

    //TODO
    //Free the loaded image
    //SDL_FreeSurface( gameScreen );

    //Quit SDL
    NETWORK_shutdown();
    SDL_Quit();
    FILESYSTEM_deinit();

    return 0;
}
