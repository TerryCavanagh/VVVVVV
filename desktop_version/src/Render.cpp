#include <SDL.h>

#include "Constants.h"
#include "Credits.h"
#include "CustomLevels.h"
#include "Editor.h"
#include "Entity.h"
#include "FileSystemUtils.h"
#include "Font.h"
#include "GlitchrunnerMode.h"
#include "Graphics.h"
#include "GraphicsUtil.h"
#include "InterimVersion.h"
#include "KeyPoll.h"
#include "Localization.h"
#include "LocalizationStorage.h"
#include "MakeAndPlay.h"
#include "Map.h"
#include "Maths.h"
#include "Music.h"
#include "ReleaseVersion.h"
#include "RoomnameTranslator.h"
#include "Screen.h"
#include "Script.h"
#include "UtilityClass.h"
#include "VFormat.h"

static int tr;
static int tg;
static int tb;

struct MapRenderData
{
    int zoom;
    int xoff;
    int yoff;
    int legendxoff;
    int legendyoff;
};

static inline void drawslowdowntext(const int y)
{
    switch (game.slowdown)
    {
    case 30:
        graphics.PrintWrap( -1, y, loc::gettext("Game speed is normal."), tr/2, tg/2, tb/2, true);
        break;
    case 24:
        graphics.PrintWrap( -1, y, loc::gettext("Game speed is at 80%"), tr, tg, tb, true);
        break;
    case 18:
        graphics.PrintWrap( -1, y, loc::gettext("Game speed is at 60%"), tr, tg, tb, true);
        break;
    case 12:
        graphics.PrintWrap( -1, y, loc::gettext("Game speed is at 40%"), tr, tg, tb, true);
        break;
    }
}

static void slider_get(char* buffer, size_t buffer_len, int position, int n_positions, int target_width)
{
    /* Print a slider to the buffer for target_width in pixels.
     *
     * <--target w-->
     * []............
     * ......[]......
     * ............[]
     * ^^^^^^         dots_per_position=6
     * ^^^^^^^^^^^^^^ max_chars=14
     */

    if (n_positions < 2 || position < 0 || position >= n_positions)
    {
        buffer[0] = '\0';
        return;
    }

    int max_chars = ((target_width - font::len(0, "[]")) / font::len(0, ".")) + 2;
    max_chars = SDL_min(max_chars, buffer_len-1);

    int dots_per_position = (max_chars-2) / (n_positions-1);
    max_chars = dots_per_position * (n_positions-1) + 2;

    VVV_fillstring(buffer, max_chars+1, '.');
    if (dots_per_position<1)
    {
        return;
    }

    int handle_idx = position*dots_per_position;
    buffer[handle_idx] = '[';
    buffer[handle_idx+1] = ']';
}

static void volumesliderrender(void)
{
    int volume_max_position = USER_VOLUME_MAX / USER_VOLUME_STEP;

    int volume;
    switch (game.currentmenuoption)
    {
    case 0:
        volume = music.user_music_volume;
        break;
    case 1:
        volume = music.user_sound_volume;
        break;
    default:
        SDL_assert(0 && "Unhandled volume slider menu option!");
        return;
    }

    char slider[40 + 1];
    slider_get(slider, sizeof(slider), volume_max_position*volume/USER_VOLUME_MAX, volume_max_position+1, 240);

    char buffer[SCREEN_WIDTH_CHARS + 1];

    if (game.slidermode == SLIDER_NONE)
    {
        SDL_strlcpy(buffer, slider, sizeof(buffer));
    }
    else
    {
        /* Draw selection brackets. */
        vformat_buf(buffer, sizeof(buffer), loc::get_langmeta()->menu_select.c_str(), "label:str", slider);
    }

    font::print(PR_CEN, -1, 95, buffer, tr, tg, tb);
}

static void inline drawglitchrunnertext(const int y)
{
    int tempr = tr;
    int tempg = tg;
    int tempb = tb;

    char buffer[SCREEN_WIDTH_CHARS + 1];

    const enum GlitchrunnerMode mode = GlitchrunnerMode_get();

    if (mode == GlitchrunnerNone)
    {
        tempr /= 2;
        tempg /= 2;
        tempb /= 2;

        SDL_strlcpy(buffer, loc::gettext("Glitchrunner mode is OFF"), sizeof(buffer));
    }
    else
    {
        const char* mode_string = loc::gettext(GlitchrunnerMode_enum_to_string(mode));
        vformat_buf(buffer, sizeof(buffer), loc::gettext("Glitchrunner mode is {version}"), "version:str", mode_string);
    }

    graphics.PrintWrap(-1, y, buffer, tempr, tempg, tempb, true);
}

