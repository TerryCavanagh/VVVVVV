#include "Touch.h"

#include <SDL.h>
#include <string>
#include <vector>

#include "ButtonGlyphs.h"
#include "Constants.h"
#include "CustomLevels.h"
#include "Editor.h"
#include "Entity.h"
#include "FileSystemUtils.h"
#include "Font.h"
#include "Game.h"
#include "GlitchrunnerMode.h"
#include "Graphics.h"
#include "GraphicsResources.h"
#include "Input.h"
#include "Localization.h"
#include "KeyPoll.h"
#include "Map.h"
#include "Music.h"
#include "Screen.h"
#include "Script.h"
#include "UtilityClass.h"

namespace touch
{
    std::vector<VVV_Finger> fingers;
    TouchButton buttons[NUM_TOUCH_BUTTONS];
    std::vector<TouchButton> dynamic_buttons;
    std::vector<TouchButton*> all_buttons;
    bool use_buttons;
    int scale;
    bool textbox_style;
    bool scroll;

    void refresh_all_buttons(void)
    {
        all_buttons.clear();

        for (int i = 0; i < NUM_TOUCH_BUTTONS; i++)
        {
            all_buttons.push_back(&buttons[i]);
        }

        for (int i = 0; i < dynamic_buttons.size(); i++)
        {
            all_buttons.push_back(&dynamic_buttons[i]);
        }
    }

    int get_rect(TouchButton* button, SDL_Rect* rect)
    {
        rect->x = button->x;
        rect->y = button->y - scroll;
        rect->w = button->width;
        rect->h = button->height;

        return 0;
    }

    int get_scale(void)
    {
        SDL_Rect rect;
        graphics.get_stretch_info(&rect);

        int scale_x = rect.w / SCREEN_WIDTH_PIXELS;
        int scale_y = rect.h / SCREEN_HEIGHT_PIXELS;

        return SDL_ceil(SDL_min(scale_x, scale_y) * ((float) scale / 10.f));
    }

    void init(void)
    {
        scale = 10;
        use_buttons = false;
        textbox_style = false;

        for (int i = 0; i < NUM_TOUCH_BUTTONS; i++)
        {
            buttons[i].image = NULL;
            buttons[i].text = "";
            buttons[i].active = false;
            buttons[i].pressed = false;
            buttons[i].down = false;
            buttons[i].fingerId = -1;
            buttons[i].core = true;
            buttons[i].ui = true;
            buttons[i].type = TOUCH_BUTTON_TYPE_NONE;
            buttons[i].id = -1;
            buttons[i].disabled = false;
            buttons[i].checked = false;
            buttons[i].flags = 0;
        }

        refresh_all_buttons();
    }

    TouchButton create_button(int x, int y, int width, int height, std::string text)
    {
        TouchButton button;
        button.x = x;
        button.y = y;
        button.width = width;
        button.height = height;
        button.image = NULL;
        button.text = text;
        button.active = true;
        button.core = false;
        button.ui = false;
        button.down = false;
        button.pressed = false;
        button.fingerId = -1;
        button.type = TOUCH_BUTTON_TYPE_NONE;
        button.id = -1;
        button.disabled = false;
        button.checked = false;
        button.flags = 0;

        return button;
    }

    /* Helper function to create menu buttons (very common) in a single line */
    void create_menu_button(int x, int y, int width, int height, std::string text, int id)
    {
        TouchButton button = create_button(x, y, width, height, text);
        button.type = TOUCH_BUTTON_TYPE_MENU;
        button.id = id;

        register_button(button);
    }

    void create_menu_button_flags(int x, int y, int width, int height, std::string text, int id, Uint8 flags)
    {
        TouchButton button = create_button(x, y, width, height, text);
        button.type = TOUCH_BUTTON_TYPE_MENU;
        button.id = id;
        button.flags = flags;

        register_button(button);
    }

