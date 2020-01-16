#ifndef TERMINALSCRIPTS_H
#define TERMINALSCRIPTS_H

#include "Script.h"

void scriptclass::loadother(std::string t)
{
    //loads script name t into the array
    if (t == "terminal_station_1")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(terminal)");
        add("text(gray,0,60,1)");
        add("        -= PERSONAL LOG =-         ");
        add("position(centerx)");
        add("backgroundtext");
        add("flipme");
        add("speak");

        add("text(gray,0,120,6)");
        add(" Almost everyone has been    ");
        add(" evacuated from the space    ");
        add(" station now. The rest of us ");
        add(" are leaving in a couple of  ");
        add(" days, once our research has ");
        add(" been completed.             ");
        add("position(center)");
        add("speak");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "alreadyvisited")
    {
        add("squeak(player)");
        add("text(cyan,0,0,1)");
        add("...oh, I've already found this.");
        add("position(player,above)");
        add("speak_active");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_outside_1")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(terminal)");
        add("text(gray,0,60,1)");
        add("      -= Research Notes =-       ");
        add("position(centerx)");
        add("backgroundtext");
        add("flipme");
        add("speak");

        add("text(gray,0,114,5)");
        add(" ... our first breakthrough was ");
        add(" the creation of the inversion  ");
        add(" plane, which creates a         ");
        add(" mirrored dimension beyond a   ");
        add(" given event horizon ...       ");
        add("position(center)");
        add("speak");

        add("endtextfast");
        add("delay(10)");

        add("ifexplored(2,16,alreadyvisited)");

        add("gamemode(teleporter)");
        add("delay(20)");

        add("squeak(terminal)");
        add("showcoordinates(2,16)");
        add("delay(10)");
        add("hidecoordinates(2,16)");
        add("delay(10)");
        add("showcoordinates(2,16)");
        add("delay(10)");
        add("hidecoordinates(2,16)");
        add("delay(10)");
        add("showcoordinates(2,16)");
        add("delay(45)");

        add("gamemode(game)");
        add("delay(20)");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_outside_2")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(terminal)");
        add("text(gray,0,60,1)");
        add("      -= Research Notes =-       ");
        add("position(centerx)");
        add("backgroundtext");
        add("flipme");
        add("speak");

        add("text(gray,0,114,3)");
        add("...with just a small modification to");
        add("the usual parameters, we were able  ");
        add("to stabilise an infinite tunnel!    ");
        add("position(center)");
        add("speak");

        add("endtextfast");
        add("delay(10)");

        add("ifexplored(8,9,alreadyvisited)");

        add("gamemode(teleporter)");
        add("delay(20)");

        add("squeak(terminal)");
        add("showcoordinates(8,9)");
        add("delay(10)");
        add("hidecoordinates(8,9)");
        add("delay(10)");
        add("showcoordinates(8,9)");
        add("delay(10)");
        add("hidecoordinates(8,9)");
        add("delay(10)");
        add("showcoordinates(8,9)");
        add("delay(45)");

        add("gamemode(game)");
        add("delay(20)");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_outside_3")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(terminal)");
        add("text(gray,0,60,1)");
        add("      -= Research Notes =-       ");
        add("position(centerx)");
        add("backgroundtext");
        add("flipme");
        add("speak");

        add("text(gray,0,114,3)");
        add(" ... the final step in creating ");
        add(" the dimensional stabiliser was ");
        add(" to create a feedback loop ... ");
        add("position(center)");
        add("speak");

        add("endtextfast");
        add("delay(10)");

        add("ifexplored(14,1,alreadyvisited)");

        add("gamemode(teleporter)");
        add("delay(20)");

        add("squeak(terminal)");
        add("showcoordinates(14,1)");
        add("delay(10)");
        add("hidecoordinates(14,1)");
        add("delay(10)");
        add("showcoordinates(14,1)");
        add("delay(10)");
        add("hidecoordinates(14,1)");
        add("delay(10)");
        add("showcoordinates(14,1)");
        add("delay(45)");

        add("gamemode(game)");
        add("delay(20)");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_outside_4")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(terminal)");
        add("text(gray,0,60,1)");
        add("      -= Research Notes =-       ");
        add("position(centerx)");
        add("backgroundtext");
        add("flipme");
        add("speak");

        add("text(gray,0,114,4)");
        add(" ...despite our best efforts, ");
        add(" the dimensional stabiliser   ");
        add(" won't hold out forever. Its  ");
        add(" collapse is inevitable...    ");
        add("position(center)");
        add("speak");

        add("endtextfast");
        add("delay(10)");

        add("squeak(player)");
        add("text(cyan,0,0,2)");
        add("Huh? These coordinates aren't");
        add("even in this dimension!");
        add("position(player,above)");
        add("speak_active");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_outside_5")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(terminal)");
        add("text(gray,0,60,1)");
        add("     -= Personal Log =-      ");
        add("position(centerx)");
        add("backgroundtext");
        add("flipme");
        add("speak");

        add("text(gray,0,114,5)");
        add(" ... I've had to seal off ");
        add("   access to most of our ");
        add(" research. Who knows what ");
        add(" could happen if it fell  ");
        add(" into the wrong hands? ...  ");
        add("position(center)");
        add("speak");

        add("endtextfast");
        add("delay(10)");

        add("gamemode(teleporter)");
        add("delay(20)");

        add("squeak(terminal)");
        add("showsecretlab()");
        add("delay(10)");
        add("hidesecretlab()");
        add("delay(10)");
        add("showsecretlab()");
        add("delay(10)");
        add("hidesecretlab()");
        add("delay(10)");
        add("showsecretlab()");
        add("delay(45)");

        add("gamemode(game)");
        add("delay(20)");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_outside_6")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(terminal)");
        add("text(gray,0,60,1)");
        add("      -= Research Notes =-       ");
        add("position(centerx)");
        add("backgroundtext");
        add("flipme");
        add("speak");

        add("text(gray,0,114,3)");
        add("... access to the control center");
        add("is still possible through the   ");
        add("main atmospheric filters ...    ");
        add("position(center)");
        add("speak");

        add("endtextfast");
        add("delay(10)");

        add("ifexplored(12,14,alreadyvisited)");

        add("gamemode(teleporter)");
        add("delay(20)");

        add("squeak(terminal)");
        add("showcoordinates(12,14)");
        add("delay(10)");
        add("hidecoordinates(12,14)");
        add("delay(10)");
        add("showcoordinates(12,14)");
        add("delay(10)");
        add("hidecoordinates(12,14)");
        add("delay(10)");
        add("showcoordinates(12,14)");
        add("delay(45)");

        add("gamemode(game)");
        add("delay(20)");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_finallevel")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(terminal)");
        add("text(gray,0,114,9)");
        add("* DIMENSIONAL STABILITY GENERATOR *");
        add("");
        add("     [ Currently Generating ]      ");
        add("        Maximum Stability    ");
        add("");
        add("            [ Status ]");
        add("              Online");
        add("");
        add("READY _");
        add("position(center)");
        add("speak_active");

        add("endtextfast");
        add("delay(10)");

        add("squeak(player)");
        add("text(cyan,0,0,2)");
        add("Aha! This must be what's" );
        add("causing the interference!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(cyan,0,0,1)");
        add("I wonder if I can turn it off?");
        add("position(player,above)");
        add("speak_active");

        add("squeak(terminal)");
        add("text(gray,0,114,4)");
        add("WARNING: Disabling the Dimensional");
        add("Stability Generator may lead to");
        add("instability! Are you sure you want");
        add("to do this?");
        add("position(center)");
        add("speak_active");

        add("squeak(player)");
        add("text(cyan,0,0,1)");
        add("Yes!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(terminal)");
        add("text(gray,0,114,6)");
        add("Seriously! The whole dimension");
        add("could collapse! Just think about");
        add("this for a minute!");
        add("");
        add("Are you really sure you want");
        add("to do this?");
        add("position(center)");
        add("speak_active");

        add("squeak(player)");
        add("text(cyan,0,0,1)");
        add("Yes!");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        add("stopmusic");
        add("gamestate(200)");
    }
    else if (t == "finalterminal_finish")
    {


        //add("delay(15)");	add("flash(5)"); add("shake(20)"); add("playef(9,10)");

        add("text(gray,0,114,3)");
        add("          -= WARNING =-         ");
        add("");
        add(" DIMENSIONAL STABILISER OFFLINE ");
        add("position(center)");
        add("speak_active");
        add("endtext");

        add("squeak(cry)");
        add("changemood(player,1)");
        add("text(cyan,0,0,1)");
        add("Uh oh...");
        add("position(player,above)");
        add("speak_active");
        add("endtext");

        add("play(2)");
        add("changemood(player,0)");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_station_2")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(terminal)");
        add("text(gray,0,60,1)");
        add("      -= Research Notes =-       ");
        add("position(centerx)");
        add("backgroundtext");
        add("flipme");
        add("speak");

        add("text(gray,0,114,3)");
        add(" ...everything collapses,  ");
        add(" eventually. It's the way  ");
        add("    of the universe.       ");
        add("position(center)");
        add("speak");

        add("endtextfast");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_station_3")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(terminal)");
        add("text(gray,0,114,3)");
        add("I wonder if the generator we set");
        add("up in the polar dimension is");
        add("what's affecting our teleporters?");
        add("position(center)");
        add("speak_active");

        add("squeak(terminal)");
        add("text(gray,0,114,1)");
        add("No, it's probably just a glitch.");
        add("position(center)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_station_4")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(terminal)");
        add("text(gray,0,60,1)");
        add("        -= PERSONAL LOG =-         ");
        add("position(centerx)");
        add("backgroundtext");
        add("flipme");
        add("speak");

        add("text(gray,0,114,2)");
        add(" Hah! Nobody will ever ");
        add(" get this one. ");
        add("position(center)");
        add("speak");

        add("endtextfast");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_warp_1")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(terminal)");
        add("text(gray,0,114,3)");
        add("...The other day I was chased");
        add("down a hallway by a giant cube");
        add("with the word AVOID on it.");
        add("position(center)");
        add("speak_active");

        add("squeak(terminal)");
        add("text(gray,0,114,1)");
        add("These security measures go too far!");
        add("position(center)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_warp_2")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(terminal)");
        add("text(gray,0,114,2)");
        add("The only way into my private lab");
        add("anymore is by teleporter.");
        add("position(center)");
        add("speak_active");

        add("squeak(terminal)");
        add("text(gray,0,114,3)");
        add("I've made sure that it's");
        add("difficult for unauthorised");
        add("personnel to gain access.");
        add("position(center)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_lab_1")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(terminal)");
        add("text(gray,0,114,4)");
        add("... it turns out the key to");
        add("stabilising this dimension was");
        add("to create a balancing force");
        add("outside of it!");
        add("position(center)");
        add("speak_active");

        add("squeak(terminal)");
        add("text(gray,0,114,2)");
        add("Though it looks like that's just");
        add("a temporary solution, at best.");
        add("position(center)");
        add("speak_active");

        add("squeak(terminal)");
        add("text(gray,0,114,4)");
        add("I've been working on");
        add("something more permanent,");
        add("but it seems it's going");
        add("to be too late...");
        add("position(center)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_lab_2")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(terminal)");
        add("text(gray,0,114,1)");
        add("?SYNTAX  ERROR");
        add("position(center)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_secretlab")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(terminal)");
        add("text(gray,0,114,4)");
        add("         -= WARNING =-");
        add("");
        add(" The Super-Gravitron is intended  ");
        add(" for entertainment purposes only. ");
        add("position(center)");
        add("speak_active");

        add("squeak(terminal)");
        add("text(gray,0,114,4)");
        add("Anyone found using the Super");
        add("Gravitron for educational");
        add("purposes may be asked to");
        add("stand in the naughty corner.");
        add("position(center)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_shipcomputer")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(terminal)");
        add("text(gray,0,114,3)");
        add("   -= D.S.S. SOULEYE =-   ");
        add("");
        add(" Ship Navigation Controls ");
        add("position(center)");
        add("speak_active");

        add("ifflag(67,terminal_letsgo)");

        add("squeak(terminal)");
        add("text(gray,0,114,3)");
        add("Error! Error! Cannot isolate");
        add("dimensional coordinates!");
        add("Interference detected!");
        add("position(center)");
        add("speak_active");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_letsgo")
    {
        add("squeak(player)");
        add("text(player,0,0,2)");
        add("Now that the ship is fixed,");
        add("we can leave anytime we want!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,3)");
        add("We've all agreed to");
        add("keep exploring this");
        add("dimension, though.");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Who knows what we'll find?");
        add("position(player,above)");
        add("speak_active");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_radio")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(terminal)");
        add("text(gray,0,114,4)");
        add("  -= SHIP RADIO =-  ");
        add("");
        add("     [ Status ]");
        add("    Broadcasting");
        add("position(center)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_jukebox")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(terminal)");
        add("text(gray,0,114,7)");
        add("          -= JUKEBOX =-       ");
        add("");
        add("   Songs will continue to play   ");
        add("   until you leave the ship.");
        add("");
        add("      Collect trinkets to");
        add("       unlock new songs!");
        add("position(center)");
        add("speak_active");

        add("endtext");

        add("iftrinketsless(5,terminal_jukeunlock1)");
        add("iftrinketsless(8,terminal_jukeunlock2)");
        add("iftrinketsless(10,terminal_jukeunlock3)");
        add("iftrinketsless(12,terminal_jukeunlock4)");
        add("iftrinketsless(14,terminal_jukeunlock41)");
        add("iftrinketsless(16,terminal_jukeunlock5)");
        add("iftrinketsless(18,terminal_jukeunlock6)");
        add("iftrinketsless(20,terminal_jukeunlock7)");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_jukeunlock1")
    {
        add("squeak(terminal)");
        add("text(gray,0,114,4)");
        add(" NEXT UNLOCK:    ");
        add(" 5 Trinkets");
        add("");
        add(" Pushing Onwards ");
        add("position(center)");
        add("speak_active");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_jukeunlock2")
    {
        add("squeak(terminal)");
        add("text(gray,0,114,4)");
        add(" NEXT UNLOCK:    ");
        add(" 8 Trinkets");
        add("");
        add(" Positive Force ");
        add("position(center)");
        add("speak_active");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_jukeunlock3")
    {
        add("squeak(terminal)");
        add("text(gray,0,114,4)");
        add(" NEXT UNLOCK:    ");
        add(" 10 Trinkets");
        add("");
        add(" Presenting VVVVVV ");
        add("position(center)");
        add("speak_active");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_jukeunlock4")
    {
        add("squeak(terminal)");
        add("text(gray,0,114,4)");
        add(" NEXT UNLOCK:    ");
        add(" 12 Trinkets");
        add("");
        add(" Potential for Anything ");
        add("position(center)");
        add("speak_active");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_jukeunlock41")
    {
        add("squeak(terminal)");
        add("text(gray,0,114,4)");
        add(" NEXT UNLOCK:    ");
        add(" 14 Trinkets");
        add("");
        add(" Pressure Cooker ");
        add("position(center)");
        add("speak_active");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_jukeunlock5")
    {
        add("squeak(terminal)");
        add("text(gray,0,114,4)");
        add(" NEXT UNLOCK:    ");
        add(" 16 Trinkets");
        add("");
        add(" Predestined Fate ");
        add("position(center)");
        add("speak_active");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_jukeunlock6")
    {
        add("squeak(terminal)");
        add("text(gray,0,114,4)");
        add(" NEXT UNLOCK:    ");
        add(" 18 Trinkets");
        add("");
        add(" Popular Potpourri ");
        add("position(center)");
        add("speak_active");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_jukeunlock7")
    {
        add("squeak(terminal)");
        add("text(gray,0,114,4)");
        add(" NEXT UNLOCK:    ");
        add(" 20 Trinkets");
        add("");
        add(" Pipe Dream ");
        add("position(center)");
        add("speak_active");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "terminal_juke1")
    {
        add("play(4)");
        add("squeak(terminal)");
        add("jukebox(1)");
    }
    else if (t == "terminal_juke2")
    {
        add("play(1)");
        add("squeak(terminal)");
        add("jukebox(2)");
    }
    else if (t == "terminal_juke3")
    {
        add("play(2)");
        add("squeak(terminal)");
        add("jukebox(3)");
    }
    else if (t == "terminal_juke4")
    {
        add("play(6)");
        add("squeak(terminal)");
        add("jukebox(4)");
    }
    else if (t == "terminal_juke5")
    {
        add("play(3)");
        add("squeak(terminal)");
        add("jukebox(5)");
    }
    else if (t == "terminal_juke6")
    {
        add("play(8)");
        add("squeak(terminal)");
        add("jukebox(6)");
    }
    else if (t == "terminal_juke7")
    {
        add("play(11)");
        add("squeak(terminal)");
        add("jukebox(7)");
    }
    else if (t == "terminal_juke8")
    {
        add("play(10)");
        add("squeak(terminal)");
        add("jukebox(8)");
    }
    else if (t == "terminal_juke9")
    {
        add("play(12)");
        add("squeak(terminal)");
        add("jukebox(9)");
    }
    else if (t == "terminal_juke10")
    {
        add("play(9)");
        add("squeak(terminal)");
        add("jukebox(10)");
    }
}

#endif /* TERMINALSCRIPTS_H */
