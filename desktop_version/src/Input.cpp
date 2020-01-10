#include "Input.h"

#include "MakeAndPlay.h"

#include "tinyxml.h"

// Found in titlerender.cpp
void updategraphicsmode(Game& game, Graphics& dwgfx);

void updatebuttonmappings(Game& game, KeyPoll& key, musicclass& music, int bind)
{
	for (
		SDL_GameControllerButton i = SDL_CONTROLLER_BUTTON_A;
		i < SDL_CONTROLLER_BUTTON_DPAD_UP;
		i = (SDL_GameControllerButton) (i + 1)
	) {
		if (key.isDown(i))
		{
			bool dupe = false;
			if (bind == 1)
			{
				for (size_t j = 0; j < game.controllerButton_flip.size(); j += 1)
				{
					if (i == game.controllerButton_flip[j])
					{
						dupe = true;
					}
				}
				if (!dupe)
				{
					game.controllerButton_flip.push_back(i);
					music.playef(11, 10);
				}
				for (size_t j = 0; j < game.controllerButton_map.size(); j += 1)
				{
					if (i == game.controllerButton_map[j])
					{
						game.controllerButton_map.erase(game.controllerButton_map.begin() + j);
					}
				}
				for (size_t j = 0; j < game.controllerButton_esc.size(); j += 1)
				{
					if (i == game.controllerButton_esc[j])
					{
						game.controllerButton_esc.erase(game.controllerButton_esc.begin() + j);
					}
				}
			}
			if (bind == 2)
			{
				for (size_t j = 0; j < game.controllerButton_map.size(); j += 1)
				{
					if (i == game.controllerButton_map[j])
					{
						dupe = true;
					}
				}
				if (!dupe)
				{
					game.controllerButton_map.push_back(i);
					music.playef(11, 10);
				}
				for (size_t j = 0; j < game.controllerButton_flip.size(); j += 1)
				{
					if (i == game.controllerButton_flip[j])
					{
						game.controllerButton_flip.erase(game.controllerButton_flip.begin() + j);
					}
				}
				for (size_t j = 0; j < game.controllerButton_esc.size(); j += 1)
				{
					if (i == game.controllerButton_esc[j])
					{
						game.controllerButton_esc.erase(game.controllerButton_esc.begin() + j);
					}
				}
			}
			if (bind == 3)
			{
				for (size_t j = 0; j < game.controllerButton_esc.size(); j += 1)
				{
					if (i == game.controllerButton_esc[j])
					{
						dupe = true;
					}
				}
				if (!dupe)
				{
					game.controllerButton_esc.push_back(i);
					music.playef(11, 10);
				}
				for (size_t j = 0; j < game.controllerButton_flip.size(); j += 1)
				{
					if (i == game.controllerButton_flip[j])
					{
						game.controllerButton_flip.erase(game.controllerButton_flip.begin() + j);
					}
				}
				for (size_t j = 0; j < game.controllerButton_map.size(); j += 1)
				{
					if (i == game.controllerButton_map[j])
					{
						game.controllerButton_map.erase(game.controllerButton_map.begin() + j);
					}
				}
			}
		}
	}
}

