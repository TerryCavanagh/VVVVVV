#ifndef ENT_H
#define ENT_H

#include <SDL.h>

#define        rn( rx,  ry) ((rx) + ((ry) * 100))

class entclass
{
public:
    entclass(void);
    void clear(void);

    bool outside(void);

    void setenemy(int t);

    void setenemyroom(int rx, int ry);

    void settreadmillcolour(int rx, int ry);

    void updatecolour(void);

    bool ishumanoid(void);

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
    int p1x, p1y, p2x, p2y, p3x, p3y;
    int pathtime, pathmaxtime;
    //Collision Rules
    int onentity;
    bool harmful;
    int onwall, onxwall, onywall;

    //Platforming specific
    bool gravity;
    int onground, onroof;
    //Animation
    int framedelay, drawframe, walkingframe, dir, actionframe;
    int collisionframedelay, collisiondrawframe, collisionwalkingframe;
    int visualonground, visualonroof;
    int yp;int xp;

    SDL_Color realcol;
    int lerpoldxp, lerpoldyp;
};

#endif /* ENT_H */
