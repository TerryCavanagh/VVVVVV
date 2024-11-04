#ifndef TEXTBOX_H
#define TEXTBOX_H

#include <stdint.h>
#include <string>
#include <vector>

/* Position of the crewmate that the text box position is based off of.
 * NOT a crewmate sprite inside the text box (that's a TextboxSprite). */
struct TextboxCrewmatePosition
{
    bool override_x;
    bool override_y;
    int x;
    int y;
    int dir;
    bool text_above;
};

struct TextboxOriginalContext
{
    int x;
    int y;
    std::vector<std::string> lines;
    std::string script_name;
    char text_case;
};

/* Similar to, but NOT the same as a loc::TextboxFormat. */
struct TextboxSpacing
{
    bool centertext;
    unsigned char pad_left;
    unsigned char pad_right;
    unsigned short padtowidth;
};

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

enum TextboxTranslate
{
    TEXTTRANSLATE_NONE,
    TEXTTRANSLATE_CUTSCENE,
    TEXTTRANSLATE_FUNCTION
};

class textboxclass;
typedef void (*TextboxFunction)(textboxclass* THIS);

class textboxclass
{
public:
    textboxclass(int gap);

    void addsprite(int x, int y, int tile, int col);

    void setimage(TextboxImage image);

    void centerx(void);

    void centery(void);

    void applyposition(void);

    void adjust(void);

    void initcol(int rr, int gg, int bb);

    void update(void);

    void remove(void);

    void removefast(void);

    void resize(void);

    void reposition(void);

    void addline(const std::string& t);

    void pad(size_t left_pad, size_t right_pad);

    void padtowidth(size_t new_w);

    void centertext(void);

    void wrap(int pad);

    void copyoriginaltext(void);

    void applyoriginalspacing(void);

    void updatetext(void);

    void translatecutscene(void);
public:
    //Fundamentals
    std::vector<std::string> lines;
    int xp, yp, w, h;
    int r,g,b;
    int linegap;
    int timer;

    float tl;
    float prev_tl;
    int tm;

    /* Whether to flip text box y-position in Flip Mode. */
    bool flipme;

    int rand;

    bool large;

    bool position_absolute;

    bool should_centerx;
    bool should_centery;

    uint32_t print_flags;
    TextboxTranslate translate;
    bool fill_buttons;

    std::vector<TextboxSprite> sprites;
    TextboxImage image;

    bool force_outline;
    bool outline;

    TextboxCrewmatePosition crewmate_position;
    TextboxOriginalContext original;
    TextboxSpacing spacing;
    TextboxFunction function;

    int other_textbox_index;
};

#endif /* TEXTBOX_H */
