#include "Touch.h"

#include <SDL.h>
#include <vector>

#include "Constants.h"
#include "Graphics.h"
#include "GraphicsResources.h"
#include "KeyPoll.h"
#include "Screen.h"
#include "Script.h"
#include "UtilityClass.h"

namespace touch
{
    std::vector<VVV_Finger> fingers;
    TouchButton buttons[NUM_TOUCH_BUTTONS];
    float scale;

    int get_rect(TouchButton* button, SDL_Rect* rect)
    {
        rect->x = button->x;
        rect->y = button->y;
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

        return SDL_ceil(SDL_min(scale_x, scale_y) * scale);
    }

    void init(void)
    {
        scale = 1;

        for (int i = 0; i < NUM_TOUCH_BUTTONS; i++)
        {
            buttons[i].image = NULL;
            buttons[i].active = false;
            buttons[i].down = false;
            buttons[i].fingerId = -1;
        }
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

        // First, reset all buttons
        for (int i = 0; i < NUM_TOUCH_BUTTONS; i++)
        {
            buttons[i].active = false;
        }

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
            if (game.menustart)
            {
                buttons[TOUCH_BUTTON_LEFT].active = true;
                buttons[TOUCH_BUTTON_RIGHT].active = true;
                buttons[TOUCH_BUTTON_CANCEL].active = true;
                buttons[TOUCH_BUTTON_CONFIRM].active = true;
            }
            break;
        case TELEPORTERMODE:
            if (game.useteleporter)
            {
                buttons[TOUCH_BUTTON_LEFT].active = true;
                buttons[TOUCH_BUTTON_RIGHT].active = true;
                buttons[TOUCH_BUTTON_CANCEL].active = true;
                buttons[TOUCH_BUTTON_CONFIRM].active = true;
            }
            break;
        case MAPMODE:
            buttons[TOUCH_BUTTON_LEFT].active = true;
            buttons[TOUCH_BUTTON_RIGHT].active = true;
            buttons[TOUCH_BUTTON_CANCEL].active = true;
            buttons[TOUCH_BUTTON_CONFIRM].active = true;
            break;
        case GAMECOMPLETE:
        case GAMECOMPLETE2:
        case EDITORMODE:
        case PRELOADER:
        default:
            break;
        }
    }

    void render(void)
    {
        if (!key.using_touch)
        {
            return;
        }

        int scale = get_scale();
        refresh_buttons();

        for (int i = 0; i < NUM_TOUCH_BUTTONS; i++)
        {
            SDL_Rect rect;
            get_rect(&buttons[i], &rect);

            if (buttons[i].image != NULL && buttons[i].active)
            {
                graphics.draw_texture(buttons[i].image, rect.x, rect.y + (buttons[i].down ? 2 * scale : 0), scale, scale);
            }
        }
    }

    void reset(void)
    {
        for (int i = 0; i < NUM_TOUCH_BUTTONS; i++)
        {
            buttons[i].down = false;
            buttons[i].fingerId = -1;
        }

        for (int i = 0; i < fingers.size(); i++)
        {
            fingers[i].pressed = false;
            fingers[i].on_button = false;
        }
    }

    void update_buttons(void)
    {
        if (graphics.fademode != FADE_NONE)
        {
            return;
        }

        SDL_Point point;
        SDL_Rect rect;

        for (int buttonId = 0; buttonId < NUM_TOUCH_BUTTONS; buttonId++)
        {
            TouchButton* button = &buttons[buttonId];
            button->down = false;

            for (int fingerId = 0; fingerId < fingers.size(); fingerId++)
            {
                point.x = fingers[fingerId].x;
                point.y = fingers[fingerId].y;
                get_rect(button, &rect);

                if (SDL_PointInRect(&point, &rect) && button->active)
                {
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
        if (key.using_touch && buttons[button].active && buttons[button].down)
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

    bool screen_tapped(void)
    {
        for (int i = 0; i < fingers.size(); i++)
        {
            if (fingers[i].on_button)
            {
                continue;
            }

            return fingers[i].pressed;
        }
        return false;
    }
}
