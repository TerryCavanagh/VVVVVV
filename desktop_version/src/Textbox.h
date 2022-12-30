#ifndef TEXTBOX_H
#define TEXTBOX_H

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

    void update(void);

    void remove(void);

    void removefast(void);

    void resize(void);

    void addline(const std::string& t);

    void pad(size_t left_pad, size_t right_pad);

    void padtowidth(size_t new_w);

    void centertext();
public:
    //Fundamentals
    std::vector<std::string> lines;
    int xp, yp, w, h;
    int r,g,b;
    int timer;

    float tl;
    float prev_tl;
    int tm;

    /* Whether to flip text box y-position in Flip Mode. */
    bool flipme;

    int rand;
};

#endif /* TEXTBOX_H */
