#define MAP_DEFINITION
#include "Map.h"

#include "Alloc.h"
#include "Constants.h"
#include "CustomLevels.h"
#include "Entity.h"
#include "Game.h"
#include "GlitchrunnerMode.h"
#include "Graphics.h"
#include "Localization.h"
#include "MakeAndPlay.h"
#include "Maths.h"
#include "Music.h"
#include "Script.h"
#include "Unused.h"
#include "UtilityClass.h"

mapclass::mapclass(void)
{
    //Start here!
    colstatedelay = 0;
    colsuperstate = 0;
    spikeleveltop = 0;
    spikelevelbottom = 0;
    oldspikeleveltop = 0;
    oldspikelevelbottom = 0;
    warpx = false;
    warpy = false;
    extrarow = 0;

    showteleporters = false;
    showtargets = false;
    showtrinkets = false;

    finalmode = false;
    finalstretch = false;

    cursorstate = 0;
    cursordelay = 0;

    towermode = false;
    cameraseekframe = 0;
    resumedelay = 0;

    final_colormode = false;
    final_colorframe = 0;
    final_colorframedelay = 0;
    final_mapcol = 0;
    final_aniframe = 0;
    final_aniframedelay = 0;

    custommode=false;
    custommodeforreal=false;
    custommmxoff=0; custommmyoff=0; custommmxsize=0; custommmysize=0;
    customzoom=0;
    customshowmm=true;

    rcol = 0;

    //This needs to be in map instead!
    invincibility = false;

    //We create a blank map
    SDL_memset(contents, 0, sizeof(contents));

    SDL_memset(roomdeaths, 0, sizeof(roomdeaths));
    SDL_memset(roomdeathsfinal, 0, sizeof(roomdeathsfinal));
    resetmap();

    setroomname("");
    hiddenname = "";

    roomname_special = false;
    specialroomnames.clear();
    roomnameset = false;

    tileset = 0;
    initmapdata();

    ypos = 0;
    oldypos = 0;

    background = 0;
    cameramode = 0;
    cameraseek = 0;
    minitowermode = false;
    roomtexton = false;

    nexttowercolour_set = false;
}

static char roomname_static[SCREEN_WIDTH_CHARS];
static char* roomname_heap;

void mapclass::destroy(void)
{
    VVV_free(roomname_heap);
}

//Areamap starts at 100,100 and extends 20x20
const int mapclass::areamap[] = {
    1,2,2,2,2,2,2,2,0,3,0,0,0,4,4,4,4,4,4,4,
    1,2,2,2,2,2,2,0,0,3,0,0,0,0,4,4,4,4,4,4,
    0,1,0,0,2,0,0,0,0,3,0,0,0,0,4,4,4,4,4,4,
    0,0,0,0,2,0,0,0,0,3,0,0,5,5,5,5,4,4,4,4,
    0,0,2,2,2,0,0,0,0,3,11,11,5,5,5,5,0,0,0,0,
    0,0,0,0,0,0,0,0,0,3,5,5,5,5,5,5,0,0,0,0,
    0,0,0,0,0,0,0,0,0,3,5,5,5,5,5,5,5,0,0,0,
    0,0,0,0,0,0,0,0,0,3,5,5,5,5,5,5,5,5,5,0,
    0,0,0,0,0,0,0,0,0,3,0,0,0,5,5,5,5,5,5,0,
    0,0,0,0,0,0,0,0,11,3,0,0,0,5,5,5,5,5,5,0,
    0,0,0,0,0,0,0,0,0,3,0,0,0,5,5,5,5,5,5,0,
    0,0,0,0,0,0,0,0,0,3,0,5,5,5,5,5,5,5,5,0,
    0,0,0,0,0,0,0,0,0,3,0,5,5,5,5,5,5,0,5,0,
    0,0,0,0,0,0,0,0,0,3,0,5,5,5,5,5,5,0,5,0,
    0,0,0,0,0,0,0,0,0,3,0,5,5,0,0,0,0,0,5,0,
    0,0,0,0,0,0,0,2,0,3,0,0,0,0,0,0,0,0,0,0,
    0,0,2,2,2,2,2,2,0,3,0,0,0,0,0,0,0,0,0,0,
    0,2,2,2,2,2,2,2,0,3,0,0,0,0,0,0,0,0,0,0,
    2,2,2,2,2,0,0,2,0,3,0,0,0,0,0,0,0,0,0,0,
    2,2,2,2,2,0,0,2,0,3,0,0,0,0,0,0,0,0,0,0,
};

int mapclass::getwidth(void)
{
#ifndef NO_CUSTOM_LEVELS
    if (custommode)
    {
        return cl.mapwidth;
    }
#endif

    return 20;
}

int mapclass::getheight(void)
{
#ifndef NO_CUSTOM_LEVELS
    if (custommode)
    {
        return cl.mapheight;
    }
#endif

    return 20;
}

int mapclass::intpol(int a, int b, float c)
{
    return static_cast<int>(a + ((b - a) * c));
}

void mapclass::setteleporter(int x, int y)
{
    if (x < 0 || x >= getwidth() || y < 0 || y >= getheight())
    {
        return;
    }

    SDL_Point temp;
    temp.x = x;
    temp.y = y;
    teleporters.push_back(temp);
}

void mapclass::settrinket(int x, int y)
{
    if (x < 0 || x >= getwidth() || y < 0 || y >= getheight())
    {
        return;
    }

    SDL_Point temp;
    temp.x = x;
    temp.y = y;
    shinytrinkets.push_back(temp);
}

void mapclass::setroomname(const char* name)
{
    VVV_free(roomname_heap);

    const size_t size = SDL_strlcpy(
        roomname_static, name, sizeof(roomname_static)
    ) + 1;
    roomname = roomname_static;

    if (size > sizeof(roomname_static))
    {
        roomname_heap = SDL_strdup(name);
        if (roomname_heap != NULL)
        {
            roomname = roomname_heap;
        }
    }
}

void mapclass::resetmap(void)
{
    //clear the explored area of the map
    SDL_memset(explored, 0, sizeof(explored));
}

void mapclass::updateroomnames(void)
{
    if (roomnameset)
    {
        return;
    }

    const int rx = game.roomx;
    const int ry = game.roomy;

    for (int i = specialroomnames.size() - 1; i >= 0; i--)
    {
        Roomname* roomname = &specialroomnames[i];
        if (rx == roomname->x && ry == roomname->y && (roomname->flag == -1 || (INBOUNDS_ARR(roomname->flag, obj.flags) && obj.flags[roomname->flag])))
        {
            roomname_special = true;
            if (roomname->type == RoomnameType_STATIC)
            {
                setroomname(roomname->text[0].c_str());
            }
            if (roomname->type == RoomnameType_GLITCH)
            {
                roomname->delay--;
                if (roomname->delay <= 0)
                {
                    roomname->progress = (roomname->progress + 1) % 2;
                    roomname->delay = 5;
                    if (roomname->progress == 0)
                    {
                        roomname->delay = 25 + (int) (fRandom() * 10);
                    }
                }
                setroomname(roomname->text[roomname->progress].c_str());
            }
            if (roomname->type == RoomnameType_TRANSFORM)
            {
                roomname->delay--;
                if (roomname->delay <= 0)
                {
                    roomname->progress++;
                    roomname->delay = 2;
                    if ((size_t) roomname->progress >= roomname->text.size())
                    {
                        roomname->progress = roomname->loop ? 0 : roomname->text.size() - 1;
                    }
                }
                setroomname(roomname->text[roomname->progress].c_str());
            }
            break;
        }
    }
}

