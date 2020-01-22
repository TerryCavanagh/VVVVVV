#ifndef ENTITY_H
#define ENTITY_H

#include "Maths.h"
#include "Ent.h"
#include "BlockV.h"

#include <vector>
#include <string>

#define		rn( rx,  ry) ((rx) + ((ry) * 100))

enum
{
    BLOCK = 0,
    TRIGGER = 1,
    DAMAGE = 2,
    DIRECTIONAL = 3,
    SAFE = 4,
    ACTIVITY = 5
};

class mapclass;
class musicclass;
class Graphics;
class Game;
class UtilityClass;

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

    void gravcreate(Game& game, int ypos, int dir, int xoff = 0, int yoff = 0);

    void generateswnwave(Game& game, UtilityClass& help, int t);

    void _createblock(int t, int xp, int yp, int w, int h, int trig = 0);
    template<class A, class B, class C, class D, class E, class F>
    void createblock(A t, B xp, C yp, D w, E h, F trig)
    {
        _createblock(static_cast<int>(t), static_cast<int>(xp), static_cast<int>(yp), static_cast<int>(w), static_cast<int>(h), static_cast<int>(trig));
    }
    template<class A, class B, class C, class D, class E>
    void createblock(A t, B xp, C yp, D w, E h)
    {
        _createblock(static_cast<int>(t), static_cast<int>(xp), static_cast<int>(yp), static_cast<int>(w), static_cast<int>(h));
    }

    void removeallblocks();

    void removeblock(int t);

    void removeblockat(int x, int y);

    void removetrigger(int t);

    void copylinecross(int t);

    void revertlinecross(int t, int s);

    bool gridmatch(int p1, int p2, int p3, int p4, int p11, int p21, int p31, int p41);

    int crewcolour(int t);

    void setenemyroom(int t, int rx, int ry);

    void setenemy(int t, int r);

    void settreadmillcolour(int t, int rx, int ry);
    /*
     * Callers of createentity relied on default casts, which would throw warnings by the compiler.
     * Instead of littering the code with static_casts, instead templatise the function and have the
     * compiler generate the required function parameter permutations for us.
     * static_cast to original function's types should make compiler error if a bad type is passed
     * in future; this isn't carte-blanche pass anything in.
    */
    void _createentity(Game& game, float xp, float yp, int t, float vx = 0, float vy = 0,
                      int p1 = 0, int p2 = 0, int p3 = 320, int p4 = 240 );
    template <class A, class B, class C, class D, class E, class F, class G, class H, class I>
    void createentity(Game& game, A xp, B yp, C t, D vx, E vy, F p1, G p2, H p3, I p4 )
    {
        return _createentity(game, static_cast<float>(xp), static_cast<float>(yp), static_cast<int>(t), static_cast<float>(vx), static_cast<float>(vy), static_cast<int>(p1), static_cast<int>(p2), static_cast<int>(p3), static_cast<int>(p4));
    }
    /* Trouble is, templated parms can't be defaulted. So we need to supply more templates for
    * signatures which match missing parameters too
    */
    template <class A, class B, class C, class D, class E, class F, class G, class H>
    void createentity(Game& game, A xp, B yp, C t, D vx, E vy, F p1, G p2, H p3)
    {
        return _createentity(game, static_cast<float>(xp), static_cast<float>(yp), static_cast<int>(t), static_cast<float>(vx), static_cast<float>(vy), static_cast<int>(p1), static_cast<int>(p2), static_cast<int>(p3));
    }
    template <class A, class B, class C, class D, class E, class F, class G>
    void createentity(Game& game, A xp, B yp, C t, D vx, E vy, F p1, G p2)
    {
        return _createentity(game, static_cast<float>(xp), static_cast<float>(yp), static_cast<int>(t), static_cast<float>(vx), static_cast<float>(vy), static_cast<int>(p1), static_cast<int>(p2));
    }
    template <class A, class B, class C, class D, class E, class F>
    void createentity(Game& game, A xp, B yp, C t, D vx, E vy, F p1)
    {
        return _createentity(game, static_cast<float>(xp), static_cast<float>(yp), static_cast<int>(t), static_cast<float>(vx), static_cast<float>(vy), static_cast<int>(p1));
    }
    template <class A, class B, class C, class D, class E>
    void createentity(Game& game, A xp, B yp, C t, D vx, E vy )
    {
        return _createentity(game, static_cast<float>(xp), static_cast<float>(yp), static_cast<int>(t), static_cast<float>(vx), static_cast<float>(vy));
    }
    template <class A, class B, class C, class D>
    void createentity(Game& game, A xp, B yp, C t, D vx )
    {
        return _createentity(game, static_cast<float>(xp), static_cast<float>(yp), static_cast<int>(t), static_cast<float>(vx));
    }
    template <class A, class B, class C>
    void createentity(Game& game, A xp, B yp, C t)
    {
        return _createentity(game, static_cast<float>(xp), static_cast<float>(yp), static_cast<int>(t));
    }
    /* End of templates for createentity's variable number of parameters */
    bool updateentities(int i, UtilityClass& help, Game& game, musicclass& music);

    void animateentities(int i, Game& game, UtilityClass& help);

    bool gettype(int t);

    int getcompanion(int t);

    int getplayer();

    int getscm();

    int getlineat(int t);

    int getcrewman(int t);
    int getcustomcrewman(int t);

    int getteleporter();

    void rectset(int xi, int yi, int wi, int hi);

    void rect2set(int xi, int yi, int wi, int hi);

    bool entitycollide(int a, int b);

    bool checkdirectional(int t);

    bool checkdamage();

    bool scmcheckdamage();

    void settemprect(int t);

    int checktrigger();

    int checkactivity();

    int getgridpoint(int t);

    bool cblocks(int t);

    bool checkplatform();

    bool checkblocks();

    bool checktowerspikes(int t, mapclass& map);

    bool checkwall(mapclass& map);

    float hplatformat();

    int yline(int a, int b);

    bool entityhlinecollide(int t, int l);

    bool entityvlinecollide(int t, int l);
		
		bool entitywarphlinecollide(int t, int l);
		bool entitywarpvlinecollide(int t, int l);

		void customwarplinecheck(int i);

    float entitycollideplatformroof(mapclass& map, int t);

    float entitycollideplatformfloor(mapclass& map, int t);

    bool entitycollidefloor(mapclass& map, int t);

    bool entitycollideroof(mapclass& map, int t);

    bool _testwallsx(int t, mapclass& map, int tx, int ty);
    template<class A, class B, class C>
    bool testwallsx(A t, mapclass& map, B tx, C ty)
    {
        return _testwallsx(static_cast<int>(t), map, static_cast<int>(tx), static_cast<int>(ty));
    }

	bool _testwallsy(int t, mapclass& map, float tx, float ty);
    template<class A, class B, class C>
    bool testwallsy(A t, mapclass& map, B tx, C ty)
    {
        return _testwallsy(static_cast<int>(t), map, static_cast<float>(tx), static_cast<float>(ty));
    }

    void fixfriction(int t, float xfix, float xrate, float yrate);

    void applyfriction(int t, float xrate, float yrate);

    void cleanup();

    void updateentitylogic(int t, Game& game);


    void entitymapcollision(int t, mapclass& map);

    void movingplatformfix(int t, mapclass& map);

    void scmmovingplatformfix(int t, mapclass& map);

    void hormovingplatformfix(int t, mapclass& map);

    void entitycollisioncheck(Graphics& dwgfx, Game& game, mapclass& map, musicclass& music);


    std::vector<entclass> entities;

    int nentity;

    std::vector<entclass> linecrosskludge;
    int nlinecrosskludge;

    point colpoint1, colpoint2;

    int tempx, tempy, tempw, temph, temp, temp2;
    //public var tempx:int, tempy:int, tempw:int, temph:int, temp:int, temp2:int;
    int tpx1, tpy1, tpx2, tpy2;

    SDL_Rect temprect, temprect2;

    int /*x,*/ k;
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

#endif /* ENTITY_H */