    void create_menu_button(int x, int y, int width, int height, std::string text, int id, bool active)
    {
        TouchButton button = create_button(x, y, width, height, text);
        button.type = TOUCH_BUTTON_TYPE_MENU;
        button.id = id;
        button.disabled = !active;

        register_button(button);
    }

    void create_slider_button(int x, int y, int width, int height, std::string text, int* var, int minvalue, int maxvalue)
    {
        TouchButton button = create_button(x, y, width, height, text);
        button.type = TOUCH_BUTTON_TYPE_MENU_SLIDER;
        button.id = -1;
        button.disabled = false;
        button.min = minvalue;
        button.max = maxvalue;
        button.var = var;

        register_button(button);
    }

    void create_toggle_button(int x, int y, int width, int height, std::string text, int id, bool checked)
    {
        TouchButton button = create_button(x, y, width, height, text);
        button.type = TOUCH_BUTTON_TYPE_MENU_TOGGLE;
        button.id = id;
        button.checked = checked;

        register_button(button);
    }

    void register_button(TouchButton button)
    {
        dynamic_buttons.push_back(button);

        refresh_all_buttons();
    }

    void remove_dynamic_buttons(void)
    {
        dynamic_buttons.clear();
        refresh_all_buttons();
    }

    void on_button_up(TouchButton* button)
    {
        bool version2_2 = GlitchrunnerMode_less_than_or_equal(Glitchrunner2_2);
        switch (button->type)
        {
        case TOUCH_BUTTON_TYPE_MENU_TOGGLE:
            button->checked = !button->checked;
            SDL_FALLTHROUGH;
        case TOUCH_BUTTON_TYPE_MENU:
        case TOUCH_BUTTON_TYPE_MENU_LANGUAGE:
            game.currentmenuoption = button->id;
            menuactionpress();
            break;
        case TOUCH_BUTTON_TYPE_MAP:
            switch (button->id)
            {
            case 0:
            case 1:
            case 2:
            case 3:
                game.menupage = button->id;
                music.playef(Sound_VIRIDIAN);
                break;
            case 4:
                game.menupage = 1;
                mapmenuactionpress(version2_2);
                break;
            case 5:
                game.menupage = 3;
                mapmenuactionpress(version2_2);
                break;
            case 6:
                music.playef(Sound_VIRIDIAN);
                game.menupage = 10;
                break;
            case 7:
                music.playef(Sound_VIRIDIAN);
                game.menupage = 3;
                break;
            case 8:
                game.menupage = 11;
                mapmenuactionpress(version2_2);
                break;
            case 9:
                game.menupage = 20;
                mapmenuactionpress(version2_2);
                break;
            case 10:
                game.menupage = 21;
                mapmenuactionpress(version2_2);
                break;
            }
            break;
        case TOUCH_BUTTON_TYPE_NONE:
        case TOUCH_BUTTON_TYPE_MENU_SLIDER:
        default:
            break;
        }
        refresh_buttons();
    }


