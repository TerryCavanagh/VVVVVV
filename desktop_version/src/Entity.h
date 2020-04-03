#ifndef ENTITY_H
#define ENTITY_H

#include "Maths.h"
#include "Ent.h"
#include "BlockV.h"

#include <vector>
#include <string>

#define removeentity_iter(index) { obj.removeentity(index); index--; }

enum
{
    BLOCK = 0,
    TRIGGER = 1,
    DAMAGE = 2,
    DIRECTIONAL = 3,
    SAFE = 4,
    ACTIVITY = 5
};

class entityclass
{
public:
    void init();

    void resetallflags();

    void resetflags();

    void confirmflags();

    void changecollect(int t, int s);

    void changecustomcollect(int t, int s);

    void changeflag(int t, int s);

    void fatal_top()
    {
        createblock(DAMAGE, -8, -8, 384, 16);
    }
    void fatal_bottom()
    {
        createblock(DAMAGE, -8, 224, 384, 16);
    }
    void fatal_left()
    {
        createblock(DAMAGE, -8, -8, 16, 260);
    }
    void fatal_right()
    {
        createblock(DAMAGE, 312, -8, 16, 260);
    }

    void setblockcolour(int t, std::string col);

    int swncolour(int t );

    void swnenemiescol(int t);

    void gravcreate(int ypos, int dir, int xoff = 0, int yoff = 0);

    void generateswnwave(int t);

    void createblock(int t, int xp, int yp, int w, int h, int trig = 0);

    void removeentity(int t);

    void removeallblocks();

    void removeblock(int t);

    void removeblockat(int x, int y);

    void removetrigger(int t);

    void copylinecross(int t);

    void revertlinecross(int t, int s);

    bool gridmatch(int p1, int p2, int p3, int p4, int p11, int p21, int p31, int p41);

    int crewcolour(int t);

    void createentity(float xp, float yp, int t, float vx = 0, float vy = 0,
                      int p1 = 0, int p2 = 0, int p3 = 320, int p4 = 240 );

    bool updateentities(int i);

    void animateentities(int i);

    bool gettype(int t);

    int getcompanion();

    int getplayer();

    int getscm();

    int getlineat(int t);

    int getcrewman(int t);
    int getcustomcrewman(int t);

    int getteleporter();

    void rectset(int xi, int yi, int wi, int hi);

    void rect2set(int xi, int yi, int wi, int hi);

    bool entitycollide(int a, int b);

    bool checkdamage();

    bool scmcheckdamage();

    void settemprect(int t);

    int checktrigger();

    int checkactivity();

    int getgridpoint(int t);

    bool cblocks(int t);

    bool checkplatform();

    bool checkblocks();

    bool checktowerspikes(int t);

    bool checkwall();

    float hplatformat();

    int yline(int a, int b);

    bool entityhlinecollide(int t, int l);

    bool entityvlinecollide(int t, int l);

    bool entitywarphlinecollide(int t, int l);
    bool entitywarpvlinecollide(int t, int l);

    void customwarplinecheck(int i);

    float entitycollideplatformroof(int t);

    float entitycollideplatformfloor(int t);

    bool entitycollidefloor(int t);

    bool entitycollideroof(int t);

    bool testwallsx(int t, int tx, int ty);

    bool testwallsy(int t, float tx, float ty);

    void fixfriction(int t, float xfix, float xrate, float yrate);

    void applyfriction(int t, float xrate, float yrate);

    void cleanup();

    void updateentitylogic(int t);


    void entitymapcollision(int t);

    void movingplatformfix(int t);

    void scmmovingplatformfix(int t);

    void hormovingplatformfix(int t);

    void entitycollisioncheck();


    std::vector<entclass> entities;

    int nentity;

    std::vector<entclass> linecrosskludge;
    int nlinecrosskludge;

    point colpoint1, colpoint2;

    int tempx, tempy, tempw, temph, temp, temp2;
    //public var tempx:int, tempy:int, tempw:int, temph:int, temp:int, temp2:int;
    int tpx1, tpy1, tpx2, tpy2;

    SDL_Rect temprect, temprect2;

    int x, k;
    float dx, dy, dr;

    int px, py, linetemp;
    int activetrigger;


    std::vector<blockclass> blocks;
    std::vector<int> flags;
    std::vector<int> collect;
    std::vector<int> customcollect;

    int nblocks;
    bool skipblocks, skipdirblocks;

    int platformtile;
    bool vertplatforms, horplatforms;

    // :(
    bool nearelephant, upsetmode;
    int upset;

    //Trophy Text
    int trophytext, trophytype;

    //Secret lab scripts
    int altstates;

    //Custom stuff
    int customenemy;
    int customplatformtile;
    bool customwarpmode, customwarpmodevon, customwarpmodehon;
    std::string customscript;
    int customcrewmoods[6];
};

extern entityclass obj;

#endif /* ENTITY_H */
