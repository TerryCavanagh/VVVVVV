#ifndef ENT_H
#define ENT_H

class entclass
{
public:
    entclass();

    void clear();

    bool outside();

    void setenemy(int t);

public:
    //Fundamentals
    bool active, invis;
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
    bool jumping;
    bool gravity;
    int onground, onroof;
    int jumpframe;
    //Animation
    int framedelay, drawframe, walkingframe, dir, actionframe;
    int yp;int xp;
};

#endif /* ENT_H */