void mapclass::initmapdata(void)
{
    if (custommode)
    {
        initcustommapdata();
        return;
    }

    teleporters.clear();
    shinytrinkets.clear();

    //Set up static map information like teleporters and shiny trinkets.
    setteleporter(0, 0);
    setteleporter(0, 16);
    setteleporter(2, 4);
    setteleporter(2, 11);
    setteleporter(7, 9);
    setteleporter(7, 15);
    setteleporter(8, 11);
    setteleporter(10, 5);
    setteleporter(11, 4);
    setteleporter(13, 2);
    setteleporter(13, 8);
    setteleporter(14, 19);
    setteleporter(15, 0);
    setteleporter(17, 12);
    setteleporter(17, 17);
    setteleporter(18, 1);
    setteleporter(18, 7);

    settrinket(14, 4);
    settrinket(13, 6);
    settrinket(11, 12);
    settrinket(15, 12);
    settrinket(14, 11);
    settrinket(18, 14);
    settrinket(11, 7);
    settrinket(9, 2);
    settrinket(9, 16);
    settrinket(2, 18);
    settrinket(7, 18);
    settrinket(6, 1);
    settrinket(17, 3);
    settrinket(10, 19);
    settrinket(5, 15);
    settrinket(1, 10);
    settrinket(3, 2);
    settrinket(10, 8);

    //Special room names
    specialroomnames.clear();

    {
        static const char* lines[] = {
            "Television Newsvel",
            "TelevisvonvNewsvel",
            "TvlvvvsvonvNevsvel",
            "vvvvvvsvovvNe svel",
            "vhv vvv'vvovv vevl",
            "vhv V v'Cvovv vewv",
            "vhe 9 v'Cvovv vewv",
            "vhe 9 v'Cvovv Newv",
            "The 9 O'Cvovk Newv",
            "The 9 O'Clock News"
        };

        roomnamechange(45, 51, lines, SDL_arraysize(lines));
    }

    {
        static const char* lines[] = {
            "Vwitvhed",
            "vVwivcvedv",
            "vvvwMvcvMdvv",
            "DvvvwMvfvvMdvvv",
            "Dvav Mvfvr Mdvvvv",
            "Diav M for Mdrver",
            "Dial M for Murder"
        };

        roomnamechange(46, 51, lines, SDL_arraysize(lines));
    }

    {
        static const char* lines[] = {
            "Gvnsmove",
            "Gvnvmovevv",
            "Gunvmove1vv6",
            "Vunsmoke 19v6",
            "Gunsmoke 1966"
        };

        roomnamechange(47, 51, lines, SDL_arraysize(lines));
    }

    {
        static const char* lines[] = {
            "Please envoy theve repeats",
            "Plse envoy tse rvpvas",
            "Plse envoy tse rvpvas",
            "Vl envoy te rvevs",
            "Vv evo tv vevs",
            "Iv vhv Mvrvivs",
            "In the Margins"
        };

        roomnamechange(50, 53, lines, SDL_arraysize(lines));
    }

    {
        static const char* lines[] = {
            "Try Viggling the Antenna",
            "TryJivglvng theAvtevna",
            "Tvvivglvng thAvtvvv",
            "Vvvgglvnv tvnvva",
            "Vvavvnvs vvtv",
            "Veavvn's Gvte",
            "Heaven's Gate"
        };

        roomnamechange(50, 54, lines, SDL_arraysize(lines));
    }

    roomnameglitch(42, 51, "Rear Window", "Rear Vindow");
    roomnameglitch(48, 51, "On the Waterfront", "On the Vaterfront");
    roomnameglitch(49, 51, "The Untouchables", "The Untouchavles");
}

void mapclass::roomnameglitch(int x, int y, const char* name, const char* text)
{
    Roomname roomname;
    roomname.x = x;
    roomname.y = y;
    roomname.type = RoomnameType_GLITCH;
    roomname.flag = -1;
    roomname.loop = false;
    roomname.progress = 1;
    roomname.delay = -1;

    roomname.text.push_back(name);
    roomname.text.push_back(text);

    specialroomnames.push_back(roomname);
}

void mapclass::roomnamechange(const int x, const int y, const char** lines, const size_t size)
{
    Roomname roomname;
    roomname.x = x;
    roomname.y = y;
    roomname.type = RoomnameType_TRANSFORM;
    roomname.flag = 72; // Flag 72 is synced with finalstretch
    roomname.loop = false;
    roomname.progress = 0;
    roomname.delay = 2;

    roomname.text.insert(roomname.text.end(), lines, lines + size);

    specialroomnames.push_back(roomname);
}

void mapclass::initcustommapdata(void)
{
    shinytrinkets.clear();

#if !defined(NO_CUSTOM_LEVELS)
    for (size_t i = 0; i < customentities.size(); i++)
    {
        const CustomEntity& ent = customentities[i];
        if (ent.t != 9)
        {
            continue;
        }

        settrinket(ent.rx, ent.ry);
    }
#endif
}

int mapclass::finalat(int x, int y)
{
    //return the tile index of the final stretch tiles offset by the colour difference
    const int tile = contents[TILE_IDX(x, y)];
    if (tile == 740)
    {
        //Special case: animated tiles
        if (final_mapcol == 1)
        {
            int offset;
            if (game.noflashingmode)
            {
                offset = 0;
            }
            else
            {
                offset = int(fRandom() * 11) * 40;
            }
            return 737 + offset;
        }
        else
        {
            int offset;
            if (game.noflashingmode)
            {
                offset = 0;
            }
            else
            {
                offset = final_aniframe * 40;
            }
            return tile - (final_mapcol * 3) + offset;
        }
    }
    else if (tile >= 80)
    {
        return tile - (final_mapcol * 3);
    }
    else
    {
        return tile;
    }
}

int mapclass::maptiletoenemycol(int t)
{
    //returns the colour index for enemies that matches the map colour t
    switch(t)
    {
    case 0:
        return 11;
        break;
    case 1:
        return 6;
        break;
    case 2:
        return 8;
        break;
    case 3:
        return 12;
        break;
    case 4:
        return 9;
        break;
    case 5:
        return 7;
        break;
    case 6:
        return 18;
        break;
    }
    return 11;
}

void mapclass::changefinalcol(int t)
{
    //change the map to colour t - for the game's final stretch.
    //First up, the tiles. This is just a setting:
    final_mapcol = t;
    const int temp = 6 - t;
    //Next, entities
    for (size_t i = 0; i < obj.entities.size(); i++)
    {
        if (obj.entities[i].type == 1) //something with a movement behavior
        {
            if (obj.entities[i].animate == 10 || obj.entities[i].animate == 11) //treadmill
            {
                if(temp<3)
                {
                    obj.entities[i].tile = 907 + (temp * 80);
                }
                else
                {
                    obj.entities[i].tile = 911 + ((temp-3) * 80);
                }
                if(obj.entities[i].animate == 10)    obj.entities[i].tile += 40;
            }
            else if (obj.entities[i].isplatform)
            {
                obj.entities[i].tile = 915+(temp*40);
            }
            else    //just an enemy
            {
                obj.entities[i].colour = maptiletoenemycol(temp);
            }
        }
        else if (obj.entities[i].type == 2)    //disappearing platforms
        {
            obj.entities[i].tile = 915+(temp*40);
        }
    }
}

void mapclass::setcol(TowerBG& bg_obj, const int r1, const int g1, const int b1 , const int r2, const int g2, const int b2, const int c)
{
    bg_obj.r = intpol(r1, r2, c / 5);
    bg_obj.g = intpol(g1, g2, c / 5);
    bg_obj.b = intpol(b1, b2, c / 5);
}

void mapclass::updatebgobj(TowerBG& bg_obj)
{
    const int check = bg_obj.colstate % 5; //current state of phase
    const int cmode = (bg_obj.colstate - check) / 5; // current colour transition;

    switch(cmode)
    {
    case 0:
        setcol(bg_obj, 255, 93, 107, 255, 255, 93, check);
        break;
    case 1:
        setcol(bg_obj, 255, 255, 93, 159, 255, 93, check);
        break;
    case 2:
        setcol(bg_obj, 159, 255, 93, 93, 245, 255, check);
        break;
    case 3:
        setcol(bg_obj, 93, 245, 255, 177, 93, 255, check);
        break;
    case 4:
        setcol(bg_obj, 177, 93, 255, 255, 93, 255, check);
        break;
    case 5:
        setcol(bg_obj, 255, 93, 255, 255, 93, 107, check);
        break;
    }

    bg_obj.tdrawback = true;
}

