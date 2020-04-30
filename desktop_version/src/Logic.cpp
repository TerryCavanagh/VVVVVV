#include "Logic.h"
#include "Script.h"
#include "Network.h"
#include "FileSystemUtils.h"

void titlelogic()
{
    //Misc
    //map.updatetowerglow();
    help.updateglow();

    map.bypos -= 2;
    map.bscroll = -2;

    if (!game.colourblindmode)
    {
        graphics.updatetowerbackground();
    }

    graphics.crewframedelay--;
    if (graphics.crewframedelay <= 0)
    {
        graphics.crewframedelay = 8;
        graphics.crewframe = (graphics.crewframe + 1) % 2;
    }

    if (game.menucountdown > 0)
    {
        game.menucountdown--;
        if (game.menucountdown == 0)
        {
            if (game.menudest == Menu::mainmenu)
            {
                music.play(6);
            }
            else if (game.menudest == Menu::gameover2)
            {
                music.playef(11);
            }
            else if (game.menudest == Menu::timetrialcomplete3)
            {
                music.playef(3);
            }
            game.createmenu(game.menudest, true);
        }
    }
}

void maplogic()
{
    //Misc
    help.updateglow();
    graphics.updatetextboxes();

    graphics.crewframedelay--;
    if (graphics.crewframedelay <= 0)
    {
        graphics.crewframedelay = 8;
        graphics.crewframe = (graphics.crewframe + 1) % 2;
    }

    graphics.oldmenuoffset = graphics.menuoffset;
    if (graphics.resumegamemode)
    {
        graphics.menuoffset += 25;
        int threshold = map.extrarow ? 230 : 240;
        if (graphics.menuoffset >= threshold)
        {
            graphics.menuoffset = threshold;
            //go back to gamemode!
            game.mapheld = true;
            game.gamestate = GAMEMODE;
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
}


void gamecompletelogic()
{
    //Misc
    map.updatetowerglow();
    help.updateglow();
    graphics.crewframe = 0;
    map.scrolldir = 1;

    game.creditposition--;
    if (game.creditposition <= -game.creditmaxposition)
    {
        game.creditposition = -game.creditmaxposition;
        map.bscroll = 0;
    }
    else if (!game.press_action)
    {
        map.bscroll = +1;
    }

    if (graphics.fademode == 1)
    {
        //Fix some graphical things
        graphics.showcutscenebars = false;
        graphics.cutscenebarspos = 0;
        graphics.oldcutscenebarspos = 0;
        map.scrolldir = 0;
        map.bypos = 0;
        //Return to game
        game.gamestate = GAMECOMPLETE2;
        graphics.fademode = 4;
    }
}

void gamecompletelogic2()
{
    //Misc
    map.updatetowerglow();
    help.updateglow();

    game.creditposdelay--;
    if (game.creditposdelay <= 0)
    {
        game.creditposdelay = 1;
        game.creditposx++;
        if (game.creditposx > 40)
        {
            game.creditposy++;
            game.creditposx = 0;
            if (game.creditposy > 30) game.creditposy = 30;
        }
    }

    if (graphics.fademode == 1)
    {
        //Fix some graphical things
        graphics.showcutscenebars = false;
        graphics.cutscenebarspos = 0;
        //Fix the save thingy
        game.deletequick();
        int tmp=music.currentsong;
        music.currentsong=4;
        obj.flags[67] = true;
        game.savetele();
        music.currentsong=tmp;
        //Return to game
        map.colstate = 10;
        game.gamestate = TITLEMODE;
        graphics.fademode = 4;
        FILESYSTEM_unmountassets(); // should be before music.playef(18)
        music.playef(18);
        game.returntomenu(Menu::play);
        game.createmenu(Menu::gamecompletecontinue);
        map.nexttowercolour();
    }
}


void gamelogic()
{
    if (!game.blackout && !game.completestop)
    {
        for (size_t i = 0; i < obj.entities.size(); i++)
        {
            //Is this entity on the ground? (needed for jumping)
            if (obj.entitycollidefloor(i))
            {
                obj.entities[i].onground = 2;
            }
            else
            {
                obj.entities[i].onground--;
            }

            if (obj.entitycollideroof(i))
            {
                obj.entities[i].onroof = 2;
            }
            else
            {
                obj.entities[i].onroof--;
            }

            //Animate the entities
            obj.animateentities(i);
        }
    }

    //Misc
    if (map.towermode)
    {
        map.updatetowerglow();
    }
    help.updateglow();

    if (game.alarmon)
    {
        game.alarmdelay--;
        if (game.alarmdelay <= 0)
        {
            music.playef(19);
            game.alarmdelay = 20;
        }
    }

    if (obj.nearelephant)
    {
        obj.upset++;
        if (obj.upset == 300)
        {
            obj.upsetmode = true;
            //change player to sad
            int i = obj.getplayer();
            if (i > -1)
            {
                obj.entities[i].tile = 144;
            }
            music.playef(2);
        }
        if (obj.upset > 301) obj.upset = 301;
    }
    else if (obj.upsetmode)
    {
        obj.upset--;
        if (obj.upset <= 0)
        {
            obj.upset = 0;
            obj.upsetmode = false;
            //change player to happy
            int i = obj.getplayer();
            if (i > -1)
            {
                obj.entities[i].tile = 0;
            }
        }
    }
    else
    {
        obj.upset = 0;
    }

    obj.oldtrophytext = obj.trophytext;

    if (map.towermode)
    {
        map.oldypos = map.ypos;
        map.oldspikeleveltop = map.spikeleveltop;
        map.oldspikelevelbottom = map.spikelevelbottom;
        if(!game.completestop)
        {
            if (map.cameramode == 0)
            {
                //do nothing!
                //a trigger will set this off in the game
                map.cameramode = 1;
                map.bscroll = 0;
            }
            else if (map.cameramode == 1)
            {
                //move normally
                if(map.scrolldir==0)
                {
                    map.ypos -= 2;
                    map.bypos -= 1;
                    map.bscroll = -1;
                }
                else
                {
                    map.ypos += 2;
                    map.bypos += 1;
                    map.bscroll = 1;
                }
            }
            else if (map.cameramode == 2)
            {
                //do nothing, but cycle colours (for taking damage)
                map.bscroll = 0;
            }
            else if (map.cameramode == 4)
            {
                int i = obj.getplayer();
                if (i > -1)
                {
                    map.cameraseek = map.ypos - (obj.entities[i].yp - 120);
                }

                map.cameraseek = map.cameraseek / 10;
                map.cameraseekframe = 10;

                map.cameramode = 5;

                map.bscroll = map.cameraseek/2;
            }
            else if (map.cameramode == 5)
            {
                //actually do it
                if (map.spikeleveltop > 0) map.spikeleveltop-=2;
                if (map.spikelevelbottom > 0) map.spikelevelbottom-=2;
                if (map.cameraseekframe > 0)
                {
                    int i = obj.getplayer();
                    map.ypos -= map.cameraseek;
                    if (map.cameraseek > 0 && i > -1)
                    {
                        if (map.ypos < obj.entities[i].yp - 120)
                        {
                            map.ypos = obj.entities[i].yp - 120;
                        }
                    }
                    else if (i > -1)
                    {
                        if (map.ypos > obj.entities[i].yp - 120)
                        {
                            map.ypos = obj.entities[i].yp - 120;
                        }
                    }
                    map.cameraseekframe--;
                    map.bypos = map.ypos / 2;
                }
                else
                {
                    int i = obj.getplayer();
                    if (i > -1)
                    {
                        map.ypos = obj.entities[i].yp - 120;
                    }
                    map.bypos = map.ypos / 2;
                    map.cameramode = 0;
                    map.colsuperstate = 0;
                }
            }
        }
        else
        {
            map.bscroll = 0;
        }

        if (map.ypos <= 0)
        {
            map.ypos = 0;
            map.bypos = 0;
            map.bscroll = 0;
        }
        if (map.towermode && map.minitowermode)
        {
            if (map.ypos >= 568)
            {
                map.ypos = 568;
                map.bypos = map.ypos / 2;
                map.bscroll = 0;
            } //100-29 * 8 = 568
        }
        else
        {
            if (map.ypos >= 5368)
            {
                map.ypos = 5368;    //700-29 * 8 = 5368
                map.bypos = map.ypos / 2.0;
            }
        }

        if (game.lifeseq > 0)
        {
            if (map.cameramode == 2)
            {
                map.cameraseekframe = 20;
                map.cameramode = 4;
                map.resumedelay = 4;
            }

            if (map.cameraseekframe <= 0)
            {
                if (map.resumedelay <= 0)
                {
                    game.lifesequence();
                    if (game.lifeseq == 0) map.cameramode = 1;
                }
                else
                {
                    map.resumedelay--;
                }
            }
        }
    }
    else
    {
        game.lifesequence();
    }


    if (game.deathseq != -1)
    {
        if (map.towermode)
        {
            map.colsuperstate = 1;
            map.cameramode = 2;
        }

        for (size_t i = 0; i < obj.entities.size(); i++)
        {
            if (game.roomx == 111 && game.roomy == 107 && !map.custommode)
            {
                if (obj.entities[i].type == 1)
                {
                    if (obj.entities[i].xp < 152)
                    {
                        obj.entities[i].xp = 152;
                        obj.entities[i].newxp = 152;
                    }
                }
            }
            if (obj.entities[i].type == 2 && obj.entities[i].state == 3)
            {
                //Ok! super magical exception for the room with the intention death for the shiny trinket
                //fix this when the maps are finalised
                if (game.roomx != 111 || game.roomy != 107 || map.custommode)
                {
                    obj.entities[i].state = 4;
                }
                else
                {
                    obj.entities[i].state = 4;
                    map.settile(18, 9, 59);
                }
            }
            else if (obj.entities[i].type == 2 && obj.entities[i].state == 2)
            {
                //ok, unfortunate case where the disappearing platform hasn't fully disappeared. Accept a little
                //graphical uglyness to avoid breaking the room!
                bool entitygone = false;
                while (obj.entities[i].state == 2)
                {
                    entitygone = obj.updateentities(i);
                    if (entitygone)
                    {
                        i--;
                        break;
                    }
                }
                if (!entitygone) obj.entities[i].state = 4;
            }
            else if (map.finalstretch && obj.entities[i].type == 2)
            {
                //for the final level. probably something 99% of players won't see.
                bool entitygone = false;
                while (obj.entities[i].state == 2)
                {
                    entitygone = obj.updateentities(i);
                    if (entitygone)
                    {
                        i--;
                        break;
                    }
                }
                if (!entitygone) obj.entities[i].state = 4;
            }
            else if (obj.entities[i].type == 23 && game.swnmode && game.deathseq<15)
            {
                //if playing SWN, get the enemies offscreen.
                obj.entities[i].xp += obj.entities[i].vx*5;
                obj.entities[i].yp += obj.entities[i].vy*5;
            }
        }
        if (game.swnmode)
        {
            //if playing SWN game a, push the clock back to the nearest 10 second interval
            if (game.swngame == 0)
            {
                game.swnpenalty();
            }
            else if (game.swngame == 1)
            {
                game.swnstate = 0;
                game.swnstate2 = 0;
                game.swnstate3 = 0;
                game.swnstate4 = 0;
                game.swndelay = 0;
                if (game.swntimer >= game.swnrecord)
                {
                    if (game.swnmessage == 0) music.playef(25);
                    game.swnmessage = 1;
                    game.swnrecord = game.swntimer;
                    game.savestats();
                }
            }
        }

        game.deathsequence();
        game.deathseq--;
        if (game.deathseq <= 0)
        {
            if (game.nodeathmode)
            {
                game.deathseq = 1;
                game.gethardestroom();
                //start depressing sequence here...
                if (game.gameoverdelay <= -10 && graphics.fademode==0) graphics.fademode = 2;
                if (graphics.fademode == 1) script.resetgametomenu();
            }
            else
            {
                if (game.swnmode)
                {
                    //if playing SWN game b, reset the clock
                    if (game.swngame == 1)
                    {
                        game.swntimer = 0;
                        game.swnmessage = 0;
                        game.swnrank = 0;
                    }
                }

                game.gethardestroom();
                game.hascontrol = true;


                game.gravitycontrol = game.savegc;
                graphics.textboxremove();
                map.resetplayer();
            }
        }
    }
    else
    {
        //Update colour thingy
        if (map.finalmode)
        {
            if (map.final_colormode)
            {
                if (map.final_colorframe > 0)
                {
                    map.final_colorframedelay--;
                    if (map.final_colorframedelay <= 0)
                    {
                        if (map.final_colorframe == 1)
                        {
                            map.final_colorframedelay = 40;
                            int temp = 1+int(fRandom() * 6);
                            if (temp == map.final_mapcol) temp = (temp + 1) % 6;
                            if (temp == 0) temp = 6;
                            map.changefinalcol(temp);
                        }
                        else if (map.final_colorframe == 2)
                        {
                            map.final_colorframedelay = 15;
                            int temp = 1+int(fRandom() * 6);
                            if (temp == map.final_mapcol) temp = (temp + 1) % 6;
                            if (temp == 0) temp = 6;
                            map.changefinalcol(temp);
                        }
                    }
                }
            }
        }
        //State machine for game logic
        game.updatestate();
        if (game.startscript)
        {
            script.load(game.newscript);
            game.startscript = false;
        }

        //Intermission 1 Logic
        //Player can't walk off a screen with SCM on it until they've left
        if (game.supercrewmate)
        {
            if (game.roomx == 41 + game.scmprogress)   //he's in the same room
            {
                int i = obj.getplayer();
                if (i > -1 && obj.entities[i].ax > 0 && obj.entities[i].xp > 280)
                {
                    obj.entities[i].ax = 0;
                    obj.entities[i].dir = 0;
                }
            }
        }

        //SWN Minigame Logic
        if (game.swnmode)  	//which game?
        {
            if(game.swngame==0)  //intermission, survive 60 seconds game
            {
                game.swntimer -= 1;
                if (game.swntimer <= 0)
                {
                    music.niceplay(8);
                    game.swngame = 5;
                }
                else
                {
                    obj.generateswnwave(0);
                }
            }
            else if(game.swngame==1)   //super gravitron game
            {
                game.swntimer += 1;
                if (game.swntimer > game.swnrecord) game.swnrecord = game.swntimer;

                if (game.swntimer >= 150 && game.swnrank == 0)
                {
                    game.swnrank = 1;
                    if (game.swnbestrank < 1)
                    {
                        NETWORK_unlockAchievement("vvvvvvsupgrav5");
                        game.swnbestrank = 1;
                        game.swnmessage = 2+30;
                        music.playef(26);
                    }
                }
                else if (game.swntimer >= 300 && game.swnrank == 1)
                {
                    game.swnrank = 2;
                    if (game.swnbestrank < 2)
                    {
                        NETWORK_unlockAchievement("vvvvvvsupgrav10");
                        game.swnbestrank = 2;
                        game.swnmessage = 2+30;
                        music.playef(26);
                    }
                }
                else if (game.swntimer >= 450 && game.swnrank == 2)
                {
                    game.swnrank = 3;
                    if (game.swnbestrank < 3)
                    {
                        NETWORK_unlockAchievement("vvvvvvsupgrav15");
                        game.swnbestrank = 3;
                        game.swnmessage = 2+30;
                        music.playef(26);
                    }
                }
                else if (game.swntimer >= 600 && game.swnrank == 3)
                {
                    game.swnrank = 4;
                    if (game.swnbestrank < 4)
                    {
                        NETWORK_unlockAchievement("vvvvvvsupgrav20");
                        game.swnbestrank = 4;
                        game.swnmessage = 2+30;
                        music.playef(26);
                    }
                }
                else if (game.swntimer >= 900 && game.swnrank == 4)
                {
                    game.swnrank = 5;
                    if (game.swnbestrank < 5)
                    {
                        NETWORK_unlockAchievement("vvvvvvsupgrav30");
                        game.swnbestrank = 5;
                        game.swnmessage = 2+30;
                        music.playef(26);
                    }
                }
                else if (game.swntimer >= 1800 && game.swnrank == 5)
                {
                    game.swnrank = 6;
                    if (game.swnbestrank < 6)
                    {
                        NETWORK_unlockAchievement("vvvvvvsupgrav60");
                        game.swnbestrank = 6;
                        game.swnmessage = 2+30;
                        music.playef(26);
                    }
                }

                obj.generateswnwave(1);

                game.swncoldelay--;
                if(game.swncoldelay<=0)
                {
                    game.swncolstate = (game.swncolstate+1)%6;
                    game.swncoldelay = 30;
                    graphics.rcol = game.swncolstate;
                    obj.swnenemiescol(game.swncolstate);
                }
            }
            else if (game.swngame == 2)    //introduce game a
            {
                game.swndelay--;
                if (game.swndelay <= 0)
                {
                    game.swngame = 0;
                    game.swndelay = 0;
                    game.swntimer = (60 * 30) - 1;
                    //game.swntimer = 15;
                }
            }
            else if (game.swngame == 3)    //extend line
            {
                obj.entities[obj.getlineat(84 - 32)].w += 24;
                if (obj.entities[obj.getlineat(84 - 32)].w > 332)
                {
                    obj.entities[obj.getlineat(84 - 32)].w = 332;
                    game.swngame = 2;
                }
            }
            else if (game.swngame == 4)    //create top line
            {
                game.swngame = 3;
                obj.createentity(-8, 84 - 32, 11, 8);  // (horizontal gravity line)
                music.niceplay(2);
                game.swndeaths = game.deathcounts;
            }
            else if (game.swngame == 5)    //remove line
            {
                obj.entities[obj.getlineat(148 + 32)].xp += 24;
                if (obj.entities[obj.getlineat(148 + 32)].xp > 320)
                {
                    obj.removeentity(obj.getlineat(148 + 32));
                    game.swnmode = false;
                    game.swngame = 6;
                }
            }
            else if (game.swngame == 6)    //Init the super gravitron
            {
                game.swngame = 7;
                music.niceplay(3);
            }
            else if (game.swngame == 7)    //introduce game b
            {
                game.swndelay--;
                if (game.swndelay <= 0)
                {
                    game.swngame = 1;
                    game.swndelay = 0;
                    game.swntimer = 0;
                    game.swncolstate = 3;
                    game.swncoldelay = 30;
                }
            }
        }

        //Time trial stuff
        if (game.intimetrial)
        {

            if (game.timetrialcountdown > 0)
            {
                game.timetrialparlost = false;
                game.hascontrol = true;
                game.timetrialcountdown--;
                if (game.timetrialcountdown > 30)
                {
                    game.hascontrol = false;
                }
                if(game.timetrialcountdown == 120) music.playef(21);
                if(game.timetrialcountdown == 90) music.playef(21);
                if(game.timetrialcountdown == 60) music.playef(21);
                if (game.timetrialcountdown == 30)
                {
                    switch(game.timetriallevel)
                    {
                    case 0:
                        music.play(1);
                        break;
                    case 1:
                        music.play(3);
                        break;
                    case 2:
                        music.play(2);
                        break;
                    case 3:
                        music.play(1);
                        break;
                    case 4:
                        music.play(12);
                        break;
                    case 5:
                        music.play(15);
                        break;
                    }
                    music.playef(22);
                }
            }

            //Have we lost the par?
            if (!game.timetrialparlost)
            {
                if ((game.minutes * 60) + game.seconds > game.timetrialpar)
                {
                    game.timetrialparlost = true;
                    int i = obj.getplayer();
                    if (i > -1)
                    {
                        obj.entities[i].tile = 144;
                    }
                    music.playef(2);
                }
            }
        }

        //Update entities
        //Ok, moving platform fuckers
        if(!game.completestop)
        {
            if(obj.vertplatforms)
            {
                for (int i = obj.entities.size() - 1; i >= 0;  i--)
                {
                    if (obj.entities[i].isplatform)
                    {
                        if(abs(obj.entities[i].vx) < 0.000001f)
                        {
                            obj.removeblockat(obj.entities[i].xp, obj.entities[i].yp);

                            bool entitygone = obj.updateentities(i);                // Behavioral logic
                            if (entitygone) continue;
                            obj.updateentitylogic(i);             // Basic Physics
                            obj.entitymapcollision(i);            // Collisions with walls

                            obj.createblock(0, obj.entities[i].xp, obj.entities[i].yp, obj.entities[i].w, obj.entities[i].h);
                            if (game.supercrewmate)
                            {
                                obj.movingplatformfix(i);
                                obj.scmmovingplatformfix(i);
                            }
                            else
                            {
                                obj.movingplatformfix(i);
                            }
                        }
                    }
                }
            }

            if(obj.horplatforms)
            {
                for (int ie = obj.entities.size() - 1; ie >= 0;  ie--)
                {
                    if (obj.entities[ie].isplatform)
                    {
                        if(abs(obj.entities[ie].vy) < 0.000001f)
                        {
                            obj.removeblockat(obj.entities[ie].xp, obj.entities[ie].yp);

                            bool entitygone = obj.updateentities(ie);                // Behavioral logic
                            if (entitygone) continue;
                            obj.updateentitylogic(ie);             // Basic Physics
                            obj.entitymapcollision(ie);            // Collisions with walls

                            obj.hormovingplatformfix(ie);
                        }
                    }
                }
                //is the player standing on a moving platform?
                int i = obj.getplayer();
                float j = obj.entitycollideplatformfloor(i);
                if (i > -1 && j > -1000)
                {
                    obj.entities[i].newxp = obj.entities[i].xp + j;
                    obj.entitymapcollision(i);
                }
                else
                {
                    j = obj.entitycollideplatformroof(i);
                    if (i > -1 && j > -1000)
                    {
                        obj.entities[i].newxp = obj.entities[i].xp + j;
                        obj.entitymapcollision(i);
                    }
                }
            }

            for (int ie = obj.entities.size() - 1; ie >= 0;  ie--)
            {
                if (!obj.entities[ie].isplatform)
                {
                    bool entitygone = obj.updateentities(ie);          // Behavioral logic
                    if (entitygone) continue;
                    obj.updateentitylogic(ie);       // Basic Physics
                    obj.entitymapcollision(ie);      // Collisions with walls
                }
            }

            obj.entitycollisioncheck();         // Check ent v ent collisions, update states

            if (map.towermode)
            {
                //special for tower: is the player touching any spike blocks?
                int player = obj.getplayer();
                if(player > -1 && obj.checktowerspikes(player) && graphics.fademode==0)
                {
                    game.deathseq = 30;
                }
            }

            if(map.towermode && game.lifeseq==0)
            {
                int player = obj.getplayer();
                if(!map.invincibility && player > -1)
                {
                    if (obj.entities[player].yp-map.ypos <= 0)
                    {
                        game.deathseq = 30;
                    }
                    else if (obj.entities[player].yp-map.ypos >= 208)
                    {
                        game.deathseq = 30;
                    }
                }
                else if (player > -1)
                {
                    if (obj.entities[player].yp-map.ypos <= 0)
                    {
                        map.ypos-=10;
                        map.bypos = map.ypos / 2;
                        map.bscroll = 0;
                    }
                    else if (obj.entities[player].yp-map.ypos >= 208)
                    {
                        map.ypos+=2;
                        map.bypos = map.ypos / 2;
                        map.bscroll = 0;
                    }
                }

                if (player > -1 && obj.entities[player].yp - map.ypos <= 40)
                {
                    map.spikeleveltop++;
                    if (map.spikeleveltop >= 8) map.spikeleveltop = 8;
                }
                else
                {
                    if (map.spikeleveltop > 0) map.spikeleveltop--;
                }

                if (player > -1 && obj.entities[player].yp - map.ypos >= 164)
                {
                    map.spikelevelbottom++;
                    if (map.spikelevelbottom >= 8) map.spikelevelbottom = 8;
                }
                else
                {
                    if (map.spikelevelbottom > 0) map.spikelevelbottom--;
                }

            }
        }

        //Using warplines?
        if (obj.customwarpmode) {
            //Rewritten system for mobile update: basically, the new logic is to
            //check if the player is leaving the map, and if so do a special check against
            //warp lines for collision
            obj.customwarpmodehon = false;
            obj.customwarpmodevon = false;

            int i = obj.getplayer();
            if (i > -1 && ((game.door_down > -2 && obj.entities[i].yp >= 226-16) || (game.door_up > -2 && obj.entities[i].yp < -2+16) ||	(game.door_left > -2 && obj.entities[i].xp < -14+16) ||	(game.door_right > -2 && obj.entities[i].xp >= 308-16))){
            //Player is leaving room
            obj.customwarplinecheck(i);
        }

        if(obj.customwarpmodehon){ map.warpy=true;
        }else{ map.warpy=false; }
        if(obj.customwarpmodevon){ map.warpx=true;
        }else{ map.warpx=false; }
        }

        //Finally: Are we changing room?
        if (map.warpx && !map.towermode)
        {
            for (size_t i = 0; i < obj.entities.size();  i++)
            {
                if(obj.entities[i].type<50 //Don't warp warp lines
                && obj.entities[i].size < 12)   //Don't wrap SWN enemies
                {
                    if (game.roomx == 118 && game.roomy == 102 && obj.entities[i].rule==1 && !map.custommode)
                    {
                        //ascii snakes
                        if (obj.entities[i].xp <= -80)
                        {
                            if (obj.entities[i].isplatform) obj.removeblockat(obj.entities[i].xp, obj.entities[i].yp);
                            obj.entities[i].xp += 400;
                        }
                        else if (obj.entities[i].xp > 320)
                        {
                            if (obj.entities[i].isplatform) obj.removeblockat(obj.entities[i].xp, obj.entities[i].yp);
                            obj.entities[i].xp -= 400;
                        }
                    }
                    else
                    {
                        if (obj.entities[i].xp <= -10)
                        {
                            if (obj.entities[i].isplatform) obj.removeblockat(obj.entities[i].xp, obj.entities[i].yp);
                            obj.entities[i].xp += 320;
                            obj.entities[i].oldxp += 320;
                        }
                        else if (obj.entities[i].xp > 310)
                        {
                            if (obj.entities[i].isplatform) obj.removeblockat(obj.entities[i].xp, obj.entities[i].yp);
                            obj.entities[i].xp -= 320;
                            obj.entities[i].oldxp -= 320;
                        }
                    }
                }
            }
        }

        if (map.warpy && !map.towermode)
        {
            for (size_t i = 0; i < obj.entities.size();  i++)
            {
                if(obj.entities[i].type<50){ //Don't warp warp lines
                    if (obj.entities[i].yp <= -12)
                    {
                        if (obj.entities[i].isplatform) obj.removeblockat(obj.entities[i].xp, obj.entities[i].yp);
                        obj.entities[i].yp += 232;
                        obj.entities[i].oldyp += 232;
                    }
                    else if (obj.entities[i].yp > 226)
                    {
                        if (obj.entities[i].isplatform) obj.removeblockat(obj.entities[i].xp, obj.entities[i].yp);
                        obj.entities[i].yp -= 232;
                        obj.entities[i].oldyp -= 232;
                    }
                }
            }
        }

        if (map.warpy && !map.warpx && !map.towermode)
        {
            for (size_t i = 0; i < obj.entities.size();  i++)
            {

                if(obj.entities[i].type<50 //Don't warp warp lines
                &&obj.entities[i].rule!=0)
                {
                    if (obj.entities[i].xp <= -30)
                    {
                        if (obj.entities[i].isplatform) obj.removeblockat(obj.entities[i].xp, obj.entities[i].yp);
                        obj.entities[i].xp += 350;
                        obj.entities[i].oldxp += 350;
                    }
                    else if (obj.entities[i].xp > 320)
                    {
                        if (obj.entities[i].isplatform) obj.removeblockat(obj.entities[i].xp, obj.entities[i].yp);
                        obj.entities[i].xp -= 350;
                        obj.entities[i].oldxp -= 350;
                    }
                }
            }
        }

        if (!map.warpy && !map.towermode)
        {
            //Normal! Just change room
            int player = obj.getplayer();
            if (player > -1 && game.door_down > -2 && obj.entities[player].yp >= 238)
            {
                obj.entities[player].yp -= 240;
                map.gotoroom(game.roomx, game.roomy + 1);
            }
            if (player > -1 && game.door_up > -2 && obj.entities[player].yp < -2)
            {
                obj.entities[player].yp += 240;
                map.gotoroom(game.roomx, game.roomy - 1);
            }
        }

        if (!map.warpx && !map.towermode)
        {
            //Normal! Just change room
            int player = obj.getplayer();
            if (player > -1 && game.door_left > -2 && obj.entities[player].xp < -14)
            {
                obj.entities[player].xp += 320;
                map.gotoroom(game.roomx - 1, game.roomy);
            }
            if (player > -1 && game.door_right > -2 && obj.entities[player].xp >= 308)
            {
                obj.entities[player].xp -= 320;
                map.gotoroom(game.roomx + 1, game.roomy);
            }
        }

        //Right so! Screenwraping for tower:
        if (map.towermode && map.minitowermode)
        {
            if (map.scrolldir == 1)
            {
                //This is minitower 1!
                int player = obj.getplayer();
                if (player > -1 && game.door_left > -2 && obj.entities[player].xp < -14)
                {
                    obj.entities[player].xp += 320;
                    map.gotoroom(48, 52);
                }
                if (player > -1 && game.door_right > -2 && obj.entities[player].xp >= 308)
                {
                    obj.entities[player].xp -= 320;
                    obj.entities[player].yp -= (71*8);
                    map.gotoroom(game.roomx + 1, game.roomy+1);
                }
            }
            else
            {
                //This is minitower 2!
                int player = obj.getplayer();
                if (player > -1 && game.door_left > -2 && obj.entities[player].xp < -14)
                {
                    if (obj.entities[player].yp > 300)
                    {
                        obj.entities[player].xp += 320;
                        obj.entities[player].yp -= (71 * 8);
                        map.gotoroom(50, 54);
                    }
                    else
                    {
                        obj.entities[player].xp += 320;
                        map.gotoroom(50, 53);
                    }
                }
                if (player > -1 && game.door_right > -2 && obj.entities[player].xp >= 308)
                {
                    obj.entities[player].xp -= 320;
                    map.gotoroom(52, 53);
                }
            }
        }
        else if (map.towermode)
        {
            //Always wrap except for the very top and very bottom of the tower
            if(map.ypos>=500 && map.ypos <=5000)
            {
                for (size_t i = 0; i < obj.entities.size();  i++)
                {
                    if (obj.entities[i].xp <= -10)
                    {
                        obj.entities[i].xp += 320;
                        obj.entities[i].oldxp += 320;
                    }
                    else if (obj.entities[i].xp > 310)
                    {
                        obj.entities[i].xp -= 320;
                        obj.entities[i].oldxp -= 320;
                    }
                }
            }
            else
            {
                //Do not wrap! Instead, go to the correct room
                int player = obj.getplayer();
                if (player > -1 && game.door_left > -2 && obj.entities[player].xp < -14)
                {
                    obj.entities[player].xp += 320;
                    obj.entities[player].yp -= (671 * 8);
                    map.gotoroom(108, 109);
                }
                if (player > -1 && game.door_right > -2 && obj.entities[player].xp >= 308)
                {
                    obj.entities[player].xp -= 320;
                    map.gotoroom(110, 104);
                }
            }
        }

        //Warp tokens
        if (map.custommode){
            if (game.teleport)
            {
                int edi=obj.entities[game.edteleportent].behave;
                int edj=obj.entities[game.edteleportent].para;
                int edi2, edj2;
                edi2 = (edi-(edi%40))/40;
                edj2 = (edj-(edj%30))/30;

                map.warpto(100+edi2, 100+edj2, obj.getplayer(), edi%40, (edj%30)+2);
                game.teleport = false;

                if (game.teleport == false)
                {
                    game.flashlight = 6;
                    game.screenshake = 25;
                }
            }
        }else{
            if (game.teleport)
            {
                if (game.roomx == 117 && game.roomy == 102)
                {
                    int i = obj.getplayer();
                    if (i > -1)
                    {
                        obj.entities[i].yp = 225;
                    }
                    map.gotoroom(119, 100);
                    game.teleport = false;
                }
                else if (game.roomx == 119 && game.roomy == 100)
                {
                    int i = obj.getplayer();
                    if (i > -1)
                    {
                        obj.entities[i].yp = 225;
                    }
                    map.gotoroom(119, 103);
                    game.teleport = false;
                }
                else if (game.roomx == 119 && game.roomy == 103)
                {
                    int i = obj.getplayer();
                    if (i > -1)
                    {
                        obj.entities[i].xp = 0;
                    }
                    map.gotoroom(116, 103);
                    game.teleport = false;
                }
                else if (game.roomx == 116 && game.roomy == 103)
                {
                    int i = obj.getplayer();
                    if (i > -1)
                    {
                        obj.entities[i].yp = 225;
                    }
                    map.gotoroom(116, 100);
                    game.teleport = false;
                }
                else if (game.roomx == 116 && game.roomy == 100)
                {
                    int i = obj.getplayer();
                    if (i > -1)
                    {
                        obj.entities[i].xp = 0;
                    }
                    map.gotoroom(114, 102);
                    game.teleport = false;
                }
                else if (game.roomx == 114 && game.roomy == 102)
                {
                    int i = obj.getplayer();
                    if (i > -1)
                    {
                        obj.entities[i].yp = 225;
                    }
                    map.gotoroom(113, 100);
                    game.teleport = false;
                }
                else if (game.roomx == 116 && game.roomy == 104)
                {
                    //pre warp zone here
                    map.warpto(107, 101, obj.getplayer(), 14, 16);
                }
                else if (game.roomx == 107 && game.roomy == 101)
                {
                    map.warpto(105, 119, obj.getplayer(), 5, 26);
                }
                else if (game.roomx == 105 && game.roomy == 118)
                {
                    map.warpto(101, 111, obj.getplayer(), 34, 6);
                }
                else if (game.roomx == 101 && game.roomy == 111)
                {
                    //There are lots of warp tokens in this room, so we have to distinguish!
                    switch(game.teleportxpos)
                    {
                    case 1:
                        map.warpto(108, 108, obj.getplayer(), 4, 27);
                        break;
                    case 2:
                        map.warpto(101, 111, obj.getplayer(), 12, 27);
                        break;
                    case 3:
                        map.warpto(119, 111, obj.getplayer(), 31, 7);
                        break;
                    case 4:
                        map.warpto(114, 117, obj.getplayer(), 19, 16);
                        break;
                    }
                }
                else if (game.roomx == 108 && game.roomy == 106)
                {
                    map.warpto(119, 111, obj.getplayer(), 4, 27);
                }
                else if (game.roomx == 100 && game.roomy == 111)
                {
                    map.warpto(101, 111, obj.getplayer(), 24, 6);
                }
                else if (game.roomx == 119 && game.roomy == 107)
                {
                    //Secret lab, to super gravitron
                    map.warpto(119, 108, obj.getplayer(), 19, 10);
                }
                if (game.teleport == false)
                {
                    game.flashlight = 6;
                    game.screenshake = 25;
                }
            }
        }
    }

    //Update colour cycling for final level
    if (map.finalmode && map.final_colormode)
    {
        map.final_aniframedelay--;
        if(map.final_aniframedelay==0)
        {
            graphics.foregrounddrawn=false;
        }
        if (map.final_aniframedelay <= 0) {
            map.final_aniframedelay = 2;
            map.final_aniframe++;
            if (map.final_aniframe >= 4)
                map.final_aniframe = 0;
        }
    }

    int j;
    if (game.roomchange)
    {
        //We've changed room? Let's bring our companion along!
        game.roomchange = false;
        int i = obj.getplayer();
        if (game.companion > 0 && i > -1)
        {
            //ok, we'll presume our companion has been destroyed in the room change. So:
            switch(game.companion)
            {
            case 6:
                obj.createentity(obj.entities[i].xp, 121.0f, 15.0f,1);  //Y=121, the floor in that particular place!
                j = obj.getcompanion();
                if (j > -1)
                {
                    obj.entities[j].vx = obj.entities[i].vx;
                    obj.entities[j].dir = obj.entities[i].dir;
                }
                break;
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
                    j = obj.getcompanion();
                    if (j > -1)
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
                        j = obj.getcompanion();
                        if (j > -1)
                        {
                            obj.entities[j].vx = obj.entities[i].vx;
                            obj.entities[j].dir = obj.entities[i].dir;
                        }
                    }
                    else
                    {
                        obj.createentity(obj.entities[i].xp, 177.0f, 17.0f, 1);
                        j = obj.getcompanion();
                        if (j > -1)
                        {
                            obj.entities[j].vx = obj.entities[i].vx;
                            obj.entities[j].dir = obj.entities[i].dir;
                        }
                    }
                }
                break;
            case 9:
                if (!map.towermode)   //don't go back into the tower!
                {
                    if (game.roomx == 110 && obj.entities[i].xp<20)
                    {
                        obj.createentity(100, 185, 18, 15, 0, 1);
                    }
                    else
                    {
                        obj.createentity(obj.entities[i].xp, 185.0f, 18.0f, 15, 0, 1);
                    }
                    j = obj.getcompanion();
                    if (j > -1)
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
                        j = obj.getcompanion();
                        if (j > -1)
                        {
                            obj.entities[j].vx = obj.entities[i].vx;
                            obj.entities[j].dir = obj.entities[i].dir;
                        }
                    }
                }
                else	if (game.roomy >= 52)
                {
                    if (obj.flags[59])
                    {
                        obj.createentity(160.0f, 177.0f, 18, graphics.crewcolour(game.lastsaved), 0, 18, 1);
                        j = obj.getcompanion();
                        if (j > -1)
                        {
                            obj.entities[j].vx = obj.entities[i].vx;
                            obj.entities[j].dir = obj.entities[i].dir;
                        }
                    }
                    else
                    {
                        obj.flags[59] = true;
                        obj.createentity(obj.entities[i].xp, -20.0f, 18.0f, graphics.crewcolour(game.lastsaved), 0, 10, 0);
                        j = obj.getcompanion();
                        if (j > -1)
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
                        if (game.scmmoveme)
                        {
                            obj.createentity(obj.entities[obj.getplayer()].xp, 185, 24, graphics.crewcolour(game.lastsaved), 2);
                            game.scmmoveme = false;
                        }
                        else
                        {
                            obj.createentity(10, 177, 24, graphics.crewcolour(game.lastsaved), 2);
                        }
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

                if (game.scmmoveme)
                {
                    obj.entities[obj.getscm()].xp = obj.entities[obj.getplayer()].xp;
                    game.scmmoveme = false;
                }
                break;
            }
        }
    }

    game.activeactivity = obj.checkactivity();

    game.oldreadytotele = game.readytotele;
    if (game.activetele)
    {
        int i = obj.getplayer();
        if (i > -1)
        {
            obj.settemprect(i);
        }
        if (help.intersects(game.teleblock, obj.temprect))
        {
            game.readytotele += 25;
            if (game.readytotele >= 255) game.readytotele = 255;
        }
        else
        {
            game.readytotele -= 50;
            if (game.readytotele < 0) game.readytotele = 0;
        }
    }
    else
    {
        if (game.readytotele > 0)
        {
            game.readytotele -= 50;
            if (game.readytotele < 0) game.readytotele = 0;
        }
    }

    if (game.teleport_to_new_area)
        script.teleport();

#if !defined(NO_CUSTOM_LEVELS)
    if (game.shouldreturntoeditor)
    {
        game.shouldreturntoeditor = false;
        game.returntoeditor();
    }
#endif

    game.prev_act_fade = game.act_fade;
    if (game.activeactivity > -1)
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

    graphics.updatetextboxes();

    if (!game.colourblindmode)
    {
        if (map.towermode)
        {
            graphics.updatetowerbackground();
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
}
