#include "Textbox.h"

#include <utf8/unchecked.h>

textboxclass::textboxclass(void)
{
    w = 0;
    h = 0;
    lw = 0;
    tl = 0;
    prev_tl = 0;
    tm = 0;
    timer = 0;

    xp = 0;
    yp = 0;
    r = 0;
    g = 0;
    b = 0;
    tr = 0;
    tg = 0;
    tb = 0;
    max = 0;

    flipme = false;
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
    tr = rr;
    tg = gg;
    tb = bb;
    r = 0;
    g = 0;
    b = 0;
    tl = 0.5;
}

void textboxclass::setcol(int rr, int gg, int bb)
{
    r = rr;
    g = gg;
    b = bb;
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
    max = 0;
    for (size_t iter = 0; iter < line.size(); iter++)
    {
        unsigned int len = utf8::unchecked::distance(line[iter].begin(), line[iter].end());
        if (len > (unsigned int)max) max = len;
    }

    lw = max;
    w = (max +2) * 8;
    h = (line.size() + 2) * 8;
}

void textboxclass::addline(std::string t)
{
    line.push_back(t);
    resize();
    if ((int) line.size() >= 12) line.clear();
}
