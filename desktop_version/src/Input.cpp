#include <tinyxml2.h>
#include <vector>

#include "ButtonGlyphs.h"
#include "Credits.h"
#include "CustomLevels.h"
#include "Editor.h"
#include "Entity.h"
#include "Enums.h"
#include "FileSystemUtils.h"
#include "Game.h"
#include "GlitchrunnerMode.h"
#include "Graphics.h"
#include "GraphicsUtil.h"
#include "KeyPoll.h"
#include "Localization.h"
#include "LocalizationMaint.h"
#include "LocalizationStorage.h"
#include "MakeAndPlay.h"
#include "Map.h"
#include "Music.h"
#include "RoomnameTranslator.h"
#include "Screen.h"
#include "Script.h"
#include "UtilityClass.h"
#include "Vlogging.h"

static void updatebuttonmappings(int bind)
{
    for (
        SDL_GameControllerButton i = SDL_CONTROLLER_BUTTON_A;
        i < SDL_CONTROLLER_BUTTON_DPAD_UP;
        i = (SDL_GameControllerButton) (i + 1)
    ) {
        if (key.isDown(i))
        {
            bool dupe = false;
            switch (bind)
            {
            case 1:
            {
                size_t j;
                for (j = 0; j < game.controllerButton_flip.size(); j += 1)
                {
                    if (i == game.controllerButton_flip[j])
                    {
                        dupe = true;
                    }
                }
                if (!dupe)
                {
                    game.controllerButton_flip.push_back(i);
                    music.playef(Sound_VIRIDIAN);
                }
                for (j = 0; j < game.controllerButton_map.size(); j += 1)
                {
                    if (i == game.controllerButton_map[j])
                    {
                        game.controllerButton_map.erase(game.controllerButton_map.begin() + j);
                    }
                }
                for (j = 0; j < game.controllerButton_esc.size(); j += 1)
                {
                    if (i == game.controllerButton_esc[j])
                    {
                        game.controllerButton_esc.erase(game.controllerButton_esc.begin() + j);
                    }
                }
                for (j = 0; j < game.controllerButton_restart.size(); j += 1)
                {
                    if (i == game.controllerButton_restart[j])
                    {
                        game.controllerButton_restart.erase(game.controllerButton_restart.begin() + j);
                    }
                }
                for (j = 0; j < game.controllerButton_interact.size(); j += 1)
                {
                    if (i == game.controllerButton_interact[j])
                    {
                        game.controllerButton_interact.erase(game.controllerButton_interact.begin() + j);
                    }
                }
                break;
            }
            case 2:
            {
                size_t j;
                for (j = 0; j < game.controllerButton_map.size(); j += 1)
                {
                    if (i == game.controllerButton_map[j])
                    {
                        dupe = true;
                    }
                }
                if (!dupe)
                {
                    game.controllerButton_map.push_back(i);
                    music.playef(Sound_VIRIDIAN);
                }
                for (j = 0; j < game.controllerButton_flip.size(); j += 1)
                {
                    if (i == game.controllerButton_flip[j])
                    {
                        game.controllerButton_flip.erase(game.controllerButton_flip.begin() + j);
                    }
                }
                for (j = 0; j < game.controllerButton_esc.size(); j += 1)
                {
                    if (i == game.controllerButton_esc[j])
                    {
                        game.controllerButton_esc.erase(game.controllerButton_esc.begin() + j);
                    }
                }
                for (j = 0; j < game.controllerButton_restart.size(); j += 1)
                {
                    if (i == game.controllerButton_restart[j])
                    {
                        game.controllerButton_restart.erase(game.controllerButton_restart.begin() + j);
                    }
                }
                for (j = 0; j < game.controllerButton_interact.size(); j += 1)
                {
                    if (i == game.controllerButton_interact[j])
                    {
                        game.controllerButton_interact.erase(game.controllerButton_interact.begin() + j);
                    }
                }
                break;
            }
            case 3:
            {
                size_t j;
                for (j = 0; j < game.controllerButton_esc.size(); j += 1)
                {
                    if (i == game.controllerButton_esc[j])
                    {
                        dupe = true;
                    }
                }
                if (!dupe)
                {
                    game.controllerButton_esc.push_back(i);
                    music.playef(Sound_VIRIDIAN);
                }
                for (j = 0; j < game.controllerButton_flip.size(); j += 1)
                {
                    if (i == game.controllerButton_flip[j])
                    {
                        game.controllerButton_flip.erase(game.controllerButton_flip.begin() + j);
                    }
                }
                for (j = 0; j < game.controllerButton_map.size(); j += 1)
                {
                    if (i == game.controllerButton_map[j])
                    {
                        game.controllerButton_map.erase(game.controllerButton_map.begin() + j);
                    }
                }
                for (j = 0; j < game.controllerButton_restart.size(); j += 1)
                {
                    if (i == game.controllerButton_restart[j])
                    {
                        game.controllerButton_restart.erase(game.controllerButton_restart.begin() + j);
                    }
                }
                for (j = 0; j < game.controllerButton_interact.size(); j += 1)
                {
                    if (i == game.controllerButton_interact[j])
                    {
                        game.controllerButton_interact.erase(game.controllerButton_interact.begin() + j);
                    }
                }
                break;
            }
            case 4:
            {
                size_t j;
                for (j = 0; j < game.controllerButton_restart.size(); j += 1)
                {
                    if (i == game.controllerButton_restart[j])
                    {
                        dupe = true;
                    }
                }
                if (!dupe)
                {
                    game.controllerButton_restart.push_back(i);
                    music.playef(Sound_VIRIDIAN);
                }
                for (j = 0; j < game.controllerButton_flip.size(); j += 1)
                {
                    if (i == game.controllerButton_flip[j])
                    {
                        game.controllerButton_flip.erase(game.controllerButton_flip.begin() + j);
                    }
                }
                for (j = 0; j < game.controllerButton_map.size(); j += 1)
                {
                    if (i == game.controllerButton_map[j])
                    {
                        game.controllerButton_map.erase(game.controllerButton_map.begin() + j);
                    }
                }
                for (j = 0; j < game.controllerButton_esc.size(); j += 1)
                {
                    if (i == game.controllerButton_esc[j])
                    {
                        game.controllerButton_esc.erase(game.controllerButton_esc.begin() + j);
                    }
                }
                for (j = 0; j < game.controllerButton_interact.size(); j += 1)
                {
                    if (i == game.controllerButton_interact[j])
                    {
                        game.controllerButton_interact.erase(game.controllerButton_interact.begin() + j);
                    }
                }
                break;
            }
            case 5:
            {
                size_t j;
                for (j = 0; j < game.controllerButton_interact.size(); j += 1)
                {
                    if (i == game.controllerButton_interact[j])
                    {
                        dupe = true;
                    }
                }
                if (!dupe)
                {
                    game.controllerButton_interact.push_back(i);
                    music.playef(Sound_VIRIDIAN);
                }
                for (j = 0; j < game.controllerButton_flip.size(); j += 1)
                {
                    if (i == game.controllerButton_flip[j])
                    {
                        game.controllerButton_flip.erase(game.controllerButton_flip.begin() + j);
                    }
                }
                for (j = 0; j < game.controllerButton_map.size(); j += 1)
                {
                    if (i == game.controllerButton_map[j])
                    {
                        game.controllerButton_map.erase(game.controllerButton_map.begin() + j);
                    }
                }
                for (j = 0; j < game.controllerButton_esc.size(); j += 1)
                {
                    if (i == game.controllerButton_esc[j])
                    {
                        game.controllerButton_esc.erase(game.controllerButton_esc.begin() + j);
                    }
                }
                for (j = 0; j < game.controllerButton_restart.size(); j += 1)
                {
                    if (i == game.controllerButton_restart[j])
                    {
                        game.controllerButton_restart.erase(game.controllerButton_restart.begin() + j);
                    }
                }
                break;
            }

            }
        }
    }
}

static void toggleflipmode(void)
{
    graphics.setflipmode = !graphics.setflipmode;
    game.savestatsandsettings_menu();
    if (graphics.setflipmode)
    {
        music.playef(Sound_GAMESAVED);
        game.screenshake = 10;
        game.flashlight = 5;
    }
    else
    {
        music.playef(Sound_VIRIDIAN);
    }
}

static bool fadetomode = false;
static int fadetomodedelay = 0;
static enum StartMode gotomode = Start_MAINGAME;

static void startmode(const enum StartMode mode)
{
    gotomode = mode;
    graphics.fademode = FADE_START_FADEOUT;
    fadetomode = true;
    fadetomodedelay = 19;
}

static void handlefadetomode(void)
{
    if (game.ingame_titlemode)
    {
        /* We shouldn't be here! */
        SDL_assert(0 && "Loading a mode from in-game options!");
        return;
    }

    if (fadetomodedelay > 0)
    {
        --fadetomodedelay;
    }
    else
    {
        fadetomode = false;
        script.startgamemode(gotomode);
    }
}

static int* user_changing_volume = NULL;
static int previous_volume = 0;

static void initvolumeslider(const int menuoption)
{
    switch (menuoption)
    {
    case 0:
        game.slidermode = SLIDER_MUSICVOLUME;
        user_changing_volume = &music.user_music_volume;
        break;
    case 1:
        game.slidermode = SLIDER_SOUNDVOLUME;
        user_changing_volume = &music.user_sound_volume;
        break;
    default:
        SDL_assert(0 && "Unhandled volume slider option!");
        game.slidermode = SLIDER_NONE;
        user_changing_volume = NULL;
        return;
    }
    previous_volume = *user_changing_volume;
}

