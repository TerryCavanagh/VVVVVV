public function titlelogic(key:KeyPoll, dwgfx:dwgraphicsclass, game:gameclass, obj:entityclass, help:helpclass, music:musicclass):void {
	//Misc
	//map.updatetowerglow();
	dwgfx.mobile_changebutton(dwgfx.BUTTON_BLANK);
	if(game.menustart && !game.showloadingnotice) {
		dwgfx.mobile_changeleftbutton(dwgfx.BUTTON_GAMECENTER);
	}else {
		dwgfx.mobile_changeleftbutton(dwgfx.BUTTON_BLANK);
	}
	dwgfx.mobile_hidearrows();
	
	help.updateglow();
	
	map.bypos -= 2;
	map.bscroll = -2;
	//if (map.ypos <= 0) { map.ypos = 0; map.bypos = 0; map.bscroll = 0; }
	//if (map.ypos >= 5368) { map.ypos = 5368; map.bypos = map.ypos / 2; } //700-29 * 8 = 5368
	
	if (game.menucountdown > 0) {
		game.menucountdown--;
		if (game.menucountdown == 0) {
			if (game.menudest == "mainmenu"){ music.play(6);
			}else if (game.menudest == "gameover2") { music.playef(11, 10);
			}else if (game.menudest == "timetrialcomplete3") { music.playef(3, 10);
			}
			game.createmenu(game.menudest);
		}
	}
}

public function maplogic(key:KeyPoll, dwgfx:dwgraphicsclass, game:gameclass, map:mapclass,
                          obj:entityclass, help:helpclass, music:musicclass):void {
	//Misc
	if (dwgfx.resumegamemode) {
		dwgfx.mobile_changebutton(dwgfx.BUTTON_MENU);
		dwgfx.mobile_changeleftbutton(dwgfx.BUTTON_BLANK);
		dwgfx.mobile_showarrows(game.controlstyle);
	}else {
		if (game.gamestate == game.TELEPORTERMODE) {
			dwgfx.mobile_changebutton(dwgfx.BUTTON_BLANK);
			dwgfx.mobile_changeleftbutton(dwgfx.BUTTON_BLANK);
		}else{
			dwgfx.mobile_changebutton(dwgfx.BUTTON_BACK);
			if (game.menupage == 0 || game.menupage >= 30) {
				dwgfx.mobile_changeleftbutton(dwgfx.BUTTON_CONTROLS);
			}else {
				dwgfx.mobile_changeleftbutton(dwgfx.BUTTON_BLANK);
			}
		}
		dwgfx.mobile_hidearrows();
	}
	help.updateglow();
}


public function gamecompletelogic(key:KeyPoll, dwgfx:dwgraphicsclass, game:gameclass, map:mapclass,
                                  obj:entityclass, help:helpclass, music:musicclass):void {
	//Misc	
	
	dwgfx.mobile_changebutton(dwgfx.BUTTON_BLANK);
	dwgfx.mobile_changeleftbutton(dwgfx.BUTTON_BLANK);
	dwgfx.mobile_hidearrows();
	
	map.updatetowerglow();
	help.updateglow(); 
	dwgfx.crewframe = 0; 
	
	map.tdrawback = true;
	
	game.creditposition--;
	if (game.creditposition <= -1650) {
		game.creditposition = -1650;
	  map.bscroll = 0;
	}else {
		map.bypos += 1; map.bscroll = +1;
	}
	
	if (dwgfx.fademode == 1) {
		//Fix some graphical things
		dwgfx.showcutscenebars = false;
		dwgfx.cutscenebarspos = 0;
		//Return to game
		game.gamestate = 7; dwgfx.fademode = 4;
	}
}

public function gamecompletelogic2(key:KeyPoll, dwgfx:dwgraphicsclass, game:gameclass, map:mapclass,
                                  obj:entityclass, help:helpclass, music:musicclass):void {
	
	dwgfx.mobile_changebutton(dwgfx.BUTTON_BLANK);
	dwgfx.mobile_changeleftbutton(dwgfx.BUTTON_BLANK);
	dwgfx.mobile_hidearrows();
	
  //Misc	
	map.updatetowerglow();
	help.updateglow(); 
	
	game.creditposdelay--;
	if (game.creditposdelay <= 0) {
		game.creditposdelay = 1;
		game.creditposx++;
		if (game.creditposx > 40) {
			game.creditposy++;
			game.creditposx = 0;
			if (game.creditposy > 30) game.creditposy = 30;
		}
	}
	/*
	game.creditposition--;
	if (game.creditposition <= -1650) {
		game.creditposition = -1650;
	  map.bscroll = 0;
	}else {
		map.bypos += 1; map.bscroll = +1;
	}
	*/
	
	if (dwgfx.fademode == 1) {
		//Fix some graphical things
		dwgfx.showcutscenebars = false;
		dwgfx.cutscenebarspos = 0;
		//Fix the save thingy
		game.deletequick();
		game.savetele(map,obj,music,help);
		game.telegotoship();
		//Return to game
		map.colstate = 10;
		game.gamestate = 1; dwgfx.fademode = 4;
		music.playef(18, 10);
		game.createmenu("gamecompletecontinue");
		map.nexttowercolour();
	}
}


