#ifndef ENTITY_H
#define ENTITY_H

#include <SDL.h>
#include <string>
#include <vector>

#include "Maths.h"
#include "Ent.h"
#include "BlockV.h"
#include "Game.h"

#define removeentity_iter(index) { if (obj.removeentity(index)) index--; }
#define removeblock_iter(index) { obj.removeblock(index); index--; }

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

    int swncolour(int t );

    void swnenemiescol(int t);

    void gravcreate(int ypos, int dir, int xoff = 0, int yoff = 0);

    void generateswnwave(int t);

    void createblock(int t, int xp, int yp, int w, int h, int trig = 0, const std::string& script = "");

    bool removeentity(int t);

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

    int getcompanion();

    int getplayer();

    int getscm();

    int getlineat(int t);

    int getcrewman(int t);
    int getcustomcrewman(int t);

    int getteleporter();

    bool entitycollide(int a, int b);

    bool checkdamage(bool scm = false);

    int checktrigger(int* block_idx);

    int checkactivity();

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

    void fixfriction(int t, float xfix, float xrate, float yrate);

    void applyfriction(int t, float xrate, float yrate);

    void updateentitylogic(int t);


    void entitymapcollision(int t);

    void movingplatformfix(int t, int j);

    void hormovingplatformfix(int t);

    void entitycollisioncheck();

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

extern entityclass obj;

#endif /* ENTITY_H */