static void menurender(void)
{

    switch (game.currentmenuname)
    {
    case Menu::mainmenu:
    {
        const int temp = 50;
        graphics.draw_sprite((160 - 96) + 0 * 32, temp, 23, tr, tg, tb);
        graphics.draw_sprite((160 - 96) + 1 * 32, temp, 23, tr, tg, tb);
        graphics.draw_sprite((160 - 96) + 2 * 32, temp, 23, tr, tg, tb);
        graphics.draw_sprite((160 - 96) + 3 * 32, temp, 23, tr, tg, tb);
        graphics.draw_sprite((160 - 96) + 4 * 32, temp, 23, tr, tg, tb);
        graphics.draw_sprite((160 - 96) + 5 * 32, temp, 23, tr, tg, tb);
#if defined(MAKEANDPLAY)
        font::print(PR_RIGHT, 264, temp+35, loc::gettext("MAKE AND PLAY EDITION"), tr, tg, tb);
#endif
#ifdef INTERIM_VERSION_EXISTS
        font::print(PR_RIGHT | PR_FONT_8X8, 310, 200, COMMIT_DATE, tr/2, tg/2, tb/2);
        font::print(PR_RIGHT | PR_FONT_8X8, 310, 210, INTERIM_COMMIT, tr/2, tg/2, tb/2);
        font::print(PR_RIGHT | PR_FONT_8X8, 310, 220, BRANCH_NAME, tr/2, tg/2, tb/2);
#endif
        font::print(PR_RIGHT, 310, 230, RELEASE_VERSION, tr/2, tg/2, tb/2);

        if(music.mmmmmm){
            font::print(0, 10, 230, loc::gettext("[MMMMMM Mod Installed]"), tr/2, tg/2, tb/2);
        }
        break;
    }
#if !defined(NO_CUSTOM_LEVELS)
    case Menu::levellist:
    {
      if(cl.ListOfMetaData.size()==0){
      graphics.PrintWrap( -1, 90, loc::gettext("ERROR: No levels found."), tr, tg, tb, true);
      }
      int tmp=game.currentmenuoption+(game.levelpage*8);
      if(INBOUNDS_VEC(tmp, cl.ListOfMetaData)){
        const bool nextlastoptions = cl.ListOfMetaData.size() > 8;
        //Don't show next/previous page or return to menu options here!
        if(nextlastoptions && game.menuoptions.size() - game.currentmenuoption<=3){

        }else{
          uint32_t level_flags = PR_FONT_IDX(cl.ListOfMetaData[tmp].level_main_font_idx);
          uint32_t title_flags = cl.ListOfMetaData[tmp].title_is_gettext ? PR_FONT_INTERFACE : level_flags;
          uint32_t creator_flags = cl.ListOfMetaData[tmp].creator_is_gettext ? PR_FONT_INTERFACE : level_flags;

          font::print(title_flags | PR_2X | PR_CEN, -1, 15, cl.ListOfMetaData[tmp].title, tr, tg, tb);
          char creatorline[SCREEN_WIDTH_CHARS + 1];
          vformat_buf(
            creatorline, sizeof(creatorline),
            loc::gettext("by {author}"),
            "author:str",
            cl.ListOfMetaData[tmp].creator.c_str()
          );
          int sp = SDL_max(10, font::height(level_flags));
          font::print(creator_flags | PR_CEN, -1, 40, creatorline, tr, tg, tb);
          font::print(level_flags | PR_CEN, -1, 40+sp, cl.ListOfMetaData[tmp].website, tr, tg, tb);
          font::print(level_flags | PR_CEN, -1, 40+sp*3, cl.ListOfMetaData[tmp].Desc1, tr, tg, tb);
          font::print(level_flags | PR_CEN, -1, 40+sp*4, cl.ListOfMetaData[tmp].Desc2, tr, tg, tb);
          if (sp <= 10)
          {
            font::print(level_flags | PR_CEN, -1, 40+sp*5, cl.ListOfMetaData[tmp].Desc3, tr, tg, tb);
          }
        }
      }
      break;
    }
#endif
    case Menu::errornostart:
        graphics.PrintWrap( -1, 65, loc::gettext("ERROR: This level has no start point!"), tr, tg, tb, true);
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
                font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Flip Mode"), tr, tg, tb);
                int next_y = graphics.PrintWrap( -1, 65, loc::gettext("Flip the entire game vertically."), tr, tg, tb, true);
                if (graphics.setflipmode)
                {
                    graphics.PrintWrap( -1, next_y, loc::gettext("Currently ENABLED!"), tr, tg, tb, true);
                }
                else
                {
                    graphics.PrintWrap( -1, next_y, loc::gettext("Currently Disabled."), tr/2, tg/2, tb/2, true);
                }
            }
        }

        if (game.currentmenuoption == gameplayoptionsoffset + 0)
        {
            //Toggle FPS
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Toggle 30+ FPS"), tr, tg, tb);
            int next_y = graphics.PrintWrap(-1, 65, loc::gettext("Change whether the game runs at 30 or over 30 FPS."), tr, tg, tb, true);

            if (!game.over30mode)
            {
                graphics.PrintWrap(-1, next_y, loc::gettext("Current mode: 30 FPS"), tr/2, tg/2, tb/2, true);
            }
            else
            {
                graphics.PrintWrap(-1, next_y, loc::gettext("Current mode: Over 30 FPS"), tr, tg, tb, true);
            }
            break;
        }
        else if (game.currentmenuoption == gameplayoptionsoffset + 1)
        {
            //Speedrunner options
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Speedrunner Options"), tr, tg, tb);
            graphics.PrintWrap(-1, 65, loc::gettext("Access some advanced settings that might be of interest to speedrunners."), tr, tg, tb, true);
        }
        else if (game.currentmenuoption == gameplayoptionsoffset + 2)
        {
            //Advanced options
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Advanced Options"), tr, tg, tb);
            graphics.PrintWrap(-1, 65, loc::gettext("All other gameplay settings."), tr, tg, tb, true);
        }
        else if (game.currentmenuoption == gameplayoptionsoffset + 3)
        {
            //Clear Data
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Clear Data"), tr, tg, tb);
            graphics.PrintWrap(-1, 65, loc::gettext("Delete your main game save data and unlocked play modes."), tr, tg, tb, true);
        }
        else if (game.currentmenuoption == gameplayoptionsoffset + 4)
        {
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Clear Data"), tr, tg, tb);
            graphics.PrintWrap(-1, 65, loc::gettext("Delete your custom level save data and completion stars."), tr, tg, tb, true);
        }

        break;
    }
    case Menu::options:
        switch (game.currentmenuoption)
        {
        case 0:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Gameplay Options"), tr, tg, tb);
            graphics.PrintWrap(-1, 65, loc::gettext("Adjust various gameplay settings."), tr, tg, tb, true);
            break;
        case 1:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Graphics Options"), tr, tg, tb);
            graphics.PrintWrap(-1, 65, loc::gettext("Adjust screen settings."), tr, tg, tb, true);
            break;
        case 2:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Audio Options"), tr, tg, tb);
            if (music.mmmmmm)
            {
                graphics.PrintWrap(-1, 65, loc::gettext("Adjust volume settings and soundtrack."), tr, tg, tb, true);
            }
            else
            {
                graphics.PrintWrap(-1, 65, loc::gettext("Adjust volume settings."), tr, tg, tb, true);
            }
            break;
        case 3:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Game Pad Options"), tr, tg, tb);
            graphics.PrintWrap(-1, 65, loc::gettext("Rebind your controller's buttons and adjust sensitivity."), tr, tg, tb, true);
            break;
        case 4:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Accessibility"), tr, tg, tb);
            graphics.PrintWrap(-1, 65, loc::gettext("Disable screen effects, enable slowdown modes or invincibility."), tr, tg, tb, true);
            break;
        case 5:
            font::print(PR_2X | PR_CEN,  -1, 30, loc::gettext("Language"), tr, tg, tb);
            graphics.PrintWrap( -1, 65, loc::gettext("Change the language."), tr, tg, tb, true);
        }
        break;
    case Menu::graphicoptions:
    {
        int offset = 0;
        if (game.currentmenuoption == offset + 0 && !gameScreen.isForcedFullscreen())
        {
            font::print(PR_2X | PR_CEN,  -1, 30, loc::gettext("Toggle Fullscreen"), tr, tg, tb);
            int next_y = graphics.PrintWrap( -1, 65, loc::gettext("Change to fullscreen/windowed mode."), tr, tg, tb, true);

            if (gameScreen.isWindowed)
            {
                graphics.PrintWrap( -1, next_y, loc::gettext("Current mode: WINDOWED"), tr, tg, tb, true);
            }
            else
            {
                graphics.PrintWrap( -1, next_y, loc::gettext("Current mode: FULLSCREEN"), tr, tg, tb, true);
            }
        }

        if (gameScreen.isForcedFullscreen())
        {
            --offset;
        }

        if (game.currentmenuoption == offset + 1)
        {
            font::print(PR_2X | PR_CEN,  -1, 30, loc::gettext("Scaling Mode"), tr, tg, tb);
            int next_y = graphics.PrintWrap( -1, 65, loc::gettext("Choose letterbox/stretch/integer mode."), tr, tg, tb, true);

            switch (gameScreen.scalingMode)
            {
            case SCALING_INTEGER:
                graphics.PrintWrap( -1, next_y, loc::gettext("Current mode: INTEGER"), tr, tg, tb, true);
                break;
            case SCALING_STRETCH:
                graphics.PrintWrap( -1, next_y, loc::gettext("Current mode: STRETCH"), tr, tg, tb, true);
                break;
            case SCALING_LETTERBOX:
            default:
                graphics.PrintWrap( -1, next_y, loc::gettext("Current mode: LETTERBOX"), tr, tg, tb, true);
                break;
            }
        }
        if (game.currentmenuoption == offset + 2 && !gameScreen.isForcedFullscreen())
        {
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Resize to Nearest"), tr, tg, tb);
            int next_y = graphics.PrintWrap(-1, 65, loc::gettext("Resize to the nearest window size that is of an integer multiple."), tr, tg, tb, true);
            if (!gameScreen.isWindowed)
            {
                graphics.PrintWrap(-1, next_y, loc::gettext("You must be in windowed mode to use this option."), tr, tg, tb, true);
            }
        }
        if (gameScreen.isForcedFullscreen())
        {
            --offset;
        }
        if (game.currentmenuoption == offset + 3)
        {
            font::print(PR_2X | PR_CEN,  -1, 30, loc::gettext("Toggle Filter"), tr, tg, tb);
            int next_y = graphics.PrintWrap( -1, 65, loc::gettext("Change to nearest/linear filter."), tr, tg, tb, true);

            if (gameScreen.isFiltered)
            {
                graphics.PrintWrap( -1, next_y, loc::gettext("Current mode: LINEAR"), tr, tg, tb, true);
            }
            else
            {
                graphics.PrintWrap( -1, next_y, loc::gettext("Current mode: NEAREST"), tr, tg, tb, true);
            }
        }

        if (game.currentmenuoption == offset + 4)
        {
            font::print(PR_2X | PR_CEN,  -1, 30, loc::gettext("Analogue Mode"), tr, tg, tb);
            graphics.PrintWrap( -1, 65, loc::gettext("There is nothing wrong with your television set. Do not attempt to adjust the picture."), tr, tg, tb, true);
        }
        if (game.currentmenuoption == offset + 5)
        {
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Toggle VSync"), tr, tg, tb);
            int next_y = graphics.PrintWrap(-1, 65, loc::gettext("Turn VSync on or off."), tr, tg, tb, true);

            if (!gameScreen.vsync)
            {
                graphics.PrintWrap(-1, next_y, loc::gettext("Current mode: VSYNC OFF"), tr/2, tg/2, tb/2, true);
            }
            else
            {
                graphics.PrintWrap(-1, next_y, loc::gettext("Current mode: VSYNC ON"), tr, tg, tb, true);
            }
        }
        break;
    }
    case Menu::audiooptions:
        switch (game.currentmenuoption)
        {
        case 0:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Music Volume"), tr, tg, tb);
            graphics.PrintWrap(-1, 65, loc::gettext("Change the volume of the music."), tr, tg, tb, true);
            volumesliderrender();
            break;
        case 1:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Sound Volume"), tr, tg, tb);
            graphics.PrintWrap(-1, 65, loc::gettext("Change the volume of sound effects."), tr, tg, tb, true);
            volumesliderrender();
            break;
        case 2:
        {
            if (!music.mmmmmm)
            {
                break;
            }

            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Soundtrack"), tr, tg, tb);
            int next_y = graphics.PrintWrap(-1, 65, loc::gettext("Toggle between MMMMMM and PPPPPP."), tr, tg, tb, true);

            const char* soundtrack;
            if (music.usingmmmmmm)
            {
                soundtrack = loc::gettext("Current soundtrack: MMMMMM");
            }
            else
            {
                soundtrack = loc::gettext("Current soundtrack: PPPPPP");
            }
            graphics.PrintWrap(-1, next_y, soundtrack, tr, tg, tb, true);
            break;
        }

        }
        break;
    case Menu::credits:
        font::print(PR_CEN, -1, 50, loc::gettext("VVVVVV is a game by"), tr, tg, tb);
        font::print(PR_2X | PR_CEN | PR_FONT_8X8, -1, 65, "Terry Cavanagh", tr, tg, tb);

        graphics.drawimagecol(IMAGE_SITE, -1, 86, graphics.getRGB(tr, tg, tb), true);

        font::print(PR_CEN, -1, 120, loc::gettext("and features music by"), tr, tg, tb);
        font::print(PR_2X | PR_CEN | PR_FONT_8X8, -1, 135, "Magnus Pålsson", tr, tg, tb);
        graphics.drawimagecol(IMAGE_SITE2, -1, 156, graphics.getRGB(tr, tg, tb), true);
        break;
    case Menu::credits2:
        font::print(PR_CEN, -1, 50, loc::gettext("Roomnames are by"), tr, tg, tb);
        font::print(PR_2X | PR_CEN | PR_FONT_8X8, -1, 65, "Bennett Foddy", tr, tg, tb);
        graphics.drawimagecol(IMAGE_SITE3, -1, 86, graphics.getRGB(tr, tg, tb), true);
        font::print(PR_CEN, -1, 110, loc::gettext("C++ version by"), tr, tg, tb);
        font::print(PR_2X | PR_CEN | PR_FONT_8X8, -1, 125, "Simon Roth", tr, tg, tb);
        font::print(PR_2X | PR_CEN | PR_FONT_8X8, -1, 145, "Ethan Lee", tr, tg, tb);
        font::print(PR_2X | PR_CEN | PR_FONT_8X8, -1, 165, "Misa Kai", tr, tg, tb);
        break;
    case Menu::credits25:
        font::print(PR_CEN, -1, 40, loc::gettext("Beta Testing by"), tr, tg, tb);
        font::print(PR_2X | PR_CEN | PR_FONT_8X8, -1, 55, "Sam Kaplan", tr, tg, tb);
        font::print(PR_2X | PR_CEN | PR_FONT_8X8, -1, 75, "Pauli Kohberger", tr, tg, tb);
        font::print(PR_CEN, -1, 130, loc::gettext("Ending Picture by"), tr, tg, tb);
        font::print(PR_2X | PR_CEN | PR_FONT_8X8, -1, 145, "Pauli Kohberger", tr, tg, tb);
        break;
    case Menu::credits3:
    {
        graphics.PrintWrap( -1, 20, loc::gettext("VVVVVV is supported by the following patrons"), tr, tg, tb, true);

        int startidx = game.current_credits_list_index;
        int endidx = SDL_min(startidx + 9, (int)SDL_arraysize(Credits::superpatrons));

        int xofs = 80 - 16;
        int yofs = 40 + 20;

        for (int i = startidx; i < endidx; ++i)
        {
            font::print(PR_FONT_8X8, xofs, yofs, Credits::superpatrons[i], tr, tg, tb);
            xofs += 4;
            yofs += 14;
        }
        break;
    }
    case Menu::credits4:
    {
        graphics.PrintWrap( -1, 20, loc::gettext("and also by"), tr, tg, tb, true);

        int startidx = game.current_credits_list_index;
        int endidx = SDL_min(startidx + 14, (int)SDL_arraysize(Credits::patrons));

        int maxheight = 10 * 14;
        int totalheight = (endidx - startidx) * 10;
        int emptyspace = maxheight - totalheight;

        int yofs = 40 + (emptyspace / 2);

        for (int i = startidx; i < endidx; ++i)
        {
            font::print(PR_FONT_8X8, 80, yofs, Credits::patrons[i], tr, tg, tb);
            yofs += 10;
        }
        break;
    }
    case Menu::credits5:
    {
        graphics.PrintWrap( -1, 20, loc::gettext("With contributions on GitHub from"), tr, tg, tb, true);

        int startidx = game.current_credits_list_index;
        int endidx = SDL_min(startidx + 9, (int)SDL_arraysize(Credits::githubfriends));

        int maxheight = 14 * 9;
        int totalheight = (endidx - startidx) * 14;
        int emptyspace = maxheight - totalheight;

        int xofs, yofs;

        xofs = 80 - 16;
        yofs = 40 + 20 + (emptyspace / 2);

        for (int i = startidx; i < endidx; ++i)
        {
            font::print(PR_FONT_8X8, xofs, yofs, Credits::githubfriends[i], tr, tg, tb);
            xofs += 4;
            yofs += 14;
        }
        break;
    }
    case Menu::credits6:
        graphics.PrintWrap( -1, 20, loc::gettext("and thanks also to:"), tr, tg, tb, true);

        font::print(PR_2X | PR_CEN, -1, 60, loc::gettext("You!"), tr, tg, tb);

        graphics.PrintWrap( 80, 100, loc::gettext("Your support makes it possible for me to continue making the games I want to make, now and into the future."), tr, tg, tb,true);

        graphics.PrintWrap( 80, 150,loc::gettext("Thank you!"), tr, tg, tb,true);
        break;
    case Menu::setinvincibility:
        graphics.PrintWrap( -1, 100, loc::gettext("Are you sure you want to enable invincibility?"), tr, tg, tb, true);
        break;
    case Menu::setslowdown:
    {
        font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Game Speed"), tr, tg, tb);
        int next_y = graphics.PrintWrap( -1, 65, loc::gettext("Select a new game speed below."), tr, tg, tb, true);
        drawslowdowntext(next_y);
        break;
    }
    case Menu::newgamewarning:
        graphics.PrintWrap( -1, 100, loc::gettext("Are you sure? This will delete your current saves..."), tr, tg, tb, true);
        break;
    case Menu::cleardatamenu:
    case Menu::clearcustomdatamenu:
        graphics.PrintWrap( -1, 100, loc::gettext("Are you sure you want to delete all your saved data?"), tr, tg, tb, true);
        break;
    case Menu::deletequicklevel:
        graphics.PrintWrap(-1, 100, loc::gettext("Are you sure you want to delete your quicksave?"), tr, tg, tb, true);
        break;
    case Menu::startnodeathmode:
    {
        int next_y;
        next_y = graphics.PrintWrap( -1, 45, loc::gettext("Good luck!"), tr, tg, tb, true);
        next_y = graphics.PrintWrap( -1, next_y+15, loc::gettext("You cannot save in this mode."), tr, tg, tb, true);
        graphics.PrintWrap( -1, next_y+5, loc::gettext("Would you like to disable the cutscenes during the game?"), tr, tg, tb, true);
        break;
    }
    case Menu::controller:
        font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Game Pad"), tr, tg, tb);
        graphics.PrintWrap( -1, 55, loc::gettext("Change controller options."), tr, tg, tb, true);
        switch (game.currentmenuoption)
        {
        case 0:
        {
            font::print(0, 32, 75, loc::gettext("Low"), tr, tg, tb);
            font::print(PR_CEN, -1, 75, loc::gettext("Medium"), tr, tg, tb);
            font::print(PR_RIGHT, 288, 75, loc::gettext("High"), tr, tg, tb);
            char slider[SCREEN_WIDTH_CHARS + 1];
            slider_get(slider, sizeof(slider), key.sensitivity, 5, 240);
            font::print(PR_CEN, -1, 85, slider, tr, tg, tb);
            break;
        }
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            font::print(PR_CEN, -1, 75, loc::gettext("Flip is bound to: ") + std::string(help.GCString(game.controllerButton_flip)) , tr, tg, tb);
            font::print(PR_CEN, -1, 85, loc::gettext("Enter is bound to: ")  + std::string(help.GCString(game.controllerButton_map)), tr, tg, tb);
            font::print(PR_CEN, -1, 95, loc::gettext("Menu is bound to: ") + std::string(help.GCString(game.controllerButton_esc)) , tr, tg, tb);
            font::print(PR_CEN, -1, 105, loc::gettext("Restart is bound to: ") + std::string(help.GCString(game.controllerButton_restart)) , tr, tg, tb);
            font::print(PR_CEN, -1, 115, loc::gettext("Interact is bound to: ") + std::string(help.GCString(game.controllerButton_interact)) , tr, tg, tb);
            break;
        }


        break;
    case Menu::language:
        if (loc::languagelist.empty())
        {
            graphics.PrintWrap(-1, 90, loc::gettext("ERROR: No language files found."), tr, tg, tb, true);
        }
        else if ((unsigned)game.currentmenuoption < loc::languagelist.size())
        {
            graphics.PrintWrap(-1, 8, loc::languagelist[game.currentmenuoption].credit, tr/2, tg/2, tb/2, true);
            font::print(PR_CEN, -1, 230, loc::languagelist[game.currentmenuoption].action_hint, tr/2, tg/2, tb/2);
        }
        break;
    case Menu::translator_main:
        switch (game.currentmenuoption)
        {
        case 0:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Translator options"), tr, tg, tb);
            graphics.PrintWrap( -1, 65, loc::gettext("Some options that are useful for translators and developers."), tr, tg, tb, true);
            break;
        case 1:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Maintenance"), tr, tg, tb);
            graphics.PrintWrap( -1, 65, loc::gettext("Sync all language files after adding new strings."), tr, tg, tb, true);
            break;
        }
        {
            if (FILESYSTEM_isMainLangDirFromRepo())
            {
                // Just giving people who manually compiled the game some hint as to why this menu is here!
                font::print(0, 8, 208, loc::gettext("Repository language folder:"), tr/2, tg/2, tb/2);
            }
            else
            {
                font::print(0, 8, 208, loc::gettext("Language folder:"), tr/2, tg/2, tb/2);
            }

            font::print(PR_RIGHT, 316, 224, FILESYSTEM_getUserMainLangDirectory(), tr/2, tg/2, tb/2);
        }
        break;
    case Menu::translator_options:
        switch (game.currentmenuoption)
        {
        case 0:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Statistics"), tr, tg, tb);
            graphics.PrintWrap( -1, 65, loc::gettext("Count the amount of untranslated strings for this language."), tr, tg, tb, true);
            break;
        case 1:
        {
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Translate rooms"), tr, tg, tb);
            int next_y = graphics.PrintWrap( -1, 65, loc::gettext("Enable room name translation mode, so you can translate room names in context. Press I for invincibility."), tr, tg, tb, true);

            if (roomname_translator::enabled)
            {
                graphics.PrintWrap( -1, next_y, loc::gettext("Currently ENABLED!"), tr, tg, tb, true);
            }
            else
            {
                graphics.PrintWrap( -1, next_y, loc::gettext("Currently Disabled."), tr/2, tg/2, tb/2, true);
            }
            break;
        }
        case 2:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Explore game"), tr, tg, tb);
            graphics.PrintWrap( -1, 65, loc::gettext("Explore the rooms of any level in the game, to find all room names to translate."), tr, tg, tb, true);
            break;
        case 3:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Menu test"), tr, tg, tb);
            graphics.PrintWrap( -1, 65, loc::gettext("Cycle through most menus in the game. The menus will not actually work, all options take you to the next menu instead. Press Escape to stop."), tr, tg, tb, true);
            break;
        case 4:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Cutscene test"), tr, tg, tb);
            graphics.PrintWrap( -1, 65, loc::gettext("Display all text boxes from cutscenes.xml. Only tests the basic appearance of each individual text box."), tr, tg, tb, true);
            break;
        case 5:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Limits check"), tr, tg, tb);
            graphics.PrintWrap( -1, 65, loc::gettext("Find translations that don't fit within their defined bounds."), tr, tg, tb, true);
            break;
        }
        break;
    case Menu::translator_options_limitscheck:
    {
        size_t of = loc::limitscheck_current_overflow;
        if (of >= loc::text_overflows.size())
        {
            int next_y;
            if (loc::text_overflows.empty())
            {
                next_y = graphics.PrintWrap(-1, 20, loc::gettext("No text overflows found!"), tr, tg, tb, true);
            }
            else
            {
                next_y = graphics.PrintWrap(-1, 20, loc::gettext("No text overflows left!"), tr, tg, tb, true);
            }

            graphics.PrintWrap(-1, next_y, loc::gettext("Note that this detection isn't perfect."), tr, tg, tb, true);
        }
        else
        {
            loc::TextOverflow& overflow = loc::text_overflows[of];

            char buffer[SCREEN_WIDTH_CHARS + 1];
            vformat_buf(buffer, sizeof(buffer),
                "{page}/{total}    {max_w}*{max_h} ({max_w_px}x{max_h_px})    [{lang}]",
                "page:int, total:int, max_w:int, max_h:int, max_w_px:int, max_h_px:int, lang:str",
                (int) of+1, (int) loc::text_overflows.size(),
                overflow.max_w, overflow.max_h,
                overflow.max_w_px, overflow.max_h_px,
                overflow.lang.c_str()
            );
            font::print(PR_FONT_8X8, 10, 10, buffer, tr/2, tg/2, tb/2);

            int box_x = SDL_min(10, (320-overflow.max_w_px)/2);
            graphics.fill_rect(box_x-1, 30-1, overflow.max_w_px+2, overflow.max_h_px+2, tr/3, tg/3, tb/3);

            int wraplimit;
            if (overflow.multiline)
            {
                wraplimit = overflow.max_w_px;
            }
            else
            {
                wraplimit = 320-box_x;
            }

            if (overflow.text != NULL)
            {
                font::print_wrap(overflow.flags, box_x, 30, overflow.text, tr, tg, tb, -1, wraplimit);
            }
        }
        break;
    }
    case Menu::translator_options_stats:
    {
        font::print(0, 16, 16, loc::get_langmeta()->nativename, tr, tg, tb);

        const char* line_template = "%4d";
        char buffer[5];
        int coldiv;

        #define stat_line(y, filename, untranslated_counter) \
            SDL_snprintf(buffer, sizeof(buffer), line_template, \
                untranslated_counter \
            ); \
            coldiv = untranslated_counter > 0 ? 1 : 2; \
            font::print(PR_FONT_8X8, 16, y, filename, tr/coldiv, tg/coldiv, tb/coldiv); \
            font::print(PR_FONT_8X8, 272, y, buffer, tr/coldiv, tg/coldiv, tb/coldiv)

        stat_line(48, "strings.xml", loc::n_untranslated[loc::UNTRANSLATED_STRINGS]);
        stat_line(64, "numbers.xml", loc::n_untranslated[loc::UNTRANSLATED_NUMBERS]);
        stat_line(80, "strings_plural.xml", loc::n_untranslated[loc::UNTRANSLATED_STRINGS_PLURAL]);
        stat_line(96, "cutscenes.xml", loc::n_untranslated[loc::UNTRANSLATED_CUTSCENES]);
        stat_line(112, "roomnames.xml", loc::n_untranslated_roomnames);
        stat_line(128, "roomnames_special.xml", loc::n_untranslated[loc::UNTRANSLATED_ROOMNAMES_SPECIAL]);

        #undef stat_line

        break;
    }
    case Menu::translator_options_exploregame:
        switch (game.currentmenuoption)
        {
        case 0:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Space Station 1"), tr, tg, tb);
            break;
        case 1:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("The Laboratory"), tr, tg, tb);
            break;
        case 2:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("The Tower"), tr, tg, tb);
            break;
        case 3:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Space Station 2"), tr, tg, tb);
            break;
        case 4:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("The Warp Zone"), tr, tg, tb);
            break;
        case 5:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Intermission 1"), tr, tg, tb);
            break;
        case 6:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Intermission 2"), tr, tg, tb);
            break;
        case 7:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("The Final Level"), tr, tg, tb);
            break;
        }
        if (roomname_translator::enabled)
        {
            if (game.currentmenuoption >= 0 && game.currentmenuoption < 8)
            {
                int names_left = loc::n_untranslated_roomnames_area[game.currentmenuoption + 1];
                int coldiv = names_left > 0 ? 1 : 2;

                char buffer[4*SCREEN_WIDTH_CHARS + 1];
                loc::gettext_plural_fill(
                    buffer, sizeof(buffer),
                    "{n} normal room names untranslated",
                    "{n} normal room name untranslated",
                    "n:int",
                    names_left
                );
                graphics.PrintWrap( -1, 65, buffer, tr/coldiv, tg/coldiv, tb/coldiv, true);
            }
        }
        else
        {
            graphics.PrintWrap( -1, 65, loc::gettext("You have not enabled room name translation mode!"), tr, tg, tb, true);
        }
        break;
    case Menu::translator_maintenance:
        switch (game.currentmenuoption)
        {
        case 0:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Sync language files"), tr, tg, tb);
            graphics.PrintWrap( -1, 65, loc::gettext("Merge all new strings from the template files into the translation files, keeping existing translations."), tr, tg, tb, true);
            break;
        case 1:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Statistics"), tr, tg, tb);
            graphics.PrintWrap( -1, 65, loc::gettext("Count the amount of untranslated strings for each language."), tr, tg, tb, true);
            break;
        case 2:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Limits check"), tr, tg, tb);
            graphics.PrintWrap( -1, 65, loc::gettext("Find translations that don't fit within their defined bounds."), tr, tg, tb, true);
        }
        break;
    case Menu::translator_maintenance_sync:
    {
        int next_y = graphics.PrintWrap(-1, 20, loc::gettext("If new strings were added to the English template language files, this feature will insert them in the translation files for all languages. Make a backup, just in case."), tr, tg, tb, true);

        font::print(PR_CEN, -1, next_y, loc::gettext("Full syncing EN→All:"), tr, tg, tb);
        next_y = font::print_wrap(PR_CEN | PR_FONT_8X8, -1, next_y+10, "meta.xml\nstrings.xml\nstrings_plural.xml\ncutscenes.xml\nroomnames.xml\nroomnames_special.xml", tr/2, tg/2, tb/2);

        font::print(PR_CEN, -1, next_y, loc::gettext("Syncing not supported:"), tr, tg, tb);
        font::print_wrap(PR_CEN | PR_FONT_8X8, -1, next_y+10, "numbers.xml", tr/2, tg/2, tb/2);
        break;
    }
    case Menu::translator_error_setlangwritedir:
        graphics.PrintWrap( -1, 95, loc::gettext("ERROR: Could not write to language folder! Make sure there is no \"lang\" folder next to the regular saves."), tr, tg, tb, true);
        break;
    case Menu::speedrunneroptions:
        switch (game.currentmenuoption)
        {
        case 0:
        {
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Glitchrunner Mode"), tr, tg, tb);
            int next_y = graphics.PrintWrap(-1, 65, loc::gettext("Re-enable glitches that existed in previous versions of the game."), tr, tg, tb, true);
            drawglitchrunnertext(next_y);
            break;
        }
        case 1:
        {
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Input Delay"), tr, tg, tb);
            int next_y = graphics.PrintWrap(-1, 65, loc::gettext("Re-enable the 1-frame input delay from previous versions of the game."), tr, tg, tb, true);
            if (game.inputdelay)
            {
                graphics.PrintWrap(-1, next_y, loc::gettext("Input delay is ON"), tr, tg, tb, true);
            }
            else
            {
                graphics.PrintWrap(-1, next_y, loc::gettext("Input delay is OFF"), tr / 2, tg / 2, tb / 2, true);
            }
            break;
        }
        case 2:
        {
            char buffer[SCREEN_WIDTH_CHARS + 1];
            const char* button;

            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Interact Button"), tr, tg, tb);
            int next_y = graphics.PrintWrap(-1, 65, loc::gettext("Toggle whether you interact with prompts using ENTER or E."), tr, tg, tb, true);

            if (game.separate_interact)
            {
                button = loc::gettext("E");
            }
            else
            {
                button = loc::gettext("ENTER");
            }

            vformat_buf(buffer, sizeof(buffer), loc::gettext("Interact button: {button}"), "button:str", button);
            graphics.PrintWrap(-1, next_y, buffer, tr, tg, tb, true);
            break;
        }
        case 3:
        {
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Fake Load Screen"), tr, tg, tb);
            int next_y = graphics.PrintWrap(-1, 65, loc::gettext("Disable the fake loading screen which appears on game launch."), tr, tg, tb, true);
            if (game.skipfakeload)
                graphics.PrintWrap(-1, next_y, loc::gettext("Fake loading screen is OFF"), tr / 2, tg / 2, tb / 2, true);
            else
                graphics.PrintWrap(-1, next_y, loc::gettext("Fake loading screen is ON"), tr, tg, tb, true);
            break;
        }
        case 4:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("In-Game Timer"), tr, tg, tb);
            int next_y = graphics.PrintWrap(-1, 65, loc::gettext("Toggle the in-game timer outside of time trials."), tr, tg, tb, true);
            if (game.showingametimer)
            {
                graphics.PrintWrap(-1, next_y, loc::gettext("In-Game Timer is ON"), tr, tg, tb, true);
            }
            else
            {
                graphics.PrintWrap(-1, next_y, loc::gettext("In-Game Timer is OFF"), tr / 2, tg / 2, tb / 2, true);
            }
            break;
        }
        break;
    case Menu::setglitchrunner:
    {
        font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Glitchrunner Mode"), tr, tg, tb);
        int next_y = graphics.PrintWrap(-1, 65, loc::gettext("Select a new glitchrunner version below."), tr, tg, tb, true);
        drawglitchrunnertext(next_y);
        break;
    }
    case Menu::advancedoptions:
        switch (game.currentmenuoption)
        {
        case 0:
        {
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Unfocus Pause"), tr, tg, tb);
            int next_y = graphics.PrintWrap( -1, 65, loc::gettext("Toggle if the game will pause when the window is unfocused."), tr, tg, tb, true);
            if (game.disablepause)
            {
                graphics.PrintWrap(-1, next_y, loc::gettext("Unfocus pause is OFF"), tr/2, tg/2, tb/2, true);
            }
            else
            {
                graphics.PrintWrap(-1, next_y, loc::gettext("Unfocus pause is ON"), tr, tg, tb, true);
            }
            break;
        }
        case 1:
        {
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Unfocus Audio"), tr, tg, tb);
            int next_y = graphics.PrintWrap(-1, 65, loc::gettext("Toggle if the audio will pause when the window is unfocused."), tr, tg, tb, true);
            if (game.disableaudiopause)
            {
                graphics.PrintWrap(-1, next_y, loc::gettext("Unfocus audio pause is OFF"), tr/2, tg/2, tb/2, true);
            }
            else
            {
                graphics.PrintWrap(-1, next_y, loc::gettext("Unfocus audio pause is ON"), tr, tg, tb, true);
            }
            break;
        }
        case 2:
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Room Name BG"), tr, tg, tb);
            int next_y = graphics.PrintWrap( -1, 65, loc::gettext("Lets you see through what is behind the name at the bottom of the screen."), tr, tg, tb, true);
            if (graphics.translucentroomname)
                graphics.PrintWrap(-1, next_y, loc::gettext("Room name background is TRANSLUCENT"), tr/2, tg/2, tb/2, true);
            else
                graphics.PrintWrap(-1, next_y, loc::gettext("Room name background is OPAQUE"), tr, tg, tb, true);
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
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Unlock Play Modes"), tr, tg, tb);
            graphics.PrintWrap(-1, 65, loc::gettext("Unlock parts of the game normally unlocked as you progress."), tr, tg, tb, true);
            break;
#endif
        case OFFSET+0:
        {
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Invincibility"), tr, tg, tb);
            int next_y = graphics.PrintWrap(-1, 65, loc::gettext("Explore the game freely without dying. (Can cause glitches.)"), tr, tg, tb, true);
            if (map.invincibility)
            {
                graphics.PrintWrap(-1, next_y, loc::gettext("Invincibility is ON."), tr, tg, tb, true);
            }
            else
            {
                graphics.PrintWrap(-1, next_y, loc::gettext("Invincibility is OFF."), tr / 2, tg / 2, tb / 2, true);
            }
            break;
        }
        case OFFSET+1:
        {
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Slowdown"), tr, tg, tb);
            int next_y = graphics.PrintWrap(-1, 65, loc::gettext("Reduce the game speed."), tr, tg, tb, true);
            drawslowdowntext(next_y);
            break;
        }
        case OFFSET+2:
        {
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Backgrounds"), tr, tg, tb);
            int next_y = graphics.PrintWrap(-1, 65, loc::gettext("Disable animated backgrounds in menus and during gameplay."), tr, tg, tb, true);
            if (!game.colourblindmode)
            {
                graphics.PrintWrap(-1, next_y, loc::gettext("Backgrounds are ON."), tr, tg, tb, true);
            }
            else
            {
                graphics.PrintWrap(-1, next_y, loc::gettext("Backgrounds are OFF."), tr / 2, tg / 2, tb / 2, true);
            }
            break;
        }
        case OFFSET+3:
        {
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Screen Effects"), tr, tg, tb);
            int next_y = graphics.PrintWrap(-1, 65, loc::gettext("Disables screen shakes and flashes."), tr, tg, tb, true);
            if (!game.noflashingmode)
            {
                graphics.PrintWrap(-1, next_y, loc::gettext("Screen Effects are ON."), tr, tg, tb, true);
            }
            else
            {
                graphics.PrintWrap(-1, next_y, loc::gettext("Screen Effects are OFF."), tr / 2, tg / 2, tb / 2, true);
            }
            break;
        }
        case OFFSET+4:
        {
            const char* text;

            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Text Outline"), tr, tg, tb);
            int next_y = graphics.PrintWrap(-1, 65, loc::gettext("Disables outline on game text."), tr, tg, tb, true);

            graphics.fill_rect(0, next_y-4, 320, 16, tr, tg, tb);

            if (!graphics.notextoutline)
            {
                text = loc::gettext("Text outlines are ON.");
            }
            else
            {
                text = loc::gettext("Text outlines are OFF.");
            }

            font::print(PR_BOR | PR_CEN, -1, next_y, text, 255, 255, 255);
            break;
        }

        }
        break;