void titleinput(KeyPoll& key, Graphics& dwgfx, mapclass& map, Game& game, entityclass& obj, UtilityClass& help, musicclass& music)
{
    //game.mx = (mouseX / 4);
    //game.my = (mouseY / 4);

	//TODO bit wasteful doing this every poll
	key.setSensitivity(game.controllerSensitivity);

    game.press_left = false;
    game.press_right = false;
    game.press_action = false;
    game.press_map = false;

    if (dwgfx.flipmode)
    {
		//GAMEPAD TODO
        if (key.isDown(KEYBOARD_LEFT) || key.isDown(KEYBOARD_DOWN) || key.isDown(KEYBOARD_a) ||  key.isDown(KEYBOARD_s) || key.controllerWantsRight(true)) game.press_left = true;
        if (key.isDown(KEYBOARD_RIGHT) || key.isDown(KEYBOARD_UP)  || key.isDown(KEYBOARD_d) ||  key.isDown(KEYBOARD_w) || key.controllerWantsLeft(true)) game.press_right = true;
    }
    else
    {
        if (key.isDown(KEYBOARD_LEFT) || key.isDown(KEYBOARD_UP) || key.isDown(KEYBOARD_a) ||  key.isDown(KEYBOARD_w) || key.controllerWantsLeft(true))
        {
            game.press_left = true;
        }
        if (key.isDown(KEYBOARD_RIGHT) || key.isDown(KEYBOARD_DOWN)  || key.isDown(KEYBOARD_d) ||  key.isDown(KEYBOARD_s) || key.controllerWantsRight(true))
        {
            game.press_right = true;
		}
    }
    if (key.isDown(KEYBOARD_z) || key.isDown(KEYBOARD_SPACE) || key.isDown(KEYBOARD_v) || key.isDown(game.controllerButton_flip)) game.press_action = true;
    //|| key.isDown(KEYBOARD_UP) || key.isDown(KEYBOARD_DOWN)) game.press_action = true; //on menus, up and down don't work as action
    if (key.isDown(KEYBOARD_ENTER)) game.press_map = true;

    //In the menu system, all keypresses are single taps rather than holds. Therefore this test has to be done for all presses
    if (!game.press_action && !game.press_left && !game.press_right) game.jumpheld = false;
    if (!game.press_map) game.mapheld = false;

    if (!game.jumpheld && dwgfx.fademode==0)
    {
        if (game.press_action || game.press_left || game.press_right || game.press_map)
        {
            game.jumpheld = true;
        }

        /*
        if (game.press_left) {
        game.mainmenu--;
        }else if (game.press_right) {
        game.mainmenu++;
        }

        if (game.mainmenu < 0) game.mainmenu = 2;
        if (game.mainmenu > 2 ) game.mainmenu = 0;
        */


        /*
        if (game.press_action) {
        if (!game.menustart) {
        game.menustart = true;
        music.play(6);
        music.playef(18, 10);
        game.screenshake = 10;
        game.flashlight = 5;
        }else{
        if(game.mainmenu==0){
        dwgfx.fademode = 2;
        }else if (game.mainmenu == 1) {
        if (game.telesummary != "") {
        dwgfx.fademode = 2;
        }
        }else if (game.mainmenu == 2) {
        if (game.quicksummary != "") {
        dwgfx.fademode = 2;
        }
        }
        }
        }
        */

        if (key.isDown(27) && game.currentmenuname != "youwannaquit" && game.menustart)
        {
            music.playef(11, 10);
            game.previousmenuname = game.currentmenuname;
            game.createmenu("youwannaquit");
            map.nexttowercolour();
        }

        if(game.menustart)
        {
            if (game.press_left)
            {
                game.currentmenuoption--;
            }
            else if (game.press_right)
            {
                game.currentmenuoption++;
            }
        }

        if (game.currentmenuoption < 0) game.currentmenuoption = game.nummenuoptions-1;
        if (game.currentmenuoption >= game.nummenuoptions ) game.currentmenuoption = 0;

        if (game.press_action)
        {
            if (!game.menustart)
            {
                game.menustart = true;
                music.play(6);
                music.playef(18, 10);
                game.screenshake = 10;
                game.flashlight = 5;
                map.colstate = 10;
                map.nexttowercolour();
            }
            else
            {
                if (game.currentmenuname == "mainmenu")
                {
								
				#if defined(MAKEANDPLAY)
				   if (game.currentmenuoption == 0)
                    {
                      //Bring you to the normal playmenu
                        music.playef(11, 10);
                        game.createmenu("playerworlds");
                        map.nexttowercolour();
                    }
                    else if (game.currentmenuoption == 1)
                    {
                        //Options
                        music.playef(11, 10);
                        game.createmenu("graphicoptions");
                        map.nexttowercolour();
                    }
                    else if (game.currentmenuoption == 2)
                    {
                        //Options
                        music.playef(11, 10);
                        game.createmenu("options");

												//Add extra menu for mmmmmm mod
												if(music.mmmmmm){
													game.menuoptions[3] = "soundtrack";
													game.menuoptionsactive[3] = true;
													game.menuoptions[4] = "return";
													game.menuoptionsactive[4] = true;
													game.nummenuoptions = 5;
												}
                        map.nexttowercolour();
                    }
                    else if (game.currentmenuoption == 3)
                    {
                        //bye!
                        music.playef(2, 10);
                        game.mainmenu = 100;
                        dwgfx.fademode = 2;
                    }
                }
				#elif !defined(MAKEANDPLAY)
                    if (game.currentmenuoption == 0)
                    {
                        //Play
                        if (game.telesummary == "" && game.quicksummary == "")
                        {
                            //No saves exist, just start a new game
                            game.mainmenu = 0;
                            dwgfx.fademode = 2;
                        }
                        else
                        {
                            //Bring you to the normal playmenu
                            music.playef(11, 10);
                            game.createmenu("play");
                            map.nexttowercolour();
                        }
                    }
                    else if (game.currentmenuoption == 1)
                    {
                      //Bring you to the normal playmenu
                        music.playef(11, 10);
                        game.createmenu("playerworlds");
                        map.nexttowercolour();
                    }
                    else if (game.currentmenuoption == 2)
                    {
                        //Options
                        music.playef(11, 10);
                        game.createmenu("graphicoptions");
                        map.nexttowercolour();
                    }
                    else if (game.currentmenuoption == 3)
                    {
                        //Options
                        music.playef(11, 10);
                        game.createmenu("options");

												//Add extra menu for mmmmmm mod
											  if(music.mmmmmm){
													game.menuoptions[4] = "soundtrack";
													game.menuoptionsactive[4] = true;
													game.menuoptions[5] = "return";
													game.menuoptionsactive[5] = true;
													game.nummenuoptions = 6;
												}
                        map.nexttowercolour();
                    }
                    else if (game.currentmenuoption == 4)
                    {
                        //Credits
                        music.playef(11, 10);
                        game.createmenu("credits");
                        map.nexttowercolour();
                    }
                    else if (game.currentmenuoption == 5)
                    {
                        //bye!
                        music.playef(2, 10);
                        game.mainmenu = 100;
                        dwgfx.fademode = 2;
                    }
                }
								#endif
                else if(game.currentmenuname=="levellist")
                {
                  if(game.currentmenuoption==game.nummenuoptions-1){
                    //go back to menu
                    music.playef(11, 10);
                    game.createmenu("mainmenu");
                    map.nexttowercolour();
                  }else if(game.currentmenuoption==game.nummenuoptions-2){
                    //next page
                    music.playef(11, 10);
                    if((size_t) ((game.levelpage*8)+8) >= ed.ListOfMetaData.size()){
                      game.levelpage=0;
                    }else{
                      game.levelpage++;
                    }
                    game.createmenu("levellist");
                    game.currentmenuoption=game.nummenuoptions-2;
                    map.nexttowercolour();
                  }else{
                    //Ok, launch the level!
                    //PLAY CUSTOM LEVEL HOOK
                    music.playef(11, 10);
                    game.playcustomlevel=(game.levelpage*8)+game.currentmenuoption;
                    game.customleveltitle=ed.ListOfMetaData[game.playcustomlevel].title;
                    game.customlevelfilename=ed.ListOfMetaData[game.playcustomlevel].filename;

                    std::string name = game.saveFilePath + ed.ListOfMetaData[game.playcustomlevel].filename.substr(7) + ".vvv";
                    TiXmlDocument doc(name.c_str());
	                  if (!doc.LoadFile()){
	                    game.mainmenu = 22;
                      dwgfx.fademode = 2;
	                  }else{
                      music.playef(11, 10);
                      game.createmenu("quickloadlevel");
                      map.nexttowercolour();
	                  }
                  }
                }
                else if(game.currentmenuname=="quickloadlevel")
                {
                  if(game.currentmenuoption==0){//continue save
                    game.mainmenu = 23;
                    dwgfx.fademode = 2;
                  }else if(game.currentmenuoption==1){
	                  game.mainmenu = 22;
                    dwgfx.fademode = 2;
                  }else if(game.currentmenuoption==2){
                    music.playef(11, 10);
                    game.levelpage=0;
                    game.createmenu("levellist");
                    map.nexttowercolour();
                  }
                }
                else if(game.currentmenuname=="playerworlds")
                {
                  if(game.currentmenuoption==0){

                    music.playef(11, 10);
                    game.levelpage=0;
                    ed.getDirectoryData();
                    game.loadcustomlevelstats(); //Should only load a file if it's needed
                    game.createmenu("levellist");
                    map.nexttowercolour();
                  }else if(game.currentmenuoption==1){
                    //LEVEL EDITOR HOOK
                    music.playef(11, 10);
                    game.mainmenu = 20;
                    dwgfx.fademode = 2;
                    ed.filename="";
                  }/*else if(game.currentmenuoption==2){
                    music.playef(11, 10);
                    //"OPENFOLDERHOOK"
                    //When the player selects the "open level folder" menu option,
                    //this is where it should run the appropriate code.
                    //This code should:
                    // - Minimise the game
                    // - Open the levels folder for whatever operating system we're on
SDL_assert(0 && "Remove open level dir");

                  }*/else if(game.currentmenuoption==2){
                    //back
                    music.playef(11, 10);
                    game.createmenu("mainmenu");
                    map.nexttowercolour();
                  }
                }
                else if(game.currentmenuname=="errornostart"){
                  music.playef(11, 10);
                  game.createmenu("mainmenu");
                  map.nexttowercolour();
                }
                else if (game.currentmenuname == "graphicoptions")
                {
                  if (game.currentmenuoption == 0){
                    music.playef(11, 10);
                    dwgfx.screenbuffer->toggleFullScreen();
                    music.playef(11, 10);
                    game.fullscreen = !game.fullscreen;
                    updategraphicsmode(game, dwgfx);
                    game.savestats(map, dwgfx);
                    game.createmenu("graphicoptions");
                    game.currentmenuoption = 0;
                  }else if (game.currentmenuoption == 1){
                    music.playef(11, 10);
                    dwgfx.screenbuffer->toggleVsync();
                    music.playef(11, 10);
                    game.vsync = !game.vsync;
                    updategraphicsmode(game, dwgfx);
                    game.savestats(map, dwgfx);
                    game.createmenu("graphicoptions");
                    game.currentmenuoption = 1;
                  }else if(game.currentmenuoption == 2){
                    music.playef(11, 10);
                    dwgfx.screenbuffer->toggleStretchMode();
                    music.playef(11, 10);
                    game.stretchMode = (game.stretchMode + 1) % 3;
                    updategraphicsmode(game, dwgfx);
                    game.savestats(map, dwgfx);
                    game.createmenu("graphicoptions");
                    game.currentmenuoption = 2;
                  }else if (game.currentmenuoption == 3){
                    music.playef(11, 10);
                    dwgfx.screenbuffer->toggleLinearFilter();
                    music.playef(11, 10);
                    game.useLinearFilter = !game.useLinearFilter;
                    updategraphicsmode(game, dwgfx);
                    game.savestats(map, dwgfx);
                    game.createmenu("graphicoptions");
                    game.currentmenuoption = 3;
                  }else if (game.currentmenuoption == 4){
                      //change smoothing
                      music.playef(11, 10);
                      game.fullScreenEffect_badSignal = !game.fullScreenEffect_badSignal;
                      //Hook the analogue thing in here: ABCDEFG
                      updategraphicsmode(game, dwgfx);
					  dwgfx.screenbuffer->badSignalEffect= !dwgfx.screenbuffer->badSignalEffect;
                      game.savestats(map, dwgfx);
                      game.createmenu("graphicoptions");
                      game.currentmenuoption = 4;
                  }
                  else
                  {
                      //back
                      music.playef(11, 10);
                      game.createmenu("mainmenu");
                      map.nexttowercolour();
                  }

                  /* //Old stuff
                    if (game.advanced_mode)
                    {
                        if (game.currentmenuoption == 0)
                        {
                            //toggle fullscreen
                            dwgfx.screenbuffer->toggleFullScreen();
                            music.playef(11, 10);
                            if (game.fullscreen)
                            {
                                game.fullscreen = false;
                            }
                            else
                            {
                                game.fullscreen = true;
                            }
                            updategraphicsmode(game, dwgfx);
                            game.savestats(map, dwgfx);
                            game.createmenu("graphicoptions");
                        }
                        else if (game.currentmenuoption == 1)
                        {
                            //enable acceleration: if in fullscreen, go back to window first
                            music.playef(11, 10);
                            game.advanced_mode = false;
                            if (game.fullscreen)
                            {
                                game.fullscreen = false;
                                updategraphicsmode(game, dwgfx);
                                game.fullscreen = true;
                            }
                            updategraphicsmode(game, dwgfx);

                            game.savestats(map, dwgfx);
                            game.createmenu("graphicoptions");
                            game.currentmenuoption = 1;
                        }
                        else if (game.currentmenuoption == 2)
                        {
                            //change scaling mode
                            music.playef(11, 10);
                            game.advanced_scaling = (game.advanced_scaling + 1) % 5;
                            dwgfx.screenbuffer->ResizeScreen(320 *game.advanced_scaling,240*game.advanced_scaling );
                            dwgfx.screenbuffer->SetScale(game.advanced_scaling);
                            updategraphicsmode(game, dwgfx);

                            game.savestats(map, dwgfx);
                            game.createmenu("graphicoptions");
                            game.currentmenuoption = 2;
                        }
                        else if (game.currentmenuoption == 3)
                        {
                            //change smoothing
                            music.playef(11, 10);
                            game.advanced_smoothing = !game.advanced_smoothing;
                            updategraphicsmode(game, dwgfx);

                            game.savestats(map, dwgfx);
                            game.createmenu("graphicoptions");
                            game.currentmenuoption = 3;
                        }
                        else
                        {
                            //back
                            music.playef(11, 10);
                            game.createmenu("mainmenu");
                            map.nexttowercolour();
                        }
                    }
                    else
                    {
                        if (game.currentmenuoption == 0)
                        {
                            dwgfx.screenbuffer->toggleFullScreen();
                            //toggle fullscreen
                            music.playef(11, 10);
                            if (game.fullscreen)
                            {
                                game.fullscreen = false;
                            }
                            else
                            {
                                game.fullscreen = true;
                            }
                            updategraphicsmode(game, dwgfx);

                            game.savestats(map, dwgfx);
                            game.createmenu("graphicoptions");
                        }
                        else if (game.currentmenuoption == 1)
                        {
                            //disable acceleration: if in fullscreen, go back to window first
                            music.playef(11, 10);
                            game.advanced_mode = true;
                            if (game.fullscreen)
                            {
                                game.fullscreen = false;
                                updategraphicsmode(game, dwgfx);
                                game.fullscreen = true;
                            }
                            updategraphicsmode(game, dwgfx);

                            game.savestats(map, dwgfx);
                            game.createmenu("graphicoptions");
                            game.currentmenuoption = 1;
                        }
                        else
                        {
                            //back
                            music.playef(11, 10);
                            game.createmenu("mainmenu");
                            map.nexttowercolour();
                        }
                    }
                    */
                }
                else if (game.currentmenuname == "youwannaquit")
                {
                    if (game.currentmenuoption == 0)
                    {
                        //bye!
                        music.playef(2, 10);
                        game.mainmenu = 100;
                        dwgfx.fademode = 2;
                    }
                    else
                    {
                        music.playef(11, 10);
                        game.createmenu(game.previousmenuname);
                        map.nexttowercolour();
                    }
                }
                else if (game.currentmenuname == "setinvincibility")
                {
                    if (game.currentmenuoption == 0)
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("accessibility");
                        game.currentmenuoption = 2;
                        map.nexttowercolour();
                    }
                    else
                    {
                        map.invincibility = !map.invincibility;
                        //game.deletequick();
                        //game.deletetele();
                        game.savestats(map, dwgfx);
                        music.playef(11, 10);
                        game.createmenu("accessibility");
                        game.currentmenuoption = 2;
                        map.nexttowercolour();
                    }
                }
                else if (game.currentmenuname == "setslowdown1")
                {
                    if (game.currentmenuoption == 0)
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("accessibility");
                        game.currentmenuoption = 3;
                        map.nexttowercolour();
                    }
                    else
                    {
                        //change game speed
                        game.deletequick();
                        game.deletetele();
                        game.createmenu("setslowdown2");
                        map.nexttowercolour();
                        music.playef(11, 10);
                    }
                }
                else if (game.currentmenuname == "setslowdown2")
                {
                    if (game.currentmenuoption == 0)
                    {
                        //back
                        game.gameframerate=34;
                        game.slowdown = 30;
                        game.savestats(map, dwgfx);
                        music.playef(11, 10);
                        game.createmenu("accessibility");
                        game.currentmenuoption = 3;
                        map.nexttowercolour();
                    }
                    else if (game.currentmenuoption == 1)
                    {
                        game.gameframerate=41;
                        game.slowdown = 24;
                        game.savestats(map, dwgfx);
                        music.playef(11, 10);
                        game.createmenu("accessibility");
                        game.currentmenuoption = 3;
                        map.nexttowercolour();
                    }
                    else if (game.currentmenuoption == 2)
                    {
                        game.gameframerate=55;
                        game.slowdown = 18;
                        game.savestats(map, dwgfx);
                        music.playef(11, 10);
                        game.createmenu("accessibility");
                        game.currentmenuoption = 3;
                        map.nexttowercolour();
                    }
                    else if (game.currentmenuoption == 3)
                    {
                        game.gameframerate=83;
                        game.slowdown = 12;
                        game.savestats(map, dwgfx);
                        music.playef(11, 10);
                        game.createmenu("accessibility");
                        game.currentmenuoption = 3;
                        map.nexttowercolour();
                    }
                }
                else if (game.currentmenuname == "accessibility")
                {
                    if (game.currentmenuoption == 0)
                    {
                        //disable animated backgrounds
                        game.colourblindmode = !game.colourblindmode;
                        game.savestats(map, dwgfx);
                        map.tdrawback = true;
                        music.playef(11, 10);
                    }
                    else if (game.currentmenuoption == 1)
                    {
                        //disable screeneffects
                        game.noflashingmode = !game.noflashingmode;
                        game.savestats(map, dwgfx);
                        if (!game.noflashingmode)
                        {
                            music.playef(18, 10);
                            game.screenshake = 10;
                            game.flashlight = 5;
                        }
                    }
                    else if (game.currentmenuoption == 2)
                    {
                        //invincibility
                        if (!map.invincibility)
                        {
                            game.createmenu("setinvincibility");
                            map.nexttowercolour();
                        }
                        else
                        {
                            map.invincibility = !map.invincibility;
                        }
                        music.playef(11, 10);
                    }
                    else if (game.currentmenuoption == 3)
                    {
                        //change game speed
                        game.createmenu("setslowdown2");
                        map.nexttowercolour();
                        music.playef(11, 10);
                    }
                    else if (game.currentmenuoption == 4)
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("options");

												//Add extra menu for mmmmmm mod
												if(music.mmmmmm){
													#if defined(MAKEANDPLAY)
														game.menuoptions[3] = "soundtrack";
														game.menuoptionsactive[3] = true;
														game.menuoptions[4] = "return";
														game.menuoptionsactive[4] = true;
														game.nummenuoptions = 5;
													#elif !defined(MAKEANDPLAY)
														game.menuoptions[4] = "soundtrack";
														game.menuoptionsactive[4] = true;
														game.menuoptions[5] = "return";
														game.menuoptionsactive[5] = true;
														game.nummenuoptions = 6;
													#endif
												}

                        map.nexttowercolour();
                    }
                    else
                    {
                        //Can't do yet! play sad sound
                        music.playef(2, 10);
                    }
                }
                else if (game.currentmenuname == "options")
                {
								
				#if defined(MAKEANDPLAY)
				if (game.currentmenuoption == 0)
                    {
                        //accessibility options
                        music.playef(11, 10);
                        game.createmenu("accessibility");
                        map.nexttowercolour();
                    }
                   
					else if (game.currentmenuoption == 1)
					{
						//clear data menu
						music.playef(11, 10);
						game.createmenu("controller");
						map.nexttowercolour();
					}
                    else if (game.currentmenuoption == 2)
                    {
                        //clear data menu
                        music.playef(11, 10);
                        game.createmenu("cleardatamenu");
                        map.nexttowercolour();
                    }

										if(music.mmmmmm){
											if (game.currentmenuoption == 3)
											{
													//**** TOGGLE MMMMMM
													if(game.usingmmmmmm > 0){
														game.usingmmmmmm=0;
													}else{
														game.usingmmmmmm=1;
													}
													music.usingmmmmmm = !music.usingmmmmmm;
													music.playef(11, 10);
													music.play(6);
													game.savestats(map, dwgfx);
													game.createmenu("mainmenu");
													map.nexttowercolour();
											}
											if (game.currentmenuoption == 4)
											{
													//back
													music.playef(11, 10);
													game.createmenu("mainmenu");
													map.nexttowercolour();
											}
										}else{
											if (game.currentmenuoption == 3)
											{
													//back
													music.playef(11, 10);
													game.createmenu("mainmenu");
													map.nexttowercolour();
											}
										}
                    
				#elif !defined(MAKEANDPLAY)
                    if (game.currentmenuoption == 0)
                    {
                        //accessibility options
                        music.playef(11, 10);
                        game.createmenu("accessibility");
                        map.nexttowercolour();
                    }
                    else if (game.currentmenuoption == 1)
                    {
                        //unlock play options
                        music.playef(11, 10);
                        game.createmenu("unlockmenu");
                        map.nexttowercolour();
                    }
					else if (game.currentmenuoption == 2)
					{
						//clear data menu
						music.playef(11, 10);
						game.createmenu("controller");
						map.nexttowercolour();
					}
                    else if (game.currentmenuoption == 3)
                    {
                        //clear data menu
                        music.playef(11, 10);
                        game.createmenu("cleardatamenu");
                        map.nexttowercolour();
                    }
                    
										if(music.mmmmmm){
											if (game.currentmenuoption == 4)
											{
													//**** TOGGLE MMMMMM
													if(game.usingmmmmmm > 0){
														game.usingmmmmmm=0;
													}else{
														game.usingmmmmmm=1;
													}
													music.usingmmmmmm = !music.usingmmmmmm;
													music.playef(11, 10);
													music.play(6);
													game.savestats(map, dwgfx);
													game.createmenu("mainmenu");
													map.nexttowercolour();
											}
											if (game.currentmenuoption == 5)
											{
													//back
													music.playef(11, 10);
													game.createmenu("mainmenu");
													map.nexttowercolour();
											}
										}else{
											if (game.currentmenuoption == 4)
											{
													//back
													music.playef(11, 10);
													game.createmenu("mainmenu");
													map.nexttowercolour();
											}
										}
										#endif
                }
                else if (game.currentmenuname == "unlockmenutrials")
                {
                    if (game.currentmenuoption == 0)   	//unlock 1
                    {
                        game.unlock[9] = true;
                        game.unlocknotify[9] = true;
                        music.playef(11, 10);
                        game.savestats(map, dwgfx);
                        game.createmenu("unlockmenutrials");
                        game.currentmenuoption = 0;
                    }
                    else if (game.currentmenuoption == 1)   	//unlock 2
                    {
                        game.unlock[10] = true;
                        game.unlocknotify[10] = true;
                        music.playef(11, 10);
                        game.savestats(map, dwgfx);
                        game.createmenu("unlockmenutrials");
                        game.currentmenuoption = 1;
                    }
                    else if (game.currentmenuoption == 2)   	//unlock 3
                    {
                        game.unlock[11] = true;
                        game.unlocknotify[11] = true;
                        music.playef(11, 10);
                        game.savestats(map, dwgfx);
                        game.createmenu("unlockmenutrials");
                        game.currentmenuoption = 2;
                    }
                    else if (game.currentmenuoption == 3)   	//unlock 4
                    {
                        game.unlock[12] = true;
                        game.unlocknotify[12] = true;
                        music.playef(11, 10);
                        game.savestats(map, dwgfx);
                        game.createmenu("unlockmenutrials");
                        game.currentmenuoption = 3;
                    }
                    else if (game.currentmenuoption == 4)   	//unlock 5
                    {
                        game.unlock[13] = true;
                        game.unlocknotify[13] = true;
                        music.playef(11, 10);
                        game.savestats(map, dwgfx);
                        game.createmenu("unlockmenutrials");
                        game.currentmenuoption = 4;
                    }
                    else if (game.currentmenuoption == 5)   	//unlock 6
                    {
                        game.unlock[14] = true;
                        game.unlocknotify[14] = true;
                        music.playef(11, 10);
                        game.savestats(map, dwgfx);
                        game.createmenu("unlockmenutrials");
                        game.currentmenuoption = 5;
                    }
                    else if (game.currentmenuoption == 6)   	//back
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("unlockmenu");
                        map.nexttowercolour();
                    }
                }
                else if (game.currentmenuname == "unlockmenu")
                {
                    if (game.currentmenuoption == 0)
                    {
                        //unlock time trials seperately...
                        music.playef(11, 10);
                        game.createmenu("unlockmenutrials");
                        map.nexttowercolour();
                    }
                    else if (game.currentmenuoption == 1)
                    {
                        //unlock intermissions
                        music.playef(11, 10);
                        game.unlock[16] = true;
                        game.unlocknotify[16] = true;
                        game.unlock[6] = true;
                        game.unlock[7] = true;
                        game.savestats(map, dwgfx);
                        game.createmenu("unlockmenu");
                        game.currentmenuoption = 1;
                    }
                    else if (game.currentmenuoption == 2)
                    {
                        //unlock no death mode
                        music.playef(11, 10);
                        game.unlock[17] = true;
                        game.unlocknotify[17] = true;
                        game.savestats(map, dwgfx);
                        game.createmenu("unlockmenu");
                        game.currentmenuoption = 2;
                    }
                    else if (game.currentmenuoption == 3)
                    {
                        //unlock flip mode
                        music.playef(11, 10);
                        game.unlock[18] = true;
                        game.unlocknotify[18] = true;
                        game.savestats(map, dwgfx);
                        game.createmenu("unlockmenu");
                        game.currentmenuoption = 3;
                    }
                    else if (game.currentmenuoption == 4)
                    {
                        //unlock jukebox
                        music.playef(11, 10);
                        game.stat_trinkets = 20;
                        game.savestats(map, dwgfx);
                        game.createmenu("unlockmenu");
                        game.currentmenuoption = 4;
                    }
                    else if (game.currentmenuoption == 5)
                    {
                        //unlock secret lab
                        music.playef(11, 10);
                        game.unlock[8] = true;
                        game.unlocknotify[8] = true;
                        game.savestats(map, dwgfx);
                        game.createmenu("unlockmenu");
                        game.currentmenuoption = 5;
                    }
                    else
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("mainmenu");
                        map.nexttowercolour();
                    }
                }
                else if (game.currentmenuname == "credits")
                {
                    if (game.currentmenuoption == 0)
                    {
                        //next page
                        music.playef(11, 10);
                        game.createmenu("credits2");
                        map.nexttowercolour();
                    }
                    else
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("mainmenu");
                        map.nexttowercolour();
                    }
                }
                else if (game.currentmenuname == "credits2")
                {
                    if (game.currentmenuoption == 0)
                    {
                        //next page
                        music.playef(11, 10);
                        game.createmenu("credits25");
                        map.nexttowercolour();
                    }
                    else
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("mainmenu");
                        map.nexttowercolour();
                    }
                }
                else if (game.currentmenuname == "credits25")
                {
                    if (game.currentmenuoption == 0)
                    {
                        //next page
                        music.playef(11, 10);
                        game.createmenu("credits3");
                        map.nexttowercolour();
                    }
                    else
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("mainmenu");
                        map.nexttowercolour();
                    }
                }
                else if (game.currentmenuname == "credits3")
                {
                    if (game.currentmenuoption == 0)
                    {
                        //next page
                        music.playef(11, 10);
                        game.createmenu("credits4");
                        map.nexttowercolour();
                    }
                    else
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("mainmenu");
                        map.nexttowercolour();
                    }
                }
                else if (game.currentmenuname == "credits4")
                {
                    if (game.currentmenuoption == 0)
                    {
                        //next page
                        music.playef(11, 10);
                        game.createmenu("credits5");
                        map.nexttowercolour();
                    }
                    else
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("mainmenu");
                        map.nexttowercolour();
                    }
                }
                else if (game.currentmenuname == "credits5")
                {
                    if (game.currentmenuoption == 0)
                    {
                        //next page
                        music.playef(11, 10);
                        game.createmenu("credits6");
                        map.nexttowercolour();
                    }
                    else
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("mainmenu");
                        map.nexttowercolour();
                    }
                }
                else if (game.currentmenuname == "credits6")
                {
                    if (game.currentmenuoption == 0)
                    {
                        //next page
                        music.playef(11, 10);
                        game.createmenu("credits7");
                        map.nexttowercolour();
                    }
                    else
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("mainmenu");
                        map.nexttowercolour();
                    }
                }
                else if (game.currentmenuname == "credits7")
                {
                    if (game.currentmenuoption == 0)
                    {
                        //next page
                        music.playef(11, 10);
                        game.createmenu("credits8");
                        map.nexttowercolour();
                    }
                    else
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("mainmenu");
                        map.nexttowercolour();
                    }
                }
                else if (game.currentmenuname == "credits8")
                {
                    if (game.currentmenuoption == 0)
                    {
                        //next page
                        music.playef(11, 10);
                        game.createmenu("credits9");
                        map.nexttowercolour();
                    }
                    else
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("mainmenu");
                        map.nexttowercolour();
                    }
                }
                else if (game.currentmenuname == "credits9")
                {
                    if (game.currentmenuoption == 0)
                    {
                        //first page
                        music.playef(11, 10);
                        game.createmenu("credits");
                        map.nexttowercolour();
                    }
                    else
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("mainmenu");
                        map.nexttowercolour();
                        music.niceplay(6);
                    }
                }
                else if (game.currentmenuname == "play")
                {
                    if (game.currentmenuoption == 0)
                    {
                        //continue
                        //right, this depends on what saves you've got
                        if (game.telesummary == "")
                        {
                            //You at least have a quicksave, or you couldn't have gotten here
                            game.mainmenu = 2;
                            dwgfx.fademode = 2;
                        }
                        else if (game.quicksummary == "")
                        {
                            //You at least have a telesave, or you couldn't have gotten here
                            game.mainmenu = 1;
                            dwgfx.fademode = 2;
                        }
                        else
                        {
                            //go to a menu!
                            music.playef(11, 10);
                            game.loadsummary(map, help); //Prepare save slots to display
                            game.createmenu("continue");
                            map.settowercolour(3);
                        }
                    }
                    else if (game.currentmenuoption == 1)
                    {
                        //play modes
                        music.playef(11, 10);
                        game.createmenu("playmodes");
                        map.nexttowercolour();
                    }
                    else if (game.currentmenuoption == 2)
                    {
                        //newgame
                        music.playef(11, 10);
                        game.createmenu("newgamewarning");
                        map.nexttowercolour();
                    }
                    else if (game.currentmenuoption == 3)
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("mainmenu");
                        map.nexttowercolour();
                    }
                }
                else if (game.currentmenuname == "playsecretlab")
                {
                    //as above, except with an extra option...
                    if (game.currentmenuoption == 0)
                    {
                        //continue
                        //right, this depends on what saves you've got
                        if (game.telesummary == "")
                        {
                            //You at least have a quicksave, or you couldn't have gotten here
                            game.mainmenu = 2;
                            dwgfx.fademode = 2;
                        }
                        else if (game.quicksummary == "")
                        {
                            //You at least have a telesave, or you couldn't have gotten here
                            game.mainmenu = 1;
                            dwgfx.fademode = 2;
                        }
                        else
                        {
                            //go to a menu!
                            music.playef(11, 10);
                            game.loadsummary(map, help); //Prepare save slots to display
                            game.createmenu("continue");
                            map.settowercolour(3);
                        }
                    }
                    else if (game.currentmenuoption == 1)
                    {
										  if(!map.invincibility){
                        game.mainmenu = 11;
                        dwgfx.fademode = 2;
											}else{
                        //Can't do yet! play sad sound
                        music.playef(2, 10);
											}
                    }
                    else if (game.currentmenuoption == 2)
                    {
                        //play modes
                        music.playef(11, 10);
                        game.createmenu("playmodes");
                        map.nexttowercolour();
                    }
                    else if (game.currentmenuoption == 3)
                    {
                        //newgame
                        music.playef(11, 10);
                        game.createmenu("newgamewarning");
                        map.nexttowercolour();
                    }
                    else if (game.currentmenuoption == 4)
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("mainmenu");
                        map.nexttowercolour();
                    }
                }
                else if (game.currentmenuname == "newgamewarning")
                {
                    if (game.currentmenuoption == 0)
                    {
                        //yep
                        game.mainmenu = 0;
                        dwgfx.fademode = 2;
                        game.deletequick();
                        game.deletetele();
                    }
                    else
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("play");
                        map.nexttowercolour();
                    }
                }

				else if (game.currentmenuname == "controller")
				{
					if (game.currentmenuoption == 0)
					{
						game.controllerSensitivity++;
						music.playef(11, 10);
						if(game.controllerSensitivity > 4)
						{
							game.controllerSensitivity = 0;
						}
					}

					if (game.currentmenuoption == 4)
					{
						game.createmenu("options");

						//Add extra menu for mmmmmm mod
						if(music.mmmmmm){
							#if defined(MAKEANDPLAY)
								game.menuoptions[3] = "soundtrack";
								game.menuoptionsactive[3] = true;
								game.menuoptions[4] = "return";
								game.menuoptionsactive[4] = true;
								game.nummenuoptions = 5;
							#elif !defined(MAKEANDPLAY)
								game.menuoptions[4] = "soundtrack";
								game.menuoptionsactive[4] = true;
								game.menuoptions[5] = "return";
								game.menuoptionsactive[5] = true;
								game.nummenuoptions = 6;
							#endif
						}
					}
				}
                else if (game.currentmenuname == "cleardatamenu")
                {
                    if (game.currentmenuoption == 0)
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("options");

												//Add extra menu for mmmmmm mod
												if(music.mmmmmm){
													#if defined(MAKEANDPLAY)
														game.menuoptions[3] = "soundtrack";
														game.menuoptionsactive[3] = true;
														game.menuoptions[4] = "return";
														game.menuoptionsactive[4] = true;
														game.nummenuoptions = 5;
													#elif !defined(MAKEANDPLAY)
														game.menuoptions[4] = "soundtrack";
														game.menuoptionsactive[4] = true;
														game.menuoptions[5] = "return";
														game.menuoptionsactive[5] = true;
														game.nummenuoptions = 6;
													#endif
												}
                        map.nexttowercolour();
                    }
                    else
                    {
                        //yep
                        music.playef(23, 10);
                        game.deletequick();
                        game.deletetele();
                        game.deletestats(map, dwgfx);
                        game.flashlight = 5;
                        game.screenshake = 15;
                        game.createmenu("mainmenu");
                        map.nexttowercolour();
                    }
                }
                else if (game.currentmenuname == "playmodes")
                {
                    if (game.currentmenuoption == 0  && game.slowdown == 30 && !map.invincibility)   //go to the time trial menu
                    {
                        music.playef(11, 10);
                        game.createmenu("timetrials");
                        map.nexttowercolour();
                    }
                    else if (game.currentmenuoption == 1 && game.unlock[16])
                    {
                        //intermission mode menu
                        music.playef(11, 10);
                        game.createmenu("intermissionmenu");
                        map.nexttowercolour();
                    }
                    else if (game.currentmenuoption == 2 && game.unlock[17] && game.slowdown == 30 && !map.invincibility)    //start a game in no death mode
                    {
                        music.playef(11, 10);
                        game.createmenu("startnodeathmode");
                        map.nexttowercolour();
                    }
                    else if (game.currentmenuoption == 3 && game.unlock[18])    //enable/disable flip mode
                    {
                        music.playef(18, 10);
                        game.screenshake = 10;
                        game.flashlight = 5;
                        dwgfx.setflipmode = !dwgfx.setflipmode;
                        game.savemystats = true;
                    }
                    else if (game.currentmenuoption == 4)
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("play");
                        map.nexttowercolour();
                    }
                    else
                    {
                        //Can't do yet! play sad sound
                        music.playef(2, 10);
                    }
                }
                else if (game.currentmenuname == "startnodeathmode")
                {
                    if (game.currentmenuoption == 0)   //start no death mode, disabling cutscenes
                    {
                        game.mainmenu = 10;
                        dwgfx.fademode = 2;
                    }
                    else if (game.currentmenuoption == 1)
                    {
                        game.mainmenu = 9;
                        dwgfx.fademode = 2;
                    }
                    else if (game.currentmenuoption == 2)
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("play");
                        map.nexttowercolour();
                    }
                }
                else if (game.currentmenuname == "continue")
                {
                    if (game.currentmenuoption == 0)
                    {
                        game.mainmenu = 1;
                        dwgfx.fademode = 2;
                    }
                    else if (game.currentmenuoption == 1)
                    {
                        game.mainmenu = 2;
                        dwgfx.fademode = 2;
                    }
                    else if (game.currentmenuoption == 2)
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("play");
                        map.nexttowercolour();
                    }
                }
                else if (game.currentmenuname == "intermissionmenu")
                {
                    if (game.currentmenuoption == 0)
                    {
                        music.playef(11, 10);
                        music.play(6);
                        game.createmenu("playint1");
                        map.nexttowercolour();
                    }
                    else if (game.currentmenuoption == 1)
                    {
                        music.playef(11, 10);
                        music.play(6);
                        game.createmenu("playint2");
                        map.nexttowercolour();
                    }
                    else if (game.currentmenuoption == 2)
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("play");
                        map.nexttowercolour();
                    }
                }
                else if (game.currentmenuname == "playint1")
                {
                    if (game.currentmenuoption == 0)
                    {
                        game.mainmenu = 12;
                        dwgfx.fademode = 2;
                    }
                    else if (game.currentmenuoption == 1)
                    {
                        game.mainmenu = 13;
                        dwgfx.fademode = 2;
                    }
                    else if (game.currentmenuoption == 2)
                    {
                        game.mainmenu = 14;
                        dwgfx.fademode = 2;
                    }
                    else if (game.currentmenuoption == 3)
                    {
                        game.mainmenu = 15;
                        dwgfx.fademode = 2;
                    }
                    else if (game.currentmenuoption == 4)
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("play");
                        map.nexttowercolour();
                    }
                }
                else if (game.currentmenuname == "playint2")
                {
                    if (game.currentmenuoption == 0)
                    {
                        game.mainmenu = 16;
                        dwgfx.fademode = 2;
                    }
                    else if (game.currentmenuoption == 1)
                    {
                        game.mainmenu = 17;
                        dwgfx.fademode = 2;
                    }
                    else if (game.currentmenuoption == 2)
                    {
                        game.mainmenu = 18;
                        dwgfx.fademode = 2;
                    }
                    else if (game.currentmenuoption == 3)
                    {
                        game.mainmenu = 19;
                        dwgfx.fademode = 2;
                    }
                    else if (game.currentmenuoption == 4)
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("play");
                        map.nexttowercolour();
                    }
                }
                else if (game.currentmenuname == "gameover2")
                {
                    //back
                    music.playef(11, 10);
                    music.play(6);
                    game.createmenu("mainmenu");
                    map.nexttowercolour();
                }
                else if (game.currentmenuname == "unlocktimetrials" || game.currentmenuname == "unlocktimetrial")
                {
                    //back
                    music.playef(11, 10);
                    game.createmenu("play");
                    map.nexttowercolour();
                }
                else if (game.currentmenuname == "unlocknodeathmode" || game.currentmenuname == "unlockintermission"
                         || game.currentmenuname == "unlockflipmode")
                {
                    //back
                    music.playef(11, 10);
                    game.createmenu("play");
                    map.nexttowercolour();
                }
                else if (game.currentmenuname == "timetrials")
                {
                    if (game.currentmenuoption == 0 && game.unlock[9])   //space station 1
                    {
                        game.mainmenu = 3;
                        dwgfx.fademode = 2;
                    }
                    else if (game.currentmenuoption == 1 && game.unlock[10])    //lab
                    {
                        game.mainmenu = 4;
                        dwgfx.fademode = 2;
                    }
                    else if (game.currentmenuoption == 2 && game.unlock[11])    //tower
                    {
                        game.mainmenu = 5;
                        dwgfx.fademode = 2;
                    }
                    else if (game.currentmenuoption == 3 && game.unlock[12])    //station 2
                    {
                        game.mainmenu = 6;
                        dwgfx.fademode = 2;
                    }
                    else if (game.currentmenuoption == 4 && game.unlock[13])    //warp
                    {
                        game.mainmenu = 7;
                        dwgfx.fademode = 2;
                    }
                    else if (game.currentmenuoption == 5 && game.unlock[14])    //final
                    {
                        game.mainmenu = 8;
                        dwgfx.fademode = 2;
                    }
                    else if (game.currentmenuoption == 6)    //go to the time trial menu
                    {
                        //back
                        music.playef(11, 10);
                        game.createmenu("play");
                        map.nexttowercolour();
                    }
                    else
                    {
                        //Can't do yet! play sad sound
                        music.playef(2, 10);
                    }
                }
                else if (game.currentmenuname == "timetrialcomplete3")
                {
                    if (game.currentmenuoption == 0)
                    {
                        //back
                        music.playef(11, 10);
                        music.play(6);
                        game.createmenu("play");
                        map.nexttowercolour();
                    }
                    else if (game.currentmenuoption == 1)
                    {
                        //duplicate the above based on given time trial level!
                        if (game.timetriallevel == 0)   //space station 1
                        {
                            game.mainmenu = 3;
                            dwgfx.fademode = 2;
                        }
                        else if (game.timetriallevel == 1)    //lab
                        {
                            game.mainmenu = 4;
                            dwgfx.fademode = 2;
                        }
                        else if (game.timetriallevel == 2)    //tower
                        {
                            game.mainmenu = 5;
                            dwgfx.fademode = 2;
                        }
                        else if (game.timetriallevel == 3)    //station 2
                        {
                            game.mainmenu = 6;
                            dwgfx.fademode = 2;
                        }
                        else if (game.timetriallevel == 4)    //warp
                        {
                            game.mainmenu = 7;
                            dwgfx.fademode = 2;
                        }
                        else if (game.timetriallevel == 5)    //final
                        {
                            game.mainmenu = 8;
                            dwgfx.fademode = 2;
                        }
                    }
                }
                else if (game.currentmenuname == "gamecompletecontinue" || game.currentmenuname == "nodeathmodecomplete2")
                {
                    if (game.currentmenuoption == 0)
                    {
                        music.play(6);
                        music.playef(11, 10);
                        game.createmenu("play");
                        map.nexttowercolour();
                    }
                }
            }
        }
        if (    game.currentmenuname == "controller" &&
                game.currentmenuoption > 0 &&
                game.currentmenuoption < 4 &&
                key.controllerButtonDown()      )
        {
            updatebuttonmappings(game, key, music, game.currentmenuoption);
        }

    }

    if (dwgfx.fademode == 1)
        script.startgamemode(game.mainmenu, key, dwgfx, game, map, obj, help, music);
}

