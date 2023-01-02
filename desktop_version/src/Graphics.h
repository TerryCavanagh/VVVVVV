#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <map>
#include <string>
#include <vector>

#include "Game.h"
#include "GraphicsResources.h"
#include "Maths.h"
#include "Textbox.h"
#include "TowerBG.h"

enum FadeBars
{
    FADE_NONE,
    FADE_FULLY_BLACK,
    FADE_START_FADEOUT,
    FADE_FADING_OUT,
    FADE_START_FADEIN,
    FADE_FADING_IN
};

#define FADEMODE_IS_FADING(mode) ((mode) != FADE_NONE && (mode) != FADE_FULLY_BLACK)

class Graphics
{
public:
    void init(void);
    void destroy(void);

    void create_buffers(const SDL_PixelFormat* fmt);
    void destroy_buffers(void);

    GraphicsResources grphx;

    int bfontlen(uint32_t ch);
    int font_idx(uint32_t ch);

    bool Makebfont(void);

    void drawhuetile(int x, int y, int t, SDL_Color ct);
    SDL_Color huetilegetcol(int t);
    SDL_Color bigchunkygetcol(int t);

    void drawgravityline(int t);

    bool MakeTileArray(void);

    bool MakeSpriteArray(void);

    bool maketelearray(void);

    void drawcoloredtile(int x, int y, int t, int r, int g, int b);

    void drawmenu(int cr, int cg, int cb, enum Menu::MenuName menu);

    void processfade(void);
    void setfade(const int amount);

    void drawfade(void);

    void createtextboxreal(
        const std::string& t,
        int xp,
        int yp,
        int r,
        int g,
        int b,
        bool flipme
    );
    void createtextbox(
        const std::string& t,
        int xp,
        int yp,
        int r,
        int g,
        int b
    );
    void createtextboxflipme(
        const std::string& t,
        int xp,
        int yp,
        int r,
        int g,
        int b
    );

    void textboxcenterx(void);

    int textboxwidth(void);

    void textboxmoveto(int xo);

    void textboxcentery(void);

    int textboxwrap(int pad);

    void textboxpad(size_t left_pad, size_t right_pad);

    void textboxpadtowidth(size_t new_w);

    void textboxcentertext();

    void textboxcommsrelay();

    void textboxadjust(void);

    void addline(const std::string& t);

    void textboxtimer(int t);

    void textboxremove(void);

    void textboxremovefast(void);

    void textboxactive(void);

    void drawtextbox(int x, int y, int w, int h, int r, int g, int b);

    void drawpixeltextbox(int x, int y, int w, int h, int r, int g, int b);

    void drawcrewman(int x, int y, int t, bool act, bool noshift =false);

    int crewcolour(const int t);

    void cutscenebars(void);
    void cutscenebarstimer(void);
    void setbars(const int position);

    void drawpartimage(int t, int xp, int yp, int wp, int hp);

    void drawimage(int t, int xp, int yp, bool cent=false);

    void drawimagecol(int t, int xp, int yp, SDL_Color ct, bool cent= false);

    void updatetextboxes(void);
    void drawgui(void);

    void drawsprite(int x, int y, int t, int r, int g, int b);
    void drawsprite(int x, int y, int t, SDL_Color color);

    void printcrewname(int x, int y, int t);
    void printcrewnamedark(int x, int y, int t);

    void printcrewnamestatus(int x, int y, int t, bool rescued);

    void map_tab(int opt, const char* text, bool selected = false);

    void map_option(int opt, int num_opts, const std::string& text, bool selected = false);

    void do_print(int x, int y, const std::string& text, int r, int g, int b, int a, int scale);

    void Print(int _x, int _y, const std::string& _s, int r, int g, int b, bool cen = false);

    void PrintAlpha(int _x, int _y, const std::string& _s, int r, int g, int b, int a, bool cen = false);

    bool next_wrap(size_t* start, size_t* len, const char* str, int maxwidth);

    bool next_wrap_s(char buffer[], size_t buffer_size, size_t* start, const char* str, int maxwidth);

    int PrintWrap(int x, int y, std::string s, int r, int g, int b, bool cen = false, int linespacing = -1, int maxwidth = -1);

    void bprint(int x, int y, const std::string& t, int r, int g, int b, bool cen = false);

    void bprintalpha(int x, int y, const std::string& t, int r, int g, int b, int a, bool cen = false);

    int len(const std::string& t);
    std::string string_wordwrap(const std::string& s, int maxwidth, short *lines = NULL);
    std::string string_wordwrap_balanced(const std::string& s, int maxwidth);
    std::string string_unwordwrap(const std::string& s);

    void bigprint( int _x, int _y, const std::string& _s, int r, int g, int b, bool cen = false, int sc = 2 );
    void bigbprint(int x, int y, const std::string& s, int r, int g, int b, bool cen = false, int sc = 2);
    void drawspritesetcol(int x, int y, int t, int c);


    void flashlight(void);
    void screenshake(void);
    void updatescreenshake(void);

    int screenshake_x;
    int screenshake_y;

    void render(void);
    void renderwithscreeneffects(void);
    void renderfixedpre(void);
    void renderfixedpost(void);

    bool Hitest(SDL_Surface* surface1, point p1, SDL_Surface* surface2, point p2);

    void drawentities(void);

    void drawentity(const int i, const int yoff);

