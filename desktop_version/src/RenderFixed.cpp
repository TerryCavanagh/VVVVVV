#include "CustomLevels.h"
#include "Game.h"
#include "Graphics.h"
#include "Editor.h"
#include "Entity.h"
#include "Enums.h"
#include "Map.h"
#include "Script.h"
#include "UtilityClass.h"

static inline void titleupdatetextcol(void)
{
    graphics.col_tr = graphics.titlebg.r - (help.glow / 4) - int(fRandom() * 4);
    graphics.col_tg = graphics.titlebg.g - (help.glow / 4) - int(fRandom() * 4);
    graphics.col_tb = graphics.titlebg.b - (help.glow / 4) - int(fRandom() * 4);
    if (graphics.col_tr < 0) graphics.col_tr = 0;
    if(graphics.col_tr>255) graphics.col_tr=255;
    if (graphics.col_tg < 0) graphics.col_tg = 0;
    if(graphics.col_tg>255) graphics.col_tg=255;
    if (graphics.col_tb < 0) graphics.col_tb = 0;
    if(graphics.col_tb>255) graphics.col_tb=255;
}

static inline void tick_skip_message_timer(void)
{
    const bool tick = graphics.fademode == FADE_NONE;
    if (!tick)
    {
        return;
    }

    game.old_skip_message_timer = game.skip_message_timer;
    if (game.skip_message_timer > 0)
    {
        game.skip_message_timer -= 15;
    }
}

void gamerenderfixed(void)
{
    if (!game.blackout && !game.completestop)
    {
        for (size_t i = 0; i < obj.entities.size(); i++)
        {
            if (obj.entitycollidefloor(i))
            {
                obj.entities[i].visualonground = 2;
            }
            else
            {
                --obj.entities[i].visualonground;
            }

            if (obj.entitycollideroof(i))
            {
                obj.entities[i].visualonroof = 2;
            }
            else
            {
                --obj.entities[i].visualonroof;
            }

            //Animate the entities
            obj.animateentities(i);
        }
    }

    game.prev_act_fade = game.act_fade;
    if (INBOUNDS_VEC(game.activeactivity, obj.blocks) && game.hascontrol && !script.running)
    {
        if (game.act_fade < 5)
        {
            game.act_fade = 5;
        }
        if (game.act_fade < 10)
        {
            game.act_fade++;
        }
    }
    else if (game.act_fade > 5)
    {
        game.act_fade--;
    }

    if (obj.trophytext > 0)
    {
        obj.trophytext--;
    }

    graphics.cutscenebarstimer();

    graphics.updatetextboxes();

    if (!game.colourblindmode)
    {
        if (map.towermode)
        {
            graphics.updatetowerbackground(graphics.towerbg);
        }
        else
        {
            graphics.updatebackground(map.background);
        }
    }

    if (!game.blackout)
    {
        //Update line colours!
        if (graphics.linedelay <= 0)
        {
            graphics.linestate++;
            if (graphics.linestate >= 10) graphics.linestate = 0;
            graphics.linedelay = 2;
        }
        else
        {
            graphics.linedelay--;
        }
    }

    graphics.trinketcolset = false;
    for (int i = obj.entities.size() - 1; i >= 0; i--)
    {
        if (obj.entities[i].invis)
        {
            continue;
        }

        obj.entities[i].updatecolour();
    }

    map.updateroomnames();

    ed.old_return_message_timer = ed.return_message_timer;
    if (map.custommode && !map.custommodeforreal && ed.return_message_timer > 0)
    {
        ed.return_message_timer -= 15;
    }

    // Editor ghosts!
    if (game.ghostsenabled)
    {
        if (map.custommode && !map.custommodeforreal)
        {
            if (game.frames % 3 == 0)
            {
                int i = obj.getplayer();
                GhostInfo ghost;
                ghost.rx = game.roomx-100;
                ghost.ry = game.roomy-100;
                if (INBOUNDS_VEC(i, obj.entities))
                {
                    ghost.x = obj.entities[i].xp;
                    ghost.y = obj.entities[i].yp;
                    ghost.col = obj.entities[i].colour;
                    ghost.realcol = obj.entities[i].realcol;
                    ghost.frame = obj.entities[i].drawframe;
                }
                ed.ghosts.push_back(ghost);
            }
            if (ed.ghosts.size() > 100)
            {
                ed.ghosts.erase(ed.ghosts.begin());
            }
        }
    }
}

void titlerenderfixed(void)
{
    if (!game.colourblindmode)
    {
        graphics.updatetowerbackground(graphics.titlebg);
    }

    if (!game.menustart)
    {
        graphics.col_tr = (int)(164 - (help.glow / 2) - int(fRandom() * 4));
        graphics.col_tg = 164 - (help.glow / 2) - int(fRandom() * 4);
        graphics.col_tb = 164 - (help.glow / 2) - int(fRandom() * 4);
    }
    else
    {
        titleupdatetextcol();

        graphics.updatetitlecolours();
    }

    graphics.crewframedelay--;
    if (graphics.crewframedelay <= 0)
    {
        graphics.crewframedelay = 8;
        graphics.crewframe = (graphics.crewframe + 1) % 2;
    }
}

void maprenderfixed(void)
{
    graphics.updatetextboxes();
    graphics.updatetitlecolours();

    graphics.crewframedelay--;
    if (graphics.crewframedelay <= 0)
    {
        graphics.crewframedelay = 8;
        graphics.crewframe = (graphics.crewframe + 1) % 2;
    }

    graphics.oldmenuoffset = graphics.menuoffset;
    if (graphics.resumegamemode)
    {
        if (game.prevgamestate == GAMEMODE
        //Failsafe: if the script command gamemode(teleporter) got ran and the
        //cutscene stopped without doing gamemode(game), then we need to go
        //back to GAMEMODE, not game.prevgamestate (TELEPORTERMODE)
        || !script.running)
        {
            graphics.menuoffset += 25;
            int threshold = 240;
            if (graphics.menuoffset >= threshold)
            {
                graphics.menuoffset = threshold;
                //go back to gamemode!
                game.mapheld = true;
                game.gamestate = GAMEMODE;
                graphics.resumegamemode = false;
            }
        }
        else
        {
            game.mapheld = true;
            game.gamestate = game.prevgamestate;
            graphics.resumegamemode = false;
        }
    }
    else if (graphics.menuoffset > 0)
    {
        graphics.menuoffset -= 25;
        if (graphics.menuoffset < 0)
        {
            graphics.menuoffset = 0;
        }
    }

    if (map.cursorstate == 0){
        map.cursordelay++;
        if (map.cursordelay > 10){
            map.cursorstate = 1;
            map.cursordelay = 0;
        }
    }else if (map.cursorstate == 1){
        map.cursordelay++;
        if (map.cursordelay > 30) map.cursorstate = 2;
    }else if (map.cursorstate == 2){
        map.cursordelay++;
    }

    map.updateroomnames();
}

void teleporterrenderfixed(void)
{
    maprenderfixed();

    graphics.cutscenebarstimer();
}

void gamecompleterenderfixed(void)
{
    graphics.updatetitlecolours();

    titleupdatetextcol();

    tick_skip_message_timer();
}

void gamecompleterenderfixed2(void)
{
    tick_skip_message_timer();
}
