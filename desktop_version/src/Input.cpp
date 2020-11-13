#include <tinyxml2.h>

#include "Credits.h"
#include "editor.h"
#include "Entity.h"
#include "Enums.h"
#include "FileSystemUtils.h"
#include "Game.h"
#include "Graphics.h"
#include "KeyPoll.h"
#include "Logic.h"
#include "MakeAndPlay.h"
#include "Map.h"
#include "Music.h"
#include "Script.h"
#include "UtilityClass.h"

void updatebuttonmappings(int bind)
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
                    music.playef(11);
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
                for (size_t j = 0; j < game.controllerButton_restart.size(); j += 1)
                {
                    if (i == game.controllerButton_restart[j])
                    {
                        game.controllerButton_restart.erase(game.controllerButton_restart.begin() + j);
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
                    music.playef(11);
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
                for (size_t j = 0; j < game.controllerButton_restart.size(); j += 1)
                {
                    if (i == game.controllerButton_restart[j])
                    {
                        game.controllerButton_restart.erase(game.controllerButton_restart.begin() + j);
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
                    music.playef(11);
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
                for (size_t j = 0; j < game.controllerButton_restart.size(); j += 1)
                {
                    if (i == game.controllerButton_restart[j])
                    {
                        game.controllerButton_restart.erase(game.controllerButton_restart.begin() + j);
                    }
                }
            }
            if (bind == 4)
            {
                for (size_t j = 0; j < game.controllerButton_restart.size(); j += 1)
                {
                    if (i == game.controllerButton_restart[j])
                    {
                        dupe = true;
                    }
                }
                if (!dupe)
                {
                    game.controllerButton_restart.push_back(i);
                    music.playef(11);
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
                for (size_t j = 0; j < game.controllerButton_esc.size(); j += 1)
                {
                    if (i == game.controllerButton_esc[j])
                    {
                        game.controllerButton_esc.erase(game.controllerButton_esc.begin() + j);
                    }
                }
            }
        }
    }
}

void menuactionpress()
{
    switch (game.currentmenuname)
    {
    case Menu::mainmenu:
#if defined(MAKEANDPLAY)
#define MPOFFSET -1
#else
#define MPOFFSET 0
#endif

#if defined(NO_CUSTOM_LEVELS)
#define NOCUSTOMSOFFSET -1
#else
#define NOCUSTOMSOFFSET 0
#endif

#define OFFSET (MPOFFSET+NOCUSTOMSOFFSET)

        switch (game.currentmenuoption)
        {
#if !defined(MAKEANDPLAY)
        case 0:
            //Play
            if (!game.save_exists() && !game.anything_unlocked())
            {
                //No saves exist, just start a new game
                music.playef(11);
                game.mainmenu = 0;
                graphics.fademode = 2;
            }
            else
            {
                //Bring you to the normal playmenu
                music.playef(11);
                game.createmenu(Menu::play);
                map.nexttowercolour();
            }
            break;
#endif
#if !defined(NO_CUSTOM_LEVELS)
        case OFFSET+1:
            //Bring you to the normal playmenu
            music.playef(11);
            game.createmenu(Menu::playerworlds);
            map.nexttowercolour();
            break;
#endif
        case OFFSET+2:
            //Options
            music.playef(11);
            game.createmenu(Menu::graphicoptions);
            map.nexttowercolour();
            break;
        case OFFSET+3:
            //Options
            music.playef(11);
            game.createmenu(Menu::options);
            map.nexttowercolour();
            break;
#if !defined(MAKEANDPLAY)
        case OFFSET+4:
            //Credits
            music.playef(11);
            game.createmenu(Menu::credits);
            map.nexttowercolour();
            break;
#else
 #undef MPOFFSET
 #define MPOFFSET -2
#endif
        case OFFSET+5:
            //bye!
            music.playef(2);
            game.mainmenu = 100;
            graphics.fademode = 2;
            break;
#undef OFFSET
#undef NOCUSTOMSOFFSET
#undef MPOFFSET
        }
        break;
#if !defined(NO_CUSTOM_LEVELS)
    case Menu::levellist:
        if(game.currentmenuoption==(int)game.menuoptions.size()-1){
            //go back to menu
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
        }else if(game.currentmenuoption==(int)game.menuoptions.size()-2){
            //previous page
            music.playef(11);
            if(game.levelpage==0){
                game.levelpage=(ed.ListOfMetaData.size()-1)/8;
            }else{
                game.levelpage--;
            }
            game.createmenu(Menu::levellist, true);
            game.currentmenuoption=game.menuoptions.size()-2;
            map.nexttowercolour();
        }else if(game.currentmenuoption==(int)game.menuoptions.size()-3){
            //next page
            music.playef(11);
            if((size_t) ((game.levelpage*8)+8) >= ed.ListOfMetaData.size()){
                game.levelpage=0;
            }else{
                game.levelpage++;
            }
            game.createmenu(Menu::levellist, true);
            game.currentmenuoption=game.menuoptions.size()-3;
            map.nexttowercolour();
        }else{
            //Ok, launch the level!
            //PLAY CUSTOM LEVEL HOOK
            music.playef(11);
            game.playcustomlevel=(game.levelpage*8)+game.currentmenuoption;
            game.customleveltitle=ed.ListOfMetaData[game.playcustomlevel].title;
            game.customlevelfilename=ed.ListOfMetaData[game.playcustomlevel].filename;

            std::string name = "saves/" + ed.ListOfMetaData[game.playcustomlevel].filename.substr(7) + ".vvv";
            tinyxml2::XMLDocument doc;
            if (!FILESYSTEM_loadTiXml2Document(name.c_str(), doc)){
                game.mainmenu = 22;
                graphics.fademode = 2;
            }else{
                game.createmenu(Menu::quickloadlevel);
                map.nexttowercolour();
            }
        }
        break;
#endif
    case Menu::quickloadlevel:
        switch (game.currentmenuoption)
        {
        case 0: //continue save
            music.playef(11);
            game.mainmenu = 23;
            graphics.fademode = 2;
            break;
        case 1:
            music.playef(11);
            game.mainmenu = 22;
            graphics.fademode = 2;
            break;
        case 2:
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
#if !defined(NO_CUSTOM_LEVELS)
    case Menu::playerworlds:
 #if defined(NO_EDITOR)
  #define OFFSET -1
 #else
  #define OFFSET 0
 #endif
        switch (game.currentmenuoption)
        {
        case 0:

            music.playef(11);
            game.levelpage=0;
            ed.getDirectoryData();
            game.loadcustomlevelstats(); //Should only load a file if it's needed
            game.createmenu(Menu::levellist);
            map.nexttowercolour();
            break;
 #if !defined(NO_EDITOR)
        case 1:
            //LEVEL EDITOR HOOK
            music.playef(11);
            game.mainmenu = 20;
            graphics.fademode = 2;
            ed.filename="";
            break;
 #endif
        case OFFSET+2:
            //"OPENFOLDERHOOK"
            if (FILESYSTEM_openDirectoryEnabled()
            && FILESYSTEM_openDirectory(FILESYSTEM_getUserLevelDirectory()))
            {
                music.playef(11);
                SDL_MinimizeWindow(graphics.screenbuffer->m_window);
            }
            else
            {
                music.playef(2);
            }
            break;
        case OFFSET+3:
            //back
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
#undef OFFSET
        break;
#endif
    case Menu::errornostart:
        music.playef(11);
        game.createmenu(Menu::mainmenu);
        map.nexttowercolour();
        break;
    case Menu::graphicoptions:
        switch (game.currentmenuoption)
        {
        case 0:
            music.playef(11);
            graphics.screenbuffer->toggleFullScreen();
            game.savestats();

            // Recreate menu to update "resize to nearest"
            game.createmenu(game.currentmenuname, true);
            game.currentmenuoption = 0;
            break;
        case 1:
            music.playef(11);
            graphics.screenbuffer->toggleStretchMode();
            game.savestats();
            break;
        case 2:
            // resize to nearest multiple
            if (graphics.screenbuffer->isWindowed)
            {
                music.playef(11);
                graphics.screenbuffer->ResizeToNearestMultiple();
                game.savestats();
            }
            else
            {
                music.playef(2);
            }
            break;
        case 3:
            music.playef(11);
            graphics.screenbuffer->toggleLinearFilter();
            game.savestats();
            break;
        case 4:
            //change smoothing
            music.playef(11);
            graphics.screenbuffer->badSignalEffect= !graphics.screenbuffer->badSignalEffect;
            game.savestats();
            break;
        case 5:
            //toggle 30+ fps
            music.playef(11);
            game.over30mode = !game.over30mode;
            game.savestats();
            break;
        case 6:
            //toggle vsync
            music.playef(11);
#ifndef __HAIKU__ // FIXME: Remove after SDL VSync bug is fixed! -flibit
            graphics.screenbuffer->vsync = !graphics.screenbuffer->vsync;
            graphics.screenbuffer->resetRendererWorkaround();
            game.savestats();
#endif
            break;
        default:
            //back
            music.playef(11);
            if (game.ingame_titlemode)
            {
                game.returntopausemenu();
            }
            else
            {
                game.returnmenu();
                map.nexttowercolour();
            }
            break;
        }
        break;
    case Menu::youwannaquit:
        switch (game.currentmenuoption)
        {
        case 0:
            //bye!
            music.playef(2);
            game.mainmenu = 100;
            graphics.fademode = 2;
            break;
        default:
            music.playef(11);
            map.nexttowercolour(); //This goes first, otherwise mismatching continuemenu color is possible
            game.returnmenu();
        }
        break;
    case Menu::setinvincibility:
        switch (game.currentmenuoption)
        {
        case 0:
            //back
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
            break;
        default:
            map.invincibility = !map.invincibility;
            game.savestats();
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::setslowdown:
        switch (game.currentmenuoption)
        {
        case 0:
            //back
            game.slowdown = 30;
            game.savestats();
            music.playef(11);
            game.returnmenu();
            game.currentmenuoption = 4;
            map.nexttowercolour();
            break;
        case 1:
            game.slowdown = 24;
            game.savestats();
            music.playef(11);
            game.returnmenu();
            game.currentmenuoption = 4;
            map.nexttowercolour();
            break;
        case 2:
            game.slowdown = 18;
            game.savestats();
            music.playef(11);
            game.returnmenu();
            game.currentmenuoption = 4;
            map.nexttowercolour();
            break;
        case 3:
            game.slowdown = 12;
            game.savestats();
            music.playef(11);
            game.returnmenu();
            game.currentmenuoption = 4;
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::advancedoptions:
        switch (game.currentmenuoption)
        {
        case 0:
            //toggle mouse cursor
            music.playef(11);
            if (graphics.showmousecursor == true) {
                SDL_ShowCursor(SDL_DISABLE);
                graphics.showmousecursor = false;
            }
            else {
                SDL_ShowCursor(SDL_ENABLE);
                graphics.showmousecursor = true;
            }
            break;
        case 1:
            // toggle unfocus pause
            game.disablepause = !game.disablepause;
            music.playef(11);
            break;
        case 2:
            // toggle fake load screen
            game.skipfakeload = !game.skipfakeload;
            music.playef(11);
            break;
        case 3:
            // toggle translucent roomname BG
            graphics.translucentroomname = !graphics.translucentroomname;
            music.playef(11);
            break;
        case 4:
            // Glitchrunner mode
            music.playef(11);
            game.glitchrunnermode = !game.glitchrunnermode;
            break;
        case 5:
            //back
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::accessibility:
        switch (game.currentmenuoption)
        {
        case 0:
            //disable animated backgrounds
            game.colourblindmode = !game.colourblindmode;
            game.savestats();
            graphics.towerbg.tdrawback = true;
            graphics.titlebg.tdrawback = true;
            music.playef(11);
            break;
        case 1:
            //disable screeneffects
            game.noflashingmode = !game.noflashingmode;
            game.savestats();
            if (!game.noflashingmode)
            {
                music.playef(18);
                game.screenshake = 10;
                game.flashlight = 5;
            }else{
                music.playef(11);
            }
            break;
        case 2:
            //disable text outline
            graphics.notextoutline = !graphics.notextoutline;
            game.savestats();
            music.playef(11);
            break;
        case 3:
            //invincibility
            if (!game.ingame_titlemode || (!game.insecretlab && !game.intimetrial && !game.nodeathmode))
            {
                if (!map.invincibility)
                {
                    game.createmenu(Menu::setinvincibility);
                    map.nexttowercolour();
                }
                else
                {
                    map.invincibility = !map.invincibility;
                }
                music.playef(11);
            }
            else
            {
                music.playef(2);
                map.invincibility = false;
            }
            break;
        case 4:
            //change game speed
            if (!game.ingame_titlemode || (!game.insecretlab && !game.intimetrial && !game.nodeathmode))
            {
                game.createmenu(Menu::setslowdown);
                map.nexttowercolour();
                music.playef(11);
            }
            else
            {
                music.playef(2);
                game.slowdown = 30;
            }
            break;
        case 5:
            //back
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
            break;
        default:
            //Can't do yet! play sad sound
            music.playef(2);
            break;
        }
        break;
    case Menu::options:
    {
#if defined(MAKEANDPLAY)
        int flipmode_offset = 0;
#else
        int flipmode_offset = game.ingame_titlemode && game.unlock[18] ? 0 : -1;
#endif

#if defined(MAKEANDPLAY)
        int unlockmode_offset = -1;
#else
        int unlockmode_offset = 0;
#endif

        int mmmmmm_offset = music.mmmmmm ? 0 : -1;

        int offset = 0;

        switch (game.currentmenuoption)
        {
        case 0:
            //accessibility options
            music.playef(11);
            game.createmenu(Menu::accessibility);
            map.nexttowercolour();
            break;
        case 1:
            //advanced options
            music.playef(11);
            game.createmenu(Menu::advancedoptions);
            map.nexttowercolour();
            break;
        case 2:
#if !defined(MAKEANDPLAY)
        if (game.ingame_titlemode && game.unlock[18])
#endif
        {
            // toggle Flip Mode
            graphics.setflipmode = !graphics.setflipmode;
            game.savemystats = true;
            if (graphics.setflipmode)
            {
                music.playef(18);
                game.screenshake = 10;
                game.flashlight = 5;
            }
            else
            {
                music.playef(11);
            }
            // Fix wrong area music in Tower (Positive Force vs. ecroF evitisoP)
            if (map.custommode)
            {
                break;
            }
            int area = map.area(game.roomx, game.roomy);
            if (area == 3 || area == 11)
            {
                if (graphics.setflipmode)
                {
                    music.play(9); // ecroF evitisoP
                }
                else
                {
                    music.play(2); // Positive Force
                }
            }
        }
            break;
        }

        offset += flipmode_offset;

#if !defined(MAKEANDPLAY)
        if (game.currentmenuoption == 3+offset)
        {
            //unlock play options
            music.playef(11);
            game.createmenu(Menu::unlockmenu);
            map.nexttowercolour();
        }
#endif

        offset += unlockmode_offset;

        if (game.currentmenuoption == 4+offset)
        {
            //clear data menu
            music.playef(11);
            game.createmenu(Menu::controller);
            map.nexttowercolour();
            break;
        }
        else if (game.currentmenuoption == 5+offset)
        {
            //clear data menu
            music.playef(11);
            game.createmenu(Menu::cleardatamenu);
            map.nexttowercolour();
        }
        else if (game.currentmenuoption == 6+offset && music.mmmmmm)
        {
            //**** TOGGLE MMMMMM
            music.usingmmmmmm = !music.usingmmmmmm;
            music.playef(11);
            music.play(music.currentsong);
            game.savestats();
        }

        offset += mmmmmm_offset;

        if (game.currentmenuoption == 7+offset)
        {
            //back
            music.playef(11);
            if (game.ingame_titlemode)
            {
                game.returntopausemenu();
            }
            else
            {
                game.returnmenu();
                map.nexttowercolour();
            }
        }
        break;
    }
    case Menu::unlockmenutrials:
        switch (game.currentmenuoption)
        {
        case 0:   	//unlock 1
            game.unlock[9] = true;
            game.unlocknotify[9] = true;
            music.playef(11);
            game.savestats();
            game.createmenu(Menu::unlockmenutrials, true);
            game.currentmenuoption = 0;
            break;
        case 1:   	//unlock 2
            game.unlock[10] = true;
            game.unlocknotify[10] = true;
            music.playef(11);
            game.savestats();
            game.createmenu(Menu::unlockmenutrials, true);
            game.currentmenuoption = 1;
            break;
        case 2:   	//unlock 3
            game.unlock[11] = true;
            game.unlocknotify[11] = true;
            music.playef(11);
            game.savestats();
            game.createmenu(Menu::unlockmenutrials, true);
            game.currentmenuoption = 2;
            break;
        case 3:   	//unlock 4
            game.unlock[12] = true;
            game.unlocknotify[12] = true;
            music.playef(11);
            game.savestats();
            game.createmenu(Menu::unlockmenutrials, true);
            game.currentmenuoption = 3;
            break;
        case 4:   	//unlock 5
            game.unlock[13] = true;
            game.unlocknotify[13] = true;
            music.playef(11);
            game.savestats();
            game.createmenu(Menu::unlockmenutrials, true);
            game.currentmenuoption = 4;
            break;
        case 5:   	//unlock 6
            game.unlock[14] = true;
            game.unlocknotify[14] = true;
            music.playef(11);
            game.savestats();
            game.createmenu(Menu::unlockmenutrials, true);
            game.currentmenuoption = 5;
            break;
        case 6:   	//back
            //back
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::unlockmenu:
        switch (game.currentmenuoption)
        {
        case 0:
            //unlock time trials separately...
            music.playef(11);
            game.createmenu(Menu::unlockmenutrials);
            map.nexttowercolour();
            break;
        case 1:
            //unlock intermissions
            music.playef(11);
            game.unlock[16] = true;
            game.unlocknotify[16] = true;
            game.unlock[6] = true;
            game.unlock[7] = true;
            game.savestats();
            game.createmenu(Menu::unlockmenu, true);
            game.currentmenuoption = 1;
            break;
        case 2:
            //unlock no death mode
            music.playef(11);
            game.unlock[17] = true;
            game.unlocknotify[17] = true;
            game.savestats();
            game.createmenu(Menu::unlockmenu, true);
            game.currentmenuoption = 2;
            break;
        case 3:
            //unlock flip mode
            music.playef(11);
            game.unlock[18] = true;
            game.unlocknotify[18] = true;
            game.savestats();
            game.createmenu(Menu::unlockmenu, true);
            game.currentmenuoption = 3;
            break;
        case 4:
            //unlock jukebox
            music.playef(11);
            game.stat_trinkets = 20;
            game.savestats();
            game.createmenu(Menu::unlockmenu, true);
            game.currentmenuoption = 4;
            break;
        case 5:
            //unlock secret lab
            music.playef(11);
            game.unlock[8] = true;
            game.unlocknotify[8] = true;
            game.savestats();
            game.createmenu(Menu::unlockmenu, true);
            game.currentmenuoption = 5;
            break;
        default:
            //back
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::credits:
        switch (game.currentmenuoption)
        {
        case 0:
            //next page
            music.playef(11);
            game.createmenu(Menu::credits2, true);
            map.nexttowercolour();
            break;
        case 1:
            //last page
            music.playef(11);
            game.createmenu(Menu::credits6, true);
            game.currentmenuoption = 1;
            map.nexttowercolour();
            break;
        default:
            //back
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::credits2:
        switch (game.currentmenuoption)
        {
        case 0:
            //next page
            music.playef(11);
            game.createmenu(Menu::credits25, true);
            map.nexttowercolour();
            break;
        case 1:
            //previous page
            music.playef(11);
            game.createmenu(Menu::credits, true);
            game.currentmenuoption = 1;
            map.nexttowercolour();
            break;
        case 2:
            //back
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::credits25:
        switch (game.currentmenuoption)
        {
        case 0:
            //next page
            music.playef(11);
            game.createmenu(Menu::credits3, true);
            map.nexttowercolour();
            break;
        case 1:
            //previous page
            music.playef(11);
            game.createmenu(Menu::credits2, true);
            game.currentmenuoption = 1;
            map.nexttowercolour();
            break;
        default:
            //back
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::credits3:
        switch (game.currentmenuoption)
        {
        case 0:
            //next page
            music.playef(11);
            game.current_credits_list_index += 9;

            if (game.current_credits_list_index >= (int)SDL_arraysize(Credits::superpatrons))
            {
                // No more super patrons. Move to the next credits section
                game.current_credits_list_index = 0;
                game.createmenu(Menu::credits4, true);
            }
            else
            {
                // There are more super patrons. Refresh the menu with the next ones
                game.createmenu(Menu::credits3, true);
            }

            map.nexttowercolour();
            break;
        case 1:
            //previous page
            music.playef(11);
            game.current_credits_list_index -= 9;

            if (game.current_credits_list_index < 0)
            {
                //No more super patrons. Move to the previous credits section
                game.current_credits_list_index = 0;
                game.createmenu(Menu::credits25, true);
            }
            else
            {
                //There are more super patrons. Refresh the menu with the next ones
                game.createmenu(Menu::credits3, true);
            }
            game.currentmenuoption = 1;

            map.nexttowercolour();
            break;
        default:
            //back
            music.playef(11);
            game.current_credits_list_index = 0;
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::credits4:
        switch (game.currentmenuoption)
        {
        case 0:
            //next page
            music.playef(11);
            game.current_credits_list_index += 14;

            if (game.current_credits_list_index >= (int)SDL_arraysize(Credits::patrons))
            {
                // No more patrons. Move to the next credits section
                game.current_credits_list_index = 0;
                game.createmenu(Menu::credits5, true);
            }
            else
            {
                // There are more patrons. Refresh the menu with the next ones
                game.createmenu(Menu::credits4, true);
            }

            map.nexttowercolour();
            break;
        case 1:
            //previous page
            music.playef(11);
            game.current_credits_list_index -= 14;

            if (game.current_credits_list_index < 0)
            {
                //No more patrons. Move to the previous credits section
                game.current_credits_list_index = SDL_arraysize(Credits::superpatrons) - 1 - (SDL_arraysize(Credits::superpatrons)-1)%9;
                game.createmenu(Menu::credits3, true);
            }
            else
            {
                //There are more patrons. Refresh the menu with the next ones
                game.createmenu(Menu::credits4, true);
            }
            game.currentmenuoption = 1;

            map.nexttowercolour();
            break;
        default:
            //back
            music.playef(11);
            game.current_credits_list_index = 0;
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::credits5:
        switch (game.currentmenuoption)
        {
        case 0:
            //next page
            music.playef(11);
            game.current_credits_list_index += 9;

            if (game.current_credits_list_index >= (int)SDL_arraysize(Credits::githubfriends))
            {
                // No more GitHub contributors. Move to the next credits section
                game.current_credits_list_index = 0;
                game.createmenu(Menu::credits6, true);
            }
            else
            {
                // There are more GitHub contributors. Refresh the menu with the next ones
                game.createmenu(Menu::credits5, true);
            }

            map.nexttowercolour();
            break;
        case 1:
            //previous page
            music.playef(11);
            game.current_credits_list_index -= 9;

            if (game.current_credits_list_index < 0)
            {
                //No more GitHub contributors. Move to the previous credits section
                game.current_credits_list_index = SDL_arraysize(Credits::patrons) - 1 - (SDL_arraysize(Credits::patrons)-1)%14;
                game.createmenu(Menu::credits4, true);
            }
            else
            {
                //There are more GitHub contributors. Refresh the menu with the next ones
                game.createmenu(Menu::credits5, true);
            }
            game.currentmenuoption = 1;

            map.nexttowercolour();
            break;
        default:
            //back
            music.playef(11);
            game.current_credits_list_index = 0;
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::credits6:
        switch (game.currentmenuoption)
        {
        case 0:
            //first page
            music.playef(11);
            game.createmenu(Menu::credits, true);
            map.nexttowercolour();
            break;
        case 1:
            //previous page
            music.playef(11);
            game.current_credits_list_index = SDL_arraysize(Credits::githubfriends) - 1 - (SDL_arraysize(Credits::githubfriends)-1)%9;
            game.createmenu(Menu::credits5, true);
            game.currentmenuoption = 1;
            map.nexttowercolour();
            break;
        default:
            //back
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
            music.niceplay(6);
            break;
        }
        break;
    case Menu::play:
    {
        //Do we have the Secret Lab option?
        int sloffset = game.unlock[8] ? 0 : -1;
        //Do we have a telesave or quicksave?
        int ngoffset = game.save_exists() ? 0 : -1;
        if (game.currentmenuoption == 0)
        {
            //continue
            //right, this depends on what saves you've got
            if (!game.save_exists())
            {
                //You have no saves but have something unlocked, or you couldn't have gotten here
                music.playef(11);
                game.mainmenu = 0;
                graphics.fademode = 2;
            }
            else if (game.telesummary == "")
            {
                //You at least have a quicksave, or you couldn't have gotten here
                music.playef(11);
                game.mainmenu = 2;
                graphics.fademode = 2;
            }
            else if (game.quicksummary == "")
            {
                //You at least have a telesave, or you couldn't have gotten here
                music.playef(11);
                game.mainmenu = 1;
                graphics.fademode = 2;
            }
            else
            {
                //go to a menu!
                music.playef(11);
                game.loadsummary(); //Prepare save slots to display
                game.createmenu(Menu::continuemenu);
            }
        }
        else if (game.currentmenuoption == 1 && game.unlock[8])
        {
            if(!map.invincibility && game.slowdown == 30){
                music.playef(11);
                game.mainmenu = 11;
                graphics.fademode = 2;
            }else{
                //Can't do yet! play sad sound
                music.playef(2);
            }
        }
        else if (game.currentmenuoption == sloffset+2)
        {
            //play modes
            music.playef(11);
            game.createmenu(Menu::playmodes);
            map.nexttowercolour();
        }
        else if (game.currentmenuoption == sloffset+3 && game.save_exists())
        {
            //newgame
            music.playef(11);
            game.createmenu(Menu::newgamewarning);
            map.nexttowercolour();
        }
        else if (game.currentmenuoption == sloffset+ngoffset+4)
        {
            //back
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
        }
        break;
    }
    case Menu::newgamewarning:
        switch (game.currentmenuoption)
        {
        case 0:
            //yep
            music.playef(11);
            game.mainmenu = 0;
            graphics.fademode = 2;
            game.deletequick();
            game.deletetele();
            break;
        default:
            //back
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;

    case Menu::controller:
        switch (game.currentmenuoption)
        {
        case 0:
            key.sensitivity++;
            music.playef(11);
            if(key.sensitivity > 4)
            {
                key.sensitivity = 0;
            }
            break;

        case 5:
            music.playef(11);
            game.returnmenu();
            break;
        }
        break;
    case Menu::cleardatamenu:
        switch (game.currentmenuoption)
        {
        case 0:
            //back
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
            break;
        default:
            //yep
            music.playef(23);
            game.deletequick();
            game.deletetele();
            game.deletestats();
            game.deletesettings();
            game.flashlight = 5;
            game.screenshake = 15;
            game.createmenu(Menu::mainmenu);
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::playmodes:
        if (game.currentmenuoption == 0 && game.slowdown == 30 && !map.invincibility)   //go to the time trial menu
        {
            music.playef(11);
            game.createmenu(Menu::timetrials);
            map.nexttowercolour();
        }
        else if (game.currentmenuoption == 1 && game.unlock[16])
        {
            //intermission mode menu
            music.playef(11);
            game.createmenu(Menu::intermissionmenu);
            map.nexttowercolour();
        }
        else if (game.currentmenuoption == 2 && game.unlock[17] && game.slowdown == 30 && !map.invincibility)    //start a game in no death mode
        {
            music.playef(11);
            game.createmenu(Menu::startnodeathmode);
            map.nexttowercolour();
        }
        else if (game.currentmenuoption == 3 && game.unlock[18])    //enable/disable flip mode
        {
            // WARNING: Partially duplicated in Menu::options
            graphics.setflipmode = !graphics.setflipmode;
            game.savemystats = true;
            if (graphics.setflipmode)
            {
                music.playef(18);
                game.screenshake = 10;
                game.flashlight = 5;
            }
            else
            {
                music.playef(11);
            }
        }
        else if (game.currentmenuoption == 4)
        {
            //back
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
        }
        else
        {
            //Can't do yet! play sad sound
            music.playef(2);
        }
        break;
    case Menu::startnodeathmode:
        switch (game.currentmenuoption)
        {
        case 0:   //start no death mode, disabling cutscenes
            music.playef(11);
            game.mainmenu = 10;
            graphics.fademode = 2;
            break;
        case 1:
            music.playef(11);
            game.mainmenu = 9;
            graphics.fademode = 2;
            break;
        case 2:
            //back
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::continuemenu:
        switch (game.currentmenuoption)
        {
        case 0:
            music.playef(11);
            game.mainmenu = 1;
            graphics.fademode = 2;
            break;
        case 1:
            music.playef(11);
            game.mainmenu = 2;
            graphics.fademode = 2;
            break;
        case 2:
            //back
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::intermissionmenu:
        switch (game.currentmenuoption)
        {
        case 0:
            music.playef(11);
            music.play(6);
            game.createmenu(Menu::playint1);
            map.nexttowercolour();
            break;
        case 1:
            music.playef(11);
            music.play(6);
            game.createmenu(Menu::playint2);
            map.nexttowercolour();
            break;
        case 2:
            //back
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::playint1:
        switch (game.currentmenuoption)
        {
        case 0:
            music.playef(11);
            game.mainmenu = 12;
            graphics.fademode = 2;
            break;
        case 1:
            music.playef(11);
            game.mainmenu = 13;
            graphics.fademode = 2;
            break;
        case 2:
            music.playef(11);
            game.mainmenu = 14;
            graphics.fademode = 2;
            break;
        case 3:
            music.playef(11);
            game.mainmenu = 15;
            graphics.fademode = 2;
            break;
        case 4:
            //back
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::playint2:
        switch (game.currentmenuoption)
        {
        case 0:
            music.playef(11);
            game.mainmenu = 16;
            graphics.fademode = 2;
            break;
        case 1:
            music.playef(11);
            game.mainmenu = 17;
            graphics.fademode = 2;
            break;
        case 2:
            music.playef(11);
            game.mainmenu = 18;
            graphics.fademode = 2;
            break;
        case 3:
            music.playef(11);
            game.mainmenu = 19;
            graphics.fademode = 2;
            break;
        case 4:
            //back
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::gameover2:
        //back
        music.playef(11);
        music.play(6);
        game.returntomenu(Menu::playmodes);
        map.nexttowercolour();
        break;
    case Menu::unlocktimetrials:
    case Menu::unlocktimetrial:
    case Menu::unlocknodeathmode:
    case Menu::unlockintermission:
    case Menu::unlockflipmode:
        //back
        music.playef(11);
        game.createmenu(Menu::play, true);
        map.nexttowercolour();
        break;
    case Menu::timetrials:
        if (game.currentmenuoption == 0 && game.unlock[9])   //space station 1
        {
            music.playef(11);
            game.mainmenu = 3;
            graphics.fademode = 2;
        }
        else if (game.currentmenuoption == 1 && game.unlock[10])    //lab
        {
            music.playef(11);
            game.mainmenu = 4;
            graphics.fademode = 2;
        }
        else if (game.currentmenuoption == 2 && game.unlock[11])    //tower
        {
            music.playef(11);
            game.mainmenu = 5;
            graphics.fademode = 2;
        }
        else if (game.currentmenuoption == 3 && game.unlock[12])    //station 2
        {
            music.playef(11);
            game.mainmenu = 6;
            graphics.fademode = 2;
        }
        else if (game.currentmenuoption == 4 && game.unlock[13])    //warp
        {
            music.playef(11);
            game.mainmenu = 7;
            graphics.fademode = 2;
        }
        else if (game.currentmenuoption == 5 && game.unlock[14])    //final
        {
            music.playef(11);
            game.mainmenu = 8;
            graphics.fademode = 2;
        }
        else if (game.currentmenuoption == 6)    //go to the time trial menu
        {
            //back
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
        }
        else
        {
            //Can't do yet! play sad sound
            music.playef(2);
        }
        break;
    case Menu::timetrialcomplete3:
        switch (game.currentmenuoption)
        {
        case 0:
            //back
            music.playef(11);
            music.play(6);
            game.returntomenu(Menu::timetrials);
            map.nexttowercolour();
            break;
        case 1:
            //Ok but first quickly remove the last stack frame to prevent piling up timetrialcomplete stack frames
            if (game.menustack.empty())
            {
                puts("Error: menu stack is empty!");
            }
            else
            {
                game.menustack.pop_back();
            }
            //duplicate the above based on given time trial level!
            if (game.timetriallevel == 0)   //space station 1
            {
                music.playef(11);
                game.mainmenu = 3;
                graphics.fademode = 2;
            }
            else if (game.timetriallevel == 1)    //lab
            {
                music.playef(11);
                game.mainmenu = 4;
                graphics.fademode = 2;
            }
            else if (game.timetriallevel == 2)    //tower
            {
                music.playef(11);
                game.mainmenu = 5;
                graphics.fademode = 2;
            }
            else if (game.timetriallevel == 3)    //station 2
            {
                music.playef(11);
                game.mainmenu = 6;
                graphics.fademode = 2;
            }
            else if (game.timetriallevel == 4)    //warp
            {
                music.playef(11);
                game.mainmenu = 7;
                graphics.fademode = 2;
            }
            else if (game.timetriallevel == 5)    //final
            {
                music.playef(11);
                game.mainmenu = 8;
                graphics.fademode = 2;
            }
            break;
        }
        break;
    case Menu::gamecompletecontinue:
    case Menu::nodeathmodecomplete2:
        music.play(6);
        music.playef(11);
        game.returnmenu();
        map.nexttowercolour();
        break;
    default:
        break;
    }
}

void titleinput()
{
    //game.mx = (mouseX / 4);
    //game.my = (mouseY / 4);

    game.press_left = false;
    game.press_right = false;
    game.press_action = false;
    game.press_map = false;

    if (graphics.flipmode)
    {
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

    if (!game.jumpheld && graphics.fademode==0)
    {
        if (game.press_action || game.press_left || game.press_right || game.press_map)
        {
            game.jumpheld = true;
        }

        if (key.isDown(27) && game.currentmenuname != Menu::youwannaquit && game.menustart)
        {
            music.playef(11);
            if (game.ingame_titlemode)
            {
                game.returntopausemenu();
            }
            else
            {
                game.createmenu(Menu::youwannaquit);
                map.nexttowercolour();
            }
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

        if (game.currentmenuoption < 0) game.currentmenuoption = game.menuoptions.size()-1;
        if (game.currentmenuoption >= (int) game.menuoptions.size() ) game.currentmenuoption = 0;

        if (game.press_action)
        {
            if (!game.menustart)
            {
                game.menustart = true;
                music.play(6);
                music.playef(18);
                game.screenshake = 10;
                game.flashlight = 5;
                graphics.titlebg.colstate = 10;
                map.nexttowercolour();
            }
            else
            {
                menuactionpress();
            }
        }
        if (    game.currentmenuname == Menu::controller &&
                game.currentmenuoption > 0 &&
                game.currentmenuoption < 5 &&
                key.controllerButtonDown()      )
        {
            updatebuttonmappings(game.currentmenuoption);
        }

    }

    if (graphics.fademode == 1)
        script.startgamemode(game.mainmenu);
}

void gameinput()
{
    //TODO mouse input
    //game.mx = (mouseX / 2);
    //game.my = (mouseY / 2);

    if(!script.running)
    {
        game.press_left = false;
        game.press_right = false;
        game.press_action = false;

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
    }

    game.press_map = false;
    if (key.isDown(KEYBOARD_ENTER) || key.isDown(SDLK_KP_ENTER) || key.isDown(game.controllerButton_map)  )
    {
        game.press_map = true;
    }

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
                if(game.glitchrunnermode || !game.glitchrunkludge) game.state++;
                    game.jumpheld = true;
                    game.glitchrunkludge=true;
                    //Bug fix! You should only be able to do this ONCE.
                    //...Unless you're in glitchrunner mode
            }
        }
    }

    if (!game.press_map) game.mapheld = false;

    if (game.intimetrial && graphics.fademode == 1 && game.quickrestartkludge)
    {
        //restart the time trial
        game.quickrestartkludge = false;
        script.startgamemode(game.timetriallevel + 3);
        game.deathseq = -1;
        game.completestop = false;
    }

    //Returning to editor mode must always be possible
#if !defined(NO_CUSTOM_LEVELS)
    if(map.custommode && !map.custommodeforreal){
        if ((game.press_map || key.isDown(27)) && !game.mapheld){
            //Return to level editor
            if (INBOUNDS_VEC(game.activeactivity, obj.blocks) && game.press_map){
                //pass, let code block below handle it
            }else if(game.activetele && game.readytotele > 20 && game.press_map){
                //pass, let code block below handle it
            }else{
                game.shouldreturntoeditor = true;
                game.mapheld = true;
            }
        }
    }
#endif

    //Entity type 0 is player controled
    bool has_control = false;
    bool enter_pressed = game.press_map && !game.mapheld;
    bool enter_already_processed = false;
    for (size_t ie = 0; ie < obj.entities.size(); ++ie)
    {
        if (obj.entities[ie].rule == 0)
        {
            if (game.hascontrol && game.deathseq == -1 && game.lifeseq <= 5)
            {
                has_control = true;
                if (enter_pressed)
                {
                    game.mapheld = true;

                    if (game.activetele && game.readytotele > 20 && !game.intimetrial)
                    {
                        enter_already_processed = true;
                        if(int(std::abs(obj.entities[ie].vx))<=1 && int(obj.entities[ie].vy)==0)
                        {
                            //wait! space station 2 debug thingy
                            if (game.teleportscript != "")
                            {

                                //trace(game.recordstring);
                                //We're teleporting! Yey!
                                game.activetele = false;
                                game.hascontrol = false;
                                music.fadeout();

                                int player = obj.getplayer();
                                if (INBOUNDS_VEC(player, obj.entities))
                                {
                                    obj.entities[player].colour = 102;
                                }

                                int teleporter = obj.getteleporter();
                                if (INBOUNDS_VEC(teleporter, obj.entities))
                                {
                                    obj.entities[teleporter].tile = 6;
                                    obj.entities[teleporter].colour = 102;
                                }
                                //which teleporter script do we use? it depends on the companion!
                                game.state = 4000;
                                game.statedelay = 0;
                            }
                            else if (game.companion == 0)
                            {
                                //Alright, normal teleporting
                                game.gamestate = TELEPORTERMODE;
                                graphics.menuoffset = 240; //actually this should count the roomname
                                graphics.oldmenuoffset = 240;
                                if (map.extrarow)
                                {
                                    graphics.menuoffset -= 10;
                                    graphics.oldmenuoffset -= 10;
                                }

                                BlitSurfaceStandard(graphics.menubuffer,NULL,graphics.backBuffer, NULL);

                                graphics.resumegamemode = false;

                                game.useteleporter = true;
                                game.initteleportermode();
                            }
                            else
                            {
                                //We're teleporting! Yey!
                                game.activetele = false;
                                game.hascontrol = false;
                                music.fadeout();

                                int player = obj.getplayer();
                                if (INBOUNDS_VEC(player, obj.entities))
                                {
                                    obj.entities[player].colour = 102;
                                }
                                int companion = obj.getcompanion();
                                if(INBOUNDS_VEC(companion, obj.entities)) obj.entities[companion].colour = 102;

                                int teleporter = obj.getteleporter();
                                if (INBOUNDS_VEC(teleporter, obj.entities))
                                {
                                    obj.entities[teleporter].tile = 6;
                                    obj.entities[teleporter].colour = 102;
                                }
                                //which teleporter script do we use? it depends on the companion!
                                game.state = 3000;
                                game.statedelay = 0;
                            }
                        }
                    }
                    else if (INBOUNDS_VEC(game.activeactivity, obj.blocks))
                    {
                        enter_already_processed = true;
                        if((int(std::abs(obj.entities[ie].vx))<=1) && (int(obj.entities[ie].vy) == 0) )
                        {
                            script.load(obj.blocks[game.activeactivity].script);
                            obj.removeblock(game.activeactivity);
                            game.activeactivity = -1;
                        }
                    }
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
                    obj.entities[ie].ax = -3;
                    obj.entities[ie].dir = 0;
                }
                else if (game.press_right)
                {
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
                        music.playef(0);
                        game.jumppressed = 0;
                        game.totalflips++;
                    }
                    if (obj.entities[ie].onroof>0 && game.gravitycontrol == 1)
                    {
                        game.gravitycontrol = 0;
                        obj.entities[ie].vy = 4;
                        obj.entities[ie].ay = 3;
                        music.playef(1);
                        game.jumppressed = 0;
                        game.totalflips++;
                    }
                }
            }
        }
    }

    if (!has_control)
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

    // Continuation of Enter processing. The rest of the if-tree runs only if
    // enter_pressed && !enter_already_pressed
    if (!enter_pressed || enter_already_processed)
    {
        // Do nothing
    }
    else if (game.swnmode == 1 && game.swngame == 1)
    {
        //quitting the super gravitron
        game.mapheld = true;
        //Quit menu, same conditions as in game menu
        game.gamestate = MAPMODE;
        game.gamesaved = false;
        game.gamesavefailed = false;
        graphics.resumegamemode = false;
        game.menupage = 20; // The Map Page
        BlitSurfaceStandard(graphics.menubuffer,NULL,graphics.backBuffer, NULL);
        graphics.menuoffset = 240; //actually this should count the roomname
        graphics.oldmenuoffset = 240;
        if (map.extrarow)
        {
            graphics.menuoffset -= 10;
            graphics.oldmenuoffset -= 10;
        }
    }
    else if (game.intimetrial && graphics.fademode == 0)
    {
        //Quick restart of time trial
        graphics.fademode = 2;
        game.completestop = true;
        music.fadeout();
        game.quickrestartkludge = true;
    }
    else if (game.intimetrial)
    {
        //Do nothing if we're in a Time Trial but a fade animation is playing
    }
    else
    {
        //Normal map screen, do transition later
        game.gamestate = MAPMODE;
        map.cursordelay = 0;
        map.cursorstate = 0;
        game.gamesaved = false;
        game.gamesavefailed = false;
        graphics.resumegamemode = false;
        game.menupage = 0; // The Map Page
        BlitSurfaceStandard(graphics.menubuffer,NULL,graphics.backBuffer, NULL);
        graphics.menuoffset = 240; //actually this should count the roomname
        graphics.oldmenuoffset = 240;
        if (map.extrarow)
        {
            graphics.menuoffset -= 10;
            graphics.oldmenuoffset -= 10;
        }
    }

    if ((key.isDown(27) || key.isDown(game.controllerButton_esc)) && (!map.custommode || map.custommodeforreal))
    {
        game.mapheld = true;
        //Quit menu, same conditions as in game menu
        game.gamestate = MAPMODE;
        game.gamesaved = false;
        game.gamesavefailed = false;
        graphics.resumegamemode = false;
        game.menupage = 30; // Pause screen

        BlitSurfaceStandard(graphics.menubuffer,NULL,graphics.backBuffer, NULL);
        graphics.menuoffset = 240; //actually this should count the roomname
        graphics.oldmenuoffset = 240;
        if (map.extrarow)
        {
            graphics.menuoffset -= 10;
            graphics.oldmenuoffset -= 10;
        }
    }

    if (game.deathseq == -1 && (key.isDown(SDLK_r) || key.isDown(game.controllerButton_restart)) && !game.nodeathmode)// && map.custommode) //Have fun glitchrunners!
    {
        game.deathseq = 30;
    }
}

void mapmenuactionpress();

void mapinput()
{
    //TODO Mouse Input!
    //game.mx = (mouseX / 2);
    //game.my = (mouseY / 2);

    game.press_left = false;
    game.press_right = false;
    game.press_action = false;
    game.press_map = false;

    if (game.glitchrunnermode && graphics.fademode == 1 && graphics.menuoffset == 0)
    {
        // Deliberate re-addition of the glitchy gamestate-based fadeout!

        // First of all, detecting a black screen means if the glitchy fadeout
        // gets interrupted but you're still on a black screen, opening a menu
        // immediately quits you to the title. This has the side effect that if
        // you accidentally press Esc during a cutscene when it's black, you'll
        // immediately be quit and lose all your progress, but that's fair in
        // glitchrunner mode.
        // Also have to check graphics.menuoffset so this doesn't run every frame

        // Have to close the menu in order to run gamestates
        graphics.resumegamemode = true;
        // Remove half-second delay
        graphics.menuoffset = 250;

        // Technically this was in <=2.2 as well
        obj.removeallblocks();

        if (game.menupage >= 20 && game.menupage <= 21)
        {
            game.state = 96;
            game.statedelay = 0;
        }
        else
        {
            // Produces more glitchiness! Necessary for credits warp to work.
            script.running = false;
            graphics.textbox.clear();

            game.state = 80;
            game.statedelay = 0;
        }
    }

    if (game.fadetomenu && !game.glitchrunnermode)
    {
        if (game.fadetomenudelay > 0)
        {
            game.fadetomenudelay--;
        }
        else
        {
            game.quittomenu();
            game.fadetomenu = false;
        }
    }

    if (game.fadetolab && !game.glitchrunnermode)
    {
        if (game.fadetolabdelay > 0)
        {
            game.fadetolabdelay--;
        }
        else
        {
            game.returntolab();
            game.fadetolab = false;
        }
    }

    if(graphics.menuoffset==0
    && ((!game.glitchrunnermode && !game.fadetomenu && game.fadetomenudelay <= 0 && !game.fadetolab && game.fadetolabdelay <= 0)
    || graphics.fademode == 0))
    {
        if (graphics.flipmode)
        {
            if (key.isDown(KEYBOARD_LEFT) || key.isDown(KEYBOARD_DOWN) || key.isDown(KEYBOARD_a) ||  key.isDown(KEYBOARD_s) || key.controllerWantsLeft(true) ) game.press_left = true;
            if (key.isDown(KEYBOARD_RIGHT) || key.isDown(KEYBOARD_UP) || key.isDown(KEYBOARD_d) ||  key.isDown(KEYBOARD_w) || key.controllerWantsRight(true)) game.press_right = true;
        }
        else
        {
            if (key.isDown(KEYBOARD_LEFT) || key.isDown(KEYBOARD_UP) || key.isDown(KEYBOARD_a) ||  key.isDown(KEYBOARD_w)|| key.controllerWantsLeft(true))
            {
                game.press_left = true;
            }
            if (key.isDown(KEYBOARD_RIGHT) || key.isDown(KEYBOARD_DOWN) || key.isDown(KEYBOARD_d) ||  key.isDown(KEYBOARD_s)|| key.controllerWantsRight(true))
            {
                game.press_right = true;
            }
        }
        if (key.isDown(KEYBOARD_z) || key.isDown(KEYBOARD_SPACE) || key.isDown(KEYBOARD_v) || key.isDown(game.controllerButton_flip))
        {
            game.press_action = true;
        }
        if (game.menupage < 12 || (game.menupage >= 30 && game.menupage <= 33))
        {
            if (key.isDown(KEYBOARD_ENTER) || key.isDown(game.controllerButton_map) ) game.press_map = true;
            if (key.isDown(27) && !game.mapheld)
            {
                game.mapheld = true;
                if (game.menupage < 9)
                {
                    game.menupage = 30;
                }
                else if (game.menupage < 12)
                {
                    game.menupage = 31;
                }
                else
                {
                    graphics.resumegamemode = true;
                }
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
            graphics.resumegamemode = true;
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

        if (game.press_action)
        {
            mapmenuactionpress();
        }

        if (game.menupage < 0) game.menupage = 3;
        if (game.menupage > 3 && game.menupage < 9) game.menupage = 0;

        if (game.menupage == 9) game.menupage = 11;
        if (game.menupage == 12) game.menupage = 10;

        if (game.menupage == 19) game.menupage = 21;
        if (game.menupage == 22) game.menupage = 20;

        if (game.menupage == 29) game.menupage = 33;
        if (game.menupage == 34) game.menupage = 30;
    }
}

void mapmenuactionpress()
{
    switch (game.menupage)
    {
    case 1:
    if (obj.flags[67] && !game.inspecial() && !map.custommode)
    {
        //Warp back to the ship
        graphics.resumegamemode = true;

        game.teleport_to_x = 2;
        game.teleport_to_y = 11;

        //trace(game.recordstring);
        //We're teleporting! Yey!
        game.activetele = false;
        game.hascontrol = false;

        int i = obj.getplayer();
        if (INBOUNDS_VEC(i, obj.entities))
        {
            obj.entities[i].colour = 102;
        }

        //which teleporter script do we use? it depends on the companion!
        game.state = 4000;
        game.statedelay = 0;
    }
        break;
    case 3:
    if (!game.gamesaved && !game.gamesavefailed && !game.inspecial())
    {
        game.flashlight = 5;
        game.screenshake = 10;
        music.playef(18);

        game.savetime = game.timestring();
        game.savearea = map.currentarea(map.area(game.roomx, game.roomy));
        game.savetrinkets = game.trinkets();

        if (game.roomx >= 102 && game.roomx <= 104 && game.roomy >= 110 && game.roomy <= 111) game.savearea = "The Ship";

        bool success;
#if !defined(NO_CUSTOM_LEVELS)
        if(map.custommodeforreal)
        {
            success = game.customsavequick(ed.ListOfMetaData[game.playcustomlevel].filename);
        }
        else
#endif
        {
            success = game.savequick();
        }
        game.gamesaved = success;
        game.gamesavefailed = !success;
    }
        break;

    case 10:
        //return to pause menu
        music.playef(11);
        game.menupage = 31;
        break;
    case 11:
        //quit to menu

        //Kill contents of offset render buffer, since we do that for some reason.
        //This fixes an apparent frame flicker.
        FillRect(graphics.tempBuffer, 0x000000);
        graphics.fademode = 2;
        if (music.currentsong != 6) {
            music.fadeout();
        }
        map.nexttowercolour();
        if (!game.glitchrunnermode)
        {
            game.fadetomenu = true;
            game.fadetomenudelay = 16;
        }
        break;

    case 20:
        //return to game
        graphics.resumegamemode = true;
        break;
    case 21:
        //quit to menu
        game.swnmode = false;
        graphics.fademode = 2;
        music.fadeout();
        if (!game.glitchrunnermode)
        {
            game.fadetolab = true;
            game.fadetolabdelay = 16;
        }
        break;
    case 30:
        // Return to game
        graphics.resumegamemode = true;
        break;
    case 31:
        // Go to quit prompt
        music.playef(11);
        game.menupage = 10;
        break;
    case 32:
    case 33:
        // Graphic options and game options
        music.playef(11);
        game.gamestate = TITLEMODE;
        graphics.flipmode = false;
        game.ingame_titlemode = true;
        if (game.menupage == 32)
        {
            game.createmenu(Menu::graphicoptions);
        }
        else
        {
            game.createmenu(Menu::options);
        }
        game.kludge_ingametemp = game.currentmenuname;

        map.nexttowercolour();

        // Fix delta rendering glitch
        graphics.updatetowerbackground(graphics.titlebg);
        titleupdatetextcol();
        break;
    }
}

void teleporterinput()
{
    //Todo Mouseinput!
    //game.mx = (mouseX / 2);
    //game.my = (mouseY / 2);

    int tempx, tempy;

    game.press_left = false;
    game.press_right = false;
    game.press_action = false;
    game.press_map = false;

    if(graphics.menuoffset==0)
    {
        if (key.isDown(KEYBOARD_LEFT)|| key.isDown(KEYBOARD_a) || key.controllerWantsLeft(false) ) game.press_left = true;
        if (key.isDown(KEYBOARD_RIGHT) || key.isDown(KEYBOARD_d)|| key.controllerWantsRight(false) ) game.press_right = true;
        if (key.isDown(KEYBOARD_z) || key.isDown(KEYBOARD_SPACE) || key.isDown(KEYBOARD_v)
                || key.isDown(KEYBOARD_UP) || key.isDown(KEYBOARD_DOWN)||  key.isDown(KEYBOARD_w)||  key.isDown(KEYBOARD_s) || key.isDown(game.controllerButton_flip)) game.press_action = true;
        if (key.isDown(KEYBOARD_ENTER) || key.isDown(game.controllerButton_map)) game.press_map = true;

        //In the menu system, all keypresses are single taps rather than holds. Therefore this test has to be done for all presses
        if (!game.press_action && !game.press_left && !game.press_right) game.jumpheld = false;
        if (!game.press_map) game.mapheld = false;

        if (key.isDown(27))
        {
            if (!map.custommode || map.custommodeforreal)
            {
                // Go to pause menu
                game.mapheld = true;
                game.menupage = 30;
                game.gamestate = MAPMODE;
            }
            else
            {
                // Close teleporter menu
                graphics.resumegamemode = true;
            }
        }
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

        bool any_tele_unlocked = false;
        if (game.press_left || game.press_right)
        {
            for (size_t i = 0; i < map.teleporters.size(); i++)
            {
                point& tele = map.teleporters[i];

                if (map.explored[tele.x + tele.y*20])
                {
                    any_tele_unlocked = true;
                    break;
                }
            }
        }

        if (game.press_left && any_tele_unlocked)
        {
            do
            {
                game.teleport_to_teleporter--;
                if (game.teleport_to_teleporter < 0) game.teleport_to_teleporter = map.teleporters.size() - 1;
                tempx = map.teleporters[game.teleport_to_teleporter].x;
                tempy = map.teleporters[game.teleport_to_teleporter].y;
            }
            while (map.explored[tempx + (20 * tempy)] == 0);
        }
        else if (game.press_right && any_tele_unlocked)
        {
            do
            {
                game.teleport_to_teleporter++;
                if (game.teleport_to_teleporter >= (int) map.teleporters.size()) game.teleport_to_teleporter = 0;
                tempx = map.teleporters[game.teleport_to_teleporter].x;
                tempy = map.teleporters[game.teleport_to_teleporter].y;
            }
            while (map.explored[tempx + (20 * tempy)] == 0);
        }

        if (game.press_map)
        {
            tempx = map.teleporters[game.teleport_to_teleporter].x;
            tempy = map.teleporters[game.teleport_to_teleporter].y;
            if (game.roomx == tempx + 100 && game.roomy == tempy + 100)
            {
                //cancel!
                graphics.resumegamemode = true;
            }
            else
            {
                //teleport
                graphics.resumegamemode = true;
                game.teleport_to_x = tempx;
                game.teleport_to_y = tempy;

                //trace(game.recordstring);
                //We're teleporting! Yey!
                game.activetele = false;
                game.hascontrol = false;

                int i = obj.getplayer();
                if (INBOUNDS_VEC(i, obj.entities))
                {
                    obj.entities[i].colour = 102;
                }

                i = obj.getteleporter();
                if (INBOUNDS_VEC(i, obj.entities))
                {
                    obj.entities[i].tile = 6;
                    obj.entities[i].colour = 102;
                }
                //which teleporter script do we use? it depends on the companion!
                game.state = 4000;
                game.statedelay = 0;
            }
        }
    }
}

void gamecompleteinput()
{
    game.press_left = false;
    game.press_right = false;
    game.press_action = false;
    game.press_map = false;

    //Do this before we update map.bypos
    if (!game.colourblindmode)
    {
        graphics.updatetowerbackground(graphics.titlebg);
    }

    //Do these here because input comes first
    graphics.titlebg.bypos += graphics.titlebg.bscroll;
    game.oldcreditposition = game.creditposition;

    if (key.isDown(KEYBOARD_z) || key.isDown(KEYBOARD_SPACE) || key.isDown(KEYBOARD_v) || key.isDown(game.controllerButton_flip))
    {
        game.creditposition -= 6;
        if (game.creditposition <= -Credits::creditmaxposition)
        {
            if(graphics.fademode==0)
            {
                graphics.fademode = 2;
            }
            game.creditposition = -Credits::creditmaxposition;
        }
        else
        {
            graphics.titlebg.bscroll = +7;
        }
        game.press_action = true;
    }
    if (key.isDown(KEYBOARD_ENTER)|| key.isDown(game.controllerButton_map)) game.press_map = true;

    if (!game.mapheld)
    {
        if(game.press_map)
        {
            //Return to game
            if(graphics.fademode==0)
            {
                graphics.fademode = 2;
            }
        }
    }
}

void gamecompleteinput2()
{
    game.press_left = false;
    game.press_right = false;
    game.press_action = false;
    game.press_map = false;

    //Do this here because input comes first
    game.oldcreditposx = game.creditposx;

    if (key.isDown(KEYBOARD_z) || key.isDown(KEYBOARD_SPACE) || key.isDown(KEYBOARD_v) || key.isDown(game.controllerButton_flip))
    {
        game.creditposx++;
        game.oldcreditposx++;
        if (game.creditposy >= 30)
        {
            if(graphics.fademode==0)
            {
                graphics.fademode = 2;
                music.fadeout();
            }
        }
        game.press_action = true;
    }
    if (key.isDown(KEYBOARD_ENTER) || key.isDown(game.controllerButton_map)) game.press_map = true;

    if (!game.mapheld)
    {
        if(game.press_map)
        {
            //Return to game
            if(graphics.fademode==0)
            {
                graphics.fademode = 2;
                music.fadeout();
            }
        }
    }
}
