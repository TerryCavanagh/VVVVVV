#if !defined(NO_CUSTOM_LEVELS) && !defined(NO_EDITOR)

#define ED_DEFINITION
#include "Editor.h"

#include <string>
#include <utf8/unchecked.h>

#include "CustomLevels.h"
#include "DeferCallbacks.h"
#include "Entity.h"
#include "Enums.h"
#include "Game.h"
#include "Graphics.h"
#include "GraphicsUtil.h"
#include "KeyPoll.h"
#include "Map.h"
#include "Music.h"
#include "Script.h"
#include "UtilityClass.h"

editorclass::editorclass(void)
{
    reset();
}

void editorclass::reset(void)
{
    roomnamehide=0;
    zmod=false;
    xmod=false;
    cmod=false;
    vmod=false;
    hmod=false;
    bmod=false;
    spacemod=false;
    spacemenu=0;
    shiftmenu=false;
    shiftkey=false;
    saveandquit=false;
    note="";
    notedelay=0;
    oldnotedelay=0;
    deletekeyheld=false;
    textmod = TEXT_NONE;

    titlemod=false;
    creatormod=false;
    desc1mod=false;
    desc2mod=false;
    desc3mod=false;
    websitemod=false;
    settingsmod=false;
    warpmod=false; //Two step process
    warpent=-1;

    boundarymod=0;
    boundarytype=0;
    boundx1=0;
    boundx2=0;
    boundy1=0;
    boundy2=0;

    textent=0;
    scripttexttype=0;

    drawmode=0;
    dmtile=0;
    dmtileeditor=0;
    entcol=0;

    tilex=0;
    tiley=0;
    levx=0;
    levy=0;
    keydelay=0;
    lclickdelay=0;
    savekey=false;
    loadkey=false;
    updatetiles=true;
    changeroom=true;

    entframe=0;
    entframedelay=0;

    SDL_zeroa(kludgewarpdir);

    hooklist.clear();

    sb.clear();

    clearscriptbuffer();
    sbx=0;
    sby=0;
    pagey=0;
    scripteditmod=false;
    sbscript="null";
    scripthelppage=0;
    scripthelppagedelay=0;

    hookmenupage=0;
    hookmenu=0;

    returneditoralpha = 0;
    oldreturneditoralpha = 0;

    ghosts.clear();
    currentghosts = 0;

    loaded_filepath = "";
}

void editorclass::gethooks(void)
{
    //Scan through the script and create a hooks list based on it
    hooklist.clear();
    for (size_t i = 0; i < script.customscripts.size(); i++)
    {
        Script& script_ = script.customscripts[i];

        hooklist.push_back(script_.name);
    }
}

void editorclass::loadhookineditor(const std::string& t)
{
    //Find hook t in the scriptclass, then load it into the editor
    clearscriptbuffer();

    for(size_t i = 0; i < script.customscripts.size(); i++)
    {
        Script& script_ = script.customscripts[i];

        if(script_.name == t)
        {
            sb = script_.contents;
            break;
        }
    }

    if(sb.empty())
    {
        //Always have one line or we'll have problems
        sb.resize(1);
    }
}

void editorclass::addhooktoscript(const std::string& t)
{
    //Adds hook+the scriptbuffer to the end of the scriptclass
    removehookfromscript(t);
    Script script_;
    script_.name = t;
    script_.contents = sb;
    script.customscripts.push_back(script_);
}

void editorclass::removehookfromscript(const std::string& t)
{
    /* Find hook t in the scriptclass, then removes it (and any other code with it)
     * When this loop reaches the end, it wraps to SIZE_MAX; SIZE_MAX + 1 is 0 */
    size_t i;
    for (i = script.customscripts.size() - 1; i + 1 > 0; --i)
    {
        if (script.customscripts[i].name == t)
        {
            script.customscripts.erase(script.customscripts.begin() + i);
        }
    }
}

void editorclass::removehook(const std::string& t)
{
    //Check the hooklist for the hook t. If it's there, remove it from here and the script
    size_t i;
    removehookfromscript(t);
    /* When this loop reaches the end, it wraps to SIZE_MAX; SIZE_MAX + 1 is 0 */
    for (i = hooklist.size() - 1; i + 1 > 0; --i)
    {
        if (hooklist[i] == t)
        {
            hooklist.erase(hooklist.begin() + i);
        }
    }
}

void editorclass::addhook(const std::string& t)
{
    //Add an empty function to the list in both editor and script
    removehook(t);
    hooklist.push_back(t);
    addhooktoscript(t);
}

bool editorclass::checkhook(const std::string& t)
{
    //returns true if hook t already is in the list
    for(size_t i=0; i<hooklist.size(); i++)
    {
        if(hooklist[i]==t) return true;
    }
    return false;
}


void editorclass::clearscriptbuffer(void)
{
    sb.clear();
}

void editorclass::removeline(int t)
{
    //Remove line t from the script
    if((int)sb.size()>1)
    {
        sb.erase(sb.begin() + t);
    }
}

void editorclass::insertline(int t)
{
    //insert a blank line into script at line t
    sb.insert(sb.begin() + t, "");
}

void editorclass::getlin(const enum textmode mode, const std::string& prompt, std::string* ptr)
{
    textmod = mode;
    textptr = ptr;
    textdesc = prompt;
    key.enabletextentry();
    if (ptr)
    {
        key.keybuffer = *ptr;
    }
    else
    {
        key.keybuffer = "";
        textptr = &(key.keybuffer);
    }

    oldenttext = key.keybuffer;
}


static void addedentity( int xp, int yp, int tp, int p1 = 0, int p2 = 0, int p3 = 0, int p4 = 0, int p5 = 320, int p6 = 240)
{
    CustomEntity entity;

    entity.x=xp;
    entity.y=yp;
    entity.t=tp;
    entity.p1=p1;
    entity.p2=p2;
    entity.p3=p3;
    entity.p4=p4;
    entity.p5=p5;
    entity.p6=p6;
    entity.scriptname="";

    customentities.push_back(entity);
}

static void removeedentity( int t )
{
    customentities.erase(customentities.begin() + t);
}

static int edentat( int xp, int yp )
{
    for(size_t i=0; i<customentities.size(); i++)
    {
        if(customentities[i].x==xp && customentities[i].y==yp) return i;
    }
    return -1;
}

static void fillbox( int x, int y, int x2, int y2, int c )
{
    FillRect(graphics.backBuffer, x, y, x2-x, 1, c);
    FillRect(graphics.backBuffer, x, y2-1, x2-x, 1, c);
    FillRect(graphics.backBuffer, x, y, 1, y2-y, c);
    FillRect(graphics.backBuffer, x2-1, y, 1, y2-y, c);
}

static void fillboxabs( int x, int y, int x2, int y2, int c )
{
    FillRect(graphics.backBuffer, x, y, x2, 1, c);
    FillRect(graphics.backBuffer, x, y+y2-1, x2, 1, c);
    FillRect(graphics.backBuffer, x, y, 1, y2, c);
    FillRect(graphics.backBuffer, x+x2-1, y, 1, y2, c);
}


static void editormenurender(int tr, int tg, int tb)
{
    extern editorclass ed;
    switch (game.currentmenuname)
    {
    case Menu::ed_settings:
        graphics.bigprint( -1, 75, "Map Settings", tr, tg, tb, true);
        if (game.currentmenuoption == 3)
        {
            if (!game.ghostsenabled)
                graphics.Print(2, 230, "Editor ghost trail is OFF", tr/2, tg/2, tb/2);
            else
                graphics.Print(2, 230, "Editor ghost trail is ON", tr, tg, tb);
        }
        break;
    case Menu::ed_desc:
        if(ed.titlemod)
        {
            if(ed.entframe<2)
            {
                graphics.bigprint( -1, 35, key.keybuffer+"_", tr, tg, tb, true);
            }
            else
            {
                graphics.bigprint( -1, 35, key.keybuffer+" ", tr, tg, tb, true);
            }
        }
        else
        {
            graphics.bigprint( -1, 35, cl.title, tr, tg, tb, true);
        }
        if(ed.creatormod)
        {
            if(ed.entframe<2)
            {
                graphics.Print( -1, 60, "by " + key.keybuffer+ "_", tr, tg, tb, true);
            }
            else
            {
                graphics.Print( -1, 60, "by " + key.keybuffer+ " ", tr, tg, tb, true);
            }
        }
        else
        {
            graphics.Print( -1, 60, "by " + cl.creator, tr, tg, tb, true);
        }
        if(ed.websitemod)
        {
            if(ed.entframe<2)
            {
                graphics.Print( -1, 70, key.keybuffer+"_", tr, tg, tb, true);
            }
            else
            {
                graphics.Print( -1, 70, key.keybuffer+" ", tr, tg, tb, true);
            }
        }
        else
        {
            graphics.Print( -1, 70, cl.website, tr, tg, tb, true);
        }
        if(ed.desc1mod)
        {
            if(ed.entframe<2)
            {
                graphics.Print( -1, 90, key.keybuffer+"_", tr, tg, tb, true);
            }
            else
            {
                graphics.Print( -1, 90, key.keybuffer+" ", tr, tg, tb, true);
            }
        }
        else
        {
            graphics.Print( -1, 90, cl.Desc1, tr, tg, tb, true);
        }
        if(ed.desc2mod)
        {
            if(ed.entframe<2)
            {
                graphics.Print( -1, 100, key.keybuffer+"_", tr, tg, tb, true);
            }
            else
            {
                graphics.Print( -1, 100, key.keybuffer+" ", tr, tg, tb, true);
            }
        }
        else
        {
            graphics.Print( -1, 100, cl.Desc2, tr, tg, tb, true);
        }
        if(ed.desc3mod)
        {
            if(ed.entframe<2)
            {
                graphics.Print( -1, 110, key.keybuffer+"_", tr, tg, tb, true);
            }
            else
            {
                graphics.Print( -1, 110, key.keybuffer+" ", tr, tg, tb, true);
            }
        }
        else
        {
            graphics.Print( -1, 110, cl.Desc3, tr, tg, tb, true);
        }
        break;
    case Menu::ed_music:
    {
        graphics.bigprint( -1, 65, "Map Music", tr, tg, tb, true);

        graphics.Print( -1, 85, "Current map music:", tr, tg, tb, true);
        std::string songname;
        switch(cl.levmusic)
        {
        case 0:
            songname = "No background music";
            break;
        case 1:
            songname = "1: Pushing Onwards";
            break;
        case 2:
            songname = "2: Positive Force";
            break;
        case 3:
            songname = "3: Potential for Anything";
            break;
        case 4:
            songname = "4: Passion for Exploring";
            break;
        case 5:
            songname = "N/A: Pause";
            break;
        case 6:
            songname = "5: Presenting VVVVVV";
            break;
        case 7:
            songname = "N/A: Plenary";
            break;
        case 8:
            songname = "6: Predestined Fate";
            break;
        case 9:
            songname = "N/A: ecroF evitisoP";
            break;
        case 10:
            songname = "7: Popular Potpourri";
            break;
        case 11:
            songname = "8: Pipe Dream";
            break;
        case 12:
            songname = "9: Pressure Cooker";
            break;
        case 13:
            songname = "10: Paced Energy";
            break;
        case 14:
            songname = "11: Piercing the Sky";
            break;
        case 15:
            songname = "N/A: Predestined Fate Remix";
            break;
        default:
            songname = "?: something else";
            break;
        }
        graphics.Print( -1, 120, songname, tr, tg, tb, true);
        break;
    }
    case Menu::ed_quit:
        graphics.bigprint( -1, 90, "Save before", tr, tg, tb, true);
        graphics.bigprint( -1, 110, "quitting?", tr, tg, tb, true);
        break;
    default:
        break;
    }
}