void gameinput(KeyPoll& key, Graphics& dwgfx, Game& game, mapclass& map,
               entityclass& obj, UtilityClass& help, musicclass& music)
{
    //TODO mouse input
    //game.mx = (mouseX / 2);
    //game.my = (mouseY / 2);

    if(!script.running)
    {
        game.press_left = false;
        game.press_right = false;
        game.press_action = false;
        game.press_map = false;
    }
/*
    if (game.recording == 2 && !game.playbackfinished)
    {
        //playback!
        //record your input and add it to the record string
        //Keys are:
        //0 - nothing
        //1 - left
        //2 - right
        //3 - left+right
        //4 - flip
        //5 - left+flip
        //6 - right+flip
        //7 - left+right+flip
        //8 - Map/teleport
        if (!game.recordinit)
        {
            //Init recording
            game.recordinit = true;
            game.combomode = false;
            game.playmove = game.playback[game.playbackpos+1];
            game.playcombo = game.playback[game.playbackpos];
        }

        if (game.playcombo <= 0)
        {
            //move on to the next action
            game.playbackpos += 2;
            game.playmove = game.playback[game.playbackpos + 1];
            game.playcombo = game.playback[game.playbackpos];
            if (game.playcombo > 1) game.playcombo--;
        }

        if (game.playcombo >= 1)
        {
            game.playcombo--;
            if (game.playmove == 1 || game.playmove == 3 || game.playmove == 5 || game.playmove == 7)
            {
                game.press_left = true;
            }
            if (game.playmove == 2 || game.playmove == 3 || game.playmove == 6 || game.playmove == 7)
            {
                game.press_right = true;
            }
            if (game.playmove == 4 || game.playmove == 5 || game.playmove == 6 || game.playmove == 7)
            {
                game.press_action = true;
            }
            if (game.playmove == 8)
            {
                game.press_map = true;
                //game.playbackfinished = true;
                //TODO WTF is trace
                //trace("finished!");
            }
        }
    }
    else
    { */
        if(!script.running)
        {
            if (key.isDown(KEYBOARD_LEFT) || key.isDown(KEYBOARD_a) || key.controllerWantsLeft(false))
            {
                game.press_left = true;
            }
            if (key.isDown(KEYBOARD_RIGHT) || key.isDown(KEYBOARD_d) || key.controllerWantsRight(false))
            {
                game.press_right = true;
            }
            if (key.isDown(KEYBOARD_z) || key.isDown(KEYBOARD_SPACE) || key.isDown(KEYBOARD_v)
                    || key.isDown(KEYBOARD_UP) || key.isDown(KEYBOARD_DOWN) || key.isDown(KEYBOARD_w) || key.isDown(KEYBOARD_s)|| key.isDown(game.controllerButton_flip))
            {
                game.press_action = true;
            };
			if (key.isDown(KEYBOARD_ENTER) || key.isDown(SDLK_KP_ENTER) || key.isDown(game.controllerButton_map)  )
			{
				game.press_map = true;
			}
        }
    //}

    if (game.advancetext)
    {
        if (game.pausescript)
        {
            game.press_action = false;
            if (key.isDown(KEYBOARD_z) || key.isDown(KEYBOARD_SPACE) || key.isDown(KEYBOARD_v)
                    || key.isDown(KEYBOARD_UP) || key.isDown(KEYBOARD_DOWN) || key.isDown(KEYBOARD_w) || key.isDown(KEYBOARD_s) || key.isDown(game.controllerButton_flip)) game.press_action = true;
        }

        if (game.press_action && !game.jumpheld)
        {
            if (game.pausescript)
            {
                game.pausescript = false;
                game.hascontrol = true;
                game.jumpheld = true;
            }
            else
            {
						    if(!game.glitchrunkludge) game.state++;
                game.jumpheld = true;
								game.glitchrunkludge=true;
								//Bug fix! You should only be able to do this ONCE.
            }
        }
    }

    if (!game.press_map) game.mapheld = false;

    /*
    if (key.isDown("1".charCodeAt(0))) {
    dwgfx.screen.width = 640;
    dwgfx.screen.height = 480;
    setstage(640,480);
    }
    if (key.isDown("2".charCodeAt(0))) {
    dwgfx.screen.width = 960;
    dwgfx.screen.height = 720;
    setstage(960,720);
    }
    if (key.isDown("3".charCodeAt(0))) {
    dwgfx.screen.width = 1280;
    dwgfx.screen.height = 960;
    setstage(1280,960);
    }
    */
    /*game.test = true;
    game.teststring = String(game.inertia);
    if (key.isDown("1".charCodeAt(0))) game.inertia = 0.5;
    if (key.isDown("2".charCodeAt(0))) game.inertia = 0.6;
    if (key.isDown("3".charCodeAt(0))) game.inertia = 0.7;
    if (key.isDown("4".charCodeAt(0))) game.inertia = 0.8;
    if (key.isDown("5".charCodeAt(0))) game.inertia = 0.9;
    if (key.isDown("6".charCodeAt(0))) game.inertia = 1;
    if (key.isDown("7".charCodeAt(0))) game.inertia = 1.1;
    if (key.isDown("8".charCodeAt(0))) game.inertia = 1.2;
    if (key.isDown("9".charCodeAt(0))) game.inertia = 1.3;
    if (key.isDown("0".charCodeAt(0))) game.inertia = 1.4;*/

    if (game.intimetrial && dwgfx.fademode == 1 && game.quickrestartkludge)
    {
        //restart the time trial
        game.quickrestartkludge = false;
        script.startgamemode(game.timetriallevel + 3, key, dwgfx, game, map, obj, help, music);
        game.deathseq = -1;
        game.completestop = false;
    }

    //Returning to editor mode must always be possible
    if(map.custommode && !map.custommodeforreal){
      if ((game.press_map || key.isDown(27)) && !game.mapheld){
        game.mapheld = true;
        //Return to level editor
        if (game.activeactivity > -1 && game.press_map){
           if((int(std::abs(obj.entities[obj.getplayer()].vx))<=1) && (int(obj.entities[obj.getplayer()].vy) == 0) )
            {
                script.load(obj.blocks[game.activeactivity].script);
                obj.removeblock(game.activeactivity);
            }
        }else{
          game.gamestate = EDITORMODE;

          dwgfx.textboxremove();
          game.hascontrol = true;
          game.advancetext = false;
          game.completestop = false;
          game.state = 0;
			    dwgfx.showcutscenebars = false;

          dwgfx.backgrounddrawn=false;
          music.fadeout();
        }
      }
    }

    //Entity type 0 is player controled
    for (int ie = 0; ie < obj.nentity; ++ie)
    {
        if (obj.entities[ie].rule == 0)
        {
            //game.test = true;
            //game.teststring = "player(" + String(int(obj.entities[i])) + "," + String(int(obj.entities[i].yp)) + ")"
            //  + ", mouse(" + String(int(game.mx)) + "," + String(int(game.my)) + ")";
            if (game.hascontrol && game.deathseq == -1 && game.lifeseq <= 5)
            {
                /*
                if (key.isDown(8)) {
                script.load("returntohub");
                }
                */
                /*
                if (key.isDown(27)) {
                game.state = 0;
                dwgfx.textboxremove();

                map.tdrawback = true;
                music.haltdasmusik();
                game.gamestate = TITLEMODE;
                }
                */

                if (game.press_map && !game.mapheld)
                {
                    game.mapheld = true;

                    if (game.activetele && game.readytotele > 20 && !game.intimetrial)
                    {
                        if(!dwgfx.flipmode)
                        {
                            obj.flags[73] = 1; //Flip mode test
                        }
                        if(int(std::abs(obj.entities[ie].vx))<=1 && int(obj.entities[ie].vy)==0)
                        {
                            //wait! space station 2 debug thingy
                            if (game.teleportscript != "")
                            {

                                //trace(game.recordstring);
                                //We're teleporting! Yey!
                                game.activetele = false;
                                game.hascontrol = false;
                                music.fadeout(); //Uncomment this when it's working!

                                int player = obj.getplayer();
                                obj.entities[player].colour = 102;

                                int teleporter = obj.getteleporter();
                                obj.entities[teleporter].tile = 6;
                                obj.entities[teleporter].colour = 102;
                                //which teleporter script do we use? it depends on the companion!
                                game.state = 4000;
                                game.statedelay = 0;
                            }
                            else if (game.companion == 0)
                            {
                                //Alright, normal teleporting
                                game.gamestate = 5;
                                dwgfx.menuoffset = 240; //actually this should count the roomname
                                if (map.extrarow) dwgfx.menuoffset -= 10;
                                //dwgfx.menubuffer.copyPixels(dwgfx.screenbuffer, dwgfx.screenbuffer.rect, dwgfx.tl, null, null, false);

                                //TODO TESTHIS
                                //dwgfx.screenbuffer->UpdateScreen(dwgfx.menubuffer, NULL);
								BlitSurfaceStandard(dwgfx.menubuffer,NULL,dwgfx.backBuffer, NULL);

                                dwgfx.resumegamemode = false;

                                game.useteleporter = true;
                                game.initteleportermode(map);
                            }
                            else
                            {
                                //trace(game.recordstring);
                                //We're teleporting! Yey!
                                game.activetele = false;
                                game.hascontrol = false;
                                music.fadeout(); //Uncomment this when it's working!

                                int player = obj.getplayer();
                                obj.entities[player].colour = 102;
                                int companion = obj.getcompanion(game.companion);
                                if(companion>-1) obj.entities[companion].colour = 102;

                                int teleporter = obj.getteleporter();
                                obj.entities[teleporter].tile = 6;
                                obj.entities[teleporter].colour = 102;
                                //which teleporter script do we use? it depends on the companion!
                                game.state = 3000;
                                game.statedelay = 0;
                            }
                        }
                    }
                    else if (game.activeactivity > -1)
                    {
                        if((int(std::abs(obj.entities[ie].vx))<=1) && (int(obj.entities[ie].vy) == 0) )
                        {
                            script.load(obj.blocks[game.activeactivity].script);
                            obj.removeblock(game.activeactivity);
                        }
                    }
                    else if (game.swnmode == 1 && game.swngame == 1)
                    {
                        //quitting the super gravitron
                        game.mapheld = true;
                        //Quit menu, same conditions as in game menu
                        game.gamestate = MAPMODE;
                        game.gamesaved = false;
                        dwgfx.resumegamemode = false;
                        game.menupage = 20; // The Map Page
                        //dwgfx.menubuffer.copyPixels(dwgfx.screenbuffer, dwgfx.screenbuffer.rect, dwgfx.tl, null, null, false);
                        //dwgfx.screenbuffer->UpdateScreen(dwgfx.menubuffer, NULL);
														BlitSurfaceStandard(dwgfx.menubuffer,NULL,dwgfx.backBuffer, NULL);
                        dwgfx.menuoffset = 240; //actually this should count the roomname
                        if (map.extrarow) dwgfx.menuoffset -= 10;
                    }
                    else if (game.intimetrial && dwgfx.fademode==0)
                    {
                        //Quick restart of time trial
                        script.hardreset(key, dwgfx, game, map, obj, help, music);
                        if (dwgfx.setflipmode) dwgfx.flipmode = true;
                        dwgfx.fademode = 2;
                        game.completestop = true;
                        music.fadeout();
                        game.intimetrial = true;
                        game.quickrestartkludge = true;
                    }
                    else if (dwgfx.fademode==0)
                    {
                        //Normal map screen, do transition later
                        game.gamestate = MAPMODE;
                        map.cursordelay = 0;
                        map.cursorstate = 0;
                        game.gamesaved = false;
                        dwgfx.resumegamemode = false;
                        game.menupage = 0; // The Map Page
														BlitSurfaceStandard(dwgfx.menubuffer,NULL,dwgfx.backBuffer, NULL);
                        //dwgfx.screenbuffer->UpdateScreen(dwgfx.menubuffer, NULL);
                        dwgfx.menuoffset = 240; //actually this should count the roomname
                        if (map.extrarow) dwgfx.menuoffset -= 10;
                    }
                }

                if ((key.isDown(27) || key.isDown(game.controllerButton_esc)) && (!map.custommode || map.custommodeforreal))
                {
                    game.mapheld = true;
                    //Quit menu, same conditions as in game menu
                    game.gamestate = MAPMODE;
                    game.gamesaved = false;
                    dwgfx.resumegamemode = false;
                    game.menupage = 10; // The Map Page

                    //dwgfx.menubuffer.copyPixels(dwgfx.screenbuffer, dwgfx.screenbuffer.rect, dwgfx.tl, NULL, NULL, false);

                    //dwgfx.screenbuffer->UpdateScreen(dwgfx.menubuffer, NULL);
													BlitSurfaceStandard(dwgfx.menubuffer,NULL,dwgfx.backBuffer, NULL);
                    dwgfx.menuoffset = 240; //actually this should count the roomname
                    if (map.extrarow) dwgfx.menuoffset -= 10;
                }

                if (key.keymap[SDLK_r] && game.deathseq<=0)// && map.custommode) //Have fun glitchrunners!
                {
                	game.deathseq = 30;
                }

                if (game.press_left)
                {
                    game.tapleft++;
                }
                else
                {
                    if (game.tapleft <= 4 && game.tapleft > 0)
                    {
                        if (obj.entities[ie].vx < 0.0f)
                        {
                            obj.entities[ie].vx = 0.0f;
                        }
                    }
                    game.tapleft = 0;
                }
                if (game.press_right)
                {
                    game.tapright++;
                }
                else
                {
                    if (game.tapright <= 4 && game.tapright > 0)
                    {
                        if (obj.entities[ie].vx > 0.0f)
                        {
                            obj.entities[ie].vx = 0.0f;
                        }
                    }
                    game.tapright = 0;
                }


                if(game.press_left)
                {
                    //obj.entities[i].vx = -4;
                    obj.entities[ie].ax = -3;
                    obj.entities[ie].dir = 0;
                }
                else if (game.press_right)
                {
                    //obj.entities[i].vx = 4;
                    obj.entities[ie].ax = 3;
                    obj.entities[ie].dir = 1;
                }

                if (!game.press_action)
                {
                    game.jumppressed = 0;
                    game.jumpheld = false;
                }

                if (game.press_action && !game.jumpheld)
                {
                    game.jumppressed = 5;
                    game.jumpheld = true;
                }

                if (game.jumppressed > 0)
                {
                    game.jumppressed--;
                    if (obj.entities[ie].onground>0 && game.gravitycontrol == 0)
                    {
                        game.gravitycontrol = 1;
                        obj.entities[ie].vy = -4;
                        obj.entities[ie].ay = -3;
                        music.playef(0, 10);
                        game.jumppressed = 0;
                        game.totalflips++;
                    }
                    if (obj.entities[ie].onroof>0 && game.gravitycontrol == 1)
                    {
                        game.gravitycontrol = 0;
                        obj.entities[ie].vy = 4;
                        obj.entities[ie].ay = 3;
                        music.playef(1, 10);
                        game.jumppressed = 0;
                        game.totalflips++;
                    }
                }
            }
            else
            {
                //Simple detection of keypresses outside player control, will probably scrap this (expand on
                //advance text function)
                if (!game.press_action)
                {
                    game.jumppressed = 0;
                    game.jumpheld = false;
                }

                if (game.press_action && !game.jumpheld)
                {
                    game.jumppressed = 5;
                    game.jumpheld = true;
                }
            }
        }
    }
}