public function towerlogic(key:KeyPoll, dwgfx:dwgraphicsclass, game:gameclass, map:mapclass,
                          obj:entityclass, help:helpclass, music:musicclass):void {
  //Logic for the tower level
	dwgfx.mobile_changebutton(dwgfx.BUTTON_MENU);
	dwgfx.mobile_changeleftbutton(dwgfx.BUTTON_BLANK);
	dwgfx.mobile_showarrows(game.controlstyle);
	
	map.updatetowerglow();
	help.updateglow();
	
	if(!game.completestop){
		if (map.cameramode == 0) {
			//do nothing!
			//a trigger will set this off in the game
			map.cameramode = 1;
			map.bscroll = 0;
		}else if (map.cameramode == 1) {
			//move normally
			i = obj.getplayer();
			if(map.scrolldir==0){
				map.ypos -= 2;
				map.bypos -= 1;
				map.bscroll = -1;
			}else {
				map.ypos += 2;
				map.bypos += 1;
				map.bscroll = 1;
			}
		}else if (map.cameramode == 2) {
			//do nothing, but cycle colours (for taking damage)
			map.bscroll = 0; 
		}else if (map.cameramode == 4) {
			i = obj.getplayer();
			map.cameraseek = map.ypos - (obj.entities[i].yp - 120);

			map.cameraseek = map.cameraseek / 10;
			map.cameraseekframe = 10;
			
			map.cameramode = 5;
			
			map.bscroll = map.cameraseek/2; 
		}else if (map.cameramode == 5) {
			//actually do it
			if (map.spikeleveltop > 0) map.spikeleveltop-=2;
			if (map.spikelevelbottom > 0) map.spikelevelbottom-=2;
			if (map.cameraseekframe > 0) {
			  i = obj.getplayer();
				map.ypos -= map.cameraseek;
				if (map.cameraseek > 0) {
					if (map.ypos < obj.entities[i].yp - 120) {
						map.ypos = obj.entities[i].yp - 120;
					}
				}else {
					if (map.ypos > obj.entities[i].yp - 120) {
						map.ypos = obj.entities[i].yp - 120;
					}
				}
				map.cameraseekframe--;
				map.bypos = map.ypos / 2;
			}else {
				i = obj.getplayer();
				map.ypos = obj.entities[i].yp - 120;
				map.bypos = map.ypos / 2;
				map.cameramode = 0;
				map.colsuperstate = 0;
			}
		}
	}else {
		map.bscroll = 0;
	}
	
	if (map.ypos <= 0) { map.ypos = 0; map.bypos = 0; map.bscroll = 0; }
	if (map.minitowermode) {
		if (map.ypos >= 568) { 
			map.ypos = 568; map.bypos = map.ypos / 2; map.bscroll = 0;
		} //100-29 * 8 = 568
	}else{
	  if (map.ypos >= 5368) { map.ypos = 5368; map.bypos = map.ypos / 2; } //700-29 * 8 = 5368
	}
	
	if (game.lifeseq > 0) {
		if (map.cameramode == 2) {
			map.cameraseekframe = 20;
			map.cameramode = 4;
			map.resumedelay = 4;
		}
		
		if (map.cameraseekframe <= 0) {
			if (map.resumedelay <= 0) {
				game.lifesequence(obj);
				if (game.lifeseq == 0) map.cameramode = 1;
			}else {
				map.resumedelay--;
			}
	  }
	}
	
	if (game.deathseq != -1) {
		map.colsuperstate = 1;
		map.cameramode = 2;
		game.deathsequence(map, obj, music);
		game.deathseq--;
		if (game.deathseq <= 0) {
			if (game.nodeathmode) {
				game.deathseq = 1;
				game.gethardestroom(map);
				//start depressing sequence here...
				if (game.gameoverdelay <= -10 && dwgfx.fademode==0) dwgfx.fademode = 2;
				if (dwgfx.fademode == 1) script.resetgametomenu(key, dwgfx, game, map, obj, help, music);
			}else{
				if (game.currentroomdeaths > game.hardestroomdeaths) {
					game.hardestroomdeaths = game.currentroomdeaths;
					game.hardestroom = map.roomname;
				}
				
				game.gravitycontrol = game.savegc;
				dwgfx.textboxremove();
				map.resetplayer(dwgfx, game, obj, music);
			}
		}
	}else{
    //State machine for game logic
		game.updatestate(dwgfx, map, obj, help, music);

		
				//Time trial stuff
		if (game.intimetrial) {
			if (game.timetrialcountdown > 0) {
				game.hascontrol = true;
				game.timetrialcountdown--;
			  if (game.timetrialcountdown > 30) {
				  game.hascontrol = false;	
				}
				if(game.timetrialcountdown == 120) music.playef(21, 10);
				if(game.timetrialcountdown == 90) music.playef(21, 10);
				if(game.timetrialcountdown == 60) music.playef(21, 10);
				if (game.timetrialcountdown == 30) {
					switch(game.timetriallevel) {
						case 0: music.play(1); break;
						case 1: music.play(3); break;
						case 2: music.play(2); break;
						case 3: music.play(1); break;
						case 4: music.play(12); break;
						case 5: music.play(15); break;
					}
					music.playef(22, 10);
				}
			}
			
			//Have we lost the par?
			if (!game.timetrialparlost) {
				if ((game.minutes * 60) + game.seconds > game.timetrialpar) {
					game.timetrialparlost = true;
					i = obj.getplayer();
					if (i > -1) {
						obj.entities[i].tile = 144;
					}
					music.playef(2, 10);
				}
			}
		}
		//Update entities
		
	  if(!game.completestop){
			for (i = obj.nentity - 1; i >= 0;  i--) {
				//Remove old platform
				//if (obj.entities[i].isplatform) obj.removeblockat(obj.entities[i].xp, obj.entities[i].yp);
				
				obj.updateentities(i, help, game, music);                // Behavioral logic
				obj.updateentitylogic(i, game);                          // Basic Physics
				obj.entitymapcollision(i, map);                          // Collisions with walls
				
				//Create new platform
				//if (obj.entities[i].isplatform) obj.movingplatformfix(i, map);
			}
			
			obj.entitycollisioncheck(dwgfx, game, map, music);         // Check ent v ent collisions, update states
			//special for tower: is the player touching any spike blocks?
			i = obj.getplayer();
			if(obj.checktowerspikes(i, map) && dwgfx.fademode==0){
				game.deathseq = 30;
			}
			
			//Right so! Screenwraping for tower:
			if (map.minitowermode) {
				if (map.scrolldir == 1) {
					//This is minitower 1!
					i = obj.getplayer();
					if (game.door_left > -2 && obj.entities[i].xp < -14) 
					{ obj.entities[i].xp += 320; map.gotoroom(48, 52, dwgfx, game, obj, music); }
					if (game.door_right > -2 && obj.entities[i].xp >= 308) 
					{ obj.entities[i].xp -= 320; obj.entities[i].yp -= (71*8); map.gotoroom(game.roomx + 1, game.roomy+1, dwgfx, game, obj, music);}
				}else {
					//This is minitower 2!
					i = obj.getplayer();
					if (game.door_left > -2 && obj.entities[i].xp < -14) 
					{ if (obj.entities[i].yp > 300) {
						obj.entities[i].xp += 320; obj.entities[i].yp -= (71 * 8); map.gotoroom(50, 54, dwgfx, game, obj, music); 
					}else
					 { obj.entities[i].xp += 320; map.gotoroom(50, 53, dwgfx, game, obj, music); }
					}
					if (game.door_right > -2 && obj.entities[i].xp >= 308) 
					{ obj.entities[i].xp -= 320; map.gotoroom(52, 53, dwgfx, game, obj, music); }
				}
			}else {
				//Always wrap except for the very top and very bottom of the tower
				if(map.ypos>=500 && map.ypos <=5000){
					for (i = 0; i < obj.nentity;  i++) {
						if (obj.entities[i].xp <= -10) {
							obj.entities[i].xp += 320;
						}else {
							if (obj.entities[i].xp > 310) {
								obj.entities[i].xp -= 320;
							}
						}
					}
				}else {
					//Do not wrap! Instead, go to the correct room
					i = obj.getplayer();
					if (game.door_left > -2 && obj.entities[i].xp < -14) 
					{ obj.entities[i].xp += 320; obj.entities[i].yp -= (671 * 8); map.gotoroom(108, 109, dwgfx, game, obj, music); }
					if (game.door_right > -2 && obj.entities[i].xp >= 308) 
					{ obj.entities[i].xp -= 320; map.gotoroom(110, 104, dwgfx, game, obj, music);}
				}
			}
			
			if(game.lifeseq==0){
				i = obj.getplayer();
				if(!map.invincibility){
					if (obj.entities[i].yp-map.ypos <= 0) {
						game.deathseq = 30;
					}else if (obj.entities[i].yp-map.ypos >= 208) {
						game.deathseq = 30;
					}
				}else {
					if (obj.entities[i].yp-map.ypos <= 0) {
						map.ypos-=10; map.bypos = map.ypos / 2; map.bscroll = 0;
					}else if (obj.entities[i].yp-map.ypos >= 208) {
						map.ypos+=2; map.bypos = map.ypos / 2; map.bscroll = 0;
					}
				}
				
				if (obj.entities[i].yp - map.ypos <= 40) {
					map.spikeleveltop++;
					if (map.spikeleveltop >= 8) map.spikeleveltop = 8;
				}else {
					if (map.spikeleveltop > 0) map.spikeleveltop--;
				}
				
				if (obj.entities[i].yp - map.ypos >= 164) {
					map.spikelevelbottom++;
					if (map.spikelevelbottom >= 8) map.spikelevelbottom = 8;
				}else {
					if (map.spikelevelbottom > 0) map.spikelevelbottom--;
				}
				
			}
			
			
		}
			//now! let's clean up removed entities
		obj.cleanup();
		
	  //Looping around, room change conditions!		
	}
	
	if (game.teleport_to_new_area) script.teleport(dwgfx, game, map,	obj, help, music);
}

