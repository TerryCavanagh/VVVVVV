#include "RoomnameTranslator.h"

#include "Constants.h"
#include "Font.h"
#include "Game.h"
#include "Graphics.h"
#include "GraphicsUtil.h"
#include "KeyPoll.h"
#include "LevelDebugger.h"
#include "Localization.h"
#include "LocalizationMaint.h"
#include "Map.h"
#include "Script.h"
#include "UtilityClass.h"
#include "VFormat.h"


namespace roomname_translator
{
    bool enabled = false;
    bool edit_mode = false;
    bool expl_mode = false;
    bool help_screen = false;

    SDL_Surface* dimbuffer;
    SDL_Rect fullscreen_rect;

    void set_enabled(bool value)
    {
        enabled = value;
        edit_mode = false;
    }

    bool is_pausing(void)
    {
        return enabled && edit_mode;
    }

    static void print_explanation(const char* explanation)
    {
        const char* use_explanation = explanation;
        if (explanation == NULL || explanation[0] == '\0')
        {
            use_explanation = "[explanation not yet set]";
        }
        else if (SDL_strcmp(explanation, ".") == 0)
        {
            use_explanation = "[no explanation]";
        }
        font::print_wrap(PR_BOR | PR_FONT_8X8, 0, 10, use_explanation, 0,192,255, 8, 320);
    }