    static void setup_map_buttons(void)
    {
        buttons[TOUCH_BUTTON_MAP_MAP].x = 16;
        buttons[TOUCH_BUTTON_MAP_MAP].y = 211;
        buttons[TOUCH_BUTTON_MAP_MAP].width = 56;
        buttons[TOUCH_BUTTON_MAP_MAP].height = 26;
        buttons[TOUCH_BUTTON_MAP_MAP].text = loc::gettext("MAP");
        buttons[TOUCH_BUTTON_MAP_MAP].id = 0;
        buttons[TOUCH_BUTTON_MAP_MAP].type = TOUCH_BUTTON_TYPE_MAP;
        buttons[TOUCH_BUTTON_MAP_MAP].ui = false;

        const char* tab_name;
        if (game.insecretlab)
        {
            tab_name = loc::gettext("GRAV");
        }
        else if (obj.flags[67] && !map.custommode)
        {
            tab_name = loc::gettext("SHIP");
        }
        else
        {
            tab_name = loc::gettext("CREW");
        }

        buttons[TOUCH_BUTTON_MAP_CREW].x = 92;
        buttons[TOUCH_BUTTON_MAP_CREW].y = 211;
        buttons[TOUCH_BUTTON_MAP_CREW].width = 56;
        buttons[TOUCH_BUTTON_MAP_CREW].height = 26;
        buttons[TOUCH_BUTTON_MAP_CREW].text = tab_name;
        buttons[TOUCH_BUTTON_MAP_CREW].id = 1;
        buttons[TOUCH_BUTTON_MAP_CREW].type = TOUCH_BUTTON_TYPE_MAP;
        buttons[TOUCH_BUTTON_MAP_CREW].ui = false;

        buttons[TOUCH_BUTTON_MAP_STATS].x = 168;
        buttons[TOUCH_BUTTON_MAP_STATS].y = 211;
        buttons[TOUCH_BUTTON_MAP_STATS].width = 56;
        buttons[TOUCH_BUTTON_MAP_STATS].height = 26;
        buttons[TOUCH_BUTTON_MAP_STATS].text = loc::gettext("STATS");
        buttons[TOUCH_BUTTON_MAP_STATS].id = 2;
        buttons[TOUCH_BUTTON_MAP_STATS].type = TOUCH_BUTTON_TYPE_MAP;
        buttons[TOUCH_BUTTON_MAP_STATS].ui = false;

        buttons[TOUCH_BUTTON_MAP_QUIT].x = 244;
        buttons[TOUCH_BUTTON_MAP_QUIT].y = 211;
        buttons[TOUCH_BUTTON_MAP_QUIT].width = 56;
        buttons[TOUCH_BUTTON_MAP_QUIT].height = 26;
        buttons[TOUCH_BUTTON_MAP_QUIT].text = loc::gettext("QUIT");
        buttons[TOUCH_BUTTON_MAP_QUIT].id = 3;
        buttons[TOUCH_BUTTON_MAP_QUIT].type = TOUCH_BUTTON_TYPE_MAP;
        buttons[TOUCH_BUTTON_MAP_QUIT].ui = false;

        buttons[TOUCH_BUTTON_MAP_SHIP_WARP].x = 80;
        buttons[TOUCH_BUTTON_MAP_SHIP_WARP].y = 104;
        buttons[TOUCH_BUTTON_MAP_SHIP_WARP].width = 160;
        buttons[TOUCH_BUTTON_MAP_SHIP_WARP].height = 30;
        buttons[TOUCH_BUTTON_MAP_SHIP_WARP].text = loc::gettext("warp to ship");
        buttons[TOUCH_BUTTON_MAP_SHIP_WARP].id = 4;
        buttons[TOUCH_BUTTON_MAP_SHIP_WARP].type = TOUCH_BUTTON_TYPE_MAP;
        buttons[TOUCH_BUTTON_MAP_SHIP_WARP].ui = false;

        buttons[TOUCH_BUTTON_MAP_QUIT_SAVE].x = 80;
        buttons[TOUCH_BUTTON_MAP_QUIT_SAVE].y = 80;
        buttons[TOUCH_BUTTON_MAP_QUIT_SAVE].width = 160;
        buttons[TOUCH_BUTTON_MAP_QUIT_SAVE].height = 26;
        buttons[TOUCH_BUTTON_MAP_QUIT_SAVE].text = loc::gettext("save");
        buttons[TOUCH_BUTTON_MAP_QUIT_SAVE].id = 5;
        buttons[TOUCH_BUTTON_MAP_QUIT_SAVE].type = TOUCH_BUTTON_TYPE_MAP;
        buttons[TOUCH_BUTTON_MAP_QUIT_SAVE].ui = false;

        buttons[TOUCH_BUTTON_MAP_QUIT_EXIT].x = 80;
        buttons[TOUCH_BUTTON_MAP_QUIT_EXIT].y = 112;
        buttons[TOUCH_BUTTON_MAP_QUIT_EXIT].width = 160;
        buttons[TOUCH_BUTTON_MAP_QUIT_EXIT].height = 26;
        buttons[TOUCH_BUTTON_MAP_QUIT_EXIT].text = loc::gettext("quit");
        buttons[TOUCH_BUTTON_MAP_QUIT_EXIT].id = 6;
        buttons[TOUCH_BUTTON_MAP_QUIT_EXIT].type = TOUCH_BUTTON_TYPE_MAP;
        buttons[TOUCH_BUTTON_MAP_QUIT_EXIT].ui = false;

        buttons[TOUCH_BUTTON_SUPERGRAVITRON_NO].x = 80;
        buttons[TOUCH_BUTTON_SUPERGRAVITRON_NO].y = 128 - 16;
        buttons[TOUCH_BUTTON_SUPERGRAVITRON_NO].width = 160;
        buttons[TOUCH_BUTTON_SUPERGRAVITRON_NO].height = 26;
        buttons[TOUCH_BUTTON_SUPERGRAVITRON_NO].text = loc::gettext("no, keep playing");
        buttons[TOUCH_BUTTON_SUPERGRAVITRON_NO].id = 9;
        buttons[TOUCH_BUTTON_SUPERGRAVITRON_NO].type = TOUCH_BUTTON_TYPE_MAP;
        buttons[TOUCH_BUTTON_SUPERGRAVITRON_NO].ui = false;

        buttons[TOUCH_BUTTON_SUPERGRAVITRON_YES].x = 80;
        buttons[TOUCH_BUTTON_SUPERGRAVITRON_YES].y = 128 + 16;
        buttons[TOUCH_BUTTON_SUPERGRAVITRON_YES].width = 160;
        buttons[TOUCH_BUTTON_SUPERGRAVITRON_YES].height = 26;
        buttons[TOUCH_BUTTON_SUPERGRAVITRON_YES].text = loc::gettext("yes, return");
        buttons[TOUCH_BUTTON_SUPERGRAVITRON_YES].id = 10;
        buttons[TOUCH_BUTTON_SUPERGRAVITRON_YES].type = TOUCH_BUTTON_TYPE_MAP;
        buttons[TOUCH_BUTTON_SUPERGRAVITRON_YES].ui = false;

        buttons[TOUCH_BUTTON_QUIT_NO].x = 80;
        buttons[TOUCH_BUTTON_QUIT_NO].y = 96 + 16;
        buttons[TOUCH_BUTTON_QUIT_NO].width = 160;
        buttons[TOUCH_BUTTON_QUIT_NO].height = 26;
        buttons[TOUCH_BUTTON_QUIT_NO].text = loc::gettext("no, keep playing");
        buttons[TOUCH_BUTTON_QUIT_NO].id = 7;
        buttons[TOUCH_BUTTON_QUIT_NO].type = TOUCH_BUTTON_TYPE_MAP;
        buttons[TOUCH_BUTTON_QUIT_NO].ui = false;

        buttons[TOUCH_BUTTON_QUIT_YES].x = 80;
        buttons[TOUCH_BUTTON_QUIT_YES].y = 96 + 32 + 16;
        buttons[TOUCH_BUTTON_QUIT_YES].width = 160;
        buttons[TOUCH_BUTTON_QUIT_YES].height = 26;
        buttons[TOUCH_BUTTON_QUIT_YES].text = loc::gettext("yes, quit to menu");
        buttons[TOUCH_BUTTON_QUIT_YES].id = 8;
        buttons[TOUCH_BUTTON_QUIT_YES].type = TOUCH_BUTTON_TYPE_MAP;
        buttons[TOUCH_BUTTON_QUIT_YES].ui = false;
    }