public function gamelogic(key:KeyPoll, dwgfx:dwgraphicsclass, game:gameclass, map:mapclass,
                          obj:entityclass, help:helpclass, music:musicclass):void {
	if (game.autoquicksave) {
		game.autoquicksave = false;
		if(!game.intimetrial && !game.nodeathmode && !game.insecretlab && !game.inintermission){
			if(map.custommodeforreal){
				game.customsavequick(editor.ListOfMetaData[game.playcustomlevel].file_num, map, obj, music, help);
			}else{
				game.savequick(map, obj, music, help);
			}
		}
	}
	
	//Misc
	help.updateglow();
	
	if (game.alarmon) {
		game.alarmdelay--;
		if (game.alarmdelay <= 0) {
			music.playef(19);
			game.alarmdelay = 20;
		}
	}
	
	if (obj.nearelephant) {
		obj.upset++;
		if (obj.upset == 300) {
			obj.upsetmode = true;
			//change player to sad
			i = obj.getplayer();
			obj.entities[i].tile = 144;
			music.playef(2, 10);
		}
		if (obj.upset > 301) obj.upset = 301;
	}else if (obj.upsetmode) {
		obj.upset--;
		if (obj.upset <= 0) {
			obj.upset = 0;
			obj.upsetmode = false;
			//change player to happy
			i = obj.getplayer();
			obj.entities[i].tile = 0;
		}
	}else {
		obj.upset = 0;
	}
	
  game.lifesequence(obj);
	
	
	if (game.deathseq != -1) {
		for (i = 0; i < obj.nentity; i++) {
			if (game.roomx == 111 && game.roomy == 107) {
			  if (obj.entities[i].type == 1) {
					if (obj.entities[i].xp < 152) {
						obj.entities[i].xp = 152;
						obj.entities[i].newxp = 152;
					}
			  }
			}
			if (obj.entities[i].type == 2 && obj.entities[i].state == 3) {
				//Ok! super magical exception for the room with the intention death for the shiny trinket
				//fix this when the maps are finalised
				if (game.roomx != 111 || game.roomy != 107 || map.custommode) {
				  obj.entities[i].state = 4;	
				}else {
				  obj.entities[i].state = 4;	
					map.settile(18, 9, 59);
				}
			}else if (obj.entities[i].type == 2 && obj.entities[i].state == 2) {
				//ok, unfortunate case where the disappearing platform hasn't fully disappeared. Accept a little
				//graphical uglyness to avoid breaking the room!
				while (obj.entities[i].state == 2) obj.updateentities(i, help, game, music);
				obj.entities[i].state = 4;
			}else if (map.finallevel && map.finalstretch && obj.entities[i].type == 2) {
				//for the final level. probably something 99% of players won't see.
				while (obj.entities[i].state == 2) obj.updateentities(i, help, game, music);
				obj.entities[i].state = 4;
			}else if (obj.entities[i].type == 23 && game.swnmode && game.deathseq<15) {
				//if playing SWN, get the enemies offscreen.
				obj.entities[i].xp += obj.entities[i].vx*5;
				obj.entities[i].yp += obj.entities[i].vy*5;
			}
		}
		if (game.swnmode) {
			//if playing SWN game a, push the clock back to the nearest 10 second interval
			if (game.swngame == 0) {
				game.swnpenalty();
			}else if (game.swngame == 1) {
				game.swnstate = 0; game.swnstate2 = 0; game.swnstate3 = 0; game.swnstate4 = 0; game.swndelay = 0;
				if (game.swntimer >= game.swnrecord) {
					if (game.swnmessage == 0) {
						music.playef(25, 10);
						game.scores.reportScore(game.swntimer);
					}
					game.swnmessage = 1;
					game.swnrecord = game.swntimer;
				}
			}
		}
		
		game.deathsequence(map, obj, music);
		game.deathseq--;
		if (game.deathseq <= 0) {
			if (game.nodeathmode) {
				game.deathseq = 1;
				game.gethardestroom(map);
				//start depressing sequence here...
				if (game.gameoverdelay <= -10 && dwgfx.fademode==0) dwgfx.fademode = 2;
				if (dwgfx.fademode == 1) script.resetgametomenu(key, dwgfx, game, map, obj, help, music);
			}else {
				if (game.swnmode) {
					//if playing SWN game b, reset the clock
					if (game.swngame == 1) {
						game.swntimer = 0;
						game.swnmessage = 0;
						game.swnrank = 0;
					}
				}
				
				game.gethardestroom(map);				
				game.activetele = false;
				game.hascontrol = true;
				
				
				game.gravitycontrol = game.savegc;
				dwgfx.textboxremove();
				map.resetplayer(dwgfx, game, obj, music);
			}
		}
	}else {
		//Update colour thingy
		if (map.finalmode) {
			if (map.final_colormode) {
				if (map.final_colorframe > 0) {
					map.final_colorframedelay--;
					if (map.final_colorframedelay <= 0) {
						if (map.final_colorframe == 1) {
							map.final_colorframedelay = 40;
							temp = 1+int(Math.random() * 6);
							if (temp == map.final_mapcol) temp = (temp + 1) % 6;
							if (temp == 0) temp = 6;
							map.changefinalcol(temp, obj,game);
						}else if (map.final_colorframe == 2) {
							map.final_colorframedelay = 15;
							temp = 1+int(Math.random() * 6);
							if (temp == map.final_mapcol) temp = (temp + 1) % 6;
							if (temp == 0) temp = 6;
							map.changefinalcol(temp, obj,game);
						}
					}
				}
			}
		}
    //State machine for game logic
		game.updatestate(dwgfx, map, obj, help, music);
		if (game.startscript) {
		  script.load(game.newscript);
		  game.startscript = false;
	  }
		
		//Intermission 1 Logic
		//Player can't walk off a screen with SCM on it until they've left
		if (game.supercrewmate) {
			if (game.roomx == 41 + game.scmprogress) { //he's in the same room
				i = obj.getplayer();
				if (obj.entities[i].ax > 0 && obj.entities[i].xp > 280) {
					obj.entities[i].ax = 0;
					obj.entities[i].dir = 0;
				}
			}
		}
		
		//SWN Minigame Logic
		if (game.swnmode) {	//which game?
			if(game.swngame==0){ //intermission, survive 60 seconds game
				game.swntimer -= 1;
				if (game.swntimer <= 0) {
					music.niceplay(8);
					game.swngame = 5;
				}else {
					obj.generateswnwave(game, help, 0);
				}
			}else if(game.swngame==1){ //super gravitron game
				game.swntimer += 1;
				if (game.swntimer > game.swnrecord) game.swnrecord = game.swntimer;
				
				if (game.swntimer >= 150 && game.swnrank == 0) {
					game.swnrank = 1; 
					if (game.swnbestrank < 1) {	
						game.scores.reportAchievement(game.scores.vvvvvvsupgrav5);
						game.swnbestrank = 1; game.swnmessage = 2 + 30; music.playef(26, 10);	
					}
				}else if (game.swntimer >= 300 && game.swnrank == 1) {
					game.swnrank = 2; 
					if (game.swnbestrank < 2) {	
						game.scores.reportAchievement(game.scores.vvvvvvsupgrav10);
						game.swnbestrank = 2; game.swnmessage = 2 + 30; music.playef(26, 10);	
					}
				}else if (game.swntimer >= 450 && game.swnrank == 2) {
					game.swnrank = 3; 
					if (game.swnbestrank < 3) {	
						game.scores.reportAchievement(game.scores.vvvvvvsupgrav15);
						game.swnbestrank = 3; game.swnmessage = 2 + 30; music.playef(26, 10);	
					}
				}else if (game.swntimer >= 600 && game.swnrank == 3) {
					game.swnrank = 4; 
					if (game.swnbestrank < 4) {	
						game.scores.reportAchievement(game.scores.vvvvvvsupgrav20);
						game.swnbestrank = 4; game.swnmessage = 2 + 30; music.playef(26, 10);	
					}
				}else if (game.swntimer >= 900 && game.swnrank == 4) {
					game.swnrank = 5; 
					if (game.swnbestrank < 5) {						
						game.scores.reportAchievement(game.scores.vvvvvvsupgrav30);
						game.swnbestrank = 5; game.swnmessage = 2 + 30; music.playef(26, 10);	
					}
				}else if (game.swntimer >= 1800 && game.swnrank == 5) {
					game.swnrank = 6; 
					if (game.swnbestrank < 6) {	
						game.scores.reportAchievement(game.scores.vvvvvvsupgrav60);
						game.swnbestrank = 6; game.swnmessage = 2 + 30; music.playef(26, 10);
					}
				}
				
				obj.generateswnwave(game, help, 1);
				
				game.swncoldelay--;
				if(game.swncoldelay<=0){
				  game.swncolstate = (game.swncolstate+1)%6; 
					game.swncoldelay = 30;
					dwgfx.rcol = game.swncolstate;
					obj.swnenemiescol(game.swncolstate);
				}
			}else if (game.swngame == 2) { //introduce game a
				game.swndelay--;
				if (game.swndelay <= 0) {
					game.swngame = 0;
					game.swndelay = 0;
					game.swntimer = (60 * 30) - 1;
					//game.swntimer = 15;
				}
			}else if (game.swngame == 3) { //extend line
				obj.entities[obj.getlineat(84 - 32)].w += 24;
				if (obj.entities[obj.getlineat(84 - 32)].w > 332) {
					obj.entities[obj.getlineat(84 - 32)].w = 332;
					game.swngame = 2;
				}
			}else if (game.swngame == 4) { //create top line
				game.swngame = 3;
			  obj.createentity(game, -8, 84 - 32, 11, 8);  // (horizontal gravity line)
				music.niceplay(2);
				game.swndeaths = game.deathcounts;
			}else if (game.swngame == 5) { //remove line
				obj.entities[obj.getlineat(148 + 32)].xp += 24;
				if (obj.entities[obj.getlineat(148 + 32)].xp > 320) {
					obj.entities[obj.getlineat(148 + 32)].active = false;
					game.swnmode = false;
					game.swngame = 6;
				}
			}else if (game.swngame == 6) { //Init the super gravitron
				game.swngame = 7;
			  music.niceplay(3);
			}else if (game.swngame == 7) { //introduce game b
				game.swndelay--;
				if (game.swndelay <= 0) {
					game.swngame = 1;
					game.swndelay = 0;
					game.swntimer = 0;
					game.swncolstate = 3; game.swncoldelay = 30;
				}
			}
		}
		
		//Time trial stuff
		if (game.intimetrial) {
			if (game.timetrialcountdown > 0) {
				game.hascontrol = true;
				game.timetrialcountdown--;
			  if (game.timetrialcountdown > 30) {
				  game.hascontrol = false;	
				}
				if(game.timetrialcountdown == 120) music.playef(21, 10);
				if(game.timetrialcountdown == 90) music.playef(21, 10);
				if(game.timetrialcountdown == 60) music.playef(21, 10);
				if (game.timetrialcountdown == 30) {
					switch(game.timetriallevel) {
						case 0: music.play(1); break;
						case 1: music.play(3); break;
						case 2: music.play(2); break;
						case 3: music.play(1); break;
						case 4: music.play(12); break;
						case 5: music.play(15); break; //Play cool remix for final level!
					}
					music.playef(22, 10);
				}
			}
			
			//Have we lost the par?
			if (!game.timetrialparlost) {
				if ((game.minutes * 60) + game.seconds > game.timetrialpar) {
					game.timetrialparlost = true;					
					i = obj.getplayer();
					if (i > -1) {
						obj.entities[i].tile = 144;
					}
					music.playef(2, 10);
				}
			}
		}
		
		//Update entities
		//Ok, moving platform fuckers
		if (!game.completestop) {
			if(obj.vertplatforms){
				for (i = obj.nentity - 1; i >= 0;  i--) {
					if (obj.entities[i].isplatform) {
						if(obj.entities[i].vx==0) {
							obj.removeblockat(obj.entities[i].xp, obj.entities[i].yp);
							
							obj.updateentities(i, help, game, music);                // Behavioral logic
							obj.updateentitylogic(i, game);                          // Basic Physics
							obj.entitymapcollision(i, map);                          // Collisions with walls
							
							obj.createblock(0, obj.entities[i].xp, obj.entities[i].yp, obj.entities[i].w, obj.entities[i].h);
							if (game.supercrewmate) {
								obj.movingplatformfix(i, map);
								obj.scmmovingplatformfix(i, map);
							}else {
								obj.movingplatformfix(i, map);
							}
						}
					}
				}
			}
		
			if(obj.horplatforms){
				for (i = obj.nentity - 1; i >= 0;  i--) {
					if (obj.entities[i].isplatform) {
						if(obj.entities[i].vy==0) {
							obj.removeblockat(obj.entities[i].xp, obj.entities[i].yp);
						
							obj.updateentities(i, help, game, music);                // Behavioral logic
							obj.updateentitylogic(i, game);                          // Basic Physics
							obj.entitymapcollision(i, map);                          // Collisions with walls
						
							obj.hormovingplatformfix(i, map);
						}
					}
				}
				//is the player standing on a moving platform?
				i = obj.getplayer(); j = obj.entitycollideplatformfloor(map, i);
				if (j > -1000) { obj.entities[i].newxp = obj.entities[i].xp + j;	obj.entitymapcollision(i, map);
				}else {
					j = obj.entitycollideplatformroof(map, i);
					if (j > -1000) { obj.entities[i].newxp = obj.entities[i].xp + j;  obj.entitymapcollision(i, map);	}
				}
			}
			
			for (i = obj.nentity - 1; i >= 0;  i--) {
				if (!obj.entities[i].isplatform) {
					obj.updateentities(i, help, game, music);          // Behavioral logic
					obj.updateentitylogic(i, game);                    // Basic Physics
					obj.entitymapcollision(i, map);                    // Collisions with walls
				}
			}
			
			obj.entitycollisioncheck(dwgfx, game, map, music);         // Check ent v ent collisions, update states
		}
		/*
		game.test = true;
		game.teststring = String(help.booltoint(game.disablekludge_1)) + "-";
		game.teststring += String(help.booltoint(game.disablekludge_2)) + "-";
		game.teststring += String(help.booltoint(game.disablekludge_3)) + "-";
		game.teststring += String(help.booltoint(game.disablekludge_4));
		game.teststring += " nblocks=" + String(obj.nblocks);
		*/
		//now! let's clean up removed entities
		obj.cleanup();
		
		//Using warplines?
		if (obj.customwarpmode) {
			//Rewritten system for mobile update: basically, the new logic is to
			//check if the player is leaving the map, and if so do a special check against
			//warp lines for collision
			obj.customwarpmodehon = false;
			obj.customwarpmodevon = false;
			
			i = obj.getplayer();
			if ((game.door_down > -2 && obj.entities[i].yp >= 226-16) ||
				  (game.door_up > -2 && obj.entities[i].yp < -2+16) ||
				  (game.door_left > -2 && obj.entities[i].xp < -14+16) ||
				  (game.door_right > -2 && obj.entities[i].xp >= 308-16)){
				//Player is leaving room
				obj.customwarplinecheck(i);
			}
			
			if(obj.customwarpmodehon){ map.warpy=true;
			}else{ map.warpy=false; }
			if(obj.customwarpmodevon){ map.warpx=true;
			}else{ map.warpx=false; }
		}
		
		//Finally: Are we changing room?
		if (map.warpx && map.warpy) {
			for (i = 0; i < obj.nentity;  i++) {
        if (obj.entities[i].type < 50) { //Don't warp warp lines
					if (obj.entities[i].size < 12) { //Don't wrap SWN enemies
						if (obj.entities[i].xp <= -10) {
							obj.entities[i].xp += 320;
						}else {
							if (obj.entities[i].xp > 310) {
								obj.entities[i].xp -= 320;
							}
						}
					}
				}
			}
			
			for (i = 0; i < obj.nentity;  i++) {
        if(obj.entities[i].type<50){ //Don't warp warp lines
					if (obj.entities[i].size < 12) { //Don't wrap SWN enemies
						if (obj.entities[i].yp <= -12) {
							obj.entities[i].yp += 232;
						}else {
							if (obj.entities[i].yp > 226) {
								obj.entities[i].yp -= 232;
							}
						}
					}
				}
			}
		}else if (map.warpx) {
			for (i = 0; i < obj.nentity;  i++) {
        if (obj.entities[i].type < 50) { //Don't warp warp lines
					if (obj.entities[i].size < 12) { //Don't wrap SWN enemies
						if (game.roomx == 118 && game.roomy == 102 && obj.entities[i].rule==1 && !map.custommode) {
							//ascii snakes
							if (obj.entities[i].xp <= -80) {
								obj.entities[i].xp += 400;
							}else {
								if (obj.entities[i].xp > 320) {
									obj.entities[i].xp -= 400;
								}
							}
						}else{
							if (obj.entities[i].xp <= -10) {
								obj.entities[i].xp += 320;
							}else {
								if (obj.entities[i].xp > 310) {
									obj.entities[i].xp -= 320;
								}
							}
						}
					}
				}
			}
			
			i = obj.getplayer();
			if (game.door_down > -2 && obj.entities[i].yp >= 238) 
			 { obj.entities[i].yp -= 240; map.gotoroom(game.roomx, game.roomy + 1, dwgfx, game, obj, music); }
			if (game.door_up > -2 && obj.entities[i].yp < -2) 
			 { obj.entities[i].yp += 240; map.gotoroom(game.roomx, game.roomy - 1, dwgfx, game, obj, music);}
		}else if (map.warpy) {
			for (i = 0; i < obj.nentity;  i++) {
        if(obj.entities[i].type<50){ //Don't warp warp lines
					if (obj.entities[i].yp <= -12) {
						obj.entities[i].yp += 232;
					}else {
						if (obj.entities[i].yp > 226) {
							obj.entities[i].yp -= 232;
						}
					}
				}
			}
			
			for (i = 0; i < obj.nentity;  i++) {
        if (obj.entities[i].type < 50) { //Don't warp warp lines
					if(obj.entities[i].rule!=0){
						if (obj.entities[i].xp <= -30) {
							obj.entities[i].xp += 350;
						}else {
							if (obj.entities[i].xp > 320) {
								obj.entities[i].xp -= 350;
							}
						}
					}
				}
			}
			
		  i = obj.getplayer();
			if (game.door_left > -2 && obj.entities[i].xp < -14) 
			 { obj.entities[i].xp += 320; map.gotoroom(game.roomx - 1, game.roomy, dwgfx, game, obj, music);}
			if (game.door_right > -2 && obj.entities[i].xp >= 308) 
			 { obj.entities[i].xp -= 320; map.gotoroom(game.roomx + 1, game.roomy, dwgfx, game, obj, music); }
		}else {
		  //Normal! Just change room	
			i = obj.getplayer();
			if (game.door_down > -2 && obj.entities[i].yp >= 238) 
			 { obj.entities[i].yp -= 240; map.gotoroom(game.roomx, game.roomy + 1, dwgfx, game, obj, music); }
			if (game.door_up > -2 && obj.entities[i].yp < -2) 
			 { obj.entities[i].yp += 240; map.gotoroom(game.roomx, game.roomy - 1, dwgfx, game, obj, music);}
			if (game.door_left > -2 && obj.entities[i].xp < -14) 
			 { obj.entities[i].xp += 320; map.gotoroom(game.roomx - 1, game.roomy, dwgfx, game, obj, music);}
			if (game.door_right > -2 && obj.entities[i].xp >= 308) 
			 { obj.entities[i].xp -= 320; map.gotoroom(game.roomx + 1, game.roomy, dwgfx, game, obj, music); }
		}
		
		//Warp tokens
		if (map.custommode){
			if (game.teleport){
				var edi:int=obj.entities[game.edteleportent].behave;
				var edj:int=obj.entities[game.edteleportent].para;
				var edi2:int, edj2:int;
				edi2 = (edi-(edi%40))/40;
				edj2 = (edj - (edj % 30)) / 30;
				
				map.warpto(100 + edi2, 100 + edj2, obj.getplayer(), edi % 40, (edj % 30) + 2, dwgfx, game, obj, music);
				game.teleport = false;
				
				if (game.teleport == false){
					game.flashlight = 6;
					game.screenshake = 25;
				}
			}
		}else{
			if (game.teleport) {
				if (game.roomx == 117 && game.roomy == 102) {
					i = obj.getplayer();
					obj.entities[i].yp = 225; map.gotoroom(119, 100, dwgfx, game, obj, music);
					game.teleport = false;
				}else if (game.roomx == 119 && game.roomy == 100) {
					i = obj.getplayer();
					obj.entities[i].yp = 225; map.gotoroom(119, 103, dwgfx, game, obj, music);
					game.teleport = false;
				}else if (game.roomx == 119 && game.roomy == 103) {
					i = obj.getplayer();
					obj.entities[i].xp = 0; map.gotoroom(116, 103, dwgfx, game, obj, music);
					game.teleport = false;
				}else if (game.roomx == 116 && game.roomy == 103) {
					i = obj.getplayer();
					obj.entities[i].yp = 225; map.gotoroom(116, 100, dwgfx, game, obj, music);
					game.teleport = false;
				}else if (game.roomx == 116 && game.roomy == 100) {
					i = obj.getplayer();
					obj.entities[i].xp = 0; map.gotoroom(114, 102, dwgfx, game, obj, music);
					game.teleport = false;
				}else if (game.roomx == 114 && game.roomy == 102) {
					i = obj.getplayer();
					obj.entities[i].yp = 225; map.gotoroom(113, 100, dwgfx, game, obj, music);
					game.teleport = false;
				}else if (game.roomx == 116 && game.roomy == 104) {
					//pre warp zone here
					map.warpto(107, 101, obj.getplayer(), 14, 16, dwgfx, game, obj, music);
				}else if (game.roomx == 107 && game.roomy == 101) {
					map.warpto(105, 119, obj.getplayer(), 5, 26, dwgfx, game, obj, music);
				}else if (game.roomx == 105 && game.roomy == 118) {
					map.warpto(101, 111, obj.getplayer(), 34, 6, dwgfx, game, obj, music);
				}else if (game.roomx == 101 && game.roomy == 111) {
					//There are lots of warp tokens in this room, so we have to distinguish!
					switch(game.teleportxpos) {
						case 1: map.warpto(108, 108, obj.getplayer(), 4, 27, dwgfx, game, obj, music);	break;
						case 2: map.warpto(101, 111, obj.getplayer(), 12, 27, dwgfx, game, obj, music);	break;
						case 3: map.warpto(119, 111, obj.getplayer(), 31, 7, dwgfx, game, obj, music);	break;
						case 4: map.warpto(114, 117, obj.getplayer(), 19, 16, dwgfx, game, obj, music);	break;
					}
				}else if (game.roomx == 108 && game.roomy == 106) {
					map.warpto(119, 111, obj.getplayer(), 4, 27, dwgfx, game, obj, music);
				}else if (game.roomx == 100 && game.roomy == 111) {
					map.warpto(101, 111, obj.getplayer(), 24, 6, dwgfx, game, obj, music);
				}else if (game.roomx == 119 && game.roomy == 107) {
					//Secret lab, to super gravitron
					map.warpto(119, 108, obj.getplayer(), 19, 10, dwgfx, game, obj, music);
				}
				if (game.teleport == false) {
					game.flashlight = 6; game.screenshake = 25;
				}
			}
		}
	}
	
	if (game.roomchange) {
		//We've changed room? Let's bring our companion along!
		game.roomchange = false;
		if (game.companion > 0) {
			i = obj.getplayer();
			//ok, we'll presume our companion has been destroyed in the room change. So:
			switch(game.companion) {
				case 6:
		      obj.createentity(game, obj.entities[i].xp, 121, 15,1);  //Y=121, the floor in that particular place!
					j = obj.getcompanion(6);
					obj.entities[j].vx = obj.entities[i].vx;
					obj.entities[j].dir = obj.entities[i].dir;
				break;
				case 7:
				  if (game.roomy <= 105) { //don't jump after him!
					  if (game.roomx == 110) { 
							obj.createentity(game, 320, 86, 16, 1);  //Y=86, the ROOF in that particular place!
						}else{
						  obj.createentity(game, obj.entities[i].xp, 86, 16, 1);  //Y=86, the ROOF in that particular place!
						}
						j = obj.getcompanion(7);
						obj.entities[j].vx = obj.entities[i].vx;
						obj.entities[j].dir = obj.entities[i].dir;
					}
				break;
				case 8:
				  if (game.roomy >= 104) { //don't jump after him!
					  if (game.roomx == 102) { 
							obj.createentity(game, 310, 177, 17, 1); 
							j = obj.getcompanion(8);
							obj.entities[j].vx = obj.entities[i].vx;
							obj.entities[j].dir = obj.entities[i].dir;
						}else{
							obj.createentity(game, obj.entities[i].xp, 177, 17, 1); 
							j = obj.getcompanion(8);
							obj.entities[j].vx = obj.entities[i].vx;
							obj.entities[j].dir = obj.entities[i].dir;
						}
					}
				break;
				case 9:
				  if (!map.towermode) { //don't go back into the tower!
						if (game.roomx == 110 && obj.entities[i].xp<20) {
							obj.createentity(game, 100, 185, 18, 15, 0, 1); 
						}else{
						  obj.createentity(game, obj.entities[i].xp, 185, 18, 15, 0, 1); 
						}
						j = obj.getcompanion(9);
						obj.entities[j].vx = obj.entities[i].vx;
						obj.entities[j].dir = obj.entities[i].dir;
					}
				break;
				case 10:
				  //intermission 2, choose colour based on lastsaved
					if (game.roomy == 51) {
						if (obj.flags[59] == 0) {
							obj.createentity(game, 225, 169, 18, dwgfx.crewcolour(game.lastsaved), 0, 10);
							j = obj.getcompanion(10);
							obj.entities[j].vx = obj.entities[i].vx;
							obj.entities[j].dir = obj.entities[i].dir;
						}
					}else	if (game.roomy >= 52) {
						if (obj.flags[59] == 1) {
							obj.createentity(game, 160, 177, 18, dwgfx.crewcolour(game.lastsaved), 0, 18, 1);
							j = obj.getcompanion(10);
							obj.entities[j].vx = obj.entities[i].vx;
							obj.entities[j].dir = obj.entities[i].dir;
						}else {
							obj.flags[59] = 1;
							obj.createentity(game, obj.entities[i].xp, -20, 18, dwgfx.crewcolour(game.lastsaved), 0, 10, 0);
							j = obj.getcompanion(10);
							obj.entities[j].vx = obj.entities[i].vx;
							obj.entities[j].dir = obj.entities[i].dir;
						}
					}
				break;
				case 11: 
				  //Intermission 1: We're using the SuperCrewMate instead!
					//obj.createentity(game, obj.entities[i].xp, obj.entities[i].yp, 24, dwgfx.crewcolour(game.lastsaved));
					if(game.roomx-41==game.scmprogress){
						switch(game.scmprogress) {
							case 0: obj.createentity(game, 76, 161, 24, dwgfx.crewcolour(game.lastsaved), 2);	break;
							case 1: obj.createentity(game, 10, 169, 24, dwgfx.crewcolour(game.lastsaved), 2);	break;
							case 2: obj.createentity(game, 10, 177, 24, dwgfx.crewcolour(game.lastsaved), 2);	break;
							case 3: 
							  if (game.scmmoveme) {
									obj.createentity(game, obj.entities[obj.getplayer()].xp, 185, 24, dwgfx.crewcolour(game.lastsaved), 2);	
									game.scmmoveme = false;
								}else{
							    obj.createentity(game, 10, 177, 24, dwgfx.crewcolour(game.lastsaved), 2);	
								}
							break;
							case 4: obj.createentity(game, 10, 185, 24, dwgfx.crewcolour(game.lastsaved), 2);	break;
							case 5: obj.createentity(game, 10, 185, 24, dwgfx.crewcolour(game.lastsaved), 2);	break;
							case 6: obj.createentity(game, 10, 185, 24, dwgfx.crewcolour(game.lastsaved), 2);	break;
							case 7: obj.createentity(game, 10, 41, 24, dwgfx.crewcolour(game.lastsaved), 2);	break;
							case 8: obj.createentity(game, 10, 169, 24, dwgfx.crewcolour(game.lastsaved), 2);	break;
							case 9: obj.createentity(game, 10, 169, 24, dwgfx.crewcolour(game.lastsaved), 2);	break;
							case 10: obj.createentity(game, 10, 129, 24, dwgfx.crewcolour(game.lastsaved), 2); break;
							case 11: obj.createentity(game, 10, 129, 24, dwgfx.crewcolour(game.lastsaved), 2);	break;
							case 12: obj.createentity(game, 10, 65, 24, dwgfx.crewcolour(game.lastsaved), 2);	break;
							case 13: obj.createentity(game, 10, 177, 24, dwgfx.crewcolour(game.lastsaved));	break;
						}
					}
					
					if (game.scmmoveme) {
						obj.entities[obj.getscm()].xp = obj.entities[obj.getplayer()].xp;
						game.scmmoveme = false;
					}
				break;
			}
		}
	}
	
	game.activeactivity = obj.checkactivity();
	
  if (game.hascontrol && !script.running && !dwgfx.showcutscenebars) {
		if (game.readytotele > 0) {
			dwgfx.mobile_changebutton(dwgfx.BUTTON_TELEPORT);	
		}else{
			if (game.activeactivity > -1) {
				if (obj.blocks[game.activeactivity].b == 20) {
					dwgfx.mobile_changebutton(dwgfx.BUTTON_USE);	
				}else {
					dwgfx.mobile_changebutton(dwgfx.BUTTON_TALK);	
				}
			}else{
				dwgfx.mobile_changebutton(dwgfx.BUTTON_MENU);
			}
		}
	}else {
		dwgfx.mobile_changebutton(dwgfx.BUTTON_BLANK);
	}
	
	if (game.insecretlab) {
		if (device.deviceresolution == device.IPAD) {
			if(obj.trophytext>0){
				dwgfx.mobile_changeleftbutton(dwgfx.BUTTON_BLANK);
			}else {
				dwgfx.mobile_changeleftbutton(dwgfx.BUTTON_GAMECENTER);
			}
		}else{
			dwgfx.mobile_changeleftbutton(dwgfx.BUTTON_GAMECENTER);
		}
	}else {
		dwgfx.mobile_changeleftbutton(dwgfx.BUTTON_BLANK);
	}
	dwgfx.mobile_showarrows(game.controlstyle);
	
	if (game.activetele) {
		i = obj.getplayer(); obj.settemprect(i);
		if (game.teleblock.intersects(obj.temprect)){
			game.readytotele += 25;
			if (game.readytotele >= 255) game.readytotele = 255;
		}else {
			game.readytotele -= 50;
			if (game.readytotele < 0) game.readytotele = 0;
		}
	}else {
		if (game.readytotele > 0){
		  game.readytotele -= 50;
		  if (game.readytotele < 0) game.readytotele = 0;
		}
	}
	
	if (game.teleport_to_new_area) script.teleport(dwgfx, game, map,	obj, help, music);
}