    void overlay_render(bool* force_roomname_hidden, bool* roomname_untranslated, int* roomname_r, int* roomname_g, int* roomname_b)
    {
        if (level_debugger::is_active())
        {
            return;
        }

        if (edit_mode || help_screen)
        {
            fullscreen_rect.x = 0;
            fullscreen_rect.y = 0;
            fullscreen_rect.w = 320;
            fullscreen_rect.h = 240;
            graphics.set_blendmode(SDL_BLENDMODE_BLEND);
            graphics.fill_rect(graphics.getRGBA(0, 0, 0, 96));
            graphics.set_blendmode(SDL_BLENDMODE_NONE);
            if (help_screen)
            {
                font::print(PR_BOR | PR_FONT_8X8, 0, 0, "=== Room name translation mode help ===", 255,255,255);
                if (expl_mode)
                {
                    font::print_wrap(
                        PR_BOR | PR_FONT_8X8,
                        0, 20,
                        "You can currently access EXPL mode to\n"
                        "set explanations for room names.\n"
                        "(Use Ctrl+E to switch to NAME mode.)",
                        255,255,255
                    );

                    const char* first_part = "Unexplained room names are ";
                    font::print(PR_BOR | PR_FONT_8X8, 0, 60, first_part, 255,255,255);
                    font::print(PR_BOR | PR_FONT_8X8, font::len(PR_FONT_8X8, first_part), 60, "cyan.", 64, 255, 255-help.glow);
                }
                else
                {
                    font::print_wrap(
                        PR_BOR | PR_FONT_8X8,
                        0, 20,
                        "You can currently access NAME mode to\n"
                        "translate room names.\n"
                        "(Use Ctrl+E to switch to EXPL mode.)",
                        255,255,255
                    );

                    const char* first_part = "English room names are ";
                    font::print(PR_BOR | PR_FONT_8X8, 0, 60, first_part, 255,255,255);
                    font::print(PR_BOR | PR_FONT_8X8, font::len(PR_FONT_8X8, first_part), 60, "cyan.", 0, 192, 255-help.glow);
                }
                font::print(PR_BOR | PR_FONT_8X8, 0, 80, "KEYS:", 255,255,255);
                if (expl_mode)
                {
                    font::print(PR_BOR | PR_FONT_8X8, 0, 90, "Tab - switch between play/expl modes", 255,255,255);
                    font::print(PR_BOR | PR_FONT_8X8, 0, 100, "E/Enter - set room name explanation", 255,255,255);
                }
                else
                {
                    font::print(PR_BOR | PR_FONT_8X8, 0, 90, "Tab - switch between play/name modes", 255,255,255);
                    font::print(PR_BOR | PR_FONT_8X8, 0, 100, "E/Enter - set room name translation", 255,255,255);
                }
                font::print(PR_BOR | PR_FONT_8X8, 0, 110, "I - toggle invincibility", 255,255,255);
                if (expl_mode)
                {
                    font::print(PR_BOR | PR_FONT_8X8, 0, 120, ". - set blank explanation", 255,255,255);
                }
                *force_roomname_hidden = true;
                return;
            }
            else if (expl_mode)
            {
                font::print(PR_BOR | PR_FONT_8X8, 0, 0, "Expl mode [TAB]", 255,255,255);
            }
            else
            {
                font::print(PR_BOR | PR_FONT_8X8, 0, 0, "Name mode [TAB]", 255,255,255);
            }
        }
        else
        {
            font::print(PR_BOR | PR_FONT_8X8, 0, 0, "Play mode [TAB]", 255,255,255);
            font::print(PR_BOR | PR_FONT_8X8, 320-64, 10, "F1: Help", 192,192,192);
        }

        char buffer[SCREEN_WIDTH_CHARS + 1];

        int n_left;
        if (expl_mode)
        {
            n_left = map.custommode ? loc::n_unexplained_roomnames_custom : loc::n_unexplained_roomnames;
        }
        else
        {
            n_left = map.custommode ? loc::n_untranslated_roomnames_custom : loc::n_untranslated_roomnames;
        }

        vformat_buf(buffer, sizeof(buffer), "{n|digits=3|spaces} left", "n:int", n_left);
        font::print(PR_BOR | PR_FONT_8X8, 144, 0, buffer, 255,255,255);

        if (map.invincibility)
        {
            font::print(PR_BOR | PR_FONT_8X8, 224, 0, "INV", 255,255,128);
        }

        vformat_buf(buffer, sizeof(buffer),
            "({x|digits=2|spaces},{y|digits=2|spaces})",
            "x:int, y:int",
            game.roomx % 100, game.roomy % 100
        );
        font::print(PR_BOR | PR_FONT_8X8, 320-56, 0, buffer, 255,255,255);

        if (map.roomname_special)
        {
            if (edit_mode)
            {
                print_explanation("This is a special room name, which cannot be translated in-game. Please see roomnames_special");
            }
        }
        else if (map.roomname[0] == '\0')
        {
            // No room name at all, so no translation/explanation interface
            if (edit_mode)
            {
                font::print(PR_CEN | PR_BOR | PR_FONT_8X8, -1, 229-font::height(PR_FONT_LEVEL), "[no roomname]", 0,192,255);
            }
        }
        else if (!expl_mode)
        {
            // Name mode affects play mode a bit as well...
            bool roomname_is_translated = loc::get_roomname_translation(map.custommode, game.roomx, game.roomy)[0] != '\0';
            *roomname_untranslated = !roomname_is_translated;

            if (edit_mode)
            {
                const char* english_roomname = map.roomname;

                font::print(PR_CEN | PR_BOR | PR_FONT_8X8, -1, 229-font::height(PR_FONT_LEVEL), english_roomname, 0,192,255);

                print_explanation(loc::get_roomname_explanation(map.custommode, game.roomx, game.roomy));

                if (key.textentry())
                {
                    *force_roomname_hidden = true;
                    graphics.render_roomname(PR_FONT_LEVEL, key.keybuffer.c_str(), 255,255,255);
                    int name_w = font::len(PR_FONT_LEVEL, key.keybuffer.c_str());
                    font::print(PR_BOR | PR_FONT_LEVEL | PR_RTL_XFLIP, (320-name_w)/2+name_w, 231, "_", 255,255,255);
                }
                else if (!roomname_is_translated)
                {
                    *force_roomname_hidden = true;
                    graphics.render_roomname(PR_FONT_8X8, "[no translation]", 255,255,128);
                }
            }
            else if (!roomname_is_translated)
            {
                *roomname_r = 0;
                *roomname_g = 192;
                *roomname_b = 255 - help.glow;
            }
        }
        else
        {
            // Explanation mode!
            const char* explanation = loc::get_roomname_explanation(map.custommode, game.roomx, game.roomy);
            if (explanation[0] == '\0')
            {
                *roomname_r = 64;
                *roomname_g = 255;
                *roomname_b = 255 - help.glow;
            }

            if (edit_mode)
            {
                if (key.textentry())
                {
                    print_explanation((key.keybuffer + "_").c_str());

                    font::print_wrap(PR_BOR | PR_FONT_8X8, 0, 90, "Use \".\" to set no explanation", 255,255,255, 8, 320);
                }
                else
                {
                    print_explanation(explanation);
                }
            }
        }
    }


    static bool key_pressed_once(SDL_Keycode keyc, bool* held)
    {
        if (key.isDown(keyc))
        {
            if (!*held)
            {
                *held = true;
                return true;
            }
        }
        else
        {
            *held = false;
        }

        return false;
    }

    static void save_explanation(const char* explanation, const char* success_message)
    {
        if (loc::save_roomname_explanation_to_files(map.custommode, game.roomx, game.roomy, explanation))
        {
            graphics.createtextboxflipme(success_message, -1, 176, TEXT_COLOUR("gray"));
            graphics.textboxprintflags(PR_FONT_8X8);
            graphics.textboxcenterx();
            graphics.textboxtimer(25);
        }
        else
        {
            graphics.createtextboxflipme("ERROR: Could not save to all langs!", -1, 176, TEXT_COLOUR("red"));
            graphics.textboxprintflags(PR_FONT_8X8);
            graphics.textboxcenterx();
            graphics.textboxtimer(50);
        }
    }

