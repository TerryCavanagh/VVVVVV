#ifndef ENT_H
#define ENT_H

#include <SDL.h>

#define        rn( rx,  ry) ((rx) + ((ry) * 100))

enum EntityTypes
{
    EntityType_INVALID = -1,
    EntityType_PLAYER,
    EntityType_MOVING,
    EntityType_DISAPPEARING_PLATFORM,
    EntityType_QUICKSAND,
    EntityType_GRAVITY_TOKEN,
    EntityType_PARTICLE,
    EntityType_COIN,
    EntityType_TRINKET,
    EntityType_CHECKPOINT,
    EntityType_HORIZONTAL_GRAVITY_LINE,
    EntityType_VERTICAL_GRAVITY_LINE,
    EntityType_WARP_TOKEN,
    EntityType_CREWMATE,
    EntityType_TERMINAL,
    EntityType_SUPERCREWMATE,
    EntityType_TROPHY,
    EntityType_GRAVITRON_ENEMY = 23,
    EntityType_WARP_LINE_LEFT = 51,
    EntityType_WARP_LINE_RIGHT = 52,
    EntityType_WARP_LINE_TOP = 53,
    EntityType_WARP_LINE_BOTTOM = 54,
    EntityType_COLLECTABLE_CREWMATE = 55,
    EntityType_TELEPORTER = 100
};

enum EntityRenderTypes
{
    EntityRenderType_INVALID = -1,
    EntityRenderType_SPRITE,
    EntityRenderType_TILE,
    EntityRenderType_PLATFORM,
    EntityRenderType_PARTICLE,
    EntityRenderType_COIN,
    EntityRenderType_HORIZONTAL_LINE,
    EntityRenderType_VERTICAL_LINE,
    EntityRenderType_TELEPORTER,
    EntityRenderType_PLATFORM_LONG,
    EntityRenderType_SPRITE_2x2,
    EntityRenderType_SPRITE_2x1,
    EntityRenderType_ELEPHANT,
    EntityRenderType_SPRITE_NO_WRAP,
    EntityRenderType_SPRITE_6x
};

enum EntityAnimationTypes
{
    EntityAnimationType_STILL,
    EntityAnimationType_OSCILLATE,
    EntityAnimationType_LOOP,
    EntityAnimationType_ONESHOT,
    EntityAnimationType_CONVEYOR_LEFT,
    EntityAnimationType_CONVEYOR_RIGHT
};

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
    EntityTypes type;
    EntityRenderTypes render_type;
    int tile, rule;
    int state, statedelay;
    int behave;
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
    int animation_frames;
    EntityAnimationTypes animation_type;
    int animation_speed;
    int collisionframedelay, collisiondrawframe, collisionwalkingframe;
    int visualonground, visualonroof;
    int yp;int xp;

    SDL_Color realcol;
    int lerpoldxp, lerpoldyp;
};

#endif /* ENT_H */