void mapclass::setbgobjlerp(TowerBG& bg_obj)
{
    bg_obj.bypos = ypos / 2;
    bg_obj.bscroll = (ypos - oldypos) / 2;
}

void mapclass::updatetowerglow(TowerBG& bg_obj)
{
    if (colstatedelay <= 0 || colsuperstate > 0)
    {
        if (colsuperstate > 0) bg_obj.colstate--;
        bg_obj.colstate++;
        if (bg_obj.colstate >= 30) bg_obj.colstate = 0;

        const int check = bg_obj.colstate % 5;
        updatebgobj(bg_obj);

        if (check == 0)
        {
            colstatedelay = 45;
        }
        else
        {
            colstatedelay = 0;
        }
        if (colsuperstate > 0) colstatedelay = 0;
    }
    else
    {
        colstatedelay--;
    }
}

void mapclass::nexttowercolour(void)
{
    /* Prevent cycling title BG more than once per frame. */
    if (nexttowercolour_set)
    {
        return;
    }
    nexttowercolour_set = true;

    graphics.titlebg.colstate+=5;
    if (graphics.titlebg.colstate >= 30) graphics.titlebg.colstate = 0;

    updatebgobj(graphics.titlebg);
}

void mapclass::settowercolour(int t)
{
    graphics.titlebg.colstate=t*5;
    if (graphics.titlebg.colstate >= 30) graphics.titlebg.colstate = 0;

    updatebgobj(graphics.titlebg);
}

bool mapclass::towerspikecollide(int x, int y)
{
    if (tower.at(x,y,0)>= 6 && tower.at(x,y,0) <= 11) return true;
    return false;
}

bool mapclass::collide(int x, int y, const bool invincible)
{
    if (towermode)
    {
        if (tower.at(x, y, 0) >= 12 && tower.at(x, y, 0) <= 27) return true;
        if (invincible)
        {
            if (tower.at(x, y, 0) >= 6 && tower.at(x, y, 0) <= 11) return true;
        }
    }
    else if (tileset == 2)
    {
        int tile;
        if (y == -1) return collide(x, y + 1, invincible);
        if (y == 29+extrarow) return collide(x, y - 1, invincible);
        if (x == -1) return collide(x + 1, y, invincible);
        if (x == 40) return collide(x - 1, y, invincible);
        if (x < 0 || y < 0 || x >= 40 || y >= 29 + extrarow) return false;
        tile = contents[TILE_IDX(x, y)];
        if (tile >= 12 && tile <= 27) return true;
        if (invincible)
        {
            if (tile >= 6 && tile <= 11) return true;
        }
    }
    else
    {
        int tile;
        if (y == -1) return collide(x, y + 1, invincible);
        if (y == 29+extrarow) return collide(x, y - 1, invincible);
        if (x == -1) return collide(x + 1, y, invincible);
        if (x == 40) return collide(x - 1, y, invincible);
        if (x < 0 || y < 0 || x >= 40 || y >= 29+extrarow) return false;
        tile = contents[TILE_IDX(x, y)];
        if (tile == 1) return true;
        if (tileset==0 && tile == 59) return true;
        if (tile>= 80 && tile < 680) return true;
        if (tile == 740 && tileset==1) return true;
        if (invincible)
        {
            if (tile>= 6 && tile <= 9) return true;
            if (tile>= 49 && tile <= 50) return true;
            if (tileset == 1)
            {
                if (tile>= 49 && tile < 80) return true;
            }
        }
    }
    return false;
}

void mapclass::settile(int xp, int yp, int t)
{
    if (xp >= 0 && xp < 40 && yp >= 0 && yp < 29+extrarow)
    {
        contents[TILE_IDX(xp, yp)] = t;
    }
}


int mapclass::area(int _rx, int _ry)
{
    //THIS IS THE BUG
    if (finalmode)
    {
        return 6;
    }
    else
    {
        int lookup = (_rx - 100) + ((_ry - 100) * 20);
        if(_rx-100>=0 && _rx-100<20 && _ry-100>=0 && _ry-100<20){
            return areamap[lookup];
        }
        else
        {
            return 6;
        }
    }
}

bool mapclass::isexplored(const int rx, const int ry)
{
    const int roomnum = rx + ry*20;
    if (INBOUNDS_ARR(roomnum, explored))
    {
        return explored[roomnum];
    }
    return false;
}

void mapclass::setexplored(const int rx, const int ry, const bool status)
{
    const int roomnum = rx + ry*20;
    if (INBOUNDS_ARR(roomnum, explored))
    {
        explored[roomnum] = status;
    }
}

void mapclass::exploretower(void)
{
    for (int i = 0; i < 20; i++)
    {
        setexplored(9, i, true);
    }
}

void mapclass::hideship(void)
{
    //remove the ship from the explored areas
    setexplored(2, 10, false);
    setexplored(3, 10, false);
    setexplored(4, 10, false);
    setexplored(2, 11, false);
    setexplored(3, 11, false);
    setexplored(4, 11, false);
}

void mapclass::showship(void)
{
    //show the ship in the explored areas
    setexplored(2, 10, true);
    setexplored(3, 10, true);
    setexplored(4, 10, true);
    setexplored(2, 11, true);
    setexplored(3, 11, true);
    setexplored(4, 11, true);
}

void mapclass::resetplayer(void)
{
    resetplayer(false);
}

void mapclass::resetplayer(const bool player_died)
{
    bool was_in_tower = towermode;
    if (game.roomx != game.saverx || game.roomy != game.savery)
    {
        gotoroom(game.saverx, game.savery);
    }

    game.deathseq = -1;
    int i = obj.getplayer();
    if(INBOUNDS_VEC(i, obj.entities))
    {
        obj.entities[i].vx = 0;
        obj.entities[i].vy = 0;
        obj.entities[i].ax = 0;
        obj.entities[i].ay = 0;
        obj.entities[i].xp = game.savex;
        obj.entities[i].yp = game.savey;

        //Fix conveyor death loop glitch
        obj.entities[i].newxp = obj.entities[i].xp;
        obj.entities[i].newyp = obj.entities[i].yp;

        obj.entities[i].dir = game.savedir;
        obj.entities[i].colour = game.savecolour;
        if (player_died)
        {
            game.lifeseq = 10;
            obj.entities[i].invis = true;
        }
        else
        {
            obj.entities[i].invis = false;
        }
        if (!GlitchrunnerMode_less_than_or_equal(Glitchrunner2_2))
        {
            obj.entities[i].size = 0;
            obj.entities[i].cx = 6;
            obj.entities[i].cy = 2;
            obj.entities[i].w = 12;
            obj.entities[i].h = 21;
        }

        // If we entered a tower as part of respawn, reposition camera
        if (!was_in_tower && towermode)
        {
            ypos = obj.entities[i].yp - 120;
            if (ypos < 0)
            {
                ypos = 0;
            }
            oldypos = ypos;
            setbgobjlerp(graphics.towerbg);
        }
    }

    if (game.state == 0 && !script.running && game.completestop)
    {
        /* Looks like a collection dialogue was interrupted.
         * Undo its effects! */
        game.advancetext = false;
        graphics.showcutscenebars = false;
        if (music.currentsong > -1)
        {
            music.fadeMusicVolumeIn(3000);
        }
    }

    game.scmhurt = false; //Just in case the supercrewmate is fucking this up!
    if (game.supercrewmate)
    {
        if (game.roomx == game.scmprogress + 41)
        {
            game.scmprogress = game.roomx - 41;
        }
        else
        {
            game.scmprogress = game.roomx - 40;
        }
    }
}

void mapclass::warpto(int rx, int ry , int t, int tx, int ty)
{
    gotoroom(rx, ry);
    game.teleport = false;
    if (INBOUNDS_VEC(t, obj.entities))
    {
        obj.entities[t].xp = tx * 8;
        obj.entities[t].yp = (ty * 8) - obj.entities[t].h;
        obj.entities[t].lerpoldxp = obj.entities[t].xp;
        obj.entities[t].lerpoldyp = obj.entities[t].yp;
    }
    game.gravitycontrol = 0;
}

