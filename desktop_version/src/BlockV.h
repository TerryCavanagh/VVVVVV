#ifndef BLOCKV_H
#define BLOCKV_H

#include "SDL.h"
#include <string>

class blockclass
{
public:
    blockclass();

    void clear();

    void rectset(const int xi, const int yi, const int wi, const int hi);

    void setblockcolour(std::string col);
public:
    //Fundamentals
    bool active;
    SDL_Rect rect;
    int type;
    int trigger;
    int xp, yp, wp, hp;
    std::string script, prompt;
    int r, g, b;

    //These would come from the sprite in the flash
    float x;
    float y;

};

#endif /* BLOCKV_H */
