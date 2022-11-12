#include "Script.h"

#include <SDL.h>

void scriptclass::load(const std::string& name)
{
    //loads script name t into the array
    position = 0;
    commands.clear();
    running = true;

    const char* t = name.c_str();

    if (SDL_strncmp(t, "custom_", 7) == 0)
    {
        loadcustom(name);
    }
    else if (SDL_strcmp(t, "intro") == 0)
    {
        static const char* lines[] = {
        "ifskip(quickstart)",
        //"createcrewman(232,113,cyan,0,faceright)",
        "createcrewman(96,177,green,0,faceright)",
        "createcrewman(122,177,purple,0,faceleft)",
        "fadein()",
        "untilfade()",
        "delay(90)",

        "flash(5)",
        "shake(20)",
        "playef(9)",
        "musicfadeout()",
        "changemood(player,1)",
        "delay(15)",
        "squeak(player)",
        "text(cyan,0,0,1)",
        "Uh oh...",
        "position(player,above)",
        //"backgroundtext",
        "speak_active",

        "squeak(purple)",
        "changeai(purple,followposition,175)",
        "text(purple,145,150,1)",
        "Is everything ok?",
        //"position(purple,above)",
        //"backgroundtext",
        "speak_active",

        "squeak(player)",
        "walk(left,2)",
        "text(cyan,0,0,2)",
        "No! We've hit some",
        "kind of interference...",
        "position(player,above)",
        //"backgroundtext",
        "speak_active",

        //"delay(30)",
        "endtext",

        "flash(5)",
        "shake(50)",
        "playef(9)",
        "changemood(green,1)",
        "changemood(purple,1)",
        "alarmon",

        "changedir(player,1)",
        "delay(30)",
        "endtext",

        "squeak(player)",
        "text(cyan,0,0,2)",
        "Something's wrong! We're",
        "going to crash!",
        "position(player,above)",
        //"backgroundtext",
        "speak_active",
        //"delay(100)",

        "endtext",

        "flash(5)",
        "shake(50)",
        "playef(9)",
        "changeai(green,followposition,-60)",
        "changeai(purple,followposition,-60)",
        "squeak(player)",
        "text(cyan,70,140,1)",
        "Evacuate!",
        "backgroundtext",
        "speak_active",
        "walk(left,35)",

        "endtextfast",

        //Ok, next room!

        "flash(5)",
        "shake(50)",
        "playef(9)",
        "gotoroom(3,10)",
        "gotoposition(310,177,0)",
        "createcrewman(208,177,green,1,followposition,120)",
        "createcrewman(240,177,purple,1,followposition,120)",
        "createcrewman(10,177,blue,1,followposition,180)",

        "squeak(blue)",
        "text(blue,80,150,1)",
        "Oh no!",
        "backgroundtext",
        "speak_active",
        "walk(left,20)",

        "endtextfast",

        //and the next!
        "flash(5)",
        "shake(50)",
        "playef(9)",
        "gotoroom(3,11)",
        "gotoposition(140,0,0)",

        "createcrewman(90,105,green,1,followblue)",
        "createcrewman(125,105,purple,1,followgreen)",
        "createcrewman(55,105,blue,1,followposition,-200)",

        "createcrewman(120,177,yellow,1,followposition,-200)",
        "createcrewman(240,177,red,1,faceleft)",

        "delay(5)",
        "changeai(red,followposition,-200)",

        "squeak(red)",
        "text(red,100,150,1)",
        "Everyone off the ship!",
        "backgroundtext",
        "speak_active",

        "walk(left,25)",

        "endtextfast",

        //final room:
        "flash(5)",
        "shake(80)",
        "playef(9)",
        "gotoroom(2,11)",
        "gotoposition(265,153,0)",

        "createcrewman(130,153,blue,1,faceleft)",
        "createcrewman(155,153,green,1,faceleft)",
        "createcrewman(180,153,purple,1,faceleft)",
        "createcrewman(205,153,yellow,1,faceleft)",
        "createcrewman(230,153,red,1,faceleft)",


        "squeak(yellow)",
        "text(yellow,0,0,1)",
        "This shouldn't be happening!",
        "position(yellow,below)",
        "backgroundtext",
        "speak_active",

        "activateteleporter()",

        "delay(10)",
        "changecolour(blue,teleporter)",
        "delay(10)",
        "changecolour(green,teleporter)",
        "delay(10)",
        "changecolour(purple,teleporter)",
        "delay(10)",
        "changecolour(yellow,teleporter)",
        "delay(10)",
        "changecolour(red,teleporter)",
        "delay(10)",

        //and teleport!
        "endtext",
        "alarmoff",
        "flash(5)",
        "shake(20)",
        "playef(10)",
        "blackout()",
        "changemood(player,0)",
        "changedir(player,1)",

        "delay(100)",
        "blackon()",
        "shake(20)",
        "playef(10)",

        //Finally, appear at the start of the game:
        "gotoroom(13,5)",
        "gotoposition(80,96,0)",
        "walk(right,20)",
        //"delay(45)",

        "squeak(player)",
        "text(cyan,0,0,1)",
        "Phew! That was scary!",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(cyan,0,0,2)",
        "At least we all",
        "escaped, right guys?",
        "position(player,above)",
        "speak_active",

        "endtext",

        "delay(45)",
        "walk(left,3)",
        "delay(45)",
        "setcheckpoint()",

        "squeak(player)",
        "text(cyan,0,0,1)",
        "...guys?",
        "position(player,above)",
        "speak_active",

        "endtext",

        "delay(25)",
        "changemood(player,1)",
        "squeak(cry)",
        "delay(25)",

        "play(1)",
        "endcutscene()",
        "untilbars()",

        "hideship()",

        "gamestate(4)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "quickstart") == 0)
    {
        //Finally, appear at the start of the game:
        static const char* lines[] = {
        "gotoroom(13,5)",
        "gotoposition(80,96,0)",
        "walk(right,17)",
        "fadein()",

        "setcheckpoint()",

        "play(1)",
        "endcutscene()",
        "untilbars()",

        "hideship()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "firststeps") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(player)",
        "text(cyan,0,0,2)",
        "I wonder why the ship",
        "teleported me here alone?",
        "position(player,above)",
        "speak_active",

        "squeak(cry)",
        "text(cyan,0,0,2)",
        "I hope everyone else",
        "got out ok...",
        "position(player,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "trenchwarfare") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "iftrinkets(1,newtrenchwarfare)",

        "squeak(player)",
        "text(cyan,0,0,1)",
        "Ohh! I wonder what that is?",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(cyan,0,0,3)",
        "I probably don't really need it,",
        "but it might be nice to take it",
        "back to the ship to study...",
        "position(player,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "newtrenchwarfare") == 0)
    {
        static const char* lines[] = {
        "squeak(player)",
        "text(cyan,0,0,2)",
        "Oh! It's another one of",
        "those shiny things!",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(cyan,0,0,3)",
        "I probably don't really need it,",
        "but it might be nice to take it",
        "back to the ship to study...",
        "position(player,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "trinketcollector") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "iftrinkets(1,newtrinketcollector)",

        "squeak(player)",
        "text(cyan,0,0,3)",
        "This seems like a good",
        "place to store anything",
        "I find out there...",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(cyan,0,0,3)",
        "Victoria loves to study the",
        "interesting things we find",
        "on our adventures!",
        "position(player,above)",
        "speak_active",

        "ifcrewlost(5,new2trinketcollector)",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "newtrinketcollector") == 0)
    {
        static const char* lines[] = {
        "squeak(player)",
        "text(cyan,0,0,3)",
        "This seems like a good",
        "place to store those",
        "shiny things.",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(cyan,0,0,3)",
        "Victoria loves to study the",
        "interesting things we find",
        "on our adventures!",
        "position(player,above)",
        "speak_active",

        "ifcrewlost(5,new2trinketcollector)",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "new2trinketcollector") == 0)
    {
        static const char* lines[] = {
        "squeak(cry)",
        "changemood(player,1)",
        "text(cyan,0,0,1)",
        "I hope she's ok...",
        "position(player,above)",
        "speak_active",
        "endtext",

        "changemood(player,0)",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
        return;
    }
    if (SDL_strcmp(t, "communicationstation") == 0)
    {
        static const char* lines[] = {
        "ifskip(communicationstationskip)",
        "cutscene()",
        "untilbars()",
        "changemood(player,0)",

        "tofloor",
        "play(5)",
        "delay(10)",

        "squeak(player)",
        "text(cyan,0,0,1)",
        "Violet! Is that you?",
        "position(player,above)",
        "speak_active",
        "endtext",

        "squeak(purple)",
        "text(purple,45,18,1)",
        "Captain! You're ok!",
        "speak_active",

        "squeak(cry)",
        "text(purple,20,16,3)",
        "Something has gone",
        "horribly wrong with the",
        "ship's teleporter!",
        "speak_active",

        "squeak(purple)",
        "text(purple,8,14,3)",
        "I think everyone has been",
        "teleported away randomly!",
        "They could be anywhere!",
        "speak_active",

        "squeak(cry)",
        "changemood(player,1)",
        "text(cyan,0,0,1)",
        "Oh no!",
        "position(player,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,10,19,2)",
        "I'm on the ship - it's damaged",
        "badly, but it's still intact!",
        "speak_active",

        "squeak(purple)",
        "text(purple,10,15,1)",
        "Where are you, Captain?",
        "speak_active",

        "squeak(player)",
        "changemood(player,0)",
        "text(cyan,0,0,3)",
        "I'm on some sort of",
        "space station... It",
        "seems pretty modern...",
        "position(player,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,15,16,2)",
        "There seems to be some sort of",
        "interference in this dimension...",
        "speak_active",

        "hideteleporters()",
        "endtextfast",
        "delay(10)",

        //add map mode here and wrap up...
        "gamemode(teleporter)",
        "delay(20)",

        "squeak(purple)",
        "text(purple,25,205,2)",
        "I'm broadcasting the coordinates",
        "of the ship to you now.",
        "speak_active",
        "endtext",

        "squeak(terminal)",
        "showship()",
        "delay(10)",
        "hideship()",
        "delay(10)",
        "showship()",
        "delay(10)",
        "hideship()",
        "delay(10)",
        "showship()",
        "delay(20)",

        "squeak(purple)",
        "text(purple,10,200,1)",
        "I can't teleport you back, but...",
        "speak_active",

        "squeak(purple)",
        "text(purple,25,195,3)",
        "If YOU can find a teleporter",
        "anywhere nearby, you should be",
        "able to teleport back to me!",
        "speak_active",
        "endtext",

        "squeak(terminal)",
        "delay(20)",
        "showteleporters()",
        "delay(10)",
        "hideteleporters()",
        "delay(10)",
        "showteleporters()",
        "delay(10)",
        "hideteleporters()",
        "delay(10)",
        "showteleporters()",
        "delay(20)",

        "squeak(player)",
        "text(cyan,20,190,1)",
        "Ok! I'll try to find one!",
        "speak_active",
        "endtext",
        "delay(20)",

        "gamemode(game)",
        "delay(20)",

        "squeak(purple)",
        "text(purple,40,22,1)",
        "Good luck, Captain!",
        "speak_active",
        "endtext",

        "squeak(purple)",
        "text(purple,10,19,2)",
        "I'll keep trying to find",
        "the rest of the crew...",
        "speak_active",
        "endtext",

        "endcutscene()",
        "untilbars()",
        "play(1)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "communicationstationskip") == 0)
    {
        static const char* lines[] = {
        "changemood(player,0)",

        "delay(10)",
        "endtext",
        //add map mode here and wrap up...
        "gamemode(teleporter)",
        "delay(5)",

        "squeak(terminal)",
        "showship()",
        "showteleporters()",
        "delay(10)",
        "hideship()",
        "hideteleporters()",
        "delay(10)",
        "showship()",
        "showteleporters()",
        "delay(10)",
        "hideship()",
        "hideteleporters()",
        "delay(10)",
        "showship()",
        "showteleporters()",
        "delay(20)",

        "gamemode(game)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "teleporterback") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(player)",
        "text(cyan,0,0,1)",
        "A teleporter!",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(cyan,0,0,2)",
        "I can get back to the",
        "ship with this!",
        "position(player,above)",
        "speak_active",

        "endtext",

        "teleportscript(levelonecomplete)",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "levelonecomplete") == 0)
    {
        static const char* lines[] = {
        "nocontrol()",
        "createcrewman(230,153,purple,0,faceleft)",

        "cutscene()",
        "untilbars()",

        "delay(30)",
        "rescued(purple)",

        "delay(10)",
        "gamestate(4090)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "levelonecomplete_ending") == 0)
    {
        static const char* lines[] = {
        "squeak(purple)",
        "text(purple,0,0,1)",
        "Captain!",
        "position(purple,above)",
        "speak_active",
        "endtext",
        "nocontrol()",

        "endcutscene()",
        "untilbars()",

        "gamestate(3050)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "levelonecompleteskip") == 0)
    {
        static const char* lines[] = {
        "nocontrol()",
        "gamestate(3050)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "bigopenworld") == 0)
    {
        static const char* lines[] = {
        "play(5)",
        "cutscene()",
        "untilbars()",

        "missing(purple)", //Set violet's rescue script to 0 to make the next bit easier

        "gotoroom(4,10)",
        "gotoposition(100,177,0)",
        "createcrewman(150,177,purple,0,faceleft)",

        //set all the crew as rescued to avoid companion issues!
        "flag(8,on)",
        "flag(9,on)",
        "flag(10,on)",
        "flag(11,on)",

        "fadein()",
        "untilfade()",

        "delay(15)",

        "squeak(player)",
        "text(player,0,0,2)",
        "So, Doctor - have you any",
        "idea what caused the crash?",
        "position(player,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,3)",
        "There's some sort of bizarre",
        "signal here that's interfering",
        "with our equipment...",
        "position(purple,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,3)",
        "It caused the ship to lose",
        "its quantum position, collapsing",
        "us into this dimension!",
        "position(purple,above)",
        "speak_active",

        "squeak(cry)",
        "changemood(player,1)",
        "text(player,0,0,1)",
        "Oh no!",
        "position(player,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,2)",
        "But I think we should be able to fix",
        "the ship and get out of here...",
        "position(purple,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,2)",
        "... as long as we can",
        "find the rest of the crew.",
        "position(purple,above)",
        "speak_active",

        "endtext",

        //Cut to Red
        "fadeout()",
        "untilfade()",

        "changeplayercolour(red)",
        "gotoroom(10,4)",
        "gotoposition(200,185,0)",
        "hideplayer()",
        "createcrewman(200,185,red,1,panic)",

        "fadein()",
        "untilfade()",

        //"walk(right,10)",

        "squeak(purple)",
        "text(purple,60,40,2)",
        "We really don't know anything",
        "about this place...",
        "speak_active",

        "endtext",

        "delay(15)",

        //Cut to Green
        "fadeout()",
        "untilfade()",

        "showplayer()",

        "changeplayercolour(green)",
        "gotoroom(13,0)",
        "gotoposition(143,20,0)",

        "fadein()",
        "untilfade()",


        "squeak(purple)",
        "text(purple,40,30,2)",
        "Our friends could be anywhere - they",
        "could be lost, or in danger!",
        "speak_active",

        "endtext",

        "delay(15)",

        //Cut to Blue
        "fadeout()",
        "untilfade()",

        "changeplayercolour(blue)",
        "gotoroom(3,4)",
        "gotoposition(190,177,0)",

        "fadein()",
        "untilfade()",


        "squeak(player)",
        "text(player,10,60,1)",
        "Can they teleport back here?",
        "speak_active",

        "squeak(purple)",
        "text(purple,50,80,2)",
        "Not unless they find some way",
        "to communicate with us!",
        "speak_active",

        "squeak(purple)",
        "text(purple,30,100,3)",
        "We can't pick up their signal and",
        "they can't teleport here unless",
        "they know where the ship is...",
        "speak_active",

        "endtext",

        "delay(15)",


        //Cut to Yellow
        "fadeout()",
        "untilfade()",

        "changeplayercolour(yellow)",
        "gotoroom(15,9)",
        //(6*8)-21
        "gotoposition(300,27,0)",

        "hideplayer()",
        "createcrewman(280,25,yellow,1,panic)",

        //"hascontrol()",
        //"walk(left,4)",

        "fadein()",
        "untilfade()",

        "squeak(player)",
        "text(player,25,60,1)",
        "So what do we do?",
        "speak_active",

        "squeak(purple)",
        "text(purple,80,125,4)",
        "We need to find them! Head",
        "out into the dimension and",
        "look for anywhere they might",
        "have ended up...",
        "speak_active",

        "endtext",

        "delay(15)",

        //Back to ship
        "fadeout()",
        "untilfade()",
        "showplayer()",

        "flag(8,off)",
        "flag(9,off)",
        "flag(10,off)",
        "flag(11,off)",

        "changeplayercolour(cyan)",
        "changemood(player,0)",
        "gotoroom(4,10)",
        "gotoposition(90,177,0)",
        "walk(right,2)",
        "createcrewman(150,177,purple,0,faceleft)",



        "fadein()",
        "untilfade()",

        "squeak(player)",
        "text(player,0,0,1)",
        "Ok! Where do we start?",
        "position(player,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,2)",
        "Well, I've been trying to find",
        "them with the ship's scanners!",
        "position(purple,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,2)",
        "It's not working, but I did",
        "find something...",
        "position(purple,above)",
        "speak_active",

        "endtext",

        "delay(15)",

        "hidecoordinates(10,4)",
        "hidecoordinates(13,0)",
        "hidecoordinates(3,4)",
        "hidecoordinates(15,9)",
        "showteleporters()",

        //Cut to map
        //add map mode here and wrap up...
        "gamemode(teleporter)",
        "delay(20)",

        "squeak(terminal)",
        "showtargets()",
        "delay(10)",
        "hidetargets()",
        "delay(10)",
        "showtargets()",
        "delay(10)",
        "hidetargets()",
        "delay(10)",
        "showtargets()",
        "delay(20)",

        "squeak(purple)",
        "text(purple,25,205,2)",
        "These points show up on our scans",
        "as having high energy patterns!",
        "speak_active",
        "endtext",

        "squeak(purple)",
        "text(purple,35,185,4)",
        "There's a good chance they're",
        "teleporters - which means",
        "they're probably built near",
        "something important...",
        "speak_active",

        "squeak(purple)",
        "text(purple,25,205,2)",
        "They could be a very good",
        "place to start looking.",
        "speak_active",
        "endtext",
        "delay(20)",

        "gamemode(game)",
        "delay(20)",
        //And finally, back to the ship!

        "squeak(player)",
        "text(player,0,0,2)",
        "Ok! I'll head out and see",
        "what I can find!",
        "position(player,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,2)",
        "I'll be right here if",
        "you need any help!",
        "position(purple,above)",
        "speak_active",
        "endtext",

        "rescued(purple)",
        "play(4)",

        "endcutscene()",
        "untilbars()",
        "hascontrol()",

        "createactivityzone(purple)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "bigopenworldskip") == 0)
    {
        static const char* lines[] = {
        "missing(purple)",
        "gotoroom(4,10)",
        "gotoposition(100,177,0)",
        "createcrewman(150,177,purple,0,faceleft)",

        "fadein()",
        "untilfade()",

        "hidecoordinates(10,4)",
        "hidecoordinates(13,0)",
        "hidecoordinates(3,4)",
        "hidecoordinates(15,9)",
        "showteleporters()",

        //Cut to map
        //add map mode here and wrap up...
        "gamemode(teleporter)",
        "delay(20)",

        "squeak(terminal)",
        "showtargets()",
        "delay(10)",
        "hidetargets()",
        "delay(10)",
        "showtargets()",
        "delay(10)",
        "hidetargets()",
        "delay(10)",
        "showtargets()",
        "delay(20)",

        "gamemode(game)",
        "delay(20)",
        //And finally, back to the ship!

        "squeak(purple)",
        "text(purple,0,0,2)",
        "I'll be right here if",
        "you need any help!",
        "position(purple,above)",
        "speak_active",
        "endtext",

        "rescued(purple)",
        "play(4)",

        "endcutscene()",
        "untilbars()",
        "hascontrol()",

        "createactivityzone(purple)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "rescueblue") == 0)
    {
        static const char* lines[] = {
        "ifskip(skipblue)",
        "cutscene()",

        "tofloor()",
        "changeai(blue,followplayer)",
        "untilbars()",

        "rescued(blue)",

        "squeak(blue)",
        "text(blue,0,0,2)",
        "Oh no! Captain! Are you",
        "stuck here too?",
        "position(blue,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "It's ok - I'm here to rescue you!",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Let me explain everything...",
        "position(player,above)",
        "speak_active",

        "endtext",

        "fadeout()",
        "untilfade()",
        "delay(30)",
        "fadein()",
        "untilfade()",

        "squeak(cry)",
        "text(blue,0,0,2)",
        "What? I didn't understand",
        "any of that!",
        "position(blue,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Oh... well, don't worry.",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,2)",
        "Follow me! Everything",
        "will be alright!",
        "position(player,above)",
        "speak_active",

        "squeak(blue)",
        "changemood(blue,0)",
        "text(blue,0,0,1)",
        "Sniff... Really?",
        "position(blue,above)",
        "speak_active",

        "squeak(blue)",
        "text(blue,0,0,1)",
        "Ok then!",
        "position(blue,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "companion(8)",
        "setcheckpoint()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "skipblue") == 0)
    {
        static const char* lines[] = {
        "changeai(blue,followplayer)",
        "squeak(blue)",
        "changemood(blue,0)",
        "companion(8)",
        "rescued(blue)",
        "setcheckpoint()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "rescueyellow") == 0)
    {
        static const char* lines[] = {
        "ifskip(skipyellow)",
        "cutscene()",

        "changeai(yellow,followplayer)",
        "changetile(yellow,6)",
        "untilbars()",

        "rescued(yellow)",

        "squeak(yellow)",
        "text(yellow,0,0,2)",
        "Ah, Viridian! You got off",
        "the ship alright too? ",
        "position(yellow,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,2)",
        "It's good to see you're",
        "alright, Professor!",
        "position(player,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,1)",
        "Is the ship ok?",
        "position(yellow,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,2)",
        "It's badly damaged, but Violet's",
        "been working on fixing it.",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "We could really use your help...",
        "position(player,above)",
        "speak_active",

        "endtext",

        "fadeout()",
        "untilfade()",
        "delay(30)",
        "fadein()",
        "untilfade()",

        "squeak(yellow)",
        "text(yellow,0,0,1)",
        "Ah, of course!",
        "position(yellow,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,4)",
        "The background interference",
        "in this dimension prevented",
        "the ship from finding a",
        "teleporter when we crashed!",
        "position(yellow,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,2)",
        "We've all been teleported",
        "to different locations!",
        "position(yellow,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Er, that sounds about right!",
        "position(player,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,2)",
        "Let's get back to",
        "the ship, then!",
        "position(yellow,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,1)",
        "After you, Captain!",
        "position(yellow,above)",
        "speak_active",

        "endtext",

        "companion(7)",
        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "skipyellow") == 0)
    {
        static const char* lines[] = {
        "changeai(yellow,followplayer)",
        "changetile(yellow,6)",
        "squeak(yellow)",
        "rescued(yellow)",
        "companion(7)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "rescuegreen") == 0)
    {
        static const char* lines[] = {
        "ifskip(skipgreen)",
        "cutscene()",

        "tofloor()",
        "changemood(green,0)",
        "untilbars()",

        "rescued(green)",

        "squeak(green)",
        "text(green,0,0,1)",
        "Captain! I've been so worried!",
        "position(green,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Chief Verdigris! You're ok!",
        "position(player,above)",
        "speak_active",

        "squeak(cry)",
        "changemood(green,1)",
        "text(green,0,0,2)",
        "I've been trying to get out, but",
        "I keep going around in circles...",
        "position(green,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,2)",
        "I've come from the ship. I'm here",
        "to teleport you back to it.",
        "position(player,above)",
        "speak_active",

        "squeak(green)",
        "text(green,0,0,2)",
        "Is everyone else",
        "alright? Is Violet...",
        "position(green,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "She's fine - she's back on the ship!",
        "position(player,above)",
        "speak_active",

        "squeak(green)",
        "changemood(green,0)",
        "text(green,0,0,2)",
        "Oh! Great - Let's",
        "get going, then!",
        "position(green,above)",
        "speak_active",

        "endtext",

        "companion(6)",
        "endcutscene()",
        "untilbars()",
        "changeai(green,followplayer)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "skipgreen") == 0)
    {
        static const char* lines[] = {
        "changeai(green,followplayer)",
        "squeak(green)",
        "rescued(green)",
        "changemood(green,0)",
        "companion(6)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "rescuered") == 0)
    {
        static const char* lines[] = {
        "ifskip(skipred)",
        "cutscene()",

        "tofloor()",
        "changemood(red,0)",
        "untilbars()",

        "rescued(red)",

        "squeak(red)",
        "text(red,0,0,1)",
        "Captain!",
        "position(red,above)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,3)",
        "Am I ever glad to see you!",
        "I thought I was the only",
        "one to escape the ship...",
        "position(red,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Vermilion! I knew you'd be ok!",
        "position(player,above)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,1)",
        "So, what's the situation?",
        "position(red,above)",
        "speak_active",

        "endtext",

        "fadeout()",
        "untilfade()",
        "delay(30)",
        "fadein()",
        "untilfade()",

        "squeak(red)",
        "text(red,0,0,2)",
        "I see! Well, we'd better",
        "get back then.",
        "position(red,above)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,2)",
        "There's a teleporter",
        "in the next room.",
        "position(red,above)",
        "speak_active",

        "endtext",

        "companion(9)",
        "endcutscene()",
        "untilbars()",
        "changeai(red,followplayer)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "skipred") == 0)
    {
        static const char* lines[] = {
        "changeai(red,followplayer)",
        "squeak(red)",
        "rescued(red)",
        "changemood(red,0)",
        "companion(9)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "startexpolevel_station1") == 0)
    {
        static const char* lines[] = {
        //For the Eurogamer EXPO! Scrap later.
        "fadeout()",
        "musicfadeout()",
        "untilfade()",

        "cutscene()",
        "untilbars()",

        "resetgame",

        "gotoroom(4,10)",
        "gotoposition(232,113,0)",
        "setcheckpoint()",
        "changedir(player,1)",
        "fadein()",

        "play(5)",
        "loadscript(intro)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "startexpolevel_lab") == 0)
    {
        static const char* lines[] = {
        //For the Eurogamer EXPO! Scrap later.
        "fadeout()",
        "musicfadeout()",
        "untilfade()",
        "delay(30)",

        "resetgame",

        "gotoroom(2,16)",
        "gotoposition(58,193,0)",
        "setcheckpoint()",
        "changedir(player,1)",
        "fadein()",

        "stopmusic()",
        "play(3)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "startexpolevel_warp") == 0)
    {
        static const char* lines[] = {
        //For the Eurogamer EXPO! Scrap later.
        "fadeout()",
        "musicfadeout()",
        "untilfade()",
        "delay(30)",

        "resetgame",

        "gotoroom(14,1)",
        "gotoposition(45,73,0)",
        "setcheckpoint()",
        "changedir(player,1)",
        "fadein()",

        "stopmusic()",
        "play(3)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "startexpolevel_tower") == 0)
    {
        static const char* lines[] = {
        //For the Eurogamer EXPO! Scrap later.
        "fadeout()",
        "musicfadeout()",
        "untilfade()",
        "delay(30)",

        "resetgame",

        "gotoroom(8,9)",
        "gotoposition(95,193,0)",
        "setcheckpoint()",
        "changedir(player,1)",
        "fadein()",

        "stopmusic()",
        "play(2)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "skipint1") == 0)
    {
        static const char* lines[] = {
        "finalmode(41,56)",
        "gotoposition(52,89,0)",
        "changedir(player,1)",
        "setcheckpoint()",
        "delay(15)",

        "flash(5)",
        "shake(20)",
        "playef(9)",
        "showplayer()",
        "play(8)",

        "hascontrol()",
        "befadein()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "intermission_1") == 0)
    {
        static const char* lines[] = {
        "ifskip(skipint1)",
        "finalmode(41,56)",
        "gotoposition(52,89,0)",
        "changedir(player,1)",
        "setcheckpoint()",

        "cutscene()",

        "delay(15)",

        "flash(5)",
        "shake(20)",
        "playef(9)",

        "delay(35)",

        "flash(5)",
        "shake(20)",
        "playef(9)",

        "delay(25)",

        "flash(5)",
        "shake(20)",
        "playef(10)",

        "showplayer()",
        "play(8)",

        "befadein()",

        "iflast(2,int1yellow_1)",
        "iflast(3,int1red_1)",
        "iflast(4,int1green_1)",
        "iflast(5,int1blue_1)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1blue_1") == 0)
    {
        static const char* lines[] = {
        "delay(45)",

        "squeak(cry)",
        "text(blue,0,0,1)",
        "Waaaa!",
        "position(blue,above)",
        "speak_active",
        "face(player,blue)",
        "face(blue,player)",

        "squeak(blue)",
        "text(blue,0,0,1)",
        "Captain! Are you ok?",
        "position(blue,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,2)",
        "I'm ok... this...",
        "this isn't the ship...",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Where are we?",
        "position(player,above)",
        "speak_active",

        "squeak(cry)",
        "text(blue,0,0,1)",
        "Waaaa!",
        "position(blue,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,2)",
        "Something's gone wrong... We",
        "should look for a way back!",
        "position(player,above)",
        "speak_active",
        "endtext",

        "telesave()",

        "endcutscene()",
        "untilbars()",
        "gamestate(14)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1blue_2") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(player)",
        "text(player,0,0,1)",
        "Follow me! I'll help you!",
        "position(player,above)",
        "speak_active",

        "squeak(cry)",
        "text(blue,0,0,1)",
        "Promise you won't leave without me!",
        "position(blue,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "I promise! Don't worry!",
        "position(player,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "gamestate(11)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1blue_3") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,blue)",
        "face(blue,player)",

        "squeak(player)",
        "text(player,0,0,1)",
        "Are you ok down there, Doctor?",
        "position(player,below)",
        "speak_active",

        "squeak(cry)",
        "text(blue,0,0,1)",
        "I wanna go home!",
        "position(blue,above)",
        "speak_active",

        "squeak(blue)",
        "text(blue,0,0,2)",
        "Where are we? How did",
        "we even get here?",
        "position(blue,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,4)",
        "Well, Violet did say that the",
        "interference in the dimension",
        "we crashed in was causing",
        "problems with the teleporters...",
        "position(player,below)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "I guess something went wrong...",
        "position(player,below)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,3)",
        "But if we can find another",
        "teleporter, I think we can",
        "get back to the ship!",
        "position(player,below)",
        "speak_active",

        "squeak(blue)",
        "text(blue,0,0,1)",
        "Sniff...",
        "position(blue,above)",
        "speak_active",
        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1blue_4") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,blue)",
        "face(blue,player)",

        "squeak(cry)",
        "text(blue,0,0,1)",
        "Captain! Captain! Wait for me!",
        "position(blue,above)",
        "speak_active",

        "squeak(blue)",
        "text(blue,0,0,2)",
        "Please don't leave me behind!",
        "I don't mean to be a burden!",
        "position(blue,above)",
        "speak_active",

        "squeak(cry)",
        "text(blue,0,0,1)",
        "I'm scared!",
        "position(blue,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,2)",
        "Oh... don't worry Victoria,",
        "I'll look after you!",
        "position(player,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1blue_5") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,blue)",
        "face(blue,player)",

        "squeak(cry)",
        "text(blue,0,0,2)",
        "We're never going to get",
        "out of here, are we?",
        "position(blue,above)",
        "speak_active",

        "squeak(cry)",
        "changemood(player,1)",
        "text(player,0,0,1)",
        "I.. I don't know...",
        "position(player,above)",
        "speak_active",

        "squeak(cry)",
        "text(player,0,0,2)",
        "I don't know where we are or",
        "how we're going to get out...",
        "position(player,above)",
        "speak_active",


        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1blue_6") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,blue)",
        "face(blue,player)",

        "squeak(cry)",
        "text(blue,0,0,1)",
        "We're going to be lost forever!",
        "position(blue,above)",
        "speak_active",

        "squeak(player)",
        "changemood(player,0)",
        "text(player,0,0,2)",
        "Ok, come on... Things",
        "aren't that bad.",
        "position(player,above)",
        "speak_active",


        "squeak(player)",
        "text(player,0,0,2)",
        "I have a feeling that",
        "we're nearly home!",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,2)",
        "We can't be too far",
        "from another teleporter!",
        "position(player,above)",
        "speak_active",

        "squeak(cry)",
        "text(blue,0,0,1)",
        "I hope you're right, captain...",
        "position(blue,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1blue_7") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,blue)",
        "face(blue,player)",

        "squeak(blue)",
        "text(blue,0,0,2",
        "Captain! You were right!",
        "It's a teleporter!",
        "position(blue,above)",
        "speak_active",

        "squeak(player)",
        "changemood(player,0)",
        "text(player,0,0,3)",
        "Phew! You had me worried for a",
        "while there... I thought we",
        "were never going to find one.",
        "position(player,above)",
        "speak_active",


        "squeak(cry)",
        "changemood(blue,1)",
        "text(blue,0,0,1",
        "What? Really?",
        "position(blue,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,2)",
        "Anyway, let's go",
        "back to the ship.",
        "position(player,above)",
        "speak_active",

        "changemood(blue,0)",
        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1green_1") == 0)
    {
        static const char* lines[] = {
        "delay(45)",

        "squeak(green)",
        "text(green,0,0,1)",
        "Huh? This isn't the ship...",
        "position(green,above)",
        "speak_active",
        "face(player,green)",
        "face(green,player)",

        "squeak(green)",
        "text(green,0,0,1)",
        "Captain! What's going on?",
        "position(green,above)",
        "speak_active",

        "squeak(cry)",
        "changemood(player,1",
        "text(player,0,0,1)",
        "I... I don't know!",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Where are we?",
        "position(player,above)",
        "speak_active",

        "squeak(green)",
        "text(green,0,0,3)",
        "Uh oh, this isn't good...",
        "Something must have gone",
        "wrong with the teleporter!",
        "position(green,above)",
        "speak_active",

        "squeak(player)",
        "changemood(player,0",
        "text(player,0,0,1)",
        "Ok... no need to panic!",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "changemood(player,0",
        "text(player,0,0,1)",
        "Let's look for another teleporter!",
        "There's bound to be one around",
        "here somewhere!",
        "position(player,above)",
        "speak_active",

        "endtext",

        "telesave()",
        "endcutscene()",
        "untilbars()",
        "gamestate(14)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1green_2") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(player)",
        "text(player,0,0,1)",
        "Let's go this way!",
        "position(player,above)",
        "speak_active",

        "squeak(green)",
        "text(green,0,0,1)",
        "After you, Captain!",
        "position(green,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "gamestate(11)",
        };
        filllines(lines);

    }
    else if (SDL_strcmp(t, "int1green_3") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,green)",
        "face(green,player)",

        "squeak(green)",
        "text(green,0,0,2)",
        "So Violet's back on the",
        "ship? She's really ok?",
        "position(green,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,2)",
        "She's fine! She helped",
        "me find my way back!",
        "position(player,below)",
        "speak_active",

        "squeak(green)",
        "text(green,0,0,1)",
        "Oh, phew! I was worried about her.",
        "position(green,above)",
        "speak_active",
        "endtext",

        "delay(45)",

        "squeak(green)",
        "text(green,0,0,1)",
        "Captain, I have a secret...",
        "position(green,above)",
        "speak_active",

        "squeak(cry)",
        "changemood(green,1)",
        "text(green,0,0,1)",
        "I really like Violet!",
        "position(green,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Is that so?",
        "position(player,below)",
        "speak_active",

        "squeak(green)",
        "changemood(green,0)",
        "text(green,0,0,2)",
        "Please promise you",
        "won't tell her!",
        "position(green,above)",
        "speak_active",
        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1green_4") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,green)",
        "face(green,player)",

        "squeak(green)",
        "text(green,0,0,1)",
        "Hey again!",
        "position(green,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Hey!",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Are you doing ok?",
        "position(player,above)",
        "speak_active",

        "squeak(green)",
        "text(green,0,0,3)",
        "I think so! I really",
        "hope we can find a way",
        "back to the ship...",
        "position(green,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1green_5") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,green)",
        "face(green,player)",

        "squeak(green)",
        "text(green,0,0,1)",
        "So, about Violet...",
        "position(green,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Um, yeah?",
        "position(player,above)",
        "speak_active",

        "squeak(green)",
        "text(green,0,0,1)",
        "Do you have any advice?",
        "position(green,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Oh!",
        "position(player,above)",
        "speak_active",
        "endtext",

        "delay(45)",

        "squeak(player)",
        "text(player,0,0,1)",
        "Hmm...",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Um... you should... be yourself!",
        "position(player,above)",
        "speak_active",
        "endtext",

        "delay(15)",

        "squeak(green)",
        "text(green,0,0,1)",
        "Oh.",
        "position(green,above)",
        "speak_active",
        "endtext",

        "delay(75)",

        "squeak(green)",
        "text(green,0,0,1)",
        "Thanks Captain!",
        "position(green,above)",
        "speak_active",
        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1green_6") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,green)",
        "face(green,player)",

        "squeak(player)",
        "text(player,0,0,2)",
        "So, do you think you'll",
        "be able to fix the ship?",
        "position(player,above)",
        "speak_active",

        "squeak(green)",
        "text(green,0,0,2)",
        "Depends on how bad it ",
        "is... I think so, though!",
        "position(green,above)",
        "speak_active",

        "squeak(green)",
        "text(green,0,0,5)",
        "It's not very hard, really. The",
        "basic dimensional warping engine",
        "design is pretty simple, and if we",
        "can get that working we shouldn't",
        "have any trouble getting home.",
        "position(green,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Oh! Good!",
        "position(player,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1green_7") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,green)",
        "face(green,player)",

        "squeak(green)",
        "text(green,0,0,1)",
        "Finally! A teleporter!",
        "position(green,above)",
        "speak_active",

        "squeak(green)",
        "text(green,0,0,2)",
        "I was getting worried",
        "we wouldn't find one...",
        "position(green,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Let's head back to the ship!",
        "position(player,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1red_1") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(red)",
        "text(red,0,0,1)",
        "Wow! Where are we?",
        "position(red,above)",
        "speak_active",

        "squeak(cry)",
        "changemood(player,1)",
        "text(player,0,0,3)",
        "This... isn't right...",
        "Something must have gone",
        "wrong with the teleporter!",
        "position(player,above)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,3)",
        "Oh well... We can work",
        "it out when we get",
        "back to the ship!",
        "position(red,above)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,1)",
        "Let's go exploring!",
        "position(red,above)",
        "speak_active",

        "squeak(player)",
        "changemood(player,0)",
        "text(player,0,0,1)",
        "Ok then!",
        "position(player,above)",
        "speak_active",

        "endtext",

        "telesave()",
        "endcutscene()",
        "untilbars()",
        "gamestate(14)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1red_2") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,red)",
        "face(red,player)",

        "squeak(player)",
        "text(player,0,0,1)",
        "Follow me!",
        "position(player,above)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,1)",
        "Aye aye, Captain!",
        "position(red,above)",
        "speak_active",
        "endtext",

        "endcutscene()",
        "untilbars()",
        "gamestate(11)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1red_3") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,red)",
        "face(red,player)",

        "squeak(red)",
        "text(red,0,0,2)",
        "Hey Viridian... how did",
        "the crash happen, exactly?",
        "position(red,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,2)",
        "Oh, I don't really know -",
        "some sort of interference...",
        "position(player,below)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,2)",
        "...or something sciencey like",
        "that. It's not really my area.",
        "position(player,below)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,3)",
        "Ah! Well, do you think",
        "we'll be able to fix",
        "the ship and go home?",
        "position(red,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Of course! Everything will be ok!",
        "position(player,below)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1red_4") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,red)",
        "face(red,player)",

        "squeak(red)",
        "text(red,0,0,1)",
        "Hi again! You doing ok?",
        "position(red,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,2)",
        "I think so! But I really want",
        "to get back to the ship...",
        "position(player,above)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,3)",
        "We'll be ok! If we can find",
        "a teleporter somewhere we",
        "should be able to get back!",
        "position(red,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1red_5") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,red)",
        "face(red,player)",

        "squeak(red)",
        "text(red,0,0,1)",
        "Are we there yet?",
        "position(red,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "We're getting closer, I think...",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "I hope...",
        "position(player,above)",
        "speak_active",
        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1red_6") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,red)",
        "face(red,player)",

        "squeak(player)",
        "text(player,0,0,1)",
        "I wonder where we are, anyway?",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,3)",
        "This seems different from",
        "that dimension we crashed",
        "in, somehow...",
        "position(player,above)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,2)",
        "I dunno... But we must be",
        "close to a teleporter by now...",
        "position(red,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1red_7") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,red)",
        "face(red,player)",

        "squeak(player)",
        "text(player,0,0,1)",
        "We're there!",
        "position(player,above)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,2)",
        "See? I told you! Let's",
        "get back to the ship!",
        "position(red,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1yellow_1") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(yellow)",
        "text(yellow,0,0,1)",
        "Oooh! This is interesting...",
        "position(yellow,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,2)",
        "Captain! Have you",
        "been here before?",
        "position(yellow,above)",
        "speak_active",

        "squeak(cry)",
        "changemood(player,1)",
        "text(player,0,0,1)",
        "What? Where are we?",
        "position(player,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,3)",
        "I suspect something deflected",
        "our teleporter transmission!",
        "This is somewhere new...",
        "position(yellow,above)",
        "speak_active",

        "squeak(cry)",
        "changemood(player,1)",
        "text(player,0,0,1)",
        "Oh no!",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "changemood(player,0)",
        "text(player,0,0,3)",
        "We should try to find a",
        "teleporter and get back",
        "to the ship...",
        "position(player,above)",
        "speak_active",

        "endtext",

        "telesave()",
        "endcutscene()",
        "untilbars()",
        "gamestate(14)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1yellow_2") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,yellow)",
        "face(yellow,player)",

        "squeak(player)",
        "text(player,0,0,1)",
        "Follow me!",
        "position(player,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,1)",
        "Right behind you, Captain!",
        "position(yellow,above)",
        "speak_active",
        "endtext",

        "endcutscene()",
        "untilbars()",
        "gamestate(11)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1yellow_3") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,yellow)",
        "face(yellow,player)",

        "squeak(player)",
        "text(player,0,0,2)",
        "What do you make of",
        "all this, Professor?",
        "position(player,below)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,4)",
        "I'm guessing this dimension",
        "has something to do with the",
        "interference that caused",
        "us to crash!",
        "position(yellow,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,2)",
        "Maybe we'll find the",
        "cause of it here?",
        "position(yellow,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Oh wow! Really?",
        "position(player,below)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,4)",
        "Well, it's just a guess.",
        "I'll need to get back to",
        "the ship before I can do",
        "any real tests...",
        "position(yellow,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1yellow_4") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,yellow)",
        "face(yellow,player)",

        "squeak(yellow)",
        "text(yellow,0,0,1)",
        "Ohh! What was that?",
        "position(yellow,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "What was what?",
        "position(player,above)",
        "speak_active",

        "squeak(yellow)",
        "changeai(yellow,faceleft)", // changedir(yellow,0) doesn't work
        "text(yellow,0,0,2)",
        "That big... C thing!",
        "I wonder what it does?",
        "position(yellow,above)",
        "speak_active",

        "squeak(cry)",
        "changemood(player,1)",
        "text(player,0,0,2)",
        "Em... I don't really know",
        "how to answer that question...",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "changemood(player,0)",
        "text(player,0,0,3)",
        "It's probably best not",
        "to acknowledge that",
        "it's there at all.",
        "position(player,above)",
        "speak_active",

        "squeak(yellow)",
        "changeai(yellow,0)", // Make him face right again
        "text(yellow,0,0,2)",
        "Maybe we should take it back",
        "to the ship to study it?",
        "position(yellow,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,3)",
        "We really shouldn't think",
        "about it too much... Let's",
        "keep moving!",
        "position(player,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1yellow_5") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,yellow)",
        "face(yellow,player)",

        "squeak(yellow)",
        "text(yellow,0,0,3)",
        "You know, there's",
        "something really odd",
        "about this dimension...",
        "position(yellow,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Yeah?",
        "position(player,above)",
        "speak_active",

        "squeak(yellow)",
        "changedir(yellow,0)",
        "text(yellow,0,0,3)",
        "We shouldn't really be able",
        "to move between dimensions",
        "with a regular teleporter...",
        "position(yellow,above)",
        "speak_active",

        "squeak(yellow)",
        "changedir(yellow,0)",
        "text(yellow,0,0,2)",
        "Maybe this isn't a proper",
        "dimension at all?",
        "position(yellow,above)",
        "speak_active",

        "squeak(yellow)",
        "changedir(yellow,0)",
        "text(yellow,0,0,4)",
        "Maybe it's some kind of",
        "polar dimension? Something",
        "artificially created for",
        "some reason?",
        "position(yellow,above)",
        "speak_active",

        "squeak(yellow)",
        "changedir(yellow,1)",
        "text(yellow,0,0,2)",
        "I can't wait to get back to the",
        "ship. I have a lot of tests to run!",
        "position(yellow,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1yellow_6") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,yellow)",
        "face(yellow,player)",

        "squeak(yellow)",
        "text(yellow,0,0,3)",
        "I wonder if there's anything",
        "else in this dimension",
        "worth exploring?",
        "position(yellow,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,3)",
        "Maybe... but we should probably",
        "just focus on finding the rest",
        "of the crew for now...",
        "position(player,above)",
        "speak_active",
        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int1yellow_7") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,yellow)",
        "face(yellow,player)",

        "squeak(yellow)",
        "text(yellow,0,0,1)",
        "At last!",
        "position(yellow,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,1)",
        "Let's go back to the ship!",
        "position(yellow,above)",
        "speak_active",
        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "skipint2") == 0)
    {
        static const char* lines[] = {
        "finalmode(53,49)",
        "gotoposition(228,129,0)",
        "changedir(player,1)",
        "setcheckpoint()",
        "flash(5)",
        "shake(20)",
        "playef(10)",

        "showplayer()",
        "play(8)",

        "hascontrol()",
        "befadein()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "intermission_2") == 0)
    {
        static const char* lines[] = {
        "ifskip(skipint2)",
        "finalmode(53,49)",
        "gotoposition(228,129,0)",
        "changedir(player,1)",
        "setcheckpoint()",

        "cutscene()",

        "delay(15)",

        "flash(5)",
        "shake(20)",
        "playef(9)",

        "delay(35)",

        "flash(5)",
        "shake(20)",
        "playef(9)",

        "delay(25)",

        "flash(5)",
        "shake(20)",
        "playef(10)",

        "showplayer()",
        "play(8)",

        "befadein()",

        "changemood(player,1)",
        "text(player,0,0,1)",
        "Uh oh...",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "changemood(player,1)",
        "text(player,0,0,1)",
        "Not again!",
        "position(player,above)",
        "speak_active",

        "iflast(2,int2intro_yellow)",
        "iflast(3,int2intro_red)",
        "iflast(4,int2intro_green)",
        "iflast(5,int2intro_blue)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int2intro_yellow") == 0)
    {
        static const char* lines[] = {
        "squeak(cry)",
        "text(player,0,0,1)",
        "Vitellary? Where are you?",
        "position(player,above)",
        "speak_active",
        "endtext",
        "delay(15)",

        "flash(5)",
        "shake(20)",
        "playef(10)",
        "delay(15)",

        "changedir(player,0)",

        "createcrewman(150,-20,yellow,1,17,1)",

        "squeak(cry)",
        "text(yellow,170,50,1)",
        "Captain!",
        "speak_active",
        "endtext",

        "delay(15)",

        "squeak(player)",
        "changemood(player,0)",
        "text(player,0,0,1)",
        "Hang on! I'll save you!",
        "position(player,above)",
        "speak_active",

        "endtext",
        "telesave()",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int2intro_red") == 0)
    {
        static const char* lines[] = {
        "squeak(cry)",
        "text(player,0,0,1)",
        "Vermilion? Where are you?",
        "position(player,above)",
        "speak_active",
        "endtext",
        "delay(15)",

        "flash(5)",
        "shake(20)",
        "playef(10)",
        "delay(15)",

        "changedir(player,0)",

        "createcrewman(150,-20,red,0,17,1)",

        "squeak(red)",
        "text(red,170,50,1)",
        "Wheeeee!",
        "speak_active",
        "endtext",

        "delay(15)",

        "squeak(player)",
        "changemood(player,0)",
        "text(player,0,0,1)",
        "Hang on! I'll save you!",
        "position(player,above)",
        "speak_active",

        "endtext",
        "telesave()",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int2intro_green") == 0)
    {
        static const char* lines[] = {
        "squeak(cry)",
        "text(player,0,0,1)",
        "Verdigris? Where are you?",
        "position(player,above)",
        "speak_active",
        "endtext",
        "delay(15)",

        "flash(5)",
        "shake(20)",
        "playef(10)",
        "delay(15)",

        "changedir(player,0)",

        "createcrewman(150,-20,green,1,17,1)",

        "squeak(cry)",
        "text(green,170,50,1)",
        "Aaagghh!",
        "speak_active",
        "endtext",

        "delay(15)",

        "squeak(player)",
        "changemood(player,0)",
        "text(player,0,0,1)",
        "Hang on! I'll save you!",
        "position(player,above)",
        "speak_active",

        "endtext",
        "telesave()",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int2intro_blue") == 0)
    {
        static const char* lines[] = {
        "squeak(cry)",
        "text(player,0,0,1)",
        "Victoria? Where are you?",
        "position(player,above)",
        "speak_active",
        "endtext",
        "delay(15)",

        "flash(5)",
        "shake(20)",
        "playef(10)",
        "delay(15)",

        "changedir(player,0)",

        "createcrewman(150,-20,blue,1,17,1)",

        "squeak(cry)",
        "text(blue,170,50,1)",
        "Help!",
        "speak_active",
        "endtext",

        "delay(15)",

        "squeak(player)",
        "changemood(player,0)",
        "text(player,0,0,1)",
        "Hang on! I'll save you!",
        "position(player,above)",
        "speak_active",

        "endtext",
        "telesave()",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int2_yellow") == 0)
    {
        static const char* lines[] = {
        "ifskip(skipint2yellow)",
        "cutscene()",

        "tofloor()",
        "changeai(yellow,followplayer)",
        "untilbars()",

        "squeak(yellow)",
        "text(yellow,0,0,1)",
        "That was interesting, wasn't it?",
        "position(yellow,above)",
        "speak_active",

        "squeak(cry)",
        "changemood(player,1)",
        "text(player,0,0,1)",
        "I feel dizzy...",
        "position(player,above)",
        "speak_active",

        "endtext",

        "changemood(player,0)",

        "endcutscene()",
        "untilbars()",
        "companion(10)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "skipint2yellow") == 0)
    {
        static const char* lines[] = {
        "squeak(yellow)",
        "companion(10)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int2_red") == 0)
    {
        static const char* lines[] = {
        "ifskip(skipint2red)",
        "cutscene()",

        "tofloor()",
        "changeai(red,followplayer)",
        "untilbars()",

        "squeak(red)",
        "text(red,0,0,1)",
        "Again! Let's go again!",
        "position(red,above)",
        "speak_active",

        "squeak(cry)",
        "changemood(player,1)",
        "text(player,0,0,1)",
        "I feel dizzy...",
        "position(player,above)",
        "speak_active",

        "endtext",

        "changemood(player,0)",

        "endcutscene()",
        "untilbars()",
        "companion(10)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "skipint2red") == 0)
    {
        static const char* lines[] = {
        "squeak(red)",
        "companion(10)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int2_green") == 0)
    {
        static const char* lines[] = {
        "ifskip(skipint2green)",
        "cutscene()",

        "tofloor()",
        "changeai(green,followplayer)",
        "untilbars()",

        "squeak(green)",
        "text(green,0,0,1)",
        "Phew! You're ok!",
        "position(green,above)",
        "speak_active",

        "squeak(cry)",
        "changemood(player,1)",
        "text(player,0,0,1)",
        "I feel dizzy...",
        "position(player,above)",
        "speak_active",

        "endtext",

        "changemood(player,0)",

        "endcutscene()",
        "untilbars()",
        "companion(10)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "skipint2green") == 0)
    {
        static const char* lines[] = {
        "squeak(green)",
        "companion(10)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "int2_blue") == 0)
    {
        static const char* lines[] = {
        "ifskip(skipint2blue)",
        "cutscene()",

        "tofloor()",
        "changeai(blue,followplayer)",
        "untilbars()",

        "squeak(cry)",
        "text(blue,0,0,1)",
        "I think I'm going to be sick...",
        "position(blue,above)",
        "speak_active",

        "squeak(cry)",
        "changemood(player,1)",
        "text(player,0,0,1)",
        "I feel dizzy...",
        "position(player,above)",
        "speak_active",

        "endtext",

        "changemood(player,0)",
        "changemood(blue,0)",

        "endcutscene()",
        "untilbars()",
        "companion(10)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "skipint2blue") == 0)
    {
        static const char* lines[] = {
        "squeak(blue)",
        "companion(10)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "startexpolevel_station2") == 0)
    {
        //For the Eurogamer EXPO! Scrap later.
        static const char* lines[] = {
        "fadeout()",
        "musicfadeout()",
        "untilfade()",
        "delay(30)",

        "resetgame",

        "gotoroom(12,14)",
        "gotoposition(126,38,1)",
        "setcheckpoint()",
        "changedir(player,0)",
        "fadein()",

        "stopmusic()",
        "play(1)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "finallevel_teleporter") == 0)
    {
        static const char* lines[] = {
        "delay(10)",

        "squeak(purple)",
        "text(purple,0,0,1)",
        "Welcome back!",
        "position(purple,above)",
        "speak_active",
        "endtext",

        "delay(30)",

        "squeak(purple)",
        "text(purple,0,0,1)",
        "...",
        "position(purple,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,1)",
        "Um, where's Captain Viridian?",
        "position(purple,above)",
        "speak_active",
        "endtext",

        "delay(30)",

        "walk(left,3)",
        "delay(60)",

        "everybodysad()",
        "squeak(cry)",
        "delay(30)",

        "fadeout()",
        "untilfade()",
        "changemood(player,0)",
        "musicfadeout()",

        "finalmode(46,54)",
        "gotoposition(101,113,0)",
        "setcheckpoint()",
        "changedir(player,1)",
        "restoreplayercolour",
        "fadein()",
        "untilfade()",

        "delay(15)",

        "squeak(player)",
        "text(player,0,0,1)",
        "... Hello?",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Is anyone there?",
        "position(player,above)",
        "speak_active",
        "endtext",

        "missing(player)",

        //"squeak(cry)",
        //"changemood(player,1)",

        "endcutscene()",
        "untilbars()",

        "play(15)",
        "telesave()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "skipfinal") == 0)
    {
        static const char* lines[] = {
        "finalmode(46,54)",
        "gotoposition(101,113,0)",
        "setcheckpoint()",
        "changedir(player,1)",
        "restoreplayercolour",

        "showplayer()",
        "hascontrol()",
        "missing(player)",
        "play(15)",
        "fadein()",
        "untilfade()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "startlevel_final") == 0)
    {
        static const char* lines[] = {
        "ifskip(skipfinal)",
        "hideplayer()",
        "cutscene()",
        "untilbars()",
        "activeteleporter()",

        "stopmusic()",
        "play(5)",

        "gotoroom(2,11)",
        "gotoposition(160,120,0)",

        "createcrewman(190,153,purple,0,faceleft)",
        "createrescuedcrew()",

        "fadein()",
        "untilfade()",
        "gamestate(4070)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "regularreturn") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "activeteleporter()",

        "stopmusic()",
        "play(4)",

        "gotoroom(2,11)",
        "gotoposition(160,120,0)",

        "createlastrescued()",

        "fadein()",
        "untilfade()",
        "endcutscene()",
        "setcheckpoint()",
        "gamestate(4010)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "returntohub") == 0)
    {
        //For the Eurogamer EXPO! Scrap later.
        static const char* lines[] = {
        "fadeout()",
        "musicfadeout()",
        "untilfade()",
        "delay(30)",

        "resetgame",

        "gotoroom(7,8)",
        "gotoposition(145,145,0)",
        "setcheckpoint()",
        "changedir(player,0)",
        "fadein()",

        "stopmusic()",
        "play(4)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "resetgame") == 0)
    {
        //For the Eurogamer EXPO! Scrap later.
        static const char* lines[] = {
        "resetgame",
        "gotoroom(4,6)",
        "fadein()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkred") == 0)
    {
        static const char* lines[] = {
        "redcontrol",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkyellow") == 0)
    {
        static const char* lines[] = {
        "yellowcontrol",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkgreen") == 0)
    {
        static const char* lines[] = {
        "greencontrol",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkblue") == 0)
    {
        static const char* lines[] = {
        "bluecontrol",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkpurple") == 0)
    {
        static const char* lines[] = {
        "purplecontrol",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkred_1") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,red)",
        "face(red,player)",

        "squeak(red)",
        "text(red,0,0,1)",
        "Don't worry, Sir!",
        "position(red,above)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,2)",
        "We'll find a way",
        "out of here!",
        "position(red,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(red)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkred_2") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,red)",
        "face(red,player)",

        "squeak(red)",
        "text(red,0,0,1)",
        "I hope Victoria is ok...",
        "position(red,above)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,2)",
        "She doesn't handle",
        "surprises very well...",
        "position(red,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(red)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkred_3") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,red)",
        "face(red,player)",

        "squeak(red)",
        "text(red,0,0,3)",
        "I don't know how we're",
        "going to get this ship",
        "working again!",
        "position(red,above)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,2)",
        "Chief Verdigris would",
        "know what to do...",
        "position(red,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(red)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkred_4") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,red)",
        "face(red,player)",

        "squeak(red)",
        "text(red,0,0,2)",
        "I wonder what caused",
        "the ship to crash here?",
        "position(red,above)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,3)",
        "It's the shame the Professor",
        "isn't here, huh? I'm sure he",
        "could work it out!",
        "position(red,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(red)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkred_5") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,red)",
        "face(red,player)",

        "squeak(red)",
        "text(red,0,0,1)",
        "It's great to be back!",
        "position(red,above)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,2)",
        "I can't wait to help you",
        "find the rest of the crew!",
        "position(red,above)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,2)",
        "It'll be like old",
        "times, huh, Captain?",
        "position(red,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(red)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkred_6") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,red)",
        "face(red,player)",

        "squeak(red)",
        "text(red,0,0,2)",
        "It's good to have",
        "Victoria back with us.",
        "position(red,above)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,2)",
        "She really seems happy to",
        "get back to work in her lab!",
        "position(red,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(red)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkred_7") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,red)",
        "face(red,player)",

        "squeak(red)",
        "text(red,0,0,3)",
        "I think I saw Verdigris",
        "working on the outside",
        "of the ship!",
        "position(red,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(red)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkred_8") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,red)",
        "face(red,player)",

        "squeak(red)",
        "text(red,0,0,2)",
        "You found Professor",
        "Vitellary! All right!",
        "position(red,above)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,2)",
        "We'll have this interference",
        "thing worked out in no time now!",
        "position(red,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(red)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkred_9") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,red)",
        "face(red,player)",

        "squeak(red)",
        "text(red,0,0,2)",
        "That other dimension was",
        "really strange, wasn't it?",
        "position(red,above)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,2)",
        "I wonder what caused the",
        "teleporter to send us there?",
        "position(red,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(red)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkred_10") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,red)",
        "face(red,player)",

        "squeak(red)",
        "text(red,0,0,1)",
        "Heya Captain!",
        "position(red,above)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,2)",
        "This way looks a little",
        "dangerous...",
        "position(red,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(red)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkred_11") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,red)",
        "face(red,player)",

        "squeak(red)",
        "text(red,0,0,1)",
        "I'm helping!",
        "position(red,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(red)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkred_12") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,red)",
        "face(red,player)",

        "squeak(red)",
        "text(red,0,0,1)",
        "Hey Captain!",
        "position(red,above)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,3)",
        "I found something interesting",
        "around here - the same warp",
        "signature I saw when I landed!",
        "position(red,above)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,2)",
        "Someone from the ship",
        "must be nearby...",
        "position(red,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(red)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkred_13") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,red)",
        "face(red,player)",

        "squeak(red)",
        "text(red,0,0,2)",
        "This dimension is pretty",
        "exciting, isn't it?",
        "position(red,above)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,1)",
        "I wonder what we'll find?",
        "position(red,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(red)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkred_14") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,red)",
        "face(red,player)",

        "squeak(red)",
        "text(red,0,0,1)",
        "Look what I found!",
        "position(red,above)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,2)",
        "It's pretty hard, I can only",
        "last for about 10 seconds...",
        "position(red,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(red)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkyellow_1") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,yellow)",
        "face(yellow,player)",

        "squeak(yellow)",
        "text(yellow,0,0,2)",
        "I'm making some fascinating",
        "discoveries, captain!",
        "position(yellow,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(yellow)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkyellow_2") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,yellow)",
        "face(yellow,player)",

        "squeak(yellow)",
        "text(yellow,0,0,3)",
        "This isn't like any",
        "other dimension we've",
        "been to, Captain.",
        "position(yellow,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,2)",
        "There's something strange",
        "about this place...",
        "position(yellow,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(yellow)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkyellow_3") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,yellow)",
        "face(yellow,player)",

        "squeak(yellow)",
        "text(yellow,0,0,3)",
        "Captain, have you noticed",
        "that this dimension seems",
        "to wrap around?",
        "position(yellow,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Yeah, it's strange...",
        "position(player,above)",
        "speak_active",

        "squeak(cry)",
        "changemood(yellow,1)",
        "text(yellow,0,0,3)",
        "It looks like this dimension",
        "is having the same stability",
        "problems as our own!",
        "position(yellow,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,2)",
        "I hope we're not the",
        "ones causing it...",
        "position(yellow,above)",
        "speak_active",

        "squeak(cry)",
        "changemood(player,1)",
        "text(player,0,0,1)",
        "What? Do you think we might be?",
        "position(player,above)",
        "speak_active",

        "squeak(yellow)",
        "changemood(yellow,0)",
        "changemood(player,0)",
        "text(yellow,0,0,2)",
        "No no... that's very",
        "unlikely, really...",
        "position(yellow,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(yellow)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkyellow_4") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,yellow)",
        "face(yellow,player)",

        "squeak(yellow)",
        "text(yellow,0,0,4)",
        "My guess is that whoever used",
        "to live here was experimenting",
        "with ways to stop the dimension",
        "from collapsing.",
        "position(yellow,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,2)",
        "It would explain why they've",
        "wrapped the edges...",
        "position(yellow,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,2)",
        "Hey, maybe that's what's",
        "causing the interference?",
        "position(yellow,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(yellow)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkyellow_5") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,yellow)",
        "face(yellow,player)",

        "squeak(yellow)",
        "text(yellow,0,0,2)",
        "I wonder where the people who",
        "used to live here have gone?",
        "position(yellow,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(yellow)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkyellow_6") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,yellow)",
        "face(yellow,player)",

        "squeak(yellow)",
        "text(yellow,0,0,3)",
        "I think it's no coincidence",
        "that the teleporter was drawn",
        "to that dimension...",
        "position(yellow,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,4)",
        "There's something there. I",
        "think it might be causing the",
        "interference that's stopping",
        "us from leaving...",
        "position(yellow,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(yellow)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkyellow_7") == 0)
    {
        //Vertigris is back
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,yellow)",
        "face(yellow,player)",

        "squeak(yellow)",
        "text(yellow,0,0,1)",
        "I'm glad Verdigris is alright.",
        "position(yellow,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,3)",
        "It'll be a lot easier to find",
        "some way out of here now that",
        "we can get the ship working again!",
        "position(yellow,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(yellow)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkyellow_8") == 0)
    {
        //Victoria is back
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,yellow)",
        "face(yellow,player)",

        "squeak(yellow)",
        "text(yellow,0,0,2)",
        "Ah, you've found Doctor",
        "Victoria? Excellent!",
        "position(yellow,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,1)",
        "I have lots of questions for her!",
        "position(yellow,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(yellow)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkyellow_9") == 0)
    {
        //Vermilion is back
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,yellow)",
        "face(yellow,player)",

        "squeak(yellow)",
        "text(yellow,0,0,3)",
        "Vermilion says that he",
        "was trapped in some",
        "sort of tunnel?",
        "position(yellow,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,2)",
        "Yeah, it just seemed to",
        "keep going and going...",
        "position(player,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,2)",
        "Interesting... I wonder",
        "why it was built?",
        "position(yellow,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(yellow)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkyellow_10") == 0)
    {
        //Back on the ship!
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,yellow)",
        "face(yellow,player)",

        "squeak(yellow)",
        "text(yellow,0,0,1)",
        "It's good to be back!",
        "position(yellow,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,2)",
        "I've got so much work",
        "to catch up on...",
        "position(yellow,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(yellow)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkyellow_11") == 0)
    {
        //Game Complete
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,yellow)",
        "face(yellow,player)",

        "squeak(yellow)",
        "text(yellow,0,0,3)",
        "I know it's probably a little",
        "dangerous to stay here now that",
        "this dimension is collapsing...",
        "position(yellow,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,2)",
        "...but it's so rare to find",
        "somewhere this interesting!",
        "position(yellow,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,2)",
        "Maybe we'll find the answers",
        "to our own problems here?",
        "position(yellow,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(yellow)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkyellow_12") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,yellow)",
        "face(yellow,player)",

        "squeak(yellow)",
        "text(yellow,0,0,1)",
        "Captain! Have you seen this?",
        "position(yellow,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,3)",
        "With their research and ours,",
        "we should be able to stabilise",
        "our own dimension!",
        "position(yellow,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,1)",
        "We're saved!",
        "position(yellow,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(yellow)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkgreen_1") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,green)",
        "face(green,player)",

        "squeak(green)",
        "text(green,0,0,1)",
        "I'm an engineer!",
        "position(green,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(green)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkgreen_2") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,green)",
        "face(green,player)",

        "squeak(green)",
        "text(green,0,0,3)",
        "I think I can get this ship",
        "moving again, but it's going",
        "to take a while...",
        "position(green,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(green)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkgreen_3") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,green)",
        "face(green,player)",

        "squeak(green)",
        "text(green,0,0,3)",
        "Victoria mentioned something",
        "about a lab? I wonder if she",
        "found anything down there?",
        "position(green,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(green)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkgreen_4") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,green)",
        "face(green,player)",

        "squeak(green)",
        "text(green,0,0,1)",
        "Vermilion's back! Yey!",
        "position(green,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(green)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkgreen_5") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,green)",
        "face(green,player)",

        "squeak(green)",
        "text(green,0,0,3)",
        "The Professor had lots of",
        "questions about this",
        "dimension for me...",
        "position(green,above)",
        "speak_active",

        "squeak(green)",
        "text(green,0,0,2)",
        "We still don't really know",
        "that much, though.",
        "position(green,above)",
        "speak_active",

        "squeak(green)",
        "text(green,0,0,3)",
        "Until we work out what's",
        "causing that interference,",
        "we can't go anywhere.",
        "position(green,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(green)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkgreen_6") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,green)",
        "face(green,player)",

        "squeak(green)",
        "text(green,0,0,2)",
        "I'm so glad that",
        "Violet's alright!",
        "position(green,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(green)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkgreen_7") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,green)",
        "face(green,player)",

        "squeak(green)",
        "text(green,0,0,3)",
        "That other dimension we ended",
        "up in must be related to this",
        "one, somehow...",
        "position(green,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(green)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkgreen_8") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,green)",
        "face(green,player)",

        "squeak(cry)",
        "text(green,0,0,3)",
        "The antenna's broken!",
        "This is going to be",
        "very hard to fix...",
        "position(green,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(green)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkgreen_9") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,green)",
        "face(green,player)",

        "squeak(green)",
        "text(green,0,0,2)",
        "It looks like we were warped",
        "into solid rock when we crashed!",
        "position(green,above)",
        "speak_active",

        "squeak(green)",
        "text(green,0,0,2)",
        "Hmm. It's going to be hard",
        "to separate from this...",
        "position(green,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(green)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkgreen_10") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,green)",
        "face(green,player)",

        "squeak(green)",
        "text(green,0,0,2)",
        "The ship's all fixed up. We",
        "can leave at a moment's notice!",
        "position(green,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(green)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkgreen_11") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,green)",
        "face(green,player)",

        "squeak(green)",
        "text(green,0,0,3)",
        "I wonder why they abandoned this",
        "dimension? They were so close to",
        "working out how to fix it...",
        "position(green,above)",
        "speak_active",

        "squeak(green)",
        "text(green,0,0,2)",
        "Maybe we can fix it for them?",
        "Maybe they'll come back?",
        "position(green,above)",
        "speak_active",
        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(green)",
        };
        filllines(lines);

    }


    if (SDL_strcmp(t, "talkpurple_1") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,purple)",
        "face(purple,player)",

        "squeak(cry)",
        "changemood(purple,1)",
        "text(purple,0,0,1)",
        "... I hope Verdigris is alright.",
        "position(purple,above)",
        "speak_active",

        "squeak(purple)",
        "changemood(purple,0)",
        "text(purple,0,0,2)",
        "If you can find him, he'd be a",
        "a big help fixing the ship!",
        "position(purple,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(purple)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkpurple_2") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,purple)",
        "face(purple,player)",

        "squeak(purple)",
        "text(purple,0,0,2)",
        "Chief Verdigris is so brave",
        "and ever so smart!",
        "position(purple,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(purple)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkpurple_3") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,purple)",
        "face(purple,player)",

        "squeak(purple)",
        "text(purple,0,0,1)",
        "Are you doing ok, Captain?",
        "position(purple,above)",
        "speak_active",

        "squeak(cry)",
        "changemood(player,1)",
        "text(player,0,0,0)",
        "specialline(1)",
        "position(player,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,2)",
        "Oh - well, don't worry,",
        "they'll show up!",
        "position(purple,above)",
        "speak_active",

        "changemood(player,0)",
        "squeak(purple)",
        "text(purple,0,0,1)",
        "Here! Have a lollipop!",
        "position(purple,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(purple)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkpurple_4") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,purple)",
        "face(purple,player)",

        "squeak(purple)",
        "text(purple,0,0,1)",
        "Welcome back, Captain!",
        "position(purple,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,2)",
        "I think Victoria is quite happy",
        "to be back on the ship.",
        "position(purple,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,2)",
        "She really doesn't like adventuring.",
        "She gets very homesick!",
        "position(purple,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(purple)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkpurple_5") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,purple)",
        "face(purple,player)",

        "squeak(purple)",
        "text(purple,0,0,2)",
        "Vermilion called in",
        "to say hello!",
        "position(purple,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,1)",
        "He's really looking forward",
        "specialline(2)",
        "position(purple,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(purple)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkpurple_6") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,purple)",
        "face(purple,player)",

        "squeak(purple)",
        "text(purple,0,0,1)",
        "Captain! You found Verdigris!",
        "position(purple,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,1)",
        "Thank you so much!",
        "position(purple,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(purple)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkpurple_7") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,purple)",
        "face(purple,player)",

        "squeak(purple)",
        "text(purple,0,0,2)",
        "I'm glad Professor",
        "Vitellary is ok!",
        "position(purple,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,2)",
        "He had lots of questions",
        "for me about this dimension.",
        "position(purple,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,2)",
        "He's already gotten to",
        "work with his research!",
        "position(purple,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(purple)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkpurple_8") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,purple)",
        "face(purple,player)",

        "squeak(purple)",
        "text(purple,0,0,4)",
        "Hey Captain! Now that you've turned",
        "off the source of the interference,",
        "we can warp everyone back to the",
        "ship instantly, if we need to!",
        "position(purple,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,3)",
        "Any time you want to come back",
        "to the ship, just select the",
        "new SHIP option in your menu!",
        "position(purple,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(purple)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkpurple_9") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,purple)",
        "face(purple,player)",

        "squeak(purple)",
        "text(purple,0,0,3)",
        "Look at all this research!",
        "This is going to be a big",
        "help back home!",
        "position(purple,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(purple)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkpurple_intermission1") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,purple)",
        "face(purple,player)",

        "squeak(player)",
        "text(player,0,0,3)",
        "Doctor, something strange",
        "happened when we teleported",
        "back to the ship...",
        "position(player,above)",
        "speak_active",

        "squeak(cry)",
        "changemood(player,1)",
        "text(player,0,0,1)",
        "We got lost in another dimension!",
        "position(player,above)",
        "speak_active",

        "squeak(cry)",
        "changemood(purple,1)",
        "text(purple,0,0,1)",
        "Oh no!",
        "position(purple,above)",
        "speak_active",

        "squeak(purple)",
        "changemood(purple,0)",
        "changemood(player,0)",
        "text(purple,0,0,3)",
        "Maybe that dimension has something",
        "to do with the interference that",
        "caused us to crash here?",
        "position(purple,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,1)",
        "I'll look into it...",
        "position(purple,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(purple)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkpurple_intermission2") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,purple)",
        "face(purple,player)",

        "squeak(cry)",
        "changemood(player,1)",
        "text(player,0,0,1)",
        "Doctor! Doctor! It happened again!",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,2)",
        "The teleporter brought us",
        "to that weird dimension...",
        "position(player,above)",
        "speak_active",

        "squeak(purple)",
        "changemood(player,0)",
        "changemood(purple,0)",
        "text(purple,0,0,2)",
        "Hmm, there's definitely",
        "something strange happening...",
        "position(purple,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,2)",
        "If only we could find the",
        "source of that interference!",
        "position(purple,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(purple)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkpurple_intermission3") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,purple)",
        "face(purple,player)",

        "squeak(player)",
        "text(player,0,0,3)",
        "Doctor, something strange has",
        "been happening when we teleport",
        "back to the ship...",
        "position(player,above)",
        "speak_active",

        "squeak(cry)",
        "changemood(player,1)",
        "text(player,0,0,2)",
        "We keep getting brought to",
        "another weird dimension!",
        "position(player,above)",
        "speak_active",

        "squeak(cry)",
        "changemood(purple,1)",
        "text(purple,0,0,1)",
        "Oh no!",
        "position(purple,above)",
        "speak_active",

        "squeak(purple)",
        "changemood(purple,0)",
        "changemood(player,0)",
        "text(purple,0,0,3)",
        "Maybe that dimension has something",
        "to do with the interference that",
        "caused us to crash here?",
        "position(purple,above)",
        "speak_active",

        "squeak(purple)",
        "changemood(player,0)",
        "changemood(purple,0)",
        "text(purple,0,0,2)",
        "Hmm, there's definitely",
        "something strange happening...",
        "position(purple,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,2)",
        "If only we could find the",
        "source of that interference!",
        "position(purple,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(purple)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkpurple_intro") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,purple)",
        "face(purple,player)",

        "squeak(cry)",
        "changemood(player,1)",
        "text(player,0,0,2)",
        "I'm feeling a bit",
        "overwhelmed, Doctor.",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Where do I begin?",
        "position(player,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,2)",
        "Remember that you can press ENTER",
        "to check where you are on the map!",
        "position(purple,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,2)",
        "Look for areas where the rest",
        "of the crew might be...",
        "position(purple,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,2)",
        "If you get lost, you can get back",
        "to the ship from any teleporter.",
        "position(purple,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,2)",
        "And don't worry!",
        "We'll find everyone!",
        "position(purple,above)",
        "speak_active",

        "endtext",

        "delay(30)",

        "changemood(player,0)",
        "squeak(purple)",
        "text(purple,0,0,1)",
        "Everything will be ok!",
        "position(purple,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(purple)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkblue_1") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,blue)",
        "face(blue,player)",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,1)",
        "Any signs of Professor Vitellary?",
        "position(blue,below)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Sorry, not yet...",
        "position(player,above)",
        "speak_active",

        "squeak(cry)",
        "changetile(blue,150)", //upside down frown :(
        "text(blue,0,0,1)",
        "I hope he's ok...",
        "position(blue,below)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(blue)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkblue_2") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,blue)",
        "face(blue,player)",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,2)",
        "Thanks so much for",
        "saving me, Captain!",
        "position(blue,below)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(blue)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkblue_3") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,blue)",
        "face(blue,player)",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,1)",
        "I'm so glad to be back!",
        "position(blue,below)",
        "speak_active",

        "squeak(cry)",
        "changetile(blue,150)", //upside down frown :(
        "text(blue,0,0,3)",
        "That lab was so dark",
        "and scary! I didn't",
        "like it at all...",
        "position(blue,below)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(blue)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkblue_4") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,blue)",
        "face(blue,player)",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,2)",
        "Vitellary's back? I",
        "knew you'd find him!",
        "position(blue,below)",
        "speak_active",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,2)",
        "I mean, I admit I was very",
        "worried that you wouldn't...",
        "position(blue,below)",
        "speak_active",

        "squeak(cry)",
        "changetile(blue,150)", //upside down frown :(
        "text(blue,0,0,2)",
        "or that something might",
        "have happened to him...",
        "position(blue,below)",
        "speak_active",

        "squeak(blue)",
        "text(blue,0,0,1)",
        "sniff...",
        "position(blue,below)",
        "speak_active",

        "endtext",

        "delay(30)",

        "squeak(player)",
        "text(player,0,0,1)",
        "Doctor Victoria? He's ok!",
        "position(player,above)",
        "speak_active",

        "squeak(cry)",
        "changetile(blue,150)", //upside down frown :(
        "text(blue,0,0,3)",
        "Oh! Sorry! I was just",
        "thinking about what",
        "if he wasn't?",
        "position(blue,below)",
        "speak_active",

        "endtext",

        "delay(30)",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,1)",
        "Thank you, Captain!",
        "position(blue,below)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(blue)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkblue_5") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,blue)",
        "face(blue,player)",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,1)",
        "You found Vermilion! Great!",
        "position(blue,below)",
        "speak_active",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,2)",
        "I wish he wasn't",
        "so reckless!",
        "position(blue,below)",
        "speak_active",

        "squeak(cry)",
        "changetile(blue,150)", //upside down frown :(
        "text(blue,0,0,2)",
        "He'll get himself",
        "into trouble...",
        "position(blue,below)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(blue)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkblue_6") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,blue)",
        "face(blue,player)",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,2)",
        "Verdigris is ok! Violet",
        "will be so happy!",
        "position(blue,below)",
        "speak_active",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,1)",
        "I'm happy!",
        "position(blue,below)",
        "speak_active",

        "endtext",

        "delay(30)",

        "squeak(cry)",
        "changetile(blue,150)", //upside down frown :(
        "text(blue,0,0,1)",
        "Though I was very worried...",
        "position(blue,below)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(blue)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkblue_7") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,blue)",
        "face(blue,player)",

        "squeak(cry)",
        "changetile(blue,150)", //upside down frown :(
        "text(blue,0,0,2)",
        "Why did the teleporter send",
        "us to that scary dimension?",
        "position(blue,below)",
        "speak_active",

        "squeak(blue)",
        "changetile(blue,150)", //upside down frown :(
        "text(blue,0,0,1)",
        "What happened?",
        "position(blue,below)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "I don't know, Doctor...",
        "position(player,above)",
        "speak_active",

        "squeak(cry)",
        "changetile(blue,150)", //upside down frown :(
        "text(blue,0,0,1)",
        "Why?",
        "position(blue,below)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(blue)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkblue_8") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,blue)",
        "face(blue,player)",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,1)",
        "Heya Captain!",
        "position(blue,below)",
        "speak_active",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,3)",
        "Are you going to try",
        "and find the rest of",
        "these shiny things?",
        "position(blue,below)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(blue)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkblue_9") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,blue)",
        "face(blue,player)",

        "squeak(blue)",
        "text(blue,0,0,3)",
        "This lab is amazing! The scientists",
        "who worked here know a lot more",
        "about warp technology than we do!",
        "position(blue,below)",
        "speak_active",
        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(blue)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkblue_trinket1") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,blue)",
        "face(blue,player)",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,2)",
        "Hey Captain, I found",
        "this in that lab...",
        "position(blue,below)",
        "speak_active",

        "endtext",
        "delay(30)",

        //found a trinket!
        "foundtrinket(18)",
        "endtext",
        "musicfadein",

        "delay(30)",
        "createentity(136,80,22,18,0)",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,1)",
        "Any idea what it does?",
        "position(blue,below)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Sorry, I don't know!",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "They seem important, though...",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,2)",
        "Maybe something will happen",
        "if we find them all?",
        "position(player,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(blue)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkblue_trinket2") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,blue)",
        "face(blue,player)",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,3)",
        "Captain! Come have a",
        "look at what I've",
        "been working on!",
        "position(blue,below)",
        "speak_active",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,3)",
        "It looks like these shiny",
        "things are giving off a",
        "strange energy reading!",
        "position(blue,below)",
        "speak_active",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,1)",
        "So I analysed it...",
        "position(blue,below)",
        "speak_active",

        "trinketbluecontrol()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkblue_trinket3") == 0)
    {
        //If you missed the first conversation
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,blue)",
        "face(blue,player)",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,3)",
        "Captain! Come have a",
        "look at what I've",
        "been working on!",
        "position(blue,below)",
        "speak_active",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,1)",
        "I found this in that lab...",
        "position(blue,below)",
        "speak_active",

        "endtext",
        "delay(30)",

        //found a trinket!
        "foundtrinket(18)",
        "endtext",
        "musicfadein",

        "delay(30)",
        "createentity(136,80,22,18,0)",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,3)",
        "It seemed to be",
        "giving off a weird",
        "energy reading...",
        "position(blue,below)",
        "speak_active",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,1)",
        "So I analysed it...",
        "position(blue,below)",
        "speak_active",

        "trinketbluecontrol()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkblue_trinket4") == 0)
    {
        static const char* lines[] = {
        "hidetrinkets()",
        "endtextfast",
        "delay(10)",

        //add map mode here and wrap up...
        "gamemode(teleporter)",
        "delay(20)",

        "squeak(blue)",
        "text(blue,50,15,2)",
        "...and I was able to find more",
        "of them with the ship's scanner!",
        "speak_active",

        "endtext",

        "squeak(terminal)",
        "showtrinkets()",
        "delay(10)",
        "hidetrinkets()",
        "delay(10)",
        "showtrinkets()",
        "delay(10)",
        "hidetrinkets()",
        "delay(10)",
        "showtrinkets()",
        "delay(75)",

        "gamemode(game)",
        "delay(20)",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,3)",
        "If you get a chance, it",
        "might be worth finding",
        "the rest of them!",
        "position(blue,below)",
        "speak_active",

        "squeak(cry)",
        "changetile(blue,150)", //upside down frown :(
        "text(blue,0,0,2)",
        "Don't put yourself in",
        "any danger, though!",
        "position(blue,below)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(blue)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkblue_trinket5") == 0)
    {
        static const char* lines[] = {
        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,3)",
        "...but it looks like you've",
        "already found all of them",
        "in this dimension!",
        "position(blue,below)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Oh? Really?",
        "position(player,above)",
        "speak_active",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,2)",
        "Yeah, well done! That",
        "can't have been easy!",
        "position(blue,below)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(blue)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkblue_trinket6") == 0)
    {
        static const char* lines[] = {
        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,3)",
        "...and they're related.",
        "They're all a part of",
        "something bigger!",
        "position(blue,below)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Oh? Really?",
        "position(player,above)",
        "speak_active",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,4)",
        "Yeah! There seem to be",
        "twenty variations of",
        "the fundamental energy",
        "signature...",
        "position(blue,below)",
        "speak_active",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,1)",
        "Wait...",
        "position(blue,below)",
        "speak_active",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,2)",
        "Does that mean you've",
        "found all of them?",
        "position(blue,below)",
        "speak_active",

        "endtext",

        "loadscript(startepilogue)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkyellow_trinket1") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,yellow)",
        "face(yellow,player)",

        "squeak(yellow)",
        "text(yellow,0,0,2)",
        "Captain! I've been meaning",
        "to give this to you...",
        "position(yellow,above)",
        "speak_active",
        "endtext",

        "delay(30)",

        //found a trinket!
        "foundtrinket(18)",
        "endtext",
        "musicfadein",

        "delay(30)",

        "squeak(player)",
        "text(player,0,0,1)",
        "Professor! Where did you find this?",
        "position(player,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,2)",
        "Oh, it was just lying",
        "around that space station.",
        "position(yellow,above)",
        "speak_active",

        "squeak(cry)",
        "changemood(yellow,1)",
        "text(yellow,0,0,3)",
        "It's a pity Doctor Victoria",
        "isn't here, she loves studying",
        "that sort of thing...",
        "position(yellow,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Any idea what it does?",
        "position(player,above)",
        "speak_active",

        "squeak(yellow)",
        "changemood(yellow,0)",
        "text(yellow,0,0,2)",
        "Nope! But it is giving off",
        "a strange energy reading...",
        "position(yellow,above)",
        "speak_active",
        "endtext",

        "trinketyellowcontrol()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkyellow_trinket2") == 0)
    {
        static const char* lines[] = {
        "hidetrinkets()",
        "endtextfast",
        "delay(10)",

        //add map mode here and wrap up...
        "gamemode(teleporter)",
        "delay(20)",

        "squeak(yellow)",
        "text(yellow,50,15,2)",
        "...so I used the ship's scanner",
        "to find more of them!",
        "speak_active",

        "endtext",

        "squeak(terminal)",
        "showtrinkets()",
        "delay(10)",
        "hidetrinkets()",
        "delay(10)",
        "showtrinkets()",
        "delay(10)",
        "hidetrinkets()",
        "delay(10)",
        "showtrinkets()",
        "delay(75)",

        "gamemode(game)",
        "delay(20)",

        "squeak(yellow)",
        "changemood(yellow,0)",
        "text(yellow,0,0,3)",
        "...Please don't let them",
        "distract you from finding",
        "Victoria, though!",
        "position(yellow,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,1)",
        "I hope she's ok...",
        "position(yellow,above)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(yellow)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "talkyellow_trinket3") == 0)
    {
        static const char* lines[] = {
        "squeak(yellow)",
        "changemood(yellow,0)",
        "text(yellow,0,0,2)",
        "Can't seem to detect any",
        "more of them nearby, though.",
        "position(yellow,above)",
        "speak_active",

        "squeak(yellow)",
        "changemood(yellow,0)",
        "text(yellow,0,0,1)",
        "Maybe you've found them all?",
        "position(yellow,above)",
        "speak_active",
        "endtext",

        "endcutscene()",
        "untilbars()",
        "createactivityzone(yellow)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "gamecomplete") == 0)
    {
        static const char* lines[] = {
        "gotoroom(2,11)",
        "gotoposition(160,120,0)",
        "nocontrol()",
        "createcrewman(185,153,purple,0,faceleft)",
        "createcrewman(205,153,yellow,0,faceleft)",
        "createcrewman(225,153,red,0,faceleft)",
        "createcrewman(245,153,green,0,faceleft)",
        "createcrewman(265,153,blue,1,faceleft)",

        "cutscene()",
        "untilbars()",

        "delay(30)",
        "rescued(player)",

        "squeak(yellow)",
        "text(yellow,0,0,1)",
        "Any moment now...",
        "position(yellow,above)",
        "speak_active",
        "endtext",
        "nocontrol()",

        "delay(60)",

        "gamestate(4080)",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "gamecomplete_ending") == 0)
    {
        const char* lines[] = {
        "delay(15)",
        "changemood(blue,0)",
        "play(10)",
        "delay(45)",

        "squeak(player)",
        "text(player,0,0,1)",
        "Hello!",
        "position(player,above)",
        "speak_active",
        "endtext",

        "squeak(purple)",
        "delay(1)",
        "squeak(yellow)",
        "delay(1)",
        "squeak(red)",
        "delay(1)",
        "squeak(green)",

        "text(purple,0,0,1)",
        "Captain!        ",
        "position(purple,above)",
        "backgroundtext",
        "speak",
        "text(yellow,0,0,1)",
        "Captain!      ",
        "position(yellow,above)",
        "backgroundtext",
        "speak",
        "text(red,0,0,1)",
        "Captain!    ",
        "position(red,above)",
        "backgroundtext",
        "speak",
        "text(green,0,0,1)",
        "Captain!  ",
        "position(green,above)",
        "backgroundtext",
        "speak",
        "text(blue,0,0,1)",
        "Captain!",
        "position(blue,above)",
        "speak",

        "endtextfast",

        "squeak(blue)",
        "text(blue,0,0,1)",
        "You're alright!",
        "position(blue,above)",
        "speak_active",

        "squeak(blue)",
        "text(blue,0,0,1)",
        "I knew you'd be ok!",
        "position(blue,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,2)",
        "We were very worried when",
        "you didn't come back...",
        "position(purple,above)",
        "speak_active",

        "squeak(green)",
        "text(green,0,0,3)",
        "...but when you turned",
        "off the source of",
        "the interference...",
        "position(green,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,3)",
        "...we were able to",
        "find you with the",
        "ship's scanners...",
        "position(yellow,above)",
        "speak_active",

        "squeak(red)",
        "text(red,0,0,2)",
        "...and teleport you",
        "back on board!",
        "position(red,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "That was lucky!",
        "Thanks guys!",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Thanks guys!",
        "position(player,above)",
        "speak_active",

        "endtext",

        //Move to Vitellary's lab
        "fadeout()",
        "untilfade()",

        "missing(purple)",
        "missing(red)",
        "missing(green)",
        "missing(blue)",
        "missing(yellow)",

        "gotoroom(3,11)",
        "gotoposition(117,105,0)",
        "changedir(player,0)",
        "createcrewman(75,105,yellow,0,faceright)",
        "createcrewman(190,105,red,0,faceleft)",

        "fadein()",
        "untilfade()",

        "squeak(yellow)",
        "text(yellow,0,0,4)",
        "...it looks like this",
        "dimension is starting",
        "to destabilise, just",
        "like our own...",
        "position(yellow,above)",
        "speak_active",

        "walk(right,3)",

        "squeak(red)",
        "text(red,0,0,3)",
        "...we can stay and",
        "explore for a little",
        "longer, but...",
        "position(red,above)",
        "speak_active",

        "walk(left,3)",

        "squeak(yellow)",
        "text(yellow,0,0,2)",
        "...eventually, it'll",
        "collapse completely.",
        "position(yellow,above)",
        "speak_active",

        "endtext",

        //Move to Vertigris' lab
        "fadeout()",
        "untilfade()",

        "gotoroom(3,10)",
        "gotoposition(210,177,0)",
        "changedir(player,1)",
        "createcrewman(245,177,green,0,faceleft)",
        "createcrewman(56,177,blue,0,faceright)",

        "fadein()",
        "untilfade()",

        "squeak(green)",
        "text(green,0,0,3)",
        "There's no telling exactly",
        "how long we have here. But",
        "the ship's fixed, so...",
        "position(green,above)",
        "speak_active",

        "walk(left,3)",

        "squeak(blue)",
        "text(blue,0,0,2)",
        "...as soon as we're",
        "ready, we can go home!",
        "position(blue,above)",
        "speak_active",

        "endtext",

        //Move to the bridge!
        "fadeout()",
        "untilfade()",

        "gotoroom(4,10)",
        "gotoposition(227,113,0)",
        "changedir(player,0)",
        "createcrewman(140,177,purple,0,faceright)",
        "createcrewman(115,177,yellow,0,faceright)",
        "createcrewman(90,177,red,0,faceright)",
        "createcrewman(65,177,green,0,faceright)",
        "createcrewman(40,177,blue,0,faceright)",

        "rescued(purple)",
        "rescued(red)",
        "rescued(green)",
        "rescued(blue)",
        "rescued(yellow)",

        "fadein()",
        "untilfade()",

        "squeak(purple)",
        "text(purple,0,0,1)",
        "What now, Captain?",
        "position(purple,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,2)",
        "Let's find a way to save",
        "this dimension!",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,2)",
        "And a way to save our",
        "home dimension too!",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "The answer is out there, somewhere!",
        "position(player,above)",
        "speak_active",
        "endtext",

        "delay(30)",

        "squeak(player)",
        "text(player,0,0,1)",
        "Let's go!",
        "position(player,above)",
        "speak_active",

        "endtext",

        "fadeout()",
        "untilfade()",
        "rollcredits()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "startepilogue") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",
        "face(player,blue)",
        "face(blue,player)",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,1)",
        "Wow! You found all of them!",
        "position(blue,below)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Really? Great!",
        "position(player,above)",
        "speak_active",

        "squeak(blue)",
        "changetile(blue,6)", //smiling again! blue always needs to specify her mood
        "text(blue,0,0,3)",
        "I'll run some tests and",
        "see if I can work out",
        "what they're for...",
        "position(blue,below)",
        "speak_active",

        "endtext",

        "flash(5)",
        "shake(20)",
        "playef(9)",

        "musicfadeout()",

        "delay(30)",

        "squeak(cry)",
        "changemood(player,1)",
        "changetile(blue,150)", //upside down frown :(
        "text(player,0,0,2)",
        "That... that didn't",
        "sound good...",
        "position(player,above)",
        "speak_active",

        "endtext",

        "delay(30)",

        "flash(5)",
        "shake(20)",
        "playef(9)",
        "alarmon",

        "delay(30)",

        "squeak(cry)",
        "text(blue,0,0,1)",
        "Run!",
        "position(blue,below)",
        "speak_active",
        "endtext",

        "delay(5)",

        "missing(green)",
        "missing(yellow)",

        "flash(5)",
        "shake(50)",
        "playef(9)",
        "gotoroom(3,10)",
        "gotoposition(40,177,0)",
        "createcrewman(208,177,green,1,followposition,120)",
        "createcrewman(240,177,purple,1,followposition,120)",
        "createcrewman(10,177,blue,1,followposition,180)",

        "squeak(player)",
        "text(player,80,150,1)",
        "Oh no!",
        "backgroundtext",
        "speak_active",
        "walk(right,20)",

        "endtextfast",

        //and the next!
        "flash(5)",
        "shake(50)",
        "playef(9)",
        "gotoroom(3,11)",
        "gotoposition(140,0,0)",

        "createcrewman(90,105,green,1,followblue)",
        "createcrewman(125,105,purple,1,followgreen)",
        "createcrewman(55,105,blue,1,followposition,-200)",

        "createcrewman(120,177,yellow,1,followposition,-200)",
        "createcrewman(240,177,red,1,faceleft)",

        "delay(5)",
        "changeai(red,followposition,-200)",

        "squeak(red)",
        "text(red,100,150,1)",
        "Not again!",
        "backgroundtext",
        "speak_active",

        "walk(left,25)",

        "endtextfast",

        //final room:
        "flash(5)",
        "alarmoff",
        "playef(9)",
        "gotoroom(2,11)",
        "gotoposition(265,153,0)",

        "createcrewman(130,153,blue,1,faceleft)",
        "createcrewman(155,153,green,1,faceleft)",
        "createcrewman(180,153,purple,1,faceleft)",
        "createcrewman(205,153,yellow,1,faceleft)",
        "createcrewman(230,153,red,1,faceleft)",

        "delay(75)",

        "squeak(player)",
        "changemood(player,0)",
        "text(player,0,0,1)",
        "Wait! It's stopped!",
        "position(player,above)",
        "speak_active",
        "endtext",

        "delay(30)",
        "changemood(purple,0)",
        "changedir(purple,1)",
        "changemood(red,0)",
        "changedir(red,1)",
        "changemood(green,0)",
        "changedir(green,1)",
        "changemood(blue,0)",
        "changedir(blue,1)",
        "changemood(yellow,0)",
        "changedir(yellow,1)",
        "delay(30)",

        "rescued(green)",
        "rescued(yellow)",
        "missing(blue)",
        "altstate(-1,-1,1)",

        "fadeout()",
        "untilfade()",

        "gotoroom(2,10)",
        "gotoposition(227,113,0)",
        "changedir(player,0)",

        "rescued(blue)",

        "createcrewman(150,177,purple,0,faceleft)",
        "createcrewman(90,177,yellow,0,faceright)",
        "createcrewman(184,185,red,0,faceleft)",
        "createcrewman(65,177,green,0,faceright)",
        "createcrewman(35,177,blue,0,faceright)",

        "rescued(purple)",
        "rescued(red)",
        "rescued(green)",
        "rescued(yellow)",

        "fadein()",
        "untilfade()",

        "delay(30)",

        "squeak(purple)",
        "text(purple,0,0,3)",
        "This is where we were",
        "storing those shiny",
        "things? What happened?",
        "position(purple,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,2)",
        "We were just playing",
        "with them, and...",
        "position(player,above)",
        "speak_active",
        "endtext",

        "squeak(cry)",
        "changemood(player,1)",
        "text(player,0,0,1)",
        "...they suddenly exploded!",
        "position(player,above)",
        "speak_active",
        "endtext",

        "squeak(blue)",
        "text(blue,0,0,2)",
        "But look what they made!",
        "Is that a teleporter?",
        "position(blue,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,1)",
        "I think so, but...",
        "position(yellow,above)",
        "speak_active",

        "squeak(yellow)",
        "text(yellow,0,0,2)",
        "I've never seen a teleporter",
        "like that before...",
        "position(yellow,above)",
        "speak_active",
        "endtext",

        "changemood(player,0)",

        "delay(30)",

        "squeak(red)",
        "text(red,0,0,1)",
        "We should investigate!",
        "position(red,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,1)",
        "What do you think, Captain?",
        "position(purple,above)",
        "speak_active",

        "squeak(purple)",
        "text(purple,0,0,2)",
        "Should we find out",
        "where it leads?",
        "position(purple,above)",
        "speak_active",
        "endtext",

        "delay(15)",

        "squeak(player)",
        "text(player,0,0,1)",
        "Let's go!",
        "position(player,above)",
        "speak_active",
        "endtext",

        "walk(left,10)",
        "flip",
        "walk(left,5)",

        "flash(5)",
        "shake(20)",
        "playef(10)",
        "blackout()",

        "delay(45)",

        "gotoroom(17,6)",
        "gotoposition(80,109,1)",
        "changedir(player,1)",
        "flash(5)",
        "shake(20)",
        "playef(10)",
        "blackon()",

        "delay(15)",
        "flash(5)",
        "shake(20)",
        "playef(10)",
        "createcrewman(28,65,purple,0,faceright)",

        "delay(15)",
        "flash(5)",
        "shake(20)",
        "playef(10)",
        "createcrewman(145,169,yellow,0,faceleft)",

        "delay(15)",
        "flash(5)",
        "shake(20)",
        "playef(10)",
        "createcrewman(32,169,red,0,faceright)",

        "delay(15)",
        "flash(5)",
        "shake(20)",
        "playef(10)",
        "createcrewman(96,149,green,0,faceleft)",

        "delay(15)",
        "flash(5)",
        "shake(20)",
        "playef(10)",
        "createcrewman(155,57,blue,0,faceleft)",

        "delay(45)",


        "squeak(cry)",
        "changemood(blue,1)",
        "text(blue,0,0,1)",
        "Oh no! We're trapped!",
        "position(blue,above)",
        "speak_active",

        "squeak(cry)",
        "changemood(yellow,1)",
        "text(yellow,0,0,1)",
        "Oh dear...",
        "position(yellow,above)",
        "speak_active",

        "squeak(cry)",
        "changemood(red,1)",
        "changemood(green,1)",
        "changemood(purple,1)",
        "changemood(player,1)",
        "text(player,0,0,2)",
        "Hmm... how should we",
        "get out of this?",
        "position(player,below)",
        "speak_active",

        "endtext",

        "delay(70)",

        "squeak(purple)",
        "delay(1)",
        "squeak(yellow)",
        "delay(1)",
        "squeak(red)",
        "delay(1)",
        "squeak(blue)",
        "delay(1)",
        "squeak(player)",
        "delay(1)",
        "squeak(green)",

        "changemood(yellow,0)",
        "changemood(blue,0)",
        "changemood(red,0)",
        "changemood(player,0)",
        "changemood(green,0)",
        "changemood(purple,0)",
        "text(player,0,0,1)",
        "COMBINE!",
        "position(player,above)",
        "backgroundtext",
        "speak",
        "text(purple,0,0,1)",
        "COMBINE!",
        "position(purple,above)",
        "backgroundtext",
        "speak",
        "text(yellow,0,0,1)",
        "COMBINE!",
        "position(yellow,above)",
        "backgroundtext",
        "speak",
        "text(red,0,0,1)",
        "COMBINE!",
        "position(red,above)",
        "backgroundtext",
        "speak",
        "text(green,0,0,1)",
        "COMBINE!",
        "position(green,above)",
        "backgroundtext",
        "speak",
        "text(blue,0,0,1)",
        "COMBINE!",
        "position(blue,above)",
        "speak",

        "endtextfast",

        "delay(15)",
        "flip",
        "changeai(purple,followplayer)",
        "changeai(blue,followplayer)",
        "changeai(red,followplayer)",
        "changeai(yellow,followplayer)",
        "changeai(green,followplayer)",
        "walk(right,3)",

        "delay(5)",
        "flash(10)",
        "shake(20)",
        "playef(24)",
        "gotoroom(17,6)",
        "vvvvvvman()",


        "delay(90)",


        "walk(right,6)",
        "flash(10)",
        "shake(20)",
        "playef(23)",
        "altstate(-1,-1,2)",
        "gotoroom(17,6)",

        "delay(20)",

        "walk(right,12)",
        "flash(10)",
        "shake(20)",
        "playef(23)",
        "altstate(-1,-1,0)",
        "gotoroom(17,6)",

        "delay(20)",

        "walk(right,15)",

        "gotoroom(18,6)",
        "gotoposition(0,46,0)",
        "walk(right,5)",

        "delay(20)",
        "flash(10)",
        "shake(20)",
        "playef(24)",
        "undovvvvvvman()",
        "createcrewman(30,99,purple,0,faceright)",
        "createcrewman(65,119,yellow,0,faceright)",
        "createcrewman(135,149,red,0,faceleft)",
        "createcrewman(170,159,green,0,faceleft)",
        "createcrewman(205,159,blue,0,faceleft)",


        "delay(60)",


        "changedir(yellow,0)",
        "changedir(player,0)",
        "delay(20)",
        "squeak(purple)",
        "text(purple,0,0,3)",
        "Or, you know... we could",
        "have just warped back",
        "to the ship...",
        "position(purple,above)",
        "speak_active",

        "endtext",

        "delay(30)",

        "changedir(purple,1)",
        "changedir(yellow,1)",
        "changedir(player,1)",
        "changedir(red,1)",

        "changedir(green,1)",
        "squeak(green)",
        "text(green,0,0,1)",
        "Wow! What is this?",
        "position(green,above)",
        "speak_active",


        "changedir(purple,1)",
        "changedir(yellow,1)",
        "changedir(player,0)",
        "changedir(red,0)",
        "changedir(green,0)",
        "squeak(yellow)",
        "text(yellow,0,0,1)",
        "It looks like another laboratory!",
        "position(yellow,above)",
        "speak_active",

        "changedir(purple,1)",
        "changedir(yellow,1)",
        "changedir(player,1)",
        "squeak(red)",
        "text(red,0,0,1)",
        "Let's have a look around!",
        "position(red,above)",
        "speak_active",

        "endtext",

        "delay(20)",
        "changeai(yellow,followposition,500)",
        "changeai(purple,followposition,500)",
        "changeai(blue,followposition,500)",
        "changeai(red,followposition,500)",
        "changeai(green,followposition,500)",

        "delay(21)",
        "changeai(yellow,faceright)",
        "flipgravity(yellow)",
        "playef(0)",
        "delay(2)",
        "changeai(purple,faceright)",
        "flipgravity(purple)",
        "playef(0)",

        "delay(48)",

        "foundlab",
        "endtext",
        "foundlab2",
        "endtext",

        "entersecretlab",
        "play(11)",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "returntolab") == 0)
    {
        //To get back to the lab from the gravitron
        static const char* lines[] = {
        "gotoroom(19,7)",
        "gotoposition(132,137,0)",
        "fadein()",

        "setcheckpoint()",

        "play(11)",
        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (SDL_strcmp(t, "disableaccessibility") == 0)
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(terminal)",
        "text(gray,0,114,3)",
        "Please disable invincibility",
        "and/or slowdown before entering",
        "the Super Gravitron.",
        "position(center)",
        "speak",

        "endtext",
        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else
    {
        loadother(t);
    }

}