void mapinput(KeyPoll& key, Graphics& dwgfx, Game& game, mapclass& map,
              entityclass& obj, UtilityClass& help, musicclass& music)
{
    //TODO Mouse Input!
    //game.mx = (mouseX / 2);
    //game.my = (mouseY / 2);

    game.press_left = false;
    game.press_right = false;
    game.press_action = false;
    game.press_map = false;

    if(dwgfx.menuoffset==0)
    {
        if (dwgfx.flipmode)
        {
            if (key.isDown(KEYBOARD_LEFT) || key.isDown(KEYBOARD_DOWN) || key.isDown(KEYBOARD_a) ||  key.isDown(KEYBOARD_s) || key.controllerWantsLeft(false) ) game.press_left = true;
            if (key.isDown(KEYBOARD_RIGHT) || key.isDown(KEYBOARD_UP) || key.isDown(KEYBOARD_d) ||  key.isDown(KEYBOARD_w) || key.controllerWantsRight(false)) game.press_right = true;
        }
        else
        {
            if (key.isDown(KEYBOARD_LEFT) || key.isDown(KEYBOARD_UP) || key.isDown(KEYBOARD_a) ||  key.isDown(KEYBOARD_w)|| key.controllerWantsLeft(false))
            {
                game.press_left = true;
            }
            if (key.isDown(KEYBOARD_RIGHT) || key.isDown(KEYBOARD_DOWN) || key.isDown(KEYBOARD_d) ||  key.isDown(KEYBOARD_s)|| key.controllerWantsRight(false))
            {
                game.press_right = true;
            }
        }
        if (key.isDown(KEYBOARD_z) || key.isDown(KEYBOARD_SPACE) || key.isDown(KEYBOARD_v) || key.isDown(game.controllerButton_flip))
        {
            game.press_action = true;
        }
        if (game.menupage < 9)
        {
            if (key.isDown(KEYBOARD_ENTER) || key.isDown(game.controllerButton_map) ) game.press_map = true;
            if (key.isDown(27))
            {
                game.mapheld = true;
                game.menupage = 10;
            }
        }
        else
        {
            if (key.isDown(KEYBOARD_ENTER) || key.isDown(27)|| key.isDown(game.controllerButton_map) ) game.press_map = true;
        }

        //In the menu system, all keypresses are single taps rather than holds. Therefore this test has to be done for all presses
        if (!game.press_action && !game.press_left && !game.press_right)
        {
            game.jumpheld = false;
        }
        if (!game.press_map && !key.isDown(27))
        {
            game.mapheld = false;
        }
    }
    else
    {
        game.mapheld = true;
        game.jumpheld = true;
    }

    if (!game.mapheld)
    {
        if(game.press_map && game.menupage < 10)
        {
            //Normal map screen, do transition later
            dwgfx.resumegamemode = true;
        }
    }

    if (dwgfx.fademode == 1)
    {
        FillRect(dwgfx.menubuffer, 0x000000);
        dwgfx.resumegamemode = true;
        obj.removeallblocks();
        game.menukludge = false;
        if (game.menupage >= 20)
        {
            game.state = 96;
            game.statedelay = 0;
        }
        else
        {
            game.state = 80;
            game.statedelay = 0;
        }
    }

    if (!game.jumpheld)
    {
        if (game.press_action || game.press_left || game.press_right || game.press_map)
        {
            game.jumpheld = true;
        }

        if (game.press_left)
        {
            game.menupage--;
        }
        else if (game.press_right)
        {
            game.menupage++;
        }

        if (game.menupage == 1 && obj.flags[67] == 1 && game.press_action && !game.insecretlab && !map.custommode)
        {
            //Warp back to the ship
            dwgfx.resumegamemode = true;

            game.teleport_to_x = 2;
            game.teleport_to_y = 11;

            //trace(game.recordstring);
            //We're teleporting! Yey!
            game.activetele = false;
            game.hascontrol = false;

            int i = obj.getplayer();
            obj.entities[i].colour = 102;

            //which teleporter script do we use? it depends on the companion!
            game.state = 4000;
            game.statedelay = 0;
        }

        if (game.menupage == 3 && !game.gamesaved && game.press_action && !game.intimetrial
                && !game.nodeathmode && !game.insecretlab && !game.inintermission)
        {
            game.flashlight = 5;
            game.screenshake = 10;
            music.playef(18, 10);
            game.gamesaved = true;

            game.savetime = game.timestring(help);
            game.savearea = map.currentarea(map.area(game.roomx, game.roomy));
            game.savetrinkets = game.trinkets;

            if (game.roomx >= 102 && game.roomx <= 104 && game.roomy >= 110 && game.roomy <= 111) game.savearea = "The Ship";

            if(map.custommodeforreal){
              game.customsavequick(ed.ListOfMetaData[game.playcustomlevel].filename, map, obj, music);
            }else{
              game.savequick(map, obj, music);
            }
        }

        if (game.menupage == 10 && game.press_action)
        {
            //return to game
            dwgfx.resumegamemode = true;
        }
        if (game.menupage == 11 && game.press_action)
        {
            //quit to menu
            if (dwgfx.fademode == 0)
            {
				//Kill contents of offset render buffer, since we do that for some reason.
				//This fixes an apparent frame flicker.
				FillRect(dwgfx.tempBuffer, 0x000000);
                if (game.intimetrial || game.insecretlab || game.nodeathmode) game.menukludge = true;
                script.hardreset(key, dwgfx, game, map, obj, help, music);
                if(dwgfx.setflipmode) dwgfx.flipmode = true;
                dwgfx.fademode = 2;
                music.fadeout();
            }
        }

        if (game.menupage == 20 && game.press_action)
        {
            //return to game
            dwgfx.resumegamemode = true;
        }
        if (game.menupage == 21 && game.press_action)
        {
            //quit to menu
            if (dwgfx.fademode == 0)
            {
                game.swnmode = false;
                dwgfx.fademode = 2;
                music.fadeout();
            }
        }

        if (game.menupage < 0) game.menupage = 3;
        if (game.menupage > 3 && game.menupage < 9) game.menupage = 0;

        if (game.menupage == 9) game.menupage = 11;
        if (game.menupage == 12) game.menupage = 10;

        if (game.menupage == 19) game.menupage = 21;
        if (game.menupage == 22) game.menupage = 20;

    }
}