    void refresh_buttons(void)
    {
        int width;
        int height;
        int scale = get_scale();

        gameScreen.GetScreenSize(&width, &height);

        buttons[TOUCH_BUTTON_LEFT].x = 0;
        buttons[TOUCH_BUTTON_LEFT].y = height - (40 * scale) - 8;
        buttons[TOUCH_BUTTON_LEFT].width = 40 * scale;
        buttons[TOUCH_BUTTON_LEFT].height = 40 * scale;
        buttons[TOUCH_BUTTON_LEFT].image = graphics.grphx.im_button_left;

        buttons[TOUCH_BUTTON_RIGHT].x = (40 * scale) + 8;
        buttons[TOUCH_BUTTON_RIGHT].y = height - (40 * scale) - 8;
        buttons[TOUCH_BUTTON_RIGHT].width = 40 * scale;
        buttons[TOUCH_BUTTON_RIGHT].height = 40 * scale;
        buttons[TOUCH_BUTTON_RIGHT].image = graphics.grphx.im_button_right;

        buttons[TOUCH_BUTTON_MAP].x = width - (35 * scale);
        buttons[TOUCH_BUTTON_MAP].y = 0;
        buttons[TOUCH_BUTTON_MAP].width = 35 * scale;
        buttons[TOUCH_BUTTON_MAP].height = 30 * scale;
        buttons[TOUCH_BUTTON_MAP].image = graphics.grphx.im_button_map;

        buttons[TOUCH_BUTTON_CANCEL].x = width - (40 * scale);
        buttons[TOUCH_BUTTON_CANCEL].y = height - (40 * scale * 2) - 16;
        buttons[TOUCH_BUTTON_CANCEL].width = 40 * scale;
        buttons[TOUCH_BUTTON_CANCEL].height = 40 * scale;
        buttons[TOUCH_BUTTON_CANCEL].image = graphics.grphx.im_button_left;

        buttons[TOUCH_BUTTON_CONFIRM].x = width - (40 * scale);
        buttons[TOUCH_BUTTON_CONFIRM].y = height - (40 * scale) - 8;
        buttons[TOUCH_BUTTON_CONFIRM].width = 40 * scale;
        buttons[TOUCH_BUTTON_CONFIRM].height = 40 * scale;
        buttons[TOUCH_BUTTON_CONFIRM].image = graphics.grphx.im_button_right;

        buttons[TOUCH_BUTTON_MAP_BACK].x = width - ((double)(240 - (int) graphics.lerp(graphics.oldmenuoffset, graphics.menuoffset)) / 240.0) * (60 * scale);
        buttons[TOUCH_BUTTON_MAP_BACK].y = 8;
        buttons[TOUCH_BUTTON_MAP_BACK].width = 60 * scale;
        buttons[TOUCH_BUTTON_MAP_BACK].height = 26 * scale;
        buttons[TOUCH_BUTTON_MAP_BACK].text = loc::gettext("BACK");

        setup_map_buttons();

        // First, reset all buttons
        for (int i = 0; i < NUM_TOUCH_BUTTONS; i++)
        {
            buttons[i].active = false;
        }

        use_buttons = true;

        // Now, set the buttons that are active

        switch (game.gamestate)
        {
        case GAMEMODE:
            if (!script.running && game.hascontrol)
            {
                buttons[TOUCH_BUTTON_LEFT].active = true;
                buttons[TOUCH_BUTTON_RIGHT].active = true;
                buttons[TOUCH_BUTTON_MAP].active = true;
            }
            break;

        case TITLEMODE:
            if (!game.menustart)
            {
                use_buttons = false;
            }
            break;
        case TELEPORTERMODE:
            if (game.useteleporter)
            {
                buttons[TOUCH_BUTTON_LEFT].active = true;
                buttons[TOUCH_BUTTON_RIGHT].active = true;
                buttons[TOUCH_BUTTON_CONFIRM].active = true;

                buttons[TOUCH_BUTTON_MAP_BACK].active = true;
            }
            break;
        case MAPMODE:
            buttons[TOUCH_BUTTON_MAP_BACK].active = true;
            if (game.menupage >= 0 && game.menupage < 4)
            {
                buttons[TOUCH_BUTTON_MAP_MAP].active = true;
                buttons[TOUCH_BUTTON_MAP_CREW].active = true;
                buttons[TOUCH_BUTTON_MAP_STATS].active = true;
                buttons[TOUCH_BUTTON_MAP_QUIT].active = true;
            }

            if (graphics.menuoffset > 0)
            {
                buttons[TOUCH_BUTTON_MAP_BACK].down = true;
            }

            switch (game.menupage)
            {
            case 0:
            case 2:
                break;
            case 1:
                if (!game.insecretlab && obj.flags[67] && !map.custommode)
                {
                    buttons[TOUCH_BUTTON_MAP_SHIP_WARP].active = true;
                }
                break;
            case 20:
            case 21:
                buttons[TOUCH_BUTTON_SUPERGRAVITRON_YES].active = true;
                buttons[TOUCH_BUTTON_SUPERGRAVITRON_NO].active = true;
                buttons[TOUCH_BUTTON_MAP_BACK].active = false;
                break;
            case 3:
                if (!game.gamesaved && !game.gamesavefailed && !game.inspecial())
                {
                    buttons[TOUCH_BUTTON_MAP_QUIT_SAVE].active = true;
                    buttons[TOUCH_BUTTON_MAP_QUIT_EXIT].y = 112;
                }
                else
                {
                    buttons[TOUCH_BUTTON_MAP_QUIT_EXIT].y = 168;
                }
                buttons[TOUCH_BUTTON_MAP_QUIT_EXIT].active = true;
                break;
            case 10:
            case 11:
                buttons[TOUCH_BUTTON_QUIT_NO].active = true;
                buttons[TOUCH_BUTTON_QUIT_YES].active = true;
                break;
            default:
                buttons[TOUCH_BUTTON_LEFT].active = true;
                buttons[TOUCH_BUTTON_RIGHT].active = true;
                break;
            }
            break;
        case PRELOADER:
            use_buttons = false;
            break;
        case GAMECOMPLETE:
        case GAMECOMPLETE2:
        case EDITORMODE:
        default:
            break;
        }
    }

