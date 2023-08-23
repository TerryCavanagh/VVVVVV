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

typedef enum
{
    LAYOUT_NINTENDO_SWITCH_PRO,
    LAYOUT_NINTENDO_SWITCH_JOYCON_L,
    LAYOUT_NINTENDO_SWITCH_JOYCON_R,
    LAYOUT_DECK,
    LAYOUT_PLAYSTATION,
    LAYOUT_XBOX,
    LAYOUT_GENERIC,

    LAYOUT_TOTAL
}
ButtonGlyphLayout;

/* SDL provides Xbox buttons, we'd like to show the correct
 * (controller-specific) glyphs or labels for those... */
static const char* glyph_layout[LAYOUT_TOTAL][SDL_CONTROLLER_BUTTON_RIGHTSHOULDER + 1] = {
    { // NINTENDO_SWITCH_PRO
        glyph[GLYPH_NINTENDO_DECK_B], glyph[GLYPH_NINTENDO_DECK_A],
        glyph[GLYPH_NINTENDO_DECK_Y], glyph[GLYPH_NINTENDO_DECK_X],
        glyph[GLYPH_NINTENDO_MINUS], "HOME", glyph[GLYPH_NINTENDO_PLUS],
        glyph[GLYPH_NINTENDO_XBOX_LSTICK], glyph[GLYPH_NINTENDO_XBOX_RSTICK],
        glyph[GLYPH_NINTENDO_L], glyph[GLYPH_NINTENDO_R]
    },
    { // NINTENDO_SWITCH_JOYCON_L
        glyph[GLYPH_NINTENDO_GENERIC_ACTIONDOWN], glyph[GLYPH_NINTENDO_GENERIC_ACTIONRIGHT],
        glyph[GLYPH_NINTENDO_GENERIC_ACTIONLEFT], glyph[GLYPH_NINTENDO_GENERIC_ACTIONUP],
        "CAPTURE", "GUIDE", glyph[GLYPH_NINTENDO_MINUS],
        glyph[GLYPH_NINTENDO_GENERIC_STICK], glyph[GLYPH_NINTENDO_XBOX_RSTICK],
        glyph[GLYPH_NINTENDO_SL], glyph[GLYPH_NINTENDO_SR]
    },
    { // NINTENDO_SWITCH_JOYCON_R
        glyph[GLYPH_NINTENDO_GENERIC_ACTIONDOWN], glyph[GLYPH_NINTENDO_GENERIC_ACTIONRIGHT],
        glyph[GLYPH_NINTENDO_GENERIC_ACTIONLEFT], glyph[GLYPH_NINTENDO_GENERIC_ACTIONUP],
        "HOME", "GUIDE", glyph[GLYPH_NINTENDO_PLUS],
        glyph[GLYPH_NINTENDO_GENERIC_STICK], glyph[GLYPH_NINTENDO_XBOX_RSTICK],
        glyph[GLYPH_NINTENDO_SL], glyph[GLYPH_NINTENDO_SR]
    },
    { // DECK
        glyph[GLYPH_NINTENDO_DECK_A], glyph[GLYPH_NINTENDO_DECK_B],
        glyph[GLYPH_NINTENDO_DECK_X], glyph[GLYPH_NINTENDO_DECK_Y],
        glyph[GLYPH_XBOX_DECK_VIEW], "GUIDE", glyph[GLYPH_XBOX_DECK_MENU],
        glyph[GLYPH_PLAYSTATION_DECK_L3], glyph[GLYPH_PLAYSTATION_DECK_R3],
        glyph[GLYPH_PLAYSTATION_DECK_L1], glyph[GLYPH_PLAYSTATION_DECK_R1]
    },
    { // PLAYSTATION
        glyph[GLYPH_PLAYSTATION_CROSS], glyph[GLYPH_PLAYSTATION_CIRCLE],
        glyph[GLYPH_PLAYSTATION_SQUARE], glyph[GLYPH_PLAYSTATION_TRIANGLE],
        glyph[GLYPH_PLAYSTATION_OPTIONS], "PS", glyph[GLYPH_PLAYSTATION_START],
        glyph[GLYPH_PLAYSTATION_DECK_L3], glyph[GLYPH_PLAYSTATION_DECK_R3],
        glyph[GLYPH_PLAYSTATION_DECK_L1], glyph[GLYPH_PLAYSTATION_DECK_R1]
    },
    { // XBOX
        glyph[GLYPH_XBOX_A], glyph[GLYPH_XBOX_B],
        glyph[GLYPH_XBOX_X], glyph[GLYPH_XBOX_Y],
        glyph[GLYPH_XBOX_DECK_VIEW], "GUIDE", glyph[GLYPH_XBOX_DECK_MENU],
        glyph[GLYPH_NINTENDO_XBOX_LSTICK], glyph[GLYPH_NINTENDO_XBOX_RSTICK],
        glyph[GLYPH_XBOX_LB], glyph[GLYPH_XBOX_RB]
    },
    { // GENERIC
        glyph[GLYPH_NINTENDO_GENERIC_ACTIONDOWN], glyph[GLYPH_NINTENDO_GENERIC_ACTIONRIGHT],
        glyph[GLYPH_NINTENDO_GENERIC_ACTIONLEFT], glyph[GLYPH_NINTENDO_GENERIC_ACTIONUP],
        "SELECT", "GUIDE", "START",
        glyph[GLYPH_NINTENDO_XBOX_LSTICK], glyph[GLYPH_NINTENDO_XBOX_RSTICK],
        glyph[GLYPH_GENERIC_L], glyph[GLYPH_GENERIC_R]
    }
};

