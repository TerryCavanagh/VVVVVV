#define SCRIPT_DEFINITION
#include "Script.h"

#include <limits.h>
#include <SDL_timer.h>

#include "Alloc.h"
#include "Constants.h"
#include "CustomLevels.h"
#include "Editor.h"
#include "Entity.h"
#include "Enums.h"
#include "Exit.h"
#include "Font.h"
#include "GlitchrunnerMode.h"
#include "Graphics.h"
#include "KeyPoll.h"
#include "Localization.h"
#include "LocalizationMaint.h"
#include "LocalizationStorage.h"
#include "Map.h"
#include "Music.h"
#include "Unreachable.h"
#include "UtilityClass.h"
#include "VFormat.h"
#include "Vlogging.h"
#include "Xoshiro.h"

scriptclass::scriptclass(void)
{
    position = 0;
    scriptdelay = 0;
    running = false;

    b = 0;
    g = 0;
    i = 0;
    j = 0;
    k = 0;
    loopcount = 0;
    looppoint = 0;
    r = 0;
    textx = 0;
    texty = 0;
    textbox_colours.clear();
    add_default_colours();
    textflipme = false;
    textcentertext = false;
    textboxtimer = 0;
    textpad_left = 0;
    textpad_right = 0;
    textpadtowidth = 0;
    textcase = 1;
    textbuttons = false;
    textlarge = false;
}

void scriptclass::add_default_colours(void)
{
    textbox_colours["player"] = graphics.getRGB(164, 164, 255);
    textbox_colours["cyan"] = graphics.getRGB(164, 164, 255);
    textbox_colours["red"] = graphics.getRGB(255, 60, 60);
    textbox_colours["green"] = graphics.getRGB(144, 255, 144);
    textbox_colours["yellow"] = graphics.getRGB(255, 255, 134);
    textbox_colours["blue"] = graphics.getRGB(95, 95, 255);
    textbox_colours["purple"] = graphics.getRGB(255, 134, 255);
    textbox_colours["white"] = graphics.getRGB(244, 244, 244);
    textbox_colours["gray"] = graphics.getRGB(174, 174, 174);
    textbox_colours["orange"] = graphics.getRGB(255, 130, 20);
    textbox_colours["transparent"] = graphics.getRGB(0, 0, 0);
}

void scriptclass::clearcustom(void)
{
    customscripts.clear();
}

static bool argexists[NUM_SCRIPT_ARGS];
static std::string raw_words[NUM_SCRIPT_ARGS];

void scriptclass::tokenize( const std::string& t )
{
    j = 0;
    std::string tempword;
    std::string temprawword;
    char currentletter;

    SDL_zeroa(argexists);

    for (size_t i = 0; i < t.length(); i++)
    {
        currentletter = t[i];
        if (currentletter == '(' || currentletter == ')' || currentletter == ',')
        {
            words[j] = tempword;
            raw_words[j] = temprawword;
            argexists[j] = words[j] != "";
            for (size_t ii = 0; ii < words[j].length(); ii++)
            {
                words[j][ii] = SDL_tolower(words[j][ii]);
            }
            j++;
            tempword = "";
            temprawword = "";
        }
        else if (currentletter == ' ')
        {
            /* Ignore spaces unless it's part of a script name. */
            temprawword += currentletter;
        }
        else
        {
            tempword += currentletter;
            temprawword += currentletter;
        }
        if (j >= (int) SDL_arraysize(words))
        {
            break;
        }
    }

    if (j < (int) SDL_arraysize(words))
    {
        const bool lastargexists = tempword != "";
        if (lastargexists)
        {
            words[j] = tempword;
            raw_words[j] = tempword;
        }
        argexists[j] = lastargexists;
    }
}

static int getcolorfromname(std::string name)
{
    if      (name == "player")     return CYAN;
    else if (name == "cyan")       return CYAN;
    else if (name == "red")        return RED;
    else if (name == "green")      return GREEN;
    else if (name == "yellow")     return YELLOW;
    else if (name == "blue")       return BLUE;
    else if (name == "purple")     return PURPLE;
    else if (name == "customcyan") return CYAN;
    else if (name == "gray")       return GRAY;
    else if (name == "teleporter") return TELEPORTER;

    int color = help.Int(name.c_str(), -1);
    if (color < 0) return -1; // Not a number (or it's negative), so we give up
    return color; // Last effort to give a valid color, maybe they just input the color?
}

static int getcrewmanfromname(std::string name)
{
    if (name == "player") return obj.getplayer(); //  Return the player
    int color = getcolorfromname(name); // Maybe they passed in a crewmate name, or an id?
    if (color == -1) return -1; // ...Nope, return -1
    return obj.getcrewman(color);
}