public function controltutoriallogic(key:KeyPoll, dwgfx:dwgraphicsclass, game:gameclass, map:mapclass,
                          obj:entityclass, help:helpclass, music:musicclass):void {
	game.press_right = false;
	game.press_left = false;
	game.press_action = false;
	game.press_map = false;
	
	//Misc
	help.updateglow();
	if (game.controllerp2 > 0) {
		game.controllerp2--;
	}
	
	switch(game.controltutorialstate) {
		case 0:
			game.controltutorialstatedelay++;
			if (game.controltutorialstatedelay > 10) {
				game.controltutorialstatedelay = 0;
				game.controltutorialstate = 1;
			}
		break;
		case 1:
			game.controllerp1+=5;
			if (game.controllerp1 >= 100) {
				game.controllerp1 = 100;
				game.controltutorialstatedelay = 0;
				game.controltutorialstate = 2;
			}
		break;
		case 2:
			game.controltutorialstatedelay++;
			if (game.controltutorialstatedelay > 30) {
				game.controltutorialstatedelay = 0;
				game.controltutorialstate = 3;
			}
		break;
		case 3:
		  game.press_right = true;
			game.controltutorialstatedelay++;
			if (game.controltutorialstatedelay > 60) {
				game.controltutorialstatedelay = 0;
				game.controltutorialstate++;
			}
		break;
		case 4:
			game.controltutorialstatedelay++;
			if (game.controltutorialstatedelay > 30) {
				game.controltutorialstatedelay = 0;
				game.controltutorialstate++;
			}
		break;
		case 5:
		  game.press_left = true;
			game.controltutorialstatedelay++;
			if (game.controltutorialstatedelay > 60) {
				game.controltutorialstatedelay = 0;
				game.controltutorialstate++;
			}
		break;
		case 6:
			game.controltutorialstatedelay++;
			if (game.controltutorialstatedelay > 30) {
				game.controltutorialstatedelay = 0;
				game.controltutorialstate++;
			}
		break;
		case 7:
			game.controltutorialstatedelay++;
			if (game.controltutorialstatedelay > 5) {
				game.controltutorialstatedelay = 0;
				game.controltutorialstate++;
			}
		break;
		case 8:
			game.jumppressed = 5;	game.jumpheld = true;
			game.controllerp2 = 10;
		  game.controltutorialstate++;
		break;
		case 9:
			game.controltutorialstatedelay++;
			if (game.controltutorialstatedelay > 45) {
				game.controltutorialstatedelay = 0;
				game.controltutorialstate++;
			}
		break;
		case 10:
			game.jumppressed = 5;	game.jumpheld = true;
			game.controllerp2 = 10;
		  game.controltutorialstate++;
		break;
		case 11:
			game.controltutorialstatedelay++;
			if (game.controltutorialstatedelay > 45) {
				game.controltutorialstatedelay = 0;
				game.controltutorialstate++;
			}
		break;
		//Case 14 on is loopable
		case 12:
		  game.press_right = true;
			game.controltutorialstatedelay++;
			if (game.controltutorialstatedelay > 28) {
				game.controltutorialstatedelay = 0;
				game.controltutorialstate++;
			}
		break;
		case 13:
		  game.press_left = true;
			game.controltutorialstatedelay++;
			if (game.controltutorialstatedelay == 30) {
				game.jumppressed = 5;	game.jumpheld = true;
				game.controllerp2 = 10;
			}
			if (game.controltutorialstatedelay > 45) {
				game.controltutorialstatedelay = 0;
				game.controltutorialstate++;
			}
		break;
		case 14:
		  game.press_right = true;
			game.controltutorialstatedelay++;
			if (game.controltutorialstatedelay == 30) {
				if(Math.random()*100>50){
					game.jumppressed = 5;	game.jumpheld = true;
					game.controllerp2 = 10;
				}
			}
			if (game.controltutorialstatedelay > 45) {
				game.controltutorialstatedelay = 0;
				game.controltutorialstate++;
			}
		break;
		case 15:
		  game.press_left = true;
			game.controltutorialstatedelay++;
			if (game.controltutorialstatedelay == 30) {
				if(Math.random()*100>50){
					game.jumppressed = 5;	game.jumpheld = true;
					game.controllerp2 = 10;
				}
			}
			if (game.controltutorialstatedelay > 45) {
				game.controltutorialstatedelay = 0;
				game.controltutorialstate = 14;
			}
		break;
	}
	
	//Entity type 0 is player controled
	for (i = 0; i < obj.nentity; i++) {
	  if (obj.entities[i].rule == 0) {
			if (game.press_left) {game.tapleft++; game.controllerp3++;
			}else { if (game.tapleft <= 4 && game.tapleft > 0) {if (obj.entities[i].vx < 0) obj.entities[i].vx = 0;}game.tapleft = 0;	}
			if (game.press_right) {game.tapright++; game.controllerp3++;
			}else { if (game.tapright <= 4 && game.tapright > 0) {if (obj.entities[i].vx > 0) obj.entities[i].vx = 0;}game.tapright = 0;}
			
			if(game.press_left){
				//obj.entities[i].vx = -4;
				if(game.controllerp3>=8){
					obj.entities[i].ax = -3;
					obj.entities[i].dir = 0;
				}
			}else if (game.press_right){
				//obj.entities[i].vx = 4;
				if(game.controllerp3>=8){
					obj.entities[i].ax = 3;
					obj.entities[i].dir = 1;
				}
			}else{
				//obj.entities[i].vx = 0;
				game.controllerp3 = 0;
			}
			
			if (game.jumppressed > 0) {
				game.jumppressed--;
				if (obj.entities[i].onground>0 && game.gravitycontrol == 0) {
					game.gravitycontrol = 1;
					obj.entities[i].vy = -4; obj.entities[i].ay = -3;
					music.playef(0, 10);
					game.jumppressed = 0;
					game.totalflips++;
				}
				if (obj.entities[i].onroof>0 && game.gravitycontrol == 1) {
					game.gravitycontrol = 0;
					obj.entities[i].vy = 4; obj.entities[i].ay = 3;
					music.playef(1, 10);
					game.jumppressed = 0;
					game.totalflips++;
				}
      }
		}
	}
	
	for (i = obj.nentity - 1; i >= 0;  i--) {
		if (!obj.entities[i].isplatform) {
			obj.updateentities(i, help, game, music);          // Behavioral logic
			obj.updateentitylogic(i, game);                    // Basic Physics
			obj.entitymapcollision(i, map);                    // Collisions with walls
		}
	}
	
	obj.entitycollisioncheck(dwgfx, game, map, music);         // Check ent v ent collisions, update states
	
	//warp around x axis!
	for (i = 0; i < obj.nentity;  i++) {
		if (obj.entities[i].type < 50) { //Don't warp warp lines
			if (obj.entities[i].size < 12) { //Don't wrap SWN enemies
				if (obj.entities[i].xp <= -10) {
					obj.entities[i].xp += 320;
				}else {
					if (obj.entities[i].xp > 310) {
						obj.entities[i].xp -= 320;
					}
				}
			}
		}
	}	
}