#undef OFFSET
    }
    case Menu::playint1:
    case Menu::playint2:
        graphics.PrintWrap( -1, 65, loc::gettext("Who do you want to play the level with?"), tr, tg, tb, true);
        break;
    case Menu::playmodes:
        switch (game.currentmenuoption)
        {
        case 0:
        {
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Time Trials"), tr, tg, tb);
            int next_y = graphics.PrintWrap( -1, 65, loc::gettext("Replay any level in the game in a competitive time trial mode."), tr, tg, tb, true);

            if (game.nocompetitive())
            {
                graphics.PrintWrap( -1, next_y, loc::gettext("Time Trials are not available with slowdown or invincibility."), tr, tg, tb, true);
            }
            break;
        }
        case 1:
        {
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Intermissions"), tr, tg, tb);
            int next_y = graphics.PrintWrap( -1, 65, loc::gettext("Replay the intermission levels."), tr, tg, tb, true);

            if (!game.unlock[15] && !game.unlock[16])
            {
                graphics.PrintWrap( -1, next_y, loc::gettext("TO UNLOCK: Complete the intermission levels in-game."), tr, tg, tb, true);
            }
            break;
        }
        case 2:
        {
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("No Death Mode"), tr, tg, tb);
            int next_y = graphics.PrintWrap( -1, 65, loc::gettext("Play the entire game without dying once."), tr, tg, tb, true);

            if (game.nocompetitive())
            {
                graphics.PrintWrap( -1, next_y, loc::gettext("No Death Mode is not available with slowdown or invincibility."), tr, tg, tb, true);
            }
            else if (!game.unlock[17])
            {
                graphics.PrintWrap( -1, next_y, loc::gettext("TO UNLOCK: Achieve an S-rank or above in at least 4 time trials."), tr, tg, tb, true);
            }
            break;
        }
        case 3:
            // WARNING: Partially duplicated in Menu::options
            font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Flip Mode"), tr, tg, tb);
            int next_y = graphics.PrintWrap( -1, 65, loc::gettext("Flip the entire game vertically. Compatible with other game modes."), tr, tg, tb, true);

            if (game.unlock[18])
            {
                if (graphics.setflipmode)
                {
                    graphics.PrintWrap( -1, next_y, loc::gettext("Currently ENABLED!"), tr, tg, tb, true);
                }
                else
                {
                    graphics.PrintWrap( -1, next_y, loc::gettext("Currently Disabled."), tr/2, tg/2, tb/2, true);
                }
            }
            else
            {
                graphics.PrintWrap( -1, next_y, loc::gettext("TO UNLOCK: Complete the game."), tr, tg, tb, true);
            }
            break;
        }
        break;
    case Menu::youwannaquit:
        graphics.PrintWrap( -1, 75, loc::gettext("Are you sure you want to quit?"), tr, tg, tb, true);
        break;
    case Menu::continuemenu:
        switch (game.currentmenuoption)
        {
        case 0:
        {
            //Show teleporter save info
            graphics.drawpixeltextbox(17, 65-20, 286, 90, 65, 185, 207);

            font::print(PR_2X | PR_CEN, -1, 20, loc::gettext("Tele Save"), tr, tg, tb);
            font::print(PR_CEN, -1, 80-20, game.tele_currentarea, 25, 255 - (help.glow / 2), 255 - (help.glow / 2));
            for (int i = 0; i < 6; i++)
            {
                graphics.drawcrewman(169-(3*42)+(i*42), 95-20, i, game.tele_crewstats[i], true);
            }
            font::print(0, 59, 132-20, game.tele_gametime, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
            char buffer[SCREEN_WIDTH_CHARS + 1];
            vformat_buf(buffer, sizeof(buffer),
                loc::gettext("{savebox_n_trinkets|wordy}"),
                "savebox_n_trinkets:int",
                game.tele_trinkets
            );
            font::print(PR_RIGHT, 262, 132-20, buffer, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));

            graphics.draw_sprite(34, 126-20, 50, graphics.col_clock);
            graphics.draw_sprite(270, 126-20, 22, graphics.col_trinket);
            break;
        }
        case 1:
        {
            //Show quick save info
            graphics.drawpixeltextbox(17, 65-20, 286, 90, 65, 185, 207);

            font::print(PR_2X | PR_CEN, -1, 20, loc::gettext("Quick Save"), tr, tg, tb);
            font::print(PR_CEN, -1, 80-20, game.quick_currentarea, 25, 255 - (help.glow / 2), 255 - (help.glow / 2));
            for (int i = 0; i < 6; i++)
            {
                graphics.drawcrewman(169-(3*42)+(i*42), 95-20, i, game.quick_crewstats[i], true);
            }
            font::print(0, 59, 132-20, game.quick_gametime, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
            char buffer[SCREEN_WIDTH_CHARS + 1];
            vformat_buf(buffer, sizeof(buffer),
                loc::gettext("{savebox_n_trinkets|wordy}"),
                "savebox_n_trinkets:int",
                game.quick_trinkets
            );
            font::print(PR_RIGHT, 262, 132-20, buffer, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));

            graphics.draw_sprite(34, 126-20, 50, graphics.col_clock);
            graphics.draw_sprite(270, 126-20, 22, graphics.col_trinket);
            break;
        }
        }
        break;
    case Menu::gameover:
    case Menu::gameover2:
    {
        font::print(PR_3X | PR_CEN, -1, 25, loc::gettext("GAME OVER"), tr, tg, tb);

        for (size_t i = 0; i < SDL_arraysize(game.ndmresultcrewstats); i++)
        {
            graphics.drawcrewman(169-(3*42)+(i*42), 68, i, game.ndmresultcrewstats[i], true);
        }
        char buffer[2*SCREEN_WIDTH_CHARS + 1];
        loc::gettext_plural_fill(
            buffer, sizeof(buffer),
            "You rescued {n_crew|wordy} crewmates",
            "You rescued {n_crew|wordy} crewmate",
            "n_crew:int",
            game.ndmresultcrewrescued
        );
        font::print(PR_CEN, -1, 100, buffer, tr, tg, tb);

        loc::gettext_plural_fill(
            buffer, sizeof(buffer),
            "and found {n_trinkets|wordy} trinkets.",
            "and found {n_trinkets|wordy} trinket.",
            "n_trinkets:int",
            game.ndmresulttrinkets
        );
        font::print_wrap(PR_CEN | PR_CJK_LOW, -1, 110, buffer, tr, tg, tb);

        font::print(PR_CEN, -1, 145, loc::gettext("You managed to reach:"), tr, tg, tb);
        font::print(PR_CEN | PR_CJK_LOW, -1, 155, game.ndmresulthardestroom, tr, tg, tb);

        const char* encouragement;
        switch (game.ndmresultcrewrescued)
        {
        case 1:
            encouragement = loc::gettext("Keep trying! You'll get there!");
            break;
        case 2:
            encouragement = loc::gettext("Nice one!");
            break;
        case 3:
            encouragement = loc::gettext("Wow! Congratulations!");
            break;
        case 4:
            encouragement = loc::gettext("Incredible!");
            break;
        case 5:
            encouragement = loc::gettext("Unbelievable! Well done!");
            break;
        default:
            encouragement = loc::gettext("Er, how did you do that?");
            break;
        }

        graphics.PrintWrap(0, 190, encouragement, tr, tg, tb, true);
        break;
    }
    case Menu::nodeathmodecomplete:
    case Menu::nodeathmodecomplete2:
    {
        font::print(PR_4X | PR_CEN | PR_CJK_LOW, -1, 8, loc::gettext("WOW"), tr, tg, tb);

        for (size_t i = 0; i < SDL_arraysize(game.ndmresultcrewstats); i++)
        {
            graphics.drawcrewman(169-(3*42)+(i*42), 68, i, game.ndmresultcrewstats[i], true);
        }
        font::print(PR_CEN, -1, 100, loc::gettext("You rescued all the crewmates!"), tr, tg, tb);

        char buffer[3*SCREEN_WIDTH_CHARS + 1];
        loc::gettext_plural_fill(
            buffer, sizeof(buffer),
            "And you found {n_trinkets|wordy} trinkets.",
            "And you found {n_trinkets|wordy} trinket.",
            "n_trinkets:int",
            game.ndmresulttrinkets
        );
        font::print_wrap(PR_CEN | PR_CJK_LOW, -1, 110, buffer, tr, tg, tb);

        graphics.PrintWrap(0, 160, loc::gettext("A new trophy has been awarded and placed in the secret lab to acknowledge your achievement!"), tr, tg, tb, true);
        break;
    }
    case Menu::timetrialcomplete:
    case Menu::timetrialcomplete2:
    case Menu::timetrialcomplete3:
    {
        font::print(PR_3X | PR_CEN, -1, 20, loc::gettext("Results"), tr, tg, tb);

        std::string tempstring = game.resulttimestring() + loc::gettext(" / ") + game.timetstring(game.timetrialresultpar) + loc::gettext(".99");

        graphics.drawspritesetcol(22, 80-15, 50, 22);
        font::print(PR_CJK_HIGH, 49, 80-15, loc::gettext("TIME TAKEN:"), 255, 255, 255);
        font::print(PR_CJK_LOW, 49, 90-15, tempstring, tr, tg, tb);
        if (game.timetrialresulttime <= game.timetrialresultpar)
        {
            font::print(PR_RIGHT | PR_CJK_LOW, 292, 90-15, loc::gettext("+1 Rank!"), 255, 255, 255);
        }

        tempstring = help.String(game.timetrialresultdeaths);
        graphics.drawspritesetcol(22-4, 80+20-4, 12, 22);
        font::print(PR_CJK_HIGH, 49, 80+20, loc::gettext("NUMBER OF DEATHS:"), 255, 255, 255);
        font::print(PR_CJK_LOW, 49, 90+20, tempstring, tr, tg, tb);
        if (game.timetrialresultdeaths == 0)
        {
            font::print(PR_RIGHT | PR_CJK_LOW, 292, 90+20, loc::gettext("+1 Rank!"), 255, 255, 255);
        }

        char buffer[SCREEN_WIDTH_CHARS + 1];
        vformat_buf(
            buffer, sizeof(buffer),
            loc::gettext("{n_trinkets} of {max_trinkets}"),
            "n_trinkets:int, max_trinkets:int",
            game.timetrialresulttrinkets, game.timetrialresultshinytarget
        );
        graphics.drawspritesetcol(22, 80+55, 22, 22);
        font::print(PR_CJK_HIGH, 49, 80+55, loc::gettext("SHINY TRINKETS:"), 255, 255, 255);
        font::print(PR_CJK_LOW, 49, 90+55, buffer, tr, tg, tb);
        if (game.timetrialresulttrinkets >= game.timetrialresultshinytarget)
        {
            font::print(PR_RIGHT | PR_CJK_LOW, 292, 90+55, loc::gettext("+1 Rank!"), 255, 255, 255);
        }

        const char* rank = "";
        switch(game.timetrialrank)
        {
        case 0:
            rank = loc::gettext("B");
            break;
        case 1:
            rank = loc::gettext("A");
            break;
        case 2:
            rank = loc::gettext("S");
            break;
        case 3:
            rank = loc::gettext("V");
            break;
        }
        int rankw = font::len(PR_4X, rank);
        int ranktextw = font::len(PR_2X, loc::gettext("Rank:")) + 16 + rankw;
        int ranktextx = (320-ranktextw)/2;
        int rankx = ranktextx + ranktextw - rankw;
        if (game.currentmenuname == Menu::timetrialcomplete2 || game.currentmenuname == Menu::timetrialcomplete3)
        {
            font::print(PR_2X, ranktextx, 175, loc::gettext("Rank:"), tr, tg, tb);
        }

        if (game.currentmenuname == Menu::timetrialcomplete3)
        {
            font::print(PR_4X, rankx, 165, rank, 255, 255, 255);
        }
        break;
    }
    case Menu::unlockmenutrials:
        font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Unlock Time Trials"), tr, tg, tb);
        graphics.PrintWrap( -1, 65, loc::gettext("You can unlock each time trial separately."), tr, tg, tb, true);
        break;
    case Menu::timetrials:
    {
        bool unlocked = false;
        int id_trial = game.currentmenuoption;
        int par;
        int max_trinkets;

        switch (game.currentmenuoption)
        {
        case 0:
            if(game.unlock[9])
            {
                font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Space Station 1"), tr, tg, tb);
                unlocked = true;
                par = 75;
                max_trinkets = 2;
            }
            else
            {
                font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("???"), tr, tg, tb);
                font::print(PR_CEN, -1, 60, loc::gettext("TO UNLOCK:"), tr, tg, tb);
                font::print(PR_CEN, -1, 75, loc::gettext("Rescue Violet"), tr, tg, tb);
                font::print(PR_CEN, -1, 87, loc::gettext("Find three trinkets"), tr, tg, tb);
            }
            break;
        case 1:
            if(game.unlock[10])
            {
                font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("The Laboratory"), tr, tg, tb);
                unlocked = true;
                par = 165;
                max_trinkets = 4;
            }
            else
            {
                font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("???"), tr, tg, tb);
                font::print(PR_CEN, -1, 60, loc::gettext("TO UNLOCK:"), tr, tg, tb);
                font::print(PR_CEN, -1, 75, loc::gettext("Rescue Victoria"), tr, tg, tb);
                font::print(PR_CEN, -1, 87, loc::gettext("Find six trinkets"), tr, tg, tb);
            }
            break;
        case 2:
            if(game.unlock[11])
            {
                font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("The Tower"), tr, tg, tb);
                unlocked = true;
                par = 105;
                max_trinkets = 2;
            }
            else
            {
                font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("???"), tr, tg, tb);
                font::print(PR_CEN, -1, 60, loc::gettext("TO UNLOCK:"), tr, tg, tb);
                font::print(PR_CEN, -1, 75, loc::gettext("Rescue Vermilion"), tr, tg, tb);
                font::print(PR_CEN, -1, 87, loc::gettext("Find nine trinkets"), tr, tg, tb);
            }
            break;
        case 3:
            if(game.unlock[12])
            {
                font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Space Station 2"), tr, tg, tb);
                unlocked = true;
                par = 200;
                max_trinkets = 5;
            }
            else
            {
                font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("???"), tr, tg, tb);
                font::print(PR_CEN, -1, 60, loc::gettext("TO UNLOCK:"), tr, tg, tb);
                font::print(PR_CEN, -1, 75, loc::gettext("Rescue Vitellary"), tr, tg, tb);
                font::print(PR_CEN, -1, 87, loc::gettext("Find twelve trinkets"), tr, tg, tb);
            }
            break;
        case 4:
            if(game.unlock[13])
            {
                font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("The Warp Zone"), tr, tg, tb);
                unlocked = true;
                par = 120;
                max_trinkets = 1;
            }
            else
            {
                font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("???"), tr, tg, tb);
                font::print(PR_CEN, -1, 60, loc::gettext("TO UNLOCK:"), tr, tg, tb);
                font::print(PR_CEN, -1, 75, loc::gettext("Rescue Verdigris"), tr, tg, tb);
                font::print(PR_CEN, -1, 87, loc::gettext("Find fifteen trinkets"), tr, tg, tb);
            }
            break;
        case 5:
            if(game.unlock[14])
            {
                font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("The Final Level"), tr, tg, tb);
                unlocked = true;
                par = 135;
                max_trinkets = 1;
            }
            else
            {
                font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("???"), tr, tg, tb);
                font::print(PR_CEN, -1, 60, loc::gettext("TO UNLOCK:"), tr, tg, tb);
                font::print(PR_CEN, -1, 75, loc::gettext("Complete the game"), tr, tg, tb);
                font::print(PR_CEN, -1, 87, loc::gettext("Find eighteen trinkets"), tr, tg, tb);
            }
            break;
        }

        if (unlocked)
        {
            if (game.besttimes[id_trial] == -1)
            {
                graphics.PrintWrap( -1, 75, loc::gettext("Not yet attempted"), tr, tg, tb, true);
            }
            else
            {
                int sp = SDL_max(10, font::height(0));

                font::print(0, 32, 65, loc::gettext("RECORDS"), tr, tg, tb);
                const char* label = loc::gettext("TIME");
                int label_len = font::len(0, label);
                font::print(0, 32, 65+sp, label, tr, tg, tb);
                label = loc::gettext("SHINY");
                label_len = SDL_max(label_len, font::len(0, label));
                font::print(0, 32, 65+sp*2, label, tr, tg, tb);
                label = loc::gettext("LIVES");
                label_len = SDL_max(label_len, font::len(0, label));
                font::print(0, 32, 65+sp*3, label, tr, tg, tb);

                font::print(0, label_len+48, 65+sp, game.timetstring(game.besttimes[id_trial]), tr, tg, tb);

                char buffer[SCREEN_WIDTH_CHARS + 1];
                vformat_buf(
                    buffer, sizeof(buffer),
                    loc::gettext("{n_trinkets}/{max_trinkets}"),
                    "n_trinkets:int, max_trinkets:int",
                    game.besttrinkets[id_trial], max_trinkets
                );
                font::print(0, label_len+48, 65+sp*2, buffer, tr, tg, tb);
                font::print(0, label_len+48, 65+sp*3, help.String(game.bestlives[id_trial]), tr, tg, tb);


                const char* str_par_time = loc::gettext("PAR TIME");
                const std::string par_time = game.timetstring(par);
                const char* str_best_rank = loc::gettext("BEST RANK");
                const char* rank;
                switch(game.bestrank[id_trial])
                {
                case 0:
                    rank = loc::gettext("B");
                    break;
                case 1:
                    rank = loc::gettext("A");
                    break;
                case 2:
                    rank = loc::gettext("S");
                    break;
                case 3:
                    rank = loc::gettext("V");
                    break;
                default:
                    rank = "?";
                }

                int w[4] = {
                    font::len(0, str_par_time),
                    font::len(0, par_time),
                    font::len(0, str_best_rank),
                    font::len(PR_2X, rank)
                };
                int longest_w = 0;
                for (size_t i = 0; i < 4; i++)
                {
                    if (w[i] > longest_w)
                    {
                        longest_w = w[i];
                    }
                }
                int center_x = 288 - longest_w/2;

                font::print(PR_CEN, center_x, 65, str_par_time, tr, tg, tb);
                font::print(PR_CEN, center_x, 65+sp, par_time, tr, tg, tb);
                font::print(PR_CEN, center_x, 65+sp*3, str_best_rank, tr, tg, tb);
                font::print(
                    PR_2X | PR_CEN,
                    center_x,
                    66+sp*4,
                    rank,
                    225, 225, 225
                );
            }
        }

        break;
    }
    case Menu::gamecompletecontinue:
        font::print(PR_2X | PR_CEN | PR_CJK_HIGH, -1, 25, loc::gettext("Congratulations!"), tr, tg, tb);

        graphics.PrintWrap( -1, 45, loc::gettext("Your save files have been updated."), tr, tg, tb, true);

        graphics.PrintWrap( -1, 110, loc::gettext("If you want to keep exploring the game, select CONTINUE from the play menu."), tr, tg, tb, true);
        break;
    case Menu::unlockmenu:
        font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Unlock Play Modes"), tr, tg, tb);

        graphics.PrintWrap( -1, 65, loc::gettext("From here, you may unlock parts of the game that are normally unlocked as you play."), tr, tg, tb, true);
        break;
    case Menu::unlocktimetrial:
        font::print(PR_2X | PR_CEN, -1, 45, loc::gettext("Congratulations!"), tr, tg, tb);

        graphics.PrintWrap( -1, 125, loc::gettext("You have unlocked a new Time Trial."), tr, tg, tb, true);
        break;
    case Menu::unlocktimetrials:
        font::print(PR_2X | PR_CEN, -1, 45, loc::gettext("Congratulations!"), tr, tg, tb);

        graphics.PrintWrap( -1, 125, loc::gettext("You have unlocked some new Time Trials."), tr, tg, tb, true);
        break;
    case Menu::unlocknodeathmode:
        font::print(PR_2X | PR_CEN, -1, 45, loc::gettext("Congratulations!"), tr, tg, tb);

        graphics.PrintWrap( -1, 125, loc::gettext("You have unlocked No Death Mode."), tr, tg, tb, true);
        break;
    case Menu::unlockflipmode:
        font::print(PR_2X | PR_CEN, -1, 45, loc::gettext("Congratulations!"), tr, tg, tb);

        graphics.PrintWrap( -1, 125, loc::gettext("You have unlocked Flip Mode."), tr, tg, tb, true);
        break;
    case Menu::unlockintermission:
        font::print(PR_2X | PR_CEN, -1, 45, loc::gettext("Congratulations!"), tr, tg, tb);

        graphics.PrintWrap( -1, 125, loc::gettext("You have unlocked the intermission levels."), tr, tg, tb, true);
        break;
    case Menu::playerworlds:
        graphics.PrintWrap(-1, 180, loc::gettext("To install new player levels, copy the .vvvvvv files to the levels folder."), tr, tg, tb, true);
        break;
    case Menu::confirmshowlevelspath:
        graphics.PrintWrap(-1, 80, loc::gettext("Are you sure you want to show the levels path? This may reveal sensitive information if you are streaming."), tr, tg, tb, true);
        break;
    case Menu::showlevelspath:
    {
        int next_y = graphics.PrintWrap(-1, 40, loc::gettext("The levels path is:"), tr, tg, tb, true);
        graphics.PrintWrap(0, next_y, FILESYSTEM_getUserLevelDirectory(), tr, tg, tb, false, 10, 320);
        break;
    }
    case Menu::errorsavingsettings:
        graphics.PrintWrap( -1, 95, loc::gettext("ERROR: Could not save settings file!"), tr, tg, tb, true);
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
        font::print(PR_2X | PR_CEN, -1, 45, loc::gettext("ERROR"), tr, tg, tb);
        graphics.PrintWrap(-1, 65, str, tr, tg, tb, true);
        break;
    }
    case Menu::warninglevellist:
        font::print(PR_2X | PR_CEN, -1, 45, loc::gettext("WARNING"), tr, tg, tb);
        graphics.PrintWrap(-1, 65, FILESYSTEM_getLevelDirError(), tr, tg, tb, true);
        break;
    default:
        break;
    }
}

