#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <map>
#include <string>
#include <vector>

#include "Game.h"
#include "GraphicsResources.h"
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

enum ImageNames
{
    IMAGE_LEVELCOMPLETE,
    IMAGE_MINIMAP,
    IMAGE_COVERED,
    IMAGE_ELEPHANT,
    IMAGE_GAMECOMPLETE,
    IMAGE_FLIPLEVELCOMPLETE,
    IMAGE_FLIPGAMECOMPLETE,
    IMAGE_SITE,
    IMAGE_SITE2,
    IMAGE_SITE3,
    IMAGE_ENDING,
    IMAGE_SITE4,
    IMAGE_CUSTOMMINIMAP,
    NUM_IMAGES
};

#define FADEMODE_IS_FADING(mode) ((mode) != FADE_NONE && (mode) != FADE_FULLY_BLACK)

class Graphics
{
public:
    void init(void);
    void destroy(void);

    void create_buffers(void);
    void destroy_buffers(void);

    GraphicsResources grphx;

    SDL_Color huetilegetcol(int t);
    SDL_Color bigchunkygetcol(int t);

    void drawgravityline(int t);

    bool MakeSpriteArray(void);

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

    void textboxprintflags(uint32_t flags);

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

    void draw_texture(SDL_Texture* image, int x, int y);

    void draw_texture_part(SDL_Texture* image, int x, int y, int x2, int y2, int w, int h, int scalex, int scaley);

    void draw_grid_tile(SDL_Texture* texture, int t, int x, int y, int width, int height, int scalex, int scaley);
    void draw_grid_tile(SDL_Texture* texture, int t, int x, int y, int width, int height);
    void draw_grid_tile(SDL_Texture* texture, int t, int x, int y, int width, int height, int r, int g, int b, int a, int scalex, int scaley);
    void draw_grid_tile(SDL_Texture* texture, int t, int x, int y, int width, int height, int r, int g, int b, int a);
    void draw_grid_tile(SDL_Texture* texture, int t, int x, int y, int width, int height, int r, int g, int b, int scalex, int scaley);
    void draw_grid_tile(SDL_Texture* texture, int t, int x, int y, int width, int height, int r, int g, int b);
    void draw_grid_tile(SDL_Texture* texture, int t, int x, int y, int width, int height, SDL_Color color, int scalex, int scaley);
    void draw_grid_tile(SDL_Texture* texture, int t, int x, int y, int width, int height, SDL_Color color);

    void updatetextboxes(void);
    void drawgui(void);

    void draw_sprite(int x, int y, int t, int r, int g, int b);
    void draw_sprite(int x, int y, int t, SDL_Color color);

    void scroll_texture(SDL_Texture* texture, int x, int y);

    void printcrewname(int x, int y, int t);
    void printcrewnamedark(int x, int y, int t);

    void printcrewnamestatus(int x, int y, int t, bool rescued);

    int set_render_target(SDL_Texture* texture);

    int set_texture_color_mod(SDL_Texture* texture, Uint8 r, Uint8 g, Uint8 b);

    int set_texture_alpha_mod(SDL_Texture* texture, Uint8 alpha);

    int query_texture(SDL_Texture* texture, Uint32* format, int* access, int* w, int* h);

    int set_blendmode(SDL_BlendMode blendmode);
    int set_blendmode(SDL_Texture* texture, SDL_BlendMode blendmode);

    int clear(int r, int g, int b, int a);
    int clear();

    int copy_texture(SDL_Texture* texture, const SDL_Rect* src, const SDL_Rect* dest);
    int copy_texture(SDL_Texture* texture, const SDL_Rect* src, const SDL_Rect* dest, double angle, const SDL_Point* center, SDL_RendererFlip flip);

    int set_color(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    int set_color(Uint8 r, Uint8 g, Uint8 b);
    int set_color(SDL_Color color);

    int fill_rect(const SDL_Rect* rect, int r, int g, int b, int a);
    int fill_rect(int x, int y, int w, int h, int r, int g, int b, int a);
    int fill_rect(int x, int y, int w, int h, int r, int g, int b);
    int fill_rect(int r, int g, int b, int a);
    int fill_rect(const SDL_Rect* rect, int r, int g, int b);
    int fill_rect(int r, int g, int b);
    int fill_rect(const SDL_Rect* rect, SDL_Color color);
    int fill_rect(int x, int y, int w, int h, SDL_Color color);
    int fill_rect(SDL_Color color);

    void map_tab(int opt, const char* text, bool selected = false);

    void map_option(int opt, int num_opts, const std::string& text, bool selected = false);

    void Print(int _x, int _y, const std::string& _s, int r, int g, int b, bool cen = false);

    void PrintAlpha(int _x, int _y, const std::string& _s, int r, int g, int b, int a, bool cen = false);

    int PrintWrap(int x, int y, const std::string& s, int r, int g, int b, bool cen = false, int linespacing = -1, int maxwidth = -1);

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

    bool Hitest(SDL_Surface* surface1, SDL_Point p1, SDL_Surface* surface2, SDL_Point p2);

    void drawentities(void);

    void drawentity(const int i, const int yoff);

    void drawtrophytext(void);

    void bigrprint(int x, int y, const std::string& t, int r, int g, int b, bool cen = false, int sc = 2);
    void bigbrprint(int x, int y, const std::string& t, int r, int g, int b, bool cen = false, int sc = 2);


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

    std::vector <SDL_Surface*> sprites_surf;
    std::vector <SDL_Surface*> flipsprites_surf;

    SDL_Texture* images[NUM_IMAGES];

    bool flipmode;
    bool setflipmode;
    bool notextoutline;

    SDL_Texture* gameTexture;
    SDL_Texture* tempTexture;
    SDL_Texture* gameplayTexture;
    SDL_Texture* menuTexture;
    SDL_Texture* ghostTexture;
    SDL_Texture* backgroundTexture;
    SDL_Texture* foregroundTexture;

    TowerBG towerbg;
    TowerBG titlebg;

    SDL_Rect tiles_rect;
    SDL_Rect sprites_rect;
    SDL_Rect line_rect;
    SDL_Rect tele_rect;

    SDL_Rect prect;
    SDL_Rect footerrect;

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

    void render_roomname(uint32_t font_flag, const char* roomname, int r, int g, int b);

    char error[128];
    char error_title[128]; /* for SDL_ShowSimpleMessageBox */
};

#ifndef GRAPHICS_DEFINITION
extern Graphics graphics;
#endif

#endif /* GRAPHICS_H */
