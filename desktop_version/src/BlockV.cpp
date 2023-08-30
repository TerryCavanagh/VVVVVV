#include "BlockV.h"

#include <SDL_stdinc.h>

#include "Script.h"
#include "Font.h"

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

    activity_y = 0;

    /* std::strings get initialized automatically, but this is
     * in case this function gets called again after construction */
    script.clear();
    prompt.clear();

    gettext = true;
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
    bool exists = ::script.textbox_colours.count(col) != 0;

    r = ::script.textbox_colours[exists ? col : "gray"].r;
    g = ::script.textbox_colours[exists ? col : "gray"].g;
    b = ::script.textbox_colours[exists ? col : "gray"].b;
}