void titlerender(void)
{
    graphics.clear();
    if (!game.menustart)
    {
        tr = graphics.col_tr;
        tg = graphics.col_tg;
        tb = graphics.col_tb;

        int temp = 50;
        graphics.draw_sprite((160 - 96) + 0 * 32, temp, 23, tr, tg, tb);
        graphics.draw_sprite((160 - 96) + 1 * 32, temp, 23, tr, tg, tb);
        graphics.draw_sprite((160 - 96) + 2 * 32, temp, 23, tr, tg, tb);
        graphics.draw_sprite((160 - 96) + 3 * 32, temp, 23, tr, tg, tb);
        graphics.draw_sprite((160 - 96) + 4 * 32, temp, 23, tr, tg, tb);
        graphics.draw_sprite((160 - 96) + 5 * 32, temp, 23, tr, tg, tb);
#if defined(MAKEANDPLAY)
        font::print(PR_RIGHT, 264, temp+35, loc::gettext("MAKE AND PLAY EDITION"), tr, tg, tb);
#endif

        graphics.PrintWrap(5, 175, loc::gettext("[ Press ACTION to Start ]"), tr, tg, tb, true);
        graphics.PrintWrap(5, 195, loc::gettext("ACTION = Space, Z, or V"), int(tr*0.5f), int(tg*0.5f), int(tb*0.5f), true);
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
        graphics.drawmenu(tr, tg, tb, game.currentmenuname);
    }

    graphics.drawfade();

    graphics.renderwithscreeneffects();
}