void mapclass::gotoroom(int rx, int ry)
{
    int roomchangedir;
    std::vector<entclass> linecrosskludge;

    //First, destroy the current room
    obj.removeallblocks();
    game.activetele = false;
    game.readytotele = 0;
    game.oldreadytotele = 0;

    //Ok, let's save the position of all lines on the screen
    for (size_t i = 0; i < obj.entities.size(); i++)
    {
        if (obj.entities[i].type == 9)
        {
            //It's a horizontal line
            if (obj.entities[i].xp <= 0 || (obj.entities[i].xp + obj.entities[i].w) >= 312)
            {
                //it's on a screen edge
                obj.copylinecross(linecrosskludge, i);
            }
        }
    }

    /* Disable all entities in the room, and deallocate any unnecessary entity slots. */
    /* However don't disable player entities, but do preserve holes between them (if any). */
    bool player_found = false;
    for (int i = obj.entities.size() - 1; i >= 0; --i)
    {
        /* Iterate in reverse order to prevent unnecessary indice shifting */
        if (obj.entities[i].rule == 0)
        {
            player_found = true;
            continue;
        }

        if (!player_found)
        {
            obj.entities.erase(obj.entities.begin() + i);
        }
        else
        {
            obj.disableentity(i);
        }
    }

    if (rx < game.roomx)
    {
        roomchangedir = 0;
    }
    else
    {
        roomchangedir = 1;
    }

    if (finalmode)
    {
        //Ok, what way are we moving?
        game.roomx = rx;
        game.roomy = ry;

        if (game.roomy < 10)
        {
            game.roomy = 11;
        }

        if(game.roomx>=41 && game.roomy>=48 && game.roomx<61 && game.roomy<68 )
        {
            game.currentroomdeaths = roomdeathsfinal[game.roomx - 41 + (20 * (game.roomy - 48))];
        }
        else
        {
            game.currentroomdeaths = 0;
        }

        //Final level for time trial
        if (game.intimetrial && game.roomx == 46 && game.roomy == 54)
        {
            music.niceplay(Music_PREDESTINEDFATEREMIX);
        }
    }
#if !defined(NO_CUSTOM_LEVELS)
    else if (custommode)
    {
        game.roomx = rx;
        game.roomy = ry;
        if (game.roomx < 100) game.roomx = 100 + cl.mapwidth-1;
        if (game.roomy < 100) game.roomy = 100 + cl.mapheight-1;
        if (game.roomx > 100 + cl.mapwidth-1) game.roomx = 100;
        if (game.roomy > 100 + cl.mapheight-1) game.roomy = 100;
    }
#endif
    else
    {
        game.roomx = rx;
        game.roomy = ry;
        if (game.roomx < 100) game.roomx = 119;
        if (game.roomy < 100) game.roomy = 119;
        if (game.roomx > 119) game.roomx = 100;
        if (game.roomy > 119) game.roomy = 100;

        game.currentroomdeaths = roomdeaths[game.roomx - 100 + (20 * (game.roomy - 100))];

        //Alright, change music depending on where we are:
        music.changemusicarea(game.roomx - 100, game.roomy - 100);
    }
    loadlevel(game.roomx, game.roomy);


    //Do we need to reload the background?
    bool redrawbg = game.roomx != game.prevroomx || game.roomy != game.prevroomy;

    if (redrawbg)
    {
        graphics.backgrounddrawn = false; //Used for background caching speedup
    }
    graphics.foregrounddrawn = false; //Used for background caching speedup

    game.prevroomx = game.roomx;
    game.prevroomy = game.roomy;

    //a very special case: if entering the communication room, room 13,4 before tag 5 is set, set the game state to a background
    //textbox thingy. if tag five is not set when changing room, reset the game state. (tag 5 is set when you get back to the ship)
    if(!game.intimetrial && !custommode)
    {
        if (!obj.flags[5] && !finalmode)
        {
            game.setstate(0);
            if (game.roomx == 113 && game.roomy == 104)
            {
                game.setstate(50);
            }
        }
    }

    //Ok, kludge to fix lines in crossing rooms - if we're intersecting a gravity line right now, let's
    //set it to an inactive state.

    //Alright! So, let's look at our lines from the previous rooms, and determine if any of them are actually
    //continuations!

    const int player_idx = obj.getplayer();
    if (INBOUNDS_VEC(player_idx, obj.entities))
    {
        entclass* player = &obj.entities[player_idx];
        player->oldxp = player->xp;
        player->oldyp = player->yp;
        player->lerpoldxp = player->xp - int(player->vx);
        player->lerpoldyp = player->yp - int(player->vy);
    }

    for (size_t i = 0; i < obj.entities.size(); i++)
    {
        if (obj.entities[i].type == 9)
        {
            //It's a horizontal line
            if (obj.entities[i].xp <= 0 || obj.entities[i].xp + obj.entities[i].w >= 312)
            {
                //it's on a screen edge
                for (size_t j = 0; j < linecrosskludge.size(); j++)
                {
                    if (obj.entities[i].yp == linecrosskludge[j].yp)
                    {
                        //y's match, how about x's?
                        //we're moving left:
                        if (roomchangedir == 0)
                        {
                            if (obj.entities[i].xp + obj.entities[i].w >= 312 && linecrosskludge[j].xp <= 0)
                            {
                                obj.revertlinecross(linecrosskludge, i, j);
                            }
                        }
                        else
                        {
                            if (obj.entities[i].xp <= 0 && linecrosskludge[j].xp + linecrosskludge[j].w >= 312)
                            {
                                obj.revertlinecross(linecrosskludge, i, j);
                            }
                        }
                    }
                }
            }
        }
    }

    if (game.companion > 0)
    {
        //We've changed room? Let's bring our companion along!
        spawncompanion();
    }
}