void editorrender(void)
{
    extern editorclass ed;
    const RoomProperty* const room = cl.getroomprop(ed.levx, ed.levy);

    //Draw grid

    ClearSurface(graphics.backBuffer);
    for(int j=0; j<30; j++)
    {
        for(int i=0; i<40; i++)
        {
            fillbox(i*8, j*8, (i*8)+7, (j*8)+7, graphics.getRGB(8,8,8)); //a simple grid
            if(i%4==0) fillbox(i*8, j*8, (i*8)+7, (j*8)+7, graphics.getRGB(16,16,16));
            if(j%4==0) fillbox(i*8, j*8, (i*8)+7, (j*8)+7, graphics.getRGB(16,16,16));

            //Minor guides
            if(i==9) fillbox(i*8, j*8, (i*8)+7, (j*8)+7, graphics.getRGB(24,24,24));
            if(i==30) fillbox(i*8, j*8, (i*8)+7, (j*8)+7, graphics.getRGB(24,24,24));
            if(j==6 || j==7) fillbox(i*8, j*8, (i*8)+7, (j*8)+7, graphics.getRGB(24,24,24));
            if(j==21 || j==22) fillbox(i*8, j*8, (i*8)+7, (j*8)+7, graphics.getRGB(24,24,24));

            //Major guides
            if(i==20 || i==19) fillbox(i*8, j*8, (i*8)+7, (j*8)+7, graphics.getRGB(32,32,32));
            if(j==14) fillbox(i*8, j*8, (i*8)+7, (j*8)+7, graphics.getRGB(32,32,32));
        }
    }

    //Or draw background
    if(!ed.settingsmod)
    {
        switch(room->warpdir)
        {
        case 1:
            graphics.rcol=cl.getwarpbackground(ed.levx, ed.levy);
            graphics.drawbackground(3);
            break;
        case 2:
            graphics.rcol=cl.getwarpbackground(ed.levx, ed.levy);
            graphics.drawbackground(4);
            break;
        case 3:
            graphics.rcol=cl.getwarpbackground(ed.levx, ed.levy);
            graphics.drawbackground(5);
            break;
        default:
            break;
        }
    }

    //Draw map, in function
    int temp;
    if(room->tileset==0 || room->tileset==10)
    {
        for (int j = 0; j < 30; j++)
        {
            for (int i = 0; i < 40; i++)
            {
                temp=cl.gettile(ed.levx, ed.levy, i, j);
                if(temp>0) graphics.drawtile(i*8,j*8,temp);
            }
        }
    }
    else
    {
        for (int j = 0; j < 30; j++)
        {
            for (int i = 0; i < 40; i++)
            {
                temp=cl.gettile(ed.levx, ed.levy, i, j);
                if(temp>0) graphics.drawtile2(i*8,j*8,temp);
            }
        }
    }

    //Edge tile fix

    //Buffer the sides of the new room with tiles from other rooms, to ensure no gap problems.
    for(int j=0; j<30; j++)
    {
        //left edge
        if(ed.freewrap((ed.levx*40)-1,j+(ed.levy*30))==1)
        {
            FillRect(graphics.backBuffer, 0,j*8, 2,8, graphics.getRGB(255 - help.glow, 255, 255));
        }
        //right edge
        if(ed.freewrap((ed.levx*40)+40,j+(ed.levy*30))==1)
        {
            FillRect(graphics.backBuffer, 318,j*8, 2,8, graphics.getRGB(255 - help.glow, 255, 255));
        }
    }

    for(int i=0; i<40; i++)
    {
        if(ed.freewrap((ed.levx*40)+i,(ed.levy*30)-1)==1)
        {
            FillRect(graphics.backBuffer, i*8,0, 8,2, graphics.getRGB(255 - help.glow, 255, 255));
        }

        if(ed.freewrap((ed.levx*40)+i,30+(ed.levy*30))==1)
        {
            FillRect(graphics.backBuffer, i*8,238, 8,2, graphics.getRGB(255 - help.glow, 255, 255));
        }
    }

    //Draw entities
    obj.customplatformtile=game.customcol*12;

    int temp2=edentat(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30));

    // Special case for drawing gray entities
    bool custom_gray = room->tileset == 3 && room->tilecol == 6;
    colourTransform gray_ct;
    gray_ct.colour = 0xFFFFFFFF;

    // Draw entities backward to remain accurate with ingame
    for (int i = customentities.size() - 1; i >= 0; i--)
    {
        point tpoint;
        SDL_Rect drawRect;

        //if() on screen
        if(customentities[i].x / 40 == ed.levx && customentities[i].y / 30 == ed.levy)
        {
            switch(customentities[i].t)
            {
            case 1: //Entities
                if (custom_gray) {
                    graphics.setcol(18);
                    ed.entcolreal = graphics.ct.colour;
                }
                graphics.drawsprite((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),ed.getenemyframe(room->enemytype),ed.entcolreal);
                if(customentities[i].p1==0) graphics.Print((customentities[i].x*8)- (ed.levx*40*8)+4,(customentities[i].y*8)- (ed.levy*30*8)+4, "V", 255, 255, 255 - help.glow, false);
                if(customentities[i].p1==1) graphics.Print((customentities[i].x*8)- (ed.levx*40*8)+4,(customentities[i].y*8)- (ed.levy*30*8)+4, "^", 255, 255, 255 - help.glow, false);
                if(customentities[i].p1==2) graphics.Print((customentities[i].x*8)- (ed.levx*40*8)+4,(customentities[i].y*8)- (ed.levy*30*8)+4, "<", 255, 255, 255 - help.glow, false);
                if(customentities[i].p1==3) graphics.Print((customentities[i].x*8)- (ed.levx*40*8)+4,(customentities[i].y*8)- (ed.levy*30*8)+4, ">", 255, 255, 255 - help.glow, false);
                fillboxabs((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),16,16,graphics.getBGR(255,164,255));
                break;
            case 2: //Threadmills & platforms
                if (!INBOUNDS_VEC(obj.customplatformtile, graphics.entcolours))
                {
                    continue;
                }
                tpoint.x = (customentities[i].x*8)- (ed.levx*40*8);
                tpoint.y = (customentities[i].y*8)- (ed.levy*30*8);
                drawRect = graphics.tiles_rect;
                drawRect.x += tpoint.x;
                drawRect.y += tpoint.y;
                for (int j = 0; j < 4; j++) {
                    if (custom_gray) BlitSurfaceTinted(graphics.entcolours[obj.customplatformtile],NULL, graphics.backBuffer, &drawRect, gray_ct);
                    else BlitSurfaceStandard(graphics.entcolours[obj.customplatformtile],NULL, graphics.backBuffer, &drawRect);
                    drawRect.x += 8;
                }

                if(customentities[i].p1<=4)
                {
                    if(customentities[i].p1==0) graphics.Print((customentities[i].x*8)- (ed.levx*40*8)+12,(customentities[i].y*8)- (ed.levy*30*8), "V", 255 - help.glow, 255 - help.glow, 255 - help.glow, false);
                    if(customentities[i].p1==1) graphics.Print((customentities[i].x*8)- (ed.levx*40*8)+12,(customentities[i].y*8)- (ed.levy*30*8), "^", 255 - help.glow, 255 - help.glow, 255 - help.glow, false);
                    if(customentities[i].p1==2) graphics.Print((customentities[i].x*8)- (ed.levx*40*8)+12,(customentities[i].y*8)- (ed.levy*30*8), "<", 255 - help.glow, 255 - help.glow, 255 - help.glow, false);
                    if(customentities[i].p1==3) graphics.Print((customentities[i].x*8)- (ed.levx*40*8)+12,(customentities[i].y*8)- (ed.levy*30*8), ">", 255 - help.glow, 255 - help.glow, 255 - help.glow, false);
                    fillboxabs((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),32,8,graphics.getBGR(255,255,255));
                }

                if(customentities[i].p1==5)
                {
                    graphics.Print((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8), ">>>>", 255 - help.glow, 255 - help.glow, 255 - help.glow, false);
                    fillboxabs((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),32,8,graphics.getBGR(255,255,255));
                }
                else if(customentities[i].p1==6)
                {
                    graphics.Print((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8), "<<<<", 255 - help.glow, 255 - help.glow, 255 - help.glow, false);
                    fillboxabs((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),32,8,graphics.getBGR(255,255,255));
                }

                if(customentities[i].p1>=7)
                {
                    tpoint.x = (customentities[i].x*8)- (ed.levx*40*8)+32;
                    tpoint.y = (customentities[i].y*8)- (ed.levy*30*8);
                    drawRect = graphics.tiles_rect;
                    drawRect.x += tpoint.x;
                    drawRect.y += tpoint.y;
                    for (int j = 0; j < 4; j++) {
                        if (custom_gray) BlitSurfaceTinted(graphics.entcolours[obj.customplatformtile],NULL, graphics.backBuffer, &drawRect, gray_ct);
                        else BlitSurfaceStandard(graphics.entcolours[obj.customplatformtile],NULL, graphics.backBuffer, &drawRect);
                        drawRect.x += 8;
                    }
                }

                if(customentities[i].p1==7)
                {
                    graphics.Print((customentities[i].x*8)- (ed.levx*40*8)+4,(customentities[i].y*8)- (ed.levy*30*8), "> > > > ", 255 - help.glow, 255 - help.glow, 255 - help.glow, false);
                    fillboxabs((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),64,8,graphics.getBGR(255,255,255));
                }
                else if(customentities[i].p1==8)
                {
                    graphics.Print((customentities[i].x*8)- (ed.levx*40*8)+4,(customentities[i].y*8)- (ed.levy*30*8), "< < < < ", 255 - help.glow, 255 - help.glow, 255 - help.glow, false);
                    fillboxabs((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),64,8,graphics.getBGR(255,255,255));
                }
                break;
            case 3: //Disappearing Platform
                if (!INBOUNDS_VEC(obj.customplatformtile, graphics.entcolours))
                {
                    continue;
                }
                tpoint.x = (customentities[i].x*8)- (ed.levx*40*8);
                tpoint.y = (customentities[i].y*8)- (ed.levy*30*8);
                drawRect = graphics.tiles_rect;
                drawRect.x += tpoint.x;
                drawRect.y += tpoint.y;
                for (int j = 0; j < 4; j++) {
                    if (custom_gray) BlitSurfaceTinted(graphics.entcolours[obj.customplatformtile],NULL, graphics.backBuffer, &drawRect, gray_ct);
                    else BlitSurfaceStandard(graphics.entcolours[obj.customplatformtile],NULL, graphics.backBuffer, &drawRect);
                    drawRect.x += 8;
                }

                graphics.Print((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8), "////", 255 - help.glow, 255 - help.glow, 255 - help.glow, false);
                fillboxabs((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),32,8,graphics.getBGR(255,255,255));
                break;
            case 9: //Shiny Trinket
                graphics.drawsprite((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),22,196,196,196);
                fillboxabs((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),16,16,graphics.getRGB(255, 164, 164));
                break;
            case 10: //Checkpoints
                graphics.drawsprite((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),20 + customentities[i].p1,196,196,196);
                fillboxabs((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),16,16,graphics.getRGB(255, 164, 164));
                break;
            case 11: //Gravity lines
                if(customentities[i].p1==0)  //Horizontal
                {
                    int tx = customentities[i].p2;
                    int tx2 = tx + customentities[i].p3/8;
                    int ty = customentities[i].y % 30;
                    FillRect(graphics.backBuffer, (tx*8),(ty*8)+4, (tx2-tx)*8,1, graphics.getRGB(194,194,194));
                    fillboxabs((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),8,8,graphics.getRGB(164,255,164));
                }
                else  //Vertical
                {
                    int tx = customentities[i].x % 40;
                    int ty = customentities[i].p2;
                    int ty2 = ty + customentities[i].p3/8;
                    FillRect(graphics.backBuffer, (tx*8)+3,(ty*8), 1,(ty2-ty)*8, graphics.getRGB(194,194,194));
                    fillboxabs((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),8,8,graphics.getRGB(164,255,164));
                }
                break;
            case 13://Warp tokens
                graphics.drawsprite((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),18+(ed.entframe%2),196,196,196);
                fillboxabs((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),16,16,graphics.getRGB(255, 164, 164));
                if(temp2==i)
                {
                    graphics.bprint((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8)-8,
                                "("+help.String(((customentities[i].p1-int(customentities[i].p1%40))/40)+1)+","+help.String(((customentities[i].p2-int(customentities[i].p2%30))/30)+1)+")",210,210,255);
                }
                else
                {
                    graphics.bprint((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8)-8,help.String(cl.findwarptoken(i)),210,210,255);
                }
                break;
            case 15: //Crewmates
                graphics.drawsprite((customentities[i].x*8)- (ed.levx*40*8)-4,(customentities[i].y*8)- (ed.levy*30*8),144,graphics.crewcolourreal(customentities[i].p1));
                fillboxabs((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),16,24,graphics.getRGB(164,164,164));
                break;
            case 16: //Start
                if(customentities[i].p1==0)  //Left
                {
                    graphics.drawsprite((customentities[i].x*8)- (ed.levx*40*8)-4,(customentities[i].y*8)- (ed.levy*30*8),0,graphics.col_crewcyan);
                }
                else if(customentities[i].p1==1)
                {
                    graphics.drawsprite((customentities[i].x*8)- (ed.levx*40*8)-4,(customentities[i].y*8)- (ed.levy*30*8),3,graphics.col_crewcyan);
                }
                fillboxabs((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),16,24,graphics.getRGB(255, 255, 164));
                if(ed.entframe<2)
                {
                    graphics.bprint((customentities[i].x*8)- (ed.levx*40*8)-12,(customentities[i].y*8)- (ed.levy*30*8)-8,"START",255,255,255);
                }
                else
                {
                    graphics.bprint((customentities[i].x*8)- (ed.levx*40*8)-12,(customentities[i].y*8)- (ed.levy*30*8)-8,"START",196,196,196);
                }
                break;
            case 17: //Roomtext
                if(customentities[i].scriptname.length()<1)
                {
                    fillboxabs((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),8,8,graphics.getRGB(96,96,96));
                }
                else
                {
                    int length = utf8::unchecked::distance(customentities[i].scriptname.begin(), customentities[i].scriptname.end());
                    fillboxabs((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),length*8,8,graphics.getRGB(96,96,96));
                }
                graphics.Print((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8), customentities[i].scriptname, 196, 196, 255 - help.glow);
                break;
            case 18: //Terminals
            {
                int usethistile = customentities[i].p1;
                int usethisy = (customentities[i].y % 30) * 8;
                // Not a boolean: just swapping 0 and 1, leaving the rest alone
                if (usethistile == 0)
                {
                    usethistile = 1; // Unflipped
                }
                else if (usethistile == 1)
                {
                    usethistile = 0; // Flipped;
                    usethisy -= 8;
                }
                graphics.drawsprite((customentities[i].x*8)- (ed.levx*40*8), usethisy + 8, usethistile + 16, 96,96,96);
                fillboxabs((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),16,24,graphics.getRGB(164,164,164));
                if(temp2==i)
                {
                    graphics.bprint((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8)-8,customentities[i].scriptname,210,210,255);
                }
                break;
            }
            case 19: //Script Triggers
                fillboxabs((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),customentities[i].p1*8,customentities[i].p2*8,graphics.getRGB(255,164,255));
                fillboxabs((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),8,8,graphics.getRGB(255,255,255));
                if(temp2==i)
                {
                    graphics.bprint((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8)-8,customentities[i].scriptname,210,210,255);
                }
                break;
            case 50: //Warp lines
                if(customentities[i].p1>=2)  //Horizontal
                {
                    int tx=customentities[i].x-(ed.levx*40);
                    int tx2=customentities[i].x-(ed.levx*40);
                    int ty=customentities[i].y-(ed.levy*30);
                    if (customentities[i].p4 != 1)
                    {
                        // Unlocked
                        while(ed.free(tx,ty)==0) tx--;
                        while(ed.free(tx2,ty)==0) tx2++;
                        tx++;
                        customentities[i].p2=tx;
                        customentities[i].p3=(tx2-tx)*8;
                    }
                    else
                    {
                        // Locked
                        tx = customentities[i].p2;
                        tx2 = tx + customentities[i].p3/8;
                    }
                    fillboxabs((tx*8),(ty*8)+1, (tx2-tx)*8,6, graphics.getRGB(194, 255, 255));
                    fillboxabs((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),8,8,graphics.getRGB(164, 255, 255));
                }
                else  //Vertical
                {
                    int tx=customentities[i].x-(ed.levx*40);
                    int ty=customentities[i].y-(ed.levy*30);
                    int ty2=customentities[i].y-(ed.levy*30);
                    if (customentities[i].p4 != 1)
                    {
                        // Unlocked
                        while(ed.free(tx,ty)==0) ty--;
                        while(ed.free(tx,ty2)==0) ty2++;
                        ty++;
                        customentities[i].p2=ty;
                        customentities[i].p3=(ty2-ty)*8;
                    }
                    else
                    {
                        // Locked
                        ty = customentities[i].p2;
                        ty2 = ty + customentities[i].p3/8;
                    }
                    fillboxabs((tx*8)+1,(ty*8), 6,(ty2-ty)*8, graphics.getRGB(194, 255, 255));
                    fillboxabs((customentities[i].x*8)- (ed.levx*40*8),(customentities[i].y*8)- (ed.levy*30*8),8,8,graphics.getRGB(164, 255, 255));
                }
                break;
            }
        }

        //Need to also check warp point destinations
        if(customentities[i].t==13 && ed.warpent!=i)
        {
            if (customentities[i].p1 / 40 == ed.levx && customentities[i].p2 / 30 == ed.levy)
            {
                graphics.drawsprite((customentities[i].p1*8)- (ed.levx*40*8),(customentities[i].p2*8)- (ed.levy*30*8),18+(ed.entframe%2),64,64,64);
                fillboxabs((customentities[i].p1*8)- (ed.levx*40*8),(customentities[i].p2*8)- (ed.levy*30*8),16,16,graphics.getRGB(96, 64, 64));
                if(ed.tilex+(ed.levx*40)==customentities[i].p1 && ed.tiley+(ed.levy*30)==customentities[i].p2)
                {
                    graphics.bprint((customentities[i].p1*8)- (ed.levx*40*8),(customentities[i].p2*8)- (ed.levy*30*8)-8,
                                "("+help.String(((customentities[i].x-int(customentities[i].x%40))/40)+1)+","+help.String(((customentities[i].y-int(customentities[i].y%30))/30)+1)+")",190,190,225);
                }
                else
                {
                    graphics.bprint((customentities[i].p1*8)- (ed.levx*40*8),(customentities[i].p2*8)- (ed.levy*30*8)-8,help.String(cl.findwarptoken(i)),190,190,225);
                }
            }
        }
    }

    if(ed.boundarymod>0)
    {
        if(ed.boundarymod==1)
        {
            fillboxabs(ed.tilex*8, ed.tiley*8, 8,8,graphics.getRGB(210 + help.glow/2, 191 + help.glow, 255 - help.glow/2));
            fillboxabs((ed.tilex*8)+2, (ed.tiley*8)+2, 4,4,graphics.getRGB(105 + help.glow/4, 100 + help.glow/2, 128 - help.glow/4));
        }
        else if(ed.boundarymod==2)
        {
            if((ed.tilex*8)+8<=ed.boundx1 || (ed.tiley*8)+8<=ed.boundy1)
            {
                fillboxabs(ed.boundx1, ed.boundy1, 8, 8,graphics.getRGB(210 + help.glow/2, 191 + help.glow, 255 - help.glow/2));
                fillboxabs(ed.boundx1+2, ed.boundy1+2, 4, 4,graphics.getRGB(105 + help.glow/4, 100 + help.glow/2, 128 - help.glow/4));
            }
            else
            {
                fillboxabs(ed.boundx1, ed.boundy1, (ed.tilex*8)+8-ed.boundx1,(ed.tiley*8)+8-ed.boundy1,graphics.getRGB(210 + help.glow/2, 191 + help.glow, 255 - help.glow/2));
                fillboxabs(ed.boundx1+2, ed.boundy1+2, (ed.tilex*8)+8-ed.boundx1-4,(ed.tiley*8)+8-ed.boundy1-4,graphics.getRGB(105 + help.glow/4, 100 + help.glow/2, 128 - help.glow/4));
            }
        }
    }
    else
    {
        //Draw boundaries
        if(room->enemyx1!=0 || room->enemyy1!=0
                || room->enemyx2!=320 || room->enemyy2!=240)
        {
            fillboxabs( room->enemyx1, room->enemyy1,
                       room->enemyx2-room->enemyx1,
                       room->enemyy2-room->enemyy1,
                       graphics.getBGR(255-(help.glow/2),64,64));
        }

        if(room->platx1!=0 || room->platy1!=0
                || room->platx2!=320 || room->platy2!=240)
        {
            fillboxabs( room->platx1, room->platy1,
                       room->platx2-room->platx1,
                       room->platy2-room->platy1,
                       graphics.getBGR(64,64,255-(help.glow/2)));
        }
    }

    //Draw ghosts (spooky!)
    if (game.ghostsenabled) {
        ClearSurface(graphics.ghostbuffer);
        for (int i = 0; i < (int)ed.ghosts.size(); i++) {
            if (i <= ed.currentghosts) { // We don't want all of them to show up at once :)
                if (ed.ghosts[i].rx != ed.levx || ed.ghosts[i].ry != ed.levy
                || !INBOUNDS_VEC(ed.ghosts[i].frame, graphics.sprites))
                    continue;
                point tpoint;
                tpoint.x = ed.ghosts[i].x;
                tpoint.y = ed.ghosts[i].y;
                graphics.setcolreal(ed.ghosts[i].realcol);
                Uint32 alpha = graphics.ct.colour & graphics.backBuffer->format->Amask;
                Uint32 therest = graphics.ct.colour & 0x00FFFFFF;
                alpha = (3 * (alpha >> 24) / 4) << 24;
                graphics.ct.colour = therest | alpha;
                SDL_Rect drawRect = graphics.sprites_rect;
                drawRect.x += tpoint.x;
                drawRect.y += tpoint.y;
                BlitSurfaceColoured(graphics.sprites[ed.ghosts[i].frame],NULL, graphics.ghostbuffer, &drawRect, graphics.ct);
            }
        }
        SDL_BlitSurface(graphics.ghostbuffer, NULL, graphics.backBuffer, &graphics.bg_rect);
    }

    //Draw Cursor
    switch(ed.drawmode)
    {
    case 0:
    case 1:
    case 2:
    case 9:
    case 10:
    case 12: //Single point
        fillboxabs((ed.tilex*8),(ed.tiley*8),8,8, graphics.getRGB(32, 32, 200));
        break;
    case 3:
    case 4:
    case 8:
    case 13://2x2
        fillboxabs((ed.tilex*8),(ed.tiley*8),16,16, graphics.getRGB(32, 32, 200));
        break;
    case 5:
    case 6:
    case 7://Platform
        fillboxabs((ed.tilex*8),(ed.tiley*8),32,8, graphics.getRGB(32, 32, 200));
        break;
    case 14: //X if not on edge
        if(ed.tilex==0 || ed.tilex==39 || ed.tiley==0 || ed.tiley==29)
        {
            fillboxabs((ed.tilex*8),(ed.tiley*8),8,8, graphics.getRGB(32, 32, 200));
        }
        else
        {
            graphics.Print((ed.tilex*8),(ed.tiley*8),"X",255,0,0);
        }
        break;
    case 11:
    case 15:
    case 16: //2x3
        fillboxabs((ed.tilex*8),(ed.tiley*8),16,24, graphics.getRGB(32, 32, 200));
        break;
    }

    if(ed.drawmode<3)
    {
        if(ed.bmod && ed.drawmode<2)
        {
            fillboxabs((ed.tilex*8),0,8,240,graphics.getRGB(32, 32, 200));
        }
        else if(ed.hmod && ed.drawmode<2)
        {
            fillboxabs(0,(ed.tiley*8),320,8,graphics.getRGB(32, 32, 200));
        }
        else if(ed.vmod && ed.drawmode<2)
        {
            fillboxabs((ed.tilex*8)-32,(ed.tiley*8)-32,24+48,24+48, graphics.getRGB(32, 32, 200));
        }
        else if(ed.cmod && ed.drawmode<2)
        {
            fillboxabs((ed.tilex*8)-24,(ed.tiley*8)-24,24+32,24+32, graphics.getRGB(32, 32, 200));
        }
        else if(ed.xmod && ed.drawmode<2)
        {
            fillboxabs((ed.tilex*8)-16,(ed.tiley*8)-16,24+16,24+16, graphics.getRGB(32, 32, 200));
        }
        else if(ed.zmod && ed.drawmode<2)
        {
            fillboxabs((ed.tilex*8)-8,(ed.tiley*8)-8,24,24, graphics.getRGB(32, 32, 200));
        }


    }

    //If in directmode, show current directmode tile
    if(room->directmode==1)
    {
        //Tile box for direct mode
        int t2=0;
        if(ed.dmtileeditor>0)
        {
            if(ed.dmtileeditor<=4)
            {
                t2=graphics.lerp((4-ed.dmtileeditor+1)*12, (4-ed.dmtileeditor)*12);
            }

            //Draw five lines of the editor
            temp=ed.dmtile-(ed.dmtile%40);
            temp-=80;
            FillRect(graphics.backBuffer, 0,-t2,320,40, graphics.getRGB(0,0,0));
            FillRect(graphics.backBuffer, 0,-t2+40,320,2, graphics.getRGB(255,255,255));
            if(room->tileset==0)
            {
                const int numtiles = (((int) graphics.tiles.size()) / 40) * 40;

                for(int i=0; i<40; i++)
                {
                    graphics.drawtile(i*8,0-t2,(temp+numtiles+i)%numtiles);
                    graphics.drawtile(i*8,8-t2,(temp+numtiles+40+i)%numtiles);
                    graphics.drawtile(i*8,16-t2,(temp+numtiles+80+i)%numtiles);
                    graphics.drawtile(i*8,24-t2,(temp+numtiles+120+i)%numtiles);
                    graphics.drawtile(i*8,32-t2,(temp+numtiles+160+i)%numtiles);
                }
            }
            else
            {
                const int numtiles = (((int) graphics.tiles2.size()) / 40) * 40;

                for(int i=0; i<40; i++)
                {
                    graphics.drawtile2(i*8,0-t2,(temp+numtiles+i)%numtiles);
                    graphics.drawtile2(i*8,8-t2,(temp+numtiles+40+i)%numtiles);
                    graphics.drawtile2(i*8,16-t2,(temp+numtiles+80+i)%numtiles);
                    graphics.drawtile2(i*8,24-t2,(temp+numtiles+120+i)%numtiles);
                    graphics.drawtile2(i*8,32-t2,(temp+numtiles+160+i)%numtiles);
                }
            }
            //Highlight our little block
            fillboxabs(((ed.dmtile%40)*8)-2,16-t2-2,12,12,graphics.getRGB(255 - help.glow, 196, 196));
            fillboxabs(((ed.dmtile%40)*8)-1,16-t2-1,10,10,graphics.getRGB(0,0,0));
        }

        if(ed.dmtileeditor>0 && t2<=30)
        {
            graphics.bprint(2, 45-t2, "Tile:", 196, 196, 255 - help.glow, false);
            graphics.bprint(58, 45-t2, help.String(ed.dmtile), 196, 196, 255 - help.glow, false);
            FillRect(graphics.backBuffer, 44,44-t2,10,10, graphics.getRGB(255 - help.glow, 196, 196));
            FillRect(graphics.backBuffer, 45,45-t2,8,8, graphics.getRGB(0,0,0));

            if(room->tileset==0)
            {
                graphics.drawtile(45,45-t2,ed.dmtile);
            }
            else
            {
                graphics.drawtile2(45,45-t2,ed.dmtile);
            }
        }
        else
        {
            graphics.bprint(2, 12, "Tile:", 196, 196, 255 - help.glow, false);
            graphics.bprint(58, 12, help.String(ed.dmtile), 196, 196, 255 - help.glow, false);
            FillRect(graphics.backBuffer, 44,11,10,10, graphics.getRGB(255 - help.glow, 196, 196));
            FillRect(graphics.backBuffer, 45,12,8,8, graphics.getRGB(0,0,0));

            if(room->tileset==0)
            {
                graphics.drawtile(45,12,ed.dmtile);
            }
            else
            {
                graphics.drawtile2(45,12,ed.dmtile);
            }
        }
    }




    //Draw GUI
    if(ed.boundarymod>0)
    {
        if(ed.boundarymod==1)
        {
            FillRect(graphics.backBuffer, 0,230,320,240, graphics.getRGB(32,32,32));
            FillRect(graphics.backBuffer, 0,231,320,240, graphics.getRGB(0,0,0));
            switch(ed.boundarytype)
            {
            case 0:
                graphics.Print(4, 232, "SCRIPT BOX: Click on top left", 255,255,255, false);
                break;
            case 1:
                graphics.Print(4, 232, "ENEMY BOUNDS: Click on top left", 255,255,255, false);
                break;
            case 2:
                graphics.Print(4, 232, "PLATFORM BOUNDS: Click on top left", 255,255,255, false);
                break;
            case 3:
                graphics.Print(4, 232, "COPY TILES: Click on top left", 255,255,255, false);
                break;
            default:
                graphics.Print(4, 232, "Click on top left", 255,255,255, false);
                break;
            }
        }
        else if(ed.boundarymod==2)
        {
            FillRect(graphics.backBuffer, 0,230,320,240, graphics.getRGB(32,32,32));
            FillRect(graphics.backBuffer, 0,231,320,240, graphics.getRGB(0,0,0));
            switch(ed.boundarytype)
            {
            case 0:
                graphics.Print(4, 232, "SCRIPT BOX: Click on bottom right", 255,255,255, false);
                break;
            case 1:
                graphics.Print(4, 232, "ENEMY BOUNDS: Click on bottom right", 255,255,255, false);
                break;
            case 2:
                graphics.Print(4, 232, "PLATFORM BOUNDS: Click on bottom right", 255,255,255, false);
                break;
            case 3:
                graphics.Print(4, 232, "COPY TILES: Click on bottom right", 255,255,255, false);
                break;
            default:
                graphics.Print(4, 232, "Click on bottom right", 255,255,255, false);
                break;
            }
        }
    }
    else if(ed.scripteditmod)
    {
        //Elaborate C64 BASIC menu goes here!
        FillRect(graphics.backBuffer, 0,0,320,240, graphics.getBGR(123, 111, 218));
        FillRect(graphics.backBuffer, 14,16,292,208, graphics.getRGB(61, 48, 162));
        switch(ed.scripthelppage)
        {
        case 0:
            graphics.Print(16,28,"**** VVVVVV SCRIPT EDITOR ****", 123, 111, 218, true);
            graphics.Print(16,44,"PRESS ESC TO RETURN TO MENU", 123, 111, 218, true);

            if(!ed.hooklist.empty())
            {
                for(int i=0; i<9; i++)
                {
                    if(ed.hookmenupage+i<(int)ed.hooklist.size())
                    {
                        if(ed.hookmenupage+i==ed.hookmenu)
                        {
                            std::string tstring="> " + ed.hooklist[(ed.hooklist.size()-1)-(ed.hookmenupage+i)] + " <";
                            for (size_t ii = 0; ii < tstring.length(); ii++)
                            {
                                tstring[ii] = SDL_toupper(tstring[ii]);
                            }
                            graphics.Print(16,68+(i*16),tstring,123, 111, 218, true);
                        }
                        else
                        {
                            graphics.Print(16,68+(i*16),ed.hooklist[(ed.hooklist.size()-1)-(ed.hookmenupage+i)],123, 111, 218, true);
                        }
                    }
                }
            }
            else
            {
                graphics.Print(16,110,"NO SCRIPT IDS FOUND", 123, 111, 218, true);
                graphics.Print(16,130,"CREATE A SCRIPT WITH EITHER", 123, 111, 218, true);
                graphics.Print(16,140,"THE TERMINAL OR SCRIPT BOX TOOLS", 123, 111, 218, true);
            }
            break;
        case 1:
            //Current scriptname
            FillRect(graphics.backBuffer, 14,226,292,12, graphics.getRGB(61, 48, 162));
            graphics.Print(16,228,"CURRENT SCRIPT: " + ed.sbscript, 123, 111, 218, true);
            //Draw text
            for(int i=0; i<25; i++)
            {
                if(i+ed.pagey<(int)ed.sb.size())
                {
                    graphics.Print(16,20+(i*8),ed.sb[i+ed.pagey], 123, 111, 218, false);
                }
            }
            //Draw cursor
            if(ed.entframe<2)
            {
                graphics.Print(16+(ed.sbx*8),20+(ed.sby*8),"_",123, 111, 218, false);
            }
            break;
        }
    }
    else if(ed.settingsmod)
    {
        if(!game.colourblindmode)
        {
            graphics.drawtowerbackground(graphics.titlebg);
        }
        else
        {
            ClearSurface(graphics.backBuffer);
        }

        int tr = graphics.titlebg.r - (help.glow / 4) - int(fRandom() * 4);
        int tg = graphics.titlebg.g - (help.glow / 4) - int(fRandom() * 4);
        int tb = graphics.titlebg.b - (help.glow / 4) - int(fRandom() * 4);
        if (tr < 0) tr = 0;
        if(tr>255) tr=255;
        if (tg < 0) tg = 0;
        if(tg>255) tg=255;
        if (tb < 0) tb = 0;
        if(tb>255) tb=255;
        editormenurender(tr, tg, tb);

        graphics.drawmenu(tr, tg, tb);
    }
    else if (ed.textmod)
    {
        FillRect(graphics.backBuffer, 0, 221, 320, 240, graphics.getRGB(32, 32, 32));
        FillRect(graphics.backBuffer, 0, 222, 320, 240, graphics.getRGB(0, 0, 0));
        graphics.Print(4, 224, ed.textdesc, 255, 255, 255, false);
        std::string input = key.keybuffer;
        if (ed.entframe < 2)
        {
            input += "_";
        }
        else
        {
            input += " ";
        }
        graphics.Print(4, 232, input, 196, 196, 255 - help.glow, true);
    }
    else if(ed.warpmod)
    {
        //placing warp token
        FillRect(graphics.backBuffer, 0,221,320,240, graphics.getRGB(32,32,32));
        FillRect(graphics.backBuffer, 0,222,320,240, graphics.getRGB(0,0,0));
        graphics.Print(4, 224, "Left click to place warp destination", 196, 196, 255 - help.glow, false);
        graphics.Print(4, 232, "Right click to cancel", 196, 196, 255 - help.glow, false);
    }
    else
    {
        if(ed.spacemod)
        {
            FillRect(graphics.backBuffer, 0,208,320,240, graphics.getRGB(32,32,32));
            FillRect(graphics.backBuffer, 0,209,320,240, graphics.getRGB(0,0,0));

            //Draw little icons for each thingy
            int tx=6, ty=211, tg=32;

            if(ed.spacemenu==0)
            {
                for(int i=0; i<10; i++)
                {
                    FillRect(graphics.backBuffer, 4+(i*tg), 209,20,20,graphics.getRGB(32,32,32));
                }
                FillRect(graphics.backBuffer, 4+(ed.drawmode*tg), 209,20,20,graphics.getRGB(64,64,64));
                //0:
                graphics.drawtile(tx,ty,83);
                graphics.drawtile(tx+8,ty,83);
                graphics.drawtile(tx,ty+8,83);
                graphics.drawtile(tx+8,ty+8,83);
                //1:
                tx+=tg;
                graphics.drawtile(tx,ty,680);
                graphics.drawtile(tx+8,ty,680);
                graphics.drawtile(tx,ty+8,680);
                graphics.drawtile(tx+8,ty+8,680);
                //2:
                tx+=tg;
                graphics.drawtile(tx+4,ty+4,8);
                //3:
                tx+=tg;
                graphics.drawsprite(tx,ty,22,196,196,196);
                //4:
                tx+=tg;
                graphics.drawsprite(tx,ty,21,196,196,196);
                //5:
                tx+=tg;
                graphics.drawtile(tx,ty+4,3);
                graphics.drawtile(tx+8,ty+4,4);
                //6:
                tx+=tg;
                graphics.drawtile(tx,ty+4,24);
                graphics.drawtile(tx+8,ty+4,24);
                //7:
                tx+=tg;
                graphics.drawtile(tx,ty+4,1);
                graphics.drawtile(tx+8,ty+4,1);
                //8:
                tx+=tg;
                graphics.drawsprite(tx,ty,78+ed.entframe,196,196,196);
                //9:
                tx+=tg;
                FillRect(graphics.backBuffer, tx+2,ty+8,12,1,graphics.getRGB(255,255,255));

                for (int i = 0; i < 10; i++)
                {
                    fillboxabs(4+(i*tg), 209,20,20,graphics.getRGB(96,96,96));
                    const int col = i == ed.drawmode ? 255 : 164;
                    const std::string glyph = i == 9 ? "0" : help.String(i + 1);
                    graphics.Print(22 + i*tg - 4, 225 - 4, glyph, col, col, col, false);
                }

                fillboxabs(4+(ed.drawmode*tg), 209,20,20,graphics.getRGB(200,200,200));

                graphics.Print(4, 232, "1/2", 196, 196, 255 - help.glow, false);
            }
            else
            {
                for(int i=0; i<7; i++)
                {
                    FillRect(graphics.backBuffer, 4+(i*tg), 209,20,20,graphics.getRGB(32,32,32));
                }
                FillRect(graphics.backBuffer, 4+((ed.drawmode-10)*tg), 209,20,20,graphics.getRGB(64,64,64));
                //10:
                graphics.Print(tx,ty,"A",196, 196, 255 - help.glow, false);
                graphics.Print(tx+8,ty,"B",196, 196, 255 - help.glow, false);
                graphics.Print(tx,ty+8,"C",196, 196, 255 - help.glow, false);
                graphics.Print(tx+8,ty+8,"D",196, 196, 255 - help.glow, false);
                //11:
                tx+=tg;
                graphics.drawsprite(tx,ty,17,196,196,196);
                //12:
                tx+=tg;
                fillboxabs(tx+4,ty+4,8,8,graphics.getRGB(96,96,96));
                //13:
                tx+=tg;
                graphics.drawsprite(tx,ty,18+(ed.entframe%2),196,196,196);
                //14:
                tx+=tg;
                FillRect(graphics.backBuffer, tx+6,ty+2,4,12,graphics.getRGB(255,255,255));
                //15:
                tx+=tg;
                graphics.drawsprite(tx,ty,186,graphics.col_crewblue);
                //16:
                tx+=tg;
                graphics.drawsprite(tx,ty,184,graphics.col_crewcyan);

                for (int i = 0; i < 7; i++)
                {
                    fillboxabs(4 +  i*tg, 209, 20, 20, graphics.getRGB(96, 96, 96));
                    const int col = i + 10 == ed.drawmode ? 255 : 164;
                    static const char glyphs[] = "RTYUIOP";
                    graphics.Print(22 + i*tg - 4, 225 - 4, std::string(1, glyphs[i]), col, col, col, false);
                }

                fillboxabs(4 + (ed.drawmode - 10) * tg, 209, 20, 20, graphics.getRGB(200, 200, 200));

                graphics.Print(4, 232, "2/2", 196, 196, 255 - help.glow, false);
            }

            graphics.Print(128, 232, "< and > keys change tool", 196, 196, 255 - help.glow, false);

            FillRect(graphics.backBuffer, 0,198,120,10, graphics.getRGB(32,32,32));
            FillRect(graphics.backBuffer, 0,199,119,9, graphics.getRGB(0,0,0));
            switch(ed.drawmode)
            {
            case 0:
                graphics.bprint(2,199, "1: Walls",196, 196, 255 - help.glow);
                break;
            case 1:
                graphics.bprint(2,199, "2: Backing",196, 196, 255 - help.glow);
                break;
            case 2:
                graphics.bprint(2,199, "3: Spikes",196, 196, 255 - help.glow);
                break;
            case 3:
                graphics.bprint(2,199, "4: Trinkets",196, 196, 255 - help.glow);
                break;
            case 4:
                graphics.bprint(2,199, "5: Checkpoint",196, 196, 255 - help.glow);
                break;
            case 5:
                graphics.bprint(2,199, "6: Disappear",196, 196, 255 - help.glow);
                break;
            case 6:
                graphics.bprint(2,199, "7: Conveyors",196, 196, 255 - help.glow);
                break;
            case 7:
                graphics.bprint(2,199, "8: Moving",196, 196, 255 - help.glow);
                break;
            case 8:
                graphics.bprint(2,199, "9: Enemies",196, 196, 255 - help.glow);
                break;
            case 9:
                graphics.bprint(2,199, "0: Grav Line",196, 196, 255 - help.glow);
                break;
            case 10:
                graphics.bprint(2,199, "R: Roomtext",196, 196, 255 - help.glow);
                break;
            case 11:
                graphics.bprint(2,199, "T: Terminal",196, 196, 255 - help.glow);
                break;
            case 12:
                graphics.bprint(2,199, "Y: Script Box",196, 196, 255 - help.glow);
                break;
            case 13:
                graphics.bprint(2,199, "U: Warp Token",196, 196, 255 - help.glow);
                break;
            case 14:
                graphics.bprint(2,199, "I: Warp Lines",196, 196, 255 - help.glow);
                break;
            case 15:
                graphics.bprint(2,199, "O: Crewmate",196, 196, 255 - help.glow);
                break;
            case 16:
                graphics.bprint(2,199, "P: Start Point",196, 196, 255 - help.glow);
                break;
            }

            FillRect(graphics.backBuffer, 260,198,80,10, graphics.getRGB(32,32,32));
            FillRect(graphics.backBuffer, 261,199,80,9, graphics.getRGB(0,0,0));
            graphics.bprint(268,199, "("+help.String(ed.levx+1)+","+help.String(ed.levy+1)+")",196, 196, 255 - help.glow, false);

        }
        else
        {
            //FillRect(graphics.backBuffer, 0,230,72,240, graphics.RGB(32,32,32));
            //FillRect(graphics.backBuffer, 0,231,71,240, graphics.RGB(0,0,0));
            if(room->roomname!="")
            {
                if (graphics.translucentroomname)
                {
                    graphics.footerrect.y = 230+ed.roomnamehide;
                    SDL_BlitSurface(graphics.footerbuffer, NULL, graphics.backBuffer, &graphics.footerrect);
                }
                else
                {
                    FillRect(graphics.backBuffer, 0,230+ed.roomnamehide,320,10, graphics.getRGB(0,0,0));
                }
                graphics.bprint(5,231+ed.roomnamehide,room->roomname, 196, 196, 255 - help.glow, true);
                graphics.bprint(4, 222, "SPACE ^  SHIFT ^", 196, 196, 255 - help.glow, false);
                graphics.bprint(268,222, "("+help.String(ed.levx+1)+","+help.String(ed.levy+1)+")",196, 196, 255 - help.glow, false);
            }
            else
            {
                graphics.bprint(4, 232, "SPACE ^  SHIFT ^", 196, 196, 255 - help.glow, false);
                graphics.bprint(268,232, "("+help.String(ed.levx+1)+","+help.String(ed.levy+1)+")",196, 196, 255 - help.glow, false);
            }
        }

        if(ed.shiftmenu)
        {
            fillboxabs(0, 117,171+8,140,graphics.getRGB(64,64,64));
            FillRect(graphics.backBuffer, 0,118,170+8,140, graphics.getRGB(0,0,0));
            graphics.Print(4, 120, "F1: Change Tileset",164,164,164,false);
            graphics.Print(4, 130, "F2: Change Colour",164,164,164,false);
            graphics.Print(4, 140, "F3: Change Enemies",164,164,164,false);
            graphics.Print(4, 150, "F4: Enemy Bounds",164,164,164,false);
            graphics.Print(4, 160, "F5: Platform Bounds",164,164,164,false);

            graphics.Print(4, 180, "F9: Reload Resources",164,164,164,false);
            graphics.Print(4, 190, "F10: Direct Mode",164,164,164,false);

            graphics.Print(4, 210, "W: Change Warp Dir",164,164,164,false);
            graphics.Print(4, 220, "E: Change Roomname",164,164,164,false);

            fillboxabs(220, 207,100,60,graphics.getRGB(64,64,64));
            FillRect(graphics.backBuffer, 221,208,160,60, graphics.getRGB(0,0,0));
            graphics.Print(224, 210, "S: Save Map",164,164,164,false);
            graphics.Print(224, 220, "L: Load Map",164,164,164,false);
        }
    }


    if(!ed.settingsmod && !ed.scripteditmod)
    {
        //Same as above, without borders
        switch(ed.drawmode)
        {
        case 0:
            graphics.bprint(2,2, "1: Walls",196, 196, 255 - help.glow);
            break;
        case 1:
            graphics.bprint(2,2, "2: Backing",196, 196, 255 - help.glow);
            break;
        case 2:
            graphics.bprint(2,2, "3: Spikes",196, 196, 255 - help.glow);
            break;
        case 3:
            graphics.bprint(2,2, "4: Trinkets",196, 196, 255 - help.glow);
            break;
        case 4:
            graphics.bprint(2,2, "5: Checkpoint",196, 196, 255 - help.glow);
            break;
        case 5:
            graphics.bprint(2,2, "6: Disappear",196, 196, 255 - help.glow);
            break;
        case 6:
            graphics.bprint(2,2, "7: Conveyors",196, 196, 255 - help.glow);
            break;
        case 7:
            graphics.bprint(2,2, "8: Moving",196, 196, 255 - help.glow);
            break;
        case 8:
            graphics.bprint(2,2, "9: Enemies",196, 196, 255 - help.glow);
            break;
        case 9:
            graphics.bprint(2,2, "0: Grav Line",196, 196, 255 - help.glow);
            break;
        case 10:
            graphics.bprint(2,2, "R: Roomtext",196, 196, 255 - help.glow);
            break;
        case 11:
            graphics.bprint(2,2, "T: Terminal",196, 196, 255 - help.glow);
            break;
        case 12:
            graphics.bprint(2,2, "Y: Script Box",196, 196, 255 - help.glow);
            break;
        case 13:
            graphics.bprint(2,2, "U: Warp Token",196, 196, 255 - help.glow);
            break;
        case 14:
            graphics.bprint(2,2, "I: Warp Lines",196, 196, 255 - help.glow);
            break;
        case 15:
            graphics.bprint(2,2, "O: Crewmate",196, 196, 255 - help.glow);
            break;
        case 16:
            graphics.bprint(2,2, "P: Start Point",196, 196, 255 - help.glow);
            break;
        }
    }

    if(ed.notedelay>0 || ed.oldnotedelay>0)
    {
        float alpha = graphics.lerp(ed.oldnotedelay, ed.notedelay);
        FillRect(graphics.backBuffer, 0,115,320,18, graphics.getRGB(92,92,92));
        FillRect(graphics.backBuffer, 0,116,320,16, graphics.getRGB(0,0,0));
        graphics.Print(0,121, ed.note,196-((45.0f-alpha)*4), 196-((45.0f-alpha)*4), 196-((45.0f-alpha)*4), true);
    }

    graphics.drawfade();

    graphics.render();
}

