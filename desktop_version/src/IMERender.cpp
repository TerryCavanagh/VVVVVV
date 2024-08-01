#include <SDL.h>

#include "Constants.h"
#include "Font.h"
#include "Graphics.h"
#include "KeyPoll.h"

static bool render_done = false;
static SDL_Rect imebox;

void ime_render(void)
{
    render_done = false;

    if (!SDL_IsTextInputActive() || key.imebuffer == "")
    {
        return;
    }

    int fontheight = font::height(PR_FONT_LEVEL);
    imebox.x = 8;
    imebox.y = SCREEN_HEIGHT_PIXELS - 32 - fontheight;
    imebox.w = font::len(PR_FONT_LEVEL, key.imebuffer.c_str()) + 1;
    imebox.h = fontheight + 1;

    SDL_Rect imebox_border = imebox;
    imebox_border.x -= 1;
    imebox_border.y -= 1;
    imebox_border.w += 2;
    imebox_border.h += 2;

    graphics.fill_rect(&imebox_border, 128, 128, 128);
    graphics.fill_rect(&imebox, 0, 0, 0);
    font::print(PR_FONT_LEVEL | PR_CJK_LOW, imebox.x + 1, imebox.y + 1, key.imebuffer, 255, 255, 255);

    render_done = true;
}

void ime_set_rect(SDL_Rect* stretch_info)
{
    if (!render_done)
    {
        return;
    }

    SDL_Rect imebox_scaled = imebox;
    float x_scale = (float) stretch_info->w / SCREEN_WIDTH_PIXELS;
    float y_scale = (float) stretch_info->h / SCREEN_HEIGHT_PIXELS;
    imebox_scaled.x *= x_scale;
    imebox_scaled.y *= y_scale;
    imebox_scaled.w *= x_scale;
    imebox_scaled.h *= y_scale;
    imebox_scaled.x += stretch_info->x;
    imebox_scaled.y += stretch_info->y;

    SDL_SetTextInputRect(&imebox_scaled);
}
