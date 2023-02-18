#ifndef BLOCKV_H
#define BLOCKV_H

#include <SDL.h>
#include <stdint.h>
#include <string>

class blockclass
{
public:
    blockclass(void);
    void clear(void);

    void rectset(const int xi, const int yi, const int wi, const int hi);

    void setblockcolour(const char* col);
public:
    //Fundamentals
    SDL_Rect rect;
    int type;
    int trigger;
    int xp, yp, wp, hp;
    std::string script, prompt;
    int r, g, b;
    int activity_y;
    uint32_t print_flags;
};

#endif /* BLOCKV_H */