void editorrenderfixed(void)
{
    extern editorclass ed;
    const RoomProperty* const room = cl.getroomprop(ed.levx, ed.levy);
    graphics.updatetitlecolours();

    game.customcol=cl.getlevelcol(room->tileset, room->tilecol)+1;
    ed.entcol=cl.getenemycol(game.customcol);

    graphics.setcol(ed.entcol);
    ed.entcolreal = graphics.ct.colour;

    if (game.ghostsenabled)
    {
        for (size_t i = 0; i < ed.ghosts.size(); i++)
        {
            GhostInfo& ghost = ed.ghosts[i];

            if ((int) i > ed.currentghosts || ghost.rx != ed.levx || ghost.ry != ed.levy)
            {
                continue;
            }

            graphics.setcol(ghost.col);
            ghost.realcol = graphics.ct.colour;
        }

        if (ed.currentghosts + 1 < (int)ed.ghosts.size()) {
            ed.currentghosts++;
            if (ed.zmod) ed.currentghosts++;
        } else {
            ed.currentghosts = (int)ed.ghosts.size() - 1;
        }
    }

    if (!ed.settingsmod)
    {
        switch(room->warpdir)
        {
        case 1:
            graphics.rcol=cl.getwarpbackground(ed.levx, ed.levy);
            graphics.updatebackground(3);
            break;
        case 2:
            graphics.rcol=cl.getwarpbackground(ed.levx, ed.levy);
            graphics.updatebackground(4);
            break;
        case 3:
            graphics.rcol=cl.getwarpbackground(ed.levx, ed.levy);
            graphics.updatebackground(5);
            break;
        default:
            break;
        }
    }
    else if (!game.colourblindmode)
    {
        graphics.updatetowerbackground(graphics.titlebg);
    }

    /* Correct gravity lines */
    for (size_t i = 0; i < customentities.size(); ++i)
    {
        if (customentities[i].x / 40 != ed.levx
        || customentities[i].y / 30 != ed.levy
        || customentities[i].t != 11
        /* Is the gravity line locked? */
        || customentities[i].p4 == 1)
        {
            continue;
        }

        if (customentities[i].p1 == 0)
        {
            /* Horizontal */
            int tx = customentities[i].x % 40;
            int tx2 = tx;
            int ty = customentities[i].y % 30;
            while (!ed.spikefree(tx, ty))
            {
                --tx;
            }
            while (!ed.spikefree(tx2, ty))
            {
                ++tx2;
            }
            ++tx;
            customentities[i].p2 = tx;
            customentities[i].p3 = (tx2 - tx) * 8;
        }
        else
        {
            /* Vertical */
            int tx = customentities[i].x % 40;
            int ty = customentities[i].y % 30;
            int ty2 = ty;
            /* Unlocked */
            while (!ed.spikefree(tx, ty))
            {
                --ty;
            }
            while (!ed.spikefree(tx, ty2))
            {
                ++ty2;
            }
            ++ty;
            customentities[i].p2 = ty;
            customentities[i].p3 = (ty2 - ty) * 8;
        }
    }

    if (cl.getroomprop(ed.levx, ed.levy)->directmode == 1)
    {
        if (ed.dmtileeditor > 0)
        {
            ed.dmtileeditor--;
        }
    }
    else
    {
        ed.dmtileeditor = 0;
    }

    if (cl.getroomprop(ed.levx, ed.levy)->roomname != "")
    {
        if (ed.tiley < 28)
        {
            if (ed.roomnamehide > 0)
            {
                ed.roomnamehide--;
            }
        }
        else
        {
            if (ed.roomnamehide < 12)
            {
                ed.roomnamehide++;
            }
        }
    }
    else
    {
        if (ed.tiley < 28)
        {
            ed.roomnamehide = 0;
        }
        else
        {
            ed.roomnamehide = 12;
        }
    }
}