void scriptclass::run(void)
{
    if (!running)
    {
        return;
    }

    // This counter here will stop the function when it gets too high
    short execution_counter = 0;
    while(running && scriptdelay<=0 && !game.pausescript)
    {
        if (INBOUNDS_VEC(position, commands))
        {
            //Let's split or command in an array of words
            tokenize(commands[position]);

            //For script assisted input
            game.press_left = false;
            game.press_right = false;
            game.press_action = false;
            game.press_map = false;

            //Ok, now we run a command based on that string
            if (words[0] == "moveplayer")
            {
                //USAGE: moveplayer(x offset, y offset)
                int player = obj.getplayer();
                if (INBOUNDS_VEC(player, obj.entities))
                {
                    obj.entities[player].xp += ss_toi(words[1]);
                    obj.entities[player].yp += ss_toi(words[2]);
                    obj.entities[player].lerpoldxp = obj.entities[player].xp;
                    obj.entities[player].lerpoldyp = obj.entities[player].yp;
                }
                scriptdelay = 1;
            }
            if (words[0] == "setroomname")
            {
                ++position;
                if (INBOUNDS_VEC(position, commands))
                {
                    map.roomname_special = true;
                    map.roomnameset = true;
                    map.setroomname(commands[position].c_str());
                }
            }
            if (words[0] == "warpdir")
            {
                int temprx = ss_toi(words[1]) - 1;
                int tempry = ss_toi(words[2]) - 1;
                const RoomProperty* room;
                cl.setroomwarpdir(temprx, tempry, ss_toi(words[3]));

                room = cl.getroomprop(temprx, tempry);

                //Do we update our own room?
                if (game.roomx - 100 == temprx && game.roomy - 100 == tempry)
                {
                    //If screen warping, then override all that:
                    graphics.backgrounddrawn = false;
                    map.warpx = false;
                    map.warpy = false;
                    if (room->warpdir == 0)
                    {
                        map.background = 1;
                        //Be careful, we could be in a Lab or Warp Zone room...
                        if (room->tileset == 2)
                        {
                            //Lab
                            map.background = 2;
                            graphics.rcol = room->tilecol;
                        }
                        else if (room->tileset == 3)
                        {
                            //Warp Zone
                            map.background = 6;
                        }
                    }
                    else if (room->warpdir == 1)
                    {
                        map.warpx = true;
                        map.background = 3;
                        graphics.rcol = cl.getwarpbackground(temprx, tempry);
                    }
                    else if (room->warpdir == 2)
                    {
                        map.warpy = true;
                        map.background = 4;
                        graphics.rcol = cl.getwarpbackground(temprx, tempry);
                    }
                    else if (room->warpdir == 3)
                    {
                        map.warpx = true;
                        map.warpy = true;
                        map.background = 5;
                        graphics.rcol = cl.getwarpbackground(temprx, tempry);
                    }
                }
            }
            if (words[0] == "ifwarp")
            {
                const RoomProperty* const room = cl.getroomprop(ss_toi(words[1])-1, ss_toi(words[2])-1);
                if (room->warpdir == ss_toi(words[3]))
                {
                    loadalts("custom_" + words[4], "custom_" + raw_words[4]);
                    position--;
                }
            }
            if (words[0] == "destroy")
            {
                if (words[1] == "gravitylines")
                {
                    for (size_t edi = 0; edi < obj.entities.size(); edi++)
                    {
                        if (obj.entities[edi].type == 9 || obj.entities[edi].type == 10)
                        {
                            obj.disableentity(edi);
                        }
                    }
                }
                else if (words[1] == "warptokens")
                {
                    for (size_t edi = 0; edi < obj.entities.size(); edi++)
                    {
                        if (obj.entities[edi].type == 11)
                        {
                            obj.disableentity(edi);
                        }
                    }
                }
                else if (words[1] == "platforms" || words[1] == "moving")
                {
                    for (size_t edi = 0; edi < obj.entities.size(); edi++)
                    {
                        if (obj.entities[edi].rule == 2 && obj.entities[edi].animate == 100)
                        {
                            if (words[1] == "moving")
                            {
                                obj.disableblockat(obj.entities[edi].xp, obj.entities[edi].yp);
                            }
                            obj.disableentity(edi);
                        }
                    }
                }
                else if (words[1] == "disappear")
                {
                    for (size_t edi = 0; edi < obj.entities.size(); edi++)
                    {
                        obj.disableblockat(obj.entities[edi].xp, obj.entities[edi].yp);
                        if (obj.entities[edi].type == 2 && obj.entities[edi].rule == 3)
                        {
                            obj.disableentity(edi);
                        }
                    }
                }
            }
            if (words[0] == "customiftrinkets")
            {
                if (game.trinkets() >= ss_toi(words[1]))
                {
                    loadalts("custom_" + words[2], "custom_" + raw_words[2]);
                    position--;
                }
            }
            if (words[0] == "customiftrinketsless")
            {
                if (game.trinkets() < ss_toi(words[1]))
                {
                    loadalts("custom_" + words[2], "custom_" + raw_words[2]);
                    position--;
                }
            }
            else if (words[0] == "customifflag")
            {
                int flag = ss_toi(words[1]);
                if (INBOUNDS_ARR(flag, obj.flags) && obj.flags[flag])
                {
                    loadalts("custom_" + words[2], "custom_" + raw_words[2]);
                    position--;
                }
            }
            if (words[0] == "custommap")
            {
                if(words[1]=="on"){
                    map.customshowmm=true;
                }else if(words[1]=="off"){
                    map.customshowmm=false;
                }
            }
            if (words[0] == "delay")
            {
                //USAGE: delay(frames)
                scriptdelay = ss_toi(words[1]);
            }
            if (words[0] == "flag")
            {
                int flag = ss_toi(words[1]);
                if (INBOUNDS_ARR(flag, obj.flags))
                {
                    if (words[2] == "on")
                    {
                        obj.flags[flag] = true;
                    }
                    else if (words[2] == "off")
                    {
                        obj.flags[flag] = false;
                    }
                }
            }
            if (words[0] == "flash")
            {
                //USAGE: flash(frames)
                game.flashlight = ss_toi(words[1]);
            }
            if (words[0] == "shake")
            {
                //USAGE: shake(frames)
                game.screenshake = ss_toi(words[1]);
            }
            if (words[0] == "walk")
            {
                //USAGE: walk(dir,frames)
                if (words[1] == "left")
                {
                    game.press_left = true;
                }
                else if (words[1] == "right")
                {
                    game.press_right = true;
                }
                scriptdelay = ss_toi(words[2]);
            }
            if (words[0] == "flip")
            {
                game.press_action = true;
                scriptdelay = 1;
            }
            if (words[0] == "tofloor")
            {
                int player = obj.getplayer();
                if(INBOUNDS_VEC(player, obj.entities) && obj.entities[player].onroof>0)
                {
                    game.press_action = true;
                    scriptdelay = 1;
                }
            }
            if (words[0] == "playef")
            {
                music.playef(ss_toi(words[1]));
            }
            if (words[0] == "play")
            {
                music.play(ss_toi(words[1]));
            }
            if (words[0] == "stopmusic")
            {
                music.haltdasmusik();
            }
            if (words[0] == "resumemusic")
            {
                music.resumefade(0);
            }
            if (words[0] == "musicfadeout")
            {
                music.fadeout(false);
            }
            if (words[0] == "musicfadein")
            {
                music.fadein();
            }
            if (words[0] == "trinketscriptmusic")
            {
                music.play(Music_PASSIONFOREXPLORING);
            }
            if (words[0] == "gotoposition")
            {
                //USAGE: gotoposition(x position, y position, gravity position)
                int player = obj.getplayer();
                if (INBOUNDS_VEC(player, obj.entities))
                {
                    obj.entities[player].xp = ss_toi(words[1]);
                    obj.entities[player].yp = ss_toi(words[2]);
                    obj.entities[player].lerpoldxp = obj.entities[player].xp;
                    obj.entities[player].lerpoldyp = obj.entities[player].yp;
                }
                game.gravitycontrol = ss_toi(words[3]);

            }
            if (words[0] == "gotoroom")
            {
                //USAGE: gotoroom(x,y) (manually add 100)
                map.gotoroom(ss_toi(words[1])+100, ss_toi(words[2])+100);
            }
            if (words[0] == "cutscene")
            {
                graphics.showcutscenebars = true;
            }
            if (words[0] == "endcutscene")
            {
                graphics.showcutscenebars = false;
            }
            if (words[0] == "audiopause")
            {
                if (words[1] == "on")
                {
                    game.disabletemporaryaudiopause = false;
                }
                else if (words[1] == "off")
                {
                    game.disabletemporaryaudiopause = true;
                }
            }
            if (words[0] == "untilbars")
            {
                if (graphics.showcutscenebars)
                {
                    if (graphics.cutscenebarspos < 360)
                    {
                        scriptdelay = 1;
                        position--;
                    }
                }
                else
                {
                    if (graphics.cutscenebarspos > 0)
                    {
                        scriptdelay = 1;
                        position--;
                    }
                }
            }
            else if (words[0] == "text")
            {
                // oh boy
                // first word is the colour.
                if (textbox_colours.count(words[1]) == 0)
                {
                    // No colour named this, use gray
                    words[1] = "gray";
                }

                r = textbox_colours[words[1]].r;
                g = textbox_colours[words[1]].g;
                b = textbox_colours[words[1]].b;

                //next are the x,y coordinates
                textx = ss_toi(words[2]);
                texty = ss_toi(words[3]);

                textlarge = endsWith(words[4].c_str(), "l") || endsWith(words[4].c_str(), "L");
                int lines = ss_toi(words[4]);

                //Number of lines for the textbox!
                txt.clear();
                for (int i = 0; i < lines; i++)
                {
                    position++;
                    if (INBOUNDS_VEC(position, commands))
                    {
                        txt.push_back(commands[position]);
                    }
                }

                textcentertext = false;
                textpad_left = 0;
                textpad_right = 0;
                textpadtowidth = 0;
                textboxtimer = 0;

                translate_dialogue();
            }
            else if (words[0] == "position")
            {
                //are we facing left or right? for some objects we don't care, default at 0.
                j = 0;

                //the first word is the object to position relative to
                if (words[1] == "centerx")
                {
                    words[2] = "donothing";
                    j = -1;
                    textx = -500;
                }
                else if (words[1] == "centery")
                {
                    words[2] = "donothing";
                    j = -1;
                    texty = -500;
                }
                else if (words[1] == "center")
                {
                    words[2] = "donothing";
                    j = -1;
                    textx = -500;
                    texty = -500;
                }
                else // Well, are they asking for a crewmate...?
                {
                    i = getcrewmanfromname(words[1]);
                    if (INBOUNDS_VEC(i, obj.entities))
                    {
                        j = obj.entities[i].dir;
                    }
                }

                //next is whether to position above or below
                if (INBOUNDS_VEC(i, obj.entities) && words[2] == "above")
                {
                    if (j == 1)    //left
                    {
                        textx = obj.entities[i].xp -10000; //tells the box to be oriented correctly later
                        texty = obj.entities[i].yp - 16 - (txt.size() * font::height(PR_FONT_LEVEL));
                    }
                    else if (j == 0)     //Right
                    {
                        textx = obj.entities[i].xp - 16;
                        texty = obj.entities[i].yp - 18 - (txt.size() * font::height(PR_FONT_LEVEL));
                    }
                }
                else if (INBOUNDS_VEC(i, obj.entities))
                {
                    if (j == 1)    //left
                    {
                        textx = obj.entities[i].xp -10000; //tells the box to be oriented correctly later
                        texty = obj.entities[i].yp + 26;
                    }
                    else if (j == 0)     //Right
                    {
                        textx = obj.entities[i].xp - 16;
                        texty = obj.entities[i].yp + 26;
                    }
                }
            }
            else if (words[0] == "customposition")
            {
                //are we facing left or right? for some objects we don't care, default at 0.
                j = 0;

                //the first word is the object to position relative to
                if (words[1] == "player")
                {
                    i = obj.getcustomcrewman(0);
                    j = obj.entities[i].dir;
                }
                else if (words[1] == "cyan")
                {
                    i = obj.getcustomcrewman(0);
                    j = obj.entities[i].dir;
                }
                else if (words[1] == "purple")
                {
                    i = obj.getcustomcrewman(1);
                    j = obj.entities[i].dir;
                }
                else if (words[1] == "yellow")
                {
                    i = obj.getcustomcrewman(2);
                    j = obj.entities[i].dir;
                }
                else if (words[1] == "red")
                {
                    i = obj.getcustomcrewman(3);
                    j = obj.entities[i].dir;
                }
                else if (words[1] == "green")
                {
                    i = obj.getcustomcrewman(4);
                    j = obj.entities[i].dir;
                }
                else if (words[1] == "blue")
                {
                    i = obj.getcustomcrewman(5);
                    j = obj.entities[i].dir;
                }
                else if (words[1] == "centerx")
                {
                    words[2] = "donothing";
                    j = -1;
                    textx = -500;
                }
                else if (words[1] == "centery")
                {
                    words[2] = "donothing";
                    j = -1;
                    texty = -500;
                }
                else if (words[1] == "center")
                {
                    words[2] = "donothing";
                    j = -1;
                    textx = -500;
                    texty = -500;
                }

                if(i==0 && words[1]!="player" && words[1]!="cyan"){
                    //Requested crewmate is not actually on screen
                    words[2] = "donothing";
                    j = -1;
                    textx = -500;
                    texty = -500;
                }

                //next is whether to position above or below
                if (INBOUNDS_VEC(i, obj.entities) && words[2] == "above")
                {
                    if (j == 1)    //left
                    {
                        textx = obj.entities[i].xp -10000; //tells the box to be oriented correctly later
                        texty = obj.entities[i].yp - 16 - (txt.size() * font::height(PR_FONT_LEVEL));
                    }
                    else if (j == 0)     //Right
                    {
                        textx = obj.entities[i].xp - 16;
                        texty = obj.entities[i].yp - 18 - (txt.size() * font::height(PR_FONT_LEVEL));
                    }
                }
                else if (INBOUNDS_VEC(i, obj.entities))
                {
                    if (j == 1)    //left
                    {
                        textx = obj.entities[i].xp -10000; //tells the box to be oriented correctly later
                        texty = obj.entities[i].yp + 26;
                    }
                    else if (j == 0)     //Right
                    {
                        textx = obj.entities[i].xp - 16;
                        texty = obj.entities[i].yp + 26;
                    }
                }
            }
            else if (words[0] == "backgroundtext")
            {
                game.backgroundtext = true;
            }
            else if (words[0] == "textboxtimer")
            {
                textboxtimer = ss_toi(words[1]);
            }
            else if (words[0] == "flipme")
            {
                textflipme = !textflipme;
            }
            else if (words[0] == "speak_active" || words[0] == "speak")
            {
                //Ok, actually display the textbox we've initilised now!
                //If using "speak", don't make the textbox active (so we can use multiple textboxes)
                if (txt.empty())
                {
                    txt.resize(1);
                }
                graphics.createtextboxreal(txt[0], textx, texty, r, g, b, textflipme);
                textflipme = false;

                graphics.setlarge(textlarge);
                textlarge = false;

                if ((int) txt.size() > 1)
                {
                    for (i = 1; i < (int) txt.size(); i++)
                    {
                        graphics.addline(txt[i]);
                    }
                }

                if (textboxtimer > 0)
                {
                    graphics.textboxtimer(textboxtimer);
                }

                // Some textbox formatting that can be set by translations...
                if (textcentertext)
                {
                    graphics.textboxcentertext();
                }
                if (textpad_left > 0 || textpad_right > 0)
                {
                    graphics.textboxpad(textpad_left, textpad_right);
                }
                if (textpadtowidth > 0)
                {
                    graphics.textboxpadtowidth(textpadtowidth);
                }

                //the textbox cannot be outside the screen. Fix if it is.
                if (textx <= -1000)
                {
                    //position to the left of the player
                    textx += 10000;
                    textx -= graphics.textboxwidth();
                    textx += 16;
                    graphics.textboxmoveto(textx);
                }

                if (textx == -500 || textx == -1)
                {
                    graphics.textboxcenterx();
                }

                if (texty == -500)
                {
                    graphics.textboxcentery();
                }

                graphics.textboxadjust();
                if (words[0] == "speak_active")
                {
                    graphics.textboxactive();
                }

                if (!game.backgroundtext)
                {
                    game.advancetext = true;
                    game.hascontrol = false;
                    game.pausescript = true;
                    if (key.isDown(90) || key.isDown(32) || key.isDown(86)
                        || key.isDown(KEYBOARD_UP) || key.isDown(KEYBOARD_DOWN)) game.jumpheld = true;
                }
                game.backgroundtext = false;

                if (textbuttons)
                {
                    graphics.textboxbuttons();
                }
                textbuttons = false;
            }
            else if (words[0] == "endtext")
            {
                graphics.textboxremove();
                game.hascontrol = true;
                game.advancetext = false;
            }
            else if (words[0] == "endtextfast")
            {
                graphics.textboxremovefast();
                game.hascontrol = true;
                game.advancetext = false;
            }
            else if (words[0] == "do")
            {
                //right, loop from this point
                looppoint = position;
                loopcount = ss_toi(words[1]);
            }
            else if (words[0] == "loop")
            {
                //right, loop from this point
                loopcount--;
                if (loopcount > 0)
                {
                    position = looppoint;
                }
            }
            else if (words[0] == "vvvvvvman")
            {
                //Create the super VVVVVV combo!
                i = obj.getplayer();
                if (INBOUNDS_VEC(i, obj.entities))
                {
                    obj.entities[i].xp = 30;
                    obj.entities[i].yp = 46;
                    obj.entities[i].lerpoldxp = obj.entities[i].xp;
                    obj.entities[i].lerpoldyp = obj.entities[i].yp;
                    obj.entities[i].size = 13;
                    obj.entities[i].colour = 23;
                    obj.entities[i].cx = 36;// 6;
                    obj.entities[i].cy = 12+80;// 2;
                    obj.entities[i].h = 126-80;// 21;
                }
            }
            else if (words[0] == "undovvvvvvman")
            {
                //Create the super VVVVVV combo!
                i = obj.getplayer();
                if (INBOUNDS_VEC(i, obj.entities))
                {
                    obj.entities[i].xp = 100;
                    obj.entities[i].lerpoldxp = obj.entities[i].xp;
                    obj.entities[i].size = 0;
                    obj.entities[i].colour = 0;
                    obj.entities[i].cx = 6;
                    obj.entities[i].cy = 2;
                    obj.entities[i].h = 21;
                }
            }
            else if (words[0] == "createentity")
            {
                std::string word6 = words[6];
                std::string word7 = words[7];
                std::string word8 = words[8];
                std::string word9 = words[9];
                if (!argexists[6]) words[6] = "0";
                if (!argexists[7]) words[7] = "0";
                if (!argexists[8]) words[8] = "320";
                if (!argexists[9]) words[9] = "240";
                obj.createentity(
                    ss_toi(words[1]),
                    ss_toi(words[2]),
                    ss_toi(words[3]),
                    ss_toi(words[4]),
                    ss_toi(words[5]),
                    ss_toi(words[6]),
                    ss_toi(words[7]),
                    ss_toi(words[8]),
                    ss_toi(words[9])
                );
                words[6] = word6;
                words[7] = word7;
                words[8] = word8;
                words[9] = word9;
            }
            else if (words[0] == "createcrewman")
            {
                // Note: Do not change the "r" variable, it's used in custom levels
                // to have glitchy textbox colors, where the game treats the value
                // we set here as the red channel for the color.
                r = getcolorfromname(words[3]);
                if (r == -1) r = 19;

                //convert the command to the right index
                if (words[5] == "followplayer") words[5] = "10";
                if (words[5] == "followpurple") words[5] = "11";
                if (words[5] == "followyellow") words[5] = "12";
                if (words[5] == "followred") words[5] = "13";
                if (words[5] == "followgreen") words[5] = "14";
                if (words[5] == "followblue") words[5] = "15";

                if (words[5] == "followposition") words[5] = "16";
                if (words[5] == "faceleft")
                {
                    words[5] = "17";
                    words[6] = "0";
                }
                if (words[5] == "faceright")
                {
                    words[5] = "17";
                    words[6] = "1";
                }
                if (words[5] == "faceplayer")
                {
                    words[5] = "18";
                    words[6] = "0";
                }
                if (words[5] == "panic")
                {
                    words[5] = "20";
                    words[6] = "0";
                }

                if (ss_toi(words[5]) >= 16)
                {
                    obj.createentity(ss_toi(words[1]), ss_toi(words[2]), 18, r, ss_toi(words[4]), ss_toi(words[5]), ss_toi(words[6]));
                }
                else
                {
                    obj.createentity(ss_toi(words[1]), ss_toi(words[2]), 18, r, ss_toi(words[4]), ss_toi(words[5]));
                }
            }
            else if (words[0] == "changemood")
            {
                int crewmate = getcrewmanfromname(words[1]);
                if (crewmate != -1) i = crewmate; // Ensure AEM is kept

                if (INBOUNDS_VEC(i, obj.entities) && ss_toi(words[2]) == 0)
                {
                    obj.entities[i].tile = 0;
                }
                else if (INBOUNDS_VEC(i, obj.entities))
                {
                    obj.entities[i].tile = 144;
                }
            }
            else if (words[0] == "changecustommood")
            {
                if (words[1] == "player")
                {
                    i=obj.getcustomcrewman(0);
                    obj.customcrewmoods[0]=ss_toi(words[2]);
                }
                else if (words[1] == "cyan")
                {
                    i=obj.getcustomcrewman(0);
                    obj.customcrewmoods[0]=ss_toi(words[2]);
                }
                else if (words[1] == "customcyan")
                {
                    i=obj.getcustomcrewman(0);
                    obj.customcrewmoods[0]=ss_toi(words[2]);
                }
                else if (words[1] == "red")
                {
                    i=obj.getcustomcrewman(3);
                    obj.customcrewmoods[3]=ss_toi(words[2]);
                }
                else if (words[1] == "green")
                {
                    i=obj.getcustomcrewman(4);
                    obj.customcrewmoods[4]=ss_toi(words[2]);
                }
                else if (words[1] == "yellow")
                {
                    i=obj.getcustomcrewman(2);
                    obj.customcrewmoods[2]=ss_toi(words[2]);
                }
                else if (words[1] == "blue")
                {
                    i=obj.getcustomcrewman(5);
                    obj.customcrewmoods[5]=ss_toi(words[2]);
                }
                else if (words[1] == "purple")
                {
                    i=obj.getcustomcrewman(1);
                    obj.customcrewmoods[1]=ss_toi(words[2]);
                }
                else if (words[1] == "pink")
                {
                    i=obj.getcustomcrewman(1);
                    obj.customcrewmoods[1]=ss_toi(words[2]);
                }

                if (INBOUNDS_VEC(i, obj.entities) && ss_toi(words[2]) == 0)
                {
                    obj.entities[i].tile = 0;
                }
                else if (INBOUNDS_VEC(i, obj.entities))
                {
                    obj.entities[i].tile = 144;
                }
            }
            else if (words[0] == "changetile")
            {
                int crewmate = getcrewmanfromname(words[1]);
                if (crewmate != -1) i = crewmate; // Ensure AEM is kept

                if (INBOUNDS_VEC(i, obj.entities))
                {
                    obj.entities[i].tile = ss_toi(words[2]);
                }
            }
            else if (words[0] == "flipgravity")
            {
                //not something I'll use a lot, I think. Doesn't need to be very robust!
                if (words[1] == "player")
                {
                    game.gravitycontrol = !game.gravitycontrol;
                    ++game.totalflips;
                }
                else
                {
                    int crewmate = getcrewmanfromname(words[1]);
                    if (crewmate != -1) i = crewmate; // Ensure AEM is kept

                    if (INBOUNDS_VEC(i, obj.entities) && obj.entities[i].rule == 7)
                    {
                        obj.entities[i].rule = 6;
                        obj.entities[i].tile = 0;
                    }
                    else if (INBOUNDS_VEC(i, obj.entities) && obj.getplayer() != i) // Don't destroy player entity
                    {
                        obj.entities[i].rule = 7;
                        obj.entities[i].tile = 6;
                    }
                }
            }
            else if (words[0] == "changegravity")
            {
                //not something I'll use a lot, I think. Doesn't need to be very robust!
                int crewmate = getcrewmanfromname(words[1]);
                if (crewmate != -1) i = crewmate; // Ensure AEM is kept

                if (INBOUNDS_VEC(i, obj.entities))
                {
                    obj.entities[i].tile +=12;
                }
            }
            else if (words[0] == "changedir")
            {
                int crewmate = getcrewmanfromname(words[1]);
                if (crewmate != -1) i = crewmate; // Ensure AEM is kept

                if (INBOUNDS_VEC(i, obj.entities) && ss_toi(words[2]) == 0)
                {
                    obj.entities[i].dir = 0;
                }
                else if (INBOUNDS_VEC(i, obj.entities))
                {
                    obj.entities[i].dir = 1;
                }
            }
            else if (words[0] == "alarmon")
            {
                game.alarmon = true;
                game.alarmdelay = 0;
            }
            else if (words[0] == "alarmoff")
            {
                game.alarmon = false;
            }
            else if (words[0] == "changeai")
            {
                int crewmate = getcrewmanfromname(words[1]);
                if (crewmate != -1) i = crewmate; // Ensure AEM is kept

                if (words[2] == "followplayer") words[2] = "10";
                if (words[2] == "followpurple") words[2] = "11";
                if (words[2] == "followyellow") words[2] = "12";
                if (words[2] == "followred") words[2] = "13";
                if (words[2] == "followgreen") words[2] = "14";
                if (words[2] == "followblue") words[2] = "15";

                if (words[2] == "followposition") words[2] = "16";
                if (words[2] == "faceleft")
                {
                    words[2] = "17";
                    words[3] = "0";
                }
                if (words[2] == "faceright")
                {
                    words[2] = "17";
                    words[3] = "1";
                }


                if (INBOUNDS_VEC(i, obj.entities))
                {
                    obj.entities[i].state = ss_toi(words[2]);
                    if (obj.entities[i].state == 16)
                    {
                        obj.entities[i].para=ss_toi(words[3]);
                    }
                    else if (obj.entities[i].state == 17)
                    {
                        obj.entities[i].dir=ss_toi(words[3]);
                    }
                }
            }
            else if (words[0] == "activateteleporter")
            {
                i = obj.getteleporter();
                if (INBOUNDS_VEC(i, obj.entities))
                {
                    obj.entities[i].tile = 6;
                    obj.entities[i].colour = 102;
                }
            }
            else if (words[0] == "changecolour")
            {
                int crewmate = getcrewmanfromname(words[1]);
                if (crewmate != -1) i = crewmate; // Ensure AEM is kept

                if (INBOUNDS_VEC(i, obj.entities))
                {
                    obj.entities[i].colour = getcolorfromname(words[2]);
                }
            }
            else if (words[0] == "squeak")
            {
                if (words[1] == "player")
                {
                    music.playef(Sound_VIRIDIAN);
                }
                else if (words[1] == "cyan")
                {
                    music.playef(Sound_VIRIDIAN);
                }
                else if (words[1] == "red")
                {
                    music.playef(Sound_VERMILION);
                }
                else if (words[1] == "green")
                {
                    music.playef(Sound_VERDIGRIS);
                }
                else if (words[1] == "yellow")
                {
                    music.playef(Sound_VITELLARY);
                }
                else if (words[1] == "blue")
                {
                    music.playef(Sound_VICTORIA);
                }
                else if (words[1] == "purple")
                {
                    music.playef(Sound_VIOLET);
                }
                else if (words[1] == "cry")
                {
                    music.playef(Sound_CRY);
                }
                else if (words[1] == "terminal")
                {
                    music.playef(Sound_TERMINALTEXT);
                }
            }
            else if (words[0] == "blackout")
            {
                game.blackout = true;
            }
            else if (words[0] == "blackon")
            {
                game.blackout = false;
            }
            else if (words[0] == "setcheckpoint")
            {
                i = obj.getplayer();
                game.savepoint = 0;
                if (INBOUNDS_VEC(i, obj.entities))
                {
                    game.savex = obj.entities[i].xp ;
                    game.savey = obj.entities[i].yp;
                }
                game.savegc = game.gravitycontrol;
                game.saverx = game.roomx;
                game.savery = game.roomy;
                if (INBOUNDS_VEC(i, obj.entities))
                {
                    game.savedir = obj.entities[i].dir;
                }
            }
            else if (words[0] == "gamestate")
            {
                // Allow the gamestate command to bypass statelock, at least for now
                game.state = ss_toi(words[1]);
                game.statedelay = 0;
            }
            else if (words[0] == "textboxactive")
            {
                graphics.textboxactive();
            }
            else if (words[0] == "gamemode")
            {
                if (words[1] == "teleporter")
                {
                    game.gamestate = GAMEMODE; /* to set prevgamestate */
                    game.mapmenuchange(TELEPORTERMODE, false);

                    game.useteleporter = false; //good heavens don't actually use it
                }
                else if (words[1] == "game")
                {
                    graphics.resumegamemode = true;
                    game.prevgamestate = GAMEMODE;
                }
            }
            else if (words[0] == "ifexplored")
            {
                if (map.isexplored(ss_toi(words[1]), ss_toi(words[2])))
                {
                    loadalts(words[3], raw_words[3]);
                    position--;
                }
            }
            else if (words[0] == "iflast")
            {
                if (game.lastsaved==ss_toi(words[1]))
                {
                    loadalts(words[2], raw_words[2]);
                    position--;
                }
            }
            else if (words[0] == "ifskip")
            {
                if (game.nocutscenes)
                {
                    loadalts(words[1], raw_words[1]);
                    position--;
                }
            }
            else if (words[0] == "ifflag")
            {
                int flag = ss_toi(words[1]);
                if (INBOUNDS_ARR(flag, obj.flags) && obj.flags[flag])
                {
                    loadalts(words[2], raw_words[2]);
                    position--;
                }
            }
            else if (words[0] == "ifcrewlost")
            {
                int crewmate = ss_toi(words[1]);
                if (INBOUNDS_ARR(crewmate, game.crewstats) && !game.crewstats[crewmate])
                {
                    loadalts(words[2], raw_words[2]);
                    position--;
                }
            }
            else if (words[0] == "iftrinkets")
            {
                if (game.trinkets() >= ss_toi(words[1]))
                {
                    loadalts(words[2], raw_words[2]);
                    position--;
                }
            }
            else if (words[0] == "iftrinketsless")
            {
                if (game.stat_trinkets < ss_toi(words[1]))
                {
                    loadalts(words[2], raw_words[2]);
                    position--;
                }
            }
            else if (words[0] == "hidecoordinates")
            {
                map.setexplored(ss_toi(words[1]), ss_toi(words[2]), false);
            }
            else if (words[0] == "showcoordinates")
            {
                map.setexplored(ss_toi(words[1]), ss_toi(words[2]), true);
            }
            else if (words[0] == "hideship")
            {
                map.hideship();
            }
            else if (words[0] == "showship")
            {
                map.showship();
            }
            else if (words[0] == "showsecretlab")
            {
                map.setexplored(16, 5, true);
                map.setexplored(17, 5, true);
                map.setexplored(18, 5, true);
                map.setexplored(17, 6, true);
                map.setexplored(18, 6, true);
                map.setexplored(19, 6, true);
                map.setexplored(19, 7, true);
                map.setexplored(19, 8, true);
            }
            else if (words[0] == "hidesecretlab")
            {
                map.setexplored(16, 5, false);
                map.setexplored(17, 5, false);
                map.setexplored(18, 5, false);
                map.setexplored(17, 6, false);
                map.setexplored(18, 6, false);
                map.setexplored(19, 6, false);
                map.setexplored(19, 7, false);
                map.setexplored(19, 8, false);
            }
            else if (words[0] == "showteleporters")
            {
                map.showteleporters = true;
            }
            else if (words[0] == "showtargets")
            {
                map.showtargets = true;
            }
            else if (words[0] == "showtrinkets")
            {
                map.showtrinkets = true;
            }
            else if (words[0] == "hideteleporters")
            {
                map.showteleporters = false;
            }
            else if (words[0] == "hidetargets")
            {
                map.showtargets = false;
            }
            else if (words[0] == "hidetrinkets")
            {
                map.showtrinkets = false;
            }
            else if (words[0] == "hideplayer")
            {
                int player = obj.getplayer();
                if (INBOUNDS_VEC(player, obj.entities))
                {
                    obj.entities[player].invis = true;
                }
            }
            else if (words[0] == "showplayer")
            {
                int player = obj.getplayer();
                if (INBOUNDS_VEC(player, obj.entities))
                {
                    obj.entities[player].invis = false;
                }
            }
            else if (words[0] == "teleportscript")
            {
                game.teleportscript = words[1];
            }
            else if (words[0] == "clearteleportscript")
            {
                game.teleportscript = "";
            }
            else if (words[0] == "nocontrol")
            {
                game.hascontrol = false;
            }
            else if (words[0] == "hascontrol")
            {
                game.hascontrol = true;
            }
            else if (words[0] == "companion")
            {
                game.companion = ss_toi(words[1]);
            }
            else if (words[0] == "befadein")
            {
                graphics.setfade(0);
                graphics.fademode = FADE_NONE;
            }
            else if (words[0] == "fadein")
            {
                graphics.fademode = FADE_START_FADEIN;
            }
            else if (words[0] == "fadeout")
            {
                graphics.fademode = FADE_START_FADEOUT;
            }
            else if (words[0] == "untilfade")
            {
                if (FADEMODE_IS_FADING(graphics.fademode))
                {
                    scriptdelay = 1;
                    position--;
                }
            }
            else if (words[0] == "entersecretlab")
            {
                game.unlocknum(Unlock_SECRETLAB);
                game.insecretlab = true;
                SDL_memset(map.explored, true, sizeof(map.explored));
            }
            else if (words[0] == "leavesecretlab")
            {
                game.insecretlab = false;
            }
            else if (words[0] == "resetgame")
            {
                map.resetmap();
                map.resetplayer();
                graphics.towerbg.tdrawback = true;

                obj.resetallflags();
                i = obj.getplayer();
                if (INBOUNDS_VEC(i, obj.entities))
                {
                    obj.entities[i].tile = 0;
                }

                for (i = 0; i < 100; i++)
                {
                    obj.collect[i] = false;
                    obj.customcollect[i] = false;
                }
                game.deathcounts = 0;
                game.advancetext = false;
                game.hascontrol = true;
                game.resetgameclock();
                game.gravitycontrol = 0;
                game.teleport = false;
                game.companion = 0;
                game.teleport_to_new_area = false;
                game.teleport_to_x = 0;
                game.teleport_to_y = 0;

                game.teleportscript = "";

                //get out of final level mode!
                map.finalmode = false;
                map.final_colormode = false;
                map.final_mapcol = 0;
                map.final_colorframe = 0;
                map.finalstretch = false;
            }
            else if (words[0] == "loadscript")
            {
                loadalts(words[1], raw_words[1]);
                position--;
            }
            else if (words[0] == "rollcredits")
            {
                if (map.custommode && !map.custommodeforreal)
                {
                    game.returntoeditor();
                    ed.show_note(loc::gettext("Rolled credits"));
                }
                else
                {
                    game.gamestate = GAMECOMPLETE;
                    graphics.fademode = FADE_START_FADEIN;
                    game.creditposition = 0;
                    game.skip_message_timer = 1000;
                    game.old_skip_message_timer = 1000;
                }
            }
            else if (words[0] == "finalmode")
            {
                map.finalmode = true;
                map.gotoroom(ss_toi(words[1]), ss_toi(words[2]));
            }
            else if (words[0] == "rescued")
            {
                if (words[1] == "red")
                {
                    game.crewstats[3] = true;
                }
                else if (words[1] == "green")
                {
                    game.crewstats[4] = true;
                }
                else if (words[1] == "yellow")
                {
                    game.crewstats[2] = true;
                }
                else if (words[1] == "blue")
                {
                    game.crewstats[5] = true;
                }
                else if (words[1] == "purple")
                {
                    game.crewstats[1] = true;
                }
                else if (words[1] == "player")
                {
                    game.crewstats[0] = true;
                }
                else if (words[1] == "cyan")
                {
                    game.crewstats[0] = true;
                }
            }
            else if (words[0] == "missing")
            {
                if (words[1] == "red")
                {
                    game.crewstats[3] = false;
                }
                else if (words[1] == "green")
                {
                    game.crewstats[4] = false;
                }
                else if (words[1] == "yellow")
                {
                    game.crewstats[2] = false;
                }
                else if (words[1] == "blue")
                {
                    game.crewstats[5] = false;
                }
                else if (words[1] == "purple")
                {
                    game.crewstats[1] = false;
                }
                else if (words[1] == "player")
                {
                    game.crewstats[0] = false;
                }
                else if (words[1] == "cyan")
                {
                    game.crewstats[0] = false;
                }
            }
            else if (words[0] == "face")
            {
                int crewmate = getcrewmanfromname(words[1]);
                if (crewmate != -1) i = crewmate; // Ensure AEM is kept

                crewmate = getcrewmanfromname(words[2]);
                if (crewmate != -1) j = crewmate; // Ensure AEM is kept

                if (INBOUNDS_VEC(i, obj.entities) && INBOUNDS_VEC(j, obj.entities) && obj.entities[j].xp > obj.entities[i].xp + 5)
                {
                    obj.entities[i].dir = 1;
                }
                else if (INBOUNDS_VEC(i, obj.entities) && INBOUNDS_VEC(j, obj.entities) && obj.entities[j].xp < obj.entities[i].xp - 5)
                {
                    obj.entities[i].dir = 0;
                }
            }
            else if (words[0] == "jukebox")
            {
                for (j = 0; j < (int) obj.entities.size(); j++)
                {
                    if (obj.entities[j].type == 13)
                    {
                        obj.entities[j].colour = 4;
                    }
                }
                if (ss_toi(words[1]) == 1)
                {
                    obj.createblock(5, 88 - 4, 80, 20, 16, 25);
                    for (j = 0; j < (int) obj.entities.size(); j++)
                    {
                        if (obj.entities[j].xp == 88 && obj.entities[j].yp==80)
                        {
                            obj.entities[j].colour = 5;
                        }
                    }
                }
                else if (ss_toi(words[1]) == 2)
                {
                    obj.createblock(5, 128 - 4, 80, 20, 16, 26);
                    for (j = 0; j < (int) obj.entities.size(); j++)
                    {
                        if (obj.entities[j].xp == 128 && obj.entities[j].yp==80)
                        {
                            obj.entities[j].colour = 5;
                        }
                    }
                }
                else if (ss_toi(words[1]) == 3)
                {
                    obj.createblock(5, 176 - 4, 80, 20, 16, 27);
                    for (j = 0; j < (int) obj.entities.size(); j++)
                    {
                        if (obj.entities[j].xp == 176 && obj.entities[j].yp==80)
                        {
                            obj.entities[j].colour = 5;
                        }
                    }
                }
                else if (ss_toi(words[1]) == 4)
                {
                    obj.createblock(5, 216 - 4, 80, 20, 16, 28);
                    for (j = 0; j < (int) obj.entities.size(); j++)
                    {
                        if (obj.entities[j].xp == 216 && obj.entities[j].yp==80)
                        {
                            obj.entities[j].colour = 5;
                        }
                    }
                }
                else if (ss_toi(words[1]) == 5)
                {
                    obj.createblock(5, 88 - 4, 128, 20, 16, 29);
                    for (j = 0; j < (int) obj.entities.size(); j++)
                    {
                        if (obj.entities[j].xp == 88 && obj.entities[j].yp==128)
                        {
                            obj.entities[j].colour = 5;
                        }
                    }
                }
                else if (ss_toi(words[1]) == 6)
                {
                    obj.createblock(5, 176 - 4, 128, 20, 16, 30);
                    for (j = 0; j < (int) obj.entities.size(); j++)
                    {
                        if (obj.entities[j].xp == 176 && obj.entities[j].yp==128)
                        {
                            obj.entities[j].colour = 5;
                        }
                    }
                }
                else if (ss_toi(words[1]) == 7)
                {
                    obj.createblock(5, 40 - 4, 40, 20, 16, 31);
                    for (j = 0; j < (int) obj.entities.size(); j++)
                    {
                        if (obj.entities[j].xp == 40 && obj.entities[j].yp==40)
                        {
                            obj.entities[j].colour = 5;
                        }
                    }
                }
                else if (ss_toi(words[1]) == 8)
                {
                    obj.createblock(5, 216 - 4, 128, 20, 16, 32);
                    for (j = 0; j < (int) obj.entities.size(); j++)
                    {
                        if (obj.entities[j].xp == 216 && obj.entities[j].yp==128)
                        {
                            obj.entities[j].colour = 5;
                        }
                    }
                }
                else if (ss_toi(words[1]) == 9)
                {
                    obj.createblock(5, 128 - 4, 128, 20, 16, 33);
                    for (j = 0; j < (int) obj.entities.size(); j++)
                    {
                        if (obj.entities[j].xp == 128 && obj.entities[j].yp==128)
                        {
                            obj.entities[j].colour = 5;
                        }
                    }
                }
                else if (ss_toi(words[1]) == 10)
                {
                    obj.createblock(5, 264 - 4, 40, 20, 16, 34);
                    for (j = 0; j < (int) obj.entities.size(); j++)
                    {
                        if (obj.entities[j].xp == 264 && obj.entities[j].yp==40)
                        {
                            obj.entities[j].colour = 5;
                        }
                    }
                }
            }
            else if (words[0] == "createactivityzone")
            {
                int crew_color = i; // stay consistent with past behavior!
                if (words[1] == "red")
                {
                    i = 3;
                    crew_color = RED;
                }
                else if (words[1] == "green")
                {
                    i = 4;
                    crew_color = GREEN;
                }
                else if (words[1] == "yellow")
                {
                    i = 2;
                    crew_color = YELLOW;
                }
                else if (words[1] == "blue")
                {
                    i = 5;
                    crew_color = BLUE;
                }
                else if (words[1] == "purple")
                {
                    i = 1;
                    crew_color = PURPLE;
                }

                int crewman = obj.getcrewman(crew_color);
                if (INBOUNDS_VEC(crewman, obj.entities) && crew_color == GREEN)
                {
                    obj.createblock(5, obj.entities[crewman].xp - 32, obj.entities[crewman].yp-20, 96, 60, i, "", (i == 35));
                }
                else if (INBOUNDS_VEC(crewman, obj.entities))
                {
                    obj.createblock(5, obj.entities[crewman].xp - 32, 0, 96, 240, i, "", (i == 35));
                }
            }
            else if (words[0] == "setactivitycolour")
            {
                obj.customactivitycolour = words[1];
            }
            else if (words[0] == "setactivitytext")
            {
                ++position;
                if (INBOUNDS_VEC(position, commands))
                {
                    obj.customactivitytext = commands[position];
                }
            }
            else if (words[0] == "setactivityposition")
            {
                obj.customactivitypositiony = ss_toi(words[1]);
            }
            else if (words[0] == "createrescuedcrew")
            {
                //special for final level cutscene
                //starting at 180, create the rescued crewmembers (ingoring violet, who's at 155)
                i = 215;
                if (game.crewstats[2] && game.lastsaved!=2)
                {
                    obj.createentity(i, 153, 18, 14, 0, 17, 0);
                    i += 25;
                }
                if (game.crewstats[3] && game.lastsaved!=3)
                {
                    obj.createentity(i, 153, 18, 15, 0, 17, 0);
                    i += 25;
                }
                if (game.crewstats[4] && game.lastsaved!=4)
                {
                    obj.createentity(i, 153, 18, 13, 0, 17, 0);
                    i += 25;
                }
                if (game.crewstats[5] && game.lastsaved!=5)
                {
                    obj.createentity(i, 153, 18, 16, 0, 17, 0);
                    i += 25;
                }
            }
            else if (words[0] == "restoreplayercolour")
            {
                i = obj.getplayer();
                if (INBOUNDS_VEC(i, obj.entities))
                {
                    obj.entities[i].colour = 0;
                }
            }
            else if (words[0] == "changeplayercolour")
            {
                i = obj.getplayer();

                if (INBOUNDS_VEC(i, obj.entities))
                {
                    obj.entities[i].colour = getcolorfromname(words[1]);
                }
            }
            else if (words[0] == "changerespawncolour")
            {
                game.savecolour = getcolorfromname(words[1]);
            }
            else if (words[0] == "altstates")
            {
                obj.altstates = ss_toi(words[1]);
            }
            else if (words[0] == "activeteleporter")
            {
                i = obj.getteleporter();
                if (INBOUNDS_VEC(i, obj.entities))
                {
                    obj.entities[i].colour = 101;
                }
            }
            else if (words[0] == "foundtrinket")
            {
                music.silencedasmusik();
                music.playef(Sound_TRINKET);

                size_t trinket = ss_toi(words[1]);
                if (trinket < SDL_arraysize(obj.collect))
                {
                    obj.collect[trinket] = true;
                }

                graphics.textboxremovefast();

                graphics.createtextboxflipme(loc::gettext("Congratulations!\n\nYou have found a shiny trinket!"), 50, 85, TEXT_COLOUR("gray"));
                graphics.textboxprintflags(PR_FONT_INTERFACE);
                int h = graphics.textboxwrap(2);
                graphics.textboxcentertext();
                graphics.textboxpad(1, 1);
                graphics.textboxcenterx();

                int max_trinkets;

                if (map.custommode)
                {
                    max_trinkets = cl.numtrinkets();
                }
                else
                {
                    max_trinkets = 20;
                }

                char buffer[SCREEN_WIDTH_CHARS + 1];
                vformat_buf(
                    buffer, sizeof(buffer),
                    loc::gettext("{n_trinkets|wordy} out of {max_trinkets|wordy}"),
                    "n_trinkets:int, max_trinkets:int",
                    game.trinkets(), max_trinkets
                );
                graphics.createtextboxflipme(buffer, 50, 95+h, TEXT_COLOUR("gray"));
                graphics.textboxprintflags(PR_FONT_INTERFACE);
                graphics.textboxwrap(2);
                graphics.textboxcentertext();
                graphics.textboxpad(1, 1);
                graphics.textboxcenterx();

                if (!game.backgroundtext)
                {
                    game.advancetext = true;
                    game.hascontrol = false;
                    game.pausescript = true;
                    if (key.isDown(90) || key.isDown(32) || key.isDown(86)
                        || key.isDown(KEYBOARD_UP) || key.isDown(KEYBOARD_DOWN)) game.jumpheld = true;
                }
                game.backgroundtext = false;
            }
            else if (words[0] == "foundlab")
            {
                music.playef(Sound_TRINKET);

                graphics.textboxremovefast();

                graphics.createtextbox(loc::gettext("Congratulations!\n\nYou have found the secret lab!"), 50, 85, TEXT_COLOUR("gray"));
                graphics.textboxprintflags(PR_FONT_INTERFACE);
                graphics.textboxwrap(2);
                graphics.textboxcentertext();
                graphics.textboxpad(1, 1);
                graphics.textboxcenterx();
                graphics.textboxcentery();

                if (!game.backgroundtext)
                {
                    game.advancetext = true;
                    game.hascontrol = false;
                    game.pausescript = true;
                    if (key.isDown(90) || key.isDown(32) || key.isDown(86)
                        || key.isDown(KEYBOARD_UP) || key.isDown(KEYBOARD_DOWN)) game.jumpheld = true;
                }
                game.backgroundtext = false;
            }
            else if (words[0] == "foundlab2")
            {
                graphics.textboxremovefast();

                graphics.createtextbox(loc::gettext("The secret lab is separate from the rest of the game. You can now come back here at any time by selecting the new SECRET LAB option in the play menu."), 50, 85, TEXT_COLOUR("gray"));
                graphics.textboxprintflags(PR_FONT_INTERFACE);
                graphics.textboxwrap(0);
                graphics.textboxcenterx();
                graphics.textboxcentery();

                if (!game.backgroundtext)
                {
                    game.advancetext = true;
                    game.hascontrol = false;
                    game.pausescript = true;
                    if (key.isDown(90) || key.isDown(32) || key.isDown(86)
                        || key.isDown(KEYBOARD_UP) || key.isDown(KEYBOARD_DOWN)) game.jumpheld = true;
                }
                game.backgroundtext = false;
            }
            else if (words[0] == "everybodysad")
            {
                for (i = 0; i < (int) obj.entities.size(); i++)
                {
                    if (obj.entities[i].rule == 6 || obj.entities[i].rule == 0)
                    {
                        obj.entities[i].tile = 144;
                    }
                }
            }
            else if (words[0] == "startintermission2")
            {
                map.finalmode = true; //Enable final level mode

                game.savex = 228;
                game.savey = 129;
                game.saverx = 53;
                game.savery = 49;
                game.savegc = 0;
                game.savedir = 0; //Intermission level 2
                game.savepoint = 0;
                game.gravitycontrol = 0;

                map.gotoroom(46, 54);
            }
            else if (words[0] == "telesave")
            {
                if (!game.intimetrial && !game.nodeathmode && !game.inintermission) game.savetele();
            }
            else if (words[0] == "createlastrescued")
            {
                r = graphics.crewcolour(game.lastsaved);
                if (r == 0 || r == PURPLE)
                {
                    r = GRAY; // Default to gray if invalid color.
                }

                obj.createentity(200, 153, 18, r, 0, 19, 30);
                i = obj.getcrewman(game.lastsaved);
                if (INBOUNDS_VEC(i, obj.entities))
                {
                    obj.entities[i].dir = 1;
                }
            }
            else if (words[0] == "specialline")
            {
                //Localization is handled with regular cutscene dialogue
                switch(ss_toi(words[1]))
                {
                case 1:
                    txt.resize(1);

                    txt[0] = "I'm worried about " + game.unrescued() + ", Doctor!";
                    break;
                case 2:
                    txt.resize(3);

                    if (game.crewrescued() < 5)
                    {
                        txt[1] = "to helping you find the";
                        txt[2] = "rest of the crew!";
                    }
                    else
                    {
                        txt.resize(2);
                        txt[1] = "to helping you find " + game.unrescued() + "!";
                    }
                    break;
                }

                translate_dialogue();
            }
            else if (words[0] == "trinketbluecontrol")
            {
                if (game.trinkets() == 20 && obj.flags[67])
                {
                    load("talkblue_trinket6");
                    position--;
                }
                else if (game.trinkets() >= 19 && !obj.flags[67])
                {
                    load("talkblue_trinket5");
                    position--;
                }
                else
                {
                    load("talkblue_trinket4");
                    position--;
                }
            }
            else if (words[0] == "trinketyellowcontrol")
            {
                if (game.trinkets() >= 19)
                {
                    load("talkyellow_trinket3");
                    position--;
                }
                else
                {
                    load("talkyellow_trinket2");
                    position--;
                }
            }
            else if (words[0] == "redcontrol")
            {
                if (game.insecretlab)
                {
                    load("talkred_14");
                    position--;
                }
                else    if (game.roomx != 104)
                {
                    if (game.roomx == 100)
                    {
                        load("talkred_10");
                        position--;
                    }
                    else if (game.roomx == 107)
                    {
                        load("talkred_11");
                        position--;
                    }
                    else if (game.roomx == 114)
                    {
                        load("talkred_12");
                        position--;
                    }
                }
                else if (obj.flags[67])
                {
                    //game complete
                    load("talkred_13");
                    position--;
                }
                else if (obj.flags[35] && !obj.flags[52])
                {
                    //Intermission level
                    obj.flags[52] = true;
                    load("talkred_9");
                    position--;
                }
                else if (!obj.flags[51])
                {
                    //We're back home!
                    obj.flags[51] = true;
                    load("talkred_5");
                    position--;
                }
                else if (!obj.flags[48] && game.crewstats[5])
                {
                    //Victoria's back
                    obj.flags[48] = true;
                    load("talkred_6");
                    position--;
                }
                else if (!obj.flags[49] && game.crewstats[4])
                {
                    //Verdigris' back
                    obj.flags[49] = true;
                    load("talkred_7");
                    position--;
                }
                else if (!obj.flags[50] && game.crewstats[2])
                {
                    //Vitellary's back
                    obj.flags[50] = true;
                    load("talkred_8");
                    position--;
                }
                else if (!obj.flags[45] && !game.crewstats[5])
                {
                    obj.flags[45] = true;
                    load("talkred_2");
                    position--;
                }
                else if (!obj.flags[46] && !game.crewstats[4])
                {
                    obj.flags[46] = true;
                    load("talkred_3");
                    position--;
                }
                else if (!obj.flags[47] && !game.crewstats[2])
                {
                    obj.flags[47] = true;
                    load("talkred_4");
                    position--;
                }
                else
                {
                    obj.flags[45] = false;
                    obj.flags[46] = false;
                    obj.flags[47] = false;
                    load("talkred_1");
                    position--;
                }
            }
            //TODO: Non Urgent fix compiler nesting errors without adding complexity
            if (words[0] == "greencontrol")
            {
                if (game.insecretlab)
                {
                    load("talkgreen_11");
                    position--;
                }
                else    if (game.roomx == 103 && game.roomy == 109)
                {
                    load("talkgreen_8");
                    position--;
                }
                else if (game.roomx == 101 && game.roomy == 109)
                {
                    load("talkgreen_9");
                    position--;
                }
                else if (obj.flags[67])
                {
                    //game complete
                    load("talkgreen_10");
                    position--;
                }
                else if (obj.flags[34] && !obj.flags[57])
                {
                    //Intermission level
                    obj.flags[57] = true;
                    load("talkgreen_7");
                    position--;
                }
                else if (!obj.flags[53])
                {
                    //Home!
                    obj.flags[53] = true;
                    load("talkgreen_6");
                    position--;
                }
                else if (!obj.flags[54] && game.crewstats[2])
                {
                    obj.flags[54] = true;
                    load("talkgreen_5");
                    position--;
                }
                else if (!obj.flags[55] && game.crewstats[3])
                {
                    obj.flags[55] = true;
                    load("talkgreen_4");
                    position--;
                }
                else if (!obj.flags[56] && game.crewstats[5])
                {
                    obj.flags[56] = true;
                    load("talkgreen_3");
                    position--;
                }
                else if (!obj.flags[58])
                {
                    obj.flags[58] = true;
                    load("talkgreen_2");
                    position--;
                }
                else
                {
                    load("talkgreen_1");
                    position--;
                }
            }
            else if (words[0] == "bluecontrol")
            {
                if (game.insecretlab)
                {
                    load("talkblue_9");
                    position--;
                }
                else    if (obj.flags[67])
                {
                    //game complete, everything changes for victoria
                    if (obj.flags[41] && !obj.flags[42])
                    {
                        //second trinket conversation
                        obj.flags[42] = true;
                        load("talkblue_trinket2");
                        position--;
                    }
                    else if (!obj.flags[41] && !obj.flags[42])
                    {
                        //Third trinket conversation
                        obj.flags[42] = true;
                        load("talkblue_trinket3");
                        position--;
                    }
                    else
                    {
                        //Ok, we've already dealt with the trinket thing; so either you have them all, or you don't. If you do:
                        if (game.trinkets() >= 20)
                        {
                            load("startepilogue");
                            position--;
                        }
                        else
                        {
                            load("talkblue_8");
                            position--;
                        }
                    }
                }
                else if (obj.flags[33] && !obj.flags[40])
                {
                    //Intermission level
                    obj.flags[40] = true;
                    load("talkblue_7");
                    position--;
                }
                else if (!obj.flags[36] && game.crewstats[5])
                {
                    //Back on the ship!
                    obj.flags[36] = true;
                    load("talkblue_3");
                    position--;
                }
                else if (!obj.flags[41] && game.crewrescued() <= 4)
                {
                    //First trinket conversation
                    obj.flags[41] = true;
                    load("talkblue_trinket1");
                    position--;
                }
                else if (obj.flags[41] && !obj.flags[42] && game.crewrescued() == 5)
                {
                    //second trinket conversation
                    obj.flags[42] = true;
                    load("talkblue_trinket2");
                    position--;
                }
                else if (!obj.flags[41] && !obj.flags[42] && game.crewrescued() == 5)
                {
                    //Third trinket conversation
                    obj.flags[42] = true;
                    load("talkblue_trinket3");
                    position--;
                }
                else if (!obj.flags[37] && game.crewstats[2])
                {
                    obj.flags[37] = true;
                    load("talkblue_4");
                    position--;
                }
                else if (!obj.flags[38] && game.crewstats[3])
                {
                    obj.flags[38] = true;
                    load("talkblue_5");
                    position--;
                }
                else if (!obj.flags[39] && game.crewstats[4])
                {
                    obj.flags[39] = true;
                    load("talkblue_6");
                    position--;
                }
                else
                {
                    //if all else fails:
                    //if yellow is found
                    if (game.crewstats[2])
                    {
                        load("talkblue_2");
                        position--;
                    }
                    else
                    {
                        load("talkblue_1");
                        position--;
                    }
                }
            }
            else if (words[0] == "yellowcontrol")
            {
                if (game.insecretlab)
                {
                    load("talkyellow_12");
                    position--;
                }
                else    if (obj.flags[67])
                {
                    //game complete
                    load("talkyellow_11");
                    position--;
                }
                else if (obj.flags[32] && !obj.flags[31])
                {
                    //Intermission level
                    obj.flags[31] = true;
                    load("talkyellow_6");
                    position--;
                }
                else if (!obj.flags[27] && game.crewstats[2])
                {
                    //Back on the ship!
                    obj.flags[27] = true;
                    load("talkyellow_10");
                    position--;
                }
                else if (!obj.flags[43] && game.crewrescued() == 5 && !game.crewstats[5])
                {
                    //If by chance we've rescued everyone except Victoria by the end, Vitellary provides you with
                    //the trinket information instead.
                    obj.flags[43] = true;
                    obj.flags[42] = true;
                    obj.flags[41] = true;
                    load("talkyellow_trinket1");
                    position--;
                }
                else if (!obj.flags[24] && game.crewstats[5])
                {
                    obj.flags[24] = true;
                    load("talkyellow_8");
                    position--;
                }
                else if (!obj.flags[26] && game.crewstats[4])
                {
                    obj.flags[26] = true;
                    load("talkyellow_7");
                    position--;
                }
                else if (!obj.flags[25] && game.crewstats[3])
                {
                    obj.flags[25] = true;
                    load("talkyellow_9");
                    position--;
                }
                else if (!obj.flags[28])
                {
                    obj.flags[28] = true;
                    load("talkyellow_3");
                    position--;
                }
                else if (!obj.flags[29])
                {
                    obj.flags[29] = true;
                    load("talkyellow_4");
                    position--;
                }
                else if (!obj.flags[30])
                {
                    obj.flags[30] = true;
                    load("talkyellow_5");
                    position--;
                }
                else if (!obj.flags[23])
                {
                    obj.flags[23] = true;
                    load("talkyellow_2");
                    position--;
                }
                else
                {
                    load("talkyellow_1");
                    position--;
                    obj.flags[23] = false;
                }
            }
            else if (words[0] == "purplecontrol")
            {
                //Controls Purple's conversion
                //Crew rescued:
                if (game.insecretlab)
                {
                    load("talkpurple_9");
                    position--;
                }
                else    if (obj.flags[67])
                {
                    //game complete
                    load("talkpurple_8");
                    position--;
                }
                else if (!obj.flags[17] && game.crewstats[4])
                {
                    obj.flags[17] = true;
                    load("talkpurple_6");
                    position--;
                }
                else if (!obj.flags[15] && game.crewstats[5])
                {
                    obj.flags[15] = true;
                    load("talkpurple_4");
                    position--;
                }
                else if (!obj.flags[16] && game.crewstats[3])
                {
                    obj.flags[16] = true;
                    load("talkpurple_5");
                    position--;
                }
                else if (!obj.flags[18] && game.crewstats[2])
                {
                    obj.flags[18] = true;
                    load("talkpurple_7");
                    position--;
                }
                else if (obj.flags[19] && !obj.flags[20] && !obj.flags[21])
                {
                    //intermission one: if played one / not had first conversation / not played two [conversation one]
                    obj.flags[21] = true;
                    load("talkpurple_intermission1");
                    position--;
                }
                else if (obj.flags[20] && obj.flags[21] && !obj.flags[22])
                {
                    //intermission two: if played two / had first conversation / not had second conversation [conversation two]
                    obj.flags[22] = true;
                    load("talkpurple_intermission2");
                    position--;
                }
                else if (obj.flags[20] && !obj.flags[21] && !obj.flags[22])
                {
                    //intermission two: if played two / not had first conversation / not had second conversation [conversation three]
                    obj.flags[22] = true;
                    load("talkpurple_intermission3");
                    position--;
                }
                else if (!obj.flags[12])
                {
                    //Intro conversation
                    obj.flags[12] = true;
                    load("talkpurple_intro");
                    position--;
                }
                else if (!obj.flags[14])
                {
                    //Shorter intro conversation
                    obj.flags[14] = true;
                    load("talkpurple_3");
                    position--;
                }
                else
                {
                    //if all else fails:
                    //if green is found
                    if (game.crewstats[4])
                    {
                        load("talkpurple_2");
                        position--;
                    }
                    else
                    {
                        load("talkpurple_1");
                        position--;
                    }
                }
            }
            else if (words[0] == "textbuttons")
            {
                // Parse buttons in the next textbox
                textbuttons = true;
            }
            else if (words[0] == "textcase")
            {
                // Used to disambiguate identical textboxes for translations (1 by default)
                textcase = ss_toi(words[1]);
            }
            else if (words[0] == "loadtext")
            {
                if (map.custommode)
                {
                    loc::lang_custom = raw_words[1];
                    loc::loadtext_custom(NULL);
                }
            }
            else if (words[0] == "iflang")
            {
                if (loc::lang == raw_words[1])
                {
                    loadalts("custom_" + words[2], "custom_" + raw_words[2]);
                    position--;
                }
            }
            else if (words[0] == "setfont")
            {
                // If any textbox is currently fading out, wait for that first
                bool blocked = false;
                for (size_t i = 0; i < graphics.textboxes.size(); i++)
                {
                    if (graphics.textboxes[i].tm == 2)
                    {
                        scriptdelay = 1;
                        position--;
                        blocked = true;
                        break;
                    }
                }

                if (!blocked)
                {
                    if (words[1] == "")
                    {
                        font::set_level_font(cl.level_font_name.c_str());
                    }
                    else
                    {
                        font::set_level_font(raw_words[1].c_str());
                    }
                }
            }

            position++;
        }
        else
        {
            running = false;
        }
        // Don't increment if we're at the max, signed int overflow is UB
        if (execution_counter == SHRT_MAX)
        {
            // We must be in an infinite loop
            vlog_warn("Warning: execution counter got to %i, stopping script", SHRT_MAX);
            running = false;
        }
        else
        {
            execution_counter++;
        }
    }

    if(scriptdelay>0)
    {
        scriptdelay--;
    }
}