    void update_sliders()
    {
        SDL_Rect stretch_rect;
        graphics.get_stretch_info(&stretch_rect);

        for (int i = 0; i < all_buttons.size(); i++)
        {
            TouchButton* button = all_buttons[i];

            if (button->type == TOUCH_BUTTON_TYPE_MENU_SLIDER && button->pressed)
            {
                int value = *button->var;
                int range = button->max - button->min;
                float percent = (float) (value - button->min) / range;

                int finger_x = (fingers[button->fingerId].x - stretch_rect.x) * SCREEN_WIDTH_PIXELS / stretch_rect.w;

                int newvalue = button->min + (int) ((finger_x - button->x) / (float)button->width * range);
                if (newvalue < button->min)
                {
                    newvalue = button->min;
                }
                if (newvalue > button->max)
                {
                    newvalue = button->max;
                }

                *button->var = newvalue;
            }
        }
    }

    void render_buttons(int scale, bool ui, int r, int g, int b)
    {
        for (int i = 0; i < all_buttons.size(); i++)
        {
            TouchButton* button = all_buttons[i];

            if (button->active && (button->ui == ui))
            {
                if (button->image != NULL)
                {
                    graphics.draw_texture(button->image, button->x, button->y + (button->down ? 2 * scale : 0), scale, scale);
                }
                else
                {
                    int use_r = button->disabled ? 127 : r;
                    int use_g = button->disabled ? 127 : g;
                    int use_b = button->disabled ? 127 : b;

                    if (button->type == TOUCH_BUTTON_TYPE_MAP)
                    {
                        if (game.menupage != button->id)
                        {
                            use_r /= 2;
                            use_g /= 2;
                            use_b /= 2;
                        }
                    }

                    float shadow_div = 4;
                    float inner_div = 1.5;

                    if (textbox_style)
                    {
                        shadow_div = 6;
                        inner_div = 6;
                    }

                    int offset = (button->down) ? 1 : 0;

                    int font_scale = (SDL_min((scale - 1), 7) << 0);
                    int height = font::height(PR_CJK_LOW | font_scale | button->flags);

                    switch (button->type)
                    {
                    case TOUCH_BUTTON_TYPE_MENU_SLIDER:
                    {
                        // Find where the slider position is!
                        int value = *button->var;
                        int range = button->max - button->min;
                        float percent = (float) (value - button->min) / range;
                        int sliderpos = (int) ((button->width - 10) * percent);

                        // Draw track
                        graphics.fill_rect(button->x * scale + 2, (button->y + (button->height / 2)) * scale, button->width, 4, use_r / shadow_div, use_g / shadow_div, use_b / shadow_div);
                        graphics.fill_rect(button->x * scale, (button->y + (button->height / 2) - 2) * scale, button->width, 4, use_r / inner_div, use_g / inner_div, use_b / inner_div);

                        // Draw slider
                        graphics.fill_rect((button->x + sliderpos + 2) * scale, (button->y + (button->height / 2) - 3) * scale, 10, 10, use_r / shadow_div, use_g / shadow_div, use_b / shadow_div);
                        graphics.fill_rect((button->x + sliderpos) * scale, (button->y + (button->height / 2) - 5) * scale, 10, 10, use_r, use_g, use_b);
                        graphics.fill_rect((button->x + sliderpos + 1) * scale, (button->y + (button->height / 2) - 4) * scale, 8, 8, use_r / inner_div, use_g / inner_div, use_b / inner_div);


                        font::print(PR_CEN | PR_CJK_LOW | font_scale | button->flags, button->x + (button->width / 2) * scale, button->y * scale, button->text, use_r, use_g, use_b);
                        break;
                    }
                    case TOUCH_BUTTON_TYPE_MENU_TOGGLE:
                        graphics.draw_rect(button->x + offset * scale, button->y + offset * scale, 10, 10, use_r, use_g, use_b);
                        if (button->checked)
                        {
                            graphics.fill_rect(button->x + 2 * scale + offset * scale, button->y + 2 * scale + offset * scale, 6, 6, use_r, use_g, use_b);
                        }

                        font::print(PR_CJK_LOW | font_scale | button->flags, button->x + 16 + offset * scale, button->y + ((button->height - height) / 2 + offset) * scale, button->text, use_r, use_g, use_b);
                        break;
                    default:
                        // This is a dumb hack for the language menu, but... if this button is NOT for the current language, darken it (unless it's negative)

                        if (game.currentmenuname == Menu::language)
                        {
                            if (button->id != game.currentmenuoption && button->id >= 0)
                            {
                                use_r /= 2;
                                use_g /= 2;
                                use_b /= 2;
                            }
                        }

                        graphics.fill_rect(button->x + 4 * scale, button->y + 4 * scale, button->width, button->height, r / shadow_div, g / shadow_div, b / shadow_div);

                        graphics.fill_rect(button->x + offset * scale, button->y + offset * scale, button->width, button->height, use_r, use_g, use_b);
                        graphics.fill_rect(button->x + (offset + 2) * scale, button->y + (2 + offset) * scale, button->width - 4 * scale, button->height - 4 * scale, use_r / inner_div, use_g / inner_div, use_b / inner_div);
                        if (button->type == TOUCH_BUTTON_TYPE_MENU_LANGUAGE)
                        {
                            graphics.set_texture_color_mod(graphics.grphx.im_button_globe, 196, 196, 255 - help.glow);
                            graphics.draw_texture(graphics.grphx.im_button_globe, (button->x + 4 + offset) * scale, (button->y + 4 + offset) * scale, scale, scale);
                            graphics.set_texture_color_mod(graphics.grphx.im_button_globe, 255, 255, 255);
                        }
                        else
                        {
                            font::print(PR_CEN | PR_CJK_LOW | font_scale | button->flags, button->x + (button->width / 2) + offset * scale, button->y + ((button->height - height) / 2 + offset * scale), button->text, 196, 196, 255 - help.glow);
                        }
                        break;
                    }
                }
            }
        }
    }