static bool keyboard_is_active = true;
static ButtonGlyphLayout layout = LAYOUT_GENERIC;

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
    return !SDL_GetHintBoolean("SteamDeck", SDL_FALSE);
}

bool BUTTONGLYPHS_keyboard_is_active(void)
{
    /* Returns true if, not only do we have a keyboard available, but it's also the
     * active input method. (So, show keyboard keys, if false, show controller glyphs) */
    return keyboard_is_active;
}

void BUTTONGLYPHS_keyboard_set_active(bool active)
{
    keyboard_is_active = active;
}

void BUTTONGLYPHS_update_layout(Uint16 vendor, Uint16 product)
{
    if (vendor == 0x054c)
    {
        layout = LAYOUT_PLAYSTATION;
    }
    else if (vendor == 0x28de)
    {
        layout = LAYOUT_DECK;
    }
    else if (vendor == 0x057e)
    {
        if (product == 0x2006)
        {
            layout = LAYOUT_NINTENDO_SWITCH_JOYCON_L;
        }
        else if (product == 0x2007)
        {
            layout = LAYOUT_NINTENDO_SWITCH_JOYCON_R;
        }
        else
        {
            layout = LAYOUT_NINTENDO_SWITCH_PRO;
        }
    }
    else if (vendor == 0x2dc8) /* 8BitDo */
    {
        layout = LAYOUT_NINTENDO_SWITCH_PRO;
    }
    else
    {
        /* For now we assume Xbox (0x045e), Generic will be used when
         * migrating to SDL_ActionSet
         */
        layout = LAYOUT_XBOX;
    }
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

static const char* sdlbutton_to_glyph(const SDL_GameControllerButton button)
{
    if (button > SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)
    {
        SDL_assert(0 && "Unhandled button!");
        return glyph[GLYPH_UNKNOWN];
    }

    return glyph_layout[layout][button];
}

static const char* glyph_for_vector(
    const std::vector<SDL_GameControllerButton>& buttons,
    const int index
) {
    if (index < 0 || index >= (int) buttons.size())
    {
        return NULL;
    }

    return sdlbutton_to_glyph(buttons[index]);
}

const char* BUTTONGLYPHS_get_button(const ActionSet actionset, const Action action, int binding)
{
    /* Given a specific action (like INTERACT in-game),
     * return either a (localized) keyboard key string like "ENTER" or "E",
     * or a controller button glyph from the table above like glyph[GLYPH_XBOX_Y],
     * to fill into strings like "Press {button} to activate terminal".
     *
     * Normally, set binding = -1. This will return the best keyboard key OR controller glyph.
     *
     * If binding >= 0, select a specific CONTROLLER binding glyph,
     * or NULL if the index is higher than the max binding index. */

    bool show_controller = binding >= 0 || !BUTTONGLYPHS_keyboard_is_active();
    if (binding < 0)
    {
        binding = 0;
    }

    switch (actionset)
    {
    case ActionSet_Menu:
        switch (action.Menu)
        {
        case Action_Menu_Accept:
            if (show_controller)
            {
                return glyph_for_vector(game.controllerButton_flip, binding);
            }
            return loc::gettext("ACTION");
        }
        break;
    case ActionSet_InGame:
        switch (action.InGame)
        {
        case Action_InGame_ACTION:
            if (show_controller)
            {
                return glyph_for_vector(game.controllerButton_flip, binding);
            }
            return loc::gettext("ACTION");

        case Action_InGame_Interact:
            if (show_controller)
            {
                /* FIXME: this really does depend on the Enter/E speedrunner option...
                 * This is messy, but let's not show the wrong thing here... */
                if (game.separate_interact)
                {
                    return glyph_for_vector(game.controllerButton_interact, binding);
                }
                return glyph_for_vector(game.controllerButton_map, binding);
            }
            if (game.separate_interact)
            {
                return "E";
            }
            return loc::gettext("ENTER");

        case Action_InGame_Map:
            if (show_controller)
            {
                return glyph_for_vector(game.controllerButton_map, binding);
            }
            return loc::gettext("ENTER");

        case Action_InGame_Esc:
            if (show_controller)
            {
                return glyph_for_vector(game.controllerButton_esc, binding);
            }
            return loc::gettext("ESC");

        case Action_InGame_Restart:
            if (show_controller)
            {
                return glyph_for_vector(game.controllerButton_restart, binding);
            }
            return "R";
        }
        break;
    }

    SDL_assert(0 && "Trying to get label/glyph for unknown action!");
    return glyph[GLYPH_UNKNOWN];
}

char* BUTTONGLYPHS_get_all_gamepad_buttons(
    char* buffer,
    size_t buffer_len,
    const ActionSet actionset,
    const int action
) {
    /* Gives a list of all controller bindings, for in the menu */
    Action union_action;
    union_action.intval = action;
    buffer[0] = '\0';
    size_t cur = 0;
    const char* glyph;
    int binding = 0;
    while ((glyph = BUTTONGLYPHS_get_button(actionset, union_action, binding)))
    {
        if (binding > 0 && buffer_len >= 1)
        {
            buffer[cur] = '/';
            cur++;
            buffer_len--;
        }

        size_t glyph_len = SDL_strlcpy(&buffer[cur], glyph, buffer_len);
        if (glyph_len >= buffer_len)
        {
            // Truncation occurred, we're done
            return buffer;
        }
        cur += glyph_len;
        buffer_len -= glyph_len;

        binding++;
    }
    return buffer;
}

} // extern "C"