void scriptclass::translate_dialogue(void)
{
    char tc = textcase;
    textcase = 1;

    if (!loc::is_cutscene_translated(scriptname))
    {
        return;
    }

    // English text needs to be un-wordwrapped, translated, and re-wordwrapped
    std::string eng;
    for (size_t i = 0; i < txt.size(); i++)
    {
        if (i != 0)
        {
            eng.append("\n");
        }
        eng.append(txt[i]);
    }

    eng = font::string_unwordwrap(eng);
    const loc::TextboxFormat* format = loc::gettext_cutscene(scriptname, eng, tc);
    if (format == NULL || format->text == NULL || format->text[0] == '\0')
    {
        return;
    }
    std::string tra;
    if (format->tt)
    {
        tra = std::string(format->text);
        size_t pipe;
        while (true)
        {
            pipe = tra.find('|', 0);
            if (pipe == std::string::npos)
            {
                break;
            }
            tra.replace(pipe, 1, "\n");
        }
    }
    else
    {
        tra = font::string_wordwrap_balanced(PR_FONT_LEVEL, format->text, format->wraplimit);
    }

    textcentertext = format->centertext;
    textpad_left = format->pad_left;
    textpad_right = format->pad_right;
    textpadtowidth = format->padtowidth;

    txt.clear();
    size_t startline = 0;
    size_t newline;
    do {
        newline = tra.find('\n', startline);
        txt.push_back(tra.substr(startline, newline-startline));
        startline = newline+1;
    } while (newline != std::string::npos);
}

