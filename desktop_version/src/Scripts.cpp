#include "Script.h"
#include <SDL.h>

void scriptclass::load(const std::string& name)
{
    //loads script name t into the array
    position = 0;
    commands.clear();
    running = true;

    const char* t = name.c_str();

    char customstring[8] = { '\0' };
    SDL_strlcpy(customstring, t, sizeof(customstring));

    if (strcmp(customstring, "custom_") == 0)
    {
        loadcustom(name);
        return;
    }

    #include "scripts/generated/intro.vsc.cpp"

    #include "scripts/generated/quickstart.vsc.cpp"
    #include "scripts/generated/firststeps.vsc.cpp"
    #include "scripts/generated/trenchwarfare.vsc.cpp"
    #include "scripts/generated/newtrenchwarfare.vsc.cpp"
    #include "scripts/generated/trinketcollector.vsc.cpp"
    #include "scripts/generated/newtrinketcollector.vsc.cpp"
    #include "scripts/generated/new2trinketcollector.vsc.cpp"
    #include "scripts/generated/communicationstation.vsc.cpp"
    #include "scripts/generated/communicationstationskip.vsc.cpp"
    #include "scripts/generated/teleporterback.vsc.cpp"
    #include "scripts/generated/levelonecomplete.vsc.cpp"
    #include "scripts/generated/levelonecomplete_ending.vsc.cpp"
    #include "scripts/generated/levelonecompleteskip.vsc.cpp"
    #include "scripts/generated/bigopenworld.vsc.cpp"
    #include "scripts/generated/bigopenworldskip.vsc.cpp"
    #include "scripts/generated/rescueblue.vsc.cpp"
    #include "scripts/generated/skipblue.vsc.cpp"
    #include "scripts/generated/rescueyellow.vsc.cpp"
    #include "scripts/generated/skipyellow.vsc.cpp"
    #include "scripts/generated/rescuegreen.vsc.cpp"
    #include "scripts/generated/skipgreen.vsc.cpp"
    #include "scripts/generated/rescuered.vsc.cpp"
    #include "scripts/generated/skipred.vsc.cpp"
    #include "scripts/generated/startexpolevel_station1.vsc.cpp"
    #include "scripts/generated/startexpolevel_lab.vsc.cpp"
    #include "scripts/generated/startexpolevel_warp.vsc.cpp"
    #include "scripts/generated/startexpolevel_tower.vsc.cpp"
    #include "scripts/generated/skipint1.vsc.cpp"
    #include "scripts/generated/intermission_1.vsc.cpp"
    #include "scripts/generated/int1blue_1.vsc.cpp"
    #include "scripts/generated/int1blue_2.vsc.cpp"
    #include "scripts/generated/int1blue_3.vsc.cpp"
    #include "scripts/generated/int1blue_4.vsc.cpp"
    #include "scripts/generated/int1blue_5.vsc.cpp"
    #include "scripts/generated/int1blue_6.vsc.cpp"
    #include "scripts/generated/int1blue_7.vsc.cpp"
    #include "scripts/generated/int1green_1.vsc.cpp"
    #include "scripts/generated/int1green_2.vsc.cpp"
    #include "scripts/generated/int1green_3.vsc.cpp"
    #include "scripts/generated/int1green_4.vsc.cpp"
    #include "scripts/generated/int1green_5.vsc.cpp"
    #include "scripts/generated/int1green_6.vsc.cpp"
    #include "scripts/generated/int1green_7.vsc.cpp"
    #include "scripts/generated/int1red_1.vsc.cpp"
    #include "scripts/generated/int1red_2.vsc.cpp"
    #include "scripts/generated/int1red_3.vsc.cpp"
    #include "scripts/generated/int1red_4.vsc.cpp"
    #include "scripts/generated/int1red_5.vsc.cpp"
    #include "scripts/generated/int1red_6.vsc.cpp"
    #include "scripts/generated/int1red_7.vsc.cpp"
    #include "scripts/generated/int1yellow_1.vsc.cpp"
    #include "scripts/generated/int1yellow_2.vsc.cpp"
    #include "scripts/generated/int1yellow_3.vsc.cpp"
    #include "scripts/generated/int1yellow_4.vsc.cpp"
    #include "scripts/generated/int1yellow_5.vsc.cpp"
    #include "scripts/generated/int1yellow_6.vsc.cpp"
    #include "scripts/generated/int1yellow_7.vsc.cpp"
    #include "scripts/generated/skipint2.vsc.cpp"
    #include "scripts/generated/intermission_2.vsc.cpp"
    #include "scripts/generated/int2intro_yellow.vsc.cpp"
    #include "scripts/generated/int2intro_red.vsc.cpp"
    #include "scripts/generated/int2intro_green.vsc.cpp"
    #include "scripts/generated/int2intro_blue.vsc.cpp"
    #include "scripts/generated/int2_yellow.vsc.cpp"
    #include "scripts/generated/skipint2yellow.vsc.cpp"
    #include "scripts/generated/int2_red.vsc.cpp"
    #include "scripts/generated/skipint2red.vsc.cpp"
    #include "scripts/generated/int2_green.vsc.cpp"
    #include "scripts/generated/skipint2green.vsc.cpp"
    #include "scripts/generated/int2_blue.vsc.cpp"
    #include "scripts/generated/skipint2blue.vsc.cpp"
    #include "scripts/generated/startexpolevel_station2.vsc.cpp"
    #include "scripts/generated/finallevel_teleporter.vsc.cpp"
    #include "scripts/generated/skipfinal.vsc.cpp"
    #include "scripts/generated/startlevel_final.vsc.cpp"
    #include "scripts/generated/regularreturn.vsc.cpp"
    #include "scripts/generated/returntohub.vsc.cpp"
    #include "scripts/generated/resetgame.vsc.cpp"
    #include "scripts/generated/talkred.vsc.cpp"
    #include "scripts/generated/talkyellow.vsc.cpp"
    #include "scripts/generated/talkgreen.vsc.cpp"
    #include "scripts/generated/talkblue.vsc.cpp"
    #include "scripts/generated/talkpurple.vsc.cpp"
    #include "scripts/generated/talkred_1.vsc.cpp"
    #include "scripts/generated/talkred_2.vsc.cpp"
    #include "scripts/generated/talkred_3.vsc.cpp"
    #include "scripts/generated/talkred_4.vsc.cpp"
    #include "scripts/generated/talkred_5.vsc.cpp"
    #include "scripts/generated/talkred_6.vsc.cpp"
    #include "scripts/generated/talkred_7.vsc.cpp"
    #include "scripts/generated/talkred_8.vsc.cpp"
    #include "scripts/generated/talkred_9.vsc.cpp"
    #include "scripts/generated/talkred_10.vsc.cpp"
    #include "scripts/generated/talkred_11.vsc.cpp"
    #include "scripts/generated/talkred_12.vsc.cpp"
    #include "scripts/generated/talkred_13.vsc.cpp"
    #include "scripts/generated/talkred_14.vsc.cpp"
    #include "scripts/generated/talkyellow_1.vsc.cpp"
    #include "scripts/generated/talkyellow_2.vsc.cpp"
    #include "scripts/generated/talkyellow_3.vsc.cpp"
    #include "scripts/generated/talkyellow_4.vsc.cpp"
    #include "scripts/generated/talkyellow_5.vsc.cpp"
    #include "scripts/generated/talkyellow_6.vsc.cpp"
    #include "scripts/generated/talkyellow_7.vsc.cpp"
    #include "scripts/generated/talkyellow_8.vsc.cpp"
    #include "scripts/generated/talkyellow_9.vsc.cpp"
    #include "scripts/generated/talkyellow_10.vsc.cpp"
    #include "scripts/generated/talkyellow_11.vsc.cpp"
    #include "scripts/generated/talkyellow_12.vsc.cpp"
    #include "scripts/generated/talkgreen_1.vsc.cpp"
    #include "scripts/generated/talkgreen_2.vsc.cpp"
    #include "scripts/generated/talkgreen_3.vsc.cpp"
    #include "scripts/generated/talkgreen_4.vsc.cpp"
    #include "scripts/generated/talkgreen_5.vsc.cpp"
    #include "scripts/generated/talkgreen_6.vsc.cpp"
    #include "scripts/generated/talkgreen_7.vsc.cpp"
    #include "scripts/generated/talkgreen_8.vsc.cpp"
    #include "scripts/generated/talkgreen_9.vsc.cpp"
    #include "scripts/generated/talkgreen_10.vsc.cpp"
    #include "scripts/generated/talkgreen_11.vsc.cpp"

    #include "scripts/generated/talkpurple_1.vsc.cpp"
    #include "scripts/generated/talkpurple_2.vsc.cpp"
    #include "scripts/generated/talkpurple_3.vsc.cpp"
    #include "scripts/generated/talkpurple_4.vsc.cpp"
    #include "scripts/generated/talkpurple_5.vsc.cpp"
    #include "scripts/generated/talkpurple_6.vsc.cpp"
    #include "scripts/generated/talkpurple_7.vsc.cpp"
    #include "scripts/generated/talkpurple_8.vsc.cpp"
    #include "scripts/generated/talkpurple_9.vsc.cpp"
    #include "scripts/generated/talkpurple_intermission1.vsc.cpp"
    #include "scripts/generated/talkpurple_intermission2.vsc.cpp"
    #include "scripts/generated/talkpurple_intermission3.vsc.cpp"
    #include "scripts/generated/talkpurple_intro.vsc.cpp"
    #include "scripts/generated/talkblue_1.vsc.cpp"
    #include "scripts/generated/talkblue_2.vsc.cpp"
    #include "scripts/generated/talkblue_3.vsc.cpp"
    #include "scripts/generated/talkblue_4.vsc.cpp"
    #include "scripts/generated/talkblue_5.vsc.cpp"
    #include "scripts/generated/talkblue_6.vsc.cpp"
    #include "scripts/generated/talkblue_7.vsc.cpp"
    #include "scripts/generated/talkblue_8.vsc.cpp"
    #include "scripts/generated/talkblue_9.vsc.cpp"
    #include "scripts/generated/talkblue_trinket1.vsc.cpp"
    #include "scripts/generated/talkblue_trinket2.vsc.cpp"
    #include "scripts/generated/talkblue_trinket3.vsc.cpp"
    #include "scripts/generated/talkblue_trinket4.vsc.cpp"
    #include "scripts/generated/talkblue_trinket5.vsc.cpp"
    #include "scripts/generated/talkblue_trinket6.vsc.cpp"
    #include "scripts/generated/talkyellow_trinket1.vsc.cpp"
    #include "scripts/generated/talkyellow_trinket2.vsc.cpp"
    #include "scripts/generated/talkyellow_trinket3.vsc.cpp"
    #include "scripts/generated/gamecomplete.vsc.cpp"
    #include "scripts/generated/gamecomplete_ending.vsc.cpp"
    #include "scripts/generated/startepilogue.vsc.cpp"
    #include "scripts/generated/returntolab.vsc.cpp"
    
    // Terminal scripts
    #include "scripts/generated/terminal_station_1.vsc.cpp"
    #include "scripts/generated/alreadyvisited.vsc.cpp"
    #include "scripts/generated/terminal_outside_1.vsc.cpp"
    #include "scripts/generated/terminal_outside_2.vsc.cpp"
    #include "scripts/generated/terminal_outside_3.vsc.cpp"
    #include "scripts/generated/terminal_outside_4.vsc.cpp"
    #include "scripts/generated/terminal_outside_5.vsc.cpp"
    #include "scripts/generated/terminal_outside_6.vsc.cpp"
    #include "scripts/generated/terminal_finallevel.vsc.cpp"
    #include "scripts/generated/finalterminal_finish.vsc.cpp"
    #include "scripts/generated/terminal_station_2.vsc.cpp"
    #include "scripts/generated/terminal_station_3.vsc.cpp"
    #include "scripts/generated/terminal_station_4.vsc.cpp"
    #include "scripts/generated/terminal_warp_1.vsc.cpp"
    #include "scripts/generated/terminal_warp_2.vsc.cpp"
    #include "scripts/generated/terminal_lab_1.vsc.cpp"
    #include "scripts/generated/terminal_lab_2.vsc.cpp"
    #include "scripts/generated/terminal_secretlab.vsc.cpp"
    #include "scripts/generated/terminal_shipcomputer.vsc.cpp"
    #include "scripts/generated/terminal_letsgo.vsc.cpp"
    #include "scripts/generated/terminal_radio.vsc.cpp"
    #include "scripts/generated/terminal_jukebox.vsc.cpp"
    #include "scripts/generated/terminal_jukeunlock1.vsc.cpp"
    #include "scripts/generated/terminal_jukeunlock2.vsc.cpp"
    #include "scripts/generated/terminal_jukeunlock3.vsc.cpp"
    #include "scripts/generated/terminal_jukeunlock4.vsc.cpp"
    #include "scripts/generated/terminal_jukeunlock41.vsc.cpp"
    #include "scripts/generated/terminal_jukeunlock5.vsc.cpp"
    #include "scripts/generated/terminal_jukeunlock6.vsc.cpp"
    #include "scripts/generated/terminal_jukeunlock7.vsc.cpp"
    #include "scripts/generated/terminal_juke1.vsc.cpp"
    #include "scripts/generated/terminal_juke2.vsc.cpp"
    #include "scripts/generated/terminal_juke3.vsc.cpp"
    #include "scripts/generated/terminal_juke4.vsc.cpp"
    #include "scripts/generated/terminal_juke5.vsc.cpp"
    #include "scripts/generated/terminal_juke6.vsc.cpp"
    #include "scripts/generated/terminal_juke7.vsc.cpp"
    #include "scripts/generated/terminal_juke8.vsc.cpp"
    #include "scripts/generated/terminal_juke9.vsc.cpp"
    #include "scripts/generated/terminal_juke10.vsc.cpp"
}
