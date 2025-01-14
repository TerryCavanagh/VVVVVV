#include "Ent.h"

#include "Entity.h"
#include "Game.h"
#include "Graphics.h"

entclass::entclass(void)
{
    clear();
}

void entclass::clear(void)
{
    invis = false;
    type = EntityType_PLAYER;
    render_type = EntityRenderType_SPRITE;
    tile = 0;
    rule = 0;
    state = 0;
    statedelay = 0;
    life = 0;
    colour = 0;
    para = 0;
    behave = 0;

    xp = 0;
    yp = 0;
    ax = 0;
    ay = 0;
    vx = 0;
    vy = 0;
    w = 16;
    h = 16;
    cx = 0;
    cy = 0;
    newxp = 0;
    newyp = 0;
    oldxp = 0;
    oldyp = 0;

    x1 = 0;
    y1 = 0;
    x2 = 320;
    y2 = 240;

    gravity = false;
    onground = 0;
    onroof = 0;
    collisionframedelay = 0;
    collisiondrawframe = 0;
    collisionwalkingframe = 0;
    visualonground = 0;
    visualonroof = 0;

    onentity = 0;
    harmful = false;
    onwall = 0;
    onxwall = 0;
    onywall = 0;
    isplatform = false;

    framedelay = 0;
    drawframe = 0;
    walkingframe = 0;
    dir = 0;
    actionframe = 0;

    SDL_zero(realcol);
    lerpoldxp = 0;
    lerpoldyp = 0;

    animation_frames = 4;
    animation_type = EntityAnimationType_OSCILLATE;
    animation_speed = 8;
}

bool entclass::outside(void)
{
    // Returns true if any point of the entity is outside the map.
    // Adjusts velocity for a clean collision.
    if (xp < x1)
    {
        xp = x1;
        return true;
    }
    if (yp < y1)
    {
        yp = y1;
        return true;
    }
    if (xp + w > x2)
    {
        xp = x2 - w;
        return true;
    }
    if (yp + h > y2)
    {
        yp = y2 - h;
        return true;
    }
    return false;
}

void entclass::setenemy( int t )
{
    switch(t)
    {
    case 0:
        //lies emitter
        switch ((int) para)
        {
        case 0:
            obj.set_enemy_type(this, "lies_emitter");
            break;
        case 1:
            obj.set_enemy_type(this, "lies");
            break;
        case 2:
            obj.set_enemy_type(this, "lies_collector");
            break;
        }
        break;
    case 1:
        //FACTORY emitter
        switch ((int) para)
        {
        case 0:
            obj.set_enemy_type(this, "factory_emitter");
            break;
        case 1:
            obj.set_enemy_type(this, "factory_clouds");
            break;
        case 2:
            obj.set_enemy_type(this, "factory_collector");
            break;
        }
        break;
    default:
        break;
    }
}

void entclass::setenemyroom( int rx, int ry )
{
    //Simple function to initilise simple enemies
    rx -= 100;
    ry -= 100;
    switch(rn(rx, ry))
    {
        //Space Station 1
    case rn(12, 3):  //Security Drone
        obj.set_enemy_type(this, "disc");
        break;
    case rn(13, 3):  //Wavelengths
        obj.set_enemy_type(this, "wavelength");
        break;
    case rn(15, 3):  //Traffic
        obj.set_enemy_type(this, "stop");
        break;
    case rn(12, 5):  //The Yes Men
        obj.set_enemy_type(this, "yes");
        break;
    case rn(13, 6):  //Hunchbacked Guards
        obj.set_enemy_type(this, "guard");
        break;
    case rn(13, 4):  //Communication Station
        if (xp == 256)
        {
            //transmittor
            obj.set_enemy_type(this, "transmitter");
        }
        else
        {
            //radar dish
            obj.set_enemy_type(this, "radar");
        }

        break;
        //The Lab
    case rn(4, 0):
        obj.set_enemy_type(this, "square");
        break;
    case rn(2, 0):
        obj.set_enemy_type(this, "circle");
        break;
        //Space Station 2
    case rn(13,7): // MAVVERRRICK
        obj.set_enemy_type(this, "bus");
        break;
    case rn(11, 13): //Truth
        obj.set_enemy_type(this, "truth");
        break;
    case rn(17, 7): //Brass sent us under the top
        obj.set_enemy_type(this, "soldier");
        break;
    case rn(10, 7): // (deception)
        obj.set_enemy_type(this, "bowtie");
        break;
    case rn(14, 13): // (chose poorly)
        obj.set_enemy_type(this, "skeleton");
        break;
    case rn(13, 12): // (backsliders)
        obj.set_enemy_type(this, "glitch");
        break;
    case rn(14, 8): // (wheel of fortune room)
        obj.set_enemy_type(this, "wheel");
        break;
    case rn(16, 9): // (seeing dollar signs)
        obj.set_enemy_type(this, "coin");
        break;
    case rn(16, 7): // (tomb of mad carew)
        obj.set_enemy_type(this, "ghost");
        break;
        //Warp Zone
    case rn(15, 2): // (numbers)
        obj.set_enemy_type(this, "numbers");
        break;
    case rn(16, 2): // (Manequins)
        obj.set_enemy_type(this, "mannequin");
        break;
    case rn(18, 0): // (Obey)
        obj.set_enemy_type(this, "obey");
        break;
    case rn(19, 1): // Ascending and Descending
        obj.set_enemy_type(this, "cross");
        break;
    case rn(19, 2): // Shockwave Rider
        obj.set_enemy_type(this, "triangle");
        break;
    case rn(18, 3): // Mind the gap
        obj.set_enemy_type(this, "ice");
        break;
    case rn(17, 3): // Edge Games
        if (yp == 96)
        {
            obj.set_enemy_type(this, "edgegames_left");
        }
        else
        {
            obj.set_enemy_type(this, "edgegames_right");
        }
        break;
    case rn(16, 0): // I love you
        obj.set_enemy_type(this, "heart");
        break;
    case rn(14, 2): // That's why I have to kill you
        obj.set_enemy_type(this, "broken_heart");
        break;
    case rn(18, 2): // Thinking with Portals
        //depends on direction
        if (xp == 88)
        {
            obj.set_enemy_type(this, "centipede_right");
        }
        else
        {
            obj.set_enemy_type(this, "centipede_left");
        }
        break;
        //Final level
    case rn(50-100, 53-100):  //The Yes Men
        obj.set_enemy_type(this, "yes");
        break;
    case rn(48-100, 51-100):  //Wavelengths
        obj.set_enemy_type(this, "wavelength");
        break;
    case rn(43-100,52-100): // Ascending and Descending
        obj.set_enemy_type(this, "cross");
        break;
    case rn(46-100,51-100): //kids his age
        obj.set_enemy_type(this, "circle");
        break;
    case rn(43-100,51-100): // Mind the gap
        obj.set_enemy_type(this, "ice");
        break;
    case rn(44-100,51-100): // vertigo?
        obj.set_enemy_type(this, "vertigo");
        break;
    case rn(44-100,52-100): // (backsliders)
        obj.set_enemy_type(this, "glitch");
        break;
    case rn(43-100, 56-100): //Intermission 1
        obj.set_enemy_type(this, "circle");
        break;
    case rn(45-100, 56-100): //Intermission 1
        obj.set_enemy_type(this, "circle");
        break;
        //The elephant
    case rn(11, 9):
    case rn(12, 9):
    case rn(11, 8):
    case rn(12, 8):
        obj.set_enemy_type(this, "elephant");
        break;
    }
}

