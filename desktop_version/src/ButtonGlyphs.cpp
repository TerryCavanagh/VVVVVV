#include "ButtonGlyphs.h"

#include <SDL.h>

#include "Game.h"
#include "Localization.h"
#include "UTF8.h"

extern "C"
{

typedef enum
{
    GLYPH_NINTENDO_DECK_A, // Note that for the Deck, the icons are same as Nintendo but the layout is the same as Xbox
    GLYPH_NINTENDO_DECK_B,
    GLYPH_NINTENDO_DECK_X,
    GLYPH_NINTENDO_DECK_Y,
    GLYPH_NINTENDO_PLUS,
    GLYPH_NINTENDO_MINUS,
    GLYPH_NINTENDO_L,
    GLYPH_NINTENDO_R,
    GLYPH_NINTENDO_ZL,
    GLYPH_NINTENDO_ZR,
    GLYPH_NINTENDO_XBOX_LSTICK,
    GLYPH_NINTENDO_XBOX_RSTICK,
    GLYPH_NINTENDO_SL,
    GLYPH_NINTENDO_SR,
    GLYPH_GENERIC_L,
    GLYPH_GENERIC_R,

    GLYPH_PLAYSTATION_CIRCLE,
    GLYPH_PLAYSTATION_CROSS,
    GLYPH_PLAYSTATION_TRIANGLE,
    GLYPH_PLAYSTATION_SQUARE,
    GLYPH_PLAYSTATION_START,
    GLYPH_PLAYSTATION_OPTIONS,
    GLYPH_PLAYSTATION_DECK_L1,
    GLYPH_PLAYSTATION_DECK_R1,
    GLYPH_PLAYSTATION_DECK_L2,
    GLYPH_PLAYSTATION_DECK_R2,
    GLYPH_PLAYSTATION_DECK_L3,
    GLYPH_PLAYSTATION_DECK_R3,
    GLYPH_DECK_L4,
    GLYPH_DECK_R4,
    GLYPH_DECK_L5,
    GLYPH_DECK_R5,

    GLYPH_XBOX_B,
    GLYPH_XBOX_A,
    GLYPH_XBOX_Y,
    GLYPH_XBOX_X,
    GLYPH_XBOX_DECK_VIEW,
    GLYPH_XBOX_DECK_MENU,
    GLYPH_XBOX_LB,
    GLYPH_XBOX_RB,
    GLYPH_XBOX_LT,
    GLYPH_XBOX_RT,
    GLYPH_NINTENDO_GENERIC_ACTIONRIGHT,
    GLYPH_NINTENDO_GENERIC_ACTIONDOWN,
    GLYPH_NINTENDO_GENERIC_ACTIONUP,
    GLYPH_NINTENDO_GENERIC_ACTIONLEFT,
    GLYPH_NINTENDO_GENERIC_STICK,
    GLYPH_UNKNOWN,

    GLYPH_TOTAL
}
ButtonGlyphKey;

static char glyph[GLYPH_TOTAL][5];

void BUTTONGLYPHS_init(void)
{
    /* Set glyph array to strings for all the button glyph codepoints (U+EBxx) */
    for (int i = 0; i < GLYPH_TOTAL; i++)
    {
        SDL_strlcpy(glyph[i], UTF8_encode(0xEB00+i).bytes, sizeof(glyph[i]));
    }
}

bool BUTTONGLYPHS_keyboard_is_available(void)
{
    /* Returns true if it makes sense to show button hints that are only available
     * on keyboards (like press M to mute), false if we're on a console. */
    return true;
}

bool BUTTONGLYPHS_keyboard_is_active(void)
{
    /* Returns true if, not only do we have a keyboard available, but it's also the
     * active input method. (So, show keyboard keys, if false, show controller glyphs) */
    return true;
}

const char* BUTTONGLYPHS_get_wasd_text(void)
{
    /* Returns the string to use in Welcome Aboard */
    if (BUTTONGLYPHS_keyboard_is_active())
    {
        return loc::gettext("Press arrow keys or WASD to move");
    }
    return loc::gettext("Press left/right to move");
}

const char* BUTTONGLYPHS_get_button(const ActionSet actionset, const Action action)
{
    /* Given a specific action (like INTERACT in-game),
     * return either a (localized) keyboard key string like "ENTER" or "E",
     * or a controller button glyph from the table above like glyph[GLYPH_XBOX_Y],
     * to fill into strings like "Press {button} to activate terminal". */
    switch (actionset)
    {
    case ActionSet_Menu:
        switch (action.Menu)
        {
        case Action_Menu_Accept:
            return loc::gettext("ACTION");
        }
        break;
    case ActionSet_InGame:
        switch (action.InGame)
        {
        case Action_InGame_ACTION:
            return loc::gettext("ACTION");

        case Action_InGame_Interact:
            if (game.separate_interact)
            {
                return "E";
            }
            return loc::gettext("ENTER");
        case Action_InGame_Map:
            return loc::gettext("ENTER");
        }
        break;
    }

    SDL_assert(0 && "Trying to get label/glyph for unknown action!");
    return glyph[GLYPH_UNKNOWN];
}

} // extern "C"