static void gotoerrorloadinglevel(void)
{
    game.quittomenu();
    game.createmenu(Menu::errorloadinglevel);
    music.currentsong = -1; /* otherwise music.play won't work */
    music.play(Music_PRESENTINGVVVVVV);
}

#define DECLARE_MODE_FUNC(funcname, modename) \
    static bool funcname(const enum StartMode mode) \
    { \
        return mode >= Start_FIRST_##modename && mode <= Start_LAST_##modename; \
    }

DECLARE_MODE_FUNC(is_no_death_mode, NODEATHMODE)
DECLARE_MODE_FUNC(is_intermission_1, INTERMISSION1)
DECLARE_MODE_FUNC(is_intermission_2, INTERMISSION2)

#undef DECLARE_MODE_FUNC

void scriptclass::startgamemode(const enum StartMode mode)
{
    if (mode == Start_QUIT)
    {
        VVV_exit(0);
    }

    struct
    {
        bool initialized;
        int size;
        int cx;
        int cy;
        int w;
        int h;
    }
    player_hitbox;
    SDL_zero(player_hitbox);

    if (GlitchrunnerMode_less_than_or_equal(Glitchrunner2_2))
    {
        /* Preserve player hitbox */
        const int player_idx = obj.getplayer();
        if (INBOUNDS_VEC(player_idx, obj.entities))
        {
            const entclass* player = &obj.entities[player_idx];
            player_hitbox.initialized = true;
            player_hitbox.size = player->size;
            player_hitbox.cx = player->cx;
            player_hitbox.cy = player->cy;
            player_hitbox.w = player->w;
            player_hitbox.h = player->h;
        }
    }

    /* Containers which need to be reset before gameplay starts
     * ex. before custom levels get loaded */

    switch (mode)
    {
    case Start_EDITORPLAYTESTING:
        break;
    default:
        textbox_colours.clear();
        add_default_colours();
        break;
    }

    hardreset();

    if (mode == Start_EDITOR)
    {
        game.gamestate = EDITORMODE;
    }
    else
    {
        game.gamestate = GAMEMODE;
    }

    // Font handling
    switch (mode)
    {
    case Start_EDITORPLAYTESTING:
    case Start_CUSTOM:
    case Start_CUSTOM_QUICKSAVE:
        break;
    case Start_EDITOR:
        font::set_level_font_new();
        break;
    default:
        font::set_level_font_interface();
    }

    game.jumpheld = true;

    switch (mode)
    {
    case Start_MAINGAME:
    case Start_MAINGAME_TELESAVE:
    case Start_MAINGAME_QUICKSAVE:
    case Start_NODEATHMODE_WITHCUTSCENES:
    case Start_NODEATHMODE_NOCUTSCENES:
        game.nodeathmode = is_no_death_mode(mode);
        game.nocutscenes = (mode == Start_NODEATHMODE_NOCUTSCENES);

        game.start();

        switch (mode)
        {
        case Start_MAINGAME_TELESAVE:
            game.loadtele();
            graphics.fademode = FADE_START_FADEIN;
            break;
        case Start_MAINGAME_QUICKSAVE:
            game.loadquick();
            graphics.fademode = FADE_START_FADEIN;
            break;
        default:
            graphics.showcutscenebars = true;
            graphics.setbars(320);
            load("intro");
        }
        break;

    case Start_TIMETRIAL_SPACESTATION1:
    case Start_TIMETRIAL_LABORATORY:
    case Start_TIMETRIAL_TOWER:
    case Start_TIMETRIAL_SPACESTATION2:
    case Start_TIMETRIAL_WARPZONE:
    case Start_TIMETRIAL_FINALLEVEL:
        music.fadeout();

        game.nocutscenes = true;
        game.intimetrial = true;
        game.timetrialcountdown = 150;
        game.timetriallevel = mode - Start_FIRST_TIMETRIAL;

        if (map.invincibility)
        {
            game.sabotage_time_trial();
        }

        switch (mode)
        {
        case Start_TIMETRIAL_SPACESTATION1:
            game.timetrialpar = 75;
            game.timetrialshinytarget = 2;
            break;
        case Start_TIMETRIAL_LABORATORY:
            game.timetrialpar = 165;
            game.timetrialshinytarget = 4;
            break;
        case Start_TIMETRIAL_TOWER:
            game.timetrialpar = 105;
            game.timetrialshinytarget = 2;
            break;
        case Start_TIMETRIAL_SPACESTATION2:
            game.timetrialpar = 200;
            game.timetrialshinytarget = 5;
            break;
        case Start_TIMETRIAL_WARPZONE:
            game.timetrialpar = 120;
            game.timetrialshinytarget = 1;
            break;
        case Start_TIMETRIAL_FINALLEVEL:
            game.timetrialpar = 135;
            game.timetrialshinytarget = 1;
            map.finalmode = true;
            map.final_colormode = false;
            map.final_mapcol = 0;
            map.final_colorframe = 0;
            break;
        default:
            VVV_unreachable();
        }

        game.starttrial(game.timetriallevel);

        if (game.translator_exploring)
        {
            game.timetrialcountdown = 0;
            game.timetrialparlost = true;
            SDL_memset(map.explored, true, sizeof(map.explored));
        }

        graphics.fademode = FADE_START_FADEIN;
        break;

    case Start_SECRETLAB:
        game.startspecial(0);

        /* Unlock the entire map */
        SDL_memset(obj.collect, true, sizeof(obj.collect[0]) * 20);
        /* Give all 20 trinkets */
        SDL_memset(map.explored, true, sizeof(map.explored));
        i = 400; /* previously a nested for-loop set this */
        game.insecretlab = true;
        map.showteleporters = true;

        music.play(Music_PIPEDREAM);
        graphics.fademode = FADE_START_FADEIN;
        break;

    case Start_INTERMISSION1_VITELLARY:
    case Start_INTERMISSION1_VERMILION:
    case Start_INTERMISSION1_VERDIGRIS:
    case Start_INTERMISSION1_VICTORIA:
    case Start_INTERMISSION2_VITELLARY:
    case Start_INTERMISSION2_VERMILION:
    case Start_INTERMISSION2_VERDIGRIS:
    case Start_INTERMISSION2_VICTORIA:
        music.fadeout();

        switch (mode)
        {
        case Start_INTERMISSION1_VITELLARY:
        case Start_INTERMISSION2_VITELLARY:
            game.lastsaved = 2;
            break;
        case Start_INTERMISSION1_VERMILION:
        case Start_INTERMISSION2_VERMILION:
            game.lastsaved = 3;
            break;
        case Start_INTERMISSION1_VERDIGRIS:
        case Start_INTERMISSION2_VERDIGRIS:
            game.lastsaved = 4;
            break;
        case Start_INTERMISSION1_VICTORIA:
        case Start_INTERMISSION2_VICTORIA:
            game.lastsaved = 5;
            break;
        default:
            VVV_unreachable();
        }

        game.crewstats[game.lastsaved] = true;
        game.inintermission = true;

        if (is_intermission_1(mode))
        {
            game.companion = 11;
            game.supercrewmate = true;
            game.scmprogress = 0;
        }

        map.finalmode = true;
        map.final_colormode = false;
        map.final_mapcol = 0;
        map.final_colorframe = 0;
        game.startspecial(1);

        if (is_intermission_1(mode))
        {
            load("intermission_1");
        }
        else if (is_intermission_2(mode))
        {
            load("intermission_2");
        }
        break;

    case Start_EDITOR:
        cl.reset();
        ed.reset();
        music.fadeout();
        map.custommode = true;
        map.custommodeforreal = false;
        graphics.fademode = FADE_START_FADEIN;
        break;

    case Start_EDITORPLAYTESTING:
        music.fadeout();

        //If warpdir() is used during playtesting, we need to set it back after!
        for (int j = 0; j < cl.maxheight; j++)
        {
            for (int i = 0; i < cl.maxwidth; i++)
            {
                ed.kludgewarpdir[i+(j*cl.maxwidth)]=cl.roomproperties[i+(j*cl.maxwidth)].warpdir;
            }
        }

        game.customstart();
        ed.ghosts.clear();

        map.custommode = true;
        map.custommodeforreal = false;
        map.customshowmm = true;

        if (cl.levmusic > 0)
        {
            music.play(cl.levmusic);
        }
        else
        {
            music.currentsong = -1;
        }
        break;

    case Start_CUSTOM:
    case Start_CUSTOM_QUICKSAVE:
    {
        map.custommodeforreal = true;
        map.custommode = true;

        std::string filename = std::string(cl.ListOfMetaData[game.playcustomlevel].filename);
        if (!cl.load(filename))
        {
            gotoerrorloadinglevel();
            return;
        }
        cl.findstartpoint();

        map.customshowmm = true;

        music.fadeout();
        game.customstart();

        switch (mode)
        {
        case Start_CUSTOM:
            if (cl.levmusic > 0)
            {
                music.play(cl.levmusic);
            }
            else
            {
                music.currentsong = -1;
            }
            break;
        case Start_CUSTOM_QUICKSAVE:
            game.customloadquick(cl.ListOfMetaData[game.playcustomlevel].filename);
            break;
        default:
            VVV_unreachable();
        }

        graphics.fademode = FADE_START_FADEIN;
        break;
    }
    case Start_CUTSCENETEST:
        music.fadeout();
        game.translator_exploring = true;
        game.translator_cutscene_test = true;
        game.startspecial(2);
        game.mapheld = true;

        loadtest(game.cutscenetest_menu_play_id);
        break;

    case Start_QUIT:
        VVV_unreachable();
        break;
    }

    game.gravitycontrol = game.savegc;
    graphics.flipmode = graphics.setflipmode;

    if (!map.custommode && !graphics.setflipmode)
    {
        /* Invalidate Flip Mode trophy */
        obj.flags[73] = true;
    }

    obj.entities.clear();
    obj.createentity(game.savex, game.savey, 0, 0);
    if (player_hitbox.initialized)
    {
        /* Restore player hitbox */
        const int player_idx = obj.getplayer();
        if (INBOUNDS_VEC(player_idx, obj.entities))
        {
            entclass* player = &obj.entities[player_idx];
            player->size = player_hitbox.size;
            player->cx = player_hitbox.cx;
            player->cy = player_hitbox.cy;
            player->w = player_hitbox.w;
            player->h = player_hitbox.h;
        }
    }

    map.resetplayer();
    map.gotoroom(game.saverx, game.savery);
    map.initmapdata();
    if (map.custommode)
    {
        cl.generatecustomminimap();
    }

    /* If we are spawning in a tower, ensure variables are set correctly */
    if (map.towermode)
    {
        map.resetplayer();

        i = obj.getplayer();
        if (INBOUNDS_VEC(i, obj.entities))
        {
            map.ypos = obj.entities[i].yp - 120;
            map.oldypos = map.ypos;
        }
        map.setbgobjlerp(graphics.towerbg);
        map.cameramode = 0;
        map.colsuperstate = 0;
    }
}

