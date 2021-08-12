#include <SDL.h>

#include "Credits.h"
#include "editor.h"
#include "Entity.h"
#include "FileSystemUtils.h"
#include "GlitchrunnerMode.h"
#include "Graphics.h"
#include "GraphicsUtil.h"
#include "KeyPoll.h"
#include "MakeAndPlay.h"
#include "Map.h"
#include "Maths.h"
#include "Music.h"
#include "Script.h"
#include "UtilityClass.h"
#include "Version.h"

static int tr;
static int tg;
static int tb;

// Macro-like inline function used in maprender()
// Used to keep some text positions the same in Flip Mode
static int inline FLIP(int ypos)
{
    if (graphics.flipmode)
    {
        return 220 - ypos;
    }
    return ypos;
}

static inline void drawslowdowntext(void)
{
    switch (game.slowdown)
    {
    case 30:
        graphics.Print( -1, 105, "Game speed is normal.", tr/2, tg/2, tb/2, true);
        break;
    case 24:
        graphics.Print( -1, 105, "Game speed is at 80%", tr, tg, tb, true);
        break;
    case 18:
        graphics.Print( -1, 105, "Game speed is at 60%", tr, tg, tb, true);
        break;
    case 12:
        graphics.Print( -1, 105, "Game speed is at 40%", tr, tg, tb, true);
        break;
    }
}

static void volumesliderrender(void)
{
    char buffer[40 + 1];

    char slider[20 + 1];
    int slider_length;

    const char symbol[] = "[]";
    int symbol_length;

    int offset;
    int num_positions;

    const int* volume_ptr;

    switch (game.currentmenuoption)
    {
    case 0:
        volume_ptr = &music.user_music_volume;
        break;
    case 1:
        volume_ptr = &music.user_sound_volume;
        break;
    default:
        SDL_assert(0 && "Unhandled volume slider menu option!");
        return;
    }

    VVV_fillstring(slider, sizeof(slider), '.');
    slider_length = sizeof(slider) - 1;

    symbol_length = sizeof(symbol) - 1;

    num_positions = slider_length - symbol_length + 1;

    offset = num_positions * (*volume_ptr) / USER_VOLUME_MAX;
    offset = clamp(offset, 0, slider_length - symbol_length);

    /* SDL_strlcpy null-terminates, which would end the string in the middle of
     * it, which we don't want!
     */
    SDL_memcpy(&slider[offset], symbol, symbol_length);

    if (game.slidermode == SLIDER_NONE)
    {
        SDL_strlcpy(buffer, slider, sizeof(buffer));
    }
    else
    {
        /* Draw selection brackets. */
        SDL_snprintf(buffer, sizeof(buffer), "[ %s ]", slider);
    }

    graphics.Print(-1, 85, buffer, tr, tg, tb, true);
}

static void inline drawglitchrunnertext(void)
{
    int tempr = tr;
    int tempg = tg;
    int tempb = tb;

    /* Screen width 40 chars, 4 per char */
    char buffer[160 + 1];

    const char* mode_string;

    const enum GlitchrunnerMode mode = GlitchrunnerMode_get();

    if (mode == GlitchrunnerNone)
    {
        tempr /= 2;
        tempg /= 2;
        tempb /= 2;

        mode_string = "OFF";
    }
    else
    {
        mode_string = GlitchrunnerMode_enum_to_string(mode);
    }

    SDL_snprintf(buffer, sizeof(buffer), "Glitchrunner mode is %s", mode_string);

    graphics.Print(-1, 95, buffer, tempr, tempg, tempb, true);
}