void editorlogic(void)
{
    extern editorclass ed;
    //Misc
    help.updateglow();

    graphics.titlebg.bypos -= 2;
    graphics.titlebg.bscroll = -2;

    ed.entframedelay--;
    if(ed.entframedelay<=0)
    {
        ed.entframe=(ed.entframe+1)%4;
        ed.entframedelay=8;
    }

    ed.oldnotedelay = ed.notedelay;
    if(ed.notedelay>0)
    {
        ed.notedelay--;
    }

    if (graphics.fademode == 1)
    {
        //Return to game
        graphics.titlebg.colstate = 10;
        map.nexttowercolour();
        game.quittomenu();
        music.play(6); //should be before game.quittomenu()
        ed.settingsmod=false;
    }
}

static void creategameoptions(void)
{
    game.createmenu(Menu::options);
}

static void nextbgcolor(void)
{
    map.nexttowercolour();
}

static void editormenuactionpress(void)
{
    extern editorclass ed;
    switch (game.currentmenuname)
    {
    case Menu::ed_desc:
        switch (game.currentmenuoption)
        {
        case 0:
            ed.titlemod=true;
            key.enabletextentry();
            key.keybuffer=cl.title;
            break;
        case 1:
            ed.creatormod=true;
            key.enabletextentry();
            key.keybuffer=cl.creator;
            break;
        case 2:
            ed.desc1mod=true;
            key.enabletextentry();
            key.keybuffer=cl.Desc1;
            break;
        case 3:
            ed.websitemod=true;
            key.enabletextentry();
            key.keybuffer=cl.website;
            break;
        case 4:
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        music.playef(11);
        break;
    case Menu::ed_settings:
        switch (game.currentmenuoption)
        {
        case 0:
            //Change level description stuff
            music.playef(11);
            game.createmenu(Menu::ed_desc);
            map.nexttowercolour();
            break;
        case 1:
            //Enter script editormode
            music.playef(11);
            ed.scripteditmod=true;
            ed.clearscriptbuffer();
            key.keybuffer="";
            ed.hookmenupage=0;
            ed.hookmenu=0;
            ed.scripthelppage=0;
            ed.scripthelppagedelay=0;
            ed.sby=0;
            ed.sbx=0, ed.pagey=0;
            break;
        case 2:
            music.playef(11);
            game.createmenu(Menu::ed_music);
            map.nexttowercolour();
            if(cl.levmusic>0) music.play(cl.levmusic);
            break;
        case 3:
            music.playef(11);
            game.ghostsenabled = !game.ghostsenabled;
            break;
        case 4:
            //Load level
            ed.settingsmod=false;
            graphics.backgrounddrawn=false;
            map.nexttowercolour();

            ed.keydelay = 6;
            ed.getlin(TEXT_LOAD, "Enter map filename to load:", &(ed.filename));
            game.mapheld=true;
            graphics.backgrounddrawn=false;
            break;
        case 5:
            //Save level
            ed.settingsmod=false;
            map.nexttowercolour();

            ed.keydelay = 6;
            ed.getlin(TEXT_SAVE, "Enter map filename to save as:", &(ed.filename));
            game.mapheld=true;
            graphics.backgrounddrawn=false;
            break;
        case 6:
            /* Game options */
            music.playef(11);
            game.gamestate = TITLEMODE;
            game.ingame_titlemode = true;
            game.ingame_editormode = true;

            DEFER_CALLBACK(creategameoptions);
            DEFER_CALLBACK(nextbgcolor);
            break;
        default:
            music.playef(11);
            game.createmenu(Menu::ed_quit);
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::ed_music:
        switch (game.currentmenuoption)
        {
        case 0:
        case 1:
            switch (game.currentmenuoption)
            {
            case 0:
                cl.levmusic++;
                break;
            case 1:
                cl.levmusic--;
                break;
            }
            cl.levmusic = (cl.levmusic % 16 + 16) % 16;
            if(cl.levmusic>0)
            {
                music.play(cl.levmusic);
            }
            else
            {
                music.haltdasmusik();
            }
            music.playef(11);
            break;
        case 2:
            music.playef(11);
            music.fadeout();
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::ed_quit:
        switch (game.currentmenuoption)
        {
        case 0:
            //Saving and quit
            ed.saveandquit=true;

            ed.settingsmod=false;
            map.nexttowercolour();

            ed.keydelay = 6;
            ed.getlin(TEXT_SAVE, "Enter map filename to save as:", &(ed.filename));
            game.mapheld=true;
            graphics.backgrounddrawn=false;
            break;
        case 1:
            //Quit without saving
            music.playef(11);
            music.fadeout();
            graphics.fademode = 2;
            break;
        case 2:
            //Go back to editor
            music.playef(11);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    default:
        break;
    }
}

void editorinput(void)
{
    extern editorclass ed;
    if (graphics.fademode == 3 /* fading out */)
    {
        return;
    }
    game.mx = (float) key.mx;
    game.my = (float) key.my;
    ed.tilex=(game.mx - (game.mx%8))/8;
    ed.tiley=(game.my - (game.my%8))/8;
    if (graphics.screenbuffer->stretchMode == 1) {
        // In this mode specifically, we have to fix the mouse coordinates
        int winwidth, winheight;
        graphics.screenbuffer->GetWindowSize(&winwidth, &winheight);
        ed.tilex = ed.tilex * 320 / winwidth;
        ed.tiley = ed.tiley * 240 / winheight;
    }

    bool up_pressed = key.isDown(SDLK_UP) || key.isDown(SDL_CONTROLLER_BUTTON_DPAD_UP);
    bool down_pressed = key.isDown(SDLK_DOWN) || key.isDown(SDL_CONTROLLER_BUTTON_DPAD_DOWN);
    bool left_pressed = key.isDown(SDLK_LEFT) || key.isDown(SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    bool right_pressed = key.isDown(SDLK_RIGHT) || key.isDown(SDL_CONTROLLER_BUTTON_DPAD_RIGHT);

    game.press_left = false;
    game.press_right = false;
    game.press_action = false;
    game.press_map = false;
    game.press_interact = false;

    if (key.isDown(KEYBOARD_LEFT) || key.isDown(KEYBOARD_a) || key.controllerWantsLeft(false))
    {
        game.press_left = true;
    }
    if (key.isDown(KEYBOARD_RIGHT) || key.isDown(KEYBOARD_d) || key.controllerWantsRight(false))
    {
        game.press_right = true;
    }
    if (key.isDown(KEYBOARD_z) || key.isDown(KEYBOARD_SPACE) || key.isDown(KEYBOARD_v) || key.isDown(game.controllerButton_flip))
    {
        game.press_action = true;
    };

    if (key.keymap[SDLK_F9] && (ed.keydelay==0)) {
        ed.keydelay = 30;
        ed.note="Reloaded resources";
        ed.notedelay=45;
        graphics.reloadresources();
    }

    if (key.isDown(KEYBOARD_ENTER)) game.press_map = true;
    if (key.isDown(27) && !ed.settingskey)
    {
        ed.settingskey=true;
        if (ed.textmod)
        {
            key.disabletextentry();
            if (ed.textmod >= FIRST_ENTTEXT && ed.textmod <= LAST_ENTTEXT)
            {
                *ed.textptr = ed.oldenttext;
                if (ed.oldenttext == "")
                {
                    removeedentity(ed.textent);
                }
            }

            ed.textmod = TEXT_NONE;

            ed.shiftmenu = false;
            ed.shiftkey = false;
        }
        else if (key.textentry())
        {
            key.disabletextentry();
            ed.titlemod=false;
            ed.desc1mod=false;
            ed.desc2mod=false;
            ed.desc3mod=false;
            ed.websitemod=false;
            ed.creatormod=false;
            music.playef(11);

            ed.shiftmenu=false;
            ed.shiftkey=false;
        }
        else if(ed.boundarymod>0)
        {
            ed.boundarymod=0;
        }
        else
        {

            music.playef(11);
            if (ed.settingsmod)
            {
                if (ed.scripteditmod)
                {
                    ed.scripteditmod = false;
                }
                else if (ed.settingsmod)
                {
                    if (game.currentmenuname == Menu::ed_settings)
                    {
                        ed.settingsmod = false;
                    }
                    else
                    {
                        game.returnmenu();
                        map.nexttowercolour();
                    }
                }
            }
            else
            {
                ed.settingsmod = true;
            }
            graphics.backgrounddrawn=false;

            if (ed.settingsmod)
            {
                bool edsettings_in_stack = game.currentmenuname == Menu::ed_settings;
                if (!edsettings_in_stack)
                {
                    size_t i;
                    for (i = 0; i < game.menustack.size(); ++i)
                    {
                        if (game.menustack[i].name == Menu::ed_settings)
                        {
                            edsettings_in_stack = true;
                            break;
                        }
                    }
                }
                if (edsettings_in_stack)
                {
                    game.returntomenu(Menu::ed_settings);
                }
                else
                {
                    game.createmenu(Menu::ed_settings);
                }
                map.nexttowercolour();
            }
        }
    }

    if (!key.isDown(27))
    {
        ed.settingskey=false;
    }

    if(key.keymap[SDLK_LCTRL] || key.keymap[SDLK_RCTRL])
    {
        if(key.leftbutton) key.rightbutton=true;
    }

    if(ed.scripteditmod)
    {
        if(ed.scripthelppage==0)
        {
            //hook select menu
            if(ed.keydelay>0) ed.keydelay--;

            if(up_pressed && ed.keydelay<=0)
            {
                ed.keydelay=6;
                ed.hookmenu--;
            }

            if(down_pressed && ed.keydelay<=0)
            {
                ed.keydelay=6;
                ed.hookmenu++;
            }

            if(ed.hookmenu>=(int)ed.hooklist.size())
            {
                ed.hookmenu=ed.hooklist.size()-1;
            }
            if(ed.hookmenu<0) ed.hookmenu=0;

            if(ed.hookmenu<ed.hookmenupage)
            {
                ed.hookmenupage=ed.hookmenu;
            }

            if(ed.hookmenu>=ed.hookmenupage+9)
            {
                ed.hookmenupage=ed.hookmenu+8;
            }

            if(!key.keymap[SDLK_BACKSPACE]) ed.deletekeyheld=0;

            if(key.keymap[SDLK_BACKSPACE] && ed.deletekeyheld==0 && !ed.hooklist.empty())
            {
                ed.deletekeyheld=1;
                music.playef(2);
                ed.removehook(ed.hooklist[(ed.hooklist.size()-1)-ed.hookmenu]);
            }

            if (!game.press_action && !game.press_left && !game.press_right
                    && !up_pressed && !down_pressed && !key.isDown(27)) game.jumpheld = false;
            if (!game.jumpheld)
            {
                if (game.press_action || game.press_left || game.press_right || game.press_map
                        || up_pressed || down_pressed || key.isDown(27))
                {
                    game.jumpheld = true;
                }
                if ((game.press_action || game.press_map) && !ed.hooklist.empty())
                {
                    game.mapheld=true;
                    ed.scripthelppage=1;
                    key.enabletextentry();
                    key.keybuffer="";
                    ed.sbscript=ed.hooklist[(ed.hooklist.size()-1)-ed.hookmenu];
                    ed.loadhookineditor(ed.sbscript);

                    ed.sby=ed.sb.size()-1;
                    ed.pagey=0;
                    while(ed.sby>=20)
                    {
                        ed.pagey++;
                        ed.sby--;
                    }
                    key.keybuffer=ed.sb[ed.pagey+ed.sby];
                    ed.sbx = utf8::unchecked::distance(ed.sb[ed.pagey+ed.sby].begin(), ed.sb[ed.pagey+ed.sby].end());
                    music.playef(11);
                }
            }
        }
        else if(ed.scripthelppage==1)
        {
            //Script editor!
            if (key.isDown(27))
            {
                ed.scripthelppage=0;
                game.jumpheld = true;
                //save the script for use again!
                ed.addhook(ed.sbscript);
            }

            if(ed.keydelay>0) ed.keydelay--;

            if(up_pressed && ed.keydelay<=0)
            {
                ed.keydelay=6;
                ed.sby--;
                if(ed.sby<=5)
                {
                    if(ed.pagey>0)
                    {
                        ed.pagey--;
                        ed.sby++;
                    }
                    else
                    {
                        if(ed.sby<0) ed.sby=0;
                    }
                }
                key.keybuffer=ed.sb[ed.pagey+ed.sby];
            }

            if(down_pressed && ed.keydelay<=0)
            {
                ed.keydelay=6;
                if(ed.sby+ed.pagey<(int)ed.sb.size()-1)
                {
                    ed.sby++;
                    if(ed.sby>=20)
                    {
                        ed.pagey++;
                        ed.sby--;
                    }
                }
                key.keybuffer=ed.sb[ed.pagey+ed.sby];
            }

            if(key.linealreadyemptykludge)
            {
                ed.keydelay=6;
                key.linealreadyemptykludge=false;
            }

            if(key.pressedbackspace && ed.sb[ed.pagey+ed.sby]=="" && ed.keydelay<=0)
            {
                //Remove this line completely
                ed.removeline(ed.pagey+ed.sby);
                ed.sby--;
                if(ed.sby<=5)
                {
                    if(ed.pagey>0)
                    {
                        ed.pagey--;
                        ed.sby++;
                    }
                    else
                    {
                        if(ed.sby<0) ed.sby=0;
                    }
                }
                key.keybuffer=ed.sb[ed.pagey+ed.sby];
                ed.keydelay=6;
            }

            /* Remove all pipes, they are the line separator in the XML
             * When this loop reaches the end, it wraps to SIZE_MAX; SIZE_MAX + 1 is 0 */
            {size_t i; for (i = key.keybuffer.length() - 1; i + 1 > 0; --i)
            {
                if (key.keybuffer[i] == '|')
                {
                    key.keybuffer.erase(key.keybuffer.begin() + i);
                }
            }}

            ed.sb[ed.pagey+ed.sby]=key.keybuffer;
            ed.sbx = utf8::unchecked::distance(ed.sb[ed.pagey+ed.sby].begin(), ed.sb[ed.pagey+ed.sby].end());

            if(!game.press_map && !key.isDown(27)) game.mapheld=false;
            if (!game.mapheld)
            {
                if(game.press_map)
                {
                    game.mapheld=true;
                    //Continue to next line
                    if(ed.sby+ed.pagey>=(int)ed.sb.size()) //we're on the last line
                    {
                        ed.sby++;
                        if(ed.sby>=20)
                        {
                            ed.pagey++;
                            ed.sby--;
                        }
                        key.keybuffer=ed.sb[ed.pagey+ed.sby];
                        ed.sbx = utf8::unchecked::distance(ed.sb[ed.pagey+ed.sby].begin(), ed.sb[ed.pagey+ed.sby].end());
                    }
                    else
                    {
                        //We're not, insert a line instead
                        ed.sby++;
                        if(ed.sby>=20)
                        {
                            ed.pagey++;
                            ed.sby--;
                        }
                        ed.insertline(ed.sby+ed.pagey);
                        key.keybuffer="";
                        ed.sbx = 0;
                    }
                }
            }
        }
    }
    else if (ed.textmod)
    {
        *ed.textptr = key.keybuffer;

        if (!game.press_map && !key.isDown(27))
        {
            game.mapheld = false;
        }
        if (!game.mapheld && game.press_map)
        {
            game.mapheld = true;
            key.disabletextentry();
            switch (ed.textmod)
            {
            case TEXT_GOTOROOM:
            {
                char coord_x[16];
                char coord_y[16];

                const char* comma = SDL_strchr(key.keybuffer.c_str(), ',');

                bool valid_input = comma != NULL;

                if (valid_input)
                {
                    SDL_strlcpy(
                        coord_x,
                        key.keybuffer.c_str(),
                        VVV_min(comma - key.keybuffer.c_str() + 1, sizeof(coord_x))
                    );
                    SDL_strlcpy(coord_y, &comma[1], sizeof(coord_y));

                    valid_input = is_number(coord_x) && is_number(coord_y);
                }

                if (!valid_input)
                {
                    ed.note = "[ ERROR: Invalid format ]";
                    ed.notedelay = 45;
                    break;
                }

                ed.levx = clamp(help.Int(coord_x) - 1, 0, cl.mapwidth - 1);
                ed.levy = clamp(help.Int(coord_y) - 1, 0, cl.mapheight - 1);
                graphics.backgrounddrawn = false;
                break;
            }
            case TEXT_LOAD:
            {
                std::string loadstring = ed.filename + ".vvvvvv";
                if (cl.load(loadstring))
                {
                    // don't use filename, it has the full path
                    char buffer[64];
                    SDL_snprintf(buffer, sizeof(buffer), "[ Loaded map: %s.vvvvvv ]", ed.filename.c_str());
                    ed.note = buffer;
                }
                else
                {
                    ed.note = "[ ERROR: Could not load level ]";
                }
                ed.notedelay = 45;
                break;
            }
            case TEXT_SAVE:
            {
                std::string savestring = ed.filename + ".vvvvvv";
                if (cl.save(savestring))
                {
                    char buffer[64];
                    SDL_snprintf(buffer, sizeof(buffer), "[ Saved map: %s.vvvvvv ]", ed.filename.c_str());
                    ed.note = buffer;
                }
                else
                {
                    ed.note = "[ ERROR: Could not save level! ]";
                    ed.saveandquit = false;
                }
                ed.notedelay = 45;

                if (ed.saveandquit)
                {
                    graphics.fademode = 2; // quit editor
                }
                break;
            }
            case TEXT_SCRIPT:
                ed.clearscriptbuffer();
                if (!ed.checkhook(key.keybuffer))
                {
                    ed.addhook(key.keybuffer);
                }
                break;
            default:
                break;
            }

            ed.shiftmenu = false;
            ed.shiftkey = false;
            ed.textmod = TEXT_NONE;
        }
    }
    else if (key.textentry())
    {
        if(ed.titlemod)
        {
            cl.title=key.keybuffer;
        }
        else if(ed.creatormod)
        {
            cl.creator=key.keybuffer;
        }
        else if(ed.websitemod)
        {
            cl.website=key.keybuffer;
        }
        else if(ed.desc1mod)
        {
            cl.Desc1=key.keybuffer;
        }
        else if(ed.desc2mod)
        {
            cl.Desc2=key.keybuffer;
        }
        else if(ed.desc3mod)
        {
            cl.Desc3=key.keybuffer;
        }

        if(!game.press_map && !key.isDown(27)) game.mapheld=false;
        if (!game.mapheld)
        {
            if(game.press_map)
            {
                game.mapheld=true;
                if(ed.titlemod)
                {
                    cl.title=key.keybuffer;
                    ed.titlemod=false;
                }
                else if(ed.creatormod)
                {
                    cl.creator=key.keybuffer;
                    ed.creatormod=false;
                }
                else if(ed.websitemod)
                {
                    cl.website=key.keybuffer;
                    ed.websitemod=false;
                }
                else if(ed.desc1mod)
                {
                    cl.Desc1=key.keybuffer;
                }
                else if(ed.desc2mod)
                {
                    cl.Desc2=key.keybuffer;
                }
                else if(ed.desc3mod)
                {
                    cl.Desc3=key.keybuffer;
                    ed.desc3mod=false;
                }
                key.disabletextentry();

                if(ed.desc1mod)
                {
                    ed.desc1mod=false;

                    ed.desc2mod=true;
                    key.enabletextentry();
                    key.keybuffer=cl.Desc2;
                }
                else if(ed.desc2mod)
                {
                    ed.desc2mod=false;

                    ed.desc3mod=true;
                    key.enabletextentry();
                    key.keybuffer=cl.Desc3;
                }
                music.playef(11);
            }
        }
    }
    else
    {
        if(ed.settingsmod)
        {
            if (!game.press_action && !game.press_left && !game.press_right
                    && !up_pressed && !down_pressed) game.jumpheld = false;
            if (!game.jumpheld)
            {
                if (game.press_action || game.press_left || game.press_right || game.press_map
                        || up_pressed || down_pressed)
                {
                    game.jumpheld = true;
                }

                if(game.menustart)
                {
                    if (game.press_left || up_pressed)
                    {
                        game.currentmenuoption--;
                    }
                    else if (game.press_right || down_pressed)
                    {
                        game.currentmenuoption++;
                    }
                }

                if (game.currentmenuoption < 0) game.currentmenuoption = game.menuoptions.size()-1;
                if (game.currentmenuoption >= (int) game.menuoptions.size() ) game.currentmenuoption = 0;

                if (game.press_action)
                {
                    editormenuactionpress();
                }
            }
        }
        else if (ed.keydelay > 0)
        {
            ed.keydelay--;
        }
        else if (key.keymap[SDLK_LCTRL] || key.keymap[SDLK_RCTRL])
        {
            // Ctrl modifiers
            int numtiles;
            if (cl.getroomprop(ed.levx, ed.levy)->tileset == 0)
            {
                numtiles = (((int) graphics.tiles.size()) / 40) * 40;
            }
            else
            {
                numtiles = (((int) graphics.tiles2.size()) / 40) * 40;
            }
            ed.dmtileeditor=10;
            if(left_pressed)
            {
                ed.dmtile--;
                ed.keydelay=3;
                if(ed.dmtile<0) ed.dmtile+=numtiles;
            }
            else if(right_pressed)
            {
                ed.dmtile++;
                ed.keydelay=3;

                if(ed.dmtile>=numtiles) ed.dmtile-=numtiles;
            }
            if(up_pressed)
            {
                ed.dmtile-=40;
                ed.keydelay=3;
                if(ed.dmtile<0) ed.dmtile+=numtiles;
            }
            else if(down_pressed)
            {
                ed.dmtile+=40;
                ed.keydelay=3;

                if(ed.dmtile>=numtiles) ed.dmtile-=numtiles;
            }
        }
        else if (key.keymap[SDLK_LSHIFT] || key.keymap[SDLK_RSHIFT])
        {
            // Shift modifiers
            if (key.keymap[SDLK_F1])
            {
                ed.switch_tileset(true);
                graphics.backgrounddrawn = false;
                ed.keydelay = 6;
            }
            if (key.keymap[SDLK_F2])
            {
                ed.switch_tilecol(true);
                graphics.backgrounddrawn = false;
                ed.keydelay = 6;
            }
            if (key.keymap[SDLK_F3])
            {
                ed.switch_enemy(true);
                ed.keydelay=6;
            }
            if (key.keymap[SDLK_w])
            {
                ed.switch_warpdir(true);
                graphics.backgrounddrawn = false;
                ed.keydelay = 6;
            }

            if (up_pressed || down_pressed || left_pressed || right_pressed)
            {
                ed.keydelay=6;
                if(up_pressed)
                {
                    cl.mapheight--;
                }
                else if(down_pressed)
                {
                    cl.mapheight++;
                }
                else if(left_pressed)
                {
                    cl.mapwidth--;
                }
                else if(right_pressed)
                {
                    cl.mapwidth++;
                }

                if(cl.mapwidth<1) cl.mapwidth=1;
                if(cl.mapheight<1) cl.mapheight=1;
                if(cl.mapwidth>=cl.maxwidth) cl.mapwidth=cl.maxwidth;
                if(cl.mapheight>=cl.maxheight) cl.mapheight=cl.maxheight;
                ed.note = "Mapsize is now [" + help.String(cl.mapwidth) + "," + help.String(cl.mapheight) + "]";
                ed.notedelay=45;
            }

            if(!ed.shiftkey)
            {
                if(ed.shiftmenu)
                {
                    ed.shiftmenu=false;
                }
                else
                {
                    ed.shiftmenu=true;
                }
            }
            ed.shiftkey=true;
        }
        else
        {
            // No modifiers
            ed.shiftkey=false;
            if(key.keymap[SDLK_F1])
            {
                ed.switch_tileset(false);
                graphics.backgrounddrawn = false;
                ed.keydelay = 6;
            }
            if(key.keymap[SDLK_F2])
            {
                ed.switch_tilecol(false);
                graphics.backgrounddrawn = false;
                ed.keydelay = 6;
            }
            if(key.keymap[SDLK_F3])
            {
                ed.switch_enemy(false);
                ed.keydelay=6;
            }
            if(key.keymap[SDLK_F4])
            {
                ed.keydelay=6;
                ed.boundarytype=1;
                ed.boundarymod=1;
            }
            if(key.keymap[SDLK_F5])
            {
                ed.keydelay=6;
                ed.boundarytype=2;
                ed.boundarymod=1;
            }
            if(key.keymap[SDLK_F10])
            {
                if(cl.getroomprop(ed.levx, ed.levy)->directmode==1)
                {
                    cl.setroomdirectmode(ed.levx, ed.levy, 0);
                    ed.note="Direct Mode Disabled";
                    /* Kludge fix for rainbow BG here... */
                    if (cl.getroomprop(ed.levx, ed.levy)->tileset == 2
                    && cl.getroomprop(ed.levx, ed.levy)->tilecol == 6)
                    {
                        cl.setroomtilecol(ed.levx, ed.levy, 0);
                    }
                }
                else
                {
                    cl.setroomdirectmode(ed.levx, ed.levy, 1);
                    ed.note="Direct Mode Enabled";
                }
                graphics.backgrounddrawn=false;

                ed.notedelay=45;
                ed.updatetiles=true;
                ed.keydelay=6;
            }
            if(key.keymap[SDLK_1]) ed.drawmode=0;
            if(key.keymap[SDLK_2]) ed.drawmode=1;
            if(key.keymap[SDLK_3]) ed.drawmode=2;
            if(key.keymap[SDLK_4]) ed.drawmode=3;
            if(key.keymap[SDLK_5]) ed.drawmode=4;
            if(key.keymap[SDLK_6]) ed.drawmode=5;
            if(key.keymap[SDLK_7]) ed.drawmode=6;
            if(key.keymap[SDLK_8]) ed.drawmode=7;
            if(key.keymap[SDLK_9]) ed.drawmode=8;
            if(key.keymap[SDLK_0]) ed.drawmode=9;
            if(key.keymap[SDLK_r]) ed.drawmode=10;
            if(key.keymap[SDLK_t]) ed.drawmode=11;
            if(key.keymap[SDLK_y]) ed.drawmode=12;
            if(key.keymap[SDLK_u]) ed.drawmode=13;
            if(key.keymap[SDLK_i]) ed.drawmode=14;
            if(key.keymap[SDLK_o]) ed.drawmode=15;
            if(key.keymap[SDLK_p]) ed.drawmode=16;

            if(key.keymap[SDLK_w])
            {
                ed.switch_warpdir(false);
                graphics.backgrounddrawn = false;
                ed.keydelay = 6;
            }
            if(key.keymap[SDLK_e])
            {
                ed.keydelay = 6;
                ed.getlin(TEXT_ROOMNAME, "Enter new room name:", const_cast<std::string*>(&(cl.getroomprop(ed.levx, ed.levy)->roomname)));
                game.mapheld=true;
            }
            if (key.keymap[SDLK_g])
            {
                ed.keydelay = 6;
                ed.getlin(TEXT_GOTOROOM, "Enter room coordinates x,y:", NULL);
                game.mapheld=true;
            }

            //Save and load
            if(key.keymap[SDLK_s])
            {
                ed.keydelay = 6;
                ed.getlin(TEXT_SAVE, "Enter map filename to save map as:", &(ed.filename));
                game.mapheld=true;
            }

            if(key.keymap[SDLK_l])
            {
                ed.keydelay = 6;
                ed.getlin(TEXT_LOAD, "Enter map filename to load:", &(ed.filename));
                game.mapheld=true;
            }

            if(!game.press_map) game.mapheld=false;
            if (!game.mapheld)
            {
                if(game.press_map)
                {
                    game.mapheld=true;

                    //Ok! Scan the room for the closest checkpoint
                    int testeditor=-1;
                    int startpoint=0;
                    //First up; is there a start point on this screen?
                    for(size_t i=0; i<customentities.size(); i++)
                    {
                        //if() on screen
                        if(customentities[i].t==16 && testeditor==-1)
                        {
                            int tx=(customentities[i].x-(customentities[i].x%40))/40;
                            int ty=(customentities[i].y-(customentities[i].y%30))/30;
                            if(tx==ed.levx && ty==ed.levy)
                            {
                                testeditor=i;
                                startpoint=1;
                            }
                        }
                    }
                    if(testeditor==-1)
                    {
                        //Ok, settle for a check point
                        for(size_t i=0; i<customentities.size(); i++)
                        {
                            //if() on screen
                            if(customentities[i].t==10 && testeditor==-1)
                            {
                                int tx=(customentities[i].x-(customentities[i].x%40))/40;
                                int ty=(customentities[i].y-(customentities[i].y%30))/30;
                                if(tx==ed.levx && ty==ed.levy)
                                {
                                    testeditor=i;
                                }
                            }
                        }
                    }

                    if(testeditor==-1)
                    {
                        ed.note="ERROR: No checkpoint to spawn at";
                        ed.notedelay=45;
                    }
                    else
                    {
                        ed.currentghosts = 0;
                        if(startpoint==0)
                        {
                            //Checkpoint spawn
                            int tx=(customentities[testeditor].x-(customentities[testeditor].x%40))/40;
                            int ty=(customentities[testeditor].y-(customentities[testeditor].y%30))/30;
                            game.edsavex = (customentities[testeditor].x%40)*8 - 4;
                            game.edsavey = (customentities[testeditor].y%30)*8;
                            game.edsaverx = 100+tx;
                            game.edsavery = 100+ty;
                            if (customentities[testeditor].p1 == 0) // NOT a bool check!
                            {
                                game.edsavegc = 1;
                                game.edsavey -= 2;
                            }
                            else
                            {
                                game.edsavegc = 0;
                                game.edsavey -= 7;
                            }
                            game.edsavedir = 0;
                        }
                        else
                        {
                            //Start point spawn
                            int tx=(customentities[testeditor].x-(customentities[testeditor].x%40))/40;
                            int ty=(customentities[testeditor].y-(customentities[testeditor].y%30))/30;
                            game.edsavex = (customentities[testeditor].x%40)*8 - 4;
                            game.edsavey = (customentities[testeditor].y%30)*8;
                            game.edsaverx = 100+tx;
                            game.edsavery = 100+ty;
                            game.edsavegc = 0;
                            game.edsavey++;
                            game.edsavedir=1-customentities[testeditor].p1;
                        }

                        music.haltdasmusik();
                        graphics.backgrounddrawn=false;
                        ed.returneditoralpha = 1000; // Let's start it higher than 255 since it gets clamped
                        ed.oldreturneditoralpha = 1000;
                        script.startgamemode(21);
                    }
                }
            }

            ed.hmod = key.keymap[SDLK_h];
            ed.vmod = key.keymap[SDLK_v];
            ed.bmod = key.keymap[SDLK_b];
            ed.cmod = key.keymap[SDLK_c];
            ed.xmod = key.keymap[SDLK_x];
            ed.zmod = key.keymap[SDLK_z];

            if(key.keymap[SDLK_COMMA])
            {
                ed.drawmode--;
                ed.keydelay=6;
            }
            else if(key.keymap[SDLK_PERIOD])
            {
                ed.drawmode++;
                ed.keydelay=6;
            }

            if(ed.drawmode<0)
            {
                ed.drawmode=16;
                if(ed.spacemod) ed.spacemenu=0;
            }
            if(ed.drawmode>16) ed.drawmode=0;
            if(ed.drawmode>9)
            {
                if(ed.spacemod) ed.spacemenu=1;
            }
            else
            {
                if(ed.spacemod) ed.spacemenu=0;
            }

            if(up_pressed)
            {
                ed.keydelay=6;
                graphics.backgrounddrawn=false;
                ed.levy--;
                ed.updatetiles=true;
                ed.changeroom=true;
            }
            else if(down_pressed)
            {
                ed.keydelay=6;
                graphics.backgrounddrawn=false;
                ed.levy++;
                ed.updatetiles=true;
                ed.changeroom=true;
            }
            else if(left_pressed)
            {
                ed.keydelay=6;
                graphics.backgrounddrawn=false;
                ed.levx--;
                ed.updatetiles=true;
                ed.changeroom=true;
            }
            else if(right_pressed)
            {
                ed.keydelay=6;
                graphics.backgrounddrawn=false;
                ed.levx++;
                ed.updatetiles=true;
                ed.changeroom=true;
            }

            if(ed.levx<0) ed.levx+=cl.mapwidth;
            if(ed.levx>= cl.mapwidth) ed.levx-=cl.mapwidth;
            if(ed.levy<0) ed.levy+=cl.mapheight;
            if(ed.levy>=cl.mapheight) ed.levy-=cl.mapheight;
            if(key.keymap[SDLK_SPACE])
            {
                ed.spacemod = !ed.spacemod;
                ed.keydelay=6;
            }
        }

        if(!ed.settingsmod)
        {
            if(ed.boundarymod>0)
            {
                if(key.leftbutton)
                {
                    if(ed.lclickdelay==0)
                    {
                        if(ed.boundarymod==1)
                        {
                            ed.lclickdelay=1;
                            ed.boundx1=(ed.tilex*8);
                            ed.boundy1=(ed.tiley*8);
                            ed.boundarymod=2;
                        }
                        else if(ed.boundarymod==2)
                        {
                            if((ed.tilex*8)+8>=ed.boundx1 && (ed.tiley*8)+8>=ed.boundy1)
                            {
                                ed.boundx2=(ed.tilex*8)+8;
                                ed.boundy2=(ed.tiley*8)+8;
                            }
                            else
                            {
                                ed.boundx2=ed.boundx1+8;
                                ed.boundy2=ed.boundy1+8;
                            }
                            if(ed.boundarytype==0)
                            {
                                //Script trigger
                                ed.lclickdelay=1;
                                ed.textent=customentities.size();
                                addedentity((ed.boundx1/8)+(ed.levx*40),(ed.boundy1/8)+ (ed.levy*30),19,
                                            (ed.boundx2-ed.boundx1)/8, (ed.boundy2-ed.boundy1)/8);
                                ed.getlin(TEXT_SCRIPT, "Enter script name:", &(customentities[ed.textent].scriptname));
                                ed.lclickdelay=1;
                            }
                            else if(ed.boundarytype==1)
                            {
                                //Enemy bounds
                                cl.setroomenemyx1(ed.levx, ed.levy, ed.boundx1);
                                cl.setroomenemyy1(ed.levx, ed.levy, ed.boundy1);
                                cl.setroomenemyx2(ed.levx, ed.levy, ed.boundx2);
                                cl.setroomenemyy2(ed.levx, ed.levy, ed.boundy2);
                            }
                            else if(ed.boundarytype==2)
                            {
                                //Platform bounds
                                cl.setroomplatx1(ed.levx, ed.levy, ed.boundx1);
                                cl.setroomplaty1(ed.levx, ed.levy, ed.boundy1);
                                cl.setroomplatx2(ed.levx, ed.levy, ed.boundx2);
                                cl.setroomplaty2(ed.levx, ed.levy, ed.boundy2);
                            }
                            else if(ed.boundarytype==3)
                            {
                                //Copy
                            }
                            ed.boundarymod=0;
                            ed.lclickdelay=1;
                        }
                    }
                }
                else
                {
                    ed.lclickdelay=0;
                }
                if(key.rightbutton)
                {
                    ed.boundarymod=0;
                }
            }
            else if(ed.warpmod)
            {
                //Placing warp token
                if(key.leftbutton)
                {
                    if(ed.lclickdelay==0)
                    {
                        if(ed.free(ed.tilex, ed.tiley)==0)
                        {
                            customentities[ed.warpent].p1=ed.tilex+(ed.levx*40);
                            customentities[ed.warpent].p2=ed.tiley+(ed.levy*30);
                            ed.warpmod=false;
                            ed.warpent=-1;
                            ed.lclickdelay=1;
                        }
                    }
                }
                else
                {
                    ed.lclickdelay=0;
                }
                if(key.rightbutton)
                {
                    removeedentity(ed.warpent);
                    ed.warpmod=false;
                    ed.warpent=-1;
                }
            }
            else
            {
                //Mouse input
                if(key.leftbutton)
                {
                    if(ed.lclickdelay==0)
                    {
                        //Depending on current mode, place something
                        if(ed.drawmode==0)
                        {
                            //place tiles
                            //Are we in direct mode?
                            if(cl.getroomprop(ed.levx, ed.levy)->directmode>=1)
                            {
                                if(ed.bmod)
                                {
                                    for(int i=0; i<30; i++)
                                    {
                                        ed.placetilelocal(ed.tilex, i, ed.dmtile);
                                    }
                                }
                                else if(ed.hmod)
                                {
                                    for(int i=0; i<40; i++)
                                    {
                                        ed.placetilelocal(i, ed.tiley, ed.dmtile);
                                    }
                                }
                                else if(ed.vmod)
                                {
                                    for(int j=-4; j<5; j++)
                                    {
                                        for(int i=-4; i<5; i++)
                                        {
                                            ed.placetilelocal(ed.tilex+i, ed.tiley+j, ed.dmtile);
                                        }
                                    }
                                }
                                else if(ed.cmod)
                                {
                                    for(int j=-3; j<4; j++)
                                    {
                                        for(int i=-3; i<4; i++)
                                        {
                                            ed.placetilelocal(ed.tilex+i, ed.tiley+j, ed.dmtile);
                                        }
                                    }
                                }
                                else if(ed.xmod)
                                {
                                    for(int j=-2; j<3; j++)
                                    {
                                        for(int i=-2; i<3; i++)
                                        {
                                            ed.placetilelocal(ed.tilex+i, ed.tiley+j, ed.dmtile);
                                        }
                                    }
                                }
                                else if(ed.zmod)
                                {
                                    for(int j=-1; j<2; j++)
                                    {
                                        for(int i=-1; i<2; i++)
                                        {
                                            ed.placetilelocal(ed.tilex+i, ed.tiley+j, ed.dmtile);
                                        }
                                    }
                                }
                                else
                                {
                                    ed.placetilelocal(ed.tilex, ed.tiley, ed.dmtile);
                                }
                            }
                            else
                            {
                                if(ed.bmod)
                                {
                                    for(int i=0; i<30; i++)
                                    {
                                        ed.placetilelocal(ed.tilex, i, 80);
                                    }
                                }
                                else if(ed.hmod)
                                {
                                    for(int i=0; i<40; i++)
                                    {
                                        ed.placetilelocal(i, ed.tiley, 80);
                                    }
                                }
                                else if(ed.vmod)
                                {
                                    for(int j=-4; j<5; j++)
                                    {
                                        for(int i=-4; i<5; i++)
                                        {
                                            ed.placetilelocal(ed.tilex+i, ed.tiley+j, 80);
                                        }
                                    }
                                }
                                else if(ed.cmod)
                                {
                                    for(int j=-3; j<4; j++)
                                    {
                                        for(int i=-3; i<4; i++)
                                        {
                                            ed.placetilelocal(ed.tilex+i, ed.tiley+j, 80);
                                        }
                                    }
                                }
                                else if(ed.xmod)
                                {
                                    for(int j=-2; j<3; j++)
                                    {
                                        for(int i=-2; i<3; i++)
                                        {
                                            ed.placetilelocal(ed.tilex+i, ed.tiley+j, 80);
                                        }
                                    }
                                }
                                else if(ed.zmod)
                                {
                                    for(int j=-1; j<2; j++)
                                    {
                                        for(int i=-1; i<2; i++)
                                        {
                                            ed.placetilelocal(ed.tilex+i, ed.tiley+j, 80);
                                        }
                                    }
                                }
                                else
                                {
                                    ed.placetilelocal(ed.tilex, ed.tiley, 80);
                                }
                            }
                        }
                        else if(ed.drawmode==1)
                        {
                            //place background tiles
                            if(ed.bmod)
                            {
                                for(int i=0; i<30; i++)
                                {
                                    ed.placetilelocal(ed.tilex, i, 2);
                                }
                            }
                            else if(ed.hmod)
                            {
                                for(int i=0; i<40; i++)
                                {
                                    ed.placetilelocal(i, ed.tiley, 2);
                                }
                            }
                            else if(ed.vmod)
                            {
                                for(int j=-4; j<5; j++)
                                {
                                    for(int i=-4; i<5; i++)
                                    {
                                        ed.placetilelocal(ed.tilex+i, ed.tiley+j, 2);
                                    }
                                }
                            }
                            else if(ed.cmod)
                            {
                                for(int j=-3; j<4; j++)
                                {
                                    for(int i=-3; i<4; i++)
                                    {
                                        ed.placetilelocal(ed.tilex+i, ed.tiley+j, 2);
                                    }
                                }
                            }
                            else if(ed.xmod)
                            {
                                for(int j=-2; j<3; j++)
                                {
                                    for(int i=-2; i<3; i++)
                                    {
                                        ed.placetilelocal(ed.tilex+i, ed.tiley+j, 2);
                                    }
                                }
                            }
                            else if(ed.zmod)
                            {
                                for(int j=-1; j<2; j++)
                                {
                                    for(int i=-1; i<2; i++)
                                    {
                                        ed.placetilelocal(ed.tilex+i, ed.tiley+j, 2);
                                    }
                                }
                            }
                            else
                            {
                                ed.placetilelocal(ed.tilex, ed.tiley, 2);
                            }
                        }
                        else if(ed.drawmode==2)
                        {
                            //place spikes
                            ed.placetilelocal(ed.tilex, ed.tiley, 8);
                        }

                        int tmp=edentat(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30));
                        if(tmp==-1)
                        {
                            //Room text and script triggers can be placed in walls
                            if(ed.drawmode==10)
                            {
                                ed.lclickdelay=1;
                                ed.textent=customentities.size();
                                addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),17);
                                ed.getlin(TEXT_ROOMTEXT, "Enter roomtext:", &(customentities[ed.textent].scriptname));
                            }
                            else if(ed.drawmode==12)   //Script Trigger
                            {
                                ed.boundarytype=0;
                                ed.boundx1=ed.tilex*8;
                                ed.boundy1=ed.tiley*8;
                                ed.boundarymod=2;
                                ed.lclickdelay=1;
                            }
                        }
                        if(tmp==-1 && ed.free(ed.tilex,ed.tiley)==0)
                        {
                            if(ed.drawmode==3)
                            {
                                if(cl.numtrinkets()<100)
                                {
                                    addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),9);
                                    ed.lclickdelay=1;
                                }
                                else
                                {
                                    ed.note="ERROR: Max number of trinkets is 100";
                                    ed.notedelay=45;
                                }
                            }
                            else if(ed.drawmode==4)
                            {
                                addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),10, 1);
                                ed.lclickdelay=1;
                            }
                            else if(ed.drawmode==5)
                            {
                                addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),3);
                                ed.lclickdelay=1;
                            }
                            else if(ed.drawmode==6)
                            {
                                addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),2,5);
                                ed.lclickdelay=1;
                            }
                            else if(ed.drawmode==7)
                            {
                                addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),2,0);
                                ed.lclickdelay=1;
                            }
                            else if(ed.drawmode==8)
                            {
                                addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),1,0);
                                ed.lclickdelay=1;
                            }
                            else if(ed.drawmode==9)
                            {
                                addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),11,0);
                                ed.lclickdelay=1;
                            }
                            else if(ed.drawmode==11)
                            {
                                ed.lclickdelay=1;
                                ed.textent=customentities.size();
                                addedentity(ed.tilex+(ed.levx*40),ed.tiley+ (ed.levy*30),18,0);
                                ed.getlin(TEXT_SCRIPT, "Enter script name:", &(customentities[ed.textent].scriptname));
                            }
                            else if(ed.drawmode==13)
                            {
                                ed.warpmod=true;
                                ed.warpent=customentities.size();
                                addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),13);
                                ed.lclickdelay=1;
                            }
                            else if(ed.drawmode==14)
                            {
                                //Warp lines
                                if(ed.tilex==0)
                                {
                                    addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),50,0);
                                }
                                else if(ed.tilex==39)
                                {
                                    addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),50,1);
                                }
                                else if(ed.tiley==0)
                                {
                                    addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),50,2);
                                }
                                else if(ed.tiley==29)
                                {
                                    addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),50,3);
                                }
                                else
                                {
                                    ed.note="ERROR: Warp lines must be on edges";
                                    ed.notedelay=45;
                                }
                                ed.lclickdelay=1;
                            }
                            else if(ed.drawmode==15)  //Crewmate
                            {
                                if(cl.numcrewmates()<100)
                                {
                                    addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),15,int(fRandom() * 6));
                                    ed.lclickdelay=1;
                                }
                                else
                                {
                                    ed.note="ERROR: Max number of crewmates is 100";
                                    ed.notedelay=45;
                                }
                            }
                            else if(ed.drawmode==16)  //Start Point
                            {
                                //If there is another start point, destroy it
                                for(size_t i=0; i<customentities.size(); i++)
                                {
                                    if(customentities[i].t==16)
                                    {
                                        removeedentity(i);
                                        i--;
                                    }
                                }
                                addedentity(ed.tilex+ (ed.levx*40),ed.tiley+ (ed.levy*30),16,0);
                                ed.lclickdelay=1;
                            }
                        }
                        else if(tmp == -1)
                        {
                            //Important! Do nothing, or else Undefined Behavior will happen
                        }
                        else if(customentities[tmp].t==1)
                        {
                            customentities[tmp].p1=(customentities[tmp].p1+1)%4;
                            ed.lclickdelay=1;
                        }
                        else if(customentities[tmp].t==2)
                        {
                            if(customentities[tmp].p1>=5)
                            {
                                customentities[tmp].p1=(customentities[tmp].p1+1)%9;
                                if(customentities[tmp].p1<5) customentities[tmp].p1=5;
                            }
                            else
                            {
                                customentities[tmp].p1=(customentities[tmp].p1+1)%4;
                            }
                            ed.lclickdelay=1;
                        }
                        else if(customentities[tmp].t==10)
                        {
                            // If it's not textured as a checkpoint, leave it alone
                            if (customentities[tmp].p1 == 0 || customentities[tmp].p1 == 1)
                            {
                                customentities[tmp].p1=(customentities[tmp].p1+1)%2;
                            }
                            ed.lclickdelay=1;
                        }
                        else if(customentities[tmp].t==11)
                        {
                            customentities[tmp].p1=(customentities[tmp].p1+1)%2;
                            ed.lclickdelay=1;
                        }
                        else if(customentities[tmp].t==15)
                        {
                            customentities[tmp].p1=(customentities[tmp].p1+1)%6;
                            ed.lclickdelay=1;
                        }
                        else if(customentities[tmp].t==16)
                        {
                            customentities[tmp].p1=(customentities[tmp].p1+1)%2;
                            ed.lclickdelay=1;
                        }
                        else if(customentities[tmp].t==17)
                        {
                            ed.getlin(TEXT_ROOMTEXT, "Enter roomtext:", &(customentities[tmp].scriptname));
                            ed.textent=tmp;
                            ed.lclickdelay=1;
                        }
                        else if(customentities[tmp].t==18 || customentities[tmp].t==19)
                        {
                            ed.lclickdelay=1;
                            ed.textent=tmp;
                            ed.getlin(TEXT_SCRIPT, "Enter script name:", &(customentities[ed.textent].scriptname));
                            if (customentities[tmp].t == 18
                            && (customentities[tmp].p1 == 0 || customentities[tmp].p1 == 1))
                            {
                                // Flip the terminal, but if it's not textured as a terminal leave it alone
                                customentities[tmp].p1 = (customentities[tmp].p1 + 1) % 2;
                            }
                        }
                    }
                }
                else
                {
                    ed.lclickdelay=0;
                }

                if(key.rightbutton)
                {
                    //place tiles
                    if(ed.drawmode < 2) {
                        if(ed.bmod)
                        {
                            for(int i=0; i<30; i++)
                            {
                                ed.placetilelocal(ed.tilex, i, 0);
                            }
                        }
                        else if(ed.hmod)
                        {
                            for(int i=0; i<40; i++)
                            {
                                ed.placetilelocal(i, ed.tiley, 0);
                            }
                        }
                        else if(ed.vmod)
                        {
                            for(int j=-4; j<5; j++)
                            {
                                for(int i=-4; i<5; i++)
                                {
                                    ed.placetilelocal(ed.tilex+i, ed.tiley+j, 0);
                                }
                            }
                        }
                        else if(ed.cmod)
                        {
                            for(int j=-3; j<4; j++)
                            {
                                for(int i=-3; i<4; i++)
                                {
                                    ed.placetilelocal(ed.tilex+i, ed.tiley+j, 0);
                                }
                            }
                        }
                        else if(ed.xmod)
                        {
                            for(int j=-2; j<3; j++)
                            {
                                for(int i=-2; i<3; i++)
                                {
                                    ed.placetilelocal(ed.tilex+i, ed.tiley+j, 0);
                                }
                            }
                        }
                        else if(ed.zmod)
                        {
                            for(int j=-1; j<2; j++)
                            {
                                for(int i=-1; i<2; i++)
                                {
                                    ed.placetilelocal(ed.tilex+i, ed.tiley+j, 0);
                                }
                            }
                        }
                        else
                        {
                            ed.placetilelocal(ed.tilex, ed.tiley, 0);
                        }
                    }
                    else
                    {
                        ed.placetilelocal(ed.tilex, ed.tiley, 0);
                    }
                    for(size_t i=0; i<customentities.size(); i++)
                    {
                        if(customentities[i].x==ed.tilex + (ed.levx*40)&& customentities[i].y==ed.tiley+ (ed.levy*30))
                        {
                            removeedentity(i);
                        }
                    }
                }

                if(key.middlebutton)
                {
                    ed.dmtile=cl.gettile(ed.levx, ed.levy, ed.tilex, ed.tiley);
                }
            }
        }
    }

    if(ed.updatetiles && cl.getroomprop(ed.levx, ed.levy)->directmode==0)
    {
        ed.updatetiles=false;
        //Correctly set the tiles in the current room
        switch(cl.getroomprop(ed.levx, ed.levy)->tileset)
        {
        case 0: //The Space Station
            for(int j=0; j<30; j++)
            {
                for(int i=0; i<40; i++)
                {
                    int temp=cl.gettile(ed.levx, ed.levy, i, j);
                    if(temp>=3 && temp<80)
                    {
                        //Fix spikes
                        cl.settile(ed.levx, ed.levy, i, j, ed.spikedir(i, j));
                    }
                    else if(temp==2 || temp>=680)
                    {
                        //Fix background
                        cl.settile(
                            ed.levx,
                            ed.levy,
                            i,
                            j,
                            ed.backedgetile(i, j) + ed.backbase(ed.levx, ed.levy)
                        );
                    }
                    else if(temp>0)
                    {
                        //Fix tiles
                        cl.settile(
                            ed.levx,
                            ed.levy,
                            i,
                            j,
                            ed.edgetile(i, j) + ed.base(ed.levx, ed.levy)
                        );
                    }
                }
            }
            break;
        case 1: //Outside
            for(int j=0; j<30; j++)
            {
                for(int i=0; i<40; i++)
                {
                    int temp=cl.gettile(ed.levx, ed.levy, i, j);
                    if(temp>=3 && temp<80)
                    {
                        //Fix spikes
                        cl.settile(ed.levx, ed.levy, i, j, ed.spikedir(i, j));
                    }
                    else if(temp==2 || temp>=680)
                    {
                        //Fix background
                        cl.settile(
                            ed.levx,
                            ed.levy,
                            i,
                            j,
                            ed.outsideedgetile(i, j) + ed.backbase(ed.levx, ed.levy)
                        );
                    }
                    else if(temp>0)
                    {
                        //Fix tiles
                        cl.settile(
                            ed.levx,
                            ed.levy,
                            i,
                            j,
                            ed.edgetile(i, j) + ed.base(ed.levx, ed.levy)
                        );
                    }
                }
            }
            break;
        case 2: //Lab
            for(int j=0; j<30; j++)
            {
                for(int i=0; i<40; i++)
                {
                    int temp=cl.gettile(ed.levx, ed.levy, i, j);
                    if(temp>=3 && temp<80)
                    {
                        //Fix spikes
                        cl.settile(
                            ed.levx,
                            ed.levy,
                            i,
                            j,
                            ed.labspikedir(
                                i,
                                j,
                                cl.getroomprop(ed.levx, ed.levy)->tilecol
                            )
                        );
                    }
                    else if(temp==2 || temp>=680)
                    {
                        //Fix background
                        cl.settile(ed.levx, ed.levy, i, j, 713);
                    }
                    else if(temp>0)
                    {
                        //Fix tiles
                        cl.settile(
                            ed.levx,
                            ed.levy,
                            i,
                            j,
                            ed.edgetile(i, j) + ed.base(ed.levx, ed.levy)
                        );
                    }
                }
            }
            break;
        case 3: //Warp Zone/Intermission
            for(int j=0; j<30; j++)
            {
                for(int i=0; i<40; i++)
                {
                    int temp=cl.gettile(ed.levx, ed.levy, i, j);
                    if(temp>=3 && temp<80)
                    {
                        //Fix spikes
                        cl.settile(ed.levx, ed.levy, i, j, ed.spikedir(i, j));
                    }
                    else if(temp==2 || temp>=680)
                    {
                        //Fix background
                        cl.settile(ed.levx, ed.levy, i, j, 713);
                    }
                    else if(temp>0)
                    {
                        //Fix tiles
                        cl.settile(
                            ed.levx,
                            ed.levy,
                            i,
                            j,
                            ed.edgetile(i, j) + ed.base(ed.levx, ed.levy)
                        );
                    }
                }
            }
            break;
        case 4: //The ship
            for(int j=0; j<30; j++)
            {
                for(int i=0; i<40; i++)
                {
                    int temp=cl.gettile(ed.levx, ed.levy, i, j);
                    if(temp>=3 && temp<80)
                    {
                        //Fix spikes
                        cl.settile(ed.levx, ed.levy, i, j, ed.spikedir(i, j));
                    }
                    else if(temp==2 || temp>=680)
                    {
                        //Fix background
                        cl.settile(
                            ed.levx,
                            ed.levy,
                            i,
                            j,
                            ed.backedgetile(i, j) + ed.backbase(ed.levx, ed.levy)
                        );
                    }
                    else if(temp>0)
                    {
                        //Fix tiles
                        cl.settile(
                            ed.levx,
                            ed.levy,
                            i,
                            j,
                            ed.edgetile(i, j) + ed.base(ed.levx, ed.levy)
                        );
                    }
                }
            }
            break;
        case 5: //The Tower
            break;
        case 6: //Custom Set 1
            break;
        case 7: //Custom Set 2
            break;
        case 8: //Custom Set 3
            break;
        case 9: //Custom Set 4
            break;
        }
    }
}