void scriptclass::teleport(void)
{
    //er, ok! Teleport to a new area, so!
    //A general rule of thumb: if you teleport with a companion, get rid of them!
    game.companion = 0;

    i = obj.getplayer(); //less likely to have a serious collision error if the player is centered
    if (INBOUNDS_VEC(i, obj.entities))
    {
        obj.entities[i].xp = 150;
        obj.entities[i].yp = 110;
        if(game.teleport_to_x==17 && game.teleport_to_y==17) obj.entities[i].xp = 88; //prevent falling!
        obj.entities[i].lerpoldxp = obj.entities[i].xp;
        obj.entities[i].lerpoldyp = obj.entities[i].yp;
    }

    if (game.teleportscript == "levelonecomplete")
    {
        game.teleport_to_x = 2;
        game.teleport_to_y = 11;
    }
    else if (game.teleportscript == "gamecomplete")
    {
        game.teleport_to_x = 2;
        game.teleport_to_y = 11;
    }

    game.gravitycontrol = 0;
    map.gotoroom(100+game.teleport_to_x, 100+game.teleport_to_y);
    j = obj.getteleporter();
    if (INBOUNDS_VEC(j, obj.entities))
    {
        obj.entities[j].state = 2;
    }
    game.teleport_to_new_area = false;

    if (INBOUNDS_VEC(j, obj.entities))
    {
        game.savepoint = obj.entities[j].para;
        game.savex = obj.entities[j].xp + 44;
        game.savey = obj.entities[j].yp + 44;
    }
    game.savegc = 0;

    game.saverx = game.roomx;
    game.savery = game.roomy;
    int player = obj.getplayer();
    if (INBOUNDS_VEC(player, obj.entities))
    {
        game.savedir = obj.entities[player].dir;
    }

    if(game.teleport_to_x==0 && game.teleport_to_y==0)
    {
        game.setstate(4020);
    }
    else if(game.teleport_to_x==0 && game.teleport_to_y==16)
    {
        game.setstate(4030);
    }
    else if(game.teleport_to_x==7 && game.teleport_to_y==9)
    {
        game.setstate(4040);
    }
    else if(game.teleport_to_x==8 && game.teleport_to_y==11)
    {
        game.setstate(4050);
    }
    else if(game.teleport_to_x==14 && game.teleport_to_y==19)
    {
        game.setstate(4030);
    }
    else if(game.teleport_to_x==17 && game.teleport_to_y==12)
    {
        game.setstate(4020);
    }
    else if(game.teleport_to_x==17 && game.teleport_to_y==17)
    {
        game.setstate(4020);
    }
    else if(game.teleport_to_x==18 && game.teleport_to_y==7)
    {
        game.setstate(4060);
    }
    else
    {
        game.setstate(4010);
    }

    if (game.teleportscript != "")
    {
        game.setstate(0);
        load(game.teleportscript);
        game.teleportscript = "";
    }
    else
    {
        //change music based on location
        if (game.teleport_to_x == 2 && game.teleport_to_y == 11)
        {
            /* Special case: Ship music needs to be set here;
             * ship teleporter on music map is -1 for jukebox. */
            music.niceplay(Music_PASSIONFOREXPLORING);
        }
        game.savetele_textbox();
    }
}