static void menurender(void)
{
    int temp = 50;

    switch (game.currentmenuname)
    {
    case Menu::mainmenu:
        graphics.drawsprite((160 - 96) + 0 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 1 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 2 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 3 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 4 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 5 * 32, temp, 23, tr, tg, tb);
#if defined(MAKEANDPLAY)
        graphics.Print(-1,temp+35,"     MAKE AND PLAY EDITION",tr, tg, tb, true);
#endif
#ifdef COMMIT_DATE
        graphics.Print( 310 - (10*8), 210, COMMIT_DATE, tr/2, tg/2, tb/2);
#endif
#ifdef INTERIM_COMMIT
        graphics.Print( 310 - (SDL_arraysize(INTERIM_COMMIT) - 1) * 8, 220, INTERIM_COMMIT, tr/2, tg/2, tb/2);
#endif
        graphics.Print( 310 - (4*8), 230, "v2.3", tr/2, tg/2, tb/2);

        if(music.mmmmmm){
            graphics.Print( 10, 230, "[MMMMMM Mod Installed]", tr/2, tg/2, tb/2);
        }
        break;
#if !defined(NO_CUSTOM_LEVELS)
    case Menu::levellist:
    {
      if(ed.ListOfMetaData.size()==0){
      graphics.Print( -1, 100, "ERROR: No levels found.", tr, tg, tb, true);
      }
      int tmp=game.currentmenuoption+(game.levelpage*8);
      if(INBOUNDS_VEC(tmp, ed.ListOfMetaData)){
        const bool nextlastoptions = ed.ListOfMetaData.size() > 8;
        //Don't show next/previous page or return to menu options here!
        if(nextlastoptions && game.menuoptions.size() - game.currentmenuoption<=3){

        }else{
          graphics.bigprint( -1, 15, ed.ListOfMetaData[tmp].title, tr, tg, tb, true);
          graphics.Print( -1, 40, "by " + ed.ListOfMetaData[tmp].creator, tr, tg, tb, true);
          graphics.Print( -1, 50, ed.ListOfMetaData[tmp].website, tr, tg, tb, true);
          graphics.Print( -1, 70, ed.ListOfMetaData[tmp].Desc1, tr, tg, tb, true);
          graphics.Print( -1, 80, ed.ListOfMetaData[tmp].Desc2, tr, tg, tb, true);
          graphics.Print( -1, 90, ed.ListOfMetaData[tmp].Desc3, tr, tg, tb, true);
        }
      }
      break;
    }
#endif
    case Menu::errornostart:
        graphics.Print( -1, 65, "ERROR: This level has", tr, tg, tb, true);
        graphics.Print( -1, 75, "no start point!", tr, tg, tb, true);
        break;
    case Menu::gameplayoptions:
    {
        int gameplayoptionsoffset = 0;
#if !defined(MAKEANDPLAY)
        if (game.ingame_titlemode && game.unlock[18])
#endif
        {
            gameplayoptionsoffset = 1;
            if (game.currentmenuoption == 0) {
                graphics.bigprint(-1, 30, "Flip Mode", tr, tg, tb, true);
                graphics.Print(-1, 65, "Flip the entire game vertically.", tr, tg, tb, true);
                if (graphics.setflipmode)
                {
                    graphics.Print(-1, 85, "Currently ENABLED!", tr, tg, tb, true);
                }
                else
                {
                    graphics.Print(-1, 85, "Currently Disabled.", tr / 2, tg / 2, tb / 2, true);
                }
            }
        }

        if (game.currentmenuoption == gameplayoptionsoffset + 0)
        {
            //Toggle FPS
            graphics.bigprint(-1, 30, "Toggle 30+ FPS", tr, tg, tb, true);
            graphics.Print(-1, 65, "Change whether the game", tr, tg, tb, true);
            graphics.Print(-1, 75, "runs at 30 or over 30 FPS.", tr, tg, tb, true);

            if (!game.over30mode)
            {
                graphics.Print(-1, 95, "Current mode: 30 FPS", tr / 2, tg / 2, tb / 2, true);
            }
            else
            {
                graphics.Print(-1, 95, "Current mode: Over 30 FPS", tr, tg, tb, true);
            }
            break;
        }
        else if (game.currentmenuoption == gameplayoptionsoffset + 1)
        {
            //Speedrunner options
            graphics.bigprint(-1, 30, "Speedrunner Options", tr, tg, tb, true);
            graphics.Print(-1, 65, "Access some advanced settings that", tr, tg, tb, true);
            graphics.Print(-1, 75, "might be of interest to speedrunners", tr, tg, tb, true);
        }
        else if (game.currentmenuoption == gameplayoptionsoffset + 2)
        {
            //Advanced options
            graphics.bigprint(-1, 30, "Advanced Options", tr, tg, tb, true);
            graphics.Print(-1, 65, "All other settings", tr, tg, tb, true);
        }
        else if (game.currentmenuoption == gameplayoptionsoffset + 3)
        {
            //Clear Data
            graphics.bigprint(-1, 30, "Clear Data", tr, tg, tb, true);
            graphics.Print(-1, 65, "Delete your save data", tr, tg, tb, true);
            graphics.Print(-1, 75, "and unlocked play modes", tr, tg, tb, true);
        }

        break;
    }
    case Menu::options:
        switch (game.currentmenuoption)
        {
        case 0:
            graphics.bigprint(-1, 30, "Gameplay Options", tr, tg, tb, true);
            graphics.Print(-1, 65, "Adjust various gameplay options", tr, tg, tb, true);
            break;
        case 1:
            graphics.bigprint(-1, 30, "Graphics Options", tr, tg, tb, true);
            graphics.Print(-1, 65, "Adjust screen settings", tr, tg, tb, true);
            break;
        case 2:
            graphics.bigprint(-1, 30, "Audio Options", tr, tg, tb, true);
            graphics.Print(-1, 65, "Adjust volume settings", tr, tg, tb, true);
            if (music.mmmmmm)
            {
                graphics.Print(-1, 75, "and soundtrack", tr, tg, tb, true);
            }
            break;
        case 3:
            graphics.bigprint(-1, 30, "Game Pad Options", tr, tg, tb, true);
            graphics.Print(-1, 65, "Rebind your controller's buttons", tr, tg, tb, true);
            graphics.Print(-1, 75, "and adjust sensitivity", tr, tg, tb, true);
            break;
        case 4:
            graphics.bigprint(-1, 30, "Accessibility", tr, tg, tb, true);
            graphics.Print(-1, 65, "Disable screen effects, enable", tr, tg, tb, true);
            graphics.Print(-1, 75, "slowdown modes or invincibility", tr, tg, tb, true);
            break;
        }
        break;
    case Menu::graphicoptions:
        if (graphics.screenbuffer == NULL)
        {
            SDL_assert(0 && "Screenbuffer is NULL!");
            break;
        }

        switch (game.currentmenuoption)
        {
        case 0:
            graphics.bigprint( -1, 30, "Toggle Fullscreen", tr, tg, tb, true);
            graphics.Print( -1, 65, "Change to fullscreen/windowed mode.", tr, tg, tb, true);

            if (graphics.screenbuffer->isWindowed)
            {
                graphics.Print( -1, 85, "Current mode: WINDOWED", tr, tg, tb, true);
            }
            else
            {
                graphics.Print( -1, 85, "Current mode: FULLSCREEN", tr, tg, tb, true);
            }
            break;

        case 1:
            graphics.bigprint( -1, 30, "Scaling Mode", tr, tg, tb, true);
            graphics.Print( -1, 65, "Choose letterbox/stretch/integer mode.", tr, tg, tb, true);

            switch (graphics.screenbuffer->stretchMode)
            {
            case 2:
                graphics.Print( -1, 85, "Current mode: INTEGER", tr, tg, tb, true);
                break;
            case 1:
                graphics.Print( -1, 85, "Current mode: STRETCH", tr, tg, tb, true);
                break;
            default:
                graphics.Print( -1, 85, "Current mode: LETTERBOX", tr, tg, tb, true);
                break;
            }
            break;
        case 2:
            graphics.bigprint(-1, 30, "Resize to Nearest", tr, tg, tb, true);
            graphics.Print(-1, 65, "Resize to the nearest window size", tr, tg, tb, true);
            graphics.Print(-1, 75, "that is of an integer multiple.", tr, tg, tb, true);
            if (!graphics.screenbuffer->isWindowed)
            {
                graphics.Print(-1, 95, "You must be in windowed mode", tr, tg, tb, true);
                graphics.Print(-1, 105, "to use this option.", tr, tg, tb, true);
            }
            break;
        case 3:
            graphics.bigprint( -1, 30, "Toggle Filter", tr, tg, tb, true);
            graphics.Print( -1, 65, "Change to nearest/linear filter.", tr, tg, tb, true);

            if (graphics.screenbuffer->isFiltered)
            {
                graphics.Print( -1, 85, "Current mode: LINEAR", tr, tg, tb, true);
            }
            else
            {
                graphics.Print( -1, 85, "Current mode: NEAREST", tr, tg, tb, true);
            }
            break;

        case 4:
            graphics.bigprint( -1, 30, "Analogue Mode", tr, tg, tb, true);
            graphics.Print( -1, 65, "There is nothing wrong with your", tr, tg, tb, true);
            graphics.Print( -1, 75, "television set. Do not attempt to", tr, tg, tb, true);
            graphics.Print( -1, 85, "adjust the picture.", tr, tg, tb, true);
            break;
        case 5:
            graphics.bigprint(-1, 30, "Toggle VSync", tr, tg, tb, true);
#ifdef __HAIKU__ // FIXME: Remove after SDL VSync bug is fixed! -flibit
            graphics.Print(-1, 65, "Edit the config file on Haiku!", tr, tg, tb, true);
#else
            graphics.Print(-1, 65, "Turn VSync on or off.", tr, tg, tb, true);
#endif

            if (!graphics.screenbuffer->vsync)
            {
                graphics.Print(-1, 95, "Current mode: VSYNC OFF", tr/2, tg/2, tb/2, true);
            }
            else
            {
                graphics.Print(-1, 95, "Current mode: VSYNC ON", tr, tg, tb, true);
            }
            break;
        }
        break;
    case Menu::audiooptions:
        switch (game.currentmenuoption)
        {
        case 0:
            graphics.bigprint(-1, 30, "Music Volume", tr, tg, tb, true);
            graphics.Print(-1, 65, "Change the volume of the music.", tr, tg, tb, true);
            volumesliderrender();
            break;
        case 1:
            graphics.bigprint(-1, 30, "Sound Volume", tr, tg, tb, true);
            graphics.Print(-1, 65, "Change the volume of sound effects.", tr, tg, tb, true);
            volumesliderrender();
            break;
        case 2:
            if (!music.mmmmmm)
            {
                break;
            }
        {
            /* Screen width 40 chars, 4 per char */
            char buffer[160 + 1];
            char soundtrack[6 + 1];
            char letter;
            if (music.usingmmmmmm)
            {
                letter = 'M';
            }
            else
            {
                letter = 'P';
            }
            VVV_fillstring(soundtrack, sizeof(soundtrack), letter);
            SDL_snprintf(buffer, sizeof(buffer), "Current soundtrack: %s", soundtrack);

            graphics.bigprint(-1, 30, "Soundtrack", tr, tg, tb, true);
            graphics.Print(-1, 65, "Toggle between MMMMMM and PPPPPP", tr, tg, tb, true);
            graphics.Print(-1, 85, buffer, tr, tg, tb, true);
            break;
        }

        }
        break;
    case Menu::credits:
        graphics.Print( -1, 50, "VVVVVV is a game by", tr, tg, tb, true);
        graphics.bigprint( 40, 65, "Terry Cavanagh", tr, tg, tb, true, 2);

        graphics.drawimagecol(7, -1, 86, tr *0.75, tg *0.75, tb *0.75, true);

        graphics.Print( -1, 120, "and features music by", tr, tg, tb, true);
        graphics.bigprint( 40, 135, "Magnus P~lsson", tr, tg, tb, true, 2);
        graphics.drawimagecol(8, -1, 156, tr *0.75, tg *0.75, tb *0.75, true);
        break;
    case Menu::credits2:
        graphics.Print( -1, 50, "Roomnames are by", tr, tg, tb, true);
        graphics.bigprint( 40, 65, "Bennett Foddy", tr, tg, tb, true);
        graphics.drawimagecol(9, -1, 86, tr*0.75, tg *0.75, tb *0.75, true);
        graphics.Print( -1, 110, "C++ version by", tr, tg, tb, true);
        graphics.bigprint( 40, 125, "Simon Roth", tr, tg, tb, true);
        graphics.bigprint( 40, 145, "Ethan Lee", tr, tg, tb, true);
        break;
    case Menu::credits25:
        graphics.Print( -1, 40, "Beta Testing by", tr, tg, tb, true);
        graphics.bigprint( 40, 55, "Sam Kaplan", tr, tg, tb, true);
        graphics.bigprint( 40, 75, "Pauli Kohberger", tr, tg, tb, true);
        graphics.Print( -1, 130, "Ending Picture by", tr, tg, tb, true);
        graphics.bigprint( 40, 145, "Pauli Kohberger", tr, tg, tb, true);
        break;
    case Menu::credits3:
    {
        graphics.Print( -1, 20, "VVVVVV is supported by", tr, tg, tb, true);
        graphics.Print( 40, 30, "the following patrons", tr, tg, tb, true);

        int startidx = game.current_credits_list_index;
        int endidx = VVV_min(startidx + 9, (int)SDL_arraysize(Credits::superpatrons));

        int xofs = 80 - 16;
        int yofs = 40 + 20;

        for (int i = startidx; i < endidx; ++i)
        {
            graphics.Print(xofs, yofs, Credits::superpatrons[i], tr, tg, tb);
            xofs += 4;
            yofs += 14;
        }
        break;
    }
    case Menu::credits4:
    {
        graphics.Print( -1, 20, "and also by", tr, tg, tb, true);

        int startidx = game.current_credits_list_index;
        int endidx = VVV_min(startidx + 14, (int)SDL_arraysize(Credits::patrons));

        int maxheight = 10 * 14;
        int totalheight = (endidx - startidx) * 10;
        int emptyspace = maxheight - totalheight;

        int yofs = 40 + (emptyspace / 2);

        for (int i = startidx; i < endidx; ++i)
        {
            graphics.Print(80, yofs, Credits::patrons[i], tr, tg, tb);
            yofs += 10;
        }
        break;
    }
    case Menu::credits5:
    {
        graphics.Print( -1, 20, "With contributions on", tr, tg, tb, true);
        graphics.Print( 40, 30, "GitHub from", tr, tg, tb, true);

        int startidx = game.current_credits_list_index;
        int endidx = VVV_min(startidx + 9, (int)SDL_arraysize(Credits::githubfriends));

        int maxheight = 14 * 9;
        int totalheight = (endidx - startidx) * 14;
        int emptyspace = maxheight - totalheight;

        int xofs, yofs;

        if (startidx == 0)
        {
            graphics.Print(-1, 60, Credits::githubfriends[0], tr, tg, tb, true);
            graphics.Print(-1, 80, Credits::githubfriends[2], tr, tg, tb, true);
            startidx += 4; // Skip the superfriends now that we've drawn them...
            xofs = 80 - 28;
            yofs = 80 + 20 + (emptyspace / 2);
        }
        else
        {
            xofs = 80 - 16;
            yofs = 40 + 20 + (emptyspace / 2);
        }

        for (int i = startidx; i < endidx; ++i)
        {
            graphics.Print(xofs, yofs, Credits::githubfriends[i], tr, tg, tb);
            xofs += 4;
            yofs += 14;
        }
        break;
    }
    case Menu::credits6:
        graphics.Print( -1, 20, "and thanks also to:", tr, tg, tb, true);

        graphics.bigprint(80, 60, "You!", tr, tg, tb, true);

        graphics.Print( 80, 100, "Your support makes it possible", tr, tg, tb,true);
        graphics.Print( 80, 110,"for me to continue making the", tr, tg, tb,true);
        graphics.Print( 80, 120,"games I want to make, now", tr, tg, tb,true);
        graphics.Print( 80, 130, "and into the future.", tr, tg, tb, true);

        graphics.Print( 80, 150,"Thank you!", tr, tg, tb,true);
        break;
    case Menu::setinvincibility:
        graphics.Print( -1, 100, "Are you sure you want to ", tr, tg, tb, true);
        graphics.Print( -1, 110, "enable invincibility?", tr, tg, tb, true);
        break;
    case Menu::setslowdown:
        graphics.bigprint( -1, 40, "Game Speed", tr, tg, tb, true);
        graphics.Print( -1, 75, "Select a new game speed below.", tr, tg, tb, true);
        drawslowdowntext();
        break;
    case Menu::newgamewarning:
        graphics.Print( -1, 100, "Are you sure? This will", tr, tg, tb, true);
        graphics.Print( -1, 110, "delete your current saves...", tr, tg, tb, true);
        break;
    case Menu::cleardatamenu:
        graphics.Print( -1, 100, "Are you sure you want to", tr, tg, tb, true);
        graphics.Print( -1, 110, "delete all your saved data?", tr, tg, tb, true);
        break;
    case Menu::deletequicklevel:
        graphics.Print(-1, 100, "Are you sure you want to", tr, tg, tb, true);
        graphics.Print(-1, 110, "delete your quicksave?", tr, tg, tb, true);
        break;
    case Menu::startnodeathmode:
        graphics.Print( -1, 45, "Good luck!", tr, tg, tb, true);
        graphics.Print( -1, 80, "You cannot save in this mode.", tr, tg, tb, true);
        graphics.Print( -1, 100, "Would you like to disable the", tr, tg, tb, true);
        graphics.Print( -1, 112, "cutscenes during the game?", tr, tg, tb, true);
        break;
    case Menu::controller:
        graphics.bigprint( -1, 30, "Game Pad", tr, tg, tb, true);
        graphics.Print( -1, 55, "Change controller options.", tr, tg, tb, true);
        switch (game.currentmenuoption)
        {
        case 0:
            switch(key.sensitivity)
            {
            case 0:
                graphics.Print( -1, 75, " Low     Medium     High", tr, tg, tb, true);
                graphics.Print( -1, 85, "[]..................", tr, tg, tb, true);
                break;
            case 1:
                graphics.Print( -1, 75, " Low     Medium     High", tr, tg, tb, true);
                graphics.Print( -1, 85, ".....[].............", tr, tg, tb, true);
                break;
            case 2:
                graphics.Print( -1, 75, " Low     Medium     High", tr, tg, tb, true);
                graphics.Print( -1, 85, ".........[].........", tr, tg, tb, true);
                break;
            case 3:
                graphics.Print( -1, 75, " Low     Medium     High", tr, tg, tb, true);
                graphics.Print( -1, 85, ".............[].....", tr, tg, tb, true);
                break;
            case 4:
                graphics.Print( -1, 75, " Low     Medium     High", tr, tg, tb, true);
                graphics.Print( -1, 85, "..................[]", tr, tg, tb, true);
                break;
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            graphics.Print( -1, 75, "Flip is bound to: " + std::string(help.GCString(game.controllerButton_flip)) , tr, tg, tb, true);
            graphics.Print( -1, 85, "Enter is bound to: "  + std::string(help.GCString(game.controllerButton_map)), tr, tg, tb, true);
            graphics.Print( -1, 95, "Menu is bound to: " + std::string(help.GCString(game.controllerButton_esc)) , tr, tg, tb, true);
            graphics.Print( -1, 105, "Restart is bound to: " + std::string(help.GCString(game.controllerButton_restart)) , tr, tg, tb, true);
            graphics.Print( -1, 115, "Interact is bound to: " + std::string(help.GCString(game.controllerButton_interact)) , tr, tg, tb, true);
            break;
        }


        break;
    case Menu::speedrunneroptions:
        switch (game.currentmenuoption)
        {
        case 0:
            graphics.bigprint(-1, 30, "Glitchrunner Mode", tr, tg, tb, true);
            graphics.Print(-1, 65, "Re-enable glitches that existed", tr, tg, tb, true);
            graphics.Print(-1, 75, "in previous versions of the game.", tr, tg, tb, true);
            drawglitchrunnertext();
            break;
        case 1:
            graphics.bigprint(-1, 30, "Input Delay", tr, tg, tb, true);
            graphics.Print(-1, 65, "Re-enable the 1-frame input delay", tr, tg, tb, true);
            graphics.Print(-1, 75, "from previous versions of the game.", tr, tg, tb, true);
            if (game.inputdelay)
            {
                graphics.Print(-1, 95, "Input delay is ON", tr, tg, tb, true);
            }
            else
            {
                graphics.Print(-1, 95, "Input delay is OFF", tr / 2, tg / 2, tb / 2, true);
            }
            break;
        case 2:
        {
            /* Screen width 40 chars, 4 per char */
            char buffer[160 + 1];
            const char* button;

            graphics.bigprint(-1, 30, "Interact Button", tr, tg, tb, true);
            graphics.Print(-1, 65, "Toggle whether you interact", tr, tg, tb, true);
            graphics.Print(-1, 75, "with prompts using ENTER or E.", tr, tg, tb, true);

            if (game.separate_interact)
            {
                button = "E";
            }
            else
            {
                button = "ENTER";
            }

            SDL_snprintf(buffer, sizeof(buffer), "Interact button: %s", button);
            graphics.Print(-1, 95, buffer, tr, tg, tb, true);
            break;
        }
        case 3:
            graphics.bigprint(-1, 30, "Fake Load Screen", tr, tg, tb, true);
            if (game.skipfakeload)
                graphics.Print(-1, 65, "Fake loading screen is OFF", tr / 2, tg / 2, tb / 2, true);
            else
                graphics.Print(-1, 65, "Fake loading screen is ON", tr, tg, tb, true);
            break;
        case 4:
            graphics.bigprint(-1, 30, "In Game Timer", tr, tg, tb, true);
            if (game.showingametimer)
            {
                graphics.Print(-1, 65, "In Game Timer is ON", tr, tg, tb, true);
            }
            else
            {
                graphics.Print(-1, 65, "In Game Timer is OFF", tr / 2, tg / 2, tb / 2, true);
            }
            break;
        }
        break;
    case Menu::setglitchrunner:
        graphics.bigprint(-1, 30, "Glitchrunner Mode", tr, tg, tb, true);
        graphics.Print(-1, 65, "Select a new glitchrunner", tr, tg, tb, true);
        graphics.Print(-1, 75, "version below.", tr, tg, tb, true);
        drawglitchrunnertext();
        break;
    case Menu::advancedoptions:
        switch (game.currentmenuoption)
        {
        case 0:
            graphics.bigprint( -1, 30, "Unfocus Pause", tr, tg, tb, true);
            graphics.Print( -1, 65, "Toggle if the game will pause", tr, tg, tb, true);
            graphics.Print( -1, 75, "when the window is unfocused.", tr, tg, tb, true);
            if (game.disablepause)
            {
                graphics.Print(-1, 95, "Unfocus pause is OFF", tr/2, tg/2, tb/2, true);
            }
            else
            {
                graphics.Print(-1, 95, "Unfocus pause is ON", tr, tg, tb, true);
            }
            break;
        case 1:
            graphics.bigprint(-1, 30, "Unfocus Audio", tr, tg, tb, true);
            graphics.Print(-1, 65, "Toggle if the audio will pause", tr, tg, tb, true);
            graphics.Print(-1, 75, "when the window is unfocused.", tr, tg, tb, true);
            if (game.disableaudiopause)
            {
                graphics.Print(-1, 95, "Unfocus audio pause is OFF", tr/2, tg/2, tb/2, true);
            }
            else
            {
                graphics.Print(-1, 95, "Unfocus audio pause is ON", tr, tg, tb, true);
            }
            break;
        case 2:
            graphics.bigprint(-1, 30, "Room Name BG", tr, tg, tb, true);
            graphics.Print( -1, 65, "Lets you see through what is behind", tr, tg, tb, true);
            graphics.Print( -1, 75, "the name at the bottom of the screen.", tr, tg, tb, true);
            if (graphics.translucentroomname)
                graphics.Print(-1, 95, "Room name background is TRANSLUCENT", tr/2, tg/2, tb/2, true);
            else
                graphics.Print(-1, 95, "Room name background is OPAQUE", tr, tg, tb, true);
            break;
        }
        break;
    case Menu::accessibility:
    {
#ifdef MAKEANDPLAY
 #define OFFSET 0
#else
 #define OFFSET 1
#endif

        switch (game.currentmenuoption)
        {
#if !defined(MAKEANDPLAY)
        case 0:
            graphics.bigprint(-1, 30, "Unlock Play Modes", tr, tg, tb, true);
            graphics.Print(-1, 65, "Unlock parts of the game normally", tr, tg, tb, true);
            graphics.Print(-1, 75, "unlocked as you progress", tr, tg, tb, true);
            break;
#endif
        case OFFSET+0:
            graphics.bigprint(-1, 40, "Invincibility", tr, tg, tb, true);
            graphics.Print(-1, 75, "Explore the game freely without", tr, tg, tb, true);
            graphics.Print(-1, 85, "dying. (Can cause glitches.)", tr, tg, tb, true);
            if (map.invincibility)
            {
                graphics.Print(-1, 105, "Invincibility is ON.", tr, tg, tb, true);
            }
            else
            {
                graphics.Print(-1, 105, "Invincibility is OFF.", tr / 2, tg / 2, tb / 2, true);
            }
            break;
        case OFFSET+1:
            graphics.bigprint(-1, 40, "Slowdown", tr, tg, tb, true);
            graphics.Print(-1, 75, "Reduce the game speed.", tr, tg, tb, true);
            drawslowdowntext();
            break;
        case OFFSET+2:
            graphics.bigprint(-1, 40, "Backgrounds", tr, tg, tb, true);
            if (!game.colourblindmode)
            {
                graphics.Print(-1, 75, "Backgrounds are ON.", tr, tg, tb, true);
            }
            else
            {
                graphics.Print(-1, 75, "Backgrounds are OFF.", tr / 2, tg / 2, tb / 2, true);
            }
            break;
        case OFFSET+3:
            graphics.bigprint(-1, 40, "Screen Effects", tr, tg, tb, true);
            graphics.Print(-1, 75, "Disables screen shakes and flashes.", tr, tg, tb, true);
            if (!game.noflashingmode)
            {
                graphics.Print(-1, 85, "Screen Effects are ON.", tr, tg, tb, true);
            }
            else
            {
                graphics.Print(-1, 85, "Screen Effects are OFF.", tr / 2, tg / 2, tb / 2, true);
            }
            break;
        case OFFSET+4:
        {
            const char* text;

            graphics.bigprint(-1, 40, "Text Outline", tr, tg, tb, true);
            graphics.Print(-1, 75, "Disables outline on game text.", tr, tg, tb, true);

            FillRect(graphics.backBuffer, 0, 84, 320, 10, tr, tg, tb);

            if (!graphics.notextoutline)
            {
                text = "Text outlines are ON.";
            }
            else
            {
                text = "Text outlines are OFF.";
            }

            graphics.bprint(-1, 85, text, 255, 255, 255, true);
            break;
        }

        }
        break;

#undef OFFSET
    }
    case Menu::playint1:
    case Menu::playint2:
        graphics.Print( -1, 65, "Who do you want to play", tr, tg, tb, true);
        graphics.Print( -1, 75, "the level with?", tr, tg, tb, true);
        break;
    case Menu::playmodes:
        switch (game.currentmenuoption)
        {
        case 0:
            graphics.bigprint( -1, 30, "Time Trials", tr, tg, tb, true);
            graphics.Print( -1, 65, "Replay any level in the game in", tr, tg, tb, true);
            graphics.Print( -1, 75, "a competitive time trial mode.", tr, tg, tb, true);

            if (game.nocompetitive())
            {
                graphics.Print( -1, 105, "Time Trials are not available", tr, tg, tb, true);
                graphics.Print( -1, 115, "with slowdown or invincibility.", tr, tg, tb, true);
            }
            break;
        case 1:
            graphics.bigprint( -1, 30, "Intermissions", tr, tg, tb, true);
            graphics.Print( -1, 65, "Replay the intermission levels.", tr, tg, tb, true);

            if (!game.unlock[15] && !game.unlock[16])
            {
                graphics.Print( -1, 95, "TO UNLOCK: Complete the", tr, tg, tb, true);
                graphics.Print( -1, 105, "intermission levels in-game.", tr, tg, tb, true);
            }
            break;
        case 2:
            graphics.bigprint( -1, 30, "No Death Mode", tr, tg, tb, true);
            graphics.Print( -1, 65, "Play the entire game", tr, tg, tb, true);
            graphics.Print( -1, 75, "without dying once.", tr, tg, tb, true);

            if (game.nocompetitive())
            {
                graphics.Print( -1, 105, "No Death Mode is not available", tr, tg, tb, true);
                graphics.Print( -1, 115, "with slowdown or invincibility.", tr, tg, tb, true);
            }
            else if (!game.unlock[17])
            {
                graphics.Print( -1, 105, "TO UNLOCK: Achieve an S-rank or", tr, tg, tb, true);
                graphics.Print( -1, 115, "above in at least 4 time trials.", tr, tg, tb, true);
            }
            break;
        case 3:
            // WARNING: Partially duplicated in Menu::options
            graphics.bigprint( -1, 30, "Flip Mode", tr, tg, tb, true);
            graphics.Print( -1, 65, "Flip the entire game vertically.", tr, tg, tb, true);
            graphics.Print( -1, 75, "Compatible with other game modes.", tr, tg, tb, true);

            if (game.unlock[18])
            {
                if (graphics.setflipmode)
                {
                    graphics.Print( -1, 105, "Currently ENABLED!", tr, tg, tb, true);
                }
                else
                {
                    graphics.Print( -1, 105, "Currently Disabled.", tr/2, tg/2, tb/2, true);
                }
            }
            else
            {
                graphics.Print( -1, 105, "TO UNLOCK: Complete the game.", tr, tg, tb, true);
            }
            break;
        }
        break;
    case Menu::youwannaquit:
        graphics.Print( -1, 75, "Are you sure you want to quit?", tr, tg, tb, true);
        break;
    case Menu::continuemenu:
        switch (game.currentmenuoption)
        {
        case 0:
        {
            //Show teleporter save info
            graphics.drawpixeltextbox(17, 65-20, 286, 90, 36,12, 65, 185, 207,0,4);

            graphics.bigprint(-1, 20, "Tele Save", tr, tg, tb, true);
            graphics.Print(0, 80-20, game.tele_currentarea, 25, 255 - (help.glow / 2), 255 - (help.glow / 2), true);
            for (int i = 0; i < 6; i++)
            {
                graphics.drawcrewman(169-(3*42)+(i*42), 95-20, i, game.tele_crewstats[i], true);
            }
            graphics.Print(59, 132-20, game.tele_gametime, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
            const std::string& trinketcount = help.number(game.tele_trinkets);
            graphics.Print(262-graphics.len(trinketcount), 132-20, trinketcount, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));

            graphics.drawsprite(34, 126-20, 50, graphics.col_clock);
            graphics.drawsprite(270, 126-20, 22, graphics.col_trinket);
            break;
        }
        case 1:
        {
            //Show quick save info
            graphics.drawpixeltextbox(17, 65-20, 286, 90, 36,12, 65, 185, 207,0,4);

            graphics.bigprint(-1, 20, "Quick Save", tr, tg, tb, true);
            graphics.Print(0, 80-20, game.quick_currentarea, 25, 255 - (help.glow / 2), 255 - (help.glow / 2), true);
            for (int i = 0; i < 6; i++)
            {
                graphics.drawcrewman(169-(3*42)+(i*42), 95-20, i, game.quick_crewstats[i], true);
            }
            graphics.Print(59, 132-20, game.quick_gametime, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
            const std::string& trinketcount = help.number(game.quick_trinkets);
            graphics.Print(262-graphics.len(trinketcount), 132-20, trinketcount, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));

            graphics.drawsprite(34, 126-20, 50, graphics.col_clock);
            graphics.drawsprite(270, 126-20, 22, graphics.col_trinket);
            break;
        }
        }
        break;
    case Menu::gameover:
    case Menu::gameover2:
    {
        graphics.bigprint( -1, 25, "GAME OVER", tr, tg, tb, true, 3);

        for (size_t i = 0; i < SDL_arraysize(game.ndmresultcrewstats); i++)
        {
            graphics.drawcrewman(169-(3*42)+(i*42), 68, i, game.ndmresultcrewstats[i], true);
        }
        std::string tempstring;
        tempstring = "You rescued " + help.number(game.ndmresultcrewrescued) + (game.ndmresultcrewrescued == 1 ? " crewmate" : " crewmates");
        graphics.Print(0, 100, tempstring, tr, tg, tb, true);

        tempstring = "and found " + help.number(game.ndmresulttrinkets) + (game.ndmresulttrinkets == 1 ? " trinket." : " trinkets.");
        graphics.Print(0, 110, tempstring, tr, tg, tb, true);

        tempstring = "You managed to reach:";
        graphics.Print(0, 145, tempstring, tr, tg, tb, true);
        graphics.Print(0, 155, game.ndmresulthardestroom, tr, tg, tb, true);

        switch (game.ndmresultcrewrescued)
        {
        case 1:
            tempstring = "Keep trying! You'll get there!";
            break;
        case 2:
            tempstring = "Nice one!";
            break;
        case 3:
            tempstring = "Wow! Congratulations!";
            break;
        case 4:
            tempstring = "Incredible!";
            break;
        case 5:
            tempstring = "Unbelievable! Well done!";
            break;
        case 6:
            tempstring = "Er, how did you do that?";
            break;
        }

        graphics.Print(0, 190, tempstring, tr, tg, tb, true);
        break;
    }
    case Menu::nodeathmodecomplete:
    case Menu::nodeathmodecomplete2:
    {
        graphics.bigprint( -1, 8, "WOW", tr, tg, tb, true, 4);

        for (size_t i = 0; i < SDL_arraysize(game.ndmresultcrewstats); i++)
        {
            graphics.drawcrewman(169-(3*42)+(i*42), 68, i, game.ndmresultcrewstats[i], true);
        }
        std::string tempstring = "You rescued all the crewmates!";
        graphics.Print(0, 100, tempstring, tr, tg, tb, true);

        tempstring = "And you found " + help.number(game.ndmresulttrinkets) + " trinkets.";
        graphics.Print(0, 110, tempstring, tr, tg, tb, true);

        graphics.Print(0, 160, "A new trophy has been awarded and", tr, tg, tb, true);
        graphics.Print(0, 170, "placed in the secret lab to", tr, tg, tb, true);
        graphics.Print(0, 180, "acknowledge your achievement!", tr, tg, tb, true);
        break;
    }
    case Menu::timetrialcomplete:
    case Menu::timetrialcomplete2:
    case Menu::timetrialcomplete3:
    {
        graphics.bigprint( -1, 20, "Results", tr, tg, tb, true, 3);

        std::string tempstring = game.resulttimestring() + " / " + game.timetstring(game.timetrialresultpar) + ".99";

        graphics.drawspritesetcol(30, 80-15, 50, 22);
        graphics.Print(65, 80-15, "TIME TAKEN:", 255, 255, 255);
        graphics.Print(65, 90-15, tempstring, tr, tg, tb);
        if (game.timetrialresulttime <= game.timetrialresultpar)
        {
            graphics.Print(220, 85-15, "+1 Rank!", 255, 255, 255);
        }

        tempstring = help.String(game.timetrialresultdeaths);
        graphics.drawspritesetcol(30-4, 80+20-4, 12, 22);
        graphics.Print(65, 80+20, "NUMBER OF DEATHS:", 255, 255, 255);
        graphics.Print(65, 90+20, tempstring, tr, tg, tb);
        if (game.timetrialresultdeaths == 0)
        {
            graphics.Print(220, 85+20, "+1 Rank!", 255, 255, 255);
        }

        tempstring = help.String(game.timetrialresulttrinkets) + " of " + help.String(game.timetrialresultshinytarget);
        graphics.drawspritesetcol(30, 80+55, 22, 22);
        graphics.Print(65, 80+55, "SHINY TRINKETS:", 255, 255, 255);
        graphics.Print(65, 90+55, tempstring, tr, tg, tb);
        if (game.timetrialresulttrinkets >= game.timetrialresultshinytarget)
        {
            graphics.Print(220, 85+55, "+1 Rank!", 255, 255, 255);
        }

        if (game.currentmenuname == Menu::timetrialcomplete2 || game.currentmenuname == Menu::timetrialcomplete3)
        {
            graphics.bigprint( 100, 175, "Rank:", tr, tg, tb, false, 2);
        }

        if (game.currentmenuname == Menu::timetrialcomplete3)
        {
            switch(game.timetrialrank)
            {
            case 0:
                graphics.bigprint( 195, 165, "B", 255, 255, 255, false, 4);
                break;
            case 1:
                graphics.bigprint( 195, 165, "A", 255, 255, 255, false, 4);
                break;
            case 2:
                graphics.bigprint( 195, 165, "S", 255, 255, 255, false, 4);
                break;
            case 3:
                graphics.bigprint( 195, 165, "V", 255, 255, 255, false, 4);
                break;
            }
        }
        break;
    }
    case Menu::unlockmenutrials:
        graphics.bigprint( -1, 30, "Unlock Time Trials", tr, tg, tb, true);
        graphics.Print( -1, 65, "You can unlock each time", tr, tg, tb, true);
        graphics.Print( -1, 75, "trial separately.", tr, tg, tb, true);
        break;
    case Menu::timetrials:
        switch (game.currentmenuoption)
        {
        case 0:
            if(game.unlock[9])
            {
                graphics.bigprint( -1, 30, "Space Station 1", tr, tg, tb, true);
                if (game.besttimes[0] == -1)
                {
                    graphics.Print( -1, 75, "Not yet attempted", tr, tg, tb, true);
                }
                else
                {
                    graphics.Print( 16, 65, "BEST TIME  ", tr, tg, tb);
                    graphics.Print( 16, 75, "BEST SHINY ", tr, tg, tb);
                    graphics.Print( 16, 85, "BEST LIVES ", tr, tg, tb);
                    graphics.Print( 110, 65, game.timetstring(game.besttimes[0]), tr, tg, tb);
                    graphics.Print( 110, 75, help.String(game.besttrinkets[0])+"/2", tr, tg, tb);
                    graphics.Print( 110, 85,help.String(game.bestlives[0]), tr, tg, tb);


                    graphics.Print( 170, 65, "PAR TIME    1:15", tr, tg, tb);
                    graphics.Print( 170, 85, "Best Rank", tr, tg, tb);
                    switch(game.bestrank[0])
                    {
                    case 0:
                        graphics.bigprint( 275, 82, "B", 225, 225, 225);
                        break;
                    case 1:
                        graphics.bigprint( 275, 82, "A", 225, 225, 225);
                        break;
                    case 2:
                        graphics.bigprint( 275, 82, "S", 225, 225, 225);
                        break;
                    case 3:
                        graphics.bigprint( 275, 82, "V", 225, 225, 225);
                        break;
                    }
                }

            }
            else
            {
                graphics.bigprint( -1, 30, "???", tr, tg, tb, true);
                graphics.Print( -1, 60, "TO UNLOCK:", tr, tg, tb, true);
                graphics.Print( -1, 75, "Rescue Violet", tr, tg, tb, true);
                graphics.Print( -1, 85, "Find three trinkets", tr, tg, tb, true);
            }
            break;
        case 1:
            if(game.unlock[10])
            {
                graphics.bigprint( -1, 30, "The Laboratory", tr, tg, tb, true);
                if (game.besttimes[1] == -1)
                {
                    graphics.Print( -1, 75, "Not yet attempted", tr, tg, tb, true);
                }
                else
                {
                    graphics.Print( 16, 65, "BEST TIME  ", tr, tg, tb);
                    graphics.Print( 16, 75, "BEST SHINY ", tr, tg, tb);
                    graphics.Print( 16, 85, "BEST LIVES ", tr, tg, tb);
                    graphics.Print( 110, 65, game.timetstring(game.besttimes[1]), tr, tg, tb);
                    graphics.Print( 110, 75, help.String(game.besttrinkets[1])+"/4", tr, tg, tb);
                    graphics.Print( 110, 85, help.String(game.bestlives[1]), tr, tg, tb);


                    graphics.Print( 170, 65, "PAR TIME    2:45", tr, tg, tb);
                    graphics.Print( 170, 85, "Best Rank", tr, tg, tb);
                    switch(game.bestrank[1])
                    {
                    case 0:
                        graphics.bigprint( 275, 82, "B", 225, 225, 225);
                        break;
                    case 1:
                        graphics.bigprint( 275, 82, "A", 225, 225, 225);
                        break;
                    case 2:
                        graphics.bigprint( 275, 82, "S", 225, 225, 225);
                        break;
                    case 3:
                        graphics.bigprint( 275, 82, "V", 225, 225, 225);
                        break;
                    }
                }

            }
            else
            {
                graphics.bigprint( -1, 30, "???", tr, tg, tb, true);
                graphics.Print( -1, 60, "TO UNLOCK:", tr, tg, tb, true);
                graphics.Print( -1, 75, "Rescue Victoria", tr, tg, tb, true);
                graphics.Print( -1, 85, "Find six trinkets", tr, tg, tb, true);
            }
            break;
        case 2:
            if(game.unlock[11])
            {
                graphics.bigprint( -1, 30, "The Tower", tr, tg, tb, true);
                if (game.besttimes[2] == -1)
                {
                    graphics.Print( -1, 75, "Not yet attempted", tr, tg, tb, true);
                }
                else
                {
                    graphics.Print( 16, 65, "BEST TIME  ", tr, tg, tb);
                    graphics.Print( 16, 75, "BEST SHINY ", tr, tg, tb);
                    graphics.Print( 16, 85, "BEST LIVES ", tr, tg, tb);
                    graphics.Print( 110, 65, game.timetstring(game.besttimes[2]), tr, tg, tb);
                    graphics.Print( 110, 75, help.String(game.besttrinkets[2])+"/2", tr, tg, tb);
                    graphics.Print( 110, 85, help.String(game.bestlives[2]), tr, tg, tb);


                    graphics.Print( 170, 65, "PAR TIME    1:45", tr, tg, tb);
                    graphics.Print( 170, 85, "Best Rank", tr, tg, tb);
                    switch(game.bestrank[2])
                    {
                    case 0:
                        graphics.bigprint( 275, 82, "B", 225, 225, 225);
                        break;
                    case 1:
                        graphics.bigprint( 275, 82, "A", 225, 225, 225);
                        break;
                    case 2:
                        graphics.bigprint( 275, 82, "S", 225, 225, 225);
                        break;
                    case 3:
                        graphics.bigprint( 275, 82, "V", 225, 225, 225);
                        break;
                    }
                }

            }
            else
            {
                graphics.bigprint( -1, 30, "???", tr, tg, tb, true);
                graphics.Print( -1, 60, "TO UNLOCK:", tr, tg, tb, true);
                graphics.Print( -1, 75, "Rescue Vermilion", tr, tg, tb, true);
                graphics.Print( -1, 85, "Find nine trinkets", tr, tg, tb, true);
            }
            break;
        case 3:
            if(game.unlock[12])
            {
                graphics.bigprint( -1, 30, "Space Station 2", tr, tg, tb, true);
                if (game.besttimes[3] == -1)
                {
                    graphics.Print( -1, 75, "Not yet attempted", tr, tg, tb, true);
                }
                else
                {
                    graphics.Print( 16, 65, "BEST TIME  ", tr, tg, tb);
                    graphics.Print( 16, 75, "BEST SHINY ", tr, tg, tb);
                    graphics.Print( 16, 85, "BEST LIVES ", tr, tg, tb);
                    graphics.Print( 110, 65, game.timetstring(game.besttimes[3]), tr, tg, tb);
                    graphics.Print( 110, 75, help.String(game.besttrinkets[3])+"/5", tr, tg, tb);
                    graphics.Print( 110, 85, help.String(game.bestlives[3]), tr, tg, tb);


                    graphics.Print( 170, 65, "PAR TIME    3:20", tr, tg, tb);
                    graphics.Print( 170, 85, "Best Rank", tr, tg, tb);
                    switch(game.bestrank[3])
                    {
                    case 0:
                        graphics.bigprint( 275, 82, "B", 225, 225, 225);
                        break;
                    case 1:
                        graphics.bigprint( 275, 82, "A", 225, 225, 225);
                        break;
                    case 2:
                        graphics.bigprint( 275, 82, "S", 225, 225, 225);
                        break;
                    case 3:
                        graphics.bigprint( 275, 82, "V", 225, 225, 225);
                        break;
                    }
                }

            }
            else
            {
                graphics.bigprint( -1, 30, "???", tr, tg, tb, true);
                graphics.Print( -1, 60, "TO UNLOCK:", tr, tg, tb, true);
                graphics.Print( -1, 75, "Rescue Vitellary", tr, tg, tb, true);
                graphics.Print( -1, 85, "Find twelve trinkets", tr, tg, tb, true);
            }
            break;
        case 4:
            if(game.unlock[13])
            {
                graphics.bigprint( -1, 30, "The Warp Zone", tr, tg, tb, true);
                if (game.besttimes[4] == -1)
                {
                    graphics.Print( -1, 75, "Not yet attempted", tr, tg, tb, true);
                }
                else
                {
                    graphics.Print( 16, 65, "BEST TIME  ", tr, tg, tb);
                    graphics.Print( 16, 75, "BEST SHINY ", tr, tg, tb);
                    graphics.Print( 16, 85, "BEST LIVES ", tr, tg, tb);
                    graphics.Print( 110, 65, game.timetstring(game.besttimes[4]), tr, tg, tb);
                    graphics.Print( 110, 75, help.String(game.besttrinkets[4])+"/1", tr, tg, tb);
                    graphics.Print( 110, 85, help.String(game.bestlives[4]), tr, tg, tb);


                    graphics.Print( 170, 65, "PAR TIME    2:00", tr, tg, tb);
                    graphics.Print( 170, 85, "Best Rank", tr, tg, tb);
                    switch(game.bestrank[4])
                    {
                    case 0:
                        graphics.bigprint( 275, 82, "B", 225, 225, 225);
                        break;
                    case 1:
                        graphics.bigprint( 275, 82, "A", 225, 225, 225);
                        break;
                    case 2:
                        graphics.bigprint( 275, 82, "S", 225, 225, 225);
                        break;
                    case 3:
                        graphics.bigprint( 275, 82, "V", 225, 225, 225);
                        break;
                    }
                }

            }
            else
            {
                graphics.bigprint( -1, 30, "???", tr, tg, tb, true);
                graphics.Print( -1, 60, "TO UNLOCK:", tr, tg, tb, true);
                graphics.Print( -1, 75, "Rescue Verdigris", tr, tg, tb, true);
                graphics.Print( -1, 85, "Find fifteen trinkets", tr, tg, tb, true);
            }
            break;
        case 5:
            if(game.unlock[14])
            {
                graphics.bigprint( -1, 30, "The Final Level", tr, tg, tb, true);
                if (game.besttimes[5] == -1)
                {
                    graphics.Print( -1, 75, "Not yet attempted", tr, tg, tb, true);
                }
                else
                {
                    graphics.Print( 16, 65, "BEST TIME  ", tr, tg, tb);
                    graphics.Print( 16, 75, "BEST SHINY ", tr, tg, tb);
                    graphics.Print( 16, 85, "BEST LIVES ", tr, tg, tb);
                    graphics.Print( 110, 65, game.timetstring(game.besttimes[5]), tr, tg, tb);
                    graphics.Print( 110, 75, help.String(game.besttrinkets[5])+"/1", tr, tg, tb);
                    graphics.Print( 110, 85, help.String(game.bestlives[5]), tr, tg, tb);


                    graphics.Print( 170, 65, "PAR TIME    2:15", tr, tg, tb);
                    graphics.Print( 170, 85, "Best Rank", tr, tg, tb);
                    switch(game.bestrank[5])
                    {
                    case 0:
                        graphics.bigprint( 275, 82, "B", 225, 225, 225);
                        break;
                    case 1:
                        graphics.bigprint( 275, 82, "A", 225, 225, 225);
                        break;
                    case 2:
                        graphics.bigprint( 275, 82, "S", 225, 225, 225);
                        break;
                    case 3:
                        graphics.bigprint( 275, 82, "V", 225, 225, 225);
                        break;
                    }
                }

            }
            else
            {
                graphics.bigprint( -1, 30, "???", tr, tg, tb, true);
                graphics.Print( -1, 60, "TO UNLOCK:", tr, tg, tb, true);
                graphics.Print( -1, 75, "Complete the game", tr, tg, tb, true);
                graphics.Print( -1, 85, "Find eighteen trinkets", tr, tg, tb, true);
            }
            break;
        }
        break;
    case Menu::gamecompletecontinue:
        graphics.bigprint( -1, 25, "Congratulations!", tr, tg, tb, true, 2);

        graphics.Print( -1, 45, "Your save files have been updated.", tr, tg, tb, true);

        graphics.Print( -1, 110, "If you want to keep exploring", tr, tg, tb, true);
        graphics.Print( -1, 120, "the game, select CONTINUE", tr, tg, tb, true);
        graphics.Print( -1, 130, "from the play menu.", tr, tg, tb, true);
        break;
    case Menu::unlockmenu:
        graphics.bigprint( -1, 25, "Unlock Play Modes", tr, tg, tb, true, 2);

        graphics.Print( -1, 55, "From here, you may unlock parts", tr, tg, tb, true);
        graphics.Print( -1, 65, "of the game that are normally", tr, tg, tb, true);
        graphics.Print( -1, 75, "unlocked as you play.", tr, tg, tb, true);
        break;
    case Menu::unlocktimetrial:
        graphics.bigprint( -1, 45, "Congratulations!", tr, tg, tb, true, 2);

        graphics.Print( -1, 125, "You have unlocked", tr, tg, tb, true);
        graphics.Print( -1, 135, "a new Time Trial.", tr, tg, tb, true);
        break;
    case Menu::unlocktimetrials:
        graphics.bigprint( -1, 45, "Congratulations!", tr, tg, tb, true, 2);

        graphics.Print( -1, 125, "You have unlocked some", tr, tg, tb, true);
        graphics.Print( -1, 135, "new Time Trials.", tr, tg, tb, true);
        break;
    case Menu::unlocknodeathmode:
        graphics.bigprint( -1, 45, "Congratulations!", tr, tg, tb, true, 2);

        graphics.Print( -1, 125, "You have unlocked", tr, tg, tb, true);
        graphics.Print( -1, 135, "No Death Mode.", tr, tg, tb, true);
        break;
    case Menu::unlockflipmode:
        graphics.bigprint( -1, 45, "Congratulations!", tr, tg, tb, true, 2);

        graphics.Print( -1, 125, "You have unlocked", tr, tg, tb, true);
        graphics.Print( -1, 135, "Flip Mode.", tr, tg, tb, true);
        break;
    case Menu::unlockintermission:
        graphics.bigprint( -1, 45, "Congratulations!", tr, tg, tb, true, 2);

        graphics.Print( -1, 125, "You have unlocked", tr, tg, tb, true);
        graphics.Print( -1, 135, "the intermission levels.", tr, tg, tb, true);
        break;
    case Menu::playerworlds:
    {
        std::string tempstring = FILESYSTEM_getUserLevelDirectory();
        if(tempstring.length()>80){
            graphics.Print( -1, 160, "To install new player levels, copy", tr, tg, tb, true);
            graphics.Print( -1, 170, "the .vvvvvv files to this folder:", tr, tg, tb, true);
            graphics.Print( 320-((tempstring.length()-80)*8), 190, tempstring.substr(0,tempstring.length()-80), tr, tg, tb);
            graphics.Print( 0, 200, tempstring.substr(tempstring.length()-80,40), tr, tg, tb);
            graphics.Print( 0, 210, tempstring.substr(tempstring.length()-40,40), tr, tg, tb);
        }else if(tempstring.length()>40){
            graphics.Print( -1, 170, "To install new player levels, copy", tr, tg, tb, true);
            graphics.Print( -1, 180, "the .vvvvvv files to this folder:", tr, tg, tb, true);
            graphics.Print( 320-((tempstring.length()-40)*8), 200, tempstring.substr(0,tempstring.length()-40), tr, tg, tb);
            graphics.Print( 0, 210, tempstring.substr(tempstring.length()-40,40), tr, tg, tb);
        }else{
            graphics.Print( -1, 180, "To install new player levels, copy", tr, tg, tb, true);
            graphics.Print( -1, 190, "the .vvvvvv files to this folder:", tr, tg, tb, true);
            graphics.Print( 320-(tempstring.length()*8), 210, tempstring, tr, tg, tb);
        }
        break;
    }
    case Menu::errorsavingsettings:
        graphics.Print( -1, 95, "ERROR: Could not save settings file!", tr, tg, tb, true);
        break;
    case Menu::errorloadinglevel:
    {
        const char* str;
        if (FILESYSTEM_levelDirHasError())
        {
            str = FILESYSTEM_getLevelDirError();
        }
        else
        {
            str = graphics.error;
        }
        graphics.bigprint(-1, 45, "ERROR", tr, tg, tb, true);
        graphics.PrintWrap(-1, 65, str, tr, tg, tb, true, 10, 304);
        break;
    }
    case Menu::warninglevellist:
        graphics.bigprint(-1, 45, "WARNING", tr, tg, tb, true);
        graphics.PrintWrap(-1, 65, FILESYSTEM_getLevelDirError(), tr, tg, tb, true, 10, 304);
        break;
    default:
        break;
    }
}

void titlerender(void)
{

    ClearSurface(graphics.backBuffer);

    if (!game.menustart)
    {
        tr = graphics.col_tr;
        tg = graphics.col_tg;
        tb = graphics.col_tb;

        int temp = 50;
        graphics.drawsprite((160 - 96) + 0 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 1 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 2 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 3 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 4 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 5 * 32, temp, 23, tr, tg, tb);
#if defined(MAKEANDPLAY)
        graphics.Print(-1,temp+35,"     MAKE AND PLAY EDITION",tr, tg, tb, true);
#endif

        graphics.Print(5, 175, "[ Press ACTION to Start ]", tr, tg, tb, true);
        graphics.Print(5, 195, "ACTION = Space, Z, or V", int(tr*0.5f), int(tg*0.5f), int(tb*0.5f), true);
    }
    else
    {
        if(!game.colourblindmode) graphics.drawtowerbackground(graphics.titlebg);

        tr = graphics.col_tr;
        tg = graphics.col_tg;
        tb = graphics.col_tb;

        menurender();

        tr = int(tr * .8f);
        tg = int(tg * .8f);
        tb = int(tb * .8f);
        if (tr < 0) tr = 0;
        if(tr>255) tr=255;
        if (tg < 0) tg = 0;
        if(tg>255) tg=255;
        if (tb < 0) tb = 0;
        if(tb>255) tb=255;
        graphics.drawmenu(tr, tg, tb, game.currentmenuname == Menu::levellist);
    }

    graphics.drawfade();

    graphics.renderwithscreeneffects();
}

void gamecompleterender(void)
{
    ClearSurface(graphics.backBuffer);

    if(!game.colourblindmode) graphics.drawtowerbackground(graphics.titlebg);

    tr = graphics.col_tr;
    tg = graphics.col_tg;
    tb = graphics.col_tb;


    //rendering starts... here!

    int position = graphics.lerp(game.oldcreditposition, game.creditposition);
    if (graphics.onscreen(220 + position))
    {
        int temp = 220 + position;
        graphics.drawsprite((160 - 96) + 0 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 1 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 2 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 3 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 4 * 32, temp, 23, tr, tg, tb);
        graphics.drawsprite((160 - 96) + 5 * 32, temp, 23, tr, tg, tb);
    }

    if (graphics.onscreen(290 + position)) graphics.bigprint( -1, 290 + position, "Starring", tr, tg, tb, true, 2);

    if (graphics.onscreen(320 + position))
    {
        graphics.drawcrewman(70, 320 + position, 0, true);
        graphics.Print(100, 330 + position, "Captain Viridian", tr, tg, tb);
    }
    if (graphics.onscreen(350 + position))
    {
        graphics.drawcrewman(70, 350 + position, 1, true);
        graphics.Print(100, 360 + position, "Doctor Violet", tr, tg, tb);
    }
    if (graphics.onscreen(380 + position))
    {
        graphics.drawcrewman(70, 380 + position, 2, true);
        graphics.Print(100, 390 + position, "Professor Vitellary", tr, tg, tb);
    }
    if (graphics.onscreen(410 + position))
    {
        graphics.drawcrewman(70, 410 + position, 3, true);
        graphics.Print(100, 420 + position, "Officer Vermilion", tr, tg, tb);
    }
    if (graphics.onscreen(440 + position))
    {
        graphics.drawcrewman(70, 440 + position, 4, true);
        graphics.Print(100, 450 + position, "Chief Verdigris", tr, tg, tb);
    }
    if (graphics.onscreen(470 + position))
    {
        graphics.drawcrewman(70, 470 + position, 5, true);
        graphics.Print(100, 480 + position, "Doctor Victoria", tr, tg, tb);
    }

    if (graphics.onscreen(520 + position)) graphics.bigprint( -1, 520 + position, "Credits", tr, tg, tb, true, 3);

    if (graphics.onscreen(560 + position))
    {
        graphics.Print(40, 560 + position, "Created by", tr, tg, tb);
        graphics.bigprint(60, 570 + position, "Terry Cavanagh", tr, tg, tb);
    }

    if (graphics.onscreen(600 + position))
    {
        graphics.Print(40, 600 + position, "With Music by", tr, tg, tb);
        graphics.bigprint(60, 610 + position, "Magnus P~lsson", tr, tg, tb);
    }

    if (graphics.onscreen(640 + position))
    {
        graphics.Print(40, 640 + position, "Rooms Named by", tr, tg, tb);
        graphics.bigprint(60, 650 + position, "Bennett Foddy", tr, tg, tb);
    }

    if (graphics.onscreen(680 + position))
    {
        graphics.Print(40, 680 + position, "C++ Port by", tr, tg, tb);
        graphics.bigprint(60, 690 + position, "Simon Roth", tr, tg, tb);
        graphics.bigprint(60, 710 + position, "Ethan Lee", tr, tg, tb);
    }


    if (graphics.onscreen(740 + position))
    {
        graphics.Print(40, 740 + position, "Beta Testing by", tr, tg, tb);
        graphics.bigprint(60, 750 + position, "Sam Kaplan", tr, tg, tb);
        graphics.bigprint(60, 770 + position, "Pauli Kohberger", tr, tg, tb);
    }

    if (graphics.onscreen(800 + position))
    {
        graphics.Print(40, 800 + position, "Ending Picture by", tr, tg, tb);
        graphics.bigprint(60, 810 + position, "Pauli Kohberger", tr, tg, tb);
    }

    if (graphics.onscreen(890 + position)) graphics.bigprint( -1, 870 + position, "Patrons", tr, tg, tb, true, 3);

    int creditOffset = 930;

    for (size_t i = 0; i < SDL_arraysize(Credits::superpatrons); i += 1)
    {
        if (graphics.onscreen(creditOffset + position))
        {
            graphics.Print(-1, creditOffset + position, Credits::superpatrons[i], tr, tg, tb, true);
        }
        creditOffset += 10;
    }

    creditOffset += 10;
    if (graphics.onscreen(creditOffset + position)) graphics.Print( -1, creditOffset + position, "and", tr, tg, tb, true);
    creditOffset += 20;

    for (size_t i = 0; i < SDL_arraysize(Credits::patrons); i += 1)
    {
        if (graphics.onscreen(creditOffset + position))
        {
            graphics.Print(-1, creditOffset + position, Credits::patrons[i], tr, tg, tb, true);
        }
        creditOffset += 10;
    }

    creditOffset += 20;
    if (graphics.onscreen(creditOffset + position)) graphics.bigprint(40, creditOffset + position, "GitHub Contributors", tr, tg, tb, true);
    creditOffset += 30;

    for (size_t i = 0; i < SDL_arraysize(Credits::githubfriends); i += 1)
    {
        if (graphics.onscreen(creditOffset + position))
        {
            graphics.Print(-1, creditOffset + position, Credits::githubfriends[i], tr, tg, tb, true);
        }
        creditOffset += 10;
    }

    creditOffset += 140;
    if (graphics.onscreen(creditOffset + position)) graphics.bigprint( -1, creditOffset + position, "Thanks for playing!", tr, tg, tb, true, 2);

    graphics.drawfade();

    graphics.render();
}

void gamecompleterender2(void)
{
    ClearSurface(graphics.backBuffer);

    graphics.drawimage(10, 0, 0);

    for (int j = 0; j < 30; j++)
    {
        for (int i = 0; i < 40; i++)
        {
            if (j == game.creditposy)
            {
                if (i > game.creditposx)
                {
                    FillRect(graphics.backBuffer, i * 8, j * 8, 8, 8, 0, 0, 0);
                }
            }

            if (j > game.creditposy)
            {
                FillRect(graphics.backBuffer, i * 8, j * 8, 8, 8, 0, 0, 0);
            }
        }
    }

    FillRect(graphics.backBuffer, graphics.lerp(game.oldcreditposx * 8, game.creditposx * 8) + 8, game.creditposy * 8, 8, 8, 0, 0, 0);

    graphics.drawfade();

    graphics.render();
}

static const char* interact_prompt(
    char* buffer,
    const size_t buffer_size,
    const char* raw
) {
    const char* string_fmt_loc = SDL_strstr(raw, "%s");
    const char* button;

    if (string_fmt_loc == NULL /* No "%s". */
    || string_fmt_loc != SDL_strchr(raw, '%') /* First "%" found is not "%s". */
    || SDL_strchr(&string_fmt_loc[1], '%') != NULL) /* Other "%" after "%s". */
    {
        return raw;
    }

    if (game.separate_interact)
    {
        button = "E";
    }
    else
    {
        button = "ENTER";
    }

    SDL_snprintf(buffer, buffer_size, raw, button);

    return buffer;
}

void gamerender(void)
{



    if(!game.blackout)
    {

        if (map.towermode)
        {
            if (!game.colourblindmode)
            {
                graphics.drawtowerbackground(graphics.towerbg);
            }
            else
            {
                ClearSurface(graphics.backBuffer);
            }
            graphics.drawtowermap();
        }
        else
        {
            if(!game.colourblindmode)
            {
                graphics.drawbackground(map.background);
            }
            else
            {
                ClearSurface(graphics.backBuffer);
            }
            if ((map.finalmode || map.custommode) && map.final_colormode)
            {
                graphics.drawfinalmap();
            }
            else
            {
                graphics.drawmap();
            }
        }


        graphics.drawentities();
        if (map.towermode)
        {
            graphics.drawtowerspikes();
        }
    }

    if (graphics.fademode==0 && !game.intimetrial && !game.isingamecompletescreen() && game.swngame != 1 && game.showingametimer)
    {
        graphics.bprint(6, 6, "TIME:",  255,255,255);
        graphics.bprint(46, 6, game.timestring(),  196, 196, 196);
    }

    if(map.extrarow==0 || (map.custommode && map.roomname!=""))
    {
        graphics.footerrect.y = 230;
        if (graphics.translucentroomname)
        {
            SDL_BlitSurface(graphics.footerbuffer, NULL, graphics.backBuffer, &graphics.footerrect);
        }
        else
        {
            FillRect(graphics.backBuffer, graphics.footerrect, 0);
        }

        if (map.finalmode)
        {
            graphics.bprint(5, 231, map.glitchname, 196, 196, 255 - help.glow, true);
        }else{
            graphics.bprint(5, 231, map.roomname, 196, 196, 255 - help.glow, true);
        }
    }

    if (map.roomtexton)
    {
        //Draw room text!
        for (size_t i = 0; i < map.roomtext.size(); i++)
        {
            graphics.Print(map.roomtext[i].x*8, (map.roomtext[i].y*8), map.roomtext[i].text, 196, 196, 255 - help.glow);
        }
    }

#if !defined(NO_CUSTOM_LEVELS)
     if(map.custommode && !map.custommodeforreal && !game.advancetext){
        //Return to level editor
        int alpha = graphics.lerp(ed.oldreturneditoralpha, ed.returneditoralpha);
        graphics.bprintalpha(5, 5, "[Press ENTER to return to editor]", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), alpha, false);
      }
#endif


    graphics.cutscenebars();
    graphics.drawfade();
    BlitSurfaceStandard(graphics.backBuffer, NULL, graphics.tempBuffer, NULL);

    graphics.drawgui();
    if (graphics.flipmode)
    {
        if (game.advancetext) graphics.bprint(5, 228, "- Press ACTION to advance text -", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
    }
    else
    {
        if (game.advancetext) graphics.bprint(5, 5, "- Press ACTION to advance text -", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
    }

    if (game.readytotele > 100 || game.oldreadytotele > 100)
    {
        /* Screen width 40 chars, 4 per char */
        char buffer[160 + 1];
        static const char raw[] = "- Press %s to Teleport - ";
        const char* final_string = interact_prompt(
            buffer,
            sizeof(buffer),
            raw
        );
        int alpha = graphics.lerp(game.oldreadytotele, game.readytotele);

        graphics.bprint(5, graphics.flipmode ? 20 : 210, final_string, alpha - 20 - (help.glow / 2), alpha - 20 - (help.glow / 2), alpha, true);
    }

    if (game.swnmode)
    {
        if (game.swngame == 0)
        {
            std::string tempstring = help.timestring(game.swntimer);
            graphics.bigbprint( -1, 20, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);
        }
        else if (game.swngame == 1)
        {
            if (game.swnmessage == 0)
            {
                std::string tempstring = help.timestring(game.swntimer);
                graphics.bprint( 10, 10, "Current Time", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);
                graphics.bigbprint( 25, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false, 2);
                tempstring = help.timestring(game.swnrecord);
                graphics.bprint( 240, 10, "Best Time", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);
                graphics.bigbrprint( 300, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false, 2);

                switch(game.swnbestrank)
                {
                case 0:
                    graphics.bprint( -1, 204, "Next Trophy at 5 seconds", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
                    break;
                case 1:
                    graphics.bprint( -1, 204, "Next Trophy at 10 seconds", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
                    break;
                case 2:
                    graphics.bprint( -1, 204, "Next Trophy at 15 seconds", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
                    break;
                case 3:
                    graphics.bprint( -1, 204, "Next Trophy at 20 seconds", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
                    break;
                case 4:
                    graphics.bprint( -1, 204, "Next Trophy at 30 seconds", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
                    break;
                case 5:
                    graphics.bprint( -1, 204, "Next Trophy at 1 minute", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
                    break;
                case 6:
                    graphics.bprint( -1, 204, "All Trophies collected!", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
                    break;
                }
            }
            else if (game.swnmessage == 1)
            {
                std::string tempstring = help.timestring(game.swntimer);
                graphics.bprint( 10, 10, "Current Time", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);
                graphics.bigbprint( 25, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false, 2);
                tempstring = help.timestring(game.swnrecord);
                if (int(game.deathseq / 5) % 2 == 1)
                {
                    graphics.bprint( 240, 10, "Best Time", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);
                    graphics.bigbrprint( 300, 24, tempstring, 128 - (help.glow), 220 - (help.glow), 128 - (help.glow / 2), false, 2);

                    graphics.bigbprint( -1, 200, "New Record!", 128 - (help.glow), 220 - (help.glow), 128 - (help.glow / 2), true, 2);
                }
            }
            else if (game.swnmessage >= 2)
            {
                game.swnmessage--;
                if (game.swnmessage == 2) game.swnmessage = 0;
                std::string tempstring = help.timestring(game.swntimer);
                graphics.bprint( 10, 10, "Current Time", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);
                graphics.bigbprint( 25, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false, 2);
                tempstring = help.timestring(game.swnrecord);
                graphics.bprint( 240, 10, "Best Time", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);
                graphics.bigbrprint( 300, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false, 2);

                if (int(game.swnmessage / 5) % 2 == 1)
                {
                    graphics.bigbprint( -1, 200, "New Trophy!", 220 - (help.glow), 128 - (help.glow), 128 - (help.glow / 2), true, 2);
                }
            }

            graphics.bprint( 20, 228, "[Press ENTER to stop]", 160 - (help.glow/2), 160 - (help.glow/2), 160 - (help.glow/2), true);
        }
        else if(game.swngame==2)
        {
            if (int(game.swndelay / 15) % 2 == 1 || game.swndelay >= 120)
            {
                int y1;
                int y2;
                if (graphics.flipmode)
                {
                    y1 = 30;
                    y2 = 10;
                }
                else
                {
                    y1 = 10;
                    y2 = 30;
                }
                graphics.bigbprint( -1, y1, "Survive for", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);
                graphics.bigbprint( -1, y2, "60 seconds!", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);
            }
        }
        else if(game.swngame==7)
        {
            if (game.swndelay >= 60)
            {
                graphics.bigbprint( -1, 20, "SUPER GRAVITRON", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);

                std::string tempstring = help.timestring(game.swnrecord);
                graphics.bprint( 240, 190, "Best Time", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
                graphics.bigbrprint( 300, 205, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);
            }
            else	if (int(game.swndelay / 10) % 2 == 1)
            {
                graphics.bigbprint( -1, 20, "SUPER GRAVITRON", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);
                graphics.bigbprint( -1, 200, "GO!", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 3);
            }
        }
    }

    if (game.intimetrial && graphics.fademode==0)
    {
        //Draw countdown!
        if (game.timetrialcountdown > 0)
        {
            if (game.timetrialcountdown < 30)
            {
                if (int(game.timetrialcountdown / 4) % 2 == 0) graphics.bigbprint( -1, 100, "Go!", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 4);
            }
            else if (game.timetrialcountdown < 60)
            {
                graphics.bigbprint( -1, 100, "1", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 4);
            }
            else if (game.timetrialcountdown < 90)
            {
                graphics.bigbprint( -1, 100, "2", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 4);
            }
            else if (game.timetrialcountdown < 120)
            {
                graphics.bigbprint( -1, 100, "3", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 4);
            }
        }
        else
        {
            //Draw OSD stuff
            graphics.bprint(6, 18, "TIME :",  255,255,255);
            graphics.bprint(6, 30, "DEATH:",  255, 255, 255);
            graphics.bprint(6, 42, "SHINY:",  255,255,255);

            if(game.timetrialparlost)
            {
                graphics.bprint(56, 18, game.timestring(),  196, 80, 80);
            }
            else
            {
                graphics.bprint(56, 18, game.timestring(),  196, 196, 196);
            }
            if(game.deathcounts>0)
            {
                graphics.bprint(56, 30,help.String(game.deathcounts),  196, 80, 80);
            }
            else
            {
                graphics.bprint(56, 30,help.String(game.deathcounts),  196, 196, 196);
            }
            if(game.trinkets()<game.timetrialshinytarget)
            {
                graphics.bprint(56, 42,help.String(game.trinkets()) + " of " +help.String(game.timetrialshinytarget),  196, 80, 80);
            }
            else
            {
                graphics.bprint(56, 42,help.String(game.trinkets()) + " of " +help.String(game.timetrialshinytarget),  196, 196, 196);
            }

            if(game.timetrialparlost)
            {
                graphics.bprint(195, 214, "PAR TIME:",  80, 80, 80);
                graphics.bprint(275, 214, game.partimestring(),  80, 80, 80);
            }
            else
            {
                graphics.bprint(195, 214, "PAR TIME:",  255, 255, 255);
                graphics.bprint(275, 214, game.partimestring(),  196, 196, 196);
            }
        }
    }

    float act_alpha = graphics.lerp(game.prev_act_fade, game.act_fade) / 10.0f;
    if(game.act_fade>5 || game.prev_act_fade>5)
    {
        /* Screen width 40 chars, 4 per char */
        char buffer[160 + 1];
        const char* final_string = interact_prompt(
            buffer,
            sizeof(buffer),
            game.activity_lastprompt.c_str()
        );

        graphics.drawtextbox(16, 4, 36, 3, game.activity_r*act_alpha, game.activity_g*act_alpha, game.activity_b*act_alpha);
        graphics.Print(5, 12, final_string, game.activity_r*act_alpha, game.activity_g*act_alpha, game.activity_b*act_alpha, true);
    }

    if (obj.trophytext > 0 || obj.oldtrophytext > 0)
    {
        graphics.drawtrophytext();
    }


    graphics.renderwithscreeneffects();
}

void maprender(void)
{
    ClearSurface(graphics.backBuffer);

    //draw screen alliteration
    //Roomname:
    if (map.hiddenname != "")
    {
        graphics.Print(5, 2, map.hiddenname, 196, 196, 255 - help.glow, true);
    }
    else
    {
      if (map.finalmode){
        graphics.Print(5, 2, map.glitchname, 196, 196, 255 - help.glow, true);
      }else{
        graphics.Print(5, 2, map.roomname, 196, 196, 255 - help.glow, true);
      }
    }

    //Background color
    FillRect(graphics.backBuffer,0, 12, 320, 240, 10, 24, 26 );



    //Menubar:
    graphics.drawtextbox( -10, 212, 43, 3, 65, 185, 207);

    // Draw the selected page name at the bottom
    // menupage 0 - 3 is the pause screen
    if (script.running && game.menupage == 3)
    {
        // While in a cutscene, you can only save
        graphics.Print(-1, 220, "[SAVE]", 196, 196, 255 - help.glow, true);
    }
    else if (game.menupage <= 3)
    {
        std::string tab1;
        if (game.insecretlab)
        {
            tab1 = "GRAV";
        }
        else if (obj.flags[67] && !map.custommode)
        {
            tab1 = "SHIP";
        }
        else
        {
            tab1 = "CREW";
        }
#define TAB(opt, text) graphics.map_tab(opt, text, game.menupage == opt)
        TAB(0, "MAP");
        TAB(1, tab1);
        TAB(2, "STATS");
        TAB(3, "SAVE");
#undef TAB
    }

    // Draw menu header
    switch (game.menupage)
    {
    case 30:
    case 31:
    case 32:
    case 33:
        graphics.Print(-1, 220, "[ PAUSE ]", 196, 196, 255 - help.glow, true);
    }

    // Draw menu options
    if (game.menupage >= 30 && game.menupage <= 33)
    {
#define OPTION(opt, text) graphics.map_option(opt, 4, text, game.menupage - 30 == opt)
        OPTION(0, "return to game");
        OPTION(1, "options");
        OPTION(2, "quit to menu");
#undef OPTION
    }

    // Draw the actual menu
    switch(game.menupage)
    {
    case 0:
        if (map.finalmode || (map.custommode&&!map.customshowmm))
        {
            //draw the map image
            graphics.drawpixeltextbox(35, 16, 250, 190, 32,24, 65, 185, 207,4,0);
            graphics.drawimage(1, 40, 21, false);
            for (int j = 0; j < 20; j++)
            {
                for (int i = 0; i < 20; i++)
                {
                    graphics.drawimage(2, 40 + (i * 12), 21 + (j * 9), false);
                }
            }
            graphics.Print(-1, 105, "NO SIGNAL", 245, 245, 245, true);
        }
#ifndef NO_CUSTOM_LEVELS
        else if(map.custommode)
        {
          //draw the map image
          graphics.drawcustompixeltextbox(35+map.custommmxoff, 16+map.custommmyoff, map.custommmxsize+10, map.custommmysize+10, (map.custommmxsize+10)/8, (map.custommmysize+10)/8, 65, 185, 207,4,0);
          if (graphics.minimap_mounted)
          {
            graphics.drawpartimage(1, 40+map.custommmxoff, 21+map.custommmyoff, map.custommmxsize, map.custommmysize);
          }
          else
          {
            graphics.drawpartimage(12, 40+map.custommmxoff, 21+map.custommmyoff, map.custommmxsize,map.custommmysize);
          }

          //Black out here
          if(map.customzoom==4){
            for (int j = 0; j < map.customheight; j++){
              for (int i = 0; i < map.customwidth; i++){
                if(!map.isexplored(i, j)){
                  //Draw the fog of war on the map
                  graphics.drawimage(2, map.custommmxoff+40 + (i * 48), map.custommmyoff+21 + (j * 36), false);
                  graphics.drawimage(2, map.custommmxoff+40 + 12 + (i * 48), map.custommmyoff+21 + (j * 36), false);
                  graphics.drawimage(2, map.custommmxoff+40 + (i * 48), map.custommmyoff+21 + 9 + (j * 36), false);
                  graphics.drawimage(2, map.custommmxoff+40 + 12 + (i * 48), map.custommmyoff+21 + 9+ (j * 36), false);

                  graphics.drawimage(2, map.custommmxoff+40 + (i * 48) + 24, map.custommmyoff+21 + (j * 36), false);
                  graphics.drawimage(2, map.custommmxoff+40 + 12 + (i * 48) + 24, map.custommmyoff+21 + (j * 36), false);
                  graphics.drawimage(2, map.custommmxoff+40 + (i * 48) + 24, map.custommmyoff+ 21 + 9 + (j * 36), false);
                  graphics.drawimage(2, map.custommmxoff+40 + 12 + (i * 48) + 24, map.custommmyoff+21 + 9+ (j * 36), false);

                  graphics.drawimage(2, map.custommmxoff+40 + (i * 48), map.custommmyoff+21 + (j * 36)+18, false);
                  graphics.drawimage(2, map.custommmxoff+40 + 12 + (i * 48), map.custommmyoff+21 + (j * 36)+18, false);
                  graphics.drawimage(2, map.custommmxoff+40 + (i * 48), map.custommmyoff+21 + 9 + (j * 36)+18, false);
                  graphics.drawimage(2, map.custommmxoff+40 + 12 + (i * 48), map.custommmyoff+21 + 9+ (j * 36)+18, false);

                  graphics.drawimage(2, map.custommmxoff+40 + (i * 48) + 24, map.custommmyoff+21 + (j * 36)+18, false);
                  graphics.drawimage(2, map.custommmxoff+40 + 12 + (i * 48) + 24, map.custommmyoff+21 + (j * 36)+18, false);
                  graphics.drawimage(2, map.custommmxoff+40 + (i * 48) + 24, map.custommmyoff+21 + 9 + (j * 36)+18, false);
                  graphics.drawimage(2, map.custommmxoff+40 + 12 + (i * 48) + 24, map.custommmyoff+21 + 9+ (j * 36)+18, false);
                }
              }
            }
          }else if(map.customzoom==2){
            for (int j = 0; j < map.customheight; j++){
              for (int i = 0; i < map.customwidth; i++){
                if(!map.isexplored(i, j)){
                  //Draw the fog of war on the map
                  graphics.drawimage(2, map.custommmxoff+40 + (i * 24), map.custommmyoff+21 + (j * 18), false);
                  graphics.drawimage(2, map.custommmxoff+40 + 12 + (i * 24), map.custommmyoff+21 + (j * 18), false);
                  graphics.drawimage(2, map.custommmxoff+40 + (i * 24), map.custommmyoff+21 + 9 + (j * 18), false);
                  graphics.drawimage(2, map.custommmxoff+40 + 12 + (i * 24), map.custommmyoff+21 + 9+ (j * 18), false);
                }
              }
            }
          }else{
            for (int j = 0; j < map.customheight; j++){
              for (int i = 0; i < map.customwidth; i++){
                if(!map.isexplored(i, j)){
                  //Draw the fog of war on the map
                  graphics.drawimage(2, map.custommmxoff+40 + (i * 12), map.custommmyoff+21 + (j * 9), false);
                }
              }
            }
          }

          //normal size maps
          if(map.customzoom==4){
            if(map.cursorstate==1){
              if (int(map.cursordelay / 4) % 2 == 0){
                graphics.drawrect(40 + ((game.roomx - 100) * 48) +map.custommmxoff, 21 + ((game.roomy - 100) * 36)+map.custommmyoff , 48 , 36 , 255,255,255);
                graphics.drawrect(40 + ((game.roomx - 100) * 48) + 2+map.custommmxoff, 21 + ((game.roomy - 100) * 36) + 2+map.custommmyoff, 48 - 4, 36 - 4, 255,255,255);
              }
            }else if (map.cursorstate == 2){
              if (int(map.cursordelay / 15) % 2 == 0){
                graphics.drawrect(40 + ((game.roomx - 100) * 48) + 2+map.custommmxoff, 21 + ((game.roomy - 100) * 36) + 2+map.custommmyoff, 48 - 4, 36 - 4, 16, 245 - (help.glow), 245 - (help.glow));
              }
            }
          }else if(map.customzoom==2){
            if(map.cursorstate==1){
              if (int(map.cursordelay / 4) % 2 == 0){
                graphics.drawrect(40 + ((game.roomx - 100) * 24)+map.custommmxoff , 21 + ((game.roomy - 100) * 18)+map.custommmyoff , 24 , 18 , 255,255,255);
                graphics.drawrect(40 + ((game.roomx - 100) * 24) + 2+map.custommmxoff, 21 + ((game.roomy - 100) * 18) + 2+map.custommmyoff, 24 - 4, 18 - 4, 255,255,255);
              }
            }else if (map.cursorstate == 2){
              if (int(map.cursordelay / 15) % 2 == 0){
                graphics.drawrect(40 + ((game.roomx - 100) * 24) + 2+map.custommmxoff, 21 + ((game.roomy - 100) * 18) + 2+map.custommmyoff, 24 - 4, 18 - 4, 16, 245 - (help.glow), 245 - (help.glow));
              }
            }
          }else{
            if(map.cursorstate==1){
              if (int(map.cursordelay / 4) % 2 == 0){
                graphics.drawrect(40 + ((game.roomx - 100) * 12)+map.custommmxoff , 21 + ((game.roomy - 100) * 9)+map.custommmyoff , 12 , 9 , 255,255,255);
                graphics.drawrect(40 + ((game.roomx - 100) * 12) + 2+map.custommmxoff, 21 + ((game.roomy - 100) * 9) + 2+map.custommmyoff, 12 - 4, 9 - 4, 255,255,255);
              }
            }else if (map.cursorstate == 2){
              if (int(map.cursordelay / 15) % 2 == 0){
                graphics.drawrect(40 + ((game.roomx - 100) * 12) + 2+map.custommmxoff, 21 + ((game.roomy - 100) * 9) + 2+map.custommmyoff, 12 - 4, 9 - 4, 16, 245 - (help.glow), 245 - (help.glow));
              }
            }
          }

          if(map.showtrinkets){
            for(size_t i=0; i<map.shinytrinkets.size(); i++){
              if(!obj.collect[i]){
                int temp = 1086;
                if(graphics.flipmode) temp += 3;
                if(map.customzoom==4){
                  graphics.drawtile(40 + (map.shinytrinkets[i].x * 48) + 20+map.custommmxoff, 21 + (map.shinytrinkets[i].y * 36) + 14+map.custommmyoff, temp);
                }else if(map.customzoom==2){
                  graphics.drawtile(40 + (map.shinytrinkets[i].x * 24) + 8+map.custommmxoff, 21 + (map.shinytrinkets[i].y * 18) + 5+map.custommmyoff, temp);
                }else{
                  graphics.drawtile(40 + 3 + (map.shinytrinkets[i].x * 12) + map.custommmxoff, 22 + (map.shinytrinkets[i].y * 9) + map.custommmyoff, temp);
                }
              }
            }
          }
        }
#endif /* NO_CUSTOM_LEVELS */
        else
        {
            //draw the map image
            graphics.drawpixeltextbox(35, 16, 250, 190, 32,24, 65, 185, 207,4,0);
            graphics.drawimage(1, 40, 21, false);

            //black out areas we can't see yet
            for (int j = 0; j < 20; j++)
            {
                for (int i = 0; i < 20; i++)
                {
                    if(!map.isexplored(i, j))
                    {
                        //Draw the fog of war on the map
                        graphics.drawimage(2, 40 + (i * 12), 21 + (j * 9), false);
                    }
                }
            }
            //draw the coordinates
            if (game.roomx == 109)
            {
                //tower!instead of room y, scale map.ypos
                if (map.cursorstate == 1)
                {
                    if (int(map.cursordelay / 4) % 2 == 0)
                    {
                        graphics.drawrect(40 + ((game.roomx - 100) * 12) , 21 , 12, 180, 255,255,255);
                        graphics.drawrect(40 + ((game.roomx - 100) * 12) + 2 , 21  + 2, 12 - 4, 180 - 4, 255,255,255);
                    }
                    if (map.cursordelay > 30) map.cursorstate = 2;
                }
                else if (map.cursorstate == 2)
                {
                    if (int(map.cursordelay / 15) % 2 == 0)
                    {
                        graphics.drawrect(40 + ((game.roomx - 100) * 12) + 2 , 21  + 2, 12 - 4, 180 - 4,16, 245 - (help.glow), 245 - (help.glow));
                    }
                }
            }
            else
            {
                if (map.cursorstate == 1)
                {
                    if (int(map.cursordelay / 4) % 2 == 0)
                    {
                        graphics.drawrect(40 + ((game.roomx - 100) * 12) , 21 + ((game.roomy - 100) * 9) , 12 , 9 , 255,255,255);
                        graphics.drawrect(40 + ((game.roomx - 100) * 12) + 2, 21 + ((game.roomy - 100) * 9) + 2, 12 - 4, 9 - 4, 255,255,255);
                    }
                }
                else if (map.cursorstate == 2)
                {
                    if (int(map.cursordelay / 15) % 2 == 0)
                    {
                        graphics.drawrect(40 + ((game.roomx - 100) * 12) + 2, 21 + ((game.roomy - 100) * 9) + 2, 12 - 4, 9 - 4, 16, 245 - (help.glow), 245 - (help.glow));
                    }
                }
            }

            //draw legend details
            for (size_t i = 0; i < map.teleporters.size(); i++)
            {
                if (map.showteleporters && map.isexplored(map.teleporters[i].x, map.teleporters[i].y))
                {
                    int temp = 1126 + (int) map.isexplored(map.teleporters[i].x, map.teleporters[i].y);
                    if (graphics.flipmode) temp += 3;
                    graphics.drawtile(40 + 3 + (map.teleporters[i].x * 12), 22 + (map.teleporters[i].y * 9), temp);
                }
                else if(map.showtargets && !map.isexplored(map.teleporters[i].x, map.teleporters[i].y))
                {
                    int temp = 1126 + (int) map.isexplored(map.teleporters[i].x, map.teleporters[i].y);
                    if (graphics.flipmode) temp += 3;
                    graphics.drawtile(40 + 3 + (map.teleporters[i].x * 12), 22 + (map.teleporters[i].y * 9), temp);
                }
            }

            if (map.showtrinkets)
            {
                for (size_t i = 0; i < map.shinytrinkets.size(); i++)
                {
                    if (!obj.collect[i])
                    {
                        int temp = 1086;
                        if (graphics.flipmode) temp += 3;
                        graphics.drawtile(40 + 3 + (map.shinytrinkets[i].x * 12), 22 + (map.shinytrinkets[i].y * 9),	temp);
                    }
                }
            }
        }
        break;
    case 1:
        if (game.insecretlab)
        {
            if (graphics.flipmode)
            {
                graphics.Print(0, 174, "SUPER GRAVITRON HIGHSCORE", 196, 196, 255 - help.glow, true);

                std::string tempstring = help.timestring(game.swnrecord);
                graphics.Print( 240, 124, "Best Time", 196, 196, 255 - help.glow, true);
                graphics.bigrprint( 300, 94, tempstring, 196, 196, 255 - help.glow, true, 2);

                switch(game.swnbestrank)
                {
                case 0:
                    graphics.Print( -1, 40, "Next Trophy at 5 seconds", 196, 196, 255 - help.glow, true);
                    break;
                case 1:
                    graphics.Print( -1, 40, "Next Trophy at 10 seconds", 196, 196, 255 - help.glow, true);
                    break;
                case 2:
                    graphics.Print( -1, 40, "Next Trophy at 15 seconds", 196, 196, 255 - help.glow, true);
                    break;
                case 3:
                    graphics.Print( -1, 40, "Next Trophy at 20 seconds", 196, 196, 255 - help.glow, true);
                    break;
                case 4:
                    graphics.Print( -1, 40, "Next Trophy at 30 seconds", 196, 196, 255 - help.glow, true);
                    break;
                case 5:
                    graphics.Print( -1, 40, "Next Trophy at 1 minute", 196, 196, 255 - help.glow, true);
                    break;
                case 6:
                    graphics.Print( -1, 40, "All Trophies collected!", 196, 196, 255 - help.glow, true);
                    break;
                }
            }
            else
            {
                graphics.Print(0, 40, "SUPER GRAVITRON HIGHSCORE", 196, 196, 255 - help.glow, true);

                std::string tempstring = help.timestring(game.swnrecord);
                graphics.Print( 240, 90, "Best Time", 196, 196, 255 - help.glow, true);
                graphics.bigrprint( 300, 104, tempstring, 196, 196, 255 - help.glow, true, 2);

                switch(game.swnbestrank)
                {
                case 0:
                    graphics.Print( -1, 174, "Next Trophy at 5 seconds", 196, 196, 255 - help.glow, true);
                    break;
                case 1:
                    graphics.Print( -1, 174, "Next Trophy at 10 seconds", 196, 196, 255 - help.glow, true);
                    break;
                case 2:
                    graphics.Print( -1, 174, "Next Trophy at 15 seconds", 196, 196, 255 - help.glow, true);
                    break;
                case 3:
                    graphics.Print( -1, 174, "Next Trophy at 20 seconds", 196, 196, 255 - help.glow, true);
                    break;
                case 4:
                    graphics.Print( -1, 174, "Next Trophy at 30 seconds", 196, 196, 255 - help.glow, true);
                    break;
                case 5:
                    graphics.Print( -1, 174, "Next Trophy at 1 minute", 196, 196, 255 - help.glow, true);
                    break;
                case 6:
                    graphics.Print( -1, 174, "All Trophies collected!", 196, 196, 255 - help.glow, true);
                    break;
                }
            }
        }
        else if (obj.flags[67] && !map.custommode)
        {
            graphics.Print(0, 105, "Press ACTION to warp to the ship.", 196, 196, 255 - help.glow, true);
        }
#if !defined(NO_CUSTOM_LEVELS)
        else if(map.custommode){
            LevelMetaData& meta = ed.ListOfMetaData[game.playcustomlevel];

            graphics.bigprint( -1, FLIP(45), meta.title, 196, 196, 255 - help.glow, true);
            graphics.Print( -1, FLIP(70), "by " + meta.creator, 196, 196, 255 - help.glow, true);
            graphics.Print( -1, FLIP(80), meta.website, 196, 196, 255 - help.glow, true);
            graphics.Print( -1, FLIP(100), meta.Desc1, 196, 196, 255 - help.glow, true);
            graphics.Print( -1, FLIP(110), meta.Desc2, 196, 196, 255 - help.glow, true);
            graphics.Print( -1, FLIP(120), meta.Desc3, 196, 196, 255 - help.glow, true);

            int remaining = ed.numcrewmates() - game.crewmates();

            if(remaining==1){
                graphics.Print(1,FLIP(165), help.number(remaining)+ " crewmate remains", 196, 196, 255 - help.glow, true);
            }else if(remaining>0){
                graphics.Print(1,FLIP(165), help.number(remaining)+ " crewmates remain", 196, 196, 255 - help.glow, true);
            }
        }
#endif
        else
        {
            if (graphics.flipmode)
            {
                for (int i = 0; i < 3; i++)
                {
                    graphics.drawcrewman(16, 32 + (i * 64), 2-i, game.crewstats[2-i]);
                    if (game.crewstats[(2-i)])
                    {
                        graphics.printcrewname(44, 32 + (i * 64)+4+10, 2-i);
                        graphics.printcrewnamestatus(44, 32 + (i * 64)+4, 2-i);
                    }
                    else
                    {
                        graphics.printcrewnamedark(44, 32 + (i * 64)+4+10, 2-i);
                        graphics.Print(44, 32 + (i * 64) + 4, "Missing...", 64,64,64);
                    }

                    graphics.drawcrewman(16+160, 32 + (i * 64), (2-i)+3, game.crewstats[(2-i)+3]);
                    if (game.crewstats[(2-i)+3])
                    {
                        graphics.printcrewname(44+160, 32 + (i * 64)+4+10, (2-i)+3);
                        graphics.printcrewnamestatus(44+160, 32 + (i * 64)+4, (2-i)+3);
                    }
                    else
                    {
                        graphics.printcrewnamedark(44+160, 32 + (i * 64)+4+10, (2-i)+3);
                        graphics.Print(44+160, 32 + (i * 64) + 4, "Missing...", 64,64,64);
                    }
                }
            }
            else
            {
                for (int i = 0; i < 3; i++)
                {
                    graphics.drawcrewman(16, 32 + (i * 64), i, game.crewstats[i]);
                    if (game.crewstats[i])
                    {
                        graphics.printcrewname(44, 32 + (i * 64)+4, i);
                        graphics.printcrewnamestatus(44, 32 + (i * 64)+4+10, i);
                    }
                    else
                    {
                        graphics.printcrewnamedark(44, 32 + (i * 64)+4, i);
                        graphics.Print(44, 32 + (i * 64) + 4 + 10, "Missing...", 64,64,64);
                    }

                    graphics.drawcrewman(16+160, 32 + (i * 64), i+3, game.crewstats[i+3]);
                    if (game.crewstats[i+3])
                    {
                        graphics.printcrewname(44+160, 32 + (i * 64)+4, i+3);
                        graphics.printcrewnamestatus(44+160, 32 + (i * 64)+4+10, i+3);
                    }
                    else
                    {
                        graphics.printcrewnamedark(44+160, 32 + (i * 64)+4, i+3);
                        graphics.Print(44+160, 32 + (i * 64) + 4 + 10, "Missing...", 64,64,64);
                    }
                }
            }
        }
        break;
    case 2:
#if !defined(NO_CUSTOM_LEVELS)
        if(map.custommode)
        {
          if (graphics.flipmode)
          {
              graphics.Print(0, 164, "[Trinkets found]", 196, 196, 255 - help.glow, true);
              graphics.Print(0, 152, help.number(game.trinkets()) + " out of " + help.number(ed.numtrinkets()), 96,96,96, true);

              graphics.Print(0, 114, "[Number of Deaths]", 196, 196, 255 - help.glow, true);
              graphics.Print(0, 102,help.String(game.deathcounts),  96,96,96, true);

              graphics.Print(0, 64, "[Time Taken]", 196, 196, 255 - help.glow, true);
              graphics.Print(0, 52, game.timestring(),  96, 96, 96, true);
          }
          else
          {
              graphics.Print(0, 52, "[Trinkets found]", 196, 196, 255 - help.glow, true);
              graphics.Print(0, 64, help.number(game.trinkets()) + " out of "+help.number(ed.numtrinkets()), 96,96,96, true);

              graphics.Print(0, 102, "[Number of Deaths]", 196, 196, 255 - help.glow, true);
              graphics.Print(0, 114,help.String(game.deathcounts),  96,96,96, true);

              graphics.Print(0, 152, "[Time Taken]", 196, 196, 255 - help.glow, true);
              graphics.Print(0, 164, game.timestring(),  96, 96, 96, true);
          }
        }
        else
#endif
        {
          if (graphics.flipmode)
          {
              graphics.Print(0, 164, "[Trinkets found]", 196, 196, 255 - help.glow, true);
              graphics.Print(0, 152, help.number(game.trinkets()) + " out of Twenty", 96,96,96, true);

              graphics.Print(0, 114, "[Number of Deaths]", 196, 196, 255 - help.glow, true);
              graphics.Print(0, 102,help.String(game.deathcounts),  96,96,96, true);

              graphics.Print(0, 64, "[Time Taken]", 196, 196, 255 - help.glow, true);
              graphics.Print(0, 52, game.timestring(),  96, 96, 96, true);
          }
          else
          {
              graphics.Print(0, 52, "[Trinkets found]", 196, 196, 255 - help.glow, true);
              graphics.Print(0, 64, help.number(game.trinkets()) + " out of Twenty", 96,96,96, true);

              graphics.Print(0, 102, "[Number of Deaths]", 196, 196, 255 - help.glow, true);
              graphics.Print(0, 114,help.String(game.deathcounts),  96,96,96, true);

              graphics.Print(0, 152, "[Time Taken]", 196, 196, 255 - help.glow, true);
              graphics.Print(0, 164, game.timestring(),  96, 96, 96, true);
          }
        }
        break;
    case 3:
        if (game.inintermission)
        {
            graphics.Print(0, 115, "Cannot Save in Level Replay", 146, 146, 180, true);
        }
        else if (game.nodeathmode)
        {
            graphics.Print(0, 115, "Cannot Save in No Death Mode", 146, 146, 180, true);
        }
        else if (game.intimetrial)
        {
            graphics.Print(0, 115, "Cannot Save in Time Trial", 146, 146, 180, true);
        }
        else if (game.insecretlab)
        {
            graphics.Print(0, 115, "Cannot Save in Secret Lab", 146, 146, 180, true);
        }
        else if (game.gamesavefailed)
        {
            graphics.Print(0, 115, "ERROR: Could not save game!", 146, 146, 180, true);
        }
        else if (map.custommode)
        {
            if (game.gamesaved)
            {
                graphics.Print(0, 36, "Game saved ok!", 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2), true);

                graphics.drawpixeltextbox(17, 65, 286, 90, 36,12, 65, 185, 207,0,4);

                if (graphics.flipmode)
                {
                    graphics.Print(0, 122, game.customleveltitle, 25, 255 - (help.glow / 2), 255 - (help.glow / 2), true);
                    graphics.Print(59, 78, game.savetime, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
                    const std::string& trinketcount = help.number(game.savetrinkets);
                    graphics.Print(262-graphics.len(trinketcount), 78, trinketcount, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));

                    graphics.drawsprite(34, 74, 50, graphics.col_clock);
                    graphics.drawsprite(270, 74, 22, graphics.col_trinket);
                }
                else
                {
                    graphics.Print(0, 90, game.customleveltitle, 25, 255 - (help.glow / 2), 255 - (help.glow / 2), true);
                    graphics.Print(59, 132, game.savetime, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
                    const std::string& trinketcount = help.number(game.savetrinkets);
                    graphics.Print(262-graphics.len(trinketcount), 132, trinketcount, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));

                    graphics.drawsprite(34, 126, 50, graphics.col_clock);
                    graphics.drawsprite(270, 126, 22, graphics.col_trinket);
                }
            }
            else
            {
                graphics.Print(0, 80, "[Press ACTION to save your game]", 255 - (help.glow * 2), 255 - (help.glow * 2), 255 - help.glow, true);
            }
        }
        else
        {
            if (graphics.flipmode)
            {
                graphics.Print(0, 186, "(Note: The game is autosaved", 146, 146, 180, true);
                graphics.Print(0, 174, "at every teleporter.)", 146, 146, 180, true);
            }
            else
            {
                graphics.Print(0, 174, "(Note: The game is autosaved", 146, 146, 180, true);
                graphics.Print(0, 186, "at every teleporter.)", 146, 146, 180, true);
            }

            if (game.gamesaved)
            {
                graphics.Print(0, 36, "Game saved ok!", 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2), true);

                graphics.drawpixeltextbox(17, 65, 286, 90, 36,12, 65, 185, 207,0,4);

                if (graphics.flipmode)
                {
                    graphics.Print(0, 132, game.savearea, 25, 255 - (help.glow / 2), 255 - (help.glow / 2), true);
                    for (int i = 0; i < 6; i++)
                    {
                        graphics.drawcrewman(169-(3*42)+(i*42), 98, i, game.crewstats[i], true);
                    }
                    graphics.Print(59, 78, game.savetime, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
                    const std::string& trinketcount = help.number(game.savetrinkets);
                    graphics.Print(262-graphics.len(trinketcount), 78, trinketcount, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));

                    graphics.drawsprite(34, 74, 50, graphics.col_clock);
                    graphics.drawsprite(270, 74, 22, graphics.col_trinket);
                }
                else
                {
                    graphics.Print(0, 80, game.savearea, 25, 255 - (help.glow / 2), 255 - (help.glow / 2), true);
                    for (int i = 0; i < 6; i++)
                    {
                        graphics.drawcrewman(169-(3*42)+(i*42), 95, i, game.crewstats[i], true);
                    }
                    graphics.Print(59, 132, game.savetime, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
                    const std::string& trinketcount = help.number(game.savetrinkets);
                    graphics.Print(262-graphics.len(trinketcount), 132, trinketcount, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));

                    graphics.drawsprite(34, 126, 50, graphics.col_clock);
                    graphics.drawsprite(270, 126, 22, graphics.col_trinket);
                }
            }
            else
            {
                graphics.Print(0, 80, "[Press ACTION to save your game]", 255 - (help.glow * 2), 255 - (help.glow * 2), 255 - help.glow, true);

                if (game.quicksummary != "")
                {
                    if (graphics.flipmode)
                    {
                        graphics.Print(0, 110, "Last Save:", 164 - (help.glow / 4), 164 - (help.glow / 4), 164, true);
                        graphics.Print(0, 100, game.quicksummary, 164  - (help.glow / 4), 164 - (help.glow / 4), 164, true);
                    }
                    else
                    {
                        graphics.Print(0, 100, "Last Save:", 164 - (help.glow / 4), 164 - (help.glow / 4), 164, true);
                        graphics.Print(0, 110, game.quicksummary, 164  - (help.glow / 4), 164 - (help.glow / 4), 164, true);
                    }
                }
            }
        }
        break;
    case 10:
        graphics.Print(128, 220, "[ QUIT ]", 196, 196, 255 - help.glow);

        if (graphics.flipmode)
        {
            if (game.inspecial())
            {
                graphics.Print(0, 135, "Return to main menu?", 196, 196, 255 - help.glow, true);
            }
            else
            {
                graphics.Print(0, 142, "Do you want to quit? You will", 196, 196, 255 - help.glow, true);
                graphics.Print(0, 130, "lose any unsaved progress.", 196, 196, 255 - help.glow, true);
            }

            graphics.Print(80-16, 88, "[ NO, KEEP PLAYING ]", 196, 196, 255 - help.glow);
            graphics.Print(80 + 32, 76, "yes, quit to menu",  96, 96, 96);
        }
        else
        {

            if (game.inspecial())
            {
                graphics.Print(0, 80, "Return to main menu?", 196, 196, 255 - help.glow, true);
            }
            else
            {
                graphics.Print(0, 76, "Do you want to quit? You will", 196, 196, 255 - help.glow, true);
                graphics.Print(0, 88, "lose any unsaved progress.", 196, 196, 255 - help.glow, true);
            }

            graphics.Print(80-16, 130, "[ NO, KEEP PLAYING ]", 196, 196, 255 - help.glow);
            graphics.Print(80 + 32, 142, "yes, quit to menu",  96, 96, 96);

        }
        break;
    case 11:
        graphics.Print(128, 220, "[ QUIT ]", 196, 196, 255 - help.glow);

        if (graphics.flipmode)
        {
            if (game.inspecial())
            {
                graphics.Print(0, 135, "Return to main menu?", 196, 196, 255 - help.glow, true);
            }
            else
            {
                graphics.Print(0, 142, "Do you want to quit? You will", 196, 196, 255 - help.glow, true);
                graphics.Print(0, 130, "lose any unsaved progress.", 196, 196, 255 - help.glow, true);
            }

            graphics.Print(80, 88, "no, keep playing", 96,96,96);
            graphics.Print(80+32-16, 76, "[ YES, QUIT TO MENU ]",  196, 196, 255 - help.glow);
        }
        else
        {
            if (game.inspecial())
            {
                graphics.Print(0, 80, "Return to main menu?", 196, 196, 255 - help.glow, true);
            }
            else
            {
                graphics.Print(0, 76, "Do you want to quit? You will", 196, 196, 255 - help.glow, true);
                graphics.Print(0, 88, "lose any unsaved progress.", 196, 196, 255 - help.glow, true);
            }

            graphics.Print(80, 130, "no, keep playing", 96,96,96);
            graphics.Print(80+32-16, 142, "[ YES, QUIT TO MENU ]", 196, 196, 255 - help.glow);
        }
        break;
    case 20:
        graphics.Print(128, 220, "[ GRAVITRON ]", 196, 196, 255 - help.glow, true);

        if (graphics.flipmode)
        {
            graphics.Print(0, 76, "the secret laboratory?", 196, 196, 255 - help.glow, true);
            graphics.Print(0, 88, "Do you want to return to", 196, 196, 255 - help.glow, true);
            graphics.Print(80-16, 142, "[ NO, KEEP PLAYING ]", 196, 196, 255 - help.glow);
            graphics.Print(80 + 32, 130, "yes, return",  96, 96, 96);
        }
        else
        {
            graphics.Print(0, 76, "Do you want to return to", 196, 196, 255 - help.glow, true);
            graphics.Print(0, 88, "the secret laboratory?", 196, 196, 255 - help.glow, true);
            graphics.Print(80-16, 130, "[ NO, KEEP PLAYING ]", 196, 196, 255 - help.glow);
            graphics.Print(80 + 32, 142, "yes, return",  96, 96, 96);
        }

        break;
    case 21:
        graphics.Print(128, 220, "[ GRAVITRON ]", 196, 196, 255 - help.glow, true);

        if (graphics.flipmode)
        {
            graphics.Print(0, 76, "the secret laboratory?", 196, 196, 255 - help.glow, true);
            graphics.Print(0, 88, "Do you want to return to", 196, 196, 255 - help.glow, true);
            graphics.Print(80, 142, "no, keep playing", 96, 96, 96);
            graphics.Print(80 + 32-16, 130, "[ YES, RETURN ]",  196, 196, 255 - help.glow);
        }
        else
        {
            graphics.Print(0, 76, "Do you want to return to", 196, 196, 255 - help.glow, true);
            graphics.Print(0, 88, "the secret laboratory?", 196, 196, 255 - help.glow, true);
            graphics.Print(80, 130, "no, keep playing", 96, 96, 96);
            graphics.Print(80 + 32-16, 142, "[ YES, RETURN ]",  196, 196, 255 - help.glow);
        }

    }




    // We need to draw the black screen above the menu in order to disguise it
    // being jankily brought down in glitchrunner mode when exiting to the title
    // Otherwise, there's no reason to obscure the menu
    if (GlitchrunnerMode_less_than_or_equal(Glitchrunner2_2)
    || graphics.fademode == 3
    || graphics.fademode == 5
    || game.fadetomenu
    || game.fadetolab)
    {
        graphics.drawfade();
    }

    if (graphics.resumegamemode || graphics.menuoffset > 0 || graphics.oldmenuoffset > 0)
    {
        graphics.menuoffrender();
    }
    else
    {
        graphics.renderwithscreeneffects();
    }
}

void teleporterrender(void)
{
    ClearSurface(graphics.backBuffer);
    int tempx;
    int tempy;
    //draw screen alliteration
    //Roomname:
    int temp = map.area(game.roomx, game.roomy);
    if (temp < 2 && !map.custommode && graphics.fademode==0)
    {
        graphics.Print(5, 2, map.hiddenname, 196, 196, 255 - help.glow, true);
    }
    else
    {
        graphics.Print(5, 2, map.roomname, 196, 196, 255 - help.glow, true);
    }

    //Background color
    FillRect(graphics.backBuffer, 0, 12, 320, 240, 10, 24, 26);

    //draw the map image
    graphics.drawpixeltextbox(35, 16, 250, 190, 32,24, 65, 185, 207,4,0);
    graphics.drawimage(1, 40, 21, false);
    //black out areas we can't see yet
    for (int j = 0; j < 20; j++)
    {
        for (int i = 0; i < 20; i++)
        {
            if(!map.isexplored(i, j))
            {
                graphics.drawimage(2, 40 + (i * 12), 21 + (j * 9), false);
            }
        }
    }

    //draw the coordinates //current
    if (game.roomx == 109)
    {
        //tower!instead of room y, scale map.ypos
        graphics.drawrect(40 + ((game.roomx - 100) * 12) + 2, 21  + 2, 12 - 4, 180 - 4, 16, 245 - (help.glow * 2), 245 - (help.glow * 2));
    }
    else
    {
        graphics.drawrect(40 + ((game.roomx - 100) * 12) + 2, 21 + ((game.roomy - 100) * 9) + 2, 12 - 4, 9 - 4, 16, 245 - (help.glow * 2), 245 - (help.glow * 2));
    }

    if (game.useteleporter)
    {
        //Draw the chosen destination coordinate!
        //TODO
        //draw the coordinates //destination
        int tempx_ = map.teleporters[game.teleport_to_teleporter].x;
        int tempy_ = map.teleporters[game.teleport_to_teleporter].y;
        graphics.drawrect(40 + (tempx_ * 12) + 1, 21 + (tempy_ * 9) + 1, 12 - 2, 9 - 2, 245 - (help.glow * 2), 16, 16);
        graphics.drawrect(40 + (tempx_ * 12) + 3, 21 + (tempy_ * 9) + 3, 12 - 6, 9 - 6, 245 - (help.glow * 2), 16, 16);
    }

    //draw legend details
    for (size_t i = 0; i < map.teleporters.size(); i++)
    {
        if (map.showteleporters && map.isexplored(map.teleporters[i].x, map.teleporters[i].y))
        {
            temp = 1126 + (int) map.isexplored(map.teleporters[i].x, map.teleporters[i].y);
            if (graphics.flipmode) temp += 3;
            graphics.drawtile(40 + 3 + (map.teleporters[i].x * 12), 22 + (map.teleporters[i].y * 9), temp);
        }
        else if(map.showtargets && !map.isexplored(map.teleporters[i].x, map.teleporters[i].y))
        {
            temp = 1126 + (int) map.isexplored(map.teleporters[i].x, map.teleporters[i].y);
            if (graphics.flipmode) temp += 3;
            graphics.drawtile(40 + 3 + (map.teleporters[i].x * 12), 22 + (map.teleporters[i].y * 9), temp);
        }
    }

    if (map.showtrinkets)
    {
        for (size_t i = 0; i < map.shinytrinkets.size(); i++)
        {
            if (!obj.collect[i])
            {
                temp = 1086;
                if (graphics.flipmode) temp += 3;
                graphics.drawtile(40 + 3 + (map.shinytrinkets[i].x * 12), 22 + (map.shinytrinkets[i].y * 9),	temp);
            }
        }
    }

    tempx = map.teleporters[game.teleport_to_teleporter].x;
    tempy = map.teleporters[game.teleport_to_teleporter].y;
    if (game.useteleporter && ((help.slowsine%16)>8))
    {
        //colour in the legend
        temp = 1128;
        if (graphics.flipmode) temp += 3;
        graphics.drawtile(40 + 3 + (tempx * 12), 22 + (tempy * 9), temp);
    }

    graphics.cutscenebars();


    if (game.useteleporter)
    {
        /* Screen width 40 chars, 4 per char */
        char buffer[160 + 1];
        static const char raw[] = "Press %s to Teleport";
        const char* final_string = interact_prompt(
            buffer,
            sizeof(buffer),
            raw
        );

        //Instructions!
        graphics.Print(5, 210, "Press Left/Right to choose a Teleporter", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
        graphics.Print(5, 225, final_string, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
    }

    graphics.drawgui();

    if (graphics.flipmode)
    {
        if (game.advancetext) graphics.bprint(5, 228, "- Press ACTION to advance text -", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
    }
    else
    {
        if (game.advancetext) graphics.bprint(5, 5, "- Press ACTION to advance text -", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
    }


    if (graphics.resumegamemode || graphics.menuoffset > 0 || graphics.oldmenuoffset > 0)
    {
        graphics.menuoffrender();
    }
    else
    {
        graphics.render();
    }
}