void gamecompleterender(void)
{
    graphics.clear();

    if(!game.colourblindmode) graphics.drawtowerbackground(graphics.titlebg);

    tr = graphics.col_tr;
    tg = graphics.col_tg;
    tb = graphics.col_tb;


    //rendering starts... here!

    int position = graphics.lerp(game.oldcreditposition, game.creditposition);
    if (graphics.onscreen(220 + position))
    {
        int temp = 220 + position;
        graphics.draw_sprite((160 - 96) + 0 * 32, temp, 23, tr, tg, tb);
        graphics.draw_sprite((160 - 96) + 1 * 32, temp, 23, tr, tg, tb);
        graphics.draw_sprite((160 - 96) + 2 * 32, temp, 23, tr, tg, tb);
        graphics.draw_sprite((160 - 96) + 3 * 32, temp, 23, tr, tg, tb);
        graphics.draw_sprite((160 - 96) + 4 * 32, temp, 23, tr, tg, tb);
        graphics.draw_sprite((160 - 96) + 5 * 32, temp, 23, tr, tg, tb);
    }

    if (graphics.onscreen(290 + position)) font::print(PR_2X | PR_CEN, -1, 290 + position, loc::gettext("Starring"), tr, tg, tb);

    if (graphics.onscreen(320 + position))
    {
        graphics.drawcrewman(70, 320 + position, 0, true);
        font::print(0, 100, 330 + position, loc::gettext("Captain Viridian"), tr, tg, tb);
    }
    if (graphics.onscreen(350 + position))
    {
        graphics.drawcrewman(70, 350 + position, 1, true);
        font::print(0, 100, 360 + position, loc::gettext("Doctor Violet"), tr, tg, tb);
    }
    if (graphics.onscreen(380 + position))
    {
        graphics.drawcrewman(70, 380 + position, 2, true);
        font::print(0, 100, 390 + position, loc::gettext("Professor Vitellary"), tr, tg, tb);
    }
    if (graphics.onscreen(410 + position))
    {
        graphics.drawcrewman(70, 410 + position, 3, true);
        font::print(0, 100, 420 + position, loc::gettext("Officer Vermilion"), tr, tg, tb);
    }
    if (graphics.onscreen(440 + position))
    {
        graphics.drawcrewman(70, 440 + position, 4, true);
        font::print(0, 100, 450 + position, loc::gettext("Chief Verdigris"), tr, tg, tb);
    }
    if (graphics.onscreen(470 + position))
    {
        graphics.drawcrewman(70, 470 + position, 5, true);
        font::print(0, 100, 480 + position, loc::gettext("Doctor Victoria"), tr, tg, tb);
    }

    if (graphics.onscreen(520 + position)) font::print(PR_3X | PR_CEN, -1, 520 + position, loc::gettext("Credits"), tr, tg, tb);

    if (graphics.onscreen(560 + position))
    {
        font::print(PR_CJK_HIGH, 40, 560 + position, loc::gettext("Created by"), tr, tg, tb);
        font::print(PR_2X | PR_FONT_8X8, 60, 570 + position, "Terry Cavanagh", tr, tg, tb);
    }

    if (graphics.onscreen(600 + position))
    {
        font::print(PR_CJK_HIGH, 40, 600 + position, loc::gettext("With Music by"), tr, tg, tb);
        font::print(PR_2X | PR_FONT_8X8, 60, 610 + position, "Magnus Pålsson", tr, tg, tb);
    }

    if (graphics.onscreen(640 + position))
    {
        font::print(PR_CJK_HIGH, 40, 640 + position, loc::gettext("Rooms Named by"), tr, tg, tb);
        font::print(PR_2X | PR_FONT_8X8, 60, 650 + position, "Bennett Foddy", tr, tg, tb);
    }

    if (graphics.onscreen(710 + position))
    {
        font::print(PR_CJK_HIGH, 40, 680 + position, loc::gettext("C++ Port by"), tr, tg, tb);
        font::print(PR_2X | PR_FONT_8X8, 60, 690 + position, "Simon Roth", tr, tg, tb);
        font::print(PR_2X | PR_FONT_8X8, 60, 710 + position, "Ethan Lee", tr, tg, tb);
        font::print(PR_2X | PR_FONT_8X8, 60, 730 + position, "Misa Kai", tr, tg, tb);
    }


    if (graphics.onscreen(770 + position))
    {
        font::print(PR_CJK_HIGH, 40, 760 + position, loc::gettext("Beta Testing by"), tr, tg, tb);
        font::print(PR_2X | PR_FONT_8X8, 60, 770 + position, "Sam Kaplan", tr, tg, tb);
        font::print(PR_2X | PR_FONT_8X8, 60, 790 + position, "Pauli Kohberger", tr, tg, tb);
    }

    if (graphics.onscreen(820 + position))
    {
        font::print(PR_CJK_HIGH, 40, 820 + position, loc::gettext("Ending Picture by"), tr, tg, tb);
        font::print(PR_2X | PR_FONT_8X8, 60, 830 + position, "Pauli Kohberger", tr, tg, tb);
    }

    if (graphics.onscreen(910 + position)) font::print(PR_3X | PR_CEN, -1, 890 + position, loc::gettext("Patrons"), tr, tg, tb);

    int creditOffset = 950;

    for (size_t i = 0; i < SDL_arraysize(Credits::superpatrons); i += 1)
    {
        if (graphics.onscreen(creditOffset + position))
        {
            font::print(PR_CEN | PR_FONT_8X8, -1, creditOffset + position, Credits::superpatrons[i], tr, tg, tb);
        }
        creditOffset += 10;
    }

    creditOffset += 10;
    if (graphics.onscreen(creditOffset + position)) font::print(PR_CEN, -1, creditOffset + position, loc::gettext("and"), tr, tg, tb);
    creditOffset += 20;

    for (size_t i = 0; i < SDL_arraysize(Credits::patrons); i += 1)
    {
        if (graphics.onscreen(creditOffset + position))
        {
            font::print(PR_CEN | PR_FONT_8X8, -1, creditOffset + position, Credits::patrons[i], tr, tg, tb);
        }
        creditOffset += 10;
    }

    creditOffset += 20;
    if (graphics.onscreen(creditOffset + position)) font::print(PR_2X | PR_CEN, -1, creditOffset + position, loc::gettext("GitHub Contributors"), tr, tg, tb);
    creditOffset += 30;

    for (size_t i = 0; i < SDL_arraysize(Credits::githubfriends); i += 1)
    {
        if (graphics.onscreen(creditOffset + position))
        {
            font::print(PR_CEN | PR_FONT_8X8, -1, creditOffset + position, Credits::githubfriends[i], tr, tg, tb);
        }
        creditOffset += 10;
    }

    creditOffset += 140;
    if (graphics.onscreen(creditOffset + position))
    {
        font::print(PR_2X | PR_CEN | PR_CJK_HIGH, -1, creditOffset + position, loc::gettext("Thanks for"), tr, tg, tb);
        creditOffset += 20;
        font::print(PR_2X | PR_CEN | PR_CJK_LOW, -1, creditOffset + position, loc::gettext("playing!"), tr, tg, tb);
    }

    graphics.drawfade();

    graphics.render();
}

void gamecompleterender2(void)
{
    graphics.clear();

    graphics.drawimage(IMAGE_ENDING, 0, 0);

    for (int j = 0; j < 30; j++)
    {
        for (int i = 0; i < 40; i++)
        {
            if (j == game.creditposy)
            {
                if (i > game.creditposx)
                {
                    graphics.fill_rect(i * 8, j * 8, 8, 8, 0, 0, 0);
                }
            }

            if (j > game.creditposy)
            {
                graphics.fill_rect(i * 8, j * 8, 8, 8, 0, 0, 0);
            }
        }
    }

    graphics.fill_rect(graphics.lerp(game.oldcreditposx * 8, game.creditposx * 8) + 8, game.creditposy * 8, 8, 8, 0, 0, 0);

    graphics.drawfade();

    graphics.render();
}

