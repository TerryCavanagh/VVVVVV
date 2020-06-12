#include "Script.h"

void scriptclass::loadother(std::string t)
{
    //loads script name t into the array
    if (t == "terminal_station_1")
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(terminal)",
        "text(gray,0,60,1)",
        "        -= PERSONAL LOG =-         ",
        "position(centerx)",
        "backgroundtext",
        "flipme",
        "speak",

        "text(gray,0,120,6)",
        " Almost everyone has been    ",
        " evacuated from the space    ",
        " station now. The rest of us ",
        " are leaving in a couple of  ",
        " days, once our research has ",
        " been completed.             ",
        "position(center)",
        "speak",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "alreadyvisited")
    {
        static const char* lines[] = {
        "squeak(player)",
        "text(cyan,0,0,1)",
        "...oh, I've already found this.",
        "position(player,above)",
        "speak_active",
        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_outside_1")
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(terminal)",
        "text(gray,0,60,1)",
        "      -= Research Notes =-       ",
        "position(centerx)",
        "backgroundtext",
        "flipme",
        "speak",

        "text(gray,0,114,5)",
        " ... our first breakthrough was ",
        " the creation of the inversion  ",
        " plane, which creates a         ",
        " mirrored dimension beyond a   ",
        " given event horizon ...       ",
        "position(center)",
        "speak",

        "endtextfast",
        "delay(10)",

        "ifexplored(2,16,alreadyvisited)",

        "gamemode(teleporter)",
        "delay(20)",

        "squeak(terminal)",
        "showcoordinates(2,16)",
        "delay(10)",
        "hidecoordinates(2,16)",
        "delay(10)",
        "showcoordinates(2,16)",
        "delay(10)",
        "hidecoordinates(2,16)",
        "delay(10)",
        "showcoordinates(2,16)",
        "delay(45)",

        "gamemode(game)",
        "delay(20)",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_outside_2")
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(terminal)",
        "text(gray,0,60,1)",
        "      -= Research Notes =-       ",
        "position(centerx)",
        "backgroundtext",
        "flipme",
        "speak",

        "text(gray,0,114,3)",
        "...with just a small modification to",
        "the usual parameters, we were able  ",
        "to stabilise an infinite tunnel!    ",
        "position(center)",
        "speak",

        "endtextfast",
        "delay(10)",

        "ifexplored(8,9,alreadyvisited)",

        "gamemode(teleporter)",
        "delay(20)",

        "squeak(terminal)",
        "showcoordinates(8,9)",
        "delay(10)",
        "hidecoordinates(8,9)",
        "delay(10)",
        "showcoordinates(8,9)",
        "delay(10)",
        "hidecoordinates(8,9)",
        "delay(10)",
        "showcoordinates(8,9)",
        "delay(45)",

        "gamemode(game)",
        "delay(20)",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_outside_3")
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(terminal)",
        "text(gray,0,60,1)",
        "      -= Research Notes =-       ",
        "position(centerx)",
        "backgroundtext",
        "flipme",
        "speak",

        "text(gray,0,114,3)",
        " ... the final step in creating ",
        " the dimensional stabiliser was ",
        " to create a feedback loop ... ",
        "position(center)",
        "speak",

        "endtextfast",
        "delay(10)",

        "ifexplored(14,1,alreadyvisited)",

        "gamemode(teleporter)",
        "delay(20)",

        "squeak(terminal)",
        "showcoordinates(14,1)",
        "delay(10)",
        "hidecoordinates(14,1)",
        "delay(10)",
        "showcoordinates(14,1)",
        "delay(10)",
        "hidecoordinates(14,1)",
        "delay(10)",
        "showcoordinates(14,1)",
        "delay(45)",

        "gamemode(game)",
        "delay(20)",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_outside_4")
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(terminal)",
        "text(gray,0,60,1)",
        "      -= Research Notes =-       ",
        "position(centerx)",
        "backgroundtext",
        "flipme",
        "speak",

        "text(gray,0,114,4)",
        " ...despite our best efforts, ",
        " the dimensional stabiliser   ",
        " won't hold out forever. Its  ",
        " collapse is inevitable...    ",
        "position(center)",
        "speak",

        "endtextfast",
        "delay(10)",

        "squeak(player)",
        "text(cyan,0,0,2)",
        "Huh? These coordinates aren't",
        "even in this dimension!",
        "position(player,above)",
        "speak_active",
        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_outside_5")
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(terminal)",
        "text(gray,0,60,1)",
        "     -= Personal Log =-      ",
        "position(centerx)",
        "backgroundtext",
        "flipme",
        "speak",

        "text(gray,0,114,5)",
        " ... I've had to seal off ",
        "   access to most of our ",
        " research. Who knows what ",
        " could happen if it fell  ",
        " into the wrong hands? ...  ",
        "position(center)",
        "speak",

        "endtextfast",
        "delay(10)",

        "gamemode(teleporter)",
        "delay(20)",

        "squeak(terminal)",
        "showsecretlab()",
        "delay(10)",
        "hidesecretlab()",
        "delay(10)",
        "showsecretlab()",
        "delay(10)",
        "hidesecretlab()",
        "delay(10)",
        "showsecretlab()",
        "delay(45)",

        "gamemode(game)",
        "delay(20)",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_outside_6")
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(terminal)",
        "text(gray,0,60,1)",
        "      -= Research Notes =-       ",
        "position(centerx)",
        "backgroundtext",
        "flipme",
        "speak",

        "text(gray,0,114,3)",
        "... access to the control center",
        "is still possible through the   ",
        "main atmospheric filters ...    ",
        "position(center)",
        "speak",

        "endtextfast",
        "delay(10)",

        "ifexplored(12,14,alreadyvisited)",

        "gamemode(teleporter)",
        "delay(20)",

        "squeak(terminal)",
        "showcoordinates(12,14)",
        "delay(10)",
        "hidecoordinates(12,14)",
        "delay(10)",
        "showcoordinates(12,14)",
        "delay(10)",
        "hidecoordinates(12,14)",
        "delay(10)",
        "showcoordinates(12,14)",
        "delay(45)",

        "gamemode(game)",
        "delay(20)",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_finallevel")
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(terminal)",
        "text(gray,0,114,9)",
        "* DIMENSIONAL STABILITY GENERATOR *",
        "",
        "     [ Currently Generating ]      ",
        "        Maximum Stability    ",
        "",
        "            [ Status ]",
        "              Online",
        "",
        "READY _",
        "position(center)",
        "speak_active",

        "endtextfast",
        "delay(10)",

        "squeak(player)",
        "text(cyan,0,0,2)",
        "Aha! This must be what's" ,
        "causing the interference!",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(cyan,0,0,1)",
        "I wonder if I can turn it off?",
        "position(player,above)",
        "speak_active",

        "squeak(terminal)",
        "text(gray,0,114,4)",
        "WARNING: Disabling the Dimensional",
        "Stability Generator may lead to",
        "instability! Are you sure you want",
        "to do this?",
        "position(center)",
        "speak_active",

        "squeak(player)",
        "text(cyan,0,0,1)",
        "Yes!",
        "position(player,above)",
        "speak_active",

        "squeak(terminal)",
        "text(gray,0,114,6)",
        "Seriously! The whole dimension",
        "could collapse! Just think about",
        "this for a minute!",
        "",
        "Are you really sure you want",
        "to do this?",
        "position(center)",
        "speak_active",

        "squeak(player)",
        "text(cyan,0,0,1)",
        "Yes!",
        "position(player,above)",
        "speak_active",

        "endtext",

        "stopmusic",
        "gamestate(200)",
        };
        filllines(lines);
    }
    else if (t == "finalterminal_finish")
    {


        static const char* lines[] = {
        //"delay(15)",	"flash(5)", "shake(20)", "playef(9)",

        "text(gray,0,114,3)",
        "          -= WARNING =-         ",
        "",
        " DIMENSIONAL STABILISER OFFLINE ",
        "position(center)",
        "speak_active",
        "endtext",

        "squeak(cry)",
        "changemood(player,1)",
        "text(cyan,0,0,1)",
        "Uh oh...",
        "position(player,above)",
        "speak_active",
        "endtext",

        "play(2)",
        "changemood(player,0)",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_station_2")
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(terminal)",
        "text(gray,0,60,1)",
        "      -= Research Notes =-       ",
        "position(centerx)",
        "backgroundtext",
        "flipme",
        "speak",

        "text(gray,0,114,3)",
        " ...everything collapses,  ",
        " eventually. It's the way  ",
        "    of the universe.       ",
        "position(center)",
        "speak",

        "endtextfast",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_station_3")
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(terminal)",
        "text(gray,0,114,3)",
        "I wonder if the generator we set",
        "up in the polar dimension is",
        "what's affecting our teleporters?",
        "position(center)",
        "speak_active",

        "squeak(terminal)",
        "text(gray,0,114,1)",
        "No, it's probably just a glitch.",
        "position(center)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_station_4")
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(terminal)",
        "text(gray,0,60,1)",
        "        -= PERSONAL LOG =-         ",
        "position(centerx)",
        "backgroundtext",
        "flipme",
        "speak",

        "text(gray,0,114,2)",
        " Hah! Nobody will ever ",
        " get this one. ",
        "position(center)",
        "speak",

        "endtextfast",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_warp_1")
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(terminal)",
        "text(gray,0,114,3)",
        "...The other day I was chased",
        "down a hallway by a giant cube",
        "with the word AVOID on it.",
        "position(center)",
        "speak_active",

        "squeak(terminal)",
        "text(gray,0,114,1)",
        "These security measures go too far!",
        "position(center)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_warp_2")
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(terminal)",
        "text(gray,0,114,2)",
        "The only way into my private lab",
        "anymore is by teleporter.",
        "position(center)",
        "speak_active",

        "squeak(terminal)",
        "text(gray,0,114,3)",
        "I've made sure that it's",
        "difficult for unauthorised",
        "personnel to gain access.",
        "position(center)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_lab_1")
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(terminal)",
        "text(gray,0,114,4)",
        "... it turns out the key to",
        "stabilising this dimension was",
        "to create a balancing force",
        "outside of it!",
        "position(center)",
        "speak_active",

        "squeak(terminal)",
        "text(gray,0,114,2)",
        "Though it looks like that's just",
        "a temporary solution, at best.",
        "position(center)",
        "speak_active",

        "squeak(terminal)",
        "text(gray,0,114,4)",
        "I've been working on",
        "something more permanent,",
        "but it seems it's going",
        "to be too late...",
        "position(center)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_lab_2")
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(terminal)",
        "text(gray,0,114,1)",
        "?SYNTAX  ERROR",
        "position(center)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_secretlab")
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(terminal)",
        "text(gray,0,114,4)",
        "         -= WARNING =-",
        "",
        " The Super-Gravitron is intended  ",
        " for entertainment purposes only. ",
        "position(center)",
        "speak_active",

        "squeak(terminal)",
        "text(gray,0,114,4)",
        "Anyone found using the Super",
        "Gravitron for educational",
        "purposes may be asked to",
        "stand in the naughty corner.",
        "position(center)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_shipcomputer")
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(terminal)",
        "text(gray,0,114,3)",
        "   -= D.S.S. SOULEYE =-   ",
        "",
        " Ship Navigation Controls ",
        "position(center)",
        "speak_active",

        "ifflag(67,terminal_letsgo)",

        "squeak(terminal)",
        "text(gray,0,114,3)",
        "Error! Error! Cannot isolate",
        "dimensional coordinates!",
        "Interference detected!",
        "position(center)",
        "speak_active",
        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_letsgo")
    {
        static const char* lines[] = {
        "squeak(player)",
        "text(player,0,0,2)",
        "Now that the ship is fixed,",
        "we can leave anytime we want!",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,3)",
        "We've all agreed to",
        "keep exploring this",
        "dimension, though.",
        "position(player,above)",
        "speak_active",

        "squeak(player)",
        "text(player,0,0,1)",
        "Who knows what we'll find?",
        "position(player,above)",
        "speak_active",
        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_radio")
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(terminal)",
        "text(gray,0,114,4)",
        "  -= SHIP RADIO =-  ",
        "",
        "     [ Status ]",
        "    Broadcasting",
        "position(center)",
        "speak_active",

        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_jukebox")
    {
        static const char* lines[] = {
        "cutscene()",
        "untilbars()",

        "squeak(terminal)",
        "text(gray,0,114,7)",
        "          -= JUKEBOX =-       ",
        "",
        "   Songs will continue to play   ",
        "   until you leave the ship.",
        "",
        "      Collect trinkets to",
        "       unlock new songs!",
        "position(center)",
        "speak_active",

        "endtext",

        "iftrinketsless(5,terminal_jukeunlock1)",
        "iftrinketsless(8,terminal_jukeunlock2)",
        "iftrinketsless(10,terminal_jukeunlock3)",
        "iftrinketsless(12,terminal_jukeunlock4)",
        "iftrinketsless(14,terminal_jukeunlock41)",
        "iftrinketsless(16,terminal_jukeunlock5)",
        "iftrinketsless(18,terminal_jukeunlock6)",
        "iftrinketsless(20,terminal_jukeunlock7)",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_jukeunlock1")
    {
        static const char* lines[] = {
        "squeak(terminal)",
        "text(gray,0,114,4)",
        " NEXT UNLOCK:    ",
        " 5 Trinkets",
        "",
        " Pushing Onwards ",
        "position(center)",
        "speak_active",
        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_jukeunlock2")
    {
        static const char* lines[] = {
        "squeak(terminal)",
        "text(gray,0,114,4)",
        " NEXT UNLOCK:    ",
        " 8 Trinkets",
        "",
        " Positive Force ",
        "position(center)",
        "speak_active",
        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_jukeunlock3")
    {
        static const char* lines[] = {
        "squeak(terminal)",
        "text(gray,0,114,4)",
        " NEXT UNLOCK:    ",
        " 10 Trinkets",
        "",
        " Presenting VVVVVV ",
        "position(center)",
        "speak_active",
        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_jukeunlock4")
    {
        static const char* lines[] = {
        "squeak(terminal)",
        "text(gray,0,114,4)",
        " NEXT UNLOCK:    ",
        " 12 Trinkets",
        "",
        " Potential for Anything ",
        "position(center)",
        "speak_active",
        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_jukeunlock41")
    {
        static const char* lines[] = {
        "squeak(terminal)",
        "text(gray,0,114,4)",
        " NEXT UNLOCK:    ",
        " 14 Trinkets",
        "",
        " Pressure Cooker ",
        "position(center)",
        "speak_active",
        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_jukeunlock5")
    {
        static const char* lines[] = {
        "squeak(terminal)",
        "text(gray,0,114,4)",
        " NEXT UNLOCK:    ",
        " 16 Trinkets",
        "",
        " Predestined Fate ",
        "position(center)",
        "speak_active",
        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_jukeunlock6")
    {
        static const char* lines[] = {
        "squeak(terminal)",
        "text(gray,0,114,4)",
        " NEXT UNLOCK:    ",
        " 18 Trinkets",
        "",
        " Popular Potpourri ",
        "position(center)",
        "speak_active",
        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_jukeunlock7")
    {
        static const char* lines[] = {
        "squeak(terminal)",
        "text(gray,0,114,4)",
        " NEXT UNLOCK:    ",
        " 20 Trinkets",
        "",
        " Pipe Dream ",
        "position(center)",
        "speak_active",
        "endtext",

        "endcutscene()",
        "untilbars()",
        };
        filllines(lines);
    }
    else if (t == "terminal_juke1")
    {
        static const char* lines[] = {
        "play(4)",
        "squeak(terminal)",
        "jukebox(1)",
        };
        filllines(lines);
    }
    else if (t == "terminal_juke2")
    {
        static const char* lines[] = {
        "play(1)",
        "squeak(terminal)",
        "jukebox(2)",
        };
        filllines(lines);
    }
    else if (t == "terminal_juke3")
    {
        static const char* lines[] = {
        "play(2)",
        "squeak(terminal)",
        "jukebox(3)",
        };
        filllines(lines);
    }
    else if (t == "terminal_juke4")
    {
        static const char* lines[] = {
        "play(6)",
        "squeak(terminal)",
        "jukebox(4)",
        };
        filllines(lines);
    }
    else if (t == "terminal_juke5")
    {
        static const char* lines[] = {
        "play(3)",
        "squeak(terminal)",
        "jukebox(5)",
        };
        filllines(lines);
    }
    else if (t == "terminal_juke6")
    {
        static const char* lines[] = {
        "play(8)",
        "squeak(terminal)",
        "jukebox(6)",
        };
        filllines(lines);
    }
    else if (t == "terminal_juke7")
    {
        static const char* lines[] = {
        "play(11)",
        "squeak(terminal)",
        "jukebox(7)",
        };
        filllines(lines);
    }
    else if (t == "terminal_juke8")
    {
        static const char* lines[] = {
        "play(10)",
        "squeak(terminal)",
        "jukebox(8)",
        };
        filllines(lines);
    }
    else if (t == "terminal_juke9")
    {
        static const char* lines[] = {
        "play(12)",
        "squeak(terminal)",
        "jukebox(9)",
        };
        filllines(lines);
    }
    else if (t == "terminal_juke10")
    {
        static const char* lines[] = {
        "play(9)",
        "squeak(terminal)",
        "jukebox(10)",
        };
        filllines(lines);
    }
}