void mapclass::spawncompanion(void)
{
    int i = obj.getplayer();
    if (!INBOUNDS_VEC(i, obj.entities))
    {
        return;
    }

    //ok, we'll presume our companion has been destroyed in the room change. So:
    switch(game.companion)
    {
    case 6:
    {
        obj.createentity(obj.entities[i].xp, 121.0f, 15.0f,1);  //Y=121, the floor in that particular place!
        int j = obj.getcompanion();
        if (INBOUNDS_VEC(j, obj.entities))
        {
            obj.entities[j].vx = obj.entities[i].vx;
            obj.entities[j].dir = obj.entities[i].dir;
        }
        break;
    }
    case 7:
        if (game.roomy <= 105)   //don't jump after him!
        {
            if (game.roomx == 110)
            {
                obj.createentity(320, 86, 16, 1);  //Y=86, the ROOF in that particular place!
            }
            else
            {
                obj.createentity(obj.entities[i].xp, 86.0f, 16.0f, 1);  //Y=86, the ROOF in that particular place!
            }
            int j = obj.getcompanion();
            if (INBOUNDS_VEC(j, obj.entities))
            {
                obj.entities[j].vx = obj.entities[i].vx;
                obj.entities[j].dir = obj.entities[i].dir;
            }
        }
        break;
    case 8:
        if (game.roomy >= 104)   //don't jump after him!
        {
            if (game.roomx == 102)
            {
                obj.createentity(310, 177, 17, 1);
                int j = obj.getcompanion();
                if (INBOUNDS_VEC(j, obj.entities))
                {
                    obj.entities[j].vx = obj.entities[i].vx;
                    obj.entities[j].dir = obj.entities[i].dir;
                }
            }
            else
            {
                obj.createentity(obj.entities[i].xp, 177.0f, 17.0f, 1);
                int j = obj.getcompanion();
                if (INBOUNDS_VEC(j, obj.entities))
                {
                    obj.entities[j].vx = obj.entities[i].vx;
                    obj.entities[j].dir = obj.entities[i].dir;
                }
            }
        }
        break;
    case 9:
        if (!towermode)   //don't go back into the tower!
        {
            if (game.roomx == 110 && obj.entities[i].xp<20)
            {
                obj.createentity(100, 185, 18, 15, 0, 1);
            }
            else
            {
                obj.createentity(obj.entities[i].xp, 185.0f, 18.0f, 15, 0, 1);
            }
            int j = obj.getcompanion();
            if (INBOUNDS_VEC(j, obj.entities))
            {
                obj.entities[j].vx = obj.entities[i].vx;
                obj.entities[j].dir = obj.entities[i].dir;
            }
        }
        break;
    case 10:
        //intermission 2, choose colour based on lastsaved
        if (game.roomy == 51)
        {
            if (!obj.flags[59])
            {
                obj.createentity(225.0f, 169.0f, 18, graphics.crewcolour(game.lastsaved), 0, 10);
                int j = obj.getcompanion();
                if (INBOUNDS_VEC(j, obj.entities))
                {
                    obj.entities[j].vx = obj.entities[i].vx;
                    obj.entities[j].dir = obj.entities[i].dir;
                }
            }
        }
        else    if (game.roomy >= 52)
        {
            if (obj.flags[59])
            {
                obj.createentity(160.0f, 177.0f, 18, graphics.crewcolour(game.lastsaved), 0, 18, 1);
                int j = obj.getcompanion();
                if (INBOUNDS_VEC(j, obj.entities))
                {
                    obj.entities[j].vx = obj.entities[i].vx;
                    obj.entities[j].dir = obj.entities[i].dir;
                }
            }
            else
            {
                obj.flags[59] = true;
                obj.createentity(obj.entities[i].xp, -20.0f, 18.0f, graphics.crewcolour(game.lastsaved), 0, 10, 0);
                int j = obj.getcompanion();
                if (INBOUNDS_VEC(j, obj.entities))
                {
                    obj.entities[j].vx = obj.entities[i].vx;
                    obj.entities[j].dir = obj.entities[i].dir;
                }
            }
        }
        break;
    case 11:
        //Intermission 1: We're using the SuperCrewMate instead!
        if(game.roomx-41==game.scmprogress)
        {
            switch(game.scmprogress)
            {
            case 0:
                obj.createentity(76, 161, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 1:
                obj.createentity(10, 169, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 2:
                obj.createentity(10, 177, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 3:
                obj.createentity(10, 177, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 4:
                obj.createentity(10, 185, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 5:
                obj.createentity(10, 185, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 6:
                obj.createentity(10, 185, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 7:
                obj.createentity(10, 41, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 8:
                obj.createentity(10, 169, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 9:
                obj.createentity(10, 169, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 10:
                obj.createentity(10, 129, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 11:
                obj.createentity(10, 129, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 12:
                obj.createentity(10, 65, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 13:
                obj.createentity(10, 177, 24, graphics.crewcolour(game.lastsaved));
                break;
            }
        }
        break;
    }
}

const char* mapclass::currentarea(int t)
{
    switch(t)
    {
    case 0:
        return loc::gettext_roomname_special("Dimension VVVVVV");
        break;
    case 1:
        return loc::gettext_roomname_special("Dimension VVVVVV");
        break;
    case 2:
        return loc::gettext_roomname_special("Laboratory");
        break;
    case 3:
        return loc::gettext_roomname_special("The Tower");
        break;
    case 4:
        return loc::gettext_roomname_special("Warp Zone");
        break;
    case 5:
        return loc::gettext_roomname_special("Space Station");
        break;
    case 6:
        return loc::gettext_roomname_special("Outside Dimension VVVVVV");
        break;
    case 7:
        return loc::gettext_roomname_special("Outside Dimension VVVVVV");
        break;
    case 8:
        return loc::gettext_roomname_special("Outside Dimension VVVVVV");
        break;
    case 9:
        return loc::gettext_roomname_special("Outside Dimension VVVVVV");
        break;
    case 10:
        return loc::gettext_roomname_special("Outside Dimension VVVVVV");
        break;
    case 11:
        return loc::gettext_roomname_special("The Tower");
        break;
    }
    return loc::gettext_roomname_special("???");
}

static void copy_short_to_int(int* dest, const short* src, const size_t size)
{
    size_t i;
    for (i = 0; i < size; ++i)
    {
        dest[i] = src[i];
    }
}

void mapclass::loadlevel(int rx, int ry)
{
    int t;
    if (!finalmode)
    {
        setexplored(rx - 100, ry - 100, true);
        if (rx == 109 && !custommode)
        {
            exploretower();
        }
    }


    roomtexton = false;
    roomtext.clear();
    roomnameset = false;

    obj.platformtile = 0;
    obj.customplatformtile=0;
    obj.vertplatforms = false;
    obj.horplatforms = false;
    setroomname("");
    hiddenname = "";
    background = 1;
    warpx = false;
    warpy = false;

    towermode = false;
    ypos = 0;
    oldypos = 0;
    extrarow = 0;
    spikeleveltop = 0;
    spikelevelbottom = 0;
    oldspikeleveltop = 0;
    oldspikelevelbottom = 0;

    //Custom stuff for warplines
    obj.customwarpmode=false;
    obj.customwarpmodevon=false;
    obj.customwarpmodehon=false;

    if (finalmode)
    {
        t = 6;
        //check if we're in the towers
        if (rx == 49 && ry == 52)
        {
            //entered tower 1
            t = 7;
        }
        else if (rx == 49 && ry == 53)
        {
            //re entered tower 1
            t = 8;
        }
        else if (rx == 51 && ry == 54)
        {
            //entered tower 2
            t = 9;
        }
        else if (rx == 51 && ry == 53)
        {
            //re entered tower 2
            t = 10;
        }
    }
    else if (custommode)
    {
        t= 12;
    }
    else
    {
        t = area(rx, ry);

        if (t == 3)
        {
            //correct position for tower
            if (ry == 109)
            {
                //entered from ground floor
                int player = obj.getplayer();
                if (INBOUNDS_VEC(player, obj.entities))
                {
                    obj.entities[player].yp += (671 * 8);
                }

                ypos = (700-29) * 8;
                oldypos = ypos;
                setbgobjlerp(graphics.towerbg);
                cameramode = 0;
                graphics.towerbg.colstate = 0;
                colsuperstate = 0;
            }
            else if (ry == 104)
            {
                //you've entered from the top floor
                ypos = 0;
                oldypos = ypos;
                setbgobjlerp(graphics.towerbg);
                cameramode = 0;
                graphics.towerbg.colstate = 0;
                colsuperstate = 0;
            }
        }

        if (t < 2) //on the world map, want to test if we're in the secret lab
        {
            if (rx >= 116)
            {
                if (ry >= 105)
                {
                    if (ry <= 107)
                    {
                        if (rx == 119 && ry == 105)
                        {
                            //Ah, this is just a normal area
                        }
                        else
                        {
                            //in the secret lab! Crazy background!
                            background = 2;
                            if (rx == 116 && ry == 105) graphics.rcol = 1;
                            if (rx == 117 && ry == 105) graphics.rcol = 5;
                            if (rx == 118 && ry == 105) graphics.rcol = 4;
                            if (rx == 117 && ry == 106) graphics.rcol = 2;
                            if (rx == 118 && ry == 106) graphics.rcol = 0;
                            if (rx == 119 && ry == 106) graphics.rcol = 3;
                            if (rx == 119 && ry == 107) graphics.rcol = 1;
                        }
                    }
                }
            }
        }
    }

    if (rx == 119 && ry == 108 && !custommode)
    {
        background = 5;
        graphics.rcol = 3;
        warpx = true;
        warpy = true;
    }

    roomname_special = false;

    switch(t)
    {
#ifdef MAKEANDPLAY
        UNUSED(copy_short_to_int);
#else
    case 0:
    case 1: //World Map
    {
        tileset = 1;
        extrarow = 1;
        const short* tmap = otherlevel.loadlevel(rx, ry);
        copy_short_to_int(contents, tmap, SDL_arraysize(contents));
        setroomname(otherlevel.roomname);
        roomname_special = otherlevel.roomname_special;
        hiddenname = otherlevel.hiddenname;
        tileset = otherlevel.roomtileset;
        break;
    }
    case 2: //The Lab
    {
        const short* tmap = lablevel.loadlevel(rx, ry);
        copy_short_to_int(contents, tmap, SDL_arraysize(contents));
        setroomname(lablevel.roomname);
        roomname_special = lablevel.roomname_special;
        tileset = 1;
        background = 2;
        graphics.rcol = lablevel.rcol;
        break;
    }
    case 3: //The Tower
        graphics.towerbg.tdrawback = true;
        minitowermode = false;
        tower.minitowermode = false;
        graphics.towerbg.scrolldir = 0;
        setbgobjlerp(graphics.towerbg);

        setroomname("The Tower");
        tileset = 1;
        background = 3;
        towermode = true;

        //All the entities for here are just loaded here; it's essentially one room after all


        obj.createentity(48, 5456, 10, 1, 505007); // (savepoint)
        obj.createentity(224, 4528, 10, 1, 505017); // (savepoint)
        obj.createentity(232, 4168, 10, 0, 505027); // (savepoint)
        obj.createentity(280, 3816, 10, 1, 505037); // (savepoint)
        obj.createentity(152, 3552, 10, 1, 505047); // (savepoint)
        obj.createentity(216, 3280, 10, 0, 505057); // (savepoint)
        obj.createentity(216, 4808, 10, 1, 505067); // (savepoint)
        obj.createentity(72, 3096, 10, 0, 505077); // (savepoint)
        obj.createentity(176, 2600, 10, 0, 505087); // (savepoint)
        obj.createentity(216, 2392, 10, 0, 505097); // (savepoint)
        obj.createentity(152, 1184, 10, 1, 505107); // (savepoint)
        obj.createentity(152, 912, 10, 1, 505117); // (savepoint)
        obj.createentity(152, 536, 10, 1, 505127); // (savepoint)
        obj.createentity(120, 5136, 10, 0, 505137); // (savepoint)
        obj.createentity(144, 1824, 10, 0, 505147); // (savepoint)
        obj.createentity(72, 2904, 10, 0, 505157); // (savepoint)
        obj.createentity(224, 1648, 10, 1, 505167); // (savepoint)
        obj.createentity(112, 5280, 10, 1, 50517); // (savepoint)

        obj.createentity(24, 4216, 9, 7); // (shiny trinket)
        obj.createentity(280, 3216, 9, 8); // (shiny trinket)
        break;
    case 4: //The Warpzone
    {
        const short* tmap = warplevel.loadlevel(rx, ry);
        copy_short_to_int(contents, tmap, SDL_arraysize(contents));
        setroomname(warplevel.roomname);
        roomname_special = warplevel.roomname_special;
        tileset = 1;
        background = 3;
        graphics.rcol = warplevel.rcol;
        graphics.backgrounddrawn = false;

        warpx = warplevel.warpx;
        warpy = warplevel.warpy;
        background = 5;
        if (warpy) background = 4;
        if (warpx) background = 3;
        if (warpx && warpy) background = 5;
        break;
    }
    case 5: //Space station
    {
        const short* tmap = spacestation2.loadlevel(rx, ry);
        copy_short_to_int(contents, tmap, SDL_arraysize(contents));
        setroomname(spacestation2.roomname);
        roomname_special = spacestation2.roomname_special;
        tileset = 0;
        break;
    }
    case 6: //final level
    {
        const short* tmap = finallevel.loadlevel(rx, ry);
        copy_short_to_int(contents, tmap, SDL_arraysize(contents));
        setroomname(finallevel.roomname);
        roomname_special = finallevel.roomname_special;
        tileset = 1;
        background = 3;
        graphics.backgrounddrawn = false;

        if (finalstretch)
        {
            background = 6;
        }
        else
        {
            warpx = finallevel.warpx;
            warpy = finallevel.warpy;
            background = 5;
            if (warpy) background = 4;
            if (warpx) background = 3;
            if (warpx && warpy) background = 5;
        }

        graphics.rcol = 6;
        changefinalcol(final_mapcol);
        break;
    }
    case 7: //Final Level, Tower 1
        graphics.towerbg.tdrawback = true;
        minitowermode = true;
        tower.minitowermode = true;
        graphics.towerbg.scrolldir = 1;
        setbgobjlerp(graphics.towerbg);

        setroomname("Panic Room");
        tileset = 1;
        background = 3;
        towermode = true;

        tower.loadminitower1();

        ypos = 0;
        oldypos = 0;
        setbgobjlerp(graphics.towerbg);
        cameramode = 0;
        graphics.towerbg.colstate = 0;
        colsuperstate = 0;
        break;
    case 8: //Final Level, Tower 1 (reentered from below)
    {
        graphics.towerbg.tdrawback = true;
        minitowermode = true;
        tower.minitowermode = true;
        graphics.towerbg.scrolldir = 1;
        setbgobjlerp(graphics.towerbg);

        setroomname("Panic Room");
        tileset = 1;
        background = 3;
        towermode = true;

        tower.loadminitower1();

        int i = obj.getplayer();
        if (INBOUNDS_VEC(i, obj.entities))
        {
            obj.entities[i].yp += (71 * 8);
        }
        game.roomy--;

        ypos = (100-29) * 8;
        oldypos = ypos;
        setbgobjlerp(graphics.towerbg);
        cameramode = 0;
        graphics.towerbg.colstate = 0;
        colsuperstate = 0;}
        break;
    case 9: //Final Level, Tower 2
    {
        graphics.towerbg.tdrawback = true;
        minitowermode = true;
        tower.minitowermode = true;
        graphics.towerbg.scrolldir = 0;
        setbgobjlerp(graphics.towerbg);

        setroomname("The Final Challenge");
        tileset = 1;
        background = 3;
        towermode = true;

        tower.loadminitower2();

        obj.createentity(56, 556, 11, 136); // (horizontal gravity line)
        obj.createentity(184, 592, 10, 0, 50500); // (savepoint)
        obj.createentity(184, 644, 11, 88); // (horizontal gravity line)
        obj.createentity(56, 460, 11, 136); // (horizontal gravity line)
        obj.createentity(216, 440, 10, 0, 50501); // (savepoint)
        obj.createentity(104, 508, 11, 168); // (horizontal gravity line)
        obj.createentity(219, 264, 12, 56); // (vertical gravity line)
        obj.createentity(120, 332, 11, 96); // (horizontal gravity line)
        obj.createentity(219, 344, 12, 56); // (vertical gravity line)
        obj.createentity(224, 332, 11, 48); // (horizontal gravity line)
        obj.createentity(56, 212, 11, 144); // (horizontal gravity line)
        obj.createentity(32, 20, 11, 96); // (horizontal gravity line)
        obj.createentity(72, 156, 11, 200); // (horizontal gravity line)

        int i = obj.getplayer();
        if (INBOUNDS_VEC(i, obj.entities))
        {
            obj.entities[i].yp += (71 * 8);
        }
        game.roomy--;

        ypos = (100-29) * 8;
        oldypos = ypos;
        setbgobjlerp(graphics.towerbg);
        cameramode = 0;
        graphics.towerbg.colstate = 0;
        colsuperstate = 0;
        break;
    }
    case 10: //Final Level, Tower 2
    {

        graphics.towerbg.tdrawback = true;
        minitowermode = true;
        tower.minitowermode = true;
        graphics.towerbg.scrolldir = 0;
        setbgobjlerp(graphics.towerbg);

        setroomname("The Final Challenge");
        tileset = 1;
        background = 3;
        towermode = true;

        tower.loadminitower2();

        obj.createentity(56, 556, 11, 136); // (horizontal gravity line)
        obj.createentity(184, 592, 10, 0, 50500); // (savepoint)
        obj.createentity(184, 644, 11, 88); // (horizontal gravity line)
        obj.createentity(56, 460, 11, 136); // (horizontal gravity line)
        obj.createentity(216, 440, 10, 0, 50501); // (savepoint)
        obj.createentity(104, 508, 11, 168); // (horizontal gravity line)
        obj.createentity(219, 264, 12, 56); // (vertical gravity line)
        obj.createentity(120, 332, 11, 96); // (horizontal gravity line)
        obj.createentity(219, 344, 12, 56); // (vertical gravity line)
        obj.createentity(224, 332, 11, 48); // (horizontal gravity line)
        obj.createentity(56, 212, 11, 144); // (horizontal gravity line)
        obj.createentity(32, 20, 11, 96); // (horizontal gravity line)
        obj.createentity(72, 156, 11, 200); // (horizontal gravity line)

        ypos = 0;
        oldypos = 0;
        setbgobjlerp(graphics.towerbg);
        cameramode = 0;
        graphics.towerbg.colstate = 0;
        colsuperstate = 0;
        break;
    }
    case 11: //Tower Hallways //Content is held in final level routine
    {
        const short* tmap = finallevel.loadlevel(rx, ry);
        copy_short_to_int(contents, tmap, SDL_arraysize(contents));
        setroomname(finallevel.roomname);
        roomname_special = finallevel.roomname_special;
        tileset = 2;
        if (rx == 108)
        {
            background = 7;
            rcol = 15;
        }
        if (rx == 110)
        {
            background = 8;
            rcol = 10;
        }
        if (rx == 111)
        {
            background = 9;
            rcol = 0;
        }
        break;
    }
#endif
#if !defined(NO_CUSTOM_LEVELS)
    case 12: //Custom level
    {
        const RoomProperty* const room = cl.getroomprop(rx - 100, ry - 100);
        game.customcol = cl.getlevelcol(room->tileset, room->tilecol) + 1;
        obj.customplatformtile = game.customcol * 12;

        switch (room->tileset)
        {
        case 0: // Space Station
            tileset = 0;
            background = 1;
            break;
        case 1: // Outside
            tileset = 1;
            background = 1;
            break;
        case 2: // Lab
            tileset = 1;
            background = 2;
            graphics.rcol = room->tilecol;
            break;
        case 3: // Warp Zone/intermission
            tileset = 1;
            background = 6;
            break;
        case 4: // Ship
            tileset = 1;
            background = 1;
            break;
        default:
            tileset = 1;
            background = 1;
            break;
        }

        // If screen warping, then override all that:
        bool redrawbg = game.roomx != game.prevroomx || game.roomy != game.prevroomy;
        if (redrawbg)
        {
            graphics.backgrounddrawn = false;
        }

        switch (room->warpdir)
        {
        case 1:
            warpx = true;
            background = 3;
            graphics.rcol = cl.getwarpbackground(rx - 100, ry - 100);
            break;
        case 2:
            warpy = true;
            background = 4;
            graphics.rcol = cl.getwarpbackground(rx - 100, ry - 100);
            break;
        case 3:
            warpx = true;
            warpy = true;
            background = 5;
            graphics.rcol = cl.getwarpbackground(rx - 100, ry - 100);
            break;
        }

        setroomname(room->roomname.c_str());
        extrarow = 1;
        const int* tmap = cl.loadlevel(rx, ry);
        SDL_memcpy(contents, tmap, sizeof(contents));


        roomtexton = false;
        roomtext.clear();

        // Entities have to be created HERE, akwardly
        int tempcheckpoints = 0;
        int tempscriptbox = 0;
        for (size_t edi = 0; edi < customentities.size(); edi++)
        {
            // If entity is in this room, create it
            const CustomEntity& ent = customentities[edi];
            if (ent.rx != rx - 100 || ent.ry != ry - 100)
            {
                continue;
            }

            const int ex = ent.x * 8;
            const int ey = ent.y * 8;

            // Platform and enemy bounding boxes
            int bx1 = 0, by1 = 0, bx2 = 0, by2 = 0;

            bool enemy = ent.t == 1;
            bool moving_plat = ent.t == 2 && ent.p1 <= 4;
            if (enemy || moving_plat)
            {
                if (enemy)
                {
                    bx1 = room->enemyx1;
                    by1 = room->enemyy1;
                    bx2 = room->enemyx2;
                    by2 = room->enemyy2;
                }
                else if (moving_plat)
                {
                    bx1 = room->platx1;
                    by1 = room->platy1;
                    bx2 = room->platx2;
                    by2 = room->platy2;
                }

                // Enlarge bounding boxes to fix warping entities
                if (warpx && bx1 == 0 && bx2 == 320)
                {
                    bx1 -= 100;
                    bx2 += 100;
                }
                if (warpy && by1 == 0 && by2 == 240)
                {
                    by1 -= 100;
                    by2 += 100;
                }
            }

            switch (ent.t)
            {
            case 1: // Enemies
                obj.customenemy = room->enemytype;
                obj.createentity(ex, ey, 56, ent.p1, 4, bx1, by1, bx2, by2);
                break;
            case 2: // Platforms and conveyors
                if (ent.p1 <= 4)
                {
                    obj.createentity(ex, ey, 2, ent.p1, room->platv, bx1, by1, bx2, by2);
                }
                else if (ent.p1 >= 5 && ent.p1 <= 8) // Conveyor
                {
                    obj.createentity(ex, ey, 2, ent.p1 + 3, 4);
                }
                break;
            case 3: // Disappearing platforms
                obj.createentity(ex, ey, 3);
                break;
            case 8: // Coins
                obj.createentity(ex, ey, 8, cl.findcoin(edi));
                break;
            case 9: // Trinkets
                obj.createentity(ex, ey, 9, cl.findtrinket(edi));
                break;
            case 10: // Checkpoints
                obj.createentity(ex, ey, 10, ent.p1, (rx + ry*100) * 20 + tempcheckpoints);
                tempcheckpoints++;
                break;
            case 11: // Gravity Lines
                if (ent.p1 == 0) //Horizontal
                {
                    obj.createentity(ent.p2 * 8, ey + 4, 11, ent.p3);
                }
                else //Vertical
                {
                    obj.createentity(ex + 3, ent.p2 * 8, 12, ent.p3);
                }
                break;
            case 13: // Warp Tokens
                obj.createentity(ex, ey, 13, ent.p1, ent.p2);
                break;
            case 15: // Collectable crewmate
                obj.createentity(ex - 4, ey + 1, 55, cl.findcrewmate(edi), ent.p1, ent.p2);
                break;
            case 17: // Roomtext!
            {
                roomtexton = true;
                Roomtext text;
                text.x = ex / 8;
                text.y = ey / 8;
                text.text = ent.scriptname.c_str();
                roomtext.push_back(text);
                break;
            }
            case 18: // Terminals
            {
                obj.customscript = ent.scriptname;

                int usethistile = ent.p1;
                int usethisy = ey;

                // This isn't a boolean: we just swap 0 and 1 around and leave the rest alone
                if (usethistile == 0)
                {
                    usethistile = 1; // Unflipped
                }
                else if (usethistile == 1)
                {
                    usethistile = 0; // Flipped;
                    usethisy -= 8;
                }

                obj.createentity(ex, usethisy + 8, 20 + SDL_clamp(ent.p2, 0, 1), usethistile);

                for (size_t i = 0; i < script.customscripts.size(); i++)
                {
                    if (script.customscripts[i].name == obj.customscript)
                    {
                        obj.createblock(ACTIVITY, ex - 8, usethisy + 8, 20, 16, 35);
                        break;
                    }
                }
                break;
            }
            case 19: //Script Box
                if (INBOUNDS_ARR(tempscriptbox, game.customscript))
                {
                    game.customscript[tempscriptbox] = ent.scriptname;
                }
                obj.createblock(TRIGGER, ex, ey, ent.p1 * 8, ent.p2 * 8, 300 + tempscriptbox, "custom_" + ent.scriptname);
                tempscriptbox++;
                break;
            case 50: // Warp Lines
                obj.customwarpmode=true;
                switch (ent.p1)
                {
                case 0: // Vertical, left
                    obj.createentity(ex + 4, ent.p2 * 8, 51, ent.p3);
                    break;
                case 1: //Horizontal, right
                    obj.createentity(ex + 4, ent.p2 * 8, 52, ent.p3);
                    break;
                case 2: //Vertical, top
                    obj.createentity(ent.p2 * 8, ey + 7, 53, ent.p3);
                    break;
                case 3: // Horizontal, bottom
                    obj.createentity(ent.p2 * 8, ey, 54, ent.p3);
                    break;
                }
                break;
            }
        }

        //do the appear/remove roomname here
        break;
    }
#endif
    }
    //The room's loaded: now we fill out damage blocks based on the tiles.
    if (towermode)
    {

    }
    else
    {
        for (int j = 0; j < 29 + extrarow; j++)
        {
            for (int i = 0; i < 40; i++)
            {
                int tile = contents[TILE_IDX(i, j)];
                //Damage blocks
                if(tileset==0)
                {
                    if (tile == 6 || tile == 8)
                    {
                        //sticking up
                        obj.createblock(2, (i * 8), (j * 8)+4, 8, 4);
                    }
                    if (tile == 7 || tile == 9)
                    {
                        //Sticking down
                        obj.createblock(2, (i * 8), (j * 8), 8, 4);
                    }
                    if (tile == 49 || tile == 50)
                    {
                        //left or right
                        obj.createblock(2, (i * 8), (j * 8)+3, 8, 2);
                    }
                }
                else if(tileset==1)
                {
                    if ((tile >= 63 && tile <= 74) ||
                            (tile >= 6 && tile <= 9))
                    {
                        //sticking up) {
                        if (tile < 10) tile++;
                        //sticking up
                        if(tile%2==0)
                        {
                            obj.createblock(2, (i * 8), (j * 8), 8, 4);
                        }
                        else
                        {
                            //Sticking down
                            obj.createblock(2, (i * 8), (j * 8) + 4, 8, 4);
                        }
                        if (tile < 11) tile--;
                    }
                    if (tile >= 49 && tile <= 62)
                    {
                        //left or right
                        obj.createblock(2, (i * 8), (j * 8)+3, 8, 2);
                    }
                }
                else if(tileset==2)
                {
                    if (tile == 6 || tile == 8)
                    {
                        //sticking up
                        obj.createblock(2, (i * 8), (j * 8)+4, 8, 4);
                    }
                    if (tile == 7 || tile == 9)
                    {
                        //Sticking down
                        obj.createblock(2, (i * 8), (j * 8), 8, 4);
                    }
                }
                //Breakable blocks
                if (tile == 10)
                {
                    settile(i, j, 0);
                    obj.createentity(i * 8, j * 8, 4);
                }
                //Directional blocks
                if (tile >= 14 && tile <= 17)
                {
                    obj.createblock(3, i * 8, j * 8, 8, 8, tile-14);
                }
            }
        }

        for (size_t i = 0; i < obj.entities.size(); i++)
        {
            if (obj.entities[i].type == 1 && obj.entities[i].behave >= 8 && obj.entities[i].behave < 10)
            {
                //put a block underneath
                int temp = obj.entities[i].xp / 8.0f;
                int temp2 = obj.entities[i].yp / 8.0f;
                settile(temp, temp2, 1);
                settile(temp+1, temp2, 1);
                settile(temp+2, temp2, 1);
                settile(temp+3, temp2, 1);
                if (obj.entities[i].w == 64)
                {
                    settile(temp+4, temp2, 1);
                    settile(temp+5, temp2, 1);
                    settile(temp+6, temp2, 1);
                    settile(temp+7, temp2, 1);
                }
            }
        }
    }

    //Special scripting: Create objects and triggers based on what crewmembers are rescued.
    if (!finalmode && !custommode)
    {
        //First up: the extra bits:
        //Vermilion's quest:
        if (rx == 100 && ry == 105)     //On path to verdigris
        {
            if (game.crewstats[3] && !game.crewstats[4])
            {
                obj.createentity(87, 105, 18, 15, 0, 18);
                obj.createblock(5, 87-32, 0, 32+32+32, 240, 3);
            }
        }
        else if (rx == 107 && ry == 100)    //victoria
        {
            if (game.crewstats[3] && !game.crewstats[5])
            {
                obj.createentity(140, 137, 18, 15, 0, 18);
                obj.createblock(5, 140-32, 0, 32+32+32, 240, 3);
            }
        }
        else if (rx == 114 && ry == 109)
        {
            if (game.crewstats[3] && !game.crewstats[2])
            {
                obj.createentity(235, 81, 18, 15, 0, 18);
                obj.createblock(5, 235-32, 0, 32+32+32, 240, 3);
            }
        }

        //Verdigris fixing the ship
        if (rx == 101 && ry == 109)
        {
            if (game.crewstats[4])
            {
                if(game.crewrescued()>4 && game.crewrescued()!=6)
                {
                    obj.createentity(175, 121, 18, 13, 0, 18);
                    obj.createblock(5, 175-32, 0, 32+32+32, 240, 4);
                }
            }
        }
        else if (rx == 103 && ry == 109)
        {
            if (game.crewstats[4])
            {
                if(game.crewrescued()<=4 && game.crewrescued()!=6)
                {
                    obj.createentity(53, 161, 18, 13, 1, 18);
                    obj.createblock(5, 53-32, 0, 32+32+32, 240, 4);
                }
            }
        }

        if (rx == 104 && ry == 111)
        {
            //Red
            //First: is he rescued?
            if (game.crewstats[3])
            {
                //If so, red will always be at his post
                obj.createentity(107, 121, 18, 15, 0, 18);
                //What script do we use?
                obj.createblock(5, 107-32, 0, 32+32+32, 240, 3);
            }
        }
        else if (rx == 103 && ry == 111)
        {
            //Yellow
            //First: is he rescued?
            if (game.crewstats[2])
            {
                obj.createentity(198, 105, 18, 14, 0, 18);
                //What script do we use?
                obj.createblock(5, 198-32, 0, 32+32+32, 240, 2);
            }
        }
        else if (rx == 103 && ry == 110)
        {
            //Green
            //First: is he rescued?
            if (game.crewstats[4])
            {
                obj.createentity(242, 177, 18, 13, 0, 18);
                //What script do we use?
                obj.createblock(5, 242-32, 177-20, 32+32+32, 40, 4);
            }
        }
        else if (rx == 104 && ry == 110)
        {
            //Purple
            //First: is she rescued?
            if (game.crewstats[1])
            {
                obj.createentity(140, 177, 18, 20, 0, 18);
                //What script do we use?
                obj.createblock(5, 140-32, 0, 32+32+32, 240, 1);
            }
        }
        else if (rx == 102 && ry == 110)
        {
            //Blue
            //First: is she rescued?
            if (game.crewstats[5])
            {
                //A slight varation - she's upside down
                obj.createentity(249, 62, 18, 16, 0, 18);
                int j = obj.getcrewman(BLUE);
                if (INBOUNDS_VEC(j, obj.entities))
                {
                    obj.entities[j].rule = 7;
                    obj.entities[j].tile +=6;
                }
                //What script do we use?
                obj.createblock(5, 249-32, 0, 32+32+32, 240, 5);
            }
        }
    }
}

void mapclass::twoframedelayfix(void)
{
    // Fixes the two-frame delay in custom levels that use scripts to spawn an entity upon room load.
    // Because when the room loads and newscript is set to run, newscript has already ran for that frame,
    // and when the script gets loaded script.run() has already ran for that frame, too.
    // A bit kludge-y, but it's the least we can do without changing the frame ordering.

    if (GlitchrunnerMode_less_than_or_equal(Glitchrunner2_2)
    || !custommode
    || game.deathseq != -1)
        return;

    int block_idx = -1;
    // obj.checktrigger() sets block_idx
    int activetrigger = obj.checktrigger(&block_idx);
    if (activetrigger <= -1
    || !INBOUNDS_VEC(block_idx, obj.blocks)
    || activetrigger < 300)
    {
        return;
    }

    game.newscript = obj.blocks[block_idx].script;
    obj.removetrigger(activetrigger);
    game.setstate(0);
    game.setstatedelay(0);
    script.load(game.newscript);
}