static void deinitvolumeslider(void)
{
    user_changing_volume = NULL;
    game.savestatsandsettings_menu();
    game.slidermode = SLIDER_NONE;
}

static void slidermodeinput(void)
{
    if (user_changing_volume == NULL)
    {
        SDL_assert(0 && "user_changing_volume is NULL!");
        return;
    }

    if (game.press_left)
    {
        *user_changing_volume -= USER_VOLUME_STEP;
    }
    else if (game.press_right)
    {
        *user_changing_volume += USER_VOLUME_STEP;
    }
    *user_changing_volume = SDL_clamp(*user_changing_volume, 0, USER_VOLUME_MAX);
}

static void menuactionpress(void)
{
    if (game.menutestmode)
    {
        music.playef(Sound_CRUMBLE);
        Menu::MenuName nextmenu = (Menu::MenuName) (game.currentmenuname + 1);
        game.returnmenu();
        game.createmenu(nextmenu);
        return;
    }

    switch (game.currentmenuname)
    {
    case Menu::mainmenu:
    {
        int option_id = -1;
        int option_seq = 0; /* option number in YOUR configuration */
#define OPTION_ID(id) \
        if (option_seq == game.currentmenuoption) \
        { \
            option_id = id; \
        } \
        option_seq++;
#if !defined(MAKEANDPLAY)
        OPTION_ID(0) /* play */
#endif
        OPTION_ID(1) /* levels */
        OPTION_ID(2) /* options */
        if (loc::show_translator_menu)
        {
            OPTION_ID(3) /* translator */
        }
        OPTION_ID(4) /* credits */
        OPTION_ID(5) /* quit */

#undef OPTION_ID


        switch (option_id)
        {
#if !defined(MAKEANDPLAY)
        case 0:
            //Play
            if (!game.save_exists() && !game.anything_unlocked())
            {
                //No saves exist, just start a new game
                music.playef(Sound_VIRIDIAN);
                startmode(Start_MAINGAME);
            }
            else
            {
                //Bring you to the normal playmenu
                music.playef(Sound_VIRIDIAN);
                game.createmenu(Menu::play);
                map.nexttowercolour();
            }
            break;
#endif
        case 1:
            //Bring you to the normal playmenu
            music.playef(Sound_VIRIDIAN);
            game.editor_disabled = !BUTTONGLYPHS_keyboard_is_available();
            game.createmenu(Menu::playerworlds);
            map.nexttowercolour();
            break;
        case 2:
            //Options
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::options);
            map.nexttowercolour();
            break;
        case 3:
            //Translator
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::translator_main);
            map.nexttowercolour();
            break;
        case 4:
            //Credits
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::credits);
            map.nexttowercolour();
            break;
        case 5:
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::youwannaquit);
            map.nexttowercolour();
            break;
        }
        break;
    }
    case Menu::levellist:
    {
        const bool nextlastoptions = cl.ListOfMetaData.size() > 8;
        if(game.currentmenuoption==(int)game.menuoptions.size()-1){
            //go back to menu
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
        }else if(nextlastoptions && game.currentmenuoption==(int)game.menuoptions.size()-2){
            //previous page
            music.playef(Sound_VIRIDIAN);
            if(game.levelpage==0){
                game.levelpage=(cl.ListOfMetaData.size()-1)/8;
            }else{
                game.levelpage--;
            }
            game.createmenu(Menu::levellist, true);
            game.currentmenuoption=game.menuoptions.size()-2;
            map.nexttowercolour();
        }else if(nextlastoptions && game.currentmenuoption==(int)game.menuoptions.size()-3){
            //next page
            music.playef(Sound_VIRIDIAN);
            if((size_t) ((game.levelpage*8)+8) >= cl.ListOfMetaData.size()){
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
            music.playef(Sound_VIRIDIAN);
            game.playcustomlevel=(game.levelpage*8)+game.currentmenuoption;
            game.customleveltitle=cl.ListOfMetaData[game.playcustomlevel].title;
            game.customlevelfilename=cl.ListOfMetaData[game.playcustomlevel].filename;

            std::string name = "saves/" + cl.ListOfMetaData[game.playcustomlevel].filename.substr(7) + ".vvv";
            tinyxml2::XMLDocument doc;
            if (!FILESYSTEM_loadTiXml2Document(name.c_str(), doc)){
                startmode(Start_CUSTOM);
            }else{
                game.createmenu(Menu::quickloadlevel);
                map.nexttowercolour();
            }
        }
        break;
    }
    case Menu::quickloadlevel:
        switch (game.currentmenuoption)
        {
        case 0: //continue save
            music.playef(Sound_VIRIDIAN);
            startmode(Start_CUSTOM_QUICKSAVE);
            break;
        case 1:
            music.playef(Sound_VIRIDIAN);
            startmode(Start_CUSTOM);
            break;
        case 2:
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::deletequicklevel);
            map.nexttowercolour();
            break;
        default:
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::deletequicklevel:
        switch (game.currentmenuoption)
        {
        default:
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            break;
        case 1:
            game.customdeletequick(cl.ListOfMetaData[game.playcustomlevel].filename);
            game.returntomenu(Menu::levellist);
            game.flashlight = 5;
            game.screenshake = 15;
            music.playef(Sound_DESTROY);
            break;
        }
        map.nexttowercolour();
        break;
    case Menu::playerworlds:
        if (game.currentmenuoption == 0)
        {
            music.playef(Sound_VIRIDIAN);
            game.levelpage = 0;
            cl.getDirectoryData();
            game.loadcustomlevelstats(); //Should only load a file if it's needed
            game.createmenu(Menu::levellist);
            if (FILESYSTEM_levelDirHasError())
            {
                game.createmenu(Menu::warninglevellist);
            }
            map.nexttowercolour();
        }
        else if (game.currentmenuoption == 1)
        {
            // LEVEL EDITOR HOOK
            if (game.editor_disabled)
            {
                music.playef(Sound_CRY);
            }
            else
            {
                music.playef(Sound_VIRIDIAN);
                startmode(Start_EDITOR);
                ed.filename = "";
            }
        }
        else if (!game.editor_disabled && game.currentmenuoption == 2)
        {
            //"OPENFOLDERHOOK"
            if (FILESYSTEM_openDirectoryEnabled()
                && FILESYSTEM_openDirectory(FILESYSTEM_getUserLevelDirectory()))
            {
                music.playef(Sound_VIRIDIAN);
                SDL_MinimizeWindow(gameScreen.m_window);
            }
            else
            {
                music.playef(Sound_CRY);
            }
        }
        else if (!game.editor_disabled && game.currentmenuoption == 3)
        {
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::confirmshowlevelspath);
            map.nexttowercolour();
        }
        else if (game.currentmenuoption == 4 || (game.editor_disabled && game.currentmenuoption == 2))
        {
            // back
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
        }
        break;
    case Menu::confirmshowlevelspath:
    {
        int prevmenuoption = game.currentmenuoption; /* returnmenu destroys this */
        music.playef(Sound_VIRIDIAN);
        game.returnmenu();
        map.nexttowercolour();
        if (prevmenuoption == 1)
        {
            game.createmenu(Menu::showlevelspath);
        }
        break;
    }
    case Menu::showlevelspath:
        music.playef(Sound_VIRIDIAN);
        game.editor_disabled = !BUTTONGLYPHS_keyboard_is_available();
        game.returntomenu(Menu::playerworlds);
        map.nexttowercolour();
        break;
    case Menu::errornostart:
        music.playef(Sound_VIRIDIAN);
        game.createmenu(Menu::mainmenu);
        map.nexttowercolour();
        break;
    case Menu::graphicoptions:
    {
        int offset = 0;
        bool processed = false;
        if (game.currentmenuoption == offset + 0 && !gameScreen.isForcedFullscreen())
        {
            processed = true;
            music.playef(Sound_VIRIDIAN);
            gameScreen.toggleFullScreen();
        }
        if (gameScreen.isForcedFullscreen())
        {
            --offset;
        }
        if (game.currentmenuoption == offset + 1)
        {
            processed = true;
            music.playef(Sound_VIRIDIAN);
            gameScreen.toggleScalingMode();
            game.savestatsandsettings_menu();
        }
        if (game.currentmenuoption == offset + 2 && !gameScreen.isForcedFullscreen())
        {
            processed = true;
            // resize to nearest multiple
            if (gameScreen.isWindowed)
            {
                music.playef(Sound_VIRIDIAN);
                gameScreen.ResizeToNearestMultiple();
                game.savestatsandsettings_menu();
            }
            else
            {
                music.playef(Sound_CRY);
            }
        }
        if (gameScreen.isForcedFullscreen())
        {
            --offset;
        }
        if (game.currentmenuoption == offset + 3)
        {
            processed = true;
            music.playef(Sound_VIRIDIAN);
            gameScreen.toggleLinearFilter();
            game.savestatsandsettings_menu();
        }
        if (game.currentmenuoption == offset + 4)
        {
            processed = true;
            //change smoothing
            music.playef(Sound_VIRIDIAN);
            gameScreen.badSignalEffect= !gameScreen.badSignalEffect;
            game.savestatsandsettings_menu();
        }
        if (game.currentmenuoption == offset + 5)
        {
            processed = true;
            //toggle vsync
            music.playef(Sound_VIRIDIAN);
            gameScreen.toggleVSync();
            game.savestatsandsettings_menu();
        }
        if (!processed)
        {
            //back
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
        }
        break;
    }
    case Menu::youwannaquit:
        switch (game.currentmenuoption)
        {
        case 0:
            //bye!
            music.playef(Sound_CRY);
            startmode(Start_QUIT);
            break;
        default:
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
        }
        break;
    case Menu::setinvincibility:
        switch (game.currentmenuoption)
        {
        case 0:
            //back
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
            break;
        default:
            map.invincibility = !map.invincibility;
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
            game.savestatsandsettings_menu();
            break;
        }
        break;
    case Menu::setslowdown:
        switch (game.currentmenuoption)
        {
        case 0:
            //back
            game.slowdown = 30;
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
            game.savestatsandsettings_menu();
            break;
        case 1:
            game.slowdown = 24;
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
            game.savestatsandsettings_menu();
            break;
        case 2:
            game.slowdown = 18;
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
            game.savestatsandsettings_menu();
            break;
        case 3:
            game.slowdown = 12;
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
            game.savestatsandsettings_menu();
            break;
        }
        break;
    case Menu::speedrunneroptions:
        switch (game.currentmenuoption)
        {
        case 0:
            // Glitchrunner mode
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::setglitchrunner);
            game.currentmenuoption = GlitchrunnerMode_get();
            map.nexttowercolour();
            break;
        case 1:
            /* Input delay */
            music.playef(Sound_VIRIDIAN);
            game.inputdelay = !game.inputdelay;
            game.savestatsandsettings_menu();
            break;
        case 2:
            /* Interact button toggle */
            music.playef(Sound_VIRIDIAN);
            game.separate_interact = !game.separate_interact;
            game.savestatsandsettings_menu();
            break;
        case 3:
            // toggle fake load screen
            game.skipfakeload = !game.skipfakeload;
            game.savestatsandsettings_menu();
            music.playef(Sound_VIRIDIAN);
            break;
        case 4:
            // toggle in game timer
            game.showingametimer = !game.showingametimer;
            game.savestatsandsettings_menu();
            music.playef(Sound_VIRIDIAN);
            break;
        default:
            //back
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::setglitchrunner:
        GlitchrunnerMode_set((enum GlitchrunnerMode) game.currentmenuoption);
        music.playef(Sound_VIRIDIAN);
        game.returnmenu();
        game.savestatsandsettings_menu();
        map.nexttowercolour();
        break;
    case Menu::advancedoptions:
        switch (game.currentmenuoption)
        {
        case 0:
            // toggle unfocus pause
            game.disablepause = !game.disablepause;
            game.savestatsandsettings_menu();
            music.playef(Sound_VIRIDIAN);
            break;
        case 1:
            /* toggle unfocus music pause */
            game.disableaudiopause = !game.disableaudiopause;
            game.savestatsandsettings_menu();
            music.playef(Sound_VIRIDIAN);
            break;
        case 2:
            // toggle translucent roomname BG
            graphics.translucentroomname = !graphics.translucentroomname;
            game.savestatsandsettings_menu();
            music.playef(Sound_VIRIDIAN);
            break;
        default:
            //back
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::accessibility:
    {
        int accessibilityoffset = 0;
#if !defined(MAKEANDPLAY)
        accessibilityoffset = 1;
        if (game.currentmenuoption == 0) {
            //unlock play options
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::unlockmenu);
            map.nexttowercolour();
        }
#endif
        if (game.currentmenuoption == accessibilityoffset + 0) {
            //invincibility
            if (!game.ingame_titlemode || !game.incompetitive())
            {
                if (!map.invincibility)
                {
                    game.createmenu(Menu::setinvincibility);
                    map.nexttowercolour();
                }
                else
                {
                    map.invincibility = !map.invincibility;
                    game.savestatsandsettings_menu();
                }
                music.playef(Sound_VIRIDIAN);
            }
            else
            {
                music.playef(Sound_CRY);
                map.invincibility = false;
            }
        }
        else if (game.currentmenuoption == accessibilityoffset + 1) {
            //change game speed
            if (!game.ingame_titlemode || !game.incompetitive())
            {
                game.createmenu(Menu::setslowdown);
                map.nexttowercolour();
                music.playef(Sound_VIRIDIAN);
            }
            else
            {
                music.playef(Sound_CRY);
                game.slowdown = 30;
            }
        }
        else if (game.currentmenuoption == accessibilityoffset + 2) {
            //disable animated backgrounds
            game.colourblindmode = !game.colourblindmode;
            game.savestatsandsettings_menu();
            graphics.towerbg.tdrawback = true;
            graphics.titlebg.tdrawback = true;
            music.playef(Sound_VIRIDIAN);
        }
        else if (game.currentmenuoption == accessibilityoffset + 3) {
            //disable screeneffects
            game.noflashingmode = !game.noflashingmode;
            game.savestatsandsettings_menu();
            if (!game.noflashingmode)
            {
                music.playef(Sound_GAMESAVED);
                game.screenshake = 10;
                game.flashlight = 5;
            }
            else {
                music.playef(Sound_VIRIDIAN);
            }
        }
        else if (game.currentmenuoption == accessibilityoffset + 4) {
            //disable text outline
            graphics.notextoutline = !graphics.notextoutline;
            game.savestatsandsettings_menu();
            music.playef(Sound_VIRIDIAN);
        }
        else if (game.currentmenuoption == accessibilityoffset + 5) {
            //back
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
        }
        break;
    }
    case Menu::gameplayoptions:
    {
        int gameplayoptionsoffset = 0;
#if !defined(MAKEANDPLAY)
        if (game.ingame_titlemode && game.unlock[Unlock_FLIPMODE])
#endif
        {
            gameplayoptionsoffset = 1;
            if (game.currentmenuoption == 0) {
                toggleflipmode();
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
                        music.play(Music_POSITIVEFORCEREVERSED);
                    }
                    else
                    {
                        music.play(Music_POSITIVEFORCE);
                    }
                }
            }
        }

        if (game.currentmenuoption == gameplayoptionsoffset + 0)
        {
            //Toggle 30+ FPS
            music.playef(Sound_VIRIDIAN);
            game.over30mode = !game.over30mode;
            game.savestatsandsettings_menu();
        }
        else if (game.currentmenuoption == gameplayoptionsoffset + 1)
        {
            //Speedrunner options
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::speedrunneroptions);
            map.nexttowercolour();
        }
        else if (game.currentmenuoption == gameplayoptionsoffset + 2)
        {
            //Advanced options
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::advancedoptions);
            map.nexttowercolour();
        }
        else if (game.currentmenuoption == gameplayoptionsoffset + 3)
        {
            //Clear Data
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::cleardatamenu);
            map.nexttowercolour();
        }
        else if (game.currentmenuoption == gameplayoptionsoffset + 4)
        {
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::clearcustomdatamenu);
            map.nexttowercolour();
        }
        else if (game.currentmenuoption == gameplayoptionsoffset + 5) {
            //return to previous menu
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
        }

        break;
    }
    case Menu::options:
        switch (game.currentmenuoption)
        {
        case 0:
            //gameplay options
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::gameplayoptions);
            map.nexttowercolour();
            break;
        case 1:
            //graphic options
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::graphicoptions);
            map.nexttowercolour();
            break;
        case 2:
            /* Audio options */
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::audiooptions);
            map.nexttowercolour();
            break;
        case 3:
            //gamepad options
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::controller);
            map.nexttowercolour();
            break;
        case 4:
            //accessibility options
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::accessibility);
            map.nexttowercolour();
            break;
        case 5:
            //language options
            music.playef(Sound_VIRIDIAN);
            loc::loadlanguagelist();
            game.createmenu(Menu::language);
            game.currentmenuoption = loc::languagelist_curlang;
            map.nexttowercolour();
            break;
        default:
            /* Return */
            music.playef(Sound_VIRIDIAN);
            if (game.ingame_titlemode)
            {
                game.returntoingame();
            }
            else
            {
                game.returnmenu();
                map.nexttowercolour();
            }
            break;
        }
        break;
    case Menu::audiooptions:
        switch (game.currentmenuoption)
        {
        case 0:
        case 1:
            music.playef(Sound_VIRIDIAN);
            if (game.slidermode == SLIDER_NONE)
            {
                initvolumeslider(game.currentmenuoption);
            }
            else
            {
                deinitvolumeslider();
            }
            break;
        case 2:
            if (!music.mmmmmm)
            {
                break;
            }

            /* Toggle MMMMMM */
            music.usingmmmmmm = !music.usingmmmmmm;
            music.playef(Sound_VIRIDIAN);
            if (music.currentsong > -1)
            {
                music.play(music.currentsong);
            }
            game.savestatsandsettings_menu();
            break;
        }

        if (game.currentmenuoption == 2 + (int) music.mmmmmm)
        {
            /* Return */
            game.returnmenu();
            map.nexttowercolour();
            music.playef(Sound_VIRIDIAN);
        }
        break;
    case Menu::language:
    {
        music.playef(Sound_VIRIDIAN);

        bool show_title = !loc::lang_set;

        if (loc::languagelist.size() != 0 && (unsigned)game.currentmenuoption < loc::languagelist.size())
        {
            loc::lang = loc::languagelist[game.currentmenuoption].code;
            loc::loadtext(false);
            loc::lang_set = true;
        }

        if (show_title)
        {
            /* Make the title screen appear, we haven't seen it yet */
            game.menustart = false;
            game.createmenu(Menu::mainmenu);
            game.currentmenuoption = 0;
        }
        else
        {
            game.returnmenu();
        }
        map.nexttowercolour();
        game.savestatsandsettings_menu();

        break;
    }
    case Menu::translator_main:
        switch (game.currentmenuoption)
        {
        case 0:
            // translator options
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::translator_options);
            map.nexttowercolour();
            break;
        case 1:
            // maintenance
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::translator_maintenance);
            map.nexttowercolour();
            break;
        case 2:
            // open lang folder
            if (FILESYSTEM_openDirectoryEnabled()
            && FILESYSTEM_openDirectory(FILESYSTEM_getUserMainLangDirectory()))
            {
                music.playef(Sound_VIRIDIAN);
                SDL_MinimizeWindow(gameScreen.m_window);
            }
            else
            {
                music.playef(Sound_CRY);
            }
            break;
        default:
            // return
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::translator_options:
        switch (game.currentmenuoption)
        {
        case 0:
            // language statistics
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::translator_options_stats);
            map.nexttowercolour();
            break;
        case 1:
            // translate room names
            music.playef(Sound_VIRIDIAN);
            roomname_translator::set_enabled(!roomname_translator::enabled);
            game.savestatsandsettings_menu();
            break;
        case 2:
            // explore game
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::translator_options_exploregame);
            map.nexttowercolour();
            break;
        case 3:
            // menu test
            music.playef(Sound_GAMESAVED);
            game.menutestmode = true;
            game.createmenu((Menu::MenuName) 0);
            map.nexttowercolour();
            break;
        case 4:
            // cutscene test
            if (loc::lang == "en")
            {
                music.playef(Sound_CRY);
            }
            else
            {
                music.playef(Sound_VIRIDIAN);
                game.cutscenetest_menu_page = 0;
                loc::populate_testable_script_ids();
                game.createmenu(Menu::translator_options_cutscenetest);
                map.nexttowercolour();
            }
            break;
        case 5:
            // limits check
            music.playef(Sound_VIRIDIAN);
            loc::local_limits_check();
            game.createmenu(Menu::translator_options_limitscheck);
            map.nexttowercolour();
            break;
        default:
            // return
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::translator_options_limitscheck:
        switch (game.currentmenuoption)
        {
        case 0:
            // next
            if (loc::limitscheck_current_overflow < loc::text_overflows.size())
            {
                music.playef(Sound_VIRIDIAN);
                loc::limitscheck_current_overflow++;
            }
            break;
        default:
            // return
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::translator_options_stats:
        music.playef(Sound_VIRIDIAN);
        game.returnmenu();
        map.nexttowercolour();
        break;
    case Menu::translator_options_exploregame:
        music.playef(Sound_VIRIDIAN);
        switch (game.currentmenuoption)
        {
        case 0:
            game.start_translator_exploring = true;
            startmode(Start_TIMETRIAL_SPACESTATION1);
            break;
        case 1:
            game.start_translator_exploring = true;
            startmode(Start_TIMETRIAL_LABORATORY);
            break;
        case 2:
            game.start_translator_exploring = true;
            startmode(Start_TIMETRIAL_TOWER);
            break;
        case 3:
            game.start_translator_exploring = true;
            startmode(Start_TIMETRIAL_SPACESTATION2);
            break;
        case 4:
            game.start_translator_exploring = true;
            startmode(Start_TIMETRIAL_WARPZONE);
            break;
        case 5:
            game.createmenu(Menu::playint1);
            game.start_translator_exploring = true;
            map.nexttowercolour();
            break;
        case 6:
            game.createmenu(Menu::playint2);
            game.start_translator_exploring = true;
            map.nexttowercolour();
            break;
        case 7:
            game.start_translator_exploring = true;
            startmode(Start_TIMETRIAL_FINALLEVEL);
            break;
        default:
            // return
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::translator_options_cutscenetest:
        if (game.currentmenuoption == (int)game.menuoptions.size()-4)
        {
            // next page
            music.playef(Sound_VIRIDIAN);
            if ((size_t) ((game.cutscenetest_menu_page*14)+14) >= loc::testable_script_ids.size())
            {
                game.cutscenetest_menu_page = 0;
            }
            else
            {
                game.cutscenetest_menu_page++;
            }
            game.createmenu(Menu::translator_options_cutscenetest, true);
            game.currentmenuoption=game.menuoptions.size()-4;
            map.nexttowercolour();
        }
        else if (game.currentmenuoption == (int)game.menuoptions.size()-3)
        {
            // previous page
            music.playef(Sound_VIRIDIAN);
            if (game.cutscenetest_menu_page == 0)
            {
                game.cutscenetest_menu_page = (loc::testable_script_ids.size()-1)/14;
            }
            else
            {
                game.cutscenetest_menu_page--;
            }
            game.createmenu(Menu::translator_options_cutscenetest, true);
            game.currentmenuoption=game.menuoptions.size()-3;
            map.nexttowercolour();
        }
        else if (game.currentmenuoption == (int)game.menuoptions.size()-2)
        {
            // play the cutscene, from clipboard
            game.cutscenetest_menu_play_id = std::string(SDL_GetClipboardText());
            startmode(Start_CUTSCENETEST);
        }
        else if (game.currentmenuoption == (int)game.menuoptions.size()-1)
        {
            // go back to menu
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
        }
        else
        {
            // play the cutscene!
            game.cutscenetest_menu_play_id = loc::testable_script_ids[(game.cutscenetest_menu_page*14)+game.currentmenuoption];
            startmode(Start_CUTSCENETEST);
        }
        break;
    case Menu::translator_maintenance:
        music.playef(Sound_VIRIDIAN);
        switch (game.currentmenuoption)
        {
        case 0:
            // sync languages
            game.createmenu(Menu::translator_maintenance_sync);
            map.nexttowercolour();
            break;
        case 1:
            // global statistics
            // TODO
            map.nexttowercolour();
            break;
        case 2:
            // global limits check
            loc::global_limits_check();
            game.createmenu(Menu::translator_options_limitscheck);
            map.nexttowercolour();
            break;
        default:
            // return
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::translator_maintenance_sync:
    {
        music.playef(Sound_VIRIDIAN);
        bool sync_success = true;
        if (game.currentmenuoption == 0)
        {
            // yes, sync files
            sync_success = loc::sync_lang_files();
        }
        game.returnmenu();
        map.nexttowercolour();
        if (!sync_success)
        {
            game.createmenu(Menu::translator_error_setlangwritedir);
        }
        break;
    }
    case Menu::translator_error_setlangwritedir:
        music.playef(Sound_VIRIDIAN);
        game.returnmenu();
        map.nexttowercolour();
        break;
    case Menu::unlockmenutrials:
        switch (game.currentmenuoption)
        {
        case 0:       //unlock 1
            game.unlock[Unlock_TIMETRIAL_SPACESTATION1] = true;
            game.unlocknotify[Unlock_TIMETRIAL_SPACESTATION1] = true;
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::unlockmenutrials, true);
            game.savestatsandsettings_menu();
            break;
        case 1:       //unlock 2
            game.unlock[Unlock_TIMETRIAL_LABORATORY] = true;
            game.unlocknotify[Unlock_TIMETRIAL_LABORATORY] = true;
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::unlockmenutrials, true);
            game.savestatsandsettings_menu();
            break;
        case 2:       //unlock 3
            game.unlock[Unlock_TIMETRIAL_TOWER] = true;
            game.unlocknotify[Unlock_TIMETRIAL_TOWER] = true;
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::unlockmenutrials, true);
            game.savestatsandsettings_menu();
            break;
        case 3:       //unlock 4
            game.unlock[Unlock_TIMETRIAL_SPACESTATION2] = true;
            game.unlocknotify[Unlock_TIMETRIAL_SPACESTATION2] = true;
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::unlockmenutrials, true);
            game.savestatsandsettings_menu();
            break;
        case 4:       //unlock 5
            game.unlock[Unlock_TIMETRIAL_WARPZONE] = true;
            game.unlocknotify[Unlock_TIMETRIAL_WARPZONE] = true;
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::unlockmenutrials, true);
            game.savestatsandsettings_menu();
            break;
        case 5:       //unlock 6
            game.unlock[Unlock_TIMETRIAL_FINALLEVEL] = true;
            game.unlocknotify[Unlock_TIMETRIAL_FINALLEVEL] = true;
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::unlockmenutrials, true);
            game.savestatsandsettings_menu();
            break;
        case 6:       //back
            //back
            music.playef(Sound_VIRIDIAN);
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
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::unlockmenutrials);
            map.nexttowercolour();
            break;
        case 1:
            //unlock intermissions
            music.playef(Sound_VIRIDIAN);
            game.unlock[Unlock_INTERMISSION_REPLAYS] = true;
            game.unlocknotify[Unlock_INTERMISSION_REPLAYS] = true;
            game.unlock[Unlock_INTERMISSION1_COMPLETE] = true;
            game.unlock[Unlock_INTERMISSION2_COMPLETE] = true;
            game.createmenu(Menu::unlockmenu, true);
            game.savestatsandsettings_menu();
            break;
        case 2:
            //unlock no death mode
            music.playef(Sound_VIRIDIAN);
            game.unlock[Unlock_NODEATHMODE] = true;
            game.unlocknotify[Unlock_NODEATHMODE] = true;
            game.createmenu(Menu::unlockmenu, true);
            game.savestatsandsettings_menu();
            break;
        case 3:
            //unlock flip mode
            music.playef(Sound_VIRIDIAN);
            game.unlock[Unlock_FLIPMODE] = true;
            game.unlocknotify[Unlock_FLIPMODE] = true;
            game.createmenu(Menu::unlockmenu, true);
            game.savestatsandsettings_menu();
            break;
        case 4:
            //unlock jukebox
            music.playef(Sound_VIRIDIAN);
            game.stat_trinkets = 20;
            game.createmenu(Menu::unlockmenu, true);
            game.savestatsandsettings_menu();
            break;
        case 5:
            //unlock secret lab
            music.playef(Sound_VIRIDIAN);
            game.unlock[Unlock_SECRETLAB] = true;
            game.unlocknotify[Unlock_SECRETLAB] = true;
            game.createmenu(Menu::unlockmenu, true);
            game.savestatsandsettings_menu();
            break;
        default:
            //back
            music.playef(Sound_VIRIDIAN);
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
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::credits2, true);
            map.nexttowercolour();
            break;
        case 1:
            //last page
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::credits6, true);
            map.nexttowercolour();
            break;
        default:
            //back
            music.playef(Sound_VIRIDIAN);
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
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::credits25, true);
            map.nexttowercolour();
            break;
        case 1:
            //previous page
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::credits, true);
            map.nexttowercolour();
            break;
        case 2:
            //back
            music.playef(Sound_VIRIDIAN);
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
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::credits3, true);
            map.nexttowercolour();
            break;
        case 1:
            //previous page
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::credits2, true);
            map.nexttowercolour();
            break;
        default:
            //back
            music.playef(Sound_VIRIDIAN);
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
            music.playef(Sound_VIRIDIAN);
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
            music.playef(Sound_VIRIDIAN);
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

            map.nexttowercolour();
            break;
        default:
            //back
            music.playef(Sound_VIRIDIAN);
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
            music.playef(Sound_VIRIDIAN);
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
            music.playef(Sound_VIRIDIAN);
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

            map.nexttowercolour();
            break;
        default:
            //back
            music.playef(Sound_VIRIDIAN);
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
            music.playef(Sound_VIRIDIAN);
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
            music.playef(Sound_VIRIDIAN);
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

            map.nexttowercolour();
            break;
        default:
            //back
            music.playef(Sound_VIRIDIAN);
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
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::credits, true);
            map.nexttowercolour();
            break;
        case 1:
            //previous page
            music.playef(Sound_VIRIDIAN);
            game.current_credits_list_index = SDL_arraysize(Credits::githubfriends) - 1 - (SDL_arraysize(Credits::githubfriends)-1)%9;
            game.createmenu(Menu::credits5, true);
            map.nexttowercolour();
            break;
        default:
            //back
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::play:
    {
        //Do we have the Secret Lab option?
        int sloffset = game.unlock[Unlock_SECRETLAB] ? 0 : -1;
        //Do we have a telesave or quicksave?
        int ngoffset = game.save_exists() ? 0 : -1;
        if (game.currentmenuoption == 0)
        {
            //continue
            //right, this depends on what saves you've got
            if (!game.save_exists())
            {
                //You have no saves but have something unlocked, or you couldn't have gotten here
                music.playef(Sound_VIRIDIAN);
                startmode(Start_MAINGAME);
            }
            else if (game.telesummary == "")
            {
                //You at least have a quicksave, or you couldn't have gotten here
                music.playef(Sound_VIRIDIAN);
                startmode(Start_MAINGAME_QUICKSAVE);
            }
            else if (game.quicksummary == "")
            {
                //You at least have a telesave, or you couldn't have gotten here
                music.playef(Sound_VIRIDIAN);
                startmode(Start_MAINGAME_TELESAVE);
            }
            else
            {
                //go to a menu!
                music.playef(Sound_VIRIDIAN);
                game.loadsummary(); //Prepare save slots to display
                game.createmenu(Menu::continuemenu);
            }
        }
        else if (game.currentmenuoption == 1 && game.unlock[Unlock_SECRETLAB])
        {
            music.playef(Sound_VIRIDIAN);
            startmode(Start_SECRETLAB);
        }
        else if (game.currentmenuoption == sloffset+2)
        {
            //play modes
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::playmodes);
            map.nexttowercolour();
        }
        else if (game.currentmenuoption == sloffset+3 && game.save_exists())
        {
            //newgame
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::newgamewarning);
            map.nexttowercolour();
        }
        else if (game.currentmenuoption == sloffset+ngoffset+4)
        {
            //back
            music.playef(Sound_VIRIDIAN);
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
            music.playef(Sound_VIRIDIAN);
            startmode(Start_MAINGAME);
            game.deletequick();
            game.deletetele();
            break;
        default:
            //back
            music.playef(Sound_VIRIDIAN);
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
            music.playef(Sound_VIRIDIAN);
            if(key.sensitivity > 4)
            {
                key.sensitivity = 0;
            }
            game.savestatsandsettings_menu();
            break;

        case 6:
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::cleardatamenu:
        switch (game.currentmenuoption)
        {
        case 0:
            //back
            music.playef(Sound_VIRIDIAN);
            break;
        default:
            //yep
            music.playef(Sound_DESTROY);
            game.deletequick();
            game.deletetele();
            game.deletestats();
            game.deletesettings();
            game.flashlight = 5;
            game.screenshake = 15;
            break;
        }
        game.returnmenu();
        map.nexttowercolour();
        break;
    case Menu::clearcustomdatamenu:
        switch (game.currentmenuoption)
        {
        default:
            music.playef(Sound_VIRIDIAN);
            break;
        case 1:
            game.deletecustomlevelstats();
            FILESYSTEM_deleteLevelSaves();
            music.playef(Sound_DESTROY);
            game.flashlight = 5;
            game.screenshake = 15;
            break;
        }
        game.returnmenu();
        map.nexttowercolour();
        break;
    case Menu::playmodes:
        if (game.currentmenuoption == 0
            && !game.nocompetitive_unless_translator())   //go to the time trial menu
        {
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::timetrials);
            map.nexttowercolour();
        }
        else if (game.currentmenuoption == 1
            && game.unlock[Unlock_INTERMISSION_REPLAYS])
        {
            //intermission mode menu
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::intermissionmenu);
            map.nexttowercolour();
        }
        else if (game.currentmenuoption == 2
            && game.unlock[Unlock_NODEATHMODE]
            && !game.nocompetitive())    //start a game in no death mode
        {
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::startnodeathmode);
            map.nexttowercolour();
        }
        else if (game.currentmenuoption == 3
            && game.unlock[Unlock_FLIPMODE])    //enable/disable flip mode
        {
            toggleflipmode();
        }
        else if (game.currentmenuoption == 4)
        {
            //back
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
        }
        else
        {
            //Can't do yet!
            music.playef(Sound_CRY);
        }
        break;
    case Menu::startnodeathmode:
        switch (game.currentmenuoption)
        {
        case 0:   //start no death mode, disabling cutscenes
            music.playef(Sound_VIRIDIAN);
            startmode(Start_NODEATHMODE_NOCUTSCENES);
            break;
        case 1:
            music.playef(Sound_VIRIDIAN);
            startmode(Start_NODEATHMODE_WITHCUTSCENES);
            break;
        case 2:
            //back
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::continuemenu:
        switch (game.currentmenuoption)
        {
        case 0:
            music.playef(Sound_VIRIDIAN);
            startmode(Start_MAINGAME_TELESAVE);
            break;
        case 1:
            music.playef(Sound_VIRIDIAN);
            startmode(Start_MAINGAME_QUICKSAVE);
            break;
        case 2:
            //back
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::intermissionmenu:
        switch (game.currentmenuoption)
        {
        case 0:
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::playint1);
            map.nexttowercolour();
            break;
        case 1:
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::playint2);
            map.nexttowercolour();
            break;
        case 2:
            //back
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::playint1:
        switch (game.currentmenuoption)
        {
        case 0:
            music.playef(Sound_VIRIDIAN);
            startmode(Start_INTERMISSION1_VITELLARY);
            break;
        case 1:
            music.playef(Sound_VIRIDIAN);
            startmode(Start_INTERMISSION1_VERMILION);
            break;
        case 2:
            music.playef(Sound_VIRIDIAN);
            startmode(Start_INTERMISSION1_VERDIGRIS);
            break;
        case 3:
            music.playef(Sound_VIRIDIAN);
            startmode(Start_INTERMISSION1_VICTORIA);
            break;
        case 4:
            //back
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::playint2:
        switch (game.currentmenuoption)
        {
        case 0:
            music.playef(Sound_VIRIDIAN);
            startmode(Start_INTERMISSION2_VITELLARY);
            break;
        case 1:
            music.playef(Sound_VIRIDIAN);
            startmode(Start_INTERMISSION2_VERMILION);
            break;
        case 2:
            music.playef(Sound_VIRIDIAN);
            startmode(Start_INTERMISSION2_VERDIGRIS);
            break;
        case 3:
            music.playef(Sound_VIRIDIAN);
            startmode(Start_INTERMISSION2_VICTORIA);
            break;
        case 4:
            //back
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::gameover2:
        //back
        music.playef(Sound_VIRIDIAN);
        music.play(Music_PRESENTINGVVVVVV);
        game.returntomenu(Menu::playmodes);
        map.nexttowercolour();
        break;
    case Menu::unlocktimetrials:
    case Menu::unlocktimetrial:
    case Menu::unlocknodeathmode:
    case Menu::unlockintermission:
    case Menu::unlockflipmode:
        //back
        music.playef(Sound_VIRIDIAN);
        game.createmenu(Menu::play, true);
        map.nexttowercolour();
        break;
    case Menu::timetrials:
        if (game.currentmenuoption == 0
            && game.unlock[Unlock_TIMETRIAL_SPACESTATION1])
        {
            music.playef(Sound_VIRIDIAN);
            startmode(Start_TIMETRIAL_SPACESTATION1);
        }
        else if (game.currentmenuoption == 1
            && game.unlock[Unlock_TIMETRIAL_LABORATORY])
        {
            music.playef(Sound_VIRIDIAN);
            startmode(Start_TIMETRIAL_LABORATORY);
        }
        else if (game.currentmenuoption == 2
            && game.unlock[Unlock_TIMETRIAL_TOWER])
        {
            music.playef(Sound_VIRIDIAN);
            startmode(Start_TIMETRIAL_TOWER);
        }
        else if (game.currentmenuoption == 3
            && game.unlock[Unlock_TIMETRIAL_SPACESTATION2])
        {
            music.playef(Sound_VIRIDIAN);
            startmode(Start_TIMETRIAL_SPACESTATION2);
        }
        else if (game.currentmenuoption == 4
            && game.unlock[Unlock_TIMETRIAL_WARPZONE])
        {
            music.playef(Sound_VIRIDIAN);
            startmode(Start_TIMETRIAL_WARPZONE);
        }
        else if (game.currentmenuoption == 5
            && game.unlock[Unlock_TIMETRIAL_FINALLEVEL])
        {
            music.playef(Sound_VIRIDIAN);
            startmode(Start_TIMETRIAL_FINALLEVEL);
        }
        else if (game.currentmenuoption == 6)
        {
            //back
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
        }
        else
        {
            //Can't do yet!
            music.playef(Sound_CRY);
        }
        break;
    case Menu::timetrialcomplete3:
        switch (game.currentmenuoption)
        {
        case 0:
            //back
            music.playef(Sound_VIRIDIAN);
            music.play(Music_PRESENTINGVVVVVV);
            game.returntomenu(Menu::timetrials);
            map.nexttowercolour();
            break;
        case 1:
            /* Replay time trial */
            music.playef(Sound_VIRIDIAN);
            startmode((enum StartMode) (game.timetriallevel + Start_FIRST_TIMETRIAL));
            break;
        }
        break;
    case Menu::gamecompletecontinue:
    case Menu::nodeathmodecomplete2:
        music.play(Music_PRESENTINGVVVVVV);
        music.playef(Sound_VIRIDIAN);
        game.returnmenu();
        map.nexttowercolour();
        break;
    case Menu::errorsavingsettings:
        if (game.currentmenuoption == 1)
        {
            game.silence_settings_error = true;
        }
        music.playef(Sound_VIRIDIAN);
        game.returnmenu();
        map.nexttowercolour();
        break;
    case Menu::errorloadinglevel:
    case Menu::warninglevellist:
        music.playef(Sound_VIRIDIAN);
        game.returnmenu();
        map.nexttowercolour();
        break;
    default:
        break;
    }
}

void titleinput(void)
{
    //game.mx = (mouseX / 4);
    //game.my = (mouseY / 4);

    game.press_left = false;
    game.press_right = false;
    game.press_action = false;
    game.press_map = false;
    game.press_interact = false;

    bool lang_press_horizontal = false;

    if (graphics.flipmode)
    {
        if (key.isDown(KEYBOARD_LEFT) || key.isDown(KEYBOARD_DOWN) || key.isDown(KEYBOARD_a) ||  key.isDown(KEYBOARD_s) || key.controllerWantsRight(true)) game.press_left = true;
        if (key.isDown(KEYBOARD_RIGHT) || key.isDown(KEYBOARD_UP)  || key.isDown(KEYBOARD_d) ||  key.isDown(KEYBOARD_w) || key.controllerWantsLeft(true)) game.press_right = true;
    }
    else if (game.currentmenuname == Menu::language)
    {
        if (key.isDown(KEYBOARD_UP) || key.isDown(KEYBOARD_w) || key.controllerWantsUp())
        {
            game.press_left = true;
        }
        if (key.isDown(KEYBOARD_DOWN) || key.isDown(KEYBOARD_s) || key.controllerWantsDown())
        {
            game.press_right = true;
        }
        if (key.isDown(KEYBOARD_LEFT) || key.isDown(KEYBOARD_a) || key.controllerWantsLeft(false)
        || key.isDown(KEYBOARD_RIGHT) || key.isDown(KEYBOARD_d) || key.controllerWantsRight(false))
        {
            lang_press_horizontal = true;
            game.press_right = true;
        }
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
    if (!game.press_action && !game.press_left && !game.press_right && !key.isDown(27) && !key.isDown(game.controllerButton_esc)) game.jumpheld = false;
    if (!game.press_map) game.mapheld = false;

    if (!game.jumpheld && graphics.fademode == FADE_NONE)
    {
        if (game.press_action || game.press_left || game.press_right || game.press_map || key.isDown(27) || key.isDown(game.controllerButton_esc))
        {
            game.jumpheld = true;
        }

        if (    game.currentmenuname == Menu::controller &&
                game.currentmenuoption > 0 &&
                game.currentmenuoption < 6 &&
                (game.separate_interact || game.currentmenuoption < 5) &&
                key.controllerButtonDown()      )
        {
            updatebuttonmappings(game.currentmenuoption);
            music.playef(Sound_VIRIDIAN);
            game.savestatsandsettings_menu();
            return;
        }

        if (game.menustart
        && game.menucountdown <= 0
        && (key.isDown(27) || key.isDown(game.controllerButton_esc)))
        {
            if (game.currentmenuname == Menu::language && !loc::lang_set)
            {
                /* Don't exit from the initial language screen,
                 * you can't do this on the loading/title screen either. */
                return;
            }
            else
            {
                music.playef(Sound_VIRIDIAN);
            }
            if (game.menutestmode)
            {
                game.menutestmode = false;
                game.returnmenu();
                map.nexttowercolour();
            }
            else if (game.currentmenuname == Menu::mainmenu)
            {
                game.createmenu(Menu::youwannaquit);
                map.nexttowercolour();
            }
            else
            {
                if (game.slidermode != SLIDER_NONE)
                {
                    switch (game.slidermode)
                    {
                    /* Cancel volume change. */
                    case SLIDER_MUSICVOLUME:
                    case SLIDER_SOUNDVOLUME:
                        if (user_changing_volume == NULL)
                        {
                            SDL_assert(0 && "user_changing_volume is NULL!");
                            break;
                        }
                        *user_changing_volume = previous_volume;
                        deinitvolumeslider();
                        break;
                    default:
                        SDL_assert(0 && "Unhandled slider mode!");
                        break;
                    }
                }
                else if (game.ingame_titlemode
                && game.currentmenuname == Menu::options)
                {
                    game.returntoingame();
                }
                else
                {
                    game.returnmenu();
                    map.nexttowercolour();
                }
            }
        }

        if(game.menustart)
        {
            if (game.slidermode == SLIDER_NONE)
            {
                if (game.currentmenuname == Menu::language)
                {
                    /* The language screen has two columns and navigation in four directions.
                     * The second column may have one less option than the first. */
                    int n_options = game.menuoptions.size();
                    int twocol_voptions = n_options - (n_options/2);

                    if (lang_press_horizontal)
                    {
                        if (game.currentmenuoption < twocol_voptions)
                        {
                            game.currentmenuoption += twocol_voptions;
                            if (game.currentmenuoption >= n_options)
                            {
                                game.currentmenuoption = n_options - 1;
                            }
                        }
                        else
                        {
                            game.currentmenuoption -= twocol_voptions;
                        }
                    }
                    else
                    {
                        /* Vertical movement */
                        int min_option;
                        int max_option;
                        if (game.currentmenuoption < twocol_voptions)
                        {
                            min_option = 0;
                            max_option = twocol_voptions-1;
                        }
                        else
                        {
                            min_option = twocol_voptions;
                            max_option = n_options-1;
                        }

                        if (game.press_left) /* Up, lol */
                        {
                            game.currentmenuoption--;
                            if (game.currentmenuoption < min_option)
                            {
                                game.currentmenuoption = max_option;
                            }
                        }
                        else if (game.press_right) /* Down, lol */
                        {
                            game.currentmenuoption++;
                            if (game.currentmenuoption > max_option)
                            {
                                game.currentmenuoption = min_option;
                            }
                        }
                    }
                }
                else if (game.press_left)
                {
                    game.currentmenuoption--;
                }
                else if (game.press_right)
                {
                    game.currentmenuoption++;
                }
            }
            else
            {
                slidermodeinput();
            }
        }

        if (game.currentmenuoption < 0) game.currentmenuoption = game.menuoptions.size()-1;
        if (game.currentmenuoption >= (int) game.menuoptions.size() ) game.currentmenuoption = 0;

        if (game.press_action)
        {
            if (!game.menustart)
            {
                game.menustart = true;
                music.play(Music_PRESENTINGVVVVVV);
                music.playef(Sound_GAMESAVED);
                game.screenshake = 10;
                game.flashlight = 5;
            }
            else
            {
                menuactionpress();
            }
        }
    }

    if (fadetomode)
    {
        handlefadetomode();
    }
}

void gameinput(void)
{
    //TODO mouse input
    //game.mx = (mouseX / 2);
    //game.my = (mouseY / 2);

    if(!script.running)
    {
        if (roomname_translator::enabled && roomname_translator::overlay_input())
        {
            return;
        }

        game.press_left = false;
        game.press_right = false;
        game.press_action = false;
        game.press_interact = false;

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
        }

        if (key.isDown(KEYBOARD_e) || key.isDown(game.controllerButton_interact))
        {
            game.press_interact = true;
        }
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
                if (GlitchrunnerMode_less_than_or_equal(Glitchrunner2_0)
                || !game.glitchrunkludge)
                {
                    game.state++;
                }
                    game.jumpheld = true;
                    game.glitchrunkludge=true;
                    //Bug fix! You should only be able to do this ONCE.
                    //...Unless you're in glitchrunner mode
            }
        }
    }

    if (!game.press_map
    //Extra conditionals as a kludge fix so if you open the quit menu during
    //the script command gamemode(teleporter) and close it with Esc, it won't
    //immediately open again
    //We really need a better input system soon...
    && !key.isDown(27)
    && !key.isDown(game.controllerButton_esc))
    {
        game.mapheld = false;
    }

    if (!game.press_interact)
    {
        game.interactheld = false;
    }

    if (game.intimetrial && graphics.fademode == FADE_FULLY_BLACK && game.quickrestartkludge && !game.translator_exploring)
    {
        //restart the time trial
        game.quickrestartkludge = false;
        script.startgamemode((enum StartMode) (game.timetriallevel + Start_FIRST_TIMETRIAL));
        game.deathseq = -1;
        game.completestop = false;
        game.hascontrol = false;
    }

    //Returning to editor mode must always be possible
    if (map.custommode && !map.custommodeforreal)
    {
        if ((game.press_map || key.isDown(27)) && !game.mapheld)
        {
            if (!game.separate_interact
            && game.press_map
            && (INBOUNDS_VEC(game.activeactivity, obj.blocks)
            || (game.activetele && game.readytotele > 20)))
            {
                /* Pass, let code block below handle it */
            }
            else
            {
                game.returntoeditor();
                game.mapheld = true;
            }
        }
    }

    //Entity type 0 is player controled
    bool has_control = false;
    bool enter_pressed = game.press_map && !game.mapheld;
    bool enter_already_processed = false;
    bool interact_pressed;
    if (game.separate_interact)
    {
        interact_pressed = game.press_interact && !game.interactheld;
    }
    else
    {
        interact_pressed = enter_pressed;
    }
    for (size_t ie = 0; ie < obj.entities.size(); ++ie)
    {
        if (obj.entities[ie].rule == 0)
        {
            if (game.hascontrol && game.deathseq == -1 && game.lifeseq <= 5)
            {
                has_control = true;
                if (interact_pressed)
                {
                    game.interactheld = true;
                    if (!game.separate_interact)
                    {
                        game.mapheld = true;
                    }
                }

                if (interact_pressed && !script.running)
                {
                    if (game.activetele && game.readytotele > 20 && (!game.intimetrial || game.translator_exploring_allowtele))
                    {
                        enter_already_processed = true;
                        if(int(SDL_fabsf(obj.entities[ie].vx))<=1 && int(obj.entities[ie].vy)==0)
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
                                game.setstate(4000);
                                game.setstatedelay(0);
                            }
                            else if (game.companion == 0 && !game.translator_exploring_allowtele)
                            {
                                //Alright, normal teleporting
                                game.mapmenuchange(TELEPORTERMODE, true);

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
                                game.setstate(3000);
                                game.setstatedelay(0);
                            }
                        }
                    }
                    else if (INBOUNDS_VEC(game.activeactivity, obj.blocks))
                    {
                        enter_already_processed = true;
                        if((int(SDL_fabsf(obj.entities[ie].vx))<=1) && (int(obj.entities[ie].vy) == 0) )
                        {
                            script.load(obj.blocks[game.activeactivity].script);
                            obj.disableblock(game.activeactivity);
                            game.activeactivity = -1;
                        }
                    }
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
            }
        }
    }

    if (has_control)
    {
        if (game.press_left)
        {
            game.tapleft++;
        }
        else
        {
            if (game.tapleft <= 4 && game.tapleft > 0)
            {
                for (size_t ie = 0; ie < obj.entities.size(); ++ie)
                {
                    if (obj.entities[ie].rule == 0)
                    {
                        if (obj.entities[ie].vx < 0.0f)
                        {
                            obj.entities[ie].vx = 0.0f;
                        }
                    }
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
                for (size_t ie = 0; ie < obj.entities.size(); ++ie)
                {
                    if (obj.entities[ie].rule == 0)
                    {
                        if (obj.entities[ie].vx > 0.0f)
                        {
                            obj.entities[ie].vx = 0.0f;
                        }
                    }
                }
            }
            game.tapright = 0;
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

        std::vector<size_t> player_entities;
        for (size_t ie = 0; ie < obj.entities.size(); ie++)
        {
            if (obj.entities[ie].rule == 0)
            {
                player_entities.push_back(ie);
            }
        }

        for (size_t ie = 0; ie < obj.entities.size(); ie++)
        {
            const bool process_flip = obj.entities[ie].rule == 0 &&
                game.jumppressed > 0;
            if (!process_flip)
            {
                continue;
            }

            game.jumppressed--;
            if (obj.entities[ie].onground > 0 && game.gravitycontrol == 0)
            {
                game.gravitycontrol = 1;
                for (size_t j = 0; j < player_entities.size(); j++)
                {
                    const size_t e = player_entities[j];
                    if (obj.entities[e].onground > 0 || obj.entities[e].onroof > 0)
                    {
                        obj.entities[e].vy = -4;
                        obj.entities[e].ay = -3;
                    }
                }
                music.playef(Sound_FLIP);
                game.jumppressed = 0;
                game.totalflips++;
            }
            if (obj.entities[ie].onroof > 0 && game.gravitycontrol == 1)
            {
                game.gravitycontrol = 0;
                for (size_t j = 0; j < player_entities.size(); j++)
                {
                    const size_t e = player_entities[j];
                    if (obj.entities[e].onground > 0 || obj.entities[e].onroof > 0)
                    {
                        obj.entities[e].vy = 4;
                        obj.entities[e].ay = 3;
                    }
                }
                music.playef(Sound_UNFLIP);
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

    /* The rest of the if-tree runs only if enter is pressed and it has not
     * already been processed with 'separate interact' off.
     */
    if (!enter_pressed || (enter_already_processed && !game.separate_interact))
    {
        // Do nothing
    }
    else if (game.swnmode == 1
    && (game.swngame == SWN_SUPERGRAVITRON ||
        game.swngame == SWN_START_SUPERGRAVITRON_STEP_1 ||
        game.swngame == SWN_START_SUPERGRAVITRON_STEP_2))
    {
        //quitting the super gravitron
        game.mapheld = true;
        //Quit menu, same conditions as in game menu
        game.mapmenuchange(MAPMODE, true);
        game.gamesaved = false;
        game.gamesavefailed = false;
        game.menupage = 20; // The Map Page
    }
    else if (game.intimetrial && graphics.fademode == FADE_NONE && !game.translator_exploring)
    {
        //Quick restart of time trial
        graphics.fademode = FADE_START_FADEOUT;
        game.completestop = true;
        music.fadeout();
        game.quickrestartkludge = true;
    }
    else if (game.intimetrial && !game.translator_exploring)
    {
        //Do nothing if we're in a Time Trial but a fade animation is playing
    }
    else if (map.custommode && !map.custommodeforreal)
    {
        // We're playtesting in the editor so don't do anything
    }
    else
    {
        //Normal map screen, do transition later
        game.mapmenuchange(MAPMODE, true);
        map.cursordelay = 0;
        map.cursorstate = 0;
        game.gamesaved = false;
        game.gamesavefailed = false;
        if (script.running)
        {
            game.menupage = 3; // Only allow saving
        }
        else
        {
            game.menupage = 0; // The Map Page
        }
    }

    if (!game.mapheld
    && (key.isDown(27) || key.isDown(game.controllerButton_esc))
    && (!map.custommode || map.custommodeforreal))
    {
        game.mapheld = true;
        //Quit menu, same conditions as in game menu
        game.mapmenuchange(MAPMODE, true);
        game.gamesaved = false;
        game.gamesavefailed = false;
        game.menupage = 30; // Pause screen
    }

    if (game.deathseq == -1 && (key.isDown(SDLK_r) || key.isDown(game.controllerButton_restart)) && !game.nodeathmode)// && map.custommode) //Have fun glitchrunners!
    {
        game.deathseq = 30;
    }
}

static void mapmenuactionpress(bool version2_2);

void mapinput(void)
{
    const bool version2_2 = GlitchrunnerMode_less_than_or_equal(Glitchrunner2_2);

    //TODO Mouse Input!
    //game.mx = (mouseX / 2);
    //game.my = (mouseY / 2);

    game.press_left = false;
    game.press_right = false;
    game.press_action = false;
    game.press_map = false;
    game.press_interact = false;

    if (version2_2 && graphics.fademode == FADE_FULLY_BLACK && graphics.menuoffset == 0)
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
            game.setstate(96);
            game.setstatedelay(0);
        }
        else
        {
            // Produces more glitchiness! Necessary for credits warp to work.
            script.running = false;
            graphics.textboxes.clear();

            game.setstate(80);
            game.setstatedelay(0);
        }
    }

    if (game.fadetomenu && !version2_2)
    {
        if (game.fadetomenudelay > 0)
        {
            game.fadetomenudelay--;
        }
        else
        {
            game.quittomenu();
            music.play(Music_PRESENTINGVVVVVV); // should be after game.quittomenu()
            game.fadetomenu = false;
        }
    }

    if (game.fadetolab && !version2_2)
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
    && ((!version2_2 && !game.fadetomenu && game.fadetomenudelay <= 0 && !game.fadetolab && game.fadetolabdelay <= 0)
    || graphics.fademode == FADE_NONE))
    {
        if (key.isDown(KEYBOARD_LEFT) || key.isDown(KEYBOARD_UP) || key.isDown(KEYBOARD_a) ||  key.isDown(KEYBOARD_w)|| key.controllerWantsLeft(true))
        {
            game.press_left = true;
        }
        if (key.isDown(KEYBOARD_RIGHT) || key.isDown(KEYBOARD_DOWN) || key.isDown(KEYBOARD_d) ||  key.isDown(KEYBOARD_s)|| key.controllerWantsRight(true))
        {
            game.press_right = true;
        }
        if (key.isDown(KEYBOARD_z) || key.isDown(KEYBOARD_SPACE) || key.isDown(KEYBOARD_v) || key.isDown(game.controllerButton_flip))
        {
            game.press_action = true;
        }
        if (game.menupage < 12
        || (game.menupage >= 20 && game.menupage <= 21)
        || (game.menupage >= 30 && game.menupage <= 32))
        {
            if (key.isDown(KEYBOARD_ENTER) || key.isDown(game.controllerButton_map) ) game.press_map = true;
            if (key.isDown(27) && !game.mapheld)
            {
                game.mapheld = true;
                if (game.menupage < 9
                || (game.menupage >= 20 && game.menupage <= 21))
                {
                    game.menupage = 30;
                }
                else if (game.menupage < 12)
                {
                    game.menupage = 32;
                }
                else
                {
                    graphics.resumegamemode = true;
                }
                music.playef(Sound_VIRIDIAN);
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

        if (script.running && game.menupage == 3)
        {
            // Force the player to stay in the SAVE tab while in a cutscene
        }
        else if (game.press_left)
        {
            game.menupage--;
        }
        else if (game.press_right)
        {
            game.menupage++;
        }

        if (game.press_action)
        {
            mapmenuactionpress(version2_2);
        }

        if (game.menupage < 0) game.menupage = 3;
        if (game.menupage > 3 && game.menupage < 9) game.menupage = 0;

        if (game.menupage == 9) game.menupage = 11;
        if (game.menupage == 12) game.menupage = 10;

        if (game.menupage == 19) game.menupage = 21;
        if (game.menupage == 22) game.menupage = 20;

        if (game.menupage == 29) game.menupage = 32;
        if (game.menupage == 33) game.menupage = 30;
    }
}

static void mapmenuactionpress(const bool version2_2)
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
        game.setstate(4000);
        game.setstatedelay(0);
        game.lockstate();
    }
        break;
    case 3:
    if (!game.gamesaved && !game.gamesavefailed && !game.inspecial())
    {
        game.flashlight = 5;
        game.screenshake = 10;
        music.playef(Sound_GAMESAVED);

        game.savetime = game.timestring();
        game.savearea = map.currentarea(map.area(game.roomx, game.roomy));
        game.savetrinkets = game.trinkets();

        if (game.roomx >= 102 && game.roomx <= 104 && game.roomy >= 110 && game.roomy <= 111) game.savearea = loc::gettext_roomname_special("The Ship");

        bool success;

        if(map.custommodeforreal)
        {
            success = game.customsavequick(cl.ListOfMetaData[game.playcustomlevel].filename);
        }
        else
        {
            success = game.savequick();
        }
        game.gamesaved = success;
        game.gamesavefailed = !success;
    }
        break;

    case 10:
        //return to pause menu
        music.playef(Sound_VIRIDIAN);
        game.menupage = 32;
        break;
    case 11:
        //quit to menu

        graphics.fademode = FADE_START_FADEOUT;
        music.fadeout();
        map.nexttowercolour();
        if (!version2_2)
        {
            game.fadetomenu = true;
            game.fadetomenudelay = 19;
        }
        music.playef(Sound_VIRIDIAN);
        break;

    case 20:
        //return to game
        graphics.resumegamemode = true;
        music.playef(Sound_VIRIDIAN);
        break;
    case 21:
        //quit to menu
        game.swnmode = false;
        graphics.fademode = FADE_START_FADEOUT;
        music.fadeout();
        if (!version2_2)
        {
            game.fadetolab = true;
            game.fadetolabdelay = 19;
        }
        music.playef(Sound_VIRIDIAN);
        break;
    case 30:
        // Return to game
        graphics.resumegamemode = true;
        music.playef(Sound_VIRIDIAN);
        break;
    case 31:
        // Graphic options and game options
        music.playef(Sound_VIRIDIAN);
        game.gamestate = TITLEMODE;
        graphics.flipmode = false;
        game.ingame_titlemode = true;
        graphics.ingame_fademode = graphics.fademode;
        graphics.fademode = FADE_NONE;

        // Set this before we create the menu
        game.kludge_ingametemp = game.currentmenuname;
        game.createmenu(Menu::options);
        map.nexttowercolour();
        break;
    case 32:
        // Go to quit prompt
        music.playef(Sound_VIRIDIAN);
        game.menupage = 10;
        break;
    }
}