int editorclass::getenemyframe(int t)
{
    switch(t)
    {
    case 0:
        return 78;
        break;
    case 1:
        return 88;
        break;
    case 2:
        return 36;
        break;
    case 3:
        return 164;
        break;
    case 4:
        return 68;
        break;
    case 5:
        return 48;
        break;
    case 6:
        return 176;
        break;
    case 7:
        return 168;
        break;
    case 8:
        return 112;
        break;
    case 9:
        return 114;
        break;
    default:
        return 78;
        break;
    }
}

void editorclass::placetilelocal( int x, int y, int t )
{
    if(x>=0 && y>=0 && x<40 && y<30)
    {
        cl.settile(levx, levy, x, y, t);
    }
    updatetiles=true;
}

int editorclass::base( int x, int y )
{
    //Return the base tile for the given tileset and colour
    const RoomProperty* const room = cl.getroomprop(x, y);
    if(room->tileset==0)  //Space Station
    {
        if(room->tilecol>=22)
        {
            return 483 + ((room->tilecol-22)*3);
        }
        else if(room->tilecol>=11)
        {
            return 283 + ((room->tilecol-11)*3);
        }
        else
        {
            return 83 + (room->tilecol*3);
        }
    }
    else if(room->tileset==1)   //Outside
    {
        return 480 + (room->tilecol*3);
    }
    else if(room->tileset==2)   //Lab
    {
        return 280 + (room->tilecol*3);
    }
    else if(room->tileset==3)   //Warp Zone/Intermission
    {
        return 80 + (room->tilecol*3);
    }
    else if(room->tileset==4)   //SHIP
    {
        return 101 + (room->tilecol*3);
    }
    return 0;
}