    void render_buttons(void)
    {
        render_buttons(64, 184, 208);
    }

    void render_buttons(int r, int g, int b)
    {
        if (!key.using_touch)
        {
            return;
        }

        render_buttons(1, false, r, g, b);
    }

    void render_buttons(int r, int g, int b, bool textbox_style)
    {
        touch::textbox_style = textbox_style;
        render_buttons(r, g, b);
        touch::textbox_style = false;
    }

    void render(void)
    {
        if (!key.using_touch)
        {
            return;
        }

        int scale = get_scale();
        refresh_buttons();

        render_buttons(scale, true, 64, 184, 208);
    }

    void reset(void)
    {
        for (int i = 0; i < fingers.size(); i++)
        {
            fingers[i].pressed = false;
            fingers[i].on_button = false;
        }
    }

    void on_menu_create(void)
    {
        scroll = 0;
    }

    void update_buttons(void)
    {
        if (!use_buttons || graphics.fademode != FADE_NONE)
        {
            return;
        }


        SDL_Rect stretch_rect;
        graphics.get_stretch_info(&stretch_rect);

        SDL_Point point;
        SDL_Rect rect;

        for (int buttonId = 0; buttonId < all_buttons.size(); buttonId++)
        {
            TouchButton* button = all_buttons[buttonId];
            button->down = false;

            for (int fingerId = 0; fingerId < fingers.size(); fingerId++)
            {
                if (button->ui)
                {
                    point.x = fingers[fingerId].x;
                    point.y = fingers[fingerId].y;
                }
                else
                {
                    point.x = (fingers[fingerId].x - stretch_rect.x) * SCREEN_WIDTH_PIXELS / stretch_rect.w;
                    point.y = (fingers[fingerId].y - stretch_rect.y) * SCREEN_HEIGHT_PIXELS / stretch_rect.h;
                }
                get_rect(button, &rect);

                if (SDL_PointInRect(&point, &rect) && button->active && !button->disabled)
                {
                    if (fingers[fingerId].pressed)
                    {
                        button->pressed = true;
                    }

                    button->down = true;
                    button->fingerId = fingers[fingerId].id;
                    fingers[fingerId].on_button = true;
                    break;
                }
            }
        }
    }

    bool button_tapped(TouchButtonID button)
    {
        if (use_buttons && key.using_touch && buttons[button].active && buttons[button].down && !buttons[button].disabled)
        {
            for (int i = 0; i < fingers.size(); i++)
            {
                if (fingers[i].id == buttons[button].fingerId)
                {
                    return fingers[i].pressed;
                }
            }
        }
        return false;
    }

    bool touching_right(void)
    {
        int width;
        int height;
        gameScreen.GetScreenSize(&width, &height);

        for (int i = 0; i < fingers.size(); i++)
        {
            if (fingers[i].on_button)
            {
                continue;
            }

            if (fingers[i].x > width / 2)
            {
                return true;
            }
        }
        return false;
    }

    bool screen_down(void)
    {
        for (int i = 0; i < fingers.size(); i++)
        {
            if (fingers[i].on_button && use_buttons)
            {
                continue;
            }

            if (fingers[i].pressed)
            {
                // Consume the input, so we don't accidentally start pressing a button or anything
                fingers[i].pressed = false;
            }
            return true;
        }
        return false;
    }
}