void teleporterinput(KeyPoll& key, Graphics& dwgfx, Game& game, mapclass& map,
                     entityclass& obj, UtilityClass& help, musicclass& music)
{
    //Todo Mouseinput!
    //game.mx = (mouseX / 2);
    //game.my = (mouseY / 2);

    int tempx, tempy;

    game.press_left = false;
    game.press_right = false;
    game.press_action = false;
    game.press_map = false;

    if(dwgfx.menuoffset==0)
    {
        if (key.isDown(KEYBOARD_LEFT)|| key.isDown(KEYBOARD_a) || key.controllerWantsLeft(false) ) game.press_left = true;
        if (key.isDown(KEYBOARD_RIGHT) || key.isDown(KEYBOARD_d)|| key.controllerWantsRight(false) ) game.press_right = true;
        if (key.isDown(KEYBOARD_z) || key.isDown(KEYBOARD_SPACE) || key.isDown(KEYBOARD_v)
                || key.isDown(KEYBOARD_UP) || key.isDown(KEYBOARD_DOWN)||  key.isDown(KEYBOARD_w)||  key.isDown(KEYBOARD_s) || key.isDown(game.controllerButton_flip)) game.press_action = true;
        if (key.isDown(KEYBOARD_ENTER) || key.isDown(game.controllerButton_map)) game.press_map = true;

        //In the menu system, all keypresses are single taps rather than holds. Therefore this test has to be done for all presses
        if (!game.press_action && !game.press_left && !game.press_right) game.jumpheld = false;
        if (!game.press_map) game.mapheld = false;
    }
    else
    {
        game.mapheld = true;
        game.jumpheld = true;
    }

    if (!game.jumpheld)
    {
        if (game.press_action || game.press_left || game.press_right || game.press_map)
        {
            game.jumpheld = true;
        }

        if (game.press_left)
        {
            game.teleport_to_teleporter--;
            if (game.teleport_to_teleporter < 0) game.teleport_to_teleporter = map.numteleporters - 1;
            tempx = map.teleporters[game.teleport_to_teleporter].x;
            tempy = map.teleporters[game.teleport_to_teleporter].y;
            while (map.explored[tempx + (20 * tempy)] == 0)
            {
                game.teleport_to_teleporter--;
                if (game.teleport_to_teleporter < 0) game.teleport_to_teleporter = map.numteleporters - 1;
                tempx = map.teleporters[game.teleport_to_teleporter].x;
                tempy = map.teleporters[game.teleport_to_teleporter].y;
            }
        }
        else if (game.press_right)
        {
            game.teleport_to_teleporter++;
            if (game.teleport_to_teleporter >= map.numteleporters) game.teleport_to_teleporter = 0;
            tempx = map.teleporters[game.teleport_to_teleporter].x;
            tempy = map.teleporters[game.teleport_to_teleporter].y;
            while (map.explored[tempx + (20 * tempy)] == 0)
            {
                game.teleport_to_teleporter++;
                if (game.teleport_to_teleporter >= map.numteleporters) game.teleport_to_teleporter = 0;
                tempx = map.teleporters[game.teleport_to_teleporter].x;
                tempy = map.teleporters[game.teleport_to_teleporter].y;
            }
        }

        if (game.press_map)
        {
            tempx = map.teleporters[game.teleport_to_teleporter].x;
            tempy = map.teleporters[game.teleport_to_teleporter].y;
            if (game.roomx == tempx + 100 && game.roomy == tempy + 100)
            {
                //cancel!
                dwgfx.resumegamemode = true;
            }
            else
            {
                //teleport
                dwgfx.resumegamemode = true;
                game.teleport_to_x = tempx;
                game.teleport_to_y = tempy;

                //trace(game.recordstring);
                //We're teleporting! Yey!
                game.activetele = false;
                game.hascontrol = false;

                int i = obj.getplayer();
                obj.entities[i].colour = 102;

                i = obj.getteleporter();
                obj.entities[i].tile = 6;
                obj.entities[i].colour = 102;
                //which teleporter script do we use? it depends on the companion!
                game.state = 4000;
                game.statedelay = 0;
            }
        }
    }
}

