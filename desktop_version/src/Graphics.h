#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <map>
#include <string>
#include <vector>

#include "GraphicsResources.h"
#include "GraphicsUtil.h"
#include "Maths.h"
#include "Screen.h"
#include "Textbox.h"
#include "TowerBG.h"

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

    void drawhuetile(int x, int y, int t);
    void huetilesetcol(int t);
    Uint32 bigchunkygetcol(int t);

    void drawgravityline(int t);

    bool MakeTileArray(void);

    bool MakeSpriteArray(void);

    bool maketelearray(void);

    void drawcoloredtile(int x, int y, int t, int r, int g, int b);

    void drawmenu(int cr, int cg, int cb, bool levelmenu = false);

    void processfade(void);
    void setfade(const int amount);

    void drawfade(void);

    void setwarprect(int a, int b, int c, int d);

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

    void drawimagecol(int t, int xp, int yp, bool cent= false);

    void updatetextboxes(void);
    void drawgui(void);

    void drawsprite(int x, int y, int t, int r, int g, int b);
    void drawsprite(int x, int y, int t, Uint32 c);

    void printcrewname(int x, int y, int t);

    void printcrewnamestatus(int x, int y, int t);

    void printcrewnamedark(int x, int y, int t);

    void map_tab(int opt, const std::string& text, bool selected = false);

    void map_option(int opt, int num_opts, const std::string& text, bool selected = false);

    void do_print(int x, int y, const std::string& text, int r, int g, int b, int a, int scale);

    void Print(int _x, int _y, const std::string& _s, int r, int g, int b, bool cen = false);

    void PrintAlpha(int _x, int _y, const std::string& _s, int r, int g, int b, int a, bool cen = false);

    bool next_wrap(size_t* start, size_t* len, const char* str, int maxwidth);

    bool next_wrap_s(char buffer[], size_t buffer_size, size_t* start, const char* str, int maxwidth);

    void PrintWrap(int x, int y, const char* str, int r, int g, int b, bool cen, int linespacing, int maxwidth);

    void bprint(int x, int y, const std::string& t, int r, int g, int b, bool cen = false);

    void bprintalpha(int x, int y, const std::string& t, int r, int g, int b, int a, bool cen = false);

    int len(const std::string& t);
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


    void drawtele(int x, int y, int t, Uint32 c);

    Uint32 getRGBA(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

    Uint32 getRGB(Uint8 r, Uint8 g, Uint8 b);

    Uint32 getRGB(Uint32 _col);

    Uint32 RGBflip(Uint8  r, Uint8  g, Uint8  b);


    Uint32 RGBf(int r, int g, int b);

    void setcolreal(Uint32 t);

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


    void menuoffrender(void);

    void drawtowerbackground(const TowerBG& bg_obj);
    void updatetowerbackground(TowerBG& bg_obj);

    void setcol(int t);
    void drawfinalmap(void);

    colourTransform ct;

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
    Screen* screenbuffer;
    SDL_Surface* menubuffer;
    SDL_Surface* foregroundBuffer;
    SDL_Surface* tempBuffer;
    SDL_Surface* warpbuffer;
    SDL_Surface* warpbuffer_lerp;

    TowerBG towerbg;
    TowerBG titlebg;

    SDL_Rect bfont_rect;
    SDL_Rect tiles_rect;
    SDL_Rect sprites_rect;
    SDL_Rect images_rect;
    SDL_Rect bg_rect;
    SDL_Rect line_rect;
    SDL_Rect tele_rect;
    SDL_Rect towerbuffer_rect;

    SDL_Rect foot_rect;
    SDL_Rect prect;
    SDL_Rect footerrect;
    SDL_Surface* footerbuffer;

    int linestate, linedelay;
    int backoffset;
    bool backgrounddrawn, foregrounddrawn;

    int menuoffset;
    int oldmenuoffset;
    bool resumegamemode;

    SDL_Rect warprect;

    int crewframe;
    int crewframedelay;

    int fademode;
    int fadeamount;
    int oldfadeamount;
    int fadebars[15];
    int ingame_fademode;

    bool trinketcolset;
    int trinketr, trinketg, trinketb;

    std::vector <textboxclass> textbox;

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

    int warpskip, warpfcol, warpbcol;

    bool translucentroomname;

    std::map<int, int> font_positions;

    SDL_Surface* ghostbuffer;

    float inline lerp(const float v0, const float v1)
    {
        return v0 + alpha * (v1 - v0);
    }
    float alpha;

    Uint32 col_crewred;
    Uint32 col_crewyellow;
    Uint32 col_crewgreen;
    Uint32 col_crewcyan;
    Uint32 col_crewblue;
    Uint32 col_crewpurple; //actually pink
    Uint32 col_crewinactive;
    Uint32 col_clock;
    Uint32 col_trinket;
    int col_tr;
    int col_tg;
    int col_tb;
    void updatetitlecolours(void);

    bool kludgeswnlinewidth;

    Uint32 crewcolourreal(int t);

    char error[128];
    char error_title[128]; /* for SDL_ShowSimpleMessageBox */
};

#ifndef GRAPHICS_DEFINITION
extern Graphics graphics;
#endif

#endif /* GRAPHICS_H */