void teleporterinput(void)
{
    //Todo Mouseinput!
    //game.mx = (mouseX / 2);
    //game.my = (mouseY / 2);

    int tempx, tempy;

    game.press_left = false;
    game.press_right = false;
    game.press_action = false;
    game.press_map = false;
    game.press_interact = false;

    if(graphics.menuoffset==0)
    {
        if (key.isDown(KEYBOARD_LEFT)|| key.isDown(KEYBOARD_a) || key.controllerWantsLeft(false) ) game.press_left = true;
        if (key.isDown(KEYBOARD_RIGHT) || key.isDown(KEYBOARD_d)|| key.controllerWantsRight(false) ) game.press_right = true;
        if (key.isDown(KEYBOARD_z) || key.isDown(KEYBOARD_SPACE) || key.isDown(KEYBOARD_v)
                || key.isDown(KEYBOARD_UP) || key.isDown(KEYBOARD_DOWN)||  key.isDown(KEYBOARD_w)||  key.isDown(KEYBOARD_s) || key.isDown(game.controllerButton_flip)) game.press_action = true;
        if (!game.separate_interact && (key.isDown(KEYBOARD_ENTER) || key.isDown(game.controllerButton_map)))
        {
            game.press_map = true;
        }
        if (key.isDown(KEYBOARD_e) || key.isDown(game.controllerButton_interact))
        {
            game.press_interact = true;
        }

        //In the menu system, all keypresses are single taps rather than holds. Therefore this test has to be done for all presses
        if (!game.press_action && !game.press_left && !game.press_right && !game.press_interact) game.jumpheld = false;
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
            music.playef(Sound_VIRIDIAN);
        }
    }
    else
    {
        game.mapheld = true;
        game.jumpheld = true;
    }

    if (!game.jumpheld)
    {
        if (game.press_action || game.press_left || game.press_right || game.press_map || game.press_interact)
        {
            game.jumpheld = true;
        }

        bool any_tele_unlocked = false;
        if (game.press_left || game.press_right)
        {
            for (size_t i = 0; i < map.teleporters.size(); i++)
            {
                SDL_Point& tele = map.teleporters[i];

                if (map.isexplored(tele.x, tele.y))
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
            while (!map.isexplored(tempx, tempy));
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
            while (!map.isexplored(tempx, tempy));
        }

        if ((game.separate_interact && game.press_interact) || game.press_map)
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
                game.setstate(4000);
                game.setstatedelay(0);
            }
        }
    }
}