static const char* interact_prompt(
    char* buffer,
    const size_t buffer_size,
    const char* raw
) {
    const char* button;

    if (game.separate_interact)
    {
        button = loc::gettext("E");
    }
    else
    {
        button = loc::gettext("ENTER");
    }

    vformat_buf(buffer, buffer_size, raw, "button:str", button);

    return buffer;
}

void gamerender(void)
{
    graphics.set_render_target(graphics.gameplayTexture);
    graphics.set_color(0, 0, 0, 255);

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
                graphics.clear();
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
                graphics.clear();
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

    if (graphics.fademode == FADE_NONE
    && !game.intimetrial
    && !game.isingamecompletescreen()
    && (!game.swnmode || game.swngame != 1)
    && game.showingametimer
    && !roomname_translator::enabled)
    {
        const char* tempstring = loc::gettext("TIME:");
        int label_len = font::len(0, tempstring);
        font::print(PR_BOR, 6, 6, tempstring, 255,255,255);
        char buffer[SCREEN_WIDTH_CHARS + 1];
        game.timestringcenti(buffer, sizeof(buffer));
        font::print(PR_BOR, 6+label_len, 6, buffer, 196,196,196);
    }

    bool force_roomname_hidden = false;
    bool roomname_untranslated = false;
    int roomname_r = 196, roomname_g = 196, roomname_b = 255 - help.glow;
    if (roomname_translator::enabled)
    {
        roomname_translator::overlay_render(
            &force_roomname_hidden,
            &roomname_untranslated,
            &roomname_r, &roomname_g, &roomname_b
        );
    }

    if ((map.extrarow==0 || (map.custommode && map.roomname[0] != '\0')) && !force_roomname_hidden)
    {
        const char* roomname;

        if (map.finalmode)
        {
            roomname = loc::gettext_roomname(map.custommode, game.roomx, game.roomy, map.glitchname, map.roomname_special);
        }
        else
        {
            roomname = loc::gettext_roomname(map.custommode, game.roomx, game.roomy, map.roomname, map.roomname_special);
        }

        graphics.render_roomname(
            roomname_untranslated ? PR_FONT_8X8 : PR_FONT_LEVEL,
            roomname,
            roomname_r, roomname_g, roomname_b
        );
    }

    if (map.roomtexton)
    {
        //Draw room text!
        for (size_t i = 0; i < map.roomtext.size(); i++)
        {
            font::print(PR_FONT_LEVEL | PR_CJK_LOW, map.roomtext[i].x*8, (map.roomtext[i].y*8), map.roomtext[i].text, 196, 196, 255 - help.glow);
        }
    }

#if !defined(NO_CUSTOM_LEVELS) && !defined(NO_EDITOR)
     if(map.custommode && !map.custommodeforreal && !game.advancetext){
        //Return to level editor
        int alpha = graphics.lerp(ed.oldreturneditoralpha, ed.returneditoralpha);
        font::print(PR_ALPHA(alpha) | PR_BOR, 5, 5, loc::gettext("[Press ENTER to return to editor]"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
      }
#endif


    graphics.cutscenebars();
    graphics.drawfade();

    graphics.set_render_target(graphics.gameTexture);

    graphics.copy_texture(graphics.gameplayTexture, NULL, NULL);

    graphics.drawgui();
    if (graphics.flipmode)
    {
        if (game.advancetext) font::print(PR_CEN | PR_BOR, -1, 228, loc::gettext("- Press ACTION to advance text -"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
    }
    else
    {
        if (game.advancetext) font::print(PR_CEN | PR_BOR, -1, 5, loc::gettext("- Press ACTION to advance text -"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
    }

    if (game.readytotele > 100 || game.oldreadytotele > 100)
    {
        char buffer[SCREEN_WIDTH_CHARS + 1];
        const char* final_string = interact_prompt(
            buffer,
            sizeof(buffer),
            loc::gettext("- Press {button} to Teleport -")
        );
        int alpha = graphics.lerp(game.oldreadytotele, game.readytotele);

        font::print(
            PR_COLORGLYPH_BRI(alpha) | PR_CEN | PR_BOR,
            -1,
            graphics.flipmode ? 20 : 210,
            final_string,
            alpha - 20 - (help.glow / 2),
            alpha - 20 - (help.glow / 2),
            alpha
        );
    }

    if (game.swnmode)
    {
        if (game.swngame == 0)
        {
            std::string tempstring = help.timestring(game.swntimer);
            font::print(PR_2X | PR_CEN | PR_BOR, -1, 20, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
        }
        else if (game.swngame == 1)
        {
            if (game.swnmessage == 0)
            {
                std::string tempstring = help.timestring(game.swntimer);
                font::print(PR_BOR, 10, 10, loc::gettext("Current Time"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
                font::print(PR_2X | PR_BOR | PR_FONT_8X8, 25, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
                tempstring = help.timestring(game.swnrecord);
                font::print(PR_BOR | PR_RIGHT, 320-8, 10, loc::gettext("Best Time"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
                font::print(PR_2X | PR_BOR | PR_FONT_8X8 | PR_RIGHT, 300, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));

                switch(game.swnbestrank)
                {
                case 0:
                    font::print_wrap(PR_CEN, -1, 204, loc::gettext("Next Trophy at 5 seconds"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
                    break;
                case 1:
                    font::print_wrap(PR_CEN, -1, 204, loc::gettext("Next Trophy at 10 seconds"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
                    break;
                case 2:
                    font::print_wrap(PR_CEN, -1, 204, loc::gettext("Next Trophy at 15 seconds"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
                    break;
                case 3:
                    font::print_wrap(PR_CEN, -1, 204, loc::gettext("Next Trophy at 20 seconds"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
                    break;
                case 4:
                    font::print_wrap(PR_CEN, -1, 204, loc::gettext("Next Trophy at 30 seconds"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
                    break;
                case 5:
                    font::print_wrap(PR_CEN, -1, 204, loc::gettext("Next Trophy at 1 minute"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
                    break;
                case 6:
                    font::print_wrap(PR_CEN, -1, 204, loc::gettext("All Trophies collected!"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
                    break;
                }
            }
            else if (game.swnmessage == 1)
            {
                std::string tempstring = help.timestring(game.swntimer);
                font::print(PR_BOR, 10, 10, loc::gettext("Current Time"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
                font::print(PR_2X | PR_BOR | PR_FONT_8X8, 25, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
                tempstring = help.timestring(game.swnrecord);
                if (int(game.deathseq / 5) % 2 == 1)
                {
                    font::print(PR_BOR | PR_RIGHT, 320-8, 10, loc::gettext("Best Time"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
                    font::print(PR_2X | PR_BOR | PR_FONT_8X8 | PR_RIGHT, 300, 24, tempstring, 128 - (help.glow), 220 - (help.glow), 128 - (help.glow / 2));

                    font::print(PR_2X | PR_BOR | PR_CEN, -1, 200, loc::gettext("New Record!"), 128 - (help.glow), 220 - (help.glow), 128 - (help.glow / 2));
                }
            }
            else if (game.swnmessage >= 2)
            {
                game.swnmessage--;
                if (game.swnmessage == 2) game.swnmessage = 0;
                std::string tempstring = help.timestring(game.swntimer);
                font::print(PR_BOR, 10, 10, loc::gettext("Current Time"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
                font::print(PR_2X | PR_BOR | PR_FONT_8X8, 25, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
                tempstring = help.timestring(game.swnrecord);
                font::print(PR_BOR | PR_RIGHT, 320-8, 10, loc::gettext("Best Time"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
                font::print(PR_2X | PR_BOR | PR_FONT_8X8 | PR_RIGHT, 300, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));

                if (int(game.swnmessage / 5) % 2 == 1)
                {
                    font::print(PR_2X | PR_BOR | PR_CEN, -1, 200, loc::gettext("New Trophy!"), 220 - (help.glow), 128 - (help.glow), 128 - (help.glow / 2));
                }
            }

            font::print(PR_BOR | PR_CEN, -1, 228, loc::gettext("[Press ENTER to stop]"), 160 - (help.glow/2), 160 - (help.glow/2), 160 - (help.glow/2));
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
                font::print(PR_2X | PR_CEN | PR_BOR | PR_CJK_HIGH, -1, y1, loc::gettext("Survive for"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
                font::print(PR_2X | PR_CEN | PR_BOR, -1, y2, loc::gettext("60 seconds!"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
            }
        }
        else if(game.swngame==7)
        {
            if (game.swndelay >= 60)
            {
                font::print(PR_2X | PR_BOR | PR_CEN, -1, 20, loc::gettext("SUPER GRAVITRON"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));

                std::string tempstring = help.timestring(game.swnrecord);
                font::print(PR_BOR | PR_CEN, -1, 190, loc::gettext("Best Time"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
                font::print(PR_2X | PR_BOR | PR_CEN, -1, 205, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
            }
            else if (int(game.swndelay / 10) % 2 == 1)
            {
                font::print(PR_2X | PR_BOR | PR_CEN, -1, 20, loc::gettext("SUPER GRAVITRON"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
                font::print(PR_3X | PR_BOR | PR_CEN, -1, 200, loc::gettext("GO!"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
            }
        }
    }

    if (game.intimetrial && graphics.fademode == FADE_NONE)
    {
        //Draw countdown!
        if (game.timetrialcountdown > 0)
        {
            if (game.timetrialcountdown < 30)
            {
                if (int(game.timetrialcountdown / 4) % 2 == 0)
                {
                    font::print(PR_4X | PR_CEN | PR_BOR, -1, 100, loc::gettext("Go!"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
                }
            }
            else if (game.timetrialcountdown < 60)
            {
                font::print(PR_4X | PR_CEN | PR_BOR, -1, 100, "1", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
            }
            else if (game.timetrialcountdown < 90)
            {
                font::print(PR_4X | PR_CEN | PR_BOR,  -1, 100, "2", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
            }
            else if (game.timetrialcountdown < 120)
            {
                font::print(PR_4X | PR_CEN | PR_BOR,  -1, 100, "3", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
            }
        }
        else if (!roomname_translator::is_pausing() && !game.translator_exploring)
        {
            char buffer[SCREEN_WIDTH_CHARS + 1];
            game.timestringcenti(buffer, sizeof(buffer));

            //Draw OSD stuff
            const char* tempstring = loc::gettext("TIME:");
            int label_len = font::len(0, tempstring);
            font::print(PR_BOR, 6, 18, tempstring,  255,255,255);
            tempstring = loc::gettext("DEATH:");
            label_len = SDL_max(label_len, font::len(0, tempstring));
            font::print(PR_BOR, 6, 30, tempstring,  255,255,255);
            tempstring = loc::gettext("SHINY:");
            label_len = SDL_max(label_len, font::len(0, tempstring));
            font::print(PR_BOR, 6, 42, tempstring,  255,255,255);

            if(game.timetrialparlost)
            {
                font::print(PR_BOR, 8+label_len, 18, buffer,  196, 80, 80);
            }
            else
            {
                font::print(PR_BOR, 8+label_len, 18, buffer,  196, 196, 196);
            }
            if(game.deathcounts>0)
            {
                font::print(PR_BOR, 8+label_len, 30,help.String(game.deathcounts),  196, 80, 80);
            }
            else
            {
                font::print(PR_BOR, 8+label_len, 30,help.String(game.deathcounts),  196, 196, 196);
            }
            vformat_buf(
                buffer, sizeof(buffer),
                loc::gettext("{n_trinkets} of {max_trinkets}"),
                "n_trinkets:int, max_trinkets:int",
                game.trinkets(), game.timetrialshinytarget
            );
            if(game.trinkets()<game.timetrialshinytarget)
            {
                font::print(PR_BOR, 8+label_len, 42, buffer,  196, 80, 80);
            }
            else
            {
                font::print(PR_BOR, 8+label_len, 42, buffer,  196, 196, 196);
            }

            std::string time = game.timetstring(game.timetrialpar);
            label_len = font::len(0, time);
            if(game.timetrialparlost)
            {
                font::print(PR_BOR | PR_RIGHT, 307-label_len-8, 214, loc::gettext("PAR TIME:"),  80, 80, 80);
                font::print(PR_BOR,            307-label_len,   214, time,  80, 80, 80);
            }
            else
            {
                font::print(PR_BOR | PR_RIGHT, 307-label_len-8, 214, loc::gettext("PAR TIME:"),  255, 255, 255);
                font::print(PR_BOR,            307-label_len,   214, time,  196, 196, 196);
            }
        }
    }

    float act_alpha = graphics.lerp(game.prev_act_fade, game.act_fade) / 10.0f;
    if(game.act_fade>5 || game.prev_act_fade>5)
    {
        char buffer[SCREEN_WIDTH_CHARS + 1];
        const char* final_string = interact_prompt(
            buffer,
            sizeof(buffer),
            game.activity_lastprompt.c_str()
        );

        int centered_x = ((160 ) - ((font::len(game.activity_print_flags, final_string)) / 2));

        if (game.activity_r == 0 && game.activity_g == 0 && game.activity_b == 0)
        {
            font::print(
                game.activity_print_flags | PR_COLORGLYPH_BRI(act_alpha*255),
                centered_x + game.activity_x,
                game.activity_y + 12,
                final_string,
                196*act_alpha,
                196*act_alpha,
                (255 - help.glow)*act_alpha
            );
        }
        else
        {
            graphics.drawpixeltextbox(
                game.activity_x + 4,
                game.activity_y + 4,
                39*8,
                16 + font::height(game.activity_print_flags),
                game.activity_r*act_alpha,
                game.activity_g*act_alpha,
                game.activity_b*act_alpha
            );
            font::print(
                game.activity_print_flags | PR_COLORGLYPH_BRI(act_alpha*255) | PR_CJK_LOW,
                centered_x + game.activity_x,
                game.activity_y + 12,
                final_string,
                game.activity_r*act_alpha,
                game.activity_g*act_alpha,
                game.activity_b*act_alpha
            );
        }
    }

    if (obj.trophytext > 0 || obj.oldtrophytext > 0)
    {
        graphics.drawtrophytext();
    }


    graphics.renderwithscreeneffects();
}

static void draw_roomname_menu(void)
{
    const char* name;

    if (map.hiddenname[0] != '\0')
    {
        name = loc::gettext_roomname_special(map.hiddenname);
    }
    else if (map.finalmode)
    {
        name = loc::gettext_roomname(map.custommode, game.roomx, game.roomy, map.glitchname, map.roomname_special);
    }
    else
    {
        name = loc::gettext_roomname(map.custommode, game.roomx, game.roomy, map.roomname, map.roomname_special);
    }

    font::print(PR_FONT_LEVEL | PR_CEN, -1, 2, name, 196, 196, 255 - help.glow);
}

/* Used to keep some graphics positions on the map screen
 * the same in Flip Mode. */
#define FLIP(y, h) (graphics.flipmode ? 220 - (y) - (h) : (y))

static MapRenderData getmaprenderdata()
{
    MapRenderData data;

    data.zoom = map.custommode ? map.customzoom : 1;
    data.xoff = map.custommode ? map.custommmxoff : 0;
    data.yoff = map.custommode ? map.custommmyoff : 0;
    data.legendxoff = 40 + data.xoff;
    data.legendyoff = 21 + data.yoff;

    // Magic numbers for centering legend tiles.
    switch (data.zoom)
    {
    case 4:
        data.legendxoff += 21;
        data.legendyoff += 16;
        break;
    case 2:
        data.legendxoff += 9;
        data.legendyoff += 5;
        break;
    default:
        data.legendxoff += 3;
        data.legendyoff += 1;
        break;
    }

    return data;
}

static void rendermap(void)
{
#ifndef NO_CUSTOM_LEVELS
    if (map.custommode && map.customshowmm)
    {
        graphics.drawpixeltextbox(35 + map.custommmxoff, 16 + map.custommmyoff, map.custommmxsize + 10, map.custommmysize + 10, 65, 185, 207);
        graphics.drawpartimage(graphics.minimap_mounted ? IMAGE_MINIMAP : IMAGE_CUSTOMMINIMAP, 40 + map.custommmxoff, 21 + map.custommmyoff, map.custommmxsize, map.custommmysize);
        return;
     }
#endif /* NO_CUSTOM_LEVELS */

    graphics.drawpixeltextbox(35, 16, 250, 190, 65, 185, 207);
    graphics.drawimage(IMAGE_MINIMAP, 40, 21, false);
}

static void rendermapfog(void)
{
    const MapRenderData data = getmaprenderdata();

    for (int j = 0; j < map.getheight(); j++)
    {
        for (int i = 0; i < map.getwidth(); i++)
        {
            if (!map.isexplored(i, j))
            {
                // Draw the fog, depending on the custom zoom size
                for (int x = 0; x < data.zoom; x++)
                {
                    for (int y = 0; y < data.zoom; y++)
                    {
                        graphics.drawimage(IMAGE_COVERED, data.xoff + 40 + (x * 12) + (i * (12 * data.zoom)), data.yoff + 21 + (y * 9) + (j * (9 * data.zoom)), false);
                    }
                }
            }
        }
    }
}

static void rendermaplegend(void)
{
    // Draw the map legend, aka teleports/targets/trinkets

    const MapRenderData data = getmaprenderdata();

    const int tile_offset = graphics.flipmode ? 3 : 0;

    for (size_t i = 0; i < map.teleporters.size(); i++)
    {
        if (map.showteleporters && map.isexplored(map.teleporters[i].x, map.teleporters[i].y))
        {
            graphics.drawtile(data.legendxoff + (map.teleporters[i].x * 12 * data.zoom), data.legendyoff + (map.teleporters[i].y * 9 * data.zoom), 1127 + tile_offset);
        }
        else if (map.showtargets && !map.isexplored(map.teleporters[i].x, map.teleporters[i].y))
        {
            graphics.drawtile(data.legendxoff + (map.teleporters[i].x * 12 * data.zoom), data.legendyoff + (map.teleporters[i].y * 9 * data.zoom), 1126 + tile_offset);
        }
    }

    if (map.showtrinkets)
    {
        for (size_t i = 0; i < map.shinytrinkets.size(); i++)
        {
            if (!obj.collect[i])
            {
                graphics.drawtile(data.legendxoff + (map.shinytrinkets[i].x * 12 * data.zoom), data.legendyoff + (map.shinytrinkets[i].y * 9 * data.zoom), 1086 + tile_offset);
            }
        }
    }
}

static void rendermapcursor(const bool flashing)
{
    const MapRenderData data = getmaprenderdata();

    if (!map.custommode && game.roomx == 109)
    {
        // Draw the tower specially
        if (!flashing)
        {
            graphics.drawrect(40 + ((game.roomx - 100) * 12) + 2, 21 + 2, 12 - 4, 180 - 4, 16, 245 - (help.glow * 2), 245 - (help.glow * 2));
        }
        else if (map.cursorstate == 1)
        {
            if (int(map.cursordelay / 4) % 2 == 0)
            {
                graphics.drawrect(40 + ((game.roomx - 100) * 12), 21, 12, 180, 255, 255, 255);
                graphics.drawrect(40 + ((game.roomx - 100) * 12) + 2, 21 + 2, 12 - 4, 180 - 4, 255, 255, 255);
            }
        }
        else if (map.cursorstate == 2 && (int(map.cursordelay / 15) % 2 == 0))
        {
            graphics.drawrect(40 + ((game.roomx - 100) * 12) + 2, 21 + 2, 12 - 4, 180 - 4, 16, 245 - (help.glow), 245 - (help.glow));
        }
        return;
    }

    if (!flashing || (map.cursorstate == 2 && int(map.cursordelay / 15) % 2 == 0))
    {
        graphics.drawrect(40 + ((game.roomx - 100) * 12 * data.zoom) + 2 + data.xoff, 21 + ((game.roomy - 100) * 9 * data.zoom) + 2 + data.yoff, (12 * data.zoom) - 4, (9 * data.zoom) - 4, 16, 245 - (help.glow), 245 - (help.glow));
    }
    else if (map.cursorstate == 1 && (int(map.cursordelay / 4) % 2 == 0))
    {
        graphics.drawrect(40 + ((game.roomx - 100) * 12 * data.zoom) + data.xoff, 21 + ((game.roomy - 100) * 9 * data.zoom) + data.yoff, 12 * data.zoom, 9 * data.zoom, 255, 255, 255);
        graphics.drawrect(40 + ((game.roomx - 100) * 12 * data.zoom) + 2 + data.xoff, 21 + ((game.roomy - 100) * 9 * data.zoom) + 2 + data.yoff, (12 * data.zoom) - 4, (9 * data.zoom) - 4, 255, 255, 255);
    }
}

void maprender(void)
{
    graphics.set_render_target(graphics.menuTexture);
    graphics.clear();

    draw_roomname_menu();

    //Background color
    graphics.fill_rect(0, 12, 320, 240, 10, 24, 26 );

    //Menubar:
    graphics.drawpixeltextbox( -10, 212, 43*8, 16 + font::height(PR_FONT_INTERFACE), 65, 185, 207);

    // Draw the selected page name at the bottom
    // menupage 0 - 3 is the pause screen
    if (script.running && game.menupage == 3)
    {
        // While in a cutscene, you can only save
        char buffer[SCREEN_WIDTH_CHARS + 1];
        vformat_buf(buffer, sizeof(buffer), loc::get_langmeta()->menu_select_tight.c_str(), "label:str", loc::gettext("SAVE"));
        font::print(PR_CEN | PR_CJK_LOW, -1, 220, buffer, 196, 196, 255 - help.glow);
    }
    else if (game.menupage <= 3)
    {
        const char* tab1;
        if (game.insecretlab)
        {
            tab1 = loc::gettext("GRAV");
        }
        else if (obj.flags[67] && !map.custommode)
        {
            tab1 = loc::gettext("SHIP");
        }
        else
        {
            tab1 = loc::gettext("CREW");
        }
#define TAB(opt, text) graphics.map_tab(opt, text, game.menupage == opt)
        TAB(0, loc::gettext("MAP"));
        TAB(1, tab1);
        TAB(2, loc::gettext("STATS"));
        TAB(3, loc::gettext("SAVE"));
#undef TAB
    }

    // Draw menu header
    switch (game.menupage)
    {
    case 30:
    case 31:
    case 32:
    case 33:
        font::print(PR_CEN | PR_CJK_LOW, -1, 220, loc::gettext("[ PAUSE ]"), 196, 196, 255 - help.glow);
    }

    // Draw menu options
    if (game.menupage >= 30 && game.menupage <= 33)
    {
#define OPTION(opt, text) graphics.map_option(opt, 4, text, game.menupage - 30 == opt)
        OPTION(0, loc::gettext("return to game"));
        OPTION(1, loc::gettext("options"));
        OPTION(2, loc::gettext("quit to menu"));
#undef OPTION
    }

    // Draw the actual menu
    switch(game.menupage)
    {
    case 0:
        rendermap();

        if (map.finalmode || (map.custommode&&!map.customshowmm))
        {
            // Cover the whole map
            for (int j = 0; j < 20; j++)
            {
                for (int i = 0; i < 20; i++)
                {
                    graphics.drawimage(IMAGE_COVERED, 40 + (i * 12), 21 + (j * 9), false);
                }
            }
            font::print(PR_CEN | PR_BOR, -1, 105, loc::gettext("NO SIGNAL"), 245, 245, 245);
        }
        else
        {
            rendermapfog();
            rendermapcursor(true);
            rendermaplegend();
        }
        break;
    case 1:
        if (game.insecretlab)
        {
            if (graphics.flipmode)
            {
                graphics.PrintWrap(0, 174, loc::gettext("SUPER GRAVITRON HIGHSCORE"), 196, 196, 255 - help.glow, true);

                std::string tempstring = help.timestring(game.swnrecord);
                font::print(PR_CEN, -1, 124, loc::gettext("Best Time"), 196, 196, 255 - help.glow);
                font::print(PR_2X | PR_CEN, -1, 102, tempstring, 196, 196, 255 - help.glow);

                switch(game.swnbestrank)
                {
                case 0:
                    font::print_wrap(PR_CEN, -1, 40, loc::gettext("Next Trophy at 5 seconds"), 196, 196, 255 - help.glow);
                    break;
                case 1:
                    font::print_wrap(PR_CEN, -1, 40, loc::gettext("Next Trophy at 10 seconds"), 196, 196, 255 - help.glow);
                    break;
                case 2:
                    font::print_wrap(PR_CEN, -1, 40, loc::gettext("Next Trophy at 15 seconds"), 196, 196, 255 - help.glow);
                    break;
                case 3:
                    font::print_wrap(PR_CEN, -1, 40, loc::gettext("Next Trophy at 20 seconds"), 196, 196, 255 - help.glow);
                    break;
                case 4:
                    font::print_wrap(PR_CEN, -1, 40, loc::gettext("Next Trophy at 30 seconds"), 196, 196, 255 - help.glow);
                    break;
                case 5:
                    font::print_wrap(PR_CEN, -1, 40, loc::gettext("Next Trophy at 1 minute"), 196, 196, 255 - help.glow);
                    break;
                case 6:
                    font::print_wrap(PR_CEN, -1, 40, loc::gettext("All Trophies collected!"), 196, 196, 255 - help.glow);
                    break;
                }
            }
            else
            {
                graphics.PrintWrap(0, 40, loc::gettext("SUPER GRAVITRON HIGHSCORE"), 196, 196, 255 - help.glow, true);

                std::string tempstring = help.timestring(game.swnrecord);
                font::print(PR_CEN, -1, 90, loc::gettext("Best Time"), 196, 196, 255 - help.glow);
                font::print(PR_2X | PR_CEN, -1, 104, tempstring, 196, 196, 255 - help.glow);

                switch(game.swnbestrank)
                {
                case 0:
                    font::print_wrap(PR_CEN, -1, 174, loc::gettext("Next Trophy at 5 seconds"), 196, 196, 255 - help.glow);
                    break;
                case 1:
                    font::print_wrap(PR_CEN, -1, 174, loc::gettext("Next Trophy at 10 seconds"), 196, 196, 255 - help.glow);
                    break;
                case 2:
                    font::print_wrap(PR_CEN, -1, 174, loc::gettext("Next Trophy at 15 seconds"), 196, 196, 255 - help.glow);
                    break;
                case 3:
                    font::print_wrap(PR_CEN, -1, 174, loc::gettext("Next Trophy at 20 seconds"), 196, 196, 255 - help.glow);
                    break;
                case 4:
                    font::print_wrap(PR_CEN, -1, 174, loc::gettext("Next Trophy at 30 seconds"), 196, 196, 255 - help.glow);
                    break;
                case 5:
                    font::print_wrap(PR_CEN, -1, 174, loc::gettext("Next Trophy at 1 minute"), 196, 196, 255 - help.glow);
                    break;
                case 6:
                    font::print_wrap(PR_CEN, -1, 174, loc::gettext("All Trophies collected!"), 196, 196, 255 - help.glow);
                    break;
                }
            }
        }
        else if (obj.flags[67] && !map.custommode)
        {
            graphics.PrintWrap(0, 105, loc::gettext("Press ACTION to warp to the ship."), 196, 196, 255 - help.glow, true);
        }
#if !defined(NO_CUSTOM_LEVELS)
        else if(map.custommode){
            LevelMetaData& meta = cl.ListOfMetaData[game.playcustomlevel];

            uint32_t title_flags = meta.title_is_gettext ? PR_FONT_INTERFACE : PR_FONT_LEVEL;
            uint32_t creator_flags = meta.creator_is_gettext ? PR_FONT_INTERFACE : PR_FONT_LEVEL;

            font::print(title_flags | PR_2X | PR_CEN, -1, FLIP(45, 8), meta.title, 196, 196, 255 - help.glow);
            char buffer[SCREEN_WIDTH_CHARS + 1];
            vformat_buf(
                buffer, sizeof(buffer),
                loc::gettext("by {author}"),
                "author:str",
                meta.creator.c_str()
            );
            int sp = SDL_max(10, font::height(PR_FONT_LEVEL));
            font::print(creator_flags | PR_CEN, -1, FLIP(70, 8), buffer, 196, 196, 255 - help.glow);
            font::print(PR_FONT_LEVEL | PR_CEN, -1, FLIP(70+sp, 8), meta.website, 196, 196, 255 - help.glow);
            font::print(PR_FONT_LEVEL | PR_CEN, -1, FLIP(70+sp*3, 8), meta.Desc1, 196, 196, 255 - help.glow);
            font::print(PR_FONT_LEVEL | PR_CEN, -1, FLIP(70+sp*4, 8), meta.Desc2, 196, 196, 255 - help.glow);
            if (sp <= 10)
            {
                font::print(PR_FONT_LEVEL | PR_CEN, -1, FLIP(70+sp*5, 8), meta.Desc3, 196, 196, 255 - help.glow);
            }

            int remaining = cl.numcrewmates() - game.crewmates();

            loc::gettext_plural_fill(
                buffer, sizeof(buffer),
                "{n_crew|wordy} crewmates remain",
                "{n_crew|wordy} crewmate remains",
                "n_crew:int",
                remaining
            );
            font::print(PR_CEN, -1, FLIP(165, 8), buffer, 196, 196, 255 - help.glow);
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
                    }
                    else
                    {
                        graphics.printcrewnamedark(44, 32 + (i * 64)+4+10, 2-i);
                    }
                    graphics.printcrewnamestatus(44, 32 + (i * 64)+4, 2-i, game.crewstats[(2-i)]);

                    graphics.drawcrewman(16+160, 32 + (i * 64), (2-i)+3, game.crewstats[(2-i)+3]);
                    if (game.crewstats[(2-i)+3])
                    {
                        graphics.printcrewname(44+160, 32 + (i * 64)+4+10, (2-i)+3);
                    }
                    else
                    {
                        graphics.printcrewnamedark(44+160, 32 + (i * 64)+4+10, (2-i)+3);
                    }
                    graphics.printcrewnamestatus(44+160, 32 + (i * 64)+4, (2-i)+3, game.crewstats[(2-i)+3]);
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
                    }
                    else
                    {
                        graphics.printcrewnamedark(44, 32 + (i * 64)+4, i);
                    }
                    graphics.printcrewnamestatus(44, 32 + (i * 64)+4+10, i, game.crewstats[i]);

                    graphics.drawcrewman(16+160, 32 + (i * 64), i+3, game.crewstats[i+3]);
                    if (game.crewstats[i+3])
                    {
                        graphics.printcrewname(44+160, 32 + (i * 64)+4, i+3);
                    }
                    else
                    {
                        graphics.printcrewnamedark(44+160, 32 + (i * 64)+4, i+3);
                    }
                    graphics.printcrewnamestatus(44+160, 32 + (i * 64)+4+10, i+3, game.crewstats[i+3]);
                }
            }
        }
        break;
    case 2:
    {
        int max_trinkets;
#ifndef NO_CUSTOM_LEVELS
        if (map.custommode)
        {
            max_trinkets = cl.numtrinkets();
        }
        else
#endif
        {
            max_trinkets = 20;
        }

        /* Stats. */
        font::print(PR_CEN | PR_CJK_HIGH, -1, FLIP(52, 8), loc::gettext("[Trinkets found]"), 196, 196, 255 - help.glow);
        char buffer[SCREEN_WIDTH_CHARS + 1];
        vformat_buf(
            buffer, sizeof(buffer),
            loc::gettext("{n_trinkets|wordy} out of {max_trinkets|wordy}"),
            "n_trinkets:int, max_trinkets:int",
            game.trinkets(), max_trinkets
        );
        font::print(PR_CEN | PR_CJK_LOW, -1, FLIP(64, 8), buffer, 96, 96, 96);

        font::print(PR_CEN | PR_CJK_HIGH, -1, FLIP(102, 8), loc::gettext("[Number of Deaths]"), 196, 196, 255 - help.glow);
        font::print(PR_CEN | PR_CJK_LOW, -1, FLIP(114, 8), help.String(game.deathcounts), 96, 96, 96);

        font::print(PR_CEN | PR_CJK_HIGH, -1, FLIP(152, 8), loc::gettext("[Time Taken]"), 196, 196, 255 - help.glow);
        font::print(PR_CEN | PR_CJK_LOW, -1, FLIP(164, 8), game.timestring(), 96, 96, 96);
        break;
    }
    case 3:
    {
        if (game.inintermission || game.translator_exploring)
        {
            graphics.PrintWrap(0, 115, loc::gettext("Cannot Save in Level Replay"), 146, 146, 180, true);
            break;
        }
        if (game.nodeathmode)
        {
            graphics.PrintWrap(0, 115, loc::gettext("Cannot Save in No Death Mode"), 146, 146, 180, true);
            break;
        }
        if (game.intimetrial)
        {
            graphics.PrintWrap(0, 115, loc::gettext("How'd you get here?"), 146, 146, 180, true);
            break;
        }
        if (game.insecretlab)
        {
            graphics.PrintWrap(0, 115, loc::gettext("Cannot Save in Secret Lab"), 146, 146, 180, true);
            break;
        }
        if (game.gamesavefailed)
        {
            graphics.PrintWrap(0, 115, loc::gettext("ERROR: Could not save game!"), 146, 146, 180, true);
            break;
        }

        /* We are not in a special case, so draw the save screen now... */

        if (!map.custommode)
        {
            /* FIXME: The text here should be automatically "balance-wrapped" instead of hardcoding the width.
             * In fact, maybe PrintWrap should balance-wrap by default. */
            graphics.PrintWrap(0, 174, loc::gettext("(Note: The game is autosaved at every teleporter.)"), 146, 146, 180, true, 12);
        }

        if (!game.gamesaved)
        {
            font::print(PR_CEN, -1, 80, loc::gettext("[Press ACTION to save your game]"), 255 - help.glow*2, 255 - help.glow*2, 255 - help.glow);

            if (map.custommode || game.quicksummary == "")
            {
                break;
            }

            font::print(PR_CEN, -1, FLIP(100, 8), loc::gettext("Last Save:"), 164 - help.glow/4, 164 - help.glow/4, 164);
            font::print(PR_CEN, -1, FLIP(112, 8), game.quicksummary, 164 - help.glow/4, 164 - help.glow/4, 164);
            break;
        }

        /* We are only still here if the game has been quicksaved... */

        graphics.PrintWrap(0, 36, loc::gettext("Game saved ok!"), 255 - help.glow/2, 255 - help.glow/2, 255 - help.glow/2, true);

        graphics.drawpixeltextbox(17, 65, 286, 90, 65, 185, 207);

        if (map.custommode)
        {
            font::print(PR_CEN | PR_FONT_LEVEL, -1, FLIP(90, 8), game.customleveltitle, 25, 255 - help.glow/2, 255 - help.glow/2);
        }
        else
        {
            size_t i;
            font::print(PR_CEN, -1, FLIP(80, 8), game.savearea, 25, 255 - help.glow/2, 255 - help.glow/2);
            for (i = 0; i < SDL_arraysize(game.crewstats); ++i)
            {
                /* Crewmates are annoying. Their height is 21 pixels, but to flip them,
                 * we also have to account for their 2-pixel y-offset (and multiply it by 2). */
                graphics.drawcrewman(169 - 3*42 + i*42, FLIP(95, 21 + 2*2), i, game.crewstats[i], true);
            }
        }

        font::print(0, 59, FLIP(132, 8), game.savetime, 255 - help.glow/2, 255 - help.glow/2, 255 - help.glow/2);
        char buffer[SCREEN_WIDTH_CHARS + 1];
        vformat_buf(buffer, sizeof(buffer),
            loc::gettext("{savebox_n_trinkets|wordy}"),
            "savebox_n_trinkets:int",
            game.savetrinkets
        );
        font::print(PR_RIGHT, 262, FLIP(132, 8), buffer, 255 - help.glow/2, 255 - help.glow/2, 255 - help.glow/2);

        graphics.draw_sprite(34, FLIP(126, 17), 50, graphics.col_clock);
        graphics.draw_sprite(270, FLIP(126, 17), 22, graphics.col_trinket);
        break;
    }
    case 10:
        font::print(PR_CEN | PR_CJK_LOW, -1, 220, loc::gettext("[ QUIT ]"), 196, 196, 255 - help.glow);

        if (graphics.flipmode)
        {
            if (game.inspecial())
            {
                graphics.PrintWrap(0, 135, loc::gettext("Return to main menu?"), 196, 196, 255 - help.glow, true, 12);
            }
            else
            {
                graphics.PrintWrap(0, 142, loc::gettext("Do you want to quit? You will lose any unsaved progress."), 196, 196, 255 - help.glow, true, 12);
            }

            font::print(0, 80-16, 88, loc::gettext("[ NO, KEEP PLAYING ]"), 196, 196, 255 - help.glow);
            font::print(0, 80 + 32, 76, loc::gettext("yes, quit to menu"),  96, 96, 96);
        }
        else
        {

            if (game.inspecial())
            {
                graphics.PrintWrap(0, 80, loc::gettext("Return to main menu?"), 196, 196, 255 - help.glow, true, 12);
            }
            else
            {
                graphics.PrintWrap(0, 76, loc::gettext("Do you want to quit? You will lose any unsaved progress."), 196, 196, 255 - help.glow, true, 12);
            }

            font::print(0, 80-16, 130, loc::gettext("[ NO, KEEP PLAYING ]"), 196, 196, 255 - help.glow);
            font::print(0, 80 + 32, 142, loc::gettext("yes, quit to menu"),  96, 96, 96);

        }
        break;
    case 11:
        font::print(PR_CEN | PR_CJK_LOW, -1, 220, loc::gettext("[ QUIT ]"), 196, 196, 255 - help.glow);

        if (graphics.flipmode)
        {
            if (game.inspecial())
            {
                graphics.PrintWrap(0, 135, loc::gettext("Return to main menu?"), 196, 196, 255 - help.glow, true, 12);
            }
            else
            {
                graphics.PrintWrap(0, 142, loc::gettext("Do you want to quit? You will lose any unsaved progress."), 196, 196, 255 - help.glow, true, 12);
            }

            font::print(0, 80, 88, loc::gettext("no, keep playing"), 96,96,96);
            font::print(0, 80+32-16, 76, loc::gettext("[ YES, QUIT TO MENU ]"),  196, 196, 255 - help.glow);
        }
        else
        {
            if (game.inspecial())
            {
                graphics.PrintWrap(0, 80, loc::gettext("Return to main menu?"), 196, 196, 255 - help.glow, true, 12);
            }
            else
            {
                graphics.PrintWrap(0, 76, loc::gettext("Do you want to quit? You will lose any unsaved progress."), 196, 196, 255 - help.glow, true, 12);
            }

            font::print(0, 80, 130, loc::gettext("no, keep playing"), 96,96,96);
            font::print(0, 80+32-16, 142, loc::gettext("[ YES, QUIT TO MENU ]"), 196, 196, 255 - help.glow);
        }
        break;
    case 20:
        font::print(PR_CEN | PR_CJK_LOW, -1, 220, loc::gettext("[ GRAVITRON ]"), 196, 196, 255 - help.glow);

        if (graphics.flipmode)
        {
            graphics.PrintWrap(0, 88, loc::gettext("Do you want to return to the secret laboratory?"), 196, 196, 255 - help.glow, true, 12);
            font::print(0, 80-16, 142, loc::gettext("[ NO, KEEP PLAYING ]"), 196, 196, 255 - help.glow);
            font::print(0, 80 + 32, 130, loc::gettext("yes, return"),  96, 96, 96);
        }
        else
        {
            graphics.PrintWrap(0, 76, loc::gettext("Do you want to return to the secret laboratory?"), 196, 196, 255 - help.glow, true, 12);
            font::print(0, 80-16, 130, loc::gettext("[ NO, KEEP PLAYING ]"), 196, 196, 255 - help.glow);
            font::print(0, 80 + 32, 142, loc::gettext("yes, return"),  96, 96, 96);
        }

        break;
    case 21:
        font::print(PR_CEN | PR_CJK_LOW, -1, 220, loc::gettext("[ GRAVITRON ]"), 196, 196, 255 - help.glow);

        if (graphics.flipmode)
        {
            graphics.PrintWrap(0, 88, loc::gettext("Do you want to return to the secret laboratory?"), 196, 196, 255 - help.glow, true, 12);
            font::print(0, 80, 142, loc::gettext("no, keep playing"), 96, 96, 96);
            font::print(0, 80 + 32-16, 130, loc::gettext("[ YES, RETURN ]"),  196, 196, 255 - help.glow);
        }
        else
        {
            graphics.PrintWrap(0, 76, loc::gettext("Do you want to return to the secret laboratory?"), 196, 196, 255 - help.glow, true, 12);
            font::print(0, 80, 130, loc::gettext("no, keep playing"), 96, 96, 96);
            font::print(0, 80 + 32-16, 142, loc::gettext("[ YES, RETURN ]"),  196, 196, 255 - help.glow);
        }

    }

    graphics.set_render_target(graphics.gameTexture);

    if (graphics.resumegamemode || graphics.menuoffset > 0 || graphics.oldmenuoffset > 0)
    {
        graphics.menuoffrender();
    }
    else
    {
        graphics.copy_texture(graphics.menuTexture, NULL, NULL);
    }

    // We need to draw the black screen above the menu in order to disguise it
    // being jankily brought down in glitchrunner mode when exiting to the title
    // Otherwise, there's no reason to obscure the menu
    if (GlitchrunnerMode_less_than_or_equal(Glitchrunner2_2)
        || FADEMODE_IS_FADING(graphics.fademode)
        || game.fadetomenu
        || game.fadetolab)
    {
        graphics.drawfade();
    }


    graphics.renderwithscreeneffects();
}

#undef FLIP

void teleporterrender(void)
{
    graphics.set_render_target(graphics.menuTexture);
    graphics.clear();

    const int telex = map.teleporters[game.teleport_to_teleporter].x;
    const int teley = map.teleporters[game.teleport_to_teleporter].y;

    draw_roomname_menu();

    //Background color
    graphics.fill_rect(0, 12, 320, 240, 10, 24, 26);

    rendermap();
    rendermapfog();
    rendermapcursor(false);

    // Draw a box around the currently selected teleporter

    const MapRenderData data = getmaprenderdata();

    if (game.useteleporter)
    {
        //Draw the chosen destination coordinate!
        //TODO
        //draw the coordinates //destination
        graphics.drawrect(40 + data.xoff + (telex * 12 * data.zoom) + 1, 21 + data.yoff + (teley * 9 * data.zoom) + 1, 12 * data.zoom - 2, 9 * data.zoom - 2, 245 - (help.glow * 2), 16, 16);
        graphics.drawrect(40 + data.xoff + (telex * 12 * data.zoom) + 3, 21 + data.yoff + (teley * 9 * data.zoom) + 3, 12 * data.zoom - 6, 9 * data.zoom - 6, 245 - (help.glow * 2), 16, 16);
    }

    // Draw the legend itself

    rendermaplegend();

    // Highlight the currently selected teleporter

    if (game.useteleporter && help.slowsine % 16 > 8)
    {
        graphics.drawtile(data.legendxoff + data.xoff + (telex * 12 * data.zoom), data.legendyoff + data.yoff + (teley * 9 * data.zoom), 1128 + (graphics.flipmode ? 3 : 0));
    }

    graphics.cutscenebars();


    if (game.useteleporter)
    {
        char buffer[SCREEN_WIDTH_CHARS + 1];
        const char* final_string = interact_prompt(
            buffer,
            sizeof(buffer),
            loc::gettext("Press {button} to Teleport")
        );

        //Instructions!
        font::print(PR_CEN, -1, 210, loc::gettext("Press Left/Right to choose a Teleporter"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
        font::print(PR_CEN, -1, 225, final_string, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
    }

    graphics.drawgui();

    if (graphics.flipmode)
    {
        if (game.advancetext) font::print(PR_CEN | PR_BOR, -1, 228, loc::gettext("- Press ACTION to advance text -"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
    }
    else
    {
        if (game.advancetext) font::print(PR_CEN | PR_BOR, -1, 5, loc::gettext("- Press ACTION to advance text -"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
    }

    graphics.set_render_target(graphics.gameTexture);

    if (graphics.resumegamemode || graphics.menuoffset > 0 || graphics.oldmenuoffset > 0)
    {
        graphics.menuoffrender();
    }
    else
    {
        graphics.copy_texture(graphics.menuTexture, NULL, NULL);
    }

    graphics.render();
}