void scriptclass::hardreset(void)
{
    const bool version2_2 = GlitchrunnerMode_less_than_or_equal(Glitchrunner2_2);

    if (game.seed_use_sdl_getticks)
    {
        /* The RNG is 32-bit. We don't _really_ need 64-bit... */
        xoshiro_seed((Uint32) SDL_GetTicks64());
    }
    else
    {
        xoshiro_seed(game.framecounter);
    }

    //Game:
    game.hascontrol = true;
    game.gravitycontrol = 0;
    game.teleport = false;
    game.companion = 0;
    if (!version2_2)
    {
        // Ironically, resetting more variables makes the janky fadeout system in glitchrunnermode even more glitchy
        game.roomx = 0;
        game.roomy = 0;
    }
    game.prevroomx = 0;
    game.prevroomy = 0;
    game.teleport_to_new_area = false;
    game.teleport_to_x = 0;
    game.teleport_to_y = 0;
    game.teleportscript = "";

    game.tapleft = 0;
    game.tapright = 0;
    game.startscript = false;
    game.newscript = "";
    game.alarmon = false;
    game.alarmdelay = 0;
    game.blackout = false;
    game.useteleporter = false;
    game.teleport_to_teleporter = 0;

    game.nodeathmode = false;
    game.nocutscenes = false;

    for (i = 0; i < (int) SDL_arraysize(game.crewstats); i++)
    {
        game.crewstats[i] = false;
    }
    game.crewstats[0] = true;
    game.lastsaved = 0;

    game.deathcounts = 0;
    game.gameoverdelay = 0;
    game.resetgameclock();
    game.gamesaved = false;
    game.gamesavefailed = false;
    game.savetime = "00:00";
    game.savearea = "nowhere";
    game.savetrinkets = 0;
    if (!version2_2)
    {
        // Ironically, resetting more variables makes the janky fadeout system in glitchrunnermode even more glitchy
        game.saverx = 0;
        game.savery = 0;
        game.savex = 0;
        game.savey = 0;
        game.savegc = 0;
    }
    game.savecolour = 0;

    game.intimetrial = false;
    game.timetrialcountdown = 0;
    game.timetrialshinytarget = 0;
    game.timetrialparlost = false;
    game.timetrialpar = 0;
    game.timetrialcheater = false;

    game.translator_exploring = game.start_translator_exploring;
    game.start_translator_exploring = false;
    game.translator_exploring_allowtele = false;
    game.translator_cutscene_test = false;

    game.totalflips = 0;
    game.hardestroom = loc::gettext_roomname(false, 13, 5, "Welcome Aboard", false);
    game.hardestroomdeaths = 0;
    game.currentroomdeaths=0;

    game.swnmode = false;
    game.swntimer = 0;
    game.swngame = SWN_NONE; // Not playing sine wave ninja!
    game.swnstate = 0;
    game.swnstate2 = 0;
    game.swnstate3 = 0;
    game.swnstate4 = 0;
    game.swndelay = 0;
    game.swndeaths = 0;
    game.supercrewmate = false;
    game.scmhurt = false;
    game.scmprogress = 0;
    game.swncolstate = 0;
    game.swncoldelay = 0;
    game.swnrank = 0;
    game.swnmessage = 0;
    game.creditposx = 0;
    game.creditposy = 0;
    game.creditposdelay = 0;

    game.inintermission = false;
    game.insecretlab = false;

    game.unlockstate();
    game.state = 0;
    game.statedelay = 0;

    game.hascontrol = true;
    if (!GlitchrunnerMode_less_than_or_equal(Glitchrunner2_0))
    {
        // Keep the "- Press ACTION to advance text -" prompt around,
        // apparently the speedrunners call it the "text storage" glitch
        game.advancetext = false;
    }

    game.pausescript = false;
    game.completestop = false;

    game.flashlight = 0;
    game.screenshake = 0;

    game.activeactivity = -1;
    game.act_fade = 5;

    game.disabletemporaryaudiopause = true;

    game.ingame_titlemode = false;

    //dwgraphicsclass
    graphics.backgrounddrawn = false;
    graphics.textboxes.clear();
    graphics.flipmode = false; //This will be reset if needs be elsewhere
    graphics.showcutscenebars = false;
    graphics.setbars(0);

    //mapclass
    map.warpx = false;
    map.warpy = false;
    map.showteleporters = false;
    map.showtargets = false;
    map.showtrinkets = false;
    map.finalmode = false;
    map.finalstretch = false;
    map.final_colormode = false;
    map.final_colorframe = 0;
    map.final_colorframedelay = 0;
    map.final_mapcol = 0;
    map.final_aniframe = 0;
    map.final_aniframedelay = 0;
    map.rcol = 0;
    map.custommode=false;
    map.custommodeforreal=false;
    if (!version2_2)
    {
        // Ironically, resetting more variables makes the janky fadeout system even more glitchy
        map.towermode=false;
    }
    map.cameraseekframe = 0;
    map.resumedelay = 0;
    graphics.towerbg.scrolldir = 0;
    map.customshowmm=true;

    SDL_memset(map.roomdeaths, 0, sizeof(map.roomdeaths));
    SDL_memset(map.roomdeathsfinal, 0, sizeof(map.roomdeathsfinal));
    map.resetmap();
    //entityclass
    obj.nearelephant = false;
    obj.upsetmode = false;
    obj.upset = 0;

    obj.trophytext = 0 ;
    obj.trophytype = 0;
    obj.altstates = 0;

    obj.resetallflags();

    for (i = 0; i < (int) SDL_arraysize(obj.customcrewmoods); i++){
        obj.customcrewmoods[i]=true;
    }

    SDL_memset(obj.collect, false, sizeof(obj.collect));
    SDL_memset(obj.customcollect, false, sizeof(obj.customcollect));
    i = 100; //previously a for-loop iterating over collect/customcollect set this to 100

    int theplayer = obj.getplayer();
    if (INBOUNDS_VEC(theplayer, obj.entities)){
        obj.entities[theplayer].tile = 0;
    }

    /* Disable duplicate player entities */
    for (int i = 0; i < (int) obj.entities.size(); i++)
    {
        if (obj.entities[i].rule == 0 && i != theplayer)
        {
            obj.disableentity(i);
        }
    }

    obj.customscript = "";

    //Script Stuff
    position = 0;
    commands.clear();
    scriptdelay = 0;
    scriptname = "null";
    running = false;
    for (size_t ii = 0; ii < NUM_SCRIPT_ARGS; ++ii)
    {
        words[ii] = "";
        raw_words[ii] = "";
    }

    obj.customactivitycolour = "";
    obj.customactivitytext = "";
    obj.customactivitypositiony = -1;
}

