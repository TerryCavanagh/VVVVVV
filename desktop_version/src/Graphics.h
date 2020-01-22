#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "GraphicsResources.h"
#include <vector>



#include "Maths.h"
#include "Textbox.h"
#include "UtilityClass.h"
#include "Game.h"


#include <string>
#include <algorithm>

#include "GraphicsUtil.h"
#include "Screen.h"

class map;

class Graphics
{
public:
	Graphics();
	~Graphics();

	GraphicsResources grphx;

	void Makebfont();

	void drawhuetile(int x, int y, int t, int c);

	void drawgravityline(int t, entityclass& obj);

	void MakeTileArray();

	void MakeSpriteArray();

	void maketelearray();

	void drawcoloredtile(int x, int y, int t, int r, int g, int b);

	void drawmenu(Game& game, int cr, int cg, int cb, int division = 30);
	void drawlevelmenu(Game& game, int cr, int cg, int cb, int division = 30);

	void processfade();

	void drawfade();

	void setwarprect(int a, int b, int c, int d);

	void createtextbox(std::string t, int xp, int yp, int r= 255, int g= 255, int b = 255);

	void textboxcleanup();

	void textboxcenter();

	void textboxcenterx();

	int textboxwidth();

	void textboxmove(int xo, int yo);

	void textboxmoveto(int xo);

	void textboxcentery();

	void textboxadjust();

	void addline(std::string t);

	void textboxtimer(int t);

	void textboxremove();

	void textboxremovefast();

	void textboxactive();