int editorclass::backbase( int x, int y )
{
    //Return the base tile for the background of the given tileset and colour
    const RoomProperty* const room = cl.getroomprop(x, y);
    if(room->tileset==0)  //Space Station
    {
        //Pick depending on tilecol
        switch(room->tilecol)
        {
        case 0:
        case 5:
        case 26:
            return 680; //Blue
            break;
        case 3:
        case 16:
        case 23:
            return 683; //Yellow
            break;
        case 9:
        case 12:
        case 21:
            return 686; //Greeny Cyan
            break;
        case 4:
        case 8:
        case 24:
        case 28:
        case 30:
            return 689; //Green
            break;
        case 20:
        case 29:
            return 692; //Orange
            break;
        case 2:
        case 6:
        case 11:
        case 22:
        case 27:
            return 695; //Red
            break;
        case 1:
        case 10:
        case 15:
        case 19:
        case 31:
            return 698; //Pink
            break;
        case 14:
        case 18:
            return 701; //Dark Blue
            break;
        case 7:
        case 13:
        case 17:
        case 25:
            return 704; //Cyan
            break;
        default:
            return 680;
            break;
        }

    }
    else if(room->tileset==1)   //outside
    {
        return 680 + (room->tilecol*3);
    }
    else if(room->tileset==2)   //Lab
    {
        return 0;
    }
    else if(room->tileset==3)   //Warp Zone/Intermission
    {
        return 120 + (room->tilecol*3);
    }
    else if(room->tileset==4)   //SHIP
    {
        return 741 + (room->tilecol*3);
    }
    return 0;
}