void gamecompleteinput(KeyPoll& key, Graphics& dwgfx, Game& game, mapclass& map,
                       entityclass& obj, UtilityClass& help, musicclass& music)
{
    //game.mx = (mouseX / 2);
    //game.my = (mouseY / 2);

    game.press_left = false;
    game.press_right = false;
    game.press_action = false;
    game.press_map = false;

    if (key.isDown(KEYBOARD_z) || key.isDown(KEYBOARD_SPACE) || key.isDown(KEYBOARD_v) || key.isDown(game.controllerButton_flip))
    {
        game.creditposition -= 6;
        if (game.creditposition <= -1650)
        {
            if(dwgfx.fademode==0)
            {
                dwgfx.fademode = 2;
            }
            game.creditposition = -1650;
        }
        else
        {
            map.bypos += 6;
            map.bscroll = +6;
        }
        game.press_action = true;
    }
    if (key.isDown(KEYBOARD_ENTER)|| key.isDown(game.controllerButton_map)) game.press_map = true;
    //if (key.isDown(27)) { game.mapheld = true;  game.menupage = 10; }

    if (!game.mapheld)
    {
        if(game.press_map)
        {
            //Return to game
            if(dwgfx.fademode==0)
            {
                dwgfx.fademode = 2;
            }
        }
    }
}

void gamecompleteinput2(KeyPoll& key, Graphics& dwgfx, Game& game, mapclass& map,
                        entityclass& obj, UtilityClass& help, musicclass& music)
{
    //TODO Mouse Input!
    //game.mx = (mouseX / 2);
    //game.my = (mouseY / 2);


    game.press_left = false;
    game.press_right = false;
    game.press_action = false;
    game.press_map = false;


    if (key.isDown(KEYBOARD_z) || key.isDown(KEYBOARD_SPACE) || key.isDown(KEYBOARD_v) || key.isDown(game.controllerButton_flip))
    {
        game.creditposx++;
        if (game.creditposy >= 30)
        {
            if(dwgfx.fademode==0)
            {
                dwgfx.fademode = 2;
                music.fadeout();
            }
        }
        game.press_action = true;
    }
    if (key.isDown(KEYBOARD_ENTER) || key.isDown(game.controllerButton_map)) game.press_map = true;
    //if (key.isDown(27)) { game.mapheld = true;  game.menupage = 10; }

    if (!game.mapheld)
    {
        if(game.press_map)
        {
            //Return to game
            if(dwgfx.fademode==0)
            {
                dwgfx.fademode = 2;
                music.fadeout();
            }
        }
    }
}
