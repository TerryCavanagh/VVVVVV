#include <SDL.h>

#include "Constants.h"
#include "Font.h"
#include "Graphics.h"
#include "KeyPoll.h"
#include "UTF8.h"

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

    if (key.imebuffer_length > 0)
    {
        /* Also show a selection, so we need to know where and how long it is...
         * Because SDL gives us the number of "characters" (so, codepoints)... */
        const char* imebuffer_ptr = key.imebuffer.c_str();
        const char* sel_start_ptr = imebuffer_ptr;
        for (int i = 0; i < key.imebuffer_start; i++)
        {
            if (UTF8_next(&sel_start_ptr) == 0)
            {
                // Already past the '\0'...
                sel_start_ptr--;
                break;
            }
        }
        const char* sel_end_ptr = sel_start_ptr;
        for (int i = 0; i < key.imebuffer_length; i++)
        {
            if (UTF8_next(&sel_end_ptr) == 0)
            {
                break;
            }
        }
        size_t before_sel_nbytes = sel_start_ptr - imebuffer_ptr;
        size_t in_sel_nbytes = sel_end_ptr - sel_start_ptr;
        char* before_sel = (char*) SDL_malloc(before_sel_nbytes + 1);
        char* in_sel = (char*) SDL_malloc(in_sel_nbytes + 1);
        if (before_sel != NULL && in_sel != NULL)
        {
            SDL_memcpy(before_sel, imebuffer_ptr, before_sel_nbytes);
            before_sel[before_sel_nbytes] = '\0';
            SDL_memcpy(in_sel, sel_start_ptr, in_sel_nbytes);
            in_sel[in_sel_nbytes] = '\0';

            int before_sel_pixels = font::len(PR_FONT_LEVEL, before_sel) - 1;
            int in_sel_pixels = font::len(PR_FONT_LEVEL, in_sel);
            if (in_sel_pixels > 0)
            {
                in_sel_pixels += 1;
            }

            SDL_Rect selrect = imebox;
            selrect.x += before_sel_pixels + 1;
            selrect.w = in_sel_pixels;
            graphics.fill_rect(&selrect, 128, 64, 0);
        }
        SDL_free(before_sel);
        SDL_free(in_sel);
    }

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