int editorclass::at( int x, int y )
{
    if(x<0) return at(0,y);
    if(y<0) return at(x,0);
    if(x>=40) return at(39,y);
    if(y>=30) return at(x,29);

    if(x>=0 && y>=0 && x<40 && y<30)
    {
        return cl.gettile(levx, levy, x, y);
    }
    return 0;
}

int editorclass::freewrap( int x, int y )
{
    if(x<0) return freewrap(x+(cl.mapwidth*40),y);
    if(y<0) return freewrap(x,y+(cl.mapheight*30));
    if(x>=(cl.mapwidth*40)) return freewrap(x-(cl.mapwidth*40),y);
    if(y>=(cl.mapheight*30)) return freewrap(x,y-(cl.mapheight*30));

    if(x>=0 && y>=0 && x<(cl.mapwidth*40) && y<(cl.mapheight*30))
    {
        if(cl.getabstile(x, y)==0)
        {
            return 0;
        }
        else
        {
            if(cl.getabstile(x, y)>=2 && cl.getabstile(x, y)<80)
            {
                return 0;
            }
            if(cl.getabstile(x, y)>=680)
            {
                return 0;
            }
        }
    }
    return 1;
}

int editorclass::backonlyfree( int x, int y )
{
    //Returns 1 if tile is a background tile, 0 otherwise
    if(x<0) return backonlyfree(0,y);
    if(y<0) return backonlyfree(x,0);
    if(x>=40) return backonlyfree(39,y);
    if(y>=30) return backonlyfree(x,29);

    if(x>=0 && y>=0 && x<40 && y<30)
    {
        if(cl.gettile(levx, levy, x, y)>=680)
        {
            return 1;
        }
    }
    return 0;
}