bool scriptclass::loadcustom(const std::string& t)
{
    //this magic function breaks down the custom script and turns into real scripting!
    std::string cscriptname="";
    for(size_t i=0; i<t.length(); i++){
        if(i>=7) cscriptname+=t[i];
    }

    std::string tstring;

    std::vector<std::string>* contents = NULL;
    for(size_t i = 0; i < customscripts.size(); i++){
        Script& script_ = customscripts[i];

        if(script_.name == cscriptname){
            contents = &script_.contents;
            break;
        }
    }
    if(contents == NULL){
        return false;
    }

    std::vector<std::string>& lines = *contents;

    //Ok, we've got the relavent script segment, we do a pass to assess it, then run it!
    int customcutscenemode=0;
    for(size_t i=0; i<lines.size(); i++){
        tokenize(lines[i]);
        if(words[0] == "say"){
            customcutscenemode=1;
        }else if(words[0] == "reply"){
            customcutscenemode=1;
        }
    }

    if(customcutscenemode==1){
        add("cutscene()");
        add("untilbars()");
    }
    int customtextmode=0;
    int speakermode=0; //0, terminal, numbers for crew
    int squeakmode=0;//default on
    //Now run the script
    for(size_t i=0; i<lines.size(); i++){
        words[0]="nothing"; //Default!
        words[1]="1"; //Default!
        tokenize(lines[i]);
        for (size_t ii = 0; ii < words[0].length(); ii++)
        {
            words[0][ii] = SDL_tolower(words[0][ii]);
        }
        if(words[0] == "music"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            if(words[1]=="0"){
                tstring="stopmusic()";
            }else{
                if(words[1]=="11"){ tstring="play(14)";
                }else if(words[1]=="10"){ tstring="play(13)";
                }else if(words[1]=="9"){ tstring="play(12)";
                }else if(words[1]=="8"){ tstring="play(11)";
                }else if(words[1]=="7"){ tstring="play(10)";
                }else if(words[1]=="6"){ tstring="play(8)";
                }else if(words[1]=="5"){ tstring="play(6)";
                }else { tstring="play("+words[1]+")"; }
            }
            add(tstring);
        }else if(words[0] == "playremix"){
            add("play(15)");
        }else if(words[0] == "flash"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            add("flash(5)");
            add("shake(20)");
            add("playef(9)");
        }else if(words[0] == "sad" || words[0] == "cry"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            if(words[1]=="player"){
                add("changemood(player,1)");
            }else if(words[1]=="cyan" || words[1]=="viridian" || words[1]=="1"){
                add("changecustommood(customcyan,1)");
            }else if(words[1]=="purple" || words[1]=="violet" || words[1]=="pink" || words[1]=="2"){
                add("changecustommood(purple,1)");
            }else if(words[1]=="yellow" || words[1]=="vitellary" || words[1]=="3"){
                add("changecustommood(yellow,1)");
            }else if(words[1]=="red" || words[1]=="vermilion" || words[1]=="4"){
                add("changecustommood(red,1)");
            }else if(words[1]=="green" || words[1]=="verdigris" || words[1]=="5"){
                add("changecustommood(green,1)");
            }else if(words[1]=="blue" || words[1]=="victoria" || words[1]=="6"){
                add("changecustommood(blue,1)");
            }else if(words[1]=="all" || words[1]=="everybody" || words[1]=="everyone"){
                add("changemood(player,1)");
                add("changecustommood(customcyan,1)");
                add("changecustommood(purple,1)");
                add("changecustommood(yellow,1)");
                add("changecustommood(red,1)");
                add("changecustommood(green,1)");
                add("changecustommood(blue,1)");
            }else{
                add("changemood(player,1)");
            }
            if(squeakmode==0) add("squeak(cry)");
        }else if(words[0] == "happy"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            if(words[1]=="player"){
                add("changemood(player,0)");
                if(squeakmode==0) add("squeak(player)");
            }else if(words[1]=="cyan" || words[1]=="viridian" || words[1]=="1"){
                add("changecustommood(customcyan,0)");
                if(squeakmode==0) add("squeak(player)");
            }else if(words[1]=="purple" || words[1]=="violet" || words[1]=="pink" || words[1]=="2"){
                add("changecustommood(purple,0)");
                if(squeakmode==0) add("squeak(purple)");
            }else if(words[1]=="yellow" || words[1]=="vitellary" || words[1]=="3"){
                add("changecustommood(yellow,0)");
                if(squeakmode==0) add("squeak(yellow)");
            }else if(words[1]=="red" || words[1]=="vermilion" || words[1]=="4"){
                add("changecustommood(red,0)");
                if(squeakmode==0) add("squeak(red)");
            }else if(words[1]=="green" || words[1]=="verdigris" || words[1]=="5"){
                add("changecustommood(green,0)");
                if(squeakmode==0) add("squeak(green)");
            }else if(words[1]=="blue" || words[1]=="victoria" || words[1]=="6"){
                add("changecustommood(blue,0)");
                if(squeakmode==0) add("squeak(blue)");
            }else if(words[1]=="all" || words[1]=="everybody" || words[1]=="everyone"){
                add("changemood(player,0)");
                add("changecustommood(customcyan,0)");
                add("changecustommood(purple,0)");
                add("changecustommood(yellow,0)");
                add("changecustommood(red,0)");
                add("changecustommood(green,0)");
                add("changecustommood(blue,0)");
            }else{
                add("changemood(player,0)");
                if(squeakmode==0) add("squeak(player)");
            }
        }else if(words[0] == "squeak"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            if(words[1]=="player"){
                add("squeak(player)");
            }else if(words[1]=="cyan" || words[1]=="viridian" || words[1]=="1"){
                add("squeak(player)");
            }else if(words[1]=="purple" || words[1]=="violet" || words[1]=="pink" || words[1]=="2"){
                add("squeak(purple)");
            }else if(words[1]=="yellow" || words[1]=="vitellary" || words[1]=="3"){
                add("squeak(yellow)");
            }else if(words[1]=="red" || words[1]=="vermilion" || words[1]=="4"){
                add("squeak(red)");
            }else if(words[1]=="green" || words[1]=="verdigris" || words[1]=="5"){
                add("squeak(green)");
            }else if(words[1]=="blue" || words[1]=="victoria" || words[1]=="6"){
                add("squeak(blue)");
            }else if(words[1]=="cry" || words[1]=="sad"){
                add("squeak(cry)");
            }else if(words[1]=="on"){
                squeakmode=0;
            }else if(words[1]=="off"){
                squeakmode=1;
            }
        }else if(words[0] == "delay"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            add(lines[i]);
        }else if(words[0] == "flag"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            add(lines[i]);
        }else if(words[0] == "map"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            add("custom"+lines[i]);
        }else if(words[0] == "warpdir"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            add(lines[i]);
        }else if(words[0] == "ifwarp"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            add(lines[i]);
        }else if(words[0] == "iftrinkets"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            add("custom"+lines[i]);
        }else if(words[0] == "ifflag"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            add("custom"+lines[i]);
        }else if(words[0] == "iftrinketsless"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            add("custom"+lines[i]);
        }else if(words[0] == "textcase"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            add(lines[i]);
        }else if(words[0] == "iflang"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            add(lines[i]);
        }else if(words[0] == "loadtext"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            add(lines[i]);
        }else if(words[0] == "setfont"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            add(lines[i]);
        }else if(words[0] == "destroy"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            add(lines[i]);
        }else if(words[0] == "speaker"){
            speakermode=0;
            if(words[1]=="gray" || words[1]=="grey" || words[1]=="terminal" || words[1]=="0") speakermode=0;
            if(words[1]=="cyan" || words[1]=="viridian" || words[1]=="player" || words[1]=="1") speakermode=1;
            if(words[1]=="purple" || words[1]=="violet" || words[1]=="pink" || words[1]=="2") speakermode=2;
            if(words[1]=="yellow" || words[1]=="vitellary" || words[1]=="3") speakermode=3;
            if(words[1]=="red" || words[1]=="vermilion" || words[1]=="4") speakermode=4;
            if(words[1]=="green" || words[1]=="verdigris" || words[1]=="5") speakermode=5;
            if(words[1]=="blue" || words[1]=="victoria" || words[1]=="6") speakermode=6;
        }else if(words[0] == "say"){
            //Speakers!
            if(words[2]=="terminal" || words[2]=="gray" || words[2]=="grey" || words[2]=="0") speakermode=0;
            if(words[2]=="cyan" || words[2]=="viridian" || words[2]=="player" || words[2]=="1") speakermode=1;
            if(words[2]=="purple" || words[2]=="violet" || words[2]=="pink" || words[2]=="2") speakermode=2;
            if(words[2]=="yellow" || words[2]=="vitellary" || words[2]=="3") speakermode=3;
            if(words[2]=="red" || words[2]=="vermilion" || words[2]=="4") speakermode=4;
            if(words[2]=="green" || words[2]=="verdigris" || words[2]=="5") speakermode=5;
            if(words[2]=="blue" || words[2]=="victoria" || words[2]=="6") speakermode=6;
            switch(speakermode){
                case 0:
                    if(squeakmode==0) add("squeak(terminal)");
                    add("text(gray,0,114,"+words[1]+")");
                break;
                case 1: //NOT THE PLAYER
                    if(squeakmode==0) add("squeak(cyan)");
                    add("text(cyan,0,0,"+words[1]+")");
                break;
                case 2:
                    if(squeakmode==0) add("squeak(purple)");
                    add("text(purple,0,0,"+words[1]+")");
                break;
                case 3:
                    if(squeakmode==0) add("squeak(yellow)");
                    add("text(yellow,0,0,"+words[1]+")");
                break;
                case 4:
                    if(squeakmode==0) add("squeak(red)");
                    add("text(red,0,0,"+words[1]+")");
                break;
                case 5:
                    if(squeakmode==0) add("squeak(green)");
                    add("text(green,0,0,"+words[1]+")");
                break;
                case 6:
                    if(squeakmode==0) add("squeak(blue)");
                    add("text(blue,0,0,"+words[1]+")");
                break;
            }
            int ti = ss_toi(words[1]);
            int nti = ti >= 0 ? ti : 1;
            for (int ti2 = 0; ti2 < nti; ti2++)
            {
                i++;
                if (INBOUNDS_VEC(i, lines))
                {
                    add(lines[i]);
                }
            }

            switch(speakermode){
                case 0: add("customposition(center)"); break;
                case 1: add("customposition(cyan,above)"); break;
                case 2: add("customposition(purple,above)"); break;
                case 3: add("customposition(yellow,above)"); break;
                case 4: add("customposition(red,above)"); break;
                case 5: add("customposition(green,above)"); break;
                case 6: add("customposition(blue,above)"); break;
            }
            add("speak_active");
            customtextmode=1;
        }else if(words[0] == "reply"){
            //For this version, terminal only
            if(squeakmode==0) add("squeak(player)");
            add("text(cyan,0,0,"+words[1]+")");

            int ti=help.Int(words[1].c_str());
            int nti = ti>=0 ? ti : 1;
            for(int ti2=0; ti2<nti; ti2++){
                i++;
                if(INBOUNDS_VEC(i, lines)){
                    add(lines[i]);
                }
            }
            add("position(player,above)");
            add("speak_active");
            customtextmode=1;
        }
    }

    if(customtextmode==1){ add("endtext"); customtextmode=0;}
    if(customcutscenemode==1){
        add("endcutscene()");
        add("untilbars()");
    }

    return true;
}

void scriptclass::loadalts(const std::string& processed, const std::string& raw)
{
    const bool exists = load(processed);
    if (!exists)
    {
        load(raw);
    }
}

void scriptclass::add_test_line(
    const std::string& speaker,
    const std::string& english,
    const char textcase,
    const bool textbuttons
) {
    if (speaker == "gray")
    {
        add("squeak(terminal)");
    }
    else
    {
        add("squeak("+speaker+")");
    }
    add("textcase("+help.String(textcase)+")");
    add("text("+speaker+",0,0,1)");
    add(english);
    add("position(center)");
    if (textbuttons)
    {
        add("textbuttons()");
    }
    add("speak_active");
}

void scriptclass::loadtest(const std::string& name)
{
    // Another magic function, that turns language files into a demo script
    position = 0;
    commands.clear();
    scriptname = name;
    running = true;

    loc::populate_cutscene_test(name.c_str());

    add("endtext");
    add("gamestate(3100)");
}