    void drawtrophytext(void);

    void bigrprint(int x, int y, const std::string& t, int r, int g, int b, bool cen = false, float sc = 2);
    void bigbrprint(int x, int y, const std::string& t, int r, int g, int b, bool cen = false, float sc = 2);


    void drawtele(int x, int y, int t, SDL_Color c);

    SDL_Color getRGBA(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

    SDL_Color getRGB(Uint8 r, Uint8 g, Uint8 b);

    SDL_Color RGBf(int r, int g, int b);

    void drawbackground(int t);
    void updatebackground(int t);
#ifndef NO_CUSTOM_LEVELS
    bool shouldrecoloroneway(const int tilenum, const bool mounted);
#endif
    void drawtile3( int x, int y, int t, int off, int height_subtract = 0 );
    void drawtile2( int x, int y, int t );
    void drawtile( int x, int y, int t );
    void drawtowertile( int x, int y, int t );
    void drawtowertile3( int x, int y, int t, TowerBG& bg_obj );

    void drawmap(void);

    void drawforetile(int x, int y, int t);

    void drawforetile2(int x, int y, int t);

    void drawforetile3(int x, int y, int t, int off);

    void drawrect(int x, int y, int w, int h, int r, int g, int b);

    void drawtowermap(void);

    void drawtowerspikes(void);

    bool onscreen(int t);

    bool reloadresources(void);
#ifndef NO_CUSTOM_LEVELS
    bool tiles1_mounted;
    bool tiles2_mounted;
    bool minimap_mounted;
#endif

    bool gamecomplete_mounted;
    bool levelcomplete_mounted;
    bool flipgamecomplete_mounted;
    bool fliplevelcomplete_mounted;


    void menuoffrender(void);

    void drawtowerbackground(const TowerBG& bg_obj);
    void updatetowerbackground(TowerBG& bg_obj);

    SDL_Color getcol(int t);
    void drawfinalmap(void);

    int rcol;



    int m;

    std::vector <SDL_Surface*> images;

    std::vector <SDL_Surface*> tele;
    std::vector <SDL_Surface*> tiles;
    std::vector <SDL_Surface*> tiles2;
    std::vector <SDL_Surface*> tiles3;
    std::vector <SDL_Surface*> entcolours;
    std::vector <SDL_Surface*> sprites;
    std::vector <SDL_Surface*> flipsprites;
    std::vector <SDL_Surface*> bfont;
    std::vector <SDL_Surface*> flipbfont;

    bool flipmode;
    bool setflipmode;
    bool notextoutline;
    //buffer objects. //TODO refactor buffer objects
    SDL_Surface* backBuffer;
    SDL_Surface* menubuffer;
    SDL_Surface* foregroundBuffer;
    SDL_Surface* menuoffbuffer;
    SDL_Surface* warpbuffer;
    SDL_Surface* warpbuffer_lerp;

    TowerBG towerbg;
    TowerBG titlebg;

    SDL_Rect tiles_rect;
    SDL_Rect sprites_rect;
    SDL_Rect line_rect;
    SDL_Rect tele_rect;
    SDL_Rect towerbuffer_rect;

    SDL_Rect prect;
    SDL_Rect footerrect;
    SDL_Surface* footerbuffer;

    int linestate, linedelay;
    int backoffset;
    bool backgrounddrawn, foregrounddrawn;

    int menuoffset;
    int oldmenuoffset;
    bool resumegamemode;

    int crewframe;
    int crewframedelay;

    enum FadeBars fademode;
    int fadeamount;
    int oldfadeamount;
    int fadebars[15];
    enum FadeBars ingame_fademode;

    bool trinketcolset;
    int trinketr, trinketg, trinketb;

    std::vector <textboxclass> textboxes;

    bool showcutscenebars;
    int cutscenebarspos;
    int oldcutscenebarspos;

    static const int numstars = 50;
    SDL_Rect stars[numstars];
    int starsspeed[numstars];

    static const int numbackboxes = 18;
    int spcol, spcoldel;
    SDL_Rect backboxes[numbackboxes];
    int backboxvx[numbackboxes];
    int backboxvy[numbackboxes];
    float backboxint[numbackboxes];

    int warpskip;

    bool translucentroomname;

    std::map<int, int> font_positions;

    SDL_Surface* ghostbuffer;

#ifndef GAME_DEFINITION
    float inline lerp(const float v0, const float v1)
    {
        if (game.physics_frozen())
        {
            return v1;
        }
        return v0 + alpha * (v1 - v0);
    }
#endif
    float alpha;

    SDL_Color col_crewred;
    SDL_Color col_crewyellow;
    SDL_Color col_crewgreen;
    SDL_Color col_crewcyan;
    SDL_Color col_crewblue;
    SDL_Color col_crewpurple; //actually pink
    SDL_Color col_crewinactive;
    SDL_Color col_clock;
    SDL_Color col_trinket;
    int col_tr;
    int col_tg;
    int col_tb;
    void updatetitlecolours(void);

    bool kludgeswnlinewidth;

    SDL_Color crewcolourreal(int t);

    void render_roomname(const char* roomname, int r, int g, int b);

    char error[128];
    char error_title[128]; /* for SDL_ShowSimpleMessageBox */
};

#ifndef GRAPHICS_DEFINITION
extern Graphics graphics;
#endif

#endif /* GRAPHICS_H */