int editorclass::backfree( int x, int y )
{
    //Returns 0 if tile is not a block or background tile, 1 otherwise
    if(x<0) return backfree(0,y);
    if(y<0) return backfree(x,0);
    if(x>=40) return backfree(39,y);
    if(y>=30) return backfree(x,29);

    if(x>=0 && y>=0 && x<40 && y<30)
    {
        if(cl.gettile(levx, levy, x, y)==0)
        {
            return 0;
        }
    }
    return 1;
}

int editorclass::spikefree( int x, int y )
{
    //Returns 0 if tile is not a block or spike, 1 otherwise
    if(x==-1) return free(0,y);
    if(y==-1) return free(x,0);
    if(x==40) return free(39,y);
    if(y==30) return free(x,29);

    if(x>=0 && y>=0 && x<40 && y<30)
    {
        if(cl.gettile(levx, levy, x, y)==0)
        {
            return 0;
        }
        else
        {
            if(cl.gettile(levx, levy, x, y)>=680)
            {
                return 0;
            }
        }
    }
    return 1;
}

int editorclass::free( int x, int y )
{
    //Returns 0 if tile is not a block, 1 otherwise
    if(x==-1) return free(0,y);
    if(y==-1) return free(x,0);
    if(x==40) return free(39,y);
    if(y==30) return free(x,29);

    if(x>=0 && y>=0 && x<40 && y<30)
    {
        if(cl.gettile(levx, levy, x, y)==0)
        {
            return 0;
        }
        else
        {
            if(cl.gettile(levx, levy, x, y)>=2 && cl.gettile(levx, levy, x, y)<80)
            {
                return 0;
            }
            if(cl.gettile(levx, levy, x, y)>=680)
            {
                return 0;
            }
        }
    }
    return 1;
}

int editorclass::match( int x, int y )
{
    if(free(x-1,y)==0 && free(x,y-1)==0 && free(x+1,y)==0 && free(x,y+1)==0) return 0;

    if(free(x-1,y)==0 && free(x,y-1)==0) return 10;
    if(free(x+1,y)==0 && free(x,y-1)==0) return 11;
    if(free(x-1,y)==0 && free(x,y+1)==0) return 12;
    if(free(x+1,y)==0 && free(x,y+1)==0) return 13;

    if(free(x,y-1)==0) return 1;
    if(free(x-1,y)==0) return 2;
    if(free(x,y+1)==0) return 3;
    if(free(x+1,y)==0) return 4;
    if(free(x-1,y-1)==0) return 5;
    if(free(x+1,y-1)==0) return 6;
    if(free(x-1,y+1)==0) return 7;
    if(free(x+1,y+1)==0) return 8;

    return 0;
}

int editorclass::outsidematch( int x, int y )
{

    if(backonlyfree(x-1,y)==0 && backonlyfree(x+1,y)==0) return 2;
    if(backonlyfree(x,y-1)==0 && backonlyfree(x,y+1)==0) return 1;

    return 0;
}

int editorclass::backmatch( int x, int y )
{
    //Returns the first position match for a border
    // 5 1 6
    // 2 X 4
    // 7 3 8
    if(backfree(x-1,y)==0 && backfree(x,y-1)==0 && backfree(x+1,y)==0 && backfree(x,y+1)==0) return 0;

    if(backfree(x-1,y)==0 && backfree(x,y-1)==0) return 10;
    if(backfree(x+1,y)==0 && backfree(x,y-1)==0) return 11;
    if(backfree(x-1,y)==0 && backfree(x,y+1)==0) return 12;
    if(backfree(x+1,y)==0 && backfree(x,y+1)==0) return 13;

    if(backfree(x,y-1)==0) return 1;
    if(backfree(x-1,y)==0) return 2;
    if(backfree(x,y+1)==0) return 3;
    if(backfree(x+1,y)==0) return 4;
    if(backfree(x-1,y-1)==0) return 5;
    if(backfree(x+1,y-1)==0) return 6;
    if(backfree(x-1,y+1)==0) return 7;
    if(backfree(x+1,y+1)==0) return 8;

    return 0;
}

int editorclass::edgetile( int x, int y )
{
    switch(match(x,y))
    {
    case 14:
        return 0;
        break;
    case 10:
        return 80;
        break;
    case 11:
        return 82;
        break;
    case 12:
        return 160;
        break;
    case 13:
        return 162;
        break;
    case 1:
        return 81;
        break;
    case 2:
        return 120;
        break;
    case 3:
        return 161;
        break;
    case 4:
        return 122;
        break;
    case 5:
        return 42;
        break;
    case 6:
        return 41;
        break;
    case 7:
        return 2;
        break;
    case 8:
        return 1;
        break;
    case 0:
    default:
        return 0;
        break;
    }
}

int editorclass::outsideedgetile( int x, int y )
{
    switch(outsidematch(x,y))
    {
    case 2:
        return 0;
        break;
    case 1:
        return 1;
        break;
    case 0:
    default:
        return 2;
        break;
    }
}


int editorclass::backedgetile( int x, int y )
{
    switch(backmatch(x,y))
    {
    case 14:
        return 0;
        break;
    case 10:
        return 80;
        break;
    case 11:
        return 82;
        break;
    case 12:
        return 160;
        break;
    case 13:
        return 162;
        break;
    case 1:
        return 81;
        break;
    case 2:
        return 120;
        break;
    case 3:
        return 161;
        break;
    case 4:
        return 122;
        break;
    case 5:
        return 42;
        break;
    case 6:
        return 41;
        break;
    case 7:
        return 2;
        break;
    case 8:
        return 1;
        break;
    case 0:
    default:
        return 0;
        break;
    }
}

int editorclass::labspikedir( int x, int y, int t )
{
    // a slightly more tricky case
    if(free(x,y+1)==1) return 63 + (t*2);
    if(free(x,y-1)==1) return 64 + (t*2);
    if(free(x-1,y)==1) return 51 + (t*2);
    if(free(x+1,y)==1) return 52 + (t*2);
    return 63 + (t*2);
}

int editorclass::spikedir( int x, int y )
{
    if(free(x,y+1)==1) return 8;
    if(free(x,y-1)==1) return 9;
    if(free(x-1,y)==1) return 49;
    if(free(x+1,y)==1) return 50;
    return 8;
}

void editorclass::switch_tileset(const bool reversed)
{
    const char* tilesets[] = {"Space Station", "Outside", "Lab", "Warp Zone", "Ship"};

    int tiles = cl.getroomprop(levx, levy)->tileset;

    if (reversed)
    {
        tiles--;
    }
    else
    {
        tiles++;
    }

    const int modulus = SDL_arraysize(tilesets);
    tiles = (tiles % modulus + modulus) % modulus;
    cl.setroomtileset(levx, levy, tiles);

    clamp_tilecol(levx, levy, false);

    char buffer[64];
    SDL_snprintf(buffer, sizeof(buffer), "Now using %s Tileset", tilesets[tiles]);

    note = buffer;
    notedelay = 45;
    updatetiles = true;
}

void editorclass::switch_tilecol(const bool reversed)
{
    int tilecol = cl.getroomprop(levx, levy)->tilecol;

    if (reversed)
    {
        tilecol--;
    }
    else
    {
        tilecol++;
    }

    cl.setroomtilecol(levx, levy, tilecol);

    clamp_tilecol(levx, levy, true);

    notedelay = 45;
    note = "Tileset Colour Changed";
    updatetiles = true;
}

void editorclass::clamp_tilecol(const int rx, const int ry, const bool wrap)
{
    const RoomProperty* const room = cl.getroomprop(rx, ry);
    const int tileset = room->tileset;
    int tilecol = room->tilecol;

    int mincol = -1;
    int maxcol = 5;

    // Only Space Station allows tileset -1
    if (tileset != 0)
    {
        mincol = 0;
    }

    switch (tileset)
    {
    case 0:
        maxcol = 31;
        break;
    case 1:
        maxcol = 7;
        break;
    case 3:
        maxcol = 6;
        break;
    case 5:
        maxcol = 29;
        break;
    }

    // If wrap is true, wrap-around, otherwise just cap
    if (tilecol > maxcol)
    {
        tilecol = (wrap ? mincol : maxcol);
    }
    if (tilecol < mincol)
    {
        tilecol = (wrap ? maxcol : mincol);
    }

    cl.setroomtilecol(rx, ry, tilecol);
}

void editorclass::switch_enemy(const bool reversed)
{
    const RoomProperty* const room = cl.getroomprop(levx, levy);

    int enemy = room->enemytype;

    if (reversed)
    {
        enemy--;
    }
    else
    {
        enemy++;
    }

    const int modulus = 10;
    enemy = (enemy % modulus + modulus) % modulus;
    cl.setroomenemytype(levx, levy, enemy);

    note = "Enemy Type Changed";
    notedelay = 45;
}

void editorclass::switch_warpdir(const bool reversed)
{
    static const int modulus = 4;
    const RoomProperty* const room = cl.getroomprop(levx, levy);

    int warpdir = room->warpdir;

    if (reversed)
    {
        --warpdir;
    }
    else
    {
        ++warpdir;
    }

    warpdir = (warpdir % modulus + modulus) % modulus;
    cl.setroomwarpdir(levx, levy, warpdir);

    switch (warpdir)
    {
    default:
        note = "Room warping disabled";
        break;
    case 1:
        note = "Room warps horizontally";
        break;
    case 2:
        note = "Room warps vertically";
        break;
    case 3:
        note = "Room warps in all directions";
        break;
    }

    notedelay = 45;
}

#endif /* NO_CUSTOM_LEVELS and NO_EDITOR */