void gamecompleteinput(void)
{
    game.press_left = false;
    game.press_right = false;
    game.press_action = false;
    game.press_map = false;
    game.press_interact = false;

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
            if (graphics.fademode == FADE_NONE)
            {
                graphics.fademode = FADE_START_FADEOUT;
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
            if(graphics.fademode == FADE_NONE)
            {
                graphics.fademode = FADE_START_FADEOUT;
            }
        }
    }

    if (!game.press_map)
    {
        game.mapheld = false;
    }
}

void gamecompleteinput2(void)
{
    game.press_left = false;
    game.press_right = false;
    game.press_action = false;
    game.press_map = false;
    game.press_interact = false;

    //Do this here because input comes first
    game.oldcreditposx = game.creditposx;

    if (key.isDown(KEYBOARD_z) || key.isDown(KEYBOARD_SPACE) || key.isDown(KEYBOARD_v) || key.isDown(game.controllerButton_flip))
    {
        game.creditposx++;
        game.oldcreditposx++;
        if (game.creditposy >= 30)
        {
            if(graphics.fademode == FADE_NONE)
            {
                graphics.fademode = FADE_START_FADEOUT;
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
            if(graphics.fademode == FADE_NONE)
            {
                graphics.fademode = FADE_START_FADEOUT;
                music.fadeout();
            }
        }
    }

    if (!game.press_map)
    {
        game.mapheld = false;
    }
}
