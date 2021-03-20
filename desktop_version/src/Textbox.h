#ifndef TEXTBOX_H
#define TEXTBOX_H

#include <SDL.h>
#include <string>
#include <vector>

class textboxclass
{
public:
    textboxclass(void);

    void centerx(void);

    void centery(void);

    void adjust(void);

    void initcol(int rr, int gg, int bb);

    void setcol(int rr, int gg, int bb);

    void update(void);

    void remove(void);

    void removefast(void);

    void resize(void);

    void addline(std::string t);
public:
    //Fundamentals
    std::vector<std::string> line;
    int xp, yp, lw, w, h;
    int r,g,b;
    int tr,tg,tb;
    int timer;

    float tl;
    float prev_tl;
    int tm;

    int max;

};

#endif /* TEXTBOX_H */
