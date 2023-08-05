#include "Textbox.h"

#include <SDL.h>

#include "Font.h"
#include "UTF8.h"

textboxclass::textboxclass(void)
{
    w = 0;
    h = 0;
    tl = 0;
    prev_tl = 0;
    tm = 0;
    timer = 0;

    xp = 0;
    yp = 0;
    r = 0;
    g = 0;
    b = 0;

    flipme = false;

    rand = 0;

    large = false;

    print_flags = PR_FONT_LEVEL;
    fill_buttons = false;

    sprites.clear();
}

void textboxclass::addsprite(int x, int y, int tile, int col)
{
    TextboxSprite sprite;
    sprite.x = x;
    sprite.y = y;
    sprite.tile = tile;
    sprite.col = col;
    sprites.push_back(sprite);
}

void textboxclass::centerx(void)
{
    resize();
    xp = 160 - (w / 2);
    resize();
}
void textboxclass::centery(void)
{
    resize();
    yp = 120 - (h / 2);
    resize();
}

void textboxclass::adjust(void)
{
    resize();
    if (xp < 10) xp = 10;
    if (yp < 10) yp = 10;
    if (xp + w > 310) xp = 310 - w;
    if (yp + h > 230) yp = 230 - h;
    resize();
}

void textboxclass::initcol(int rr, int gg, int bb)
{
    r = rr;
    g = gg;
    b = bb;
    tl = 0.5;
}

void textboxclass::update(void)
{
    prev_tl = tl;
    if (tm == 0)
    {
        tl += .1f;
        if (tl >= 1)
        {
            tl = 1;
            tm = 1;
        }
    }
    else if (tm == 2)
    {
        tl -= .1f;
        if (tl <= 0.5)
        {
            tl = 0.5;
            //this textbox will be removed by updatetextboxes() later
        }
    }
    if (timer > 0)
    {
        timer--;
        if (timer == 0) tm = 2;
    }
}

void textboxclass::remove(void)
{
    tm = 2;
    tl = 1.0f; //Remove mode
}

void textboxclass::removefast(void)
{
    tm = 2;
    tl = 0.4f; //Remove mode
}

void textboxclass::resize(void)
{
    //Set the width and height to the correct sizes
    int max = 0;
    for (size_t iter = 0; iter < lines.size(); iter++)
    {
        int len = font::len(print_flags, lines[iter].c_str());
        if (len > max) max = len;
    }

    // 16 for the borders
    w = max + 16;
    h = lines.size()*font::height(print_flags) + 16;
}

void textboxclass::addline(const std::string& t)
{
    lines.push_back(t);
    resize();
    if ((int)lines.size() > (large ? 26 : 11))
    {
        lines.clear();
    }
}

void textboxclass::pad(size_t left_pad, size_t right_pad)
{
    // Pad the current text with a certain number of spaces on the left and right
    for (size_t iter = 0; iter < lines.size(); iter++)
    {
        lines[iter] = std::string(left_pad, ' ') + lines[iter] + std::string(right_pad, ' ');
    }
    resize();
}

void textboxclass::padtowidth(size_t new_w)
{
    /* Pad the current text so that each line is new_w pixels wide.
     * Each existing line is centered in that width. */
    resize();
    uint8_t glyph_w = 8;
    font::glyph_dimensions(print_flags, &glyph_w, NULL);
    size_t chars_w = SDL_max(w-16, new_w) / glyph_w;
    for (size_t iter = 0; iter < lines.size(); iter++)
    {
        size_t n_glyphs = UTF8_total_codepoints(lines[iter].c_str());
        signed int padding_needed = chars_w - n_glyphs;
        if (padding_needed < 0)
        {
            continue;
        }
        size_t left_pad = padding_needed / 2;
        size_t right_pad = padding_needed - left_pad;

        lines[iter] = std::string(left_pad, ' ') + lines[iter] + std::string(right_pad, ' ');
    }
    resize();
}

void textboxclass::centertext(void)
{
    padtowidth(w-16);
}