void entclass::settreadmillcolour( int rx, int ry )
{
    rx -= 100;
    ry -= 100;
    rx += 50 - 12;
    ry += 50 - 14;   //Space Station

    tile = 20; //default as blue
    switch(rn(rx, ry))
    {
    case rn(52, 48):
        tile = 791;
        break; //Cyan

    case rn(49, 47):
        tile = 24;
        break; //Yellow
    case rn(56, 44):
        tile = 24;
        break; //Yellow
    case rn(54, 49):
        tile = 24;
        break; //Yellow

    case rn(49, 49):
        tile = 36;
        break; //Green
    case rn(55, 44):
        tile = 36;
        break; //Green
    case rn(54, 43):
        tile = 36;
        break; //Green
    case rn(53, 49):
        tile = 36;
        break; //Green
    case rn(54, 45):
        tile = 711;
        break; //Green (special)
    case rn(51, 48):
        tile = 711;
        break; //Green (special)

    case rn(50, 49):
        tile = 28;
        break; //Purple
    case rn(54, 44):
        tile = 28;
        break; //Purple
    case rn(49, 42):
        tile = 28;
        break; //Purple
    case rn(55, 43):
        tile = 28;
        break; //Purple
    case rn(54, 47):
        tile = 28;
        break; //Purple
    case rn(53, 48):
        tile = 28;
        break; //Purple

    case rn(51, 47):
        tile = 32;
        break; //Red
    case rn(52, 49):
        tile = 32;
        break; //Red
    case rn(48, 43):
        tile = 32;
        break; //Red
    case rn(55, 47):
        tile = 32;
        break; //Red
    case rn(54, 48):
        tile = 32;
        break; //Red
    default:
        return;
        break;
    }
}

void entclass::updatecolour(void)
{
    switch (render_type)
    {
    case EntityRenderType_SPRITE: // Sprites
    case EntityRenderType_TELEPORTER: // Teleporter
    case EntityRenderType_SPRITE_2x2: // Really Big Sprite! (2x2)
    case EntityRenderType_SPRITE_2x1: // 2x1 Sprite
    case EntityRenderType_SPRITE_6x: // Special for epilogue: huge hero!
        realcol = graphics.getcol(colour);
        break;
    case EntityRenderType_PARTICLE: // Big chunky pixels!
        realcol = graphics.bigchunkygetcol(colour);
        break;
    case EntityRenderType_COIN: // Small pickups
        realcol = graphics.huetilegetcol();
        break;
    case EntityRenderType_ELEPHANT: // The fucking elephant
        if (game.noflashingmode)
        {
            realcol = graphics.getcol(22);
        }
        else
        {
            realcol = graphics.getcol(colour);
        }
        break;
    case EntityRenderType_SPRITE_NO_WRAP: // Regular sprites that don't wrap
        // if we're outside the screen, we need to draw indicators
        if ((xp < -20 && vx > 0) || (xp > 340 && vx < 0))
        {
            realcol = graphics.getcol(23);
        }
        else
        {
            realcol = graphics.getcol(colour);
        }
    }
}

bool entclass::ishumanoid(void)
{
    return type == EntityType_PLAYER
        || type == EntityType_CREWMATE
        || type == EntityType_SUPERCREWMATE
        || type == EntityType_COLLECTABLE_CREWMATE;
}
