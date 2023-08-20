#ifndef TEXTBOX_H
#define TEXTBOX_H

#include <stdint.h>
#include <string>
#include <vector>

struct TextboxSprite
{
    int x;
    int y;
    int col;
    int tile;
};

enum TextboxImage
{
    TEXTIMAGE_NONE,
    TEXTIMAGE_LEVELCOMPLETE,
    TEXTIMAGE_GAMECOMPLETE
};

class textboxclass
{
public:
    textboxclass(void);

    void addsprite(int x, int y, int tile, int col);

    void setimage(TextboxImage image);

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

    void centertext(void);
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

    bool large;

    uint32_t print_flags;
    bool fill_buttons;

    std::vector<TextboxSprite> sprites;
    TextboxImage image;
};

#endif /* TEXTBOX_H */
