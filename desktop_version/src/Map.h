#ifndef MAPGAME_H
#define MAPGAME_H

#include <vector>
#include <string>

#include "Finalclass.h"
#include "Labclass.h"
#include "Otherlevel.h"
#include "Spacestation2.h"
#include "Tower.h"
#include "TowerBG.h"
#include "WarpClass.h"

struct Roomtext
{
    int x, y;
    const char* text;
};

enum RoomnameType
{
    RoomnameType_STATIC,
    RoomnameType_GLITCH,
    RoomnameType_TRANSFORM
};

struct Roomname
{
    int x;
    int y;
    bool loop;
    int flag;
    RoomnameType type;
    std::vector<std::string> text;
    int progress;
    int delay;
};

struct MarkerRoom
{
    SDL_Point coords;
    int flag;
    int trinket_id;
};

struct MapMarker
{
    std::string name;
    bool show_hidden;
    bool show_visited;
    std::vector<MarkerRoom> rooms;
    int hidden_id;
    int visited_id;
    int flip_hidden_id;
    int flip_visited_id;
};

class mapclass
{
public:
    mapclass(void);
    void destroy(void);

    int getwidth(void);

    int getheight(void);

    int intpol(int a, int b, float c);

    void set_targets_visible(bool visible);
    void set_teleporters_visible(bool visible);

    void setteleporter(int x, int y);

    void settrinket(int id, int x, int y);

    void setroomname(const char* name);

    void resetmap(void);

    void updateroomnames(void);

    void initmapdata(void);
    void initcustommapdata(void);

    void roomnamechange(int x, int y, const char** lines, size_t size);
    void roomnameglitch(int x, int y, const char* name, const char* glitch);

    int finalat(int x, int y);

    int maptiletoenemycol(int t);

    void changefinalcol(int t);

    void setcol(TowerBG& bg_obj, const int r1, const int g1, const int b1 , const int r2, const  int g2, const int b2, const int c);

    void updatebgobj(TowerBG& bg_obj);

    void setbgobjlerp(TowerBG& bg_obj);

    void updatetowerglow(TowerBG& bg_obj);

    void nexttowercolour(void);
    bool nexttowercolour_set;

    void settowercolour(int t);

    bool towerspikecollide(int x, int y);

    bool collide(int x, int y, bool invincible);

    void settile(int xp, int yp, int t);


    int area(int _rx, int _ry);

    void exploretower(void);

    void hideship(void);

    void showship(void);

    void resetplayer(const bool player_died);
    void resetplayer(void);

    void warpto(int rx, int ry , int t, int tx, int ty);

    void gotoroom(int rx, int ry);

    void spawncompanion(void);

    const char* currentarea(int t);

    void loadlevel(int rx, int ry);

    void twoframedelayfix(void);


    int roomdeaths[20 * 20];
    int roomdeathsfinal[20 * 20];
    static const int areamap[20 * 20];
    int contents[40 * 30];
    bool explored[20 * 20];

    bool isexplored(const int rx, const int ry);
    void setexplored(const int rx, const int ry, const bool status);

    int background;
    int rcol;
    int tileset;
    bool warpx;
    bool warpy;


    const char* roomname;
    bool roomname_special;
    bool roomnameset;
    const char* hiddenname;

    std::vector<Roomname> specialroomnames;

    //Special tower stuff
    bool towermode;
    int ypos;
    int oldypos;
    int cameramode;
    int cameraseek, cameraseekframe;
    int resumedelay;
    bool minitowermode;

    int colstatedelay;
    int colsuperstate;
    int spikeleveltop, spikelevelbottom;
    int oldspikeleveltop, oldspikelevelbottom;
    //final level navigation
    bool finalmode;
    bool finalstretch;

    //Variables for playing custom levels
    bool custommode;
    bool custommodeforreal;
    int custommmxoff, custommmyoff, custommmxsize, custommmysize;
    int customzoom;
    bool customshowmm;

    //final level colour cycling stuff
    bool final_colormode;
    int final_mapcol;
    int final_aniframe;
    int final_aniframedelay;
    int final_colorframe, final_colorframedelay;

    // Teleporters
    std::vector<SDL_Point> teleporters;

    MapMarker shinytrinkets;
    MapMarker teleporter_markers;

    bool showteleporters, showtargets, showtrinkets;

    // Custom markers!
    std::vector<MapMarker> markers;

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
