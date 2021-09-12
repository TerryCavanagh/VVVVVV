#include "BlockV.h"

#include <SDL_stdinc.h>

blockclass::blockclass(void)
{
    clear();
}

void blockclass::clear(void)
{
    type = 0;
    trigger = 0;

    xp = 0;
    yp = 0;
    wp = 0;
    hp = 0;
    rect.x = xp;
    rect.y = yp;
    rect.w = wp;
    rect.h = hp;

    r = 0;
    g = 0;
    b = 0;

    /* std::strings get initialized automatically, but this is */
    /* in case this function gets called again after construction */
    script.clear();
    prompt.clear();
}

void blockclass::rectset(const int xi, const int yi, const int wi, const int hi)
{
    rect.x = xi;
    rect.y = yi;
    rect.w = wi;
    rect.h = hi;
}

void blockclass::setblockcolour(const char* col)
{
    if (SDL_strcmp(col, "cyan") == 0)
    {
        r = 164;
        g = 164;
        b = 255;
    }
    else if (SDL_strcmp(col, "red") == 0)
    {
        r = 255;
        g = 60;
        b = 60;
    }
    else if (SDL_strcmp(col, "green") == 0)
    {
        r = 144;
        g = 255;
        b = 144;
    }
    else if (SDL_strcmp(col, "yellow") == 0)
    {
        r = 255;
        g = 255;
        b = 134;
    }
    else if (SDL_strcmp(col, "blue") == 0)
    {
        r = 95;
        g = 95;
        b = 255;
    }
    else if (SDL_strcmp(col, "purple") == 0)
    {
        r = 255;
        g = 134;
        b = 255;
    }
    else if (SDL_strcmp(col, "white") == 0)
    {
        r = 244;
        g = 244;
        b = 244;
    }
    else if (SDL_strcmp(col, "gray") == 0)
    {
        r = 174;
        g = 174;
        b = 174;
    }
    else if (SDL_strcmp(col, "orange") == 0)
    {
        r = 255;
        g = 130;
        b = 20;
    }
    else
    {
        //use a gray
        r = 174;
        g = 174;
        b = 174;
    }
}
