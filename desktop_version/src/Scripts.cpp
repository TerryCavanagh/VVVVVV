#ifndef SCRIPTS_H
#define SCRIPTS_H

#include "Script.h"

#include <algorithm>

extern scriptclass script;

void scriptclass::load(std::string t)
{
    //loads script name t into the array
    position = 0;
    scriptlength=0;
    running = true;

	int maxlength = (std::min(int(t.length()),7));
    std::string customstring="";
    for(int i=0; i<maxlength; i++){
      customstring+=t[i];
    }

    if (customstring == "custom_"){
      //this magic function breaks down the custom script and turns into real scripting!
      std::string cscriptname="";
      for(size_t i=0; i<t.length(); i++){
        if(i>=7) cscriptname+=t[i];
      }

      int scriptstart=-1;
      int scriptend=-1;
      std::string tstring;

      for(size_t i=0; i<customscript.size(); i++){
        if(scriptstart==-1){
          //Find start of the script
          if(script.customscript[i]==cscriptname+":"){
            scriptstart=i+1;
          }
        }else if(scriptend==-1){
          //Find the end
          tstring=script.customscript[i];
          tstring=tstring[tstring.size()-1];
          if(tstring==":"){
            scriptend=i;
          }
        }
      }
      if(scriptstart>-1){
        if(scriptend==-1){
          scriptend=customscript.size();
        }

        //Ok, we've got the relavent script segment, we do a pass to assess it, then run it!
        int customcutscenemode=0;
        for(int i=scriptstart; i<scriptend; i++){
          tokenize(script.customscript[i]);
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
        for(int i=scriptstart; i<scriptend; i++){
          words[0]="nothing"; //Default!
          words[1]="1"; //Default!
          tokenize(script.customscript[i]);
          std::transform(words[0].begin(), words[0].end(), words[0].begin(), ::tolower);
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
            add("playef(9,10)");
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
            add(script.customscript[i]);
          }else if(words[0] == "flag"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            add(script.customscript[i]);
          }else if(words[0] == "map"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            add("custom"+script.customscript[i]);
          }else if(words[0] == "warpdir"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            add(script.customscript[i]);
          }else if(words[0] == "ifwarp"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            add(script.customscript[i]);
          }else if(words[0] == "iftrinkets"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            add("custom"+script.customscript[i]);
          }else if(words[0] == "ifflag"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            add("custom"+script.customscript[i]);
          }else if(words[0] == "iftrinketsless"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            add("custom"+script.customscript[i]);
          }else if(words[0] == "destroy"){
            if(customtextmode==1){ add("endtext"); customtextmode=0;}
            if(words[1]=="gravitylines"){
              add("destroy(gravitylines)");
            }else if(words[1]=="warptokens"){
              add("destroy(warptokens)");
            }else if(words[1]=="platforms"){
              add("destroy(platforms)");
            }
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
            int ti=atoi(words[1].c_str());
            if(ti>=0 && ti<=50){
              for(int ti2=0; ti2<ti; ti2++){
                i++; add(script.customscript[i]);
              }
            }else{
              i++; add(script.customscript[i]);
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

            int ti=atoi(words[1].c_str());
            if(ti>=0 && ti<=50){
              for(int ti2=0; ti2<ti; ti2++){
                i++; add(script.customscript[i]);
              }
            }else{
              i++; add(script.customscript[i]);
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
      }
    }
    else if (t == "intro")
    {
        add("ifskip(quickstart)");
        //add("createcrewman(232,113,cyan,0,faceright)");
        add("createcrewman(96,177,green,0,faceright)");
        add("createcrewman(122,177,purple,0,faceleft)");
        add("fadein()");
        add("untilfade()");
        add("delay(90)");

        add("flash(5)");
        add("shake(20)");
        add("playef(9,10)");
        add("musicfadeout()");
        add("changemood(player,1)");
        add("delay(15)");
        add("squeak(player)");
        add("text(cyan,0,0,1)");
        add("Uh oh...");
        add("position(player,above)");
        //add("backgroundtext");
        add("speak_active");

        add("squeak(purple)");
        add("changeai(purple,followposition,175)");
        add("text(purple,145,150,1)");
        add("Is everything ok?");
        //add("position(purple,above)");
        //add("backgroundtext");
        add("speak_active");

        add("squeak(player)");
        add("walk(left,2)");
        add("text(cyan,0,0,2)");
        add("No! We've hit some");
        add("kind of interference...");
        add("position(player,above)");
        //add("backgroundtext");
        add("speak_active");

        //add("delay(30)");
        add("endtext");

        add("flash(5)");
        add("shake(50)");
        add("playef(9,10)");
        add("changemood(green,1)");
        add("changemood(purple,1)");
        add("alarmon");

        add("changedir(player,1)");
        add("delay(30)");
        add("endtext");

        add("squeak(player)");
        add("text(cyan,0,0,2)");
        add("Something's wrong! We're");
        add("going to crash!");
        add("position(player,above)");
        //add("backgroundtext");
        add("speak_active");
        //add("delay(100)");

        add("endtext");

        add("flash(5)");
        add("shake(50)");
        add("playef(9,10)");
        add("changeai(green,followposition,-60)");
        add("changeai(purple,followposition,-60)");
        add("squeak(player)");
        add("text(cyan,70,140,1)");
        add("Evacuate!");
        add("backgroundtext");
        add("speak_active");
        add("walk(left,35)");

        add("endtextfast");

        //Ok, next room!

        add("flash(5)");
        add("shake(50)");
        add("playef(9,10)");
        add("gotoroom(3,10)");
        add("gotoposition(310,177,0)");
        add("createcrewman(208,177,green,1,followposition,120)");
        add("createcrewman(240,177,purple,1,followposition,120)");
        add("createcrewman(10,177,blue,1,followposition,180)");

        add("squeak(blue)");
        add("text(blue,80,150,1)");
        add("Oh no!");
        add("backgroundtext");
        add("speak_active");
        add("walk(left,20)");

        add("endtextfast");

        //and the next!
        add("flash(5)");
        add("shake(50)");
        add("playef(9,10)");
        add("gotoroom(3,11)");
        add("gotoposition(140,0,0)");

        add("createcrewman(90,105,green,1,followblue)");
        add("createcrewman(125,105,purple,1,followgreen)");
        add("createcrewman(55,105,blue,1,followposition,-200)");

        add("createcrewman(120,177,yellow,1,followposition,-200)");
        add("createcrewman(240,177,red,1,faceleft)");

        add("delay(5)");
        add("changeai(red,followposition,-200)");

        add("squeak(red)");
        add("text(red,100,150,1)");
        add("Everyone off the ship!");
        add("backgroundtext");
        add("speak_active");

        add("walk(left,25)");

        add("endtextfast");

        //final room:
        add("flash(5)");
        add("shake(80)");
        add("playef(9,10)");
        add("gotoroom(2,11)");
        add("gotoposition(265,153,0)");

        add("createcrewman(130,153,blue,1,faceleft)");
        add("createcrewman(155,153,green,1,faceleft)");
        add("createcrewman(180,153,purple,1,faceleft)");
        add("createcrewman(205,153,yellow,1,faceleft)");
        add("createcrewman(230,153,red,1,faceleft)");


        add("squeak(yellow)");
        add("text(yellow,0,0,1)");
        add("This shouldn't be happening!");
        add("position(yellow,below)");
        add("backgroundtext");
        add("speak_active");

        add("activateteleporter()");

        add("delay(10)");
        add("changecolour(blue,teleporter)");
        add("delay(10)");
        add("changecolour(green,teleporter)");
        add("delay(10)");
        add("changecolour(purple,teleporter)");
        add("delay(10)");
        add("changecolour(yellow,teleporter)");
        add("delay(10)");
        add("changecolour(red,teleporter)");
        add("delay(10)");

        //and teleport!
        add("endtext");
        add("alarmoff");
        add("flash(5)");
        add("shake(20)");
        add("playef(10,10)");
        add("blackout()");
        add("changemood(player,0)");
        add("changedir(player,1)");

        add("delay(100)");
        add("blackon()");
        add("shake(20)");
        add("playef(10,10)");

        //Finally, appear at the start of the game:
        add("gotoroom(13,5)");
        add("gotoposition(80,96,0)");
        add("walk(right,20)");
        //add("delay(45)");

        add("squeak(player)");
        add("text(cyan,0,0,1)");
        add("Phew! That was scary!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(cyan,0,0,2)");
        add("At least we all");
        add("escaped, right guys?");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        add("delay(45)");
        add("walk(left,3)");
        add("delay(45)");
        add("setcheckpoint()");

        add("squeak(player)");
        add("text(cyan,0,0,1)");
        add("...guys?");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        add("delay(25)");
        add("changemood(player,1)");
        add("squeak(cry)");
        add("delay(25)");

        add("play(1)");
        add("endcutscene()");
        add("untilbars()");

        add("hideship()");

        add("gamestate(4)");
    }
    else if (t == "quickstart")
    {
        //Finally, appear at the start of the game:
        add("gotoroom(13,5)");
        add("gotoposition(80,96,0)");
        add("walk(right,17)");
        add("fadein()");

        add("setcheckpoint()");

        add("play(1)");
        add("endcutscene()");
        add("untilbars()");

        add("hideship()");
    }
    else if (t == "firststeps")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(player)");
        add("text(cyan,0,0,2)");
        add("I wonder why the ship");
        add("teleported me here alone?");
        add("position(player,above)");
        add("speak_active");

        add("squeak(cry)");
        add("text(cyan,0,0,2)");
        add("I hope everyone else");
        add("got out ok...");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "trenchwarfare")
    {
        add("cutscene()");
        add("untilbars()");

        add("iftrinkets(1,newtrenchwarfare)");

        add("squeak(player)");
        add("text(cyan,0,0,1)");
        add("Ohh! I wonder what that is?");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(cyan,0,0,3)");
        add("I probably don't really need it,");
        add("but it might be nice to take it");
        add("back to the ship to study...");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "newtrenchwarfare")
    {
        add("squeak(player)");
        add("text(cyan,0,0,2)");
        add("Oh! It's another one of");
        add("those shiny things!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(cyan,0,0,3)");
        add("I probably don't really need it,");
        add("but it might be nice to take it");
        add("back to the ship to study...");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "trinketcollector")
    {
        add("cutscene()");
        add("untilbars()");

        add("iftrinkets(1,newtrinketcollector)");

        add("squeak(player)");
        add("text(cyan,0,0,3)");
        add("This seems like a good");
        add("place to store anything");
        add("I find out there...");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(cyan,0,0,3)");
        add("Victoria loves to study the");
        add("interesting things we find");
        add("on our adventures!");
        add("position(player,above)");
        add("speak_active");

        add("ifcrewlost(5,new2trinketcollector)");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "newtrinketcollector")
    {
        add("squeak(player)");
        add("text(cyan,0,0,3)");
        add("This seems like a good");
        add("place to store those");
        add("shiny things.");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(cyan,0,0,3)");
        add("Victoria loves to study the");
        add("interesting things we find");
        add("on our adventures!");
        add("position(player,above)");
        add("speak_active");

        add("ifcrewlost(5,new2trinketcollector)");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "new2trinketcollector")
    {
        add("squeak(cry)");
        add("changemood(player,1)");
        add("text(cyan,0,0,1)");
        add("I hope she's ok...");
        add("position(player,above)");
        add("speak_active");
        add("endtext");

        add("changemood(player,0)");

        add("endcutscene()");
        add("untilbars()");
		return;
    }
    if (t == "communicationstation")
    {
        add("ifskip(communicationstationskip)");
        add("cutscene()");
        add("untilbars()");
        add("changemood(player,0)");

        add("tofloor");
        add("play(5)");
        add("delay(10)");

        add("squeak(player)");
        add("text(cyan,0,0,1)");
        add("Violet! Is that you?");
        add("position(player,above)");
        add("speak_active");
        add("endtext");

        add("squeak(purple)");
        add("text(purple,45,18,1)");
        add("Captain! You're ok!");
        add("speak_active");

        add("squeak(cry)");
        add("text(purple,20,16,3)");
        add("Something has gone");
        add("horribly wrong with the");
        add("ship's teleporter!");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,8,14,3)");
        add("I think everyone has been");
        add("teleported away randomly!");
        add("They could be anywhere!");
        add("speak_active");

        add("squeak(cry)");
        add("changemood(player,1)");
        add("text(cyan,0,0,1)");
        add("Oh no!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,10,19,2)");
        add("I'm on the ship - it's damaged");
        add("badly, but it's still intact!");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,10,15,1)");
        add("Where are you, Captain?");
        add("speak_active");

        add("squeak(player)");
        add("changemood(player,0)");
        add("text(cyan,0,0,3)");
        add("I'm on some sort of");
        add("space station... It");
        add("seems pretty modern...");
        add("position(player,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,15,16,2)");
        add("There seems to be some sort of");
        add("interference in this dimension...");
        add("speak_active");

        add("hideteleporters()");
        add("endtextfast");
        add("delay(10)");

        //add map mode here and wrap up...
        add("gamemode(teleporter)");
        add("delay(20)");

        add("squeak(purple)");
        add("text(purple,25,205,2)");
        add("I'm broadcasting the coordinates");
        add("of the ship to you now.");
        add("speak_active");
        add("endtext");

        add("squeak(terminal)");
        add("showship()");
        add("delay(10)");
        add("hideship()");
        add("delay(10)");
        add("showship()");
        add("delay(10)");
        add("hideship()");
        add("delay(10)");
        add("showship()");
        add("delay(20)");

        add("squeak(purple)");
        add("text(purple,10,200,1)");
        add("I can't teleport you back, but...");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,25,195,3)");
        add("If YOU can find a teleporter");
        add("anywhere nearby, you should be");
        add("able to teleport back to me!");
        add("speak_active");
        add("endtext");

        add("squeak(terminal)");
        add("delay(20)");
        add("showteleporters()");
        add("delay(10)");
        add("hideteleporters()");
        add("delay(10)");
        add("showteleporters()");
        add("delay(10)");
        add("hideteleporters()");
        add("delay(10)");
        add("showteleporters()");
        add("delay(20)");

        add("squeak(player)");
        add("text(cyan,20,190,1)");
        add("Ok! I'll try to find one!");
        add("speak_active");
        add("endtext");
        add("delay(20)");

        add("gamemode(game)");
        add("delay(20)");

        add("squeak(purple)");
        add("text(purple,40,22,1)");
        add("Good luck, Captain!");
        add("speak_active");
        add("endtext");

        add("squeak(purple)");
        add("text(purple,10,19,2)");
        add("I'll keep trying to find");
        add("the rest of the crew...");
        add("speak_active");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("play(1)");
    }
    else if (t == "communicationstationskip")
    {
        add("changemood(player,0)");

        add("delay(10)");
        add("endtext");
        //add map mode here and wrap up...
        add("gamemode(teleporter)");
        add("delay(5)");

        add("squeak(terminal)");
        add("showship()");
        add("showteleporters()");
        add("delay(10)");
        add("hideship()");
        add("hideteleporters()");
        add("delay(10)");
        add("showship()");
        add("showteleporters()");
        add("delay(10)");
        add("hideship()");
        add("hideteleporters()");
        add("delay(10)");
        add("showship()");
        add("showteleporters()");
        add("delay(20)");

        add("gamemode(game)");
    }
    else if (t == "teleporterback")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(player)");
        add("text(cyan,0,0,1)");
        add("A teleporter!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(cyan,0,0,2)");
        add("I can get back to the");
        add("ship with this!");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        add("teleportscript(levelonecomplete)");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "levelonecomplete")
    {
        add("nocontrol()");
        add("createcrewman(230,153,purple,0,faceleft)");

        add("cutscene()");
        add("untilbars()");

        add("delay(30)");
        add("rescued(purple)");

        add("delay(10)");
        add("gamestate(4090)");
    }
    else if (t == "levelonecomplete_ending")
    {
        add("squeak(purple)");
        add("text(purple,0,0,1)");
        add("Captain!");
        add("position(purple,above)");
        add("speak_active");
        add("endtext");
        add("nocontrol()");

        add("endcutscene()");
        add("untilbars()");

        add("gamestate(3050)");
    }
    else if (t == "levelonecompleteskip")
    {
        add("nocontrol()");
        add("gamestate(3050)");
    }
    else if (t == "bigopenworld")
    {
        add("play(5)");
        add("cutscene()");
        add("untilbars()");

        add("gotoroom(4,10)");
        add("gotoposition(100,177,0)");
        add("createcrewman(150,177,purple,0,faceleft)");

        //set all the crew as rescued to avoid companion issues!
        add("rescued(red)");
        add("rescued(green)");
        add("rescued(blue)");
        add("rescued(yellow)");

        add("fadein()");
        add("untilfade()");

        add("delay(15)");

        add("squeak(player)");
        add("text(player,0,0,2)");
        add("So, Doctor - have you any");
        add("idea what caused the crash?");
        add("position(player,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,3)");
        add("There's some sort of bizarre");
        add("signal here that's interfering");
        add("with our equipment...");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,3)");
        add("It caused the ship to lose");
        add("its quantum position, collapsing");
        add("us into this dimension!");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changemood(player,1)");
        add("text(player,0,0,1)");
        add("Oh no!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,2)");
        add("But I think we should be able to fix");
        add("the ship and get out of here...");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,2)");
        add("... as long as we can");
        add("find the rest of the crew.");
        add("position(purple,above)");
        add("speak_active");

        add("endtext");

        //Cut to Red
        add("fadeout()");
        add("untilfade()");

        add("changeplayercolour(red)");
        add("gotoroom(10,4)");
        add("gotoposition(200,185,0)");
        add("hideplayer()");
        add("createcrewman(200,185,red,1,panic)");

        add("fadein()");
        add("untilfade()");

        //add("walk(right,10)");

        add("squeak(purple)");
        add("text(purple,60,40,2)");
        add("We really don't know anything");
        add("about this place...");
        add("speak_active");

        add("endtext");

        add("delay(15)");

        //Cut to Green
        add("fadeout()");
        add("untilfade()");

        add("showplayer()");

        add("changeplayercolour(green)");
        add("gotoroom(13,0)");
        add("gotoposition(143,20,0)");

        add("fadein()");
        add("untilfade()");


        add("squeak(purple)");
        add("text(purple,40,30,2)");
        add("Our friends could be anywhere - they");
        add("could be lost, or in danger!");
        add("speak_active");

        add("endtext");

        add("delay(15)");

        //Cut to Blue
        add("fadeout()");
        add("untilfade()");

        add("changeplayercolour(blue)");
        add("gotoroom(3,4)");
        add("gotoposition(190,177,0)");

        add("fadein()");
        add("untilfade()");


        add("squeak(player)");
        add("text(player,10,60,1)");
        add("Can they teleport back here?");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,50,80,2)");
        add("Not unless they find some way");
        add("to communicate with us!");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,30,100,3)");
        add("We can't pick up their signal and");
        add("they can't teleport here unless");
        add("they know where the ship is...");
        add("speak_active");

        add("endtext");

        add("delay(15)");


        //Cut to Yellow
        add("fadeout()");
        add("untilfade()");

        add("changeplayercolour(yellow)");
        add("gotoroom(15,9)");
		//(6*8)-21
        add("gotoposition(300,27,0)");

        add("hideplayer()");
        add("createcrewman(280,25,yellow,1,panic)");

        //add("hascontrol()");
        //add("walk(left,4)");

        add("fadein()");
        add("untilfade()");

        add("squeak(player)");
        add("text(player,25,60,1)");
        add("So what do we do?");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,80,125,4)");
        add("We need to find them! Head");
        add("out into the dimension and");
        add("look for anywhere they might");
        add("have ended up...");
        add("speak_active");

        add("endtext");

        add("delay(15)");

        //Back to ship
        add("fadeout()");
        add("untilfade()");
        add("showplayer()");

        add("missing(red)");
        add("missing(green)");
        add("missing(blue)");
        add("missing(yellow)");

        add("changeplayercolour(cyan)");
        add("changemood(player,0)");
        add("gotoroom(4,10)");
        add("gotoposition(90,177,0)");
        add("walk(right,2)");
        add("createcrewman(150,177,purple,0,faceleft)");



        add("fadein()");
        add("untilfade()");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Ok! Where do we start?");
        add("position(player,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,2)");
        add("Well, I've been trying to find");
        add("them with the ship's scanners!");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,2)");
        add("It's not working, but I did");
        add("find something...");
        add("position(purple,above)");
        add("speak_active");

        add("endtext");

        add("delay(15)");

        add("hidecoordinates(10,4)");
        add("hidecoordinates(13,0)");
        add("hidecoordinates(3,4)");
        add("hidecoordinates(15,9)");
        add("showteleporters()");

        //Cut to map
        //add map mode here and wrap up...
        add("gamemode(teleporter)");
        add("delay(20)");

        add("squeak(terminal)");
        add("showtargets()");
        add("delay(10)");
        add("hidetargets()");
        add("delay(10)");
        add("showtargets()");
        add("delay(10)");
        add("hidetargets()");
        add("delay(10)");
        add("showtargets()");
        add("delay(20)");

        add("squeak(purple)");
        add("text(purple,25,205,2)");
        add("These points show up on our scans");
        add("as having high energy patterns!");
        add("speak_active");
        add("endtext");

        add("squeak(purple)");
        add("text(purple,35,185,4)");
        add("There's a good chance they're");
        add("teleporters - which means");
        add("they're probably built near");
        add("something important...");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,25,205,2)");
        add("They could be a very good");
        add("place to start looking.");
        add("speak_active");
        add("endtext");
        add("delay(20)");

        add("gamemode(game)");
        add("delay(20)");
        //And finally, back to the ship!

        add("squeak(player)");
        add("text(player,0,0,2)");
        add("Ok! I'll head out and see");
        add("what I can find!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,2)");
        add("I'll be right here if");
        add("you need any help!");
        add("position(purple,above)");
        add("speak_active");
        add("endtext");

        add("rescued(purple)");
        add("play(4)");

        add("endcutscene()");
        add("untilbars()");
        add("hascontrol()");

        add("createactivityzone(purple)");
    }
    else if (t == "bigopenworldskip")
    {
        add("gotoroom(4,10)");
        add("gotoposition(100,177,0)");
        add("createcrewman(150,177,purple,0,faceleft)");

        add("fadein()");
        add("untilfade()");

        add("hidecoordinates(10,4)");
        add("hidecoordinates(13,0)");
        add("hidecoordinates(3,4)");
        add("hidecoordinates(15,9)");
        add("showteleporters()");

        //Cut to map
        //add map mode here and wrap up...
        add("gamemode(teleporter)");
        add("delay(20)");

        add("squeak(terminal)");
        add("showtargets()");
        add("delay(10)");
        add("hidetargets()");
        add("delay(10)");
        add("showtargets()");
        add("delay(10)");
        add("hidetargets()");
        add("delay(10)");
        add("showtargets()");
        add("delay(20)");

        add("gamemode(game)");
        add("delay(20)");
        //And finally, back to the ship!

        add("squeak(purple)");
        add("text(purple,0,0,2)");
        add("I'll be right here if");
        add("you need any help!");
        add("position(purple,above)");
        add("speak_active");
        add("endtext");

        add("rescued(purple)");
        add("play(4)");

        add("endcutscene()");
        add("untilbars()");
        add("hascontrol()");

        add("createactivityzone(purple)");
    }
    else if (t == "rescueblue")
    {
        add("ifskip(skipblue)");
        add("cutscene()");

        add("tofloor()");
        add("changeai(blue,followplayer)");
        add("untilbars()");

        add("rescued(blue)");

        add("squeak(blue)");
        add("text(blue,0,0,2)");
        add("Oh no! Captain! Are you");
        add("stuck here too?");
        add("position(blue,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("It's ok - I'm here to rescue you!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Let me explain everything...");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        add("fadeout()");
        add("untilfade()");
        add("delay(30)");
        add("fadein()");
        add("untilfade()");

        add("squeak(cry)");
        add("text(blue,0,0,2)");
        add("What? I didn't understand");
        add("any of that!");
        add("position(blue,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Oh... well, don't worry.");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,2)");
        add("Follow me! Everything");
        add("will be alright!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(blue)");
        add("changemood(blue,0)");
        add("text(blue,0,0,1)");
        add("Sniff... Really?");
        add("position(blue,above)");
        add("speak_active");

        add("squeak(blue)");
        add("text(blue,0,0,1)");
        add("Ok then!");
        add("position(blue,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("companion(8)");
        add("setcheckpoint()");
    }
    else if (t == "skipblue")
    {
        add("changeai(blue,followplayer)");
        add("squeak(blue)");
        add("changemood(blue,0)");
        add("companion(8)");
        add("rescued(blue)");
        add("setcheckpoint()");
    }
    else if (t == "rescueyellow")
    {
        add("ifskip(skipyellow)");
        add("cutscene()");

        add("changeai(yellow,followplayer)");
        add("changetile(yellow,6)");
        add("untilbars()");

        add("rescued(yellow)");

        add("squeak(yellow)");
        add("text(yellow,0,0,2)");
        add("Ah, Viridian! You got off");
        add("the ship alright too? ");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,2)");
        add("It's good to see you're");
        add("alright, Professor!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,1)");
        add("Is the ship ok?");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,2)");
        add("It's badly damaged, but Violet's");
        add("been working on fixing it.");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("We could really use your help...");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        add("fadeout()");
        add("untilfade()");
        add("delay(30)");
        add("fadein()");
        add("untilfade()");

        add("squeak(yellow)");
        add("text(yellow,0,0,1)");
        add("Ah, of course!");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,4)");
        add("The background interference");
        add("in this dimension prevented");
        add("the ship from finding a");
        add("teleporter when we crashed!");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,2)");
        add("We've all been teleported");
        add("to different locations!");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Er, that sounds about right!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,2)");
        add("Let's get back to");
        add("the ship, then!");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,1)");
        add("After you, Captain!");
        add("position(yellow,above)");
        add("speak_active");

        add("endtext");

        add("companion(7)");
        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "skipyellow")
    {
        add("changeai(yellow,followplayer)");
        add("changetile(yellow,6)");
        add("squeak(yellow)");
        add("rescued(yellow)");
        add("companion(7)");
    }
    else if (t == "rescuegreen")
    {
        add("ifskip(skipgreen)");
        add("cutscene()");

        add("tofloor()");
        add("changemood(green,0)");
        add("untilbars()");

        add("rescued(green)");

        add("squeak(green)");
        add("text(green,0,0,1)");
        add("Captain! I've been so worried!");
        add("position(green,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Chief Verdigris! You're ok!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changemood(green,1)");
        add("text(green,0,0,2)");
        add("I've been trying to get out, but");
        add("I keep going around in circles...");
        add("position(green,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,2)");
        add("I've come from the ship. I'm here");
        add("to teleport you back to it.");
        add("position(player,above)");
        add("speak_active");

        add("squeak(green)");
        add("text(green,0,0,2)");
        add("Is everyone else");
        add("alright? Is Violet...");
        add("position(green,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("She's fine - she's back on the ship!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(green)");
        add("changemood(green,0)");
        add("text(green,0,0,2)");
        add("Oh! Great - Let's");
        add("get going, then!");
        add("position(green,above)");
        add("speak_active");

        add("endtext");

        add("companion(6)");
        add("endcutscene()");
        add("untilbars()");
        add("changeai(green,followplayer)");
    }
    else if (t == "skipgreen")
    {
        add("changeai(green,followplayer)");
        add("squeak(green)");
        add("rescued(green)");
        add("changemood(green,0)");
        add("companion(6)");
    }
    else if (t == "rescuered")
    {
        add("ifskip(skipred)");
        add("cutscene()");

        add("tofloor()");
        add("changemood(red,0)");
        add("untilbars()");

        add("rescued(red)");

        add("squeak(red)");
        add("text(red,0,0,1)");
        add("Captain!");
        add("position(red,above)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,3)");
        add("Am I ever glad to see you!");
        add("I thought I was the only");
        add("one to escape the ship...");
        add("position(red,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Vermilion! I knew you'd be ok!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,1)");
        add("So, what's the situation?");
        add("position(red,above)");
        add("speak_active");

        add("endtext");

        add("fadeout()");
        add("untilfade()");
        add("delay(30)");
        add("fadein()");
        add("untilfade()");

        add("squeak(red)");
        add("text(red,0,0,2)");
        add("I see! Well, we'd better");
        add("get back then.");
        add("position(red,above)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,2)");
        add("There's a teleporter");
        add("in the next room.");
        add("position(red,above)");
        add("speak_active");

        add("endtext");

        add("companion(9)");
        add("endcutscene()");
        add("untilbars()");
        add("changeai(red,followplayer)");
    }
    else if (t == "skipred")
    {
        add("changeai(red,followplayer)");
        add("squeak(red)");
        add("rescued(red)");
        add("changemood(red,0)");
        add("companion(9)");
    }
    else if (t == "startexpolevel_station1")
    {
        //For the Eurogamer EXPO! Scrap later.
        add("fadeout()");
        add("musicfadeout()");
        add("untilfade()");

        add("cutscene()");
        add("untilbars()");

        add("resetgame");

        add("gotoroom(4,10)");
        add("gotoposition(232,113,0)");
        add("setcheckpoint()");
        add("changedir(player,1)");
        add("fadein()");

        add("play(5)");
        add("loadscript(intro)");
    }
    else if (t == "startexpolevel_lab")
    {
        //For the Eurogamer EXPO! Scrap later.
        add("fadeout()");
        add("musicfadeout()");
        add("untilfade()");
        add("delay(30)");

        add("resetgame");

        add("gotoroom(2,16)");
        add("gotoposition(58,193,0)");
        add("setcheckpoint()");
        add("changedir(player,1)");
        add("fadein()");

        add("stopmusic()");
        add("play(3)");
    }
    else if (t == "startexpolevel_warp")
    {
        //For the Eurogamer EXPO! Scrap later.
        add("fadeout()");
        add("musicfadeout()");
        add("untilfade()");
        add("delay(30)");

        add("resetgame");

        add("gotoroom(14,1)");
        add("gotoposition(45,73,0)");
        add("setcheckpoint()");
        add("changedir(player,1)");
        add("fadein()");

        add("stopmusic()");
        add("play(3)");
    }
    else if (t == "startexpolevel_tower")
    {
        //For the Eurogamer EXPO! Scrap later.
        add("fadeout()");
        add("musicfadeout()");
        add("untilfade()");
        add("delay(30)");

        add("resetgame");

        add("gotoroom(8,9)");
        add("gotoposition(95,193,0)");
        add("setcheckpoint()");
        add("changedir(player,1)");
        add("fadein()");

        add("stopmusic()");
        add("play(2)");
    }
    else if (t == "skipint1")
    {
        add("finalmode(41,56)");
        add("gotoposition(52,89,0)");
        add("changedir(player,1)");
        add("setcheckpoint()");
        add("delay(15)");

        add("flash(5)");
        add("shake(20)");
        add("playef(9,10)");
        add("showplayer()");
        add("play(8)");

        add("hascontrol()");
        add("befadein()");
    }
    else if (t == "intermission_1")
    {
        add("ifskip(skipint1)");
        add("finalmode(41,56)");
        add("gotoposition(52,89,0)");
        add("changedir(player,1)");
        add("setcheckpoint()");

        add("cutscene()");

        add("delay(15)");

        add("flash(5)");
        add("shake(20)");
        add("playef(9,10)");

        add("delay(35)");

        add("flash(5)");
        add("shake(20)");
        add("playef(9,10)");

        add("delay(25)");

        add("flash(5)");
        add("shake(20)");
        add("playef(10,10)");

        add("showplayer()");
        add("play(8)");

        add("befadein()");

        add("iflast(2,int1yellow_1)");
        add("iflast(3,int1red_1)");
        add("iflast(4,int1green_1)");
        add("iflast(5,int1blue_1)");
    }
    else if (t == "int1blue_1")
    {
        add("delay(45)");

        add("squeak(cry)");
        add("text(blue,0,0,1)");
        add("Waaaa!");
        add("position(blue,above)");
        add("speak_active");
        add("face(player,blue)");
        add("face(blue,player)");

        add("squeak(blue)");
        add("text(blue,0,0,1)");
        add("Captain! Are you ok?");
        add("position(blue,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,2)");
        add("I'm ok... this...");
        add("this isn't the ship...");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Where are we?");
        add("position(player,above)");
        add("speak_active");

        add("squeak(cry)");
        add("text(blue,0,0,1)");
        add("Waaaa!");
        add("position(blue,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,2)");
        add("Something's gone wrong... We");
        add("should look for a way back!");
        add("position(player,above)");
        add("speak_active");
        add("endtext");

        add("telesave()");

        add("endcutscene()");
        add("untilbars()");
        add("gamestate(14)");
    }
    else if (t == "int1blue_2")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Follow me! I'll help you!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(cry)");
        add("text(blue,0,0,1)");
        add("Promise you won't leave without me!");
        add("position(blue,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("I promise! Don't worry!");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("gamestate(11)");
    }
    else if (t == "int1blue_3")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,blue)");
        add("face(blue,player)");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Are you ok down there, Doctor?");
        add("position(player,below)");
        add("speak_active");

        add("squeak(cry)");
        add("text(blue,0,0,1)");
        add("I wanna go home!");
        add("position(blue,above)");
        add("speak_active");

        add("squeak(blue)");
        add("text(blue,0,0,2)");
        add("Where are we? How did");
        add("we even get here?");
        add("position(blue,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,4)");
        add("Well, Violet did say that the");
        add("interference in the dimension");
        add("we crashed in was causing");
        add("problems with the teleporters...");
        add("position(player,below)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("I guess something went wrong...");
        add("position(player,below)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,3)");
        add("But if we can find another");
        add("teleporter, I think we can");
        add("get back to the ship!");
        add("position(player,below)");
        add("speak_active");

        add("squeak(blue)");
        add("text(blue,0,0,1)");
        add("Sniff...");
        add("position(blue,above)");
        add("speak_active");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "int1blue_4")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,blue)");
        add("face(blue,player)");

        add("squeak(cry)");
        add("text(blue,0,0,1)");
        add("Captain! Captain! Wait for me!");
        add("position(blue,above)");
        add("speak_active");

        add("squeak(blue)");
        add("text(blue,0,0,2)");
        add("Please don't leave me behind!");
        add("I don't mean to be a burden!");
        add("position(blue,above)");
        add("speak_active");

        add("squeak(cry)");
        add("text(blue,0,0,1)");
        add("I'm scared!");
        add("position(blue,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,2)");
        add("Oh... don't worry Victoria,");
        add("I'll look after you!");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "int1blue_5")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,blue)");
        add("face(blue,player)");

        add("squeak(cry)");
        add("text(blue,0,0,2)");
        add("We're never going to get");
        add("out of here, are we?");
        add("position(blue,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changemood(player,1)");
        add("text(player,0,0,1)");
        add("I.. I don't know...");
        add("position(player,above)");
        add("speak_active");

        add("squeak(cry)");
        add("text(player,0,0,2)");
        add("I don't know where we are or");
        add("how we're going to get out...");
        add("position(player,above)");
        add("speak_active");


        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "int1blue_6")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,blue)");
        add("face(blue,player)");

        add("squeak(cry)");
        add("text(blue,0,0,1)");
        add("We're going to be lost forever!");
        add("position(blue,above)");
        add("speak_active");

        add("squeak(player)");
        add("changemood(player,0)");
        add("text(player,0,0,2)");
        add("Ok, come on... Things");
        add("aren't that bad.");
        add("position(player,above)");
        add("speak_active");


        add("squeak(player)");
        add("text(player,0,0,2)");
        add("I have a feeling that");
        add("we're nearly home!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,2)");
        add("We can't be too far");
        add("from another teleporter!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(cry)");
        add("text(blue,0,0,1)");
        add("I hope you're right, captain...");
        add("position(blue,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "int1blue_7")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,blue)");
        add("face(blue,player)");

        add("squeak(blue)");
        add("text(blue,0,0,2");
        add("Captain! You were right!");
        add("It's a teleporter!");
        add("position(blue,above)");
        add("speak_active");

        add("squeak(player)");
        add("changemood(player,0)");
        add("text(player,0,0,3)");
        add("Phew! You had me worried for a");
        add("while there... I thought we");
        add("were never going to find one.");
        add("position(player,above)");
        add("speak_active");


        add("squeak(cry)");
        add("changemood(blue,1)");
        add("text(blue,0,0,1");
        add("What? Really?");
        add("position(blue,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,2)");
        add("Anyway, let's go");
        add("back to the ship.");
        add("position(player,above)");
        add("speak_active");

        add("changemood(blue,0)");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "int1green_1")
    {
        add("delay(45)");

        add("squeak(green)");
        add("text(green,0,0,1)");
        add("Huh? This isn't the ship...");
        add("position(green,above)");
        add("speak_active");
        add("face(player,green)");
        add("face(green,player)");

        add("squeak(green)");
        add("text(green,0,0,1)");
        add("Captain! What's going on?");
        add("position(green,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changemood(player,1");
        add("text(player,0,0,1)");
        add("I... I don't know!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Where are we?");
        add("position(player,above)");
        add("speak_active");

        add("squeak(green)");
        add("text(green,0,0,3)");
        add("Uh oh, this isn't good...");
        add("Something must have gone");
        add("wrong with the teleporter!");
        add("position(green,above)");
        add("speak_active");

        add("squeak(player)");
        add("changemood(player,0");
        add("text(player,0,0,1)");
        add("Ok... no need to panic!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("changemood(player,0");
        add("text(player,0,0,1)");
        add("Let's look for another teleporter!");
        add("There's bound to be one around");
        add("here somewhere!");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        add("telesave()");
        add("endcutscene()");
        add("untilbars()");
        add("gamestate(14)");
    }
    else if (t == "int1green_2")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Let's go this way!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(green)");
        add("text(green,0,0,1)");
        add("After you, Captain!");
        add("position(green,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("gamestate(11)");

    }
    else if (t == "int1green_3")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,green)");
        add("face(green,player)");

        add("squeak(green)");
        add("text(green,0,0,2)");
        add("So Violet's back on the");
        add("ship? She's really ok?");
        add("position(green,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,2)");
        add("She's fine! She helped");
        add("me find my way back!");
        add("position(player,below)");
        add("speak_active");

        add("squeak(green)");
        add("text(green,0,0,1)");
        add("Oh, phew! I was worried about her.");
        add("position(green,above)");
        add("speak_active");
        add("endtext");

        add("delay(45)");

        add("squeak(green)");
        add("text(green,0,0,1)");
        add("Captain, I have a secret...");
        add("position(green,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changemood(green,1)");
        add("text(green,0,0,1)");
        add("I really like Violet!");
        add("position(green,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Is that so?");
        add("position(player,below)");
        add("speak_active");

        add("squeak(green)");
        add("changemood(green,0)");
        add("text(green,0,0,2)");
        add("Please promise you");
        add("won't tell her!");
        add("position(green,above)");
        add("speak_active");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "int1green_4")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,green)");
        add("face(green,player)");

        add("squeak(green)");
        add("text(green,0,0,1)");
        add("Hey again!");
        add("position(green,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Hey!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Are you doing ok?");
        add("position(player,above)");
        add("speak_active");

        add("squeak(green)");
        add("text(green,0,0,3)");
        add("I think so! I really");
        add("hope we can find a way");
        add("back to the ship...");
        add("position(green,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "int1green_5")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,green)");
        add("face(green,player)");

        add("squeak(green)");
        add("text(green,0,0,1)");
        add("So, about Violet...");
        add("position(green,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Um, yeah?");
        add("position(player,above)");
        add("speak_active");

        add("squeak(green)");
        add("text(green,0,0,1)");
        add("Do you have any advice?");
        add("position(green,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Oh!");
        add("position(player,above)");
        add("speak_active");
        add("endtext");

        add("delay(45)");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Hmm...");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Um... you should... be yourself!");
        add("position(player,above)");
        add("speak_active");
        add("endtext");

        add("delay(15)");

        add("squeak(green)");
        add("text(green,0,0,1)");
        add("Oh.");
        add("position(green,above)");
        add("speak_active");
        add("endtext");

        add("delay(75)");

        add("squeak(green)");
        add("text(green,0,0,1)");
        add("Thanks Captain!");
        add("position(green,above)");
        add("speak_active");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "int1green_6")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,green)");
        add("face(green,player)");

        add("squeak(player)");
        add("text(player,0,0,2)");
        add("So, do you think you'll");
        add("be able to fix the ship?");
        add("position(player,above)");
        add("speak_active");

        add("squeak(green)");
        add("text(green,0,0,2)");
        add("Depends on how bad it ");
        add("is... I think so, though!");
        add("position(green,above)");
        add("speak_active");

        add("squeak(green)");
        add("text(green,0,0,5)");
        add("It's not very hard, really. The");
        add("basic dimensional warping engine");
        add("design is pretty simple, and if we");
        add("can get that working we shouldn't");
        add("have any trouble getting home.");
        add("position(green,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Oh! Good!");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "int1green_7")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,green)");
        add("face(green,player)");

        add("squeak(green)");
        add("text(green,0,0,1)");
        add("Finally! A teleporter!");
        add("position(green,above)");
        add("speak_active");

        add("squeak(green)");
        add("text(green,0,0,2)");
        add("I was getting worried");
        add("we wouldn't find one...");
        add("position(green,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Let's head back to the ship!");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "int1red_1")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(red)");
        add("text(red,0,0,1)");
        add("Wow! Where are we?");
        add("position(red,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changemood(player,1)");
        add("text(player,0,0,3)");
        add("This... isn't right...");
        add("Something must have gone");
        add("wrong with the teleporter!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,3)");
        add("Oh well... We can work");
        add("it out when we get");
        add("back to the ship!");
        add("position(red,above)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,1)");
        add("Let's go exploring!");
        add("position(red,above)");
        add("speak_active");

        add("squeak(player)");
        add("changemood(player,0)");
        add("text(player,0,0,1)");
        add("Ok then!");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        add("telesave()");
        add("endcutscene()");
        add("untilbars()");
        add("gamestate(14)");
    }
    else if (t == "int1red_2")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,red)");
        add("face(red,player)");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Follow me!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,1)");
        add("Aye aye, Captain!");
        add("position(red,above)");
        add("speak_active");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("gamestate(11)");
    }
    else if (t == "int1red_3")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,red)");
        add("face(red,player)");

        add("squeak(red)");
        add("text(red,0,0,2)");
        add("Hey Viridian... how did");
        add("the crash happen, exactly?");
        add("position(red,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,2)");
        add("Oh, I don't really know -");
        add("some sort of interference...");
        add("position(player,below)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,2)");
        add("...or something sciencey like");
        add("that. It's not really my area.");
        add("position(player,below)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,3)");
        add("Ah! Well, do you think");
        add("we'll be able to fix");
        add("the ship and go home?");
        add("position(red,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Of course! Everything will be ok!");
        add("position(player,below)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "int1red_4")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,red)");
        add("face(red,player)");

        add("squeak(red)");
        add("text(red,0,0,1)");
        add("Hi again! You doing ok?");
        add("position(red,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,2)");
        add("I think so! But I really want");
        add("to get back to the ship...");
        add("position(player,above)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,3)");
        add("We'll be ok! If we can find");
        add("a teleporter somewhere we");
        add("should be able to get back!");
        add("position(red,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "int1red_5")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,red)");
        add("face(red,player)");

        add("squeak(red)");
        add("text(red,0,0,1)");
        add("Are we there yet?");
        add("position(red,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("We're getting closer, I think...");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("I hope...");
        add("position(player,above)");
        add("speak_active");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "int1red_6")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,red)");
        add("face(red,player)");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("I wonder where we are, anyway?");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,3)");
        add("This seems different from");
        add("that dimension we crashed");
        add("in, somehow...");
        add("position(player,above)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,2)");
        add("I dunno... But we must be");
        add("close to a teleporter by now...");
        add("position(red,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "int1red_7")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,red)");
        add("face(red,player)");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("We're there!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,2)");
        add("See? I told you! Let's");
        add("get back to the ship!");
        add("position(red,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "int1yellow_1")
    {
        add("cutscene()");
        add("untilbars()");

        add("squeak(yellow)");
        add("text(yellow,0,0,1)");
        add("Oooh! This is interesting...");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,2)");
        add("Captain! Have you");
        add("been here before?");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changemood(player,1)");
        add("text(player,0,0,1)");
        add("What? Where are we?");
        add("position(player,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,3)");
        add("I suspect something deflected");
        add("our teleporter transmission!");
        add("This is somewhere new...");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changemood(player,1)");
        add("text(player,0,0,1)");
        add("Oh no!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("changemood(player,0)");
        add("text(player,0,0,3)");
        add("We should try to find a");
        add("teleporter and get back");
        add("to the ship...");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        add("telesave()");
        add("endcutscene()");
        add("untilbars()");
        add("gamestate(14)");
    }
    else if (t == "int1yellow_2")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,yellow)");
        add("face(yellow,player)");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Follow me!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,1)");
        add("Right behind you, Captain!");
        add("position(yellow,above)");
        add("speak_active");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("gamestate(11)");
    }
    else if (t == "int1yellow_3")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,yellow)");
        add("face(yellow,player)");

        add("squeak(player)");
        add("text(player,0,0,2)");
        add("What do you make of");
        add("all this, Professor?");
        add("position(player,below)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,4)");
        add("I'm guessing this dimension");
        add("has something to do with the");
        add("interference that caused");
        add("us to crash!");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,2)");
        add("Maybe we'll find the");
        add("cause of it here?");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Oh wow! Really?");
        add("position(player,below)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,4)");
        add("Well, it's just a guess.");
        add("I'll need to get back to");
        add("the ship before I can do");
        add("any real tests...");
        add("position(yellow,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "int1yellow_4")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,yellow)");
        add("face(yellow,player)");

        add("squeak(yellow)");
        add("text(yellow,0,0,1)");
        add("Ohh! What was that?");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("What was what?");
        add("position(player,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("changedir(yellow,0)");
        add("text(yellow,0,0,2)");
        add("That big... C thing!");
        add("I wonder what it does?");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changemood(player,1)");
        add("text(player,0,0,2)");
        add("Em... I don't really know");
        add("how to answer that question...");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("changemood(player,0)");
        add("text(player,0,0,3)");
        add("It's probably best not");
        add("to acknowledge that");
        add("it's there at all.");
        add("position(player,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("changedir(yellow,1)");
        add("text(yellow,0,0,2)");
        add("Maybe we should take it back");
        add("to the ship to study it?");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,3)");
        add("We really shouldn't think");
        add("about it too much... Let's");
        add("keep moving!");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "int1yellow_5")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,yellow)");
        add("face(yellow,player)");

        add("squeak(yellow)");
        add("text(yellow,0,0,3)");
        add("You know, there's");
        add("something really odd");
        add("about this dimension...");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Yeah?");
        add("position(player,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("changedir(yellow,0)");
        add("text(yellow,0,0,3)");
        add("We shouldn't really be able");
        add("to move between dimensions");
        add("with a regular teleporter...");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("changedir(yellow,0)");
        add("text(yellow,0,0,2)");
        add("Maybe this isn't a proper");
        add("dimension at all?");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("changedir(yellow,0)");
        add("text(yellow,0,0,4)");
        add("Maybe it's some kind of");
        add("polar dimension? Something");
        add("artificially created for");
        add("some reason?");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("changedir(yellow,1)");
        add("text(yellow,0,0,2)");
        add("I can't wait to get back to the");
        add("ship. I have a lot of tests to run!");
        add("position(yellow,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "int1yellow_6")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,yellow)");
        add("face(yellow,player)");

        add("squeak(yellow)");
        add("text(yellow,0,0,3)");
        add("I wonder if there's anything");
        add("else in this dimension");
        add("worth exploring?");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,3)");
        add("Maybe... but we should probably");
        add("just focus on finding the rest");
        add("of the crew for now...");
        add("position(player,above)");
        add("speak_active");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "int1yellow_7")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,yellow)");
        add("face(yellow,player)");

        add("squeak(yellow)");
        add("text(yellow,0,0,1)");
        add("At last!");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,1)");
        add("Let's go back to the ship!");
        add("position(yellow,above)");
        add("speak_active");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "skipint2")
    {
        add("finalmode(53,49)");
        add("gotoposition(228,129,0)");
        add("changedir(player,1)");
        add("setcheckpoint()");
        add("flash(5)");
        add("shake(20)");
        add("playef(10,10)");

        add("showplayer()");
        add("play(8)");

        add("hascontrol()");
        add("befadein()");
    }
    else if (t == "intermission_2")
    {
        add("ifskip(skipint2)");
        add("finalmode(53,49)");
        add("gotoposition(228,129,0)");
        add("changedir(player,1)");
        add("setcheckpoint()");

        add("cutscene()");

        add("delay(15)");

        add("flash(5)");
        add("shake(20)");
        add("playef(9,10)");

        add("delay(35)");

        add("flash(5)");
        add("shake(20)");
        add("playef(9,10)");

        add("delay(25)");

        add("flash(5)");
        add("shake(20)");
        add("playef(10,10)");

        add("showplayer()");
        add("play(8)");

        add("befadein()");

        add("changemood(player,1)");
        add("text(player,0,0,1)");
        add("Uh oh...");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("changemood(player,1)");
        add("text(player,0,0,1)");
        add("Not again!");
        add("position(player,above)");
        add("speak_active");

        add("iflast(2,int2intro_yellow)");
        add("iflast(3,int2intro_red)");
        add("iflast(4,int2intro_green)");
        add("iflast(5,int2intro_blue)");
    }
    else if (t == "int2intro_yellow")
    {
        add("squeak(cry)");
        add("text(player,0,0,1)");
        add("Vitellary? Where are you?");
        add("position(player,above)");
        add("speak_active");
        add("endtext");
        add("delay(15)");

        add("flash(5)");
        add("shake(20)");
        add("playef(10,10)");
        add("delay(15)");

        add("changedir(player,0)");

        add("createcrewman(150,-20,yellow,1,17,1)");

        add("squeak(cry)");
        add("text(yellow,170,50,1)");
        add("Captain!");
        add("speak_active");
        add("endtext");

        add("delay(15)");

        add("squeak(player)");
        add("changemood(player,0)");
        add("text(player,0,0,1)");
        add("Hang on! I'll save you!");
        add("position(player,above)");
        add("speak_active");

        add("endtext");
        add("telesave()");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "int2intro_red")
    {
        add("squeak(cry)");
        add("text(player,0,0,1)");
        add("Vermilion? Where are you?");
        add("position(player,above)");
        add("speak_active");
        add("endtext");
        add("delay(15)");

        add("flash(5)");
        add("shake(20)");
        add("playef(10,10)");
        add("delay(15)");

        add("changedir(player,0)");

        add("createcrewman(150,-20,red,0,17,1)");

        add("squeak(red)");
        add("text(red,170,50,1)");
        add("Wheeeee!");
        add("speak_active");
        add("endtext");

        add("delay(15)");

        add("squeak(player)");
        add("changemood(player,0)");
        add("text(player,0,0,1)");
        add("Hang on! I'll save you!");
        add("position(player,above)");
        add("speak_active");

        add("endtext");
        add("telesave()");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "int2intro_green")
    {
        add("squeak(cry)");
        add("text(player,0,0,1)");
        add("Verdigris? Where are you?");
        add("position(player,above)");
        add("speak_active");
        add("endtext");
        add("delay(15)");

        add("flash(5)");
        add("shake(20)");
        add("playef(10,10)");
        add("delay(15)");

        add("changedir(player,0)");

        add("createcrewman(150,-20,green,1,17,1)");

        add("squeak(cry)");
        add("text(green,170,50,1)");
        add("Aaagghh!");
        add("speak_active");
        add("endtext");

        add("delay(15)");

        add("squeak(player)");
        add("changemood(player,0)");
        add("text(player,0,0,1)");
        add("Hang on! I'll save you!");
        add("position(player,above)");
        add("speak_active");

        add("endtext");
        add("telesave()");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "int2intro_blue")
    {
        add("squeak(cry)");
        add("text(player,0,0,1)");
        add("Victoria? Where are you?");
        add("position(player,above)");
        add("speak_active");
        add("endtext");
        add("delay(15)");

        add("flash(5)");
        add("shake(20)");
        add("playef(10,10)");
        add("delay(15)");

        add("changedir(player,0)");

        add("createcrewman(150,-20,blue,1,17,1)");

        add("squeak(cry)");
        add("text(blue,170,50,1)");
        add("Help!");
        add("speak_active");
        add("endtext");

        add("delay(15)");

        add("squeak(player)");
        add("changemood(player,0)");
        add("text(player,0,0,1)");
        add("Hang on! I'll save you!");
        add("position(player,above)");
        add("speak_active");

        add("endtext");
        add("telesave()");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "int2_yellow")
    {
        add("ifskip(skipint2yellow)");
        add("cutscene()");

        add("tofloor()");
        add("changeai(yellow,followplayer)");
        add("untilbars()");

        add("squeak(yellow)");
        add("text(yellow,0,0,1)");
        add("That was interesting, wasn't it?");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changemood(player,1)");
        add("text(player,0,0,1)");
        add("I feel dizzy...");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        add("changemood(player,0)");

        add("endcutscene()");
        add("untilbars()");
        add("companion(10)");
    }
    else if (t == "skipint2yellow")
    {
        add("squeak(yellow)");
        add("companion(10)");
    }
    else if (t == "int2_red")
    {
        add("ifskip(skipint2red)");
        add("cutscene()");

        add("tofloor()");
        add("changeai(red,followplayer)");
        add("untilbars()");

        add("squeak(red)");
        add("text(red,0,0,1)");
        add("Again! Let's go again!");
        add("position(red,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changemood(player,1)");
        add("text(player,0,0,1)");
        add("I feel dizzy...");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        add("changemood(player,0)");

        add("endcutscene()");
        add("untilbars()");
        add("companion(10)");
    }
    else if (t == "skipint2red")
    {
        add("squeak(red)");
        add("companion(10)");
    }
    else if (t == "int2_green")
    {
        add("ifskip(skipint2green)");
        add("cutscene()");

        add("tofloor()");
        add("changeai(green,followplayer)");
        add("untilbars()");

        add("squeak(green)");
        add("text(green,0,0,1)");
        add("Phew! You're ok!");
        add("position(green,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changemood(player,1)");
        add("text(player,0,0,1)");
        add("I feel dizzy...");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        add("changemood(player,0)");

        add("endcutscene()");
        add("untilbars()");
        add("companion(10)");
    }
    else if (t == "skipint2green")
    {
        add("squeak(green)");
        add("companion(10)");
    }
    else if (t == "int2_blue")
    {
        add("ifskip(skipint2blue)");
        add("cutscene()");

        add("tofloor()");
        add("changeai(blue,followplayer)");
        add("untilbars()");

        add("squeak(cry)");
        add("text(blue,0,0,1)");
        add("I think I'm going to be sick...");
        add("position(blue,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changemood(player,1)");
        add("text(player,0,0,1)");
        add("I feel dizzy...");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        add("changemood(player,0)");
        add("changemood(blue,0)");

        add("endcutscene()");
        add("untilbars()");
        add("companion(10)");
    }
    else if (t == "skipint2blue")
    {
        add("squeak(blue)");
        add("companion(10)");
    }
    else if (t == "startexpolevel_station2")
    {
        //For the Eurogamer EXPO! Scrap later.
        add("fadeout()");
        add("musicfadeout()");
        add("untilfade()");
        add("delay(30)");

        add("resetgame");

        add("gotoroom(12,14)");
        add("gotoposition(126,38,1)");
        add("setcheckpoint()");
        add("changedir(player,0)");
        add("fadein()");

        add("stopmusic()");
        add("play(1)");
    }
    else if (t == "finallevel_teleporter")
    {
        add("delay(10)");

        add("squeak(purple)");
        add("text(purple,0,0,1)");
        add("Welcome back!");
        add("position(purple,above)");
        add("speak_active");
        add("endtext");

        add("delay(30)");

        add("squeak(purple)");
        add("text(purple,0,0,1)");
        add("...");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,1)");
        add("Um, where's Captain Viridian?");
        add("position(purple,above)");
        add("speak_active");
        add("endtext");

        add("delay(30)");

        add("walk(left,3)");
        add("delay(60)");

        add("everybodysad()");
        add("squeak(cry)");
        add("delay(30)");

        add("fadeout()");
        add("untilfade()");
        add("changemood(player,0)");
        add("musicfadeout()");

        add("finalmode(46,54)");
        add("gotoposition(101,113,0)");
        add("setcheckpoint()");
        add("changedir(player,1)");
        add("restoreplayercolour");
        add("fadein()");
        add("untilfade()");

        add("delay(15)");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("... Hello?");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Is anyone there?");
        add("position(player,above)");
        add("speak_active");
        add("endtext");

        add("missing(player)");

        //add("squeak(cry)");
        //add("changemood(player,1)");

        add("endcutscene()");
        add("untilbars()");

        add("play(15)");
        add("telesave()");
    }
    else if (t == "skipfinal")
    {
        add("finalmode(46,54)");
        add("gotoposition(101,113,0)");
        add("setcheckpoint()");
        add("changedir(player,1)");
        add("restoreplayercolour");

        add("showplayer()");
        add("hascontrol()");
        add("missing(player)");
        add("play(15)");
        add("fadein()");
        add("untilfade()");
    }
    else if (t == "startlevel_final")
    {
        add("ifskip(skipfinal)");
        add("cutscene()");
        add("untilbars()");
        add("activeteleporter()");

        add("stopmusic()");
        add("play(5)");

        add("gotoroom(2,11)");
        add("gotoposition(160,120,0)");

        add("createcrewman(190,153,purple,0,faceleft)");
        add("createrescuedcrew()");

        add("fadein()");
        add("untilfade()");
        add("gamestate(4070)");
    }
    else if (t == "regularreturn")
    {
        add("cutscene()");
        add("untilbars()");
        add("activeteleporter()");

        add("stopmusic()");
        add("play(4)");

        add("gotoroom(2,11)");
        add("gotoposition(160,120,0)");

        add("createlastrescued()");

        add("fadein()");
        add("untilfade()");
        add("endcutscene()");
        add("setcheckpoint()");
        add("gamestate(4010)");
    }
    else if (t == "returntohub")
    {
        //For the Eurogamer EXPO! Scrap later.
        add("fadeout()");
        add("musicfadeout()");
        add("untilfade()");
        add("delay(30)");

        add("resetgame");

        add("gotoroom(7,8)");
        add("gotoposition(145,145,0)");
        add("setcheckpoint()");
        add("changedir(player,0)");
        add("fadein()");

        add("stopmusic()");
        add("play(4)");
    }
    else if (t == "resetgame")
    {
        //For the Eurogamer EXPO! Scrap later.
        add("resetgame");
        add("gotoroom(4,6)");
        add("fadein()");
    }
    else if (t == "talkred")
    {
        add("redcontrol");
    }
    else if (t == "talkyellow")
    {
        add("yellowcontrol");
    }
    else if (t == "talkgreen")
    {
        add("greencontrol");
    }
    else if (t == "talkblue")
    {
        add("bluecontrol");
    }
    else if (t == "talkpurple")
    {
        add("purplecontrol");
    }
    else if (t == "talkred_1")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,red)");
        add("face(red,player)");

        add("squeak(red)");
        add("text(red,0,0,1)");
        add("Don't worry, Sir!");
        add("position(red,above)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,2)");
        add("We'll find a way");
        add("out of here!");
        add("position(red,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(red)");
    }
    else if (t == "talkred_2")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,red)");
        add("face(red,player)");

        add("squeak(red)");
        add("text(red,0,0,1)");
        add("I hope Victoria is ok...");
        add("position(red,above)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,2)");
        add("She doesn't handle");
        add("surprises very well...");
        add("position(red,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(red)");
    }
    else if (t == "talkred_3")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,red)");
        add("face(red,player)");

        add("squeak(red)");
        add("text(red,0,0,3)");
        add("I don't know how we're");
        add("going to get this ship");
        add("working again!");
        add("position(red,above)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,2)");
        add("Chief Verdigris would");
        add("know what to do...");
        add("position(red,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(red)");
    }
    else if (t == "talkred_4")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,red)");
        add("face(red,player)");

        add("squeak(red)");
        add("text(red,0,0,2)");
        add("I wonder what caused");
        add("the ship to crash here?");
        add("position(red,above)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,3)");
        add("It's the shame the Professor");
        add("isn't here, huh? I'm sure he");
        add("could work it out!");
        add("position(red,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(red)");
    }
    else if (t == "talkred_5")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,red)");
        add("face(red,player)");

        add("squeak(red)");
        add("text(red,0,0,1)");
        add("It's great to be back!");
        add("position(red,above)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,2)");
        add("I can't wait to help you");
        add("find the rest of the crew!");
        add("position(red,above)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,2)");
        add("It'll be like old");
        add("times, huh, Captain?");
        add("position(red,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(red)");
    }
    else if (t == "talkred_6")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,red)");
        add("face(red,player)");

        add("squeak(red)");
        add("text(red,0,0,2)");
        add("It's good to have");
        add("Victoria back with us.");
        add("position(red,above)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,2)");
        add("She really seems happy to");
        add("get back to work in her lab!");
        add("position(red,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(red)");
    }
    else if (t == "talkred_7")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,red)");
        add("face(red,player)");

        add("squeak(red)");
        add("text(red,0,0,3)");
        add("I think I saw Verdigris");
        add("working on the outside");
        add("of the ship!");
        add("position(red,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(red)");
    }
    else if (t == "talkred_8")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,red)");
        add("face(red,player)");

        add("squeak(red)");
        add("text(red,0,0,2)");
        add("You found Professor");
        add("Vitellary! All right!");
        add("position(red,above)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,2)");
        add("We'll have this interference");
        add("thing worked out in no time now!");
        add("position(red,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(red)");
    }
    else if (t == "talkred_9")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,red)");
        add("face(red,player)");

        add("squeak(red)");
        add("text(red,0,0,2)");
        add("That other dimension was");
        add("really strange, wasn't it?");
        add("position(red,above)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,2)");
        add("I wonder what caused the");
        add("teleporter to send us there?");
        add("position(red,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(red)");
    }
    else if (t == "talkred_10")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,red)");
        add("face(red,player)");

        add("squeak(red)");
        add("text(red,0,0,1)");
        add("Heya Captain!");
        add("position(red,above)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,2)");
        add("This way looks a little");
        add("dangerous...");
        add("position(red,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(red)");
    }
    else if (t == "talkred_11")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,red)");
        add("face(red,player)");

        add("squeak(red)");
        add("text(red,0,0,1)");
        add("I'm helping!");
        add("position(red,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(red)");
    }
    else if (t == "talkred_12")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,red)");
        add("face(red,player)");

        add("squeak(red)");
        add("text(red,0,0,1)");
        add("Hey Captain!");
        add("position(red,above)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,3)");
        add("I found something interesting");
        add("around here - the same warp");
        add("signature I saw when I landed!");
        add("position(red,above)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,2)");
        add("Someone from the ship");
        add("must be nearby...");
        add("position(red,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(red)");
    }
    else if (t == "talkred_13")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,red)");
        add("face(red,player)");

        add("squeak(red)");
        add("text(red,0,0,2)");
        add("This dimension is pretty");
        add("exciting, isn't it?");
        add("position(red,above)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,1)");
        add("I wonder what we'll find?");
        add("position(red,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(red)");
    }
    else if (t == "talkred_14")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,red)");
        add("face(red,player)");

        add("squeak(red)");
        add("text(red,0,0,1)");
        add("Look what I found!");
        add("position(red,above)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,2)");
        add("It's pretty hard, I can only");
        add("last for about 10 seconds...");
        add("position(red,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(red)");
    }
    else if (t == "talkyellow_1")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,yellow)");
        add("face(yellow,player)");

        add("squeak(yellow)");
        add("text(yellow,0,0,2)");
        add("I'm making some fascinating");
        add("discoveries, captain!");
        add("position(yellow,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(yellow)");
    }
    else if (t == "talkyellow_2")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,yellow)");
        add("face(yellow,player)");

        add("squeak(yellow)");
        add("text(yellow,0,0,3)");
        add("This isn't like any");
        add("other dimension we've");
        add("been to, Captain.");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,2)");
        add("There's something strange");
        add("about this place...");
        add("position(yellow,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(yellow)");
    }
    else if (t == "talkyellow_3")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,yellow)");
        add("face(yellow,player)");

        add("squeak(yellow)");
        add("text(yellow,0,0,3)");
        add("Captain, have you noticed");
        add("that this dimension seems");
        add("to wrap around?");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Yeah, it's strange...");
        add("position(player,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changemood(yellow,1)");
        add("text(yellow,0,0,3)");
        add("It looks like this dimension");
        add("is having the same stability");
        add("problems as our own!");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,2)");
        add("I hope we're not the");
        add("ones causing it...");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changemood(player,1)");
        add("text(player,0,0,1)");
        add("What? Do you think we might be?");
        add("position(player,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("changemood(yellow,0)");
        add("changemood(player,0)");
        add("text(yellow,0,0,2)");
        add("No no... that's very");
        add("unlikely, really...");
        add("position(yellow,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(yellow)");
    }
    else if (t == "talkyellow_4")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,yellow)");
        add("face(yellow,player)");

        add("squeak(yellow)");
        add("text(yellow,0,0,4)");
        add("My guess is that whoever used");
        add("to live here was experimenting");
        add("with ways to stop the dimension");
        add("from collapsing.");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,2)");
        add("It would explain why they've");
        add("wrapped the edges...");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,2)");
        add("Hey, maybe that's what's");
        add("causing the interference?");
        add("position(yellow,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(yellow)");
    }
    else if (t == "talkyellow_5")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,yellow)");
        add("face(yellow,player)");

        add("squeak(yellow)");
        add("text(yellow,0,0,2)");
        add("I wonder where the people who");
        add("used to live here have gone?");
        add("position(yellow,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(yellow)");
    }
    else if (t == "talkyellow_6")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,yellow)");
        add("face(yellow,player)");

        add("squeak(yellow)");
        add("text(yellow,0,0,3)");
        add("I think it's no coincidence");
        add("that the teleporter was drawn");
        add("to that dimension...");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,4)");
        add("There's something there. I");
        add("think it might be causing the");
        add("interference that's stopping");
        add("us from leaving...");
        add("position(yellow,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(yellow)");
    }
    else if (t == "talkyellow_7")
    {
        //Vertigris is back
        add("cutscene()");
        add("untilbars()");
        add("face(player,yellow)");
        add("face(yellow,player)");

        add("squeak(yellow)");
        add("text(yellow,0,0,1)");
        add("I'm glad Verdigris is alright.");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,3)");
        add("It'll be a lot easier to find");
        add("some way out of here now that");
        add("we can get the ship working again!");
        add("position(yellow,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(yellow)");
    }
    else if (t == "talkyellow_8")
    {
        //Victoria is back
        add("cutscene()");
        add("untilbars()");
        add("face(player,yellow)");
        add("face(yellow,player)");

        add("squeak(yellow)");
        add("text(yellow,0,0,2)");
        add("Ah, you've found Doctor");
        add("Victoria? Excellent!");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,1)");
        add("I have lots of questions for her!");
        add("position(yellow,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(yellow)");
    }
    else if (t == "talkyellow_9")
    {
        //Vermilion is back
        add("cutscene()");
        add("untilbars()");
        add("face(player,yellow)");
        add("face(yellow,player)");

        add("squeak(yellow)");
        add("text(yellow,0,0,3)");
        add("Vermilion says that he");
        add("was trapped in some");
        add("sort of tunnel?");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,2)");
        add("Yeah, it just seemed to");
        add("keep going and going...");
        add("position(player,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,2)");
        add("Interesting... I wonder");
        add("why it was built?");
        add("position(yellow,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(yellow)");
    }
    else if (t == "talkyellow_10")
    {
        //Back on the ship!
        add("cutscene()");
        add("untilbars()");
        add("face(player,yellow)");
        add("face(yellow,player)");

        add("squeak(yellow)");
        add("text(yellow,0,0,1)");
        add("It's good to be back!");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,2)");
        add("I've got so much work");
        add("to catch up on...");
        add("position(yellow,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(yellow)");
    }
    else if (t == "talkyellow_11")
    {
        //Game Complete
        add("cutscene()");
        add("untilbars()");
        add("face(player,yellow)");
        add("face(yellow,player)");

        add("squeak(yellow)");
        add("text(yellow,0,0,3)");
        add("I know it's probably a little");
        add("dangerous to stay here now that");
        add("this dimension is collapsing...");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,2)");
        add("...but it's so rare to find");
        add("somewhere this interesting!");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,2)");
        add("Maybe we'll find the answers");
        add("to our own problems here?");
        add("position(yellow,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(yellow)");
    }
    else if (t == "talkyellow_12")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,yellow)");
        add("face(yellow,player)");

        add("squeak(yellow)");
        add("text(yellow,0,0,1)");
        add("Captain! Have you seen this?");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,3)");
        add("With their research and ours,");
        add("we should be able to stabilise");
        add("our own dimension!");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,1)");
        add("We're saved!");
        add("position(yellow,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(yellow)");
    }
    else if (t == "talkgreen_1")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,green)");
        add("face(green,player)");

        add("squeak(green)");
        add("text(green,0,0,1)");
        add("I'm an engineer!");
        add("position(green,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(green)");
    }
    else if (t == "talkgreen_2")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,green)");
        add("face(green,player)");

        add("squeak(green)");
        add("text(green,0,0,3)");
        add("I think I can get this ship");
        add("moving again, but it's going");
        add("to take a while...");
        add("position(green,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(green)");
    }
    else if (t == "talkgreen_2")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,green)");
        add("face(green,player)");

        add("squeak(green)");
        add("text(green,0,0,3)");
        add("I think I can get this ship");
        add("moving again, but it's going");
        add("to take a while...");
        add("position(green,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(green)");
    }
    else if (t == "talkgreen_3")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,green)");
        add("face(green,player)");

        add("squeak(green)");
        add("text(green,0,0,3)");
        add("Victoria mentioned something");
        add("about a lab? I wonder if she");
        add("found anything down there?");
        add("position(green,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(green)");
    }
    else if (t == "talkgreen_4")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,green)");
        add("face(green,player)");

        add("squeak(green)");
        add("text(green,0,0,1)");
        add("Vermilion's back! Yey!");
        add("position(green,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(green)");
    }
    else if (t == "talkgreen_5")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,green)");
        add("face(green,player)");

        add("squeak(green)");
        add("text(green,0,0,3)");
        add("The Professor had lots of");
        add("questions about this");
        add("dimension for me...");
        add("position(green,above)");
        add("speak_active");

        add("squeak(green)");
        add("text(green,0,0,2)");
        add("We still don't really know");
        add("that much, though.");
        add("position(green,above)");
        add("speak_active");

        add("squeak(green)");
        add("text(green,0,0,3)");
        add("Until we work out what's");
        add("causing that interference,");
        add("we can't go anywhere.");
        add("position(green,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(green)");
    }
    else if (t == "talkgreen_6")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,green)");
        add("face(green,player)");

        add("squeak(green)");
        add("text(green,0,0,2)");
        add("I'm so glad that");
        add("Violet's alright!");
        add("position(green,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(green)");
    }
    else if (t == "talkgreen_7")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,green)");
        add("face(green,player)");

        add("squeak(green)");
        add("text(green,0,0,3)");
        add("That other dimension we ended");
        add("up in must be related to this");
        add("one, somehow...");
        add("position(green,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(green)");
    }
    else if (t == "talkgreen_8")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,green)");
        add("face(green,player)");

        add("squeak(cry)");
        add("text(green,0,0,3)");
        add("The antenna's broken!");
        add("This is going to be");
        add("very hard to fix...");
        add("position(green,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(green)");
    }
    else if (t == "talkgreen_9")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,green)");
        add("face(green,player)");

        add("squeak(green)");
        add("text(green,0,0,2)");
        add("It looks like we were warped");
        add("into solid rock when we crashed!");
        add("position(green,above)");
        add("speak_active");

        add("squeak(green)");
        add("text(green,0,0,2)");
        add("Hmm. It's going to be hard");
        add("to separate from this...");
        add("position(green,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(green)");
    }
    else if (t == "talkgreen_10")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,green)");
        add("face(green,player)");

        add("squeak(green)");
        add("text(green,0,0,2)");
        add("The ship's all fixed up. We");
        add("can leave at a moment's notice!");
        add("position(green,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(green)");
    }
    else if (t == "talkgreen_11")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,green)");
        add("face(green,player)");

        add("squeak(green)");
        add("text(green,0,0,3)");
        add("I wonder why they abandoned this");
        add("dimension? They were so close to");
        add("working out how to fix it...");
        add("position(green,above)");
        add("speak_active");

        add("squeak(green)");
        add("text(green,0,0,2)");
        add("Maybe we can fix it for them?");
        add("Maybe they'll come back?");
        add("position(green,above)");
        add("speak_active");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(green)");

    }


    if (t == "talkpurple_1")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,purple)");
        add("face(purple,player)");

        add("squeak(cry)");
        add("changemood(purple,1)");
        add("text(purple,0,0,1)");
        add("... I hope Verdigris is alright.");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(purple)");
        add("changemood(purple,0)");
        add("text(purple,0,0,2)");
        add("If you can find him, he'd be a");
        add("a big help fixing the ship!");
        add("position(purple,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(purple)");
    }
    else if (t == "talkpurple_2")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,purple)");
        add("face(purple,player)");

        add("squeak(purple)");
        add("text(purple,0,0,2)");
        add("Chief Verdigris is so brave");
        add("and ever so smart!");
        add("position(purple,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(purple)");
    }
    else if (t == "talkpurple_3")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,purple)");
        add("face(purple,player)");

        add("squeak(purple)");
        add("text(purple,0,0,1)");
        add("Are you doing ok, Captain?");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changemood(player,1)");
        add("text(player,0,0,0)");
        add("specialline(1)");
        add("position(player,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,2)");
        add("Oh - well, don't worry,");
        add("they'll show up!");
        add("position(purple,above)");
        add("speak_active");

        add("changemood(player,0)");
        add("squeak(purple)");
        add("text(purple,0,0,1)");
        add("Here! Have a lollipop!");
        add("position(purple,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(purple)");
    }
    else if (t == "talkpurple_4")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,purple)");
        add("face(purple,player)");

        add("squeak(purple)");
        add("text(purple,0,0,1)");
        add("Welcome back, Captain!");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,2)");
        add("I think Victoria is quite happy");
        add("to be back on the ship.");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,2)");
        add("She really doesn't like adventuring.");
        add("She gets very homesick!");
        add("position(purple,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(purple)");
    }
    else if (t == "talkpurple_5")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,purple)");
        add("face(purple,player)");

        add("squeak(purple)");
        add("text(purple,0,0,2)");
        add("Vermilion called in");
        add("to say hello!");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,1)");
        add("He's really looking forward");
        add("specialline(2)");
        add("position(purple,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(purple)");
    }
    else if (t == "talkpurple_6")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,purple)");
        add("face(purple,player)");

        add("squeak(purple)");
        add("text(purple,0,0,1)");
        add("Captain! You found Verdigris!");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,1)");
        add("Thank you so much!");
        add("position(purple,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(purple)");
    }
    else if (t == "talkpurple_7")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,purple)");
        add("face(purple,player)");

        add("squeak(purple)");
        add("text(purple,0,0,2)");
        add("I'm glad Professor");
        add("Vitellary is ok!");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,2)");
        add("He had lots of questions");
        add("for me about this dimension.");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,2)");
        add("He's already gotten to");
        add("work with his research!");
        add("position(purple,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(purple)");
    }
    else if (t == "talkpurple_8")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,purple)");
        add("face(purple,player)");

        add("squeak(purple)");
        add("text(purple,0,0,4)");
        add("Hey Captain! Now that you've turned");
        add("off the source of the interference,");
        add("we can warp everyone back to the");
        add("ship instantly, if we need to!");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,3)");
        add("Any time you want to come back");
        add("to the ship, just select the");
        add("new SHIP option in your menu!");
        add("position(purple,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(purple)");
    }
    else if (t == "talkpurple_9")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,purple)");
        add("face(purple,player)");

        add("squeak(purple)");
        add("text(purple,0,0,3)");
        add("Look at all this research!");
        add("This is going to be a big");
        add("help back home!");
        add("position(purple,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(purple)");
    }
    else if (t == "talkpurple_intermission1")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,purple)");
        add("face(purple,player)");

        add("squeak(player)");
        add("text(player,0,0,3)");
        add("Doctor, something strange");
        add("happened when we teleported");
        add("back to the ship...");
        add("position(player,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changemood(player,1)");
        add("text(player,0,0,1)");
        add("We got lost in another dimension!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changemood(purple,1)");
        add("text(purple,0,0,1)");
        add("Oh no!");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(purple)");
        add("changemood(purple,0)");
        add("changemood(player,0)");
        add("text(purple,0,0,3)");
        add("Maybe that dimension has something");
        add("to do with the interference that");
        add("caused us to crash here?");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,1)");
        add("I'll look into it...");
        add("position(purple,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(purple)");
    }
    else if (t == "talkpurple_intermission2")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,purple)");
        add("face(purple,player)");

        add("squeak(cry)");
        add("changemood(player,1)");
        add("text(player,0,0,1)");
        add("Doctor! Doctor! It happened again!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,2)");
        add("The teleporter brought us");
        add("to that weird dimension...");
        add("position(player,above)");
        add("speak_active");

        add("squeak(purple)");
        add("changemood(player,0)");
        add("changemood(purple,0)");
        add("text(purple,0,0,2)");
        add("Hmm, there's definitely");
        add("something strange happening...");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,2)");
        add("If only we could find the");
        add("source of that interference!");
        add("position(purple,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(purple)");
    }
    else if (t == "talkpurple_intermission3")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,purple)");
        add("face(purple,player)");

        add("squeak(player)");
        add("text(player,0,0,3)");
        add("Doctor, something strange has");
        add("been happening when we teleport");
        add("back to the ship...");
        add("position(player,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changemood(player,1)");
        add("text(player,0,0,2)");
        add("We keep getting brought to");
        add("another weird dimension!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changemood(purple,1)");
        add("text(purple,0,0,1)");
        add("Oh no!");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(purple)");
        add("changemood(purple,0)");
        add("changemood(player,0)");
        add("text(purple,0,0,3)");
        add("Maybe that dimension has something");
        add("to do with the interference that");
        add("caused us to crash here?");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(purple)");
        add("changemood(player,0)");
        add("changemood(purple,0)");
        add("text(purple,0,0,2)");
        add("Hmm, there's definitely");
        add("something strange happening...");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,2)");
        add("If only we could find the");
        add("source of that interference!");
        add("position(purple,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(purple)");
    }
    else if (t == "talkpurple_intro")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,purple)");
        add("face(purple,player)");

        add("squeak(cry)");
        add("changemood(player,1)");
        add("text(player,0,0,2)");
        add("I'm feeling a bit");
        add("overwhelmed, Doctor.");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Where do I begin?");
        add("position(player,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,2)");
        add("Remember that you can press ENTER");
        add("to check where you are on the map!");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,2)");
        add("Look for areas where the rest");
        add("of the crew might be...");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,2)");
        add("If you get lost, you can get back");
        add("to the ship from any teleporter.");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,2)");
        add("And don't worry!");
        add("We'll find everyone!");
        add("position(purple,above)");
        add("speak_active");

        add("endtext");

        add("delay(30)");

        add("changemood(player,0)");
        add("squeak(purple)");
        add("text(purple,0,0,1)");
        add("Everything will be ok!");
        add("position(purple,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(purple)");
    }
    else if (t == "talkblue_1")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,blue)");
        add("face(blue,player)");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,1)");
        add("Any signs of Professor Vitellary?");
        add("position(blue,below)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Sorry, not yet...");
        add("position(player,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changetile(blue,150)"); //upside down frown :(
        add("text(blue,0,0,1)");
        add("I hope he's ok...");
        add("position(blue,below)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(blue)");
    }
    else if (t == "talkblue_2")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,blue)");
        add("face(blue,player)");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,2)");
        add("Thanks so much for");
        add("saving me, Captain!");
        add("position(blue,below)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(blue)");
    }
    else if (t == "talkblue_3")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,blue)");
        add("face(blue,player)");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,1)");
        add("I'm so glad to be back!");
        add("position(blue,below)");
        add("speak_active");

        add("squeak(cry)");
        add("changetile(blue,150)"); //upside down frown :(
        add("text(blue,0,0,3)");
        add("That lab was so dark");
        add("and scary! I didn't");
        add("like it at all...");
        add("position(blue,below)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(blue)");
    }
    else if (t == "talkblue_4")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,blue)");
        add("face(blue,player)");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,2)");
        add("Vitellary's back? I");
        add("knew you'd find him!");
        add("position(blue,below)");
        add("speak_active");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,2)");
        add("I mean, I admit I was very");
        add("worried that you wouldn't...");
        add("position(blue,below)");
        add("speak_active");

        add("squeak(cry)");
        add("changetile(blue,150)"); //upside down frown :(
        add("text(blue,0,0,2)");
        add("or that something might");
        add("have happened to him...");
        add("position(blue,below)");
        add("speak_active");

        add("squeak(blue)");
        add("text(blue,0,0,1)");
        add("sniff...");
        add("position(blue,below)");
        add("speak_active");

        add("endtext");

        add("delay(30)");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Doctor Victoria? He's ok!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changetile(blue,150)"); //upside down frown :(
        add("text(blue,0,0,3)");
        add("Oh! Sorry! I was just");
        add("thinking about what");
        add("if he wasn't?");
        add("position(blue,below)");
        add("speak_active");

        add("endtext");

        add("delay(30)");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,1)");
        add("Thank you, Captain!");
        add("position(blue,below)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(blue)");
    }
    else if (t == "talkblue_5")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,blue)");
        add("face(blue,player)");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,1)");
        add("You found Vermilion! Great!");
        add("position(blue,below)");
        add("speak_active");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,2)");
        add("I wish he wasn't");
        add("so reckless!");
        add("position(blue,below)");
        add("speak_active");

        add("squeak(cry)");
        add("changetile(blue,150)"); //upside down frown :(
        add("text(blue,0,0,2)");
        add("He'll get himself");
        add("into trouble...");
        add("position(blue,below)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(blue)");
    }
    else if (t == "talkblue_6")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,blue)");
        add("face(blue,player)");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,2)");
        add("Verdigris is ok! Violet");
        add("will be so happy!");
        add("position(blue,below)");
        add("speak_active");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,1)");
        add("I'm happy!");
        add("position(blue,below)");
        add("speak_active");

        add("endtext");

        add("delay(30)");

        add("squeak(cry)");
        add("changetile(blue,150)"); //upside down frown :(
        add("text(blue,0,0,1)");
        add("Though I was very worried...");
        add("position(blue,below)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(blue)");
    }
    else if (t == "talkblue_7")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,blue)");
        add("face(blue,player)");

        add("squeak(cry)");
        add("changetile(blue,150)"); //upside down frown :(
        add("text(blue,0,0,2)");
        add("Why did the teleporter send");
        add("us to that scary dimension?");
        add("position(blue,below)");
        add("speak_active");

        add("squeak(blue)");
        add("changetile(blue,150)"); //upside down frown :(
        add("text(blue,0,0,1)");
        add("What happened?");
        add("position(blue,below)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("I don't know, Doctor...");
        add("position(player,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changetile(blue,150)"); //upside down frown :(
        add("text(blue,0,0,1)");
        add("Why?");
        add("position(blue,below)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(blue)");
    }
    else if (t == "talkblue_8")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,blue)");
        add("face(blue,player)");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,1)");
        add("Heya Captain!");
        add("position(blue,below)");
        add("speak_active");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,3)");
        add("Are you going to try");
        add("and find the rest of");
        add("these shiny things?");
        add("position(blue,below)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(blue)");
    }
    else if (t == "talkblue_9")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,blue)");
        add("face(blue,player)");

        add("squeak(blue)");
        add("text(blue,0,0,3)");
        add("This lab is amazing! The scientists");
        add("who worked here know a lot more");
        add("about warp technology than we do!");
        add("position(blue,below)");
        add("speak_active");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(blue)");
    }
    else if (t == "talkblue_trinket1")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,blue)");
        add("face(blue,player)");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,2)");
        add("Hey Captain, I found");
        add("this in that lab...");
        add("position(blue,below)");
        add("speak_active");

        add("endtext");
        add("delay(30)");

        //found a trinket!
        add("foundtrinket(18)");
        add("endtext");
        //add("musicfadein");
        add("trinketscriptmusic");

        add("delay(30)");
        add("createentity(136,80,22,18,0)");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,1)");
        add("Any idea what it does?");
        add("position(blue,below)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Sorry, I don't know!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("They seem important, though...");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,2)");
        add("Maybe something will happen");
        add("if we find them all?");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(blue)");
    }
    else if (t == "talkblue_trinket2")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,blue)");
        add("face(blue,player)");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,3)");
        add("Captain! Come have a");
        add("look at what I've");
        add("been working on!");
        add("position(blue,below)");
        add("speak_active");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,3)");
        add("It looks like these shiny");
        add("things are giving off a");
        add("strange energy reading!");
        add("position(blue,below)");
        add("speak_active");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,1)");
        add("So I analysed it...");
        add("position(blue,below)");
        add("speak_active");

        add("trinketbluecontrol()");
    }
    else if (t == "talkblue_trinket3")
    {
        //If you missed the first conversation
        add("cutscene()");
        add("untilbars()");
        add("face(player,blue)");
        add("face(blue,player)");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,3)");
        add("Captain! Come have a");
        add("look at what I've");
        add("been working on!");
        add("position(blue,below)");
        add("speak_active");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,1)");
        add("I found this in that lab...");
        add("position(blue,below)");
        add("speak_active");

        add("endtext");
        add("delay(30)");

        //found a trinket!
        add("foundtrinket(18)");
        add("endtext");
        //add("musicfadein");
        add("trinketscriptmusic");

        add("delay(30)");
        add("createentity(136,80,22,18,0)");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,3)");
        add("It seemed to be");
        add("giving off a weird");
        add("energy reading...");
        add("position(blue,below)");
        add("speak_active");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,1)");
        add("So I analysed it...");
        add("position(blue,below)");
        add("speak_active");

        add("trinketbluecontrol()");
    }
    else if (t == "talkblue_trinket4")
    {
        add("hidetrinkets()");
        add("endtextfast");
        add("delay(10)");

        //add map mode here and wrap up...
        add("gamemode(teleporter)");
        add("delay(20)");

        add("squeak(blue)");
        add("text(blue,50,15,2)");
        add("...and I was able to find more");
        add("of them with the ship's scanner!");
        add("speak_active");

        add("endtext");

        add("squeak(terminal)");
        add("showtrinkets()");
        add("delay(10)");
        add("hidetrinkets()");
        add("delay(10)");
        add("showtrinkets()");
        add("delay(10)");
        add("hidetrinkets()");
        add("delay(10)");
        add("showtrinkets()");
        add("delay(75)");

        add("gamemode(game)");
        add("delay(20)");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,3)");
        add("If you get a chance, it");
        add("might be worth finding");
        add("the rest of them!");
        add("position(blue,below)");
        add("speak_active");

        add("squeak(cry)");
        add("changetile(blue,150)"); //upside down frown :(
        add("text(blue,0,0,2)");
        add("Don't put yourself in");
        add("any danger, though!");
        add("position(blue,below)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(blue)");
    }
    else if (t == "talkblue_trinket5")
    {
        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,3)");
        add("...but it looks like you've");
        add("already found all of them");
        add("in this dimension!");
        add("position(blue,below)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Oh? Really?");
        add("position(player,above)");
        add("speak_active");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,2)");
        add("Yeah, well done! That");
        add("can't have been easy!");
        add("position(blue,below)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(blue)");
    }
    else if (t == "talkblue_trinket6")
    {
        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,3)");
        add("...and they're related.");
        add("They're all a part of");
        add("something bigger!");
        add("position(blue,below)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Oh? Really?");
        add("position(player,above)");
        add("speak_active");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,4)");
        add("Yeah! There seem to be");
        add("twenty variations of");
        add("the fundamental energy");
        add("signature...");
        add("position(blue,below)");
        add("speak_active");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,1)");
        add("Wait...");
        add("position(blue,below)");
        add("speak_active");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,2)");
        add("Does that mean you've");
        add("found all of them?");
        add("position(blue,below)");
        add("speak_active");

        add("endtext");

        add("loadscript(startepilogue)");
    }
    else if (t == "talkyellow_trinket1")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,yellow)");
        add("face(yellow,player)");

        add("squeak(yellow)");
        add("text(yellow,0,0,2)");
        add("Captain! I've been meaning");
        add("to give this to you...");
        add("position(yellow,above)");
        add("speak_active");
        add("endtext");

        add("delay(30)");

        //found a trinket!
        add("foundtrinket(18)");
        add("endtext");
        //add("musicfadein");
        add("trinketscriptmusic");

        add("delay(30)");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Professor! Where did you find this?");
        add("position(player,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,2)");
        add("Oh, it was just lying");
        add("around that space station.");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changemood(yellow,1)");
        add("text(yellow,0,0,3)");
        add("It's a pity Doctor Victoria");
        add("isn't here, she loves studying");
        add("that sort of thing...");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Any idea what it does?");
        add("position(player,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("changemood(yellow,0)");
        add("text(yellow,0,0,2)");
        add("Nope! But it is giving off");
        add("a strange energy reading...");
        add("position(yellow,above)");
        add("speak_active");
        add("endtext");

        add("trinketyellowcontrol()");
    }
    else if (t == "talkyellow_trinket2")
    {
        add("hidetrinkets()");
        add("endtextfast");
        add("delay(10)");

        //add map mode here and wrap up...
        add("gamemode(teleporter)");
        add("delay(20)");

        add("squeak(yellow)");
        add("text(yellow,50,15,2)");
        add("...so I used the ship's scanner");
        add("to find more of them!");
        add("speak_active");

        add("endtext");

        add("squeak(terminal)");
        add("showtrinkets()");
        add("delay(10)");
        add("hidetrinkets()");
        add("delay(10)");
        add("showtrinkets()");
        add("delay(10)");
        add("hidetrinkets()");
        add("delay(10)");
        add("showtrinkets()");
        add("delay(75)");

        add("gamemode(game)");
        add("delay(20)");

        add("squeak(yellow)");
        add("changemood(yellow,0)");
        add("text(yellow,0,0,3)");
        add("...Please don't let them");
        add("distract you from finding");
        add("Victoria, though!");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,1)");
        add("I hope she's ok...");
        add("position(yellow,above)");
        add("speak_active");

        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(yellow)");
    }
    else if (t == "talkyellow_trinket3")
    {
        add("squeak(yellow)");
        add("changemood(yellow,0)");
        add("text(yellow,0,0,2)");
        add("Can't seem to detect any");
        add("more of them nearby, though.");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("changemood(yellow,0)");
        add("text(yellow,0,0,1)");
        add("Maybe you've found them all?");
        add("position(yellow,above)");
        add("speak_active");
        add("endtext");

        add("endcutscene()");
        add("untilbars()");
        add("createactivityzone(yellow)");
    }
    else if (t == "gamecomplete")
    {
        add("gotoroom(2,11)");
        add("gotoposition(160,120,0)");
        add("nocontrol()");
        add("createcrewman(185,153,purple,0,faceleft)");
        add("createcrewman(205,153,yellow,0,faceleft)");
        add("createcrewman(225,153,red,0,faceleft)");
        add("createcrewman(245,153,green,0,faceleft)");
        add("createcrewman(265,153,blue,1,faceleft)");

        add("cutscene()");
        add("untilbars()");

        add("delay(30)");
        add("rescued(player)");

        add("squeak(yellow)");
        add("text(yellow,0,0,1)");
        add("Any moment now...");
        add("position(yellow,above)");
        add("speak_active");
        add("endtext");
        add("nocontrol()");

        add("delay(60)");

        add("gamestate(4080)");
    }
    else if (t == "gamecomplete_ending")
    {
        add("delay(15)");
        add("changemood(blue,0)");
        add("play(10)");
        add("delay(45)");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Hello!");
        add("position(player,above)");
        add("speak_active");
        add("endtext");

        add("squeak(purple)");
        add("delay(1)");
        add("squeak(yellow)");
        add("delay(1)");
        add("squeak(red)");
        add("delay(1)");
        add("squeak(green)");

        add("text(purple,0,0,1)");
        add("Captain!        ");
        add("position(purple,above)");
        add("backgroundtext");
        add("speak");
        add("text(yellow,0,0,1)");
        add("Captain!      ");
        add("position(yellow,above)");
        add("backgroundtext");
        add("speak");
        add("text(red,0,0,1)");
        add("Captain!    ");
        add("position(red,above)");
        add("backgroundtext");
        add("speak");
        add("text(green,0,0,1)");
        add("Captain!  ");
        add("position(green,above)");
        add("backgroundtext");
        add("speak");
        add("text(blue,0,0,1)");
        add("Captain!");
        add("position(blue,above)");
        add("speak");

        add("endtextfast");

        add("squeak(blue)");
        add("text(blue,0,0,1)");
        add("You're alright!");
        add("position(blue,above)");
        add("speak_active");

        add("squeak(blue)");
        add("text(blue,0,0,1)");
        add("I knew you'd be ok!");
        add("position(blue,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,2)");
        add("We were very worried when");
        add("you didn't come back...");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(green)");
        add("text(green,0,0,3)");
        add("...but when you turned");
        add("off the source of");
        add("the interference...");
        add("position(green,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,3)");
        add("...we were able to");
        add("find you with the");
        add("ship's scanners...");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(red)");
        add("text(red,0,0,2)");
        add("...and teleport you");
        add("back on board!");
        add("position(red,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("That was lucky!");
        add("Thanks guys!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Thanks guys!");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        //Move to Vitellary's lab
        add("fadeout()");
        add("untilfade()");

        add("missing(purple)");
        add("missing(red)");
        add("missing(green)");
        add("missing(blue)");
        add("missing(yellow)");

        add("gotoroom(3,11)");
        add("gotoposition(117,105,0)");
        add("changedir(player,0)");
        add("createcrewman(75,105,yellow,0,faceright)");
        add("createcrewman(190,105,red,0,faceleft)");

        add("fadein()");
        add("untilfade()");

        add("squeak(yellow)");
        add("text(yellow,0,0,4)");
        add("...it looks like this");
        add("dimension is starting");
        add("to destabilise, just");
        add("like our own...");
        add("position(yellow,above)");
        add("speak_active");

        add("walk(right,3)");

        add("squeak(red)");
        add("text(red,0,0,3)");
        add("...we can stay and");
        add("explore for a little");
        add("longer, but...");
        add("position(red,above)");
        add("speak_active");

        add("walk(left,3)");

        add("squeak(yellow)");
        add("text(yellow,0,0,2)");
        add("...eventually, it'll");
        add("collapse completely.");
        add("position(yellow,above)");
        add("speak_active");

        add("endtext");

        //Move to Vertigris' lab
        add("fadeout()");
        add("untilfade()");

        add("gotoroom(3,10)");
        add("gotoposition(210,177,0)");
        add("changedir(player,1)");
        add("createcrewman(245,177,green,0,faceleft)");
        add("createcrewman(56,177,blue,0,faceright)");

        add("fadein()");
        add("untilfade()");

        add("squeak(green)");
        add("text(green,0,0,3)");
        add("There's no telling exactly");
        add("how long we have here. But");
        add("the ship's fixed, so...");
        add("position(green,above)");
        add("speak_active");

        add("walk(left,3)");

        add("squeak(blue)");
        add("text(blue,0,0,2)");
        add("...as soon as we're");
        add("ready, we can go home!");
        add("position(blue,above)");
        add("speak_active");

        add("endtext");

        //Move to the bridge!
        add("fadeout()");
        add("untilfade()");

        add("gotoroom(4,10)");
        add("gotoposition(227,113,0)");
        add("changedir(player,0)");
        add("createcrewman(140,177,purple,0,faceright)");
        add("createcrewman(115,177,yellow,0,faceright)");
        add("createcrewman(90,177,red,0,faceright)");
        add("createcrewman(65,177,green,0,faceright)");
        add("createcrewman(40,177,blue,0,faceright)");

        add("rescued(purple)");
        add("rescued(red)");
        add("rescued(green)");
        add("rescued(blue)");
        add("rescued(yellow)");

        add("fadein()");
        add("untilfade()");

        add("squeak(purple)");
        add("text(purple,0,0,1)");
        add("What now, Captain?");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,2)");
        add("Let's find a way to save");
        add("this dimension!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,2)");
        add("And a way to save our");
        add("home dimension too!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("The answer is out there, somewhere!");
        add("position(player,above)");
        add("speak_active");
        add("endtext");

        add("delay(30)");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Let's go!");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        add("fadeout()");
        add("untilfade()");
        add("rollcredits()");
    }
    else if (t == "startepilogue")
    {
        add("cutscene()");
        add("untilbars()");
        add("face(player,blue)");
        add("face(blue,player)");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,1)");
        add("Wow! You found all of them!");
        add("position(blue,below)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Really? Great!");
        add("position(player,above)");
        add("speak_active");

        add("squeak(blue)");
        add("changetile(blue,6)"); //smiling again! blue always needs to specify her mood
        add("text(blue,0,0,3)");
        add("I'll run some tests and");
        add("see if I can work out");
        add("what they're for...");
        add("position(blue,below)");
        add("speak_active");

        add("endtext");

        add("flash(5)");
        add("shake(20)");
        add("playef(9,10)");

        add("musicfadeout()");

        add("delay(30)");

        add("squeak(cry)");
        add("changemood(player,1)");
        add("changetile(blue,150)"); //upside down frown :(
        add("text(player,0,0,2)");
        add("That... that didn't");
        add("sound good...");
        add("position(player,above)");
        add("speak_active");

        add("endtext");

        add("delay(30)");

        add("flash(5)");
        add("shake(20)");
        add("playef(9,10)");
        add("alarmon");

        add("delay(30)");

        add("squeak(cry)");
        add("text(blue,0,0,1)");
        add("Run!");
        add("position(blue,below)");
        add("speak_active");
        add("endtext");

        add("delay(5)");

        add("missing(green)");
        add("missing(yellow)");

        add("flash(5)");
        add("shake(50)");
        add("playef(9,10)");
        add("gotoroom(3,10)");
        add("gotoposition(40,177,0)");
        add("createcrewman(208,177,green,1,followposition,120)");
        add("createcrewman(240,177,purple,1,followposition,120)");
        add("createcrewman(10,177,blue,1,followposition,180)");

        add("squeak(player)");
        add("text(player,80,150,1)");
        add("Oh no!");
        add("backgroundtext");
        add("speak_active");
        add("walk(right,20)");

        add("endtextfast");

        //and the next!
        add("flash(5)");
        add("shake(50)");
        add("playef(9,10)");
        add("gotoroom(3,11)");
        add("gotoposition(140,0,0)");

        add("createcrewman(90,105,green,1,followblue)");
        add("createcrewman(125,105,purple,1,followgreen)");
        add("createcrewman(55,105,blue,1,followposition,-200)");

        add("createcrewman(120,177,yellow,1,followposition,-200)");
        add("createcrewman(240,177,red,1,faceleft)");

        add("delay(5)");
        add("changeai(red,followposition,-200)");

        add("squeak(red)");
        add("text(red,100,150,1)");
        add("Not again!");
        add("backgroundtext");
        add("speak_active");

        add("walk(left,25)");

        add("endtextfast");

        //final room:
        add("flash(5)");
        add("alarmoff");
        add("playef(9,10)");
        add("gotoroom(2,11)");
        add("gotoposition(265,153,0)");

        add("createcrewman(130,153,blue,1,faceleft)");
        add("createcrewman(155,153,green,1,faceleft)");
        add("createcrewman(180,153,purple,1,faceleft)");
        add("createcrewman(205,153,yellow,1,faceleft)");
        add("createcrewman(230,153,red,1,faceleft)");

        add("delay(75)");

        add("squeak(player)");
        add("changemood(player,0)");
        add("text(player,0,0,1)");
        add("Wait! It's stopped!");
        add("position(player,above)");
        add("speak_active");
        add("endtext");

        add("delay(30)");
        add("changemood(purple,0)");
        add("changedir(purple,1)");
        add("changemood(red,0)");
        add("changedir(red,1)");
        add("changemood(green,0)");
        add("changedir(green,1)");
        add("changemood(blue,0)");
        add("changedir(blue,1)");
        add("changemood(yellow,0)");
        add("changedir(yellow,1)");
        add("delay(30)");

        add("rescued(green)");
        add("rescued(yellow)");
        add("missing(blue)");
        add("altstates(1)");

        add("fadeout()");
        add("untilfade()");

        add("gotoroom(2,10)");
        add("gotoposition(227,113,0)");
        add("changedir(player,0)");

        add("rescued(blue)");

        add("createcrewman(150,177,purple,0,faceleft)");
        add("createcrewman(90,177,yellow,0,faceright)");
        add("createcrewman(184,185,red,0,faceleft)");
        add("createcrewman(65,177,green,0,faceright)");
        add("createcrewman(35,177,blue,0,faceright)");

        add("rescued(purple)");
        add("rescued(red)");
        add("rescued(green)");
        add("rescued(yellow)");

        add("fadein()");
        add("untilfade()");

        add("delay(30)");

        add("squeak(purple)");
        add("text(purple,0,0,3)");
        add("This is where we were");
        add("storing those shiny");
        add("things? What happened?");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(player)");
        add("text(player,0,0,2)");
        add("We were just playing");
        add("with them, and...");
        add("position(player,above)");
        add("speak_active");
        add("endtext");

        add("squeak(cry)");
        add("changemood(player,1)");
        add("text(player,0,0,1)");
        add("...they suddenly exploded!");
        add("position(player,above)");
        add("speak_active");
        add("endtext");

        add("squeak(blue)");
        add("text(blue,0,0,2)");
        add("But look what they made!");
        add("Is that a teleporter?");
        add("position(blue,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,1)");
        add("I think so, but...");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(yellow)");
        add("text(yellow,0,0,2)");
        add("I've never seen a teleporter");
        add("like that before...");
        add("position(yellow,above)");
        add("speak_active");
        add("endtext");

        add("changemood(player,0)");

        add("delay(30)");

        add("squeak(red)");
        add("text(red,0,0,1)");
        add("We should investigate!");
        add("position(red,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,1)");
        add("What do you think, Captain?");
        add("position(purple,above)");
        add("speak_active");

        add("squeak(purple)");
        add("text(purple,0,0,2)");
        add("Should we find out");
        add("where it leads?");
        add("position(purple,above)");
        add("speak_active");
        add("endtext");

        add("delay(15)");

        add("squeak(player)");
        add("text(player,0,0,1)");
        add("Let's go!");
        add("position(player,above)");
        add("speak_active");
        add("endtext");

        add("walk(left,10)");
        add("flip");
        add("walk(left,5)");

        add("flash(5)");
        add("shake(20)");
        add("playef(10,10)");
        add("blackout()");

        add("delay(45)");

        add("gotoroom(17,6)");
        add("gotoposition(80,109,1)");
        add("changedir(player,1)");
        add("flash(5)");
        add("shake(20)");
        add("playef(10,10)");
        add("blackon()");

        add("delay(15)");
        add("flash(5)");
        add("shake(20)");
        add("playef(10,10)");
        add("createcrewman(28,65,purple,0,faceright)");

        add("delay(15)");
        add("flash(5)");
        add("shake(20)");
        add("playef(10,10)");
        add("createcrewman(145,169,yellow,0,faceleft)");

        add("delay(15)");
        add("flash(5)");
        add("shake(20)");
        add("playef(10,10)");
        add("createcrewman(32,169,red,0,faceright)");

        add("delay(15)");
        add("flash(5)");
        add("shake(20)");
        add("playef(10,10)");
        add("createcrewman(96,149,green,0,faceleft)");

        add("delay(15)");
        add("flash(5)");
        add("shake(20)");
        add("playef(10,10)");
        add("createcrewman(155,57,blue,0,faceleft)");

        add("delay(45)");


        add("squeak(cry)");
        add("changemood(blue,1)");
        add("text(blue,0,0,1)");
        add("Oh no! We're trapped!");
        add("position(blue,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changemood(yellow,1)");
        add("text(yellow,0,0,1)");
        add("Oh dear...");
        add("position(yellow,above)");
        add("speak_active");

        add("squeak(cry)");
        add("changemood(red,1)");
        add("changemood(green,1)");
        add("changemood(purple,1)");
        add("changemood(player,1)");
        add("text(player,0,0,2)");
        add("Hmm... how should we");
        add("get out of this?");
        add("position(player,below)");
        add("speak_active");

        add("endtext");

        add("delay(70)");

        add("squeak(purple)");
        add("delay(1)");
        add("squeak(yellow)");
        add("delay(1)");
        add("squeak(red)");
        add("delay(1)");
        add("squeak(blue)");
        add("delay(1)");
        add("squeak(player)");
        add("delay(1)");
        add("squeak(green)");

        add("changemood(yellow,0)");
        add("changemood(blue,0)");
        add("changemood(red,0)");
        add("changemood(player,0)");
        add("changemood(green,0)");
        add("changemood(purple,0)");
        add("text(player,0,0,1)");
        add("COMBINE!");
        add("position(player,above)");
        add("backgroundtext");
        add("speak");
        add("text(purple,0,0,1)");
        add("COMBINE!");
        add("position(purple,above)");
        add("backgroundtext");
        add("speak");
        add("text(yellow,0,0,1)");
        add("COMBINE!");
        add("position(yellow,above)");
        add("backgroundtext");
        add("speak");
        add("text(red,0,0,1)");
        add("COMBINE!");
        add("position(red,above)");
        add("backgroundtext");
        add("speak");
        add("text(green,0,0,1)");
        add("COMBINE!");
        add("position(green,above)");
        add("backgroundtext");
        add("speak");
        add("text(blue,0,0,1)");
        add("COMBINE!");
        add("position(blue,above)");
        add("speak");

        add("endtextfast");

        add("delay(15)");
        add("flip");
        add("changeai(purple,followplayer)");
        add("changeai(blue,followplayer)");
        add("changeai(red,followplayer)");
        add("changeai(yellow,followplayer)");
        add("changeai(green,followplayer)");
        add("walk(right,3)");

        add("delay(5)");
        add("flash(10)");
        add("shake(20)");
        add("playef(24,10)");
        add("gotoroom(17,6)");
        add("vvvvvvman()");


        add("delay(90)");


        add("walk(right,6)");
        add("flash(10)");
        add("shake(20)");
        add("playef(23,10)");
        add("altstates(2)");
        add("gotoroom(17,6)");

        add("delay(20)");

        add("walk(right,12)");
        add("flash(10)");
        add("shake(20)");
        add("playef(23,10)");
        add("altstates(0)");
        add("gotoroom(17,6)");

        add("delay(20)");

        add("walk(right,15)");

        add("gotoroom(18,6)");
        add("gotoposition(0,46,0)");
        add("walk(right,5)");

        add("delay(20)");
        add("flash(10)");
        add("shake(20)");
        add("playef(24,10)");
        add("undovvvvvvman()");
        add("createcrewman(30,99,purple,0,faceright)");
        add("createcrewman(65,119,yellow,0,faceright)");
        add("createcrewman(135,149,red,0,faceleft)");
        add("createcrewman(170,159,green,0,faceleft)");
        add("createcrewman(205,159,blue,0,faceleft)");


        add("delay(60)");


        add("changedir(yellow,0)");
        add("changedir(player,0)");
        add("delay(20)");
        add("squeak(purple)");
        add("text(purple,0,0,3)");
        add("Or, you know... we could");
        add("have just warped back");
        add("to the ship...");
        add("position(purple,above)");
        add("speak_active");

        add("endtext");

        add("delay(30)");

        add("changedir(purple,1)");
        add("changedir(yellow,1)");
        add("changedir(player,1)");
        add("changedir(red,1)");

        add("changedir(green,1)");
        add("squeak(green)");
        add("text(green,0,0,1)");
        add("Wow! What is this?");
        add("position(green,above)");
        add("speak_active");


        add("changedir(purple,1)");
        add("changedir(yellow,1)");
        add("changedir(player,0)");
        add("changedir(red,0)");
        add("changedir(green,0)");
        add("squeak(yellow)");
        add("text(yellow,0,0,1)");
        add("It looks like another laboratory!");
        add("position(yellow,above)");
        add("speak_active");

        add("changedir(purple,1)");
        add("changedir(yellow,1)");
        add("changedir(player,1)");
        add("squeak(red)");
        add("text(red,0,0,1)");
        add("Let's have a look around!");
        add("position(red,above)");
        add("speak_active");

        add("endtext");

        add("delay(20)");
        add("changeai(yellow,followposition,500)");
        add("changeai(purple,followposition,500)");
        add("changeai(blue,followposition,500)");
        add("changeai(red,followposition,500)");
        add("changeai(green,followposition,500)");

        add("delay(21)");
        add("changeai(yellow,faceright)");
        add("flipgravity(yellow)");
        add("playef(0,10)");
        add("delay(2)");
        add("changeai(purple,faceright)");
        add("flipgravity(purple)");
        add("playef(0,10)");

        add("delay(48)");

        add("foundlab");
        add("endtext");
        add("foundlab2");
        add("endtext");

        add("entersecretlab");
        add("play(11)");

        add("endcutscene()");
        add("untilbars()");
    }
    else if (t == "returntolab")
    {
        //To get back to the lab from the gravitron
        add("gotoroom(19,7)");
        add("gotoposition(132,137,0)");
        add("fadein()");

        add("setcheckpoint()");

        add("play(11)");
        add("endcutscene()");
        add("untilbars()");
    }
    else
    {
        loadother(t);
    }

}
#endif /* SCRIPTS_H */