    static void save_translation(const char* translation)
    {
        if (loc::save_roomname_to_file(loc::lang, map.custommode, game.roomx, game.roomy, translation, NULL))
        {
            graphics.createtextboxflipme("Translation saved!", -1, 176, TEXT_COLOUR("gray"));
            graphics.textboxprintflags(PR_FONT_8X8);
            graphics.textboxcenterx();
            graphics.textboxtimer(25);
        }
        else
        {
            graphics.createtextboxflipme("ERROR: Could not save!", -1, 144, TEXT_COLOUR("red"));
            graphics.addline("");
            graphics.addline("1) Do the language files exist?");
            graphics.addline("2) Make sure there is no \"lang\"");
            graphics.addline("   folder next to the regular saves.");
            graphics.textboxprintflags(PR_FONT_8X8);
            graphics.textboxcenterx();
            graphics.textboxtimer(180);
        }
    }

    bool held_tab = false;
    bool held_i = false;
    bool held_escape = false;
    bool held_return = false;
    bool held_e = false;
    bool held_period = false;
    bool held_f1 = false;

    bool overlay_input(void)
    {
        // Returns true if input "caught" and should not go to gameinput

        if (key.isDown(SDLK_ESCAPE) && held_escape)
        {
            // Avoid opening the pause menu
            return true;
        }
        else
        {
            held_escape = false;
        }

        if (help_screen)
        {
            if ((key.isDown(SDLK_LCTRL) || key.isDown(SDLK_RCTRL)) && key_pressed_once(SDLK_e, &held_e))
            {
                expl_mode = !expl_mode;
            }

            if (key_pressed_once(SDLK_ESCAPE, &held_escape) || key_pressed_once(SDLK_F1, &held_f1))
            {
                help_screen = false;
            }

            return true;
        }

        if (key.textentry())
        {
            if (key_pressed_once(SDLK_ESCAPE, &held_escape))
            {
                // Without saving
                key.disabletextentry();
            }

            if (key_pressed_once(SDLK_RETURN, &held_return))
            {
                key.disabletextentry();

                if (!expl_mode)
                {
                    if (loc::lang == "en")
                    {
                        graphics.createtextboxflipme("ERROR: Can't add EN-EN translation", -1, 176, TEXT_COLOUR("red"));
                        graphics.textboxprintflags(PR_FONT_8X8);
                        graphics.textboxcenterx();
                        graphics.textboxtimer(50);
                    }
                    else
                    {
                        save_translation(key.keybuffer.c_str());
                    }
                }
                else
                {
                    save_explanation(key.keybuffer.c_str(), "Explanation saved!");
                }

                edit_mode = false;
                game.mapheld = true;
            }

            return true;
        }

        if (key_pressed_once(SDLK_F1, &held_f1))
        {
            help_screen = true;
        }

        if (key_pressed_once(SDLK_TAB, &held_tab) && !level_debugger::is_active())
        {
            edit_mode = !edit_mode;
        }

        if (key_pressed_once(SDLK_i, &held_i))
        {
            if (game.intimetrial)
            {
                // We'll let you enable it in a time trial, but with a twist.
                if (!game.timetrialcheater)
                {
                    game.sabotage_time_trial();
                }
            }
            else if (game.incompetitive())
            {
                return edit_mode;
            }

            map.invincibility = !map.invincibility;
        }

        if (edit_mode)
        {
            if (key_pressed_once(SDLK_ESCAPE, &held_escape))
            {
                edit_mode = false;
                return true;
            }

            if ((key.isDown(SDLK_LCTRL) || key.isDown(SDLK_RCTRL)) && key_pressed_once(SDLK_e, &held_e))
            {
                expl_mode = !expl_mode;
                return true;
            }

            if (key_pressed_once(SDLK_RETURN, &held_return) || key_pressed_once(SDLK_e, &held_e))
            {
                if (map.roomname_special || map.roomname[0] == '\0')
                {
                    return true;
                }

                key.enabletextentry();
                if (!expl_mode)
                {
                    key.keybuffer = loc::get_roomname_translation(map.custommode, game.roomx, game.roomy);
                }
                else
                {
                    key.keybuffer = loc::get_roomname_explanation(map.custommode, game.roomx, game.roomy);
                }
            }

            if (expl_mode && key_pressed_once(SDLK_PERIOD, &held_period))
            {
                const char* old_explanation = loc::get_roomname_explanation(map.custommode, game.roomx, game.roomy);
                const char* new_explanation = NULL;
                const char* success_message;
                if (old_explanation[0] == '\0')
                {
                    new_explanation = ".";
                    success_message = "Blank explanation set!";
                }
                else if (SDL_strcmp(old_explanation, ".") == 0)
                {
                    new_explanation = "";
                    success_message = "Blank explanation deleted!";
                }

                if (new_explanation != NULL)
                {
                    save_explanation(new_explanation, success_message);
                }
            }
        }

        return edit_mode;
    }
}
