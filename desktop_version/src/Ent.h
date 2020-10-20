#ifndef ENT_H
#define ENT_H

#include <SDL.h>

#define		rn( rx,  ry) ((rx) + ((ry) * 100))

class entclass
{
public:
    entclass();

    bool outside();

    void setenemy(int t);

    void setenemyroom(int rx, int ry);

    void settreadmillcolour(int rx, int ry);

    void updatecolour();

    bool ishumanoid();

public:
    //Fundamentals
    bool invis;
    int type, size, tile, rule;
    int state, statedelay;
    int behave, animate;
    float para;
    int life, colour;

    //Position and velocity
    int oldxp, oldyp;
    float ax, ay, vx, vy;
    int cx, cy, w, h;
    float newxp, newyp;
    bool isplatform;
    int x1,y1,x2,y2;
    //Collision Rules
    int onentity;
    bool harmful;
    int onwall, onxwall, onywall;

    //Platforming specific
    bool gravity;
    int onground, onroof;
    //Animation
    int framedelay, drawframe, walkingframe, dir, actionframe;
    int yp;int xp;

    Uint32 realcol;
    int lerpoldxp, lerpoldyp;
};

#endif /* ENT_H */
