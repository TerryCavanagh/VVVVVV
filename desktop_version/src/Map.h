#ifndef MAPGAME_H
#define MAPGAME_H

#include <vector>

#include "Finalclass.h"
#include "Labclass.h"
#include "Maths.h"
#include "Otherlevel.h"
#include "Spacestation2.h"
#include "Tower.h"
#include "TowerBG.h"
#include "WarpClass.h"

struct Roomtext
{
    int x, y;
    std::string text;
};

class mapclass
{
public:
    mapclass();

    int intpol(int a, int b, float c);

    void setteleporter(int x, int y);

    void settrinket(int x, int y);

    void resetmap();

    void resetnames();

    void transformname(int t);

    std::string getglitchname(int x, int y);

    void initmapdata();
    void initcustommapdata();

    int finalat(int x, int y);

    int maptiletoenemycol(int t);

    void changefinalcol(int t);

    void setcol(TowerBG& bg_obj, const int r1, const int g1, const int b1 , const int r2, const  int g2, const int b2, const int c);

    void updatebgobj(TowerBG& bg_obj);

    void updatetowerglow(TowerBG& bg_obj);

    void nexttowercolour();

    void settowercolour(int t);

    bool spikecollide(int x, int y);

    bool collide(int x, int y);

    void settile(int xp, int yp, int t);


    int area(int _rx, int _ry);

    void exploretower();

    void hideship();

    void showship();

    void resetplayer(const bool player_died);
    void resetplayer();

    void warpto(int rx, int ry , int t, int tx, int ty);

    void gotoroom(int rx, int ry);

    std::string currentarea(int t);

    void loadlevel(int rx, int ry);

    void twoframedelayfix();


    int roomdeaths[20 * 20];
    int roomdeathsfinal[20 * 20];
    static const int areamap[20 * 20];
    short contents[40 * 30];
    bool explored[20 * 20];
    int vmult[30];

    int background;
    int rcol;
    int tileset;
    bool warpx;
    bool warpy;


    std::string roomname;
    std::string hiddenname;

    //Special tower stuff
    bool towermode;
    float ypos;
    float oldypos;
    int cameramode;
    int cameraseek, cameraseekframe;
    int resumedelay;
    bool minitowermode;

    int colstatedelay;
    int colsuperstate;
    int spikeleveltop, spikelevelbottom;
    int oldspikeleveltop, oldspikelevelbottom;
    //final level navigation
    int finalx;
    int finaly;
    bool finalmode;
    bool finalstretch;

    //Variables for playing custom levels
    bool custommode;
    bool custommodeforreal;
    int customx, customy;
    int customwidth, customheight;
    int custommmxoff, custommmyoff, custommmxsize, custommmysize;
    int customzoom;
    bool customshowmm;

    std::string specialnames[8];
    int glitchmode;
    int glitchdelay;
    std::string glitchname;

    //final level colour cycling stuff
    bool final_colormode;
    int final_mapcol;
    int final_aniframe;
    int final_aniframedelay;
    int final_colorframe, final_colorframedelay;

    //Teleporters and Trinkets on the map
    std::vector<point> teleporters;
    std::vector<point> shinytrinkets;

    bool showteleporters, showtargets, showtrinkets;

    //Roomtext
    bool roomtexton;
    std::vector<Roomtext> roomtext;

    //Levels
    otherlevelclass otherlevel;
    spacestation2class spacestation2;
    labclass lablevel;
    finalclass finallevel;
    warpclass warplevel;
    towerclass tower;
    int extrarow;

    //Accessibility options
    bool invincibility;

    //Map cursor
    int cursorstate, cursordelay;
};

#ifndef MAP_DEFINITION
extern mapclass map;
#endif

#endif /* MAPGAME_H */
