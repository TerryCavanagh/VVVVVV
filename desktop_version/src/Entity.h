#ifndef ENTITY_H
#define ENTITY_H

#include <SDL.h>
#include <string>
#include <vector>

#include "Maths.h"
#include "Ent.h"
#include "BlockV.h"
#include "Game.h"

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
    void init(void);

    void resetallflags(void);

    void fatal_top(void)
    {
        createblock(DAMAGE, -8, -8, 384, 16);
    }
    void fatal_bottom(void)
    {
        createblock(DAMAGE, -8, 224, 384, 16);
    }
    void fatal_left(void)
    {
        createblock(DAMAGE, -8, -8, 16, 260);
    }
    void fatal_right(void)
    {
        createblock(DAMAGE, 312, -8, 16, 260);
    }

    int swncolour(int t );

    void swnenemiescol(int t);

    void gravcreate(int ypos, int dir, int xoff = 0, int yoff = 0);

    void generateswnwave(int t);

    void createblock(int t, int xp, int yp, int w, int h, int trig = 0, const std::string& script = "");

    bool disableentity(int t);

    void removeallblocks(void);

    void disableblock(int t);

    void disableblockat(int x, int y);

    void moveblockto(int x1, int y1, int x2, int y2, int w, int h);

    void removetrigger(int t);

    void copylinecross(int t);

    void revertlinecross(int t, int s);

    bool gridmatch(int p1, int p2, int p3, int p4, int p11, int p21, int p31, int p41);

    int crewcolour(int t);

    void createentity(int xp, int yp, int t, int meta1, int meta2,
                      int p1, int p2, int p3, int p4);
    void createentity(int xp, int yp, int t, int meta1, int meta2,
                      int p1, int p2);
    void createentity(int xp, int yp, int t, int meta1, int meta2,
                      int p1);
    void createentity(int xp, int yp, int t, int meta1, int meta2);
    void createentity(int xp, int yp, int t, int meta1);
    void createentity(int xp, int yp, int t);

    bool updateentities(int i);

    void animateentities(int i);

    int getcompanion(void);

    int getplayer(void);

    int getscm(void);

    int getlineat(int t);

    int getcrewman(int t);
    int getcustomcrewman(int t);

    int getteleporter(void);

    bool entitycollide(int a, int b);

    bool checkdamage(bool scm = false);

    int checktrigger(int* block_idx);

    int checkactivity(void);

    int getgridpoint(int t);

    bool checkplatform(const SDL_Rect& temprect, int* px, int* py);

    bool checkblocks(const SDL_Rect& temprect, const float dx, const float dy, const float dr, const bool skipdirblocks);

    bool checktowerspikes(int t);

    bool checkwall(const SDL_Rect& temprect, const float dx, const float dy, const float dr, const bool skipblocks, const bool skipdirblocks);
    bool checkwall(const SDL_Rect& temprect);

    float hplatformat(const int px, const int py);

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

    bool testwallsx(int t, int tx, int ty, const bool skipdirblocks);

    bool testwallsy(int t, float tx, float ty);

    void applyfriction(int t, float xrate, float yrate);

    void updateentitylogic(int t);


    void entitymapcollision(int t);

    void movingplatformfix(int t, int j);

    void entitycollisioncheck(void);

    void collisioncheck(int i, int j, bool scm = false);

    void stuckprevention(int t);


    std::vector<entclass> entities;

    std::vector<entclass> linecrosskludge;

    int k;


    std::vector<blockclass> blocks;
    bool flags[100];
    bool collect[100];
    bool customcollect[100];

    int platformtile;
    bool vertplatforms, horplatforms;

    // :(
    bool nearelephant, upsetmode;
    int upset;

    //Trophy Text
    int trophytext, trophytype;
    int oldtrophytext;

    //Secret lab scripts
    int altstates;

    //Custom stuff
    int customenemy;
    int customplatformtile;
    bool customwarpmode, customwarpmodevon, customwarpmodehon;
    std::string customscript;
    bool customcrewmoods[Game::numcrew];
};

#ifndef OBJ_DEFINITION
extern entityclass obj;
#endif

#endif /* ENTITY_H */
