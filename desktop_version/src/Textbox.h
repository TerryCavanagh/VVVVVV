#ifndef TEXTBOX_H
#define TEXTBOX_H

#include "SDL.h"
#include <string>
#include <vector>

class textboxclass
{
public:
    textboxclass();

    void centerx();

    void centery();

    void adjust();

    void initcol(int rr, int gg, int bb);

    void setcol(int rr, int gg, int bb);

    void update();

    void remove();

    void removefast();

    void resize();

    void addline(std::string t);
public:
    //Fundamentals
    std::vector<std::string> line;
    int xp, yp, lw, w, h;
    int x,y;
    int r,g,b;
    int tr,tg,tb;
    SDL_Rect textrect;
    int timer;

    float tl;
    float prev_tl;
    int tm;

    int max;

};

#endif /* TEXTBOX_H */