	void _drawtextbox(int x, int y, int w, int h, int r, int g, int b);
	template<class A, class B, class C, class D, class E, class F, class G>
	void drawtextbox(A x, B y, C w, D h, E r, F g, G b)
	{
		_drawtextbox(static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h), static_cast<int>(r), static_cast<int>(g), static_cast<int>(b));
	}

	void drawpixeltextbox(int x, int y, int w, int h, int w2, int h2, int r, int g, int b, int xo, int yo);
	void drawcustompixeltextbox(int x, int y, int w, int h, int w2, int h2, int r, int g, int b, int xo, int yo);

	void drawcrewman(int x, int y, int t, bool act, UtilityClass& help, bool noshift =false);

	int crewcolour(const int t);

	void cutscenebars();

	void drawpartimage(int t, int xp, int yp, int wp, int hp);

	void drawimage(int t, int xp, int yp, bool cent=false);

	void _drawimagecol(int t, int xp, int yp, int r, int g, int b, bool cent= false);
	template <class A, class B, class C, class D, class E, class F>
	void drawimagecol(A t, B xp, C yp, D r, E g, F b, bool cent = false)
	{
		_drawimagecol(static_cast<int>(t), static_cast<int>(xp), static_cast<int>(yp), static_cast<int>(r), static_cast<int>(g), static_cast<int>(b), cent);
	}

	void drawgui(UtilityClass& help);

	void drawsprite(int x, int y, int t, int r, int g, int b);

	void printcrewname(int x, int y, int t);

	void printcrewnamestatus(int x, int y, int t);

	void printcrewnamedark(int x, int y, int t);

	void Print(int _x, int _y, std::string _s, int r, int g, int b, bool cen = false);

	void RPrint(int _x, int _y, std::string _s, int r, int g, int b, bool cen = false);

	void PrintOff(int _x, int _y, std::string _s, int r, int g, int b, bool cen = false);

	void bprint(int x, int y, std::string t, int r, int g, int b, bool cen = false);

	int len(std::string t);
	void bigprint( int _x, int _y, std::string _s, int r, int g, int b, bool cen = false, int sc = 2 );
	void drawspritesetcol(int x, int y, int t, int c, UtilityClass& help);


	void flashlight();
	void screenshake();

	void render();

	bool Hitest(SDL_Surface* surface1, point p1, int col, SDL_Surface* surface2, point p2, int col2);

	void drawentities(mapclass& map, entityclass& obj, UtilityClass& help);

	void drawtrophytext(entityclass&, UtilityClass& help);

	void bigrprint(int x, int y, std::string& t, int r, int g, int b, bool cen = false, float sc = 2);


	void drawtele(int x, int y, int t, int c, UtilityClass& help);

	Uint32 _getRGB(Uint8 r, Uint8 g, Uint8 b);
	/* rather than making hundreds of casts through the code, instead make these functions templates
	 * which will accept aything which the compiler can static_cast to a Uint8
	 * Compiler should error if non-castable parameter is given.
	 */
	template <class R, class G, class B>
	Uint32 getRGB(R r, G g, B b)
	{
		return _getRGB(static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b));
	}
	Uint32 _getBGR(Uint8 r, Uint8 g, Uint8 b);
	template <class R, class G, class B>
	Uint32 getBGR(R r, G g, B b)
	{
		return _getBGR(static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b));
	}
	Uint32 _RGBflip(Uint8  r, Uint8  g, Uint8  b);
	template <class R, class G, class B>
	Uint32 RGBflip(R  r, G  g, B  b)
	{
		return _RGBflip(static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b));
	}

	Uint32 getRGB(Uint32 _col);

	Uint32 RGBflip(Uint8  r, Uint8  g, Uint8  b);


	Uint32 _RGBf(int r, int g, int b);
	template <class R, class G, class B>
	Uint32 RGBf(R r, G g, B b)
	{
		return _RGBf(static_cast<int>(r), static_cast<int>(g), static_cast<int>(b));
	}

	void setcolreal(Uint32 t);

	void drawbackground(int t, mapclass& map);
	void drawtile3( int x, int y, int t, int off );
	void drawentcolours( int x, int y, int t);
	void drawtile2( int x, int y, int t, int r, int g, int b );
	void drawtile( int x, int y, int t, int r, int g, int b );
	void drawtowertile( int x, int y, int t );
	void drawtowertile3( int x, int y, int t, int off );

	void drawtile(int x, int y, int t);

	void drawmap(mapclass& map);

	void drawforetile(int x, int y, int t);

	void drawforetile2(int x, int y, int t);

	void drawforetile3(int x, int y, int t, int off);

	void drawrect(int x, int y, int w, int h, int r, int g, int b);

	void drawtowermap(mapclass& map);

	void drawtowermap_nobackground(mapclass& map);

	void drawtowerspikes(mapclass& map);

	void drawtowerentities(mapclass& map, entityclass& obj, UtilityClass& help);

	bool onscreen(int t);

	void drawtowerbackgroundsolo(mapclass& map);


	void menuoffrender();

	void drawtowerbackground(mapclass& map);

	void setcol(int t, UtilityClass& help);
	void drawfinalmap(mapclass & map);

	colourTransform ct;

	std::string tempstring;

	int bcol, bcol2, rcol;



	int j, k, m;

	std::vector <SDL_Surface*> backgrounds;
	std::vector <SDL_Surface*> images;

	std::vector <SDL_Surface*> tele;
	std::vector <SDL_Surface*> tiles;
	std::vector <SDL_Surface*> tiles2;
	std::vector <SDL_Surface*> tiles3;
	std::vector <SDL_Surface*> entcolours;
	std::vector <SDL_Surface*> sprites;
	std::vector <SDL_Surface*> flipsprites;
	std::vector <SDL_Surface*> bfont;
	std::vector <SDL_Surface*> bfontmask;
	std::vector <SDL_Surface*> flipbfont;
	std::vector <SDL_Surface*> flipbfontmask;
	std::vector <int> bfontlen;

	bool flipmode;
	bool setflipmode;
	bool notextoutline;
	point tl;
	//buffer objects. //TODO refactor buffer objects
	SDL_Surface* backBuffer;
	Screen* screenbuffer;
	SDL_Surface* menubuffer;
	SDL_Surface* towerbuffer;
	SDL_Surface* foregroundBuffer;
	SDL_Surface* tempBuffer;

	SDL_Rect bfont_rect;
	SDL_Rect tiles_rect;
	SDL_Rect sprites_rect;
	SDL_Rect bfontmask_rect;
	SDL_Rect images_rect;
	SDL_Rect bg_rect;
	SDL_Rect line_rect;
	SDL_Rect tele_rect;

	SDL_Rect foot_rect;
	SDL_Rect prect;
	SDL_Rect footerrect;

	int linestate, linedelay;
	int backoffset;
	bool backgrounddrawn, foregrounddrawn;

	int menuoffset;
	bool resumegamemode;

	SDL_Rect warprect;

	int crewframe;
	int crewframedelay;

	int fademode;
	int fadeamount;
	std::vector <int> fadebars;

	bool trinketcolset;
	int trinketr, trinketg, trinketb;

	std::vector <textboxclass> textbox;
	int ntextbox;

	bool showcutscenebars;
	int cutscenebarspos;

	std::vector<SDL_Rect> stars;
	std::vector<int> starsspeed;

	int spcol, spcoldel;
	std::vector<SDL_Rect> backboxes;
	std::vector<int> backboxvx;
	std::vector<int> backboxvy;
	std::vector<float> backboxint;
	SDL_Rect backboxrect;

	int warpskip, warpfcol, warpbcol;

};

#endif /* GRAPHICS_H */
