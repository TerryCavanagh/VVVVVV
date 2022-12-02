import flash.geom.Point;
import starling.textures.Texture;
public function titlerender(key:KeyPoll, dwgfx:dwgraphicsclass, map:mapclass, game:gameclass, obj:entityclass, help:helpclass):void {
  dwgfx.drawbutton(game, help);
	
	//dwgfx.backbuffer.lock();
	
	if (!game.menustart) {
		dwgfx.cls(0x000000);
		tr = 164 - (help.glow / 2) - Math.random() * 4;
		tg = 164 - (help.glow / 2) - Math.random() * 4;
		tb = 164 - (help.glow / 2) - Math.random() * 4;
		
		temp = 100;
		dwgfx.drawsprite((160 - 96) + 0 * 32, temp, 23, tr, tg, tb);
		dwgfx.drawsprite((160 - 96) + 1 * 32, temp, 23, tr, tg, tb);
		dwgfx.drawsprite((160 - 96) + 2 * 32, temp, 23, tr, tg, tb);
		dwgfx.drawsprite((160 - 96) + 3 * 32, temp, 23, tr, tg, tb);
		dwgfx.drawsprite((160 - 96) + 4 * 32, temp, 23, tr, tg, tb);
		dwgfx.drawsprite((160 - 96) + 5 * 32, temp, 23, tr, tg, tb);
		
		
		dwgfx.print(5, 135, "[ tap to start ]", tr, tg, tb, true);	
		//dwgfx.print(5, 195, "ACTION = Space, Z, or V", tr*0.5, tg*0.5, tb*0.5, true);	
		
		//dwgfx.print(5, 215, "Press CTRL-F for Fullscreen", tr, tg, tb, true);	
		
		/*dwgfx.print(5, 5, "IGF WIP Build, 29th Oct '09", tr, tg, tb, true);			
		dwgfx.print(5, 200, "Game by Terry Cavanagh", tr, tg, tb, true);
		dwgfx.print(5, 210, "Music by Magnus Palsson", tr, tg, tb, true);
		dwgfx.print(5, 220, "Roomnames by Bennett Foddy", tr, tg, tb, true);*/
	}else {
		dwgfx.cls(dwgfx.tower_bgdarkcol[map.colstate]);
		if(!game.colourblindmode) dwgfx.drawtowerbackgroundsolo(map);
		
		tr = map.r - (help.glow / 4) - Math.random() * 4;
		tg = map.g - (help.glow / 4) - Math.random() * 4;
		tb = map.b - (help.glow / 4) - Math.random() * 4;
		if (tr < 0) tr = 0; if(tr>255) tr=255;
		if (tg < 0) tg = 0; if(tg>255) tg=255;
		if (tb < 0) tb = 0; if(tb>255) tb=255;
		
		temp = 30+20;
		if (game.currentmenuname == "mainmenu") {
			dwgfx.drawsprite((160 - 96) + 0 * 32, temp, 23, tr, tg, tb);
			dwgfx.drawsprite((160 - 96) + 1 * 32, temp, 23, tr, tg, tb);
			dwgfx.drawsprite((160 - 96) + 2 * 32, temp, 23, tr, tg, tb);
			dwgfx.drawsprite((160 - 96) + 3 * 32, temp, 23, tr, tg, tb);
			dwgfx.drawsprite((160 - 96) + 4 * 32, temp, 23, tr, tg, tb);
			dwgfx.drawsprite((160 - 96) + 5 * 32, temp, 23, tr, tg, tb);
			dwgfx.print( 320 - 8 * 11, 2, "more games", tr / 2, tg / 2, tb / 2);
			dwgfx.print( 320 - 8 * 8, 230, "credits", tr / 2, tg / 2, tb / 2);
			dwgfx.print( 8, 230, "options", tr / 2, tg / 2, tb / 2);
		}else if (game.currentmenuname == "options") {
			/*
			if (game.currentmenuoption == 0) {
				dwgfx.bigprint( -1, 30, "Accessibility", tr, tg, tb, true);
				dwgfx.print( -1, 65, "Disable screen effects, enable", tr, tg, tb, true);
				dwgfx.print( -1, 75, "slowdown modes or invincibility", tr, tg, tb, true);
			}else if (game.currentmenuoption == 1) {
				dwgfx.bigprint( -1, 30, "Unlock Play Modes", tr, tg, tb, true);
				dwgfx.print( -1, 65, "Unlock parts of the game normally", tr, tg, tb, true);
				dwgfx.print( -1, 75, "unlocked as you progress", tr, tg, tb, true);
			}else if (game.currentmenuoption == 2) {
				dwgfx.bigprint( -1, 30, "Clear Data", tr, tg, tb, true);
				dwgfx.print( -1, 65, "Delete your save data", tr, tg, tb, true);
				dwgfx.print( -1, 75, "and unlocked play modes", tr, tg, tb, true);
			}
			*/
		}else if (game.currentmenuname == "graphicoptions") {
			if (game.advanced_mode) {
				if (game.currentmenuoption == 0) {
					dwgfx.bigprint( -1, 30, "Screen Mode", tr, tg, tb, true);
					dwgfx.print( -1, 65, "Change between windowed", tr, tg, tb, true);
					dwgfx.print( -1, 75, "and fullscreen modes", tr, tg, tb, true);
				}else if (game.currentmenuoption == 1) {
					dwgfx.bigprint( -1, 20, "Enable Hardware", tr, tg, tb, true);
					dwgfx.bigprint( -1, 45, "Acceleration", tr, tg, tb, true);
					dwgfx.print( -1, 85, "Uses graphics hardware to manually", tr, tg, tb, true);
					dwgfx.print( -1, 95, "manage screen scaling. Can cause", tr, tg, tb, true);
					dwgfx.print( -1, 105, "slowdown on some computers.", tr, tg, tb, true);
				}else if (game.currentmenuoption == 2) {
					dwgfx.bigprint( -1, 30, "Screen Scaling", tr, tg, tb, true);
					dwgfx.print( -1, 65, "Manually adjust scaling ratio. Full", tr, tg, tb, true);
					dwgfx.print( -1, 75, "scaling can cause distortions if", tr, tg, tb, true);
					dwgfx.print( -1, 85, "screen smoothing is disabled.", tr, tg, tb, true);
					switch(game.advanced_scaling) {
						case 0: dwgfx.print( -1, 105, "[Full]   x1    x2    x3    x4 ", tr, tg, tb, true); break;
						case 1: dwgfx.print( -1, 105, " Full   [x1]   x2    x3    x4 ", tr, tg, tb, true); break;
						case 2: dwgfx.print( -1, 105, " Full    x1   [x2]   x3    x4 ", tr, tg, tb, true); break;
						case 3: dwgfx.print( -1, 105, " Full    x1    x2   [x3]   x4 ", tr, tg, tb, true); break;
						case 4: dwgfx.print( -1, 105, " Full    x1    x2    x3   [x4]", tr, tg, tb, true); break;
					}
				} else if (game.currentmenuoption == 3) {
					dwgfx.bigprint( -1, 30, "Screen Smoothing", tr, tg, tb, true);
					dwgfx.print( -1, 65, "Scales the screen evenly, but", tr, tg, tb, true);
					dwgfx.print( -1, 75, "causes a blurring effect.", tr, tg, tb, true);
				} 
			}else{
				if (game.currentmenuoption == 0) {
					dwgfx.bigprint( -1, 30, "Screen Mode", tr, tg, tb, true);
					dwgfx.print( -1, 65, "Change between windowed", tr, tg, tb, true);
					dwgfx.print( -1, 75, "and fullscreen modes", tr, tg, tb, true);
				}else if (game.currentmenuoption == 1) {
					dwgfx.bigprint( -1, 20, "Disable Hardware", tr, tg, tb, true);
					dwgfx.bigprint( -1, 45, "Acceleration", tr, tg, tb, true);
					dwgfx.print( -1, 85, "May improve fullscreen performance on", tr, tg, tb, true);
					dwgfx.print( -1, 95, "some computers. Provides additional", tr, tg, tb, true);
					dwgfx.print( -1, 105, "scaling and smoothing options.", tr, tg, tb, true);
				} 
			}
	  }else if (game.currentmenuname == "credits") {
			dwgfx.print( -1, 50, "VVVVVV is a game by", tr, tg, tb, true);
			dwgfx.bigprint( 40, 65, "Terry Cavanagh", tr, tg, tb, true, 2);
			
			dwgfx.drawimagecol(7, -1, 86, tr *0.75, tg *0.75, tb *0.75, true);
		  //dwgfx.print( 40, 85, "http://www.distractionware.com", tr, tg, tb, true);
			
			dwgfx.print( -1, 120, "and features music by", tr, tg, tb, true);
			dwgfx.bigprint( 40, 135, "Magnus Palsson", tr, tg, tb, true, 2);
			dwgfx.drawimagecol(8, -1, 156, tr *0.75, tg *0.75, tb *0.75, true);
		  //dwgfx.print( 40, 155, "http://souleye.madtracker.net", tr, tg, tb, true);
		}else if (game.currentmenuname == "credits2") {
			dwgfx.print( -1, 30, "Roomnames are by", tr, tg, tb, true);
			dwgfx.bigprint( 40, 45, "Bennett Foddy", tr, tg, tb, true);
			dwgfx.drawimagecol(9, -1, 66, tr*0.75, tg *0.75, tb *0.75, true);
		  //dwgfx.print( 40, 80, "http://www.distractionware.com", tr, tg, tb);
			
			dwgfx.print( -1, 100, "Beta Testing by", tr, tg, tb, true);
			dwgfx.bigprint( 40, 110, "Sam Kaplan", tr, tg, tb, true);
			dwgfx.bigprint( 40, 130, "Pauli Kohberger", tr, tg, tb, true);
			dwgfx.print( -1, 160, "Ending Picture by", tr, tg, tb, true);
			dwgfx.bigprint( 40, 170, "Pauli Kohberger", tr, tg, tb, true);
		}else if (game.currentmenuname == "credits3") {
			dwgfx.print( -1, 20, "VVVVVV is supported by", tr, tg, tb, true);
			dwgfx.print( 40, 30, "the following patrons", tr, tg, tb, true);
			
			dwgfx.print( 80-16, 40+20, "Anders Ekermo", tr, tg, tb);
			dwgfx.print( 80-12, 54+20, "Andreas K|mper", tr, tg, tb);
			dwgfx.print( 80-8, 68+20, "Anthony Burch", tr, tg, tb);
			dwgfx.print( 80-4, 82+20, "Bennett Foddy", tr, tg, tb);
			dwgfx.print( 80, 96+20, "Brendan O'Sullivan", tr, tg, tb);
			dwgfx.print( 80+4, 110+20, "Christopher Armstrong", tr, tg, tb);
			dwgfx.print( 80+8, 124+20, "Daniel Benmergui", tr, tg, tb);
			dwgfx.print( 80+12, 138+20, "David Pittman", tr, tg, tb);
			dwgfx.print( 80+16, 152+20, "Ian Bogost", tr, tg, tb);
		}else if (game.currentmenuname == "credits4") {
			dwgfx.print( -1, 20, "VVVVVV is supported by", tr, tg, tb, true);
			dwgfx.print( 40, 30, "the following patrons", tr, tg, tb, true);
			dwgfx.print( 80-16, 20+40, "Ian Poma", tr, tg, tb);
			dwgfx.print( 80-12, 34+40, "Jaz McDougall", tr, tg, tb);
			dwgfx.print( 80-8, 48+40, "John Faulkenbury", tr, tg, tb);
			dwgfx.print( 80-4, 62+40, "Jonathan Whiting", tr, tg, tb);
			dwgfx.print( 80, 76+40, "Kyle Pulver", tr, tg, tb);
			dwgfx.print( 80+4, 90+40, "Markus Persson", tr, tg, tb);
			dwgfx.print( 80+8, 104+40, "Nathan Ostgard", tr, tg, tb);
			dwgfx.print( 80+12, 118+40, "Nick Easler", tr, tg, tb);
			dwgfx.print( 80+16, 132+40, "Stephen Lavelle", tr, tg, tb);
		}else if (game.currentmenuname == "credits5") {
			dwgfx.print( -1, 20, "and also by", tr, tg, tb, true);
			
			dwgfx.print( 80, 40,"Adam Wendt", tr, tg, tb);
			dwgfx.print( 80, 50,"Andreas J{rgensen", tr, tg, tb);
			dwgfx.print( 80, 60,"}ngel Louzao Penalva", tr, tg, tb);
			dwgfx.print( 80, 70,"Ashley Burton", tr, tg, tb);
			dwgfx.print( 80, 80,"Aubrey Hesselgren", tr, tg, tb);
			dwgfx.print( 80, 90,"Bradley Rose", tr, tg, tb);
			dwgfx.print( 80, 100,"Brendan Urquhart", tr, tg, tb);
			dwgfx.print( 80, 110,"Chris Ayotte", tr, tg, tb);
			dwgfx.print( 80, 120,"Christopher Zamanillo", tr, tg, tb);
			dwgfx.print( 80, 130,"Daniel Schuller", tr, tg, tb);
			dwgfx.print( 80, 140,"Hybrid Mind Studios", tr, tg, tb);
			dwgfx.print( 80, 150,"Emilie McGinley", tr, tg, tb);
			dwgfx.print( 80, 160,"Francisco Solares", tr, tg, tb);
			dwgfx.print( 80, 170,"Hal Helms", tr, tg, tb);
		}else if (game.currentmenuname == "credits6") {
			dwgfx.print( -1, 20, "and also by", tr, tg, tb, true);
			
			dwgfx.print( 80, 40,"Hayden Scott-Baron", tr, tg, tb);
			dwgfx.print( 80, 50,"Hermit Games", tr, tg, tb);
			dwgfx.print( 80, 60,"Ido Yehieli", tr, tg, tb);
			dwgfx.print( 80, 70,"Jade Vault Games", tr, tg, tb);
			dwgfx.print( 80, 80,"James Andrews", tr, tg, tb);
			dwgfx.print( 80, 90,"James Riley", tr, tg, tb);
			dwgfx.print( 80, 100,"James Hsieh", tr, tg, tb);
			dwgfx.print( 80, 110,"Jasper Byrne", tr, tg, tb);
			dwgfx.print( 80, 120,"Jedediah Baker", tr, tg, tb);
			dwgfx.print( 80, 130,"Jens Bergensten", tr, tg, tb);
			dwgfx.print( 80, 140,"Jeremy J. Penner", tr, tg, tb);
			dwgfx.print( 80, 150,"Jeremy Peterson", tr, tg, tb);
			dwgfx.print( 80, 160,"Jim McGinley", tr, tg, tb);
			dwgfx.print( 80, 170,"Jonathan Cartwright", tr, tg, tb);
		}else if (game.currentmenuname == "credits7") {
			dwgfx.print( -1, 20, "and also by", tr, tg, tb, true);
			
			dwgfx.print( 80, 40,"John Nesky", tr, tg, tb);
			dwgfx.print( 80, 50,"Jos Yule", tr, tg, tb);
			dwgfx.print( 80, 60,"Jose Flores", tr, tg, tb);
			dwgfx.print( 80, 70,"Josh Bizeau", tr, tg, tb);
			dwgfx.print( 80, 80,"Joshua Buergel", tr, tg, tb);
			dwgfx.print( 80, 90,"Joshua Hochner", tr, tg, tb);
			dwgfx.print( 80, 100,"Kurt Ostfeld", tr, tg, tb);
			dwgfx.print( 80, 110,"Magnus Palsson", tr, tg, tb);
			dwgfx.print( 80, 120,"Mark Neschadimenko", tr, tg, tb);
			dwgfx.print( 80, 130,"Matt Antonellis", tr, tg, tb);
			dwgfx.print( 80, 140,"Matthew Reppert", tr, tg, tb);
			dwgfx.print( 80, 150,"Michael Falkensteiner", tr, tg, tb);
			dwgfx.print( 80, 160,"Michael Vendittelli", tr, tg, tb);
			dwgfx.print( 80, 170,"Mike Kasprzak", tr, tg, tb);
		}else if (game.currentmenuname == "credits8") {
			dwgfx.print( -1, 20, "and also by", tr, tg, tb, true);
			
			dwgfx.print( 80, 70,"Mitchel Stein", tr, tg, tb);
			dwgfx.print( 80, 80,"Sean Murray", tr, tg, tb);
			dwgfx.print( 80, 90,"Simon Michael", tr, tg, tb);
			dwgfx.print( 80, 100,"Simon Schmid", tr, tg, tb);
			dwgfx.print( 80, 110,"Stephen Maxwell", tr, tg, tb);
			dwgfx.print( 80, 120,"Swing Swing Submarine", tr, tg, tb);
			dwgfx.print( 80, 130,"Tam Toucan", tr, tg, tb);
			dwgfx.print( 80, 140,"Terry Dooher", tr, tg, tb);
			dwgfx.print( 80, 150,"Tim W.", tr, tg, tb);
			dwgfx.print( 80, 160,"Timothy Bragan", tr, tg, tb);
		}else if (game.currentmenuname == "credits9") {
			dwgfx.print( -1, 20, "and thanks also to:", tr, tg, tb, true);
			
			dwgfx.bigprint(80, 60, "You!", tr, tg, tb, true);
			
			dwgfx.print( 80, 100, "Your support makes it possible", tr, tg, tb,true);
			dwgfx.print( 80, 110,"for me to continue making the", tr, tg, tb,true);
			dwgfx.print( 80, 120,"games I want to make, now", tr, tg, tb,true);
			dwgfx.print( 80, 130, "and into the future.", tr, tg, tb, true);
			
			dwgfx.print( 80, 150,"Thank you!", tr, tg, tb,true);
		}else if (game.currentmenuname == "setinvincibility") {
			dwgfx.print( -1, 90, "Warning! Enabling invincibility", tr, tg, tb, true);
			dwgfx.print( -1, 100, "requires a game restart, and will", tr, tg, tb, true);
			dwgfx.print( -1, 110, "delete your current saves.", tr, tg, tb, true);
			dwgfx.print( -1, 120, "Is this ok?", tr, tg, tb, true);
		}else if (game.currentmenuname == "setslowdown1") {
			dwgfx.print( -1, 90, "Warning! Changing the game speed", tr, tg, tb, true);
			dwgfx.print( -1, 100, "requires a game restart, and will", tr, tg, tb, true);
			dwgfx.print( -1, 110, "delete your current saves.", tr, tg, tb, true);
			dwgfx.print( -1, 120, "Is this ok?", tr, tg, tb, true);
		}else if (game.currentmenuname == "setslowdown2") {
			dwgfx.bigprint( -1, 40, "Game Speed", tr, tg, tb, true);		
			dwgfx.print( -1, 75, "Select a new game speed below.", tr, tg, tb, true);
			/*
			if (stage.frameRate==30) {  dwgfx.print( -1, 105, "Game speed is normal.", tr/2, tg/2, tb/2, true);
			}else if (stage.frameRate==24){ dwgfx.print( -1, 105, "Game speed is at 80%", tr, tg, tb, true); 
			}else if (stage.frameRate==18){ dwgfx.print( -1, 105, "Game speed is at 60%", tr, tg, tb, true); 
			}else if (stage.frameRate==12){ dwgfx.print( -1, 105, "Game speed is at 40%", tr, tg, tb, true); 
			}
			*/
		}else if (game.currentmenuname == "newgamewarning") {
			dwgfx.print( -1, 100-32, "Are you sure? This will", tr, tg, tb, true);
			dwgfx.print( -1, 110-32, "delete your current saves...", tr, tg, tb, true);
		}else if (game.currentmenuname == "cleardatamenu") {
			dwgfx.print( -1, 100-32, "Are you sure you want to", tr, tg, tb, true);
			dwgfx.print( -1, 110-32, "delete all your saved data?", tr, tg, tb, true);
		}else if (game.currentmenuname == "startnodeathmode") {
			dwgfx.print( -1, 45, "Good luck!", tr, tg, tb, true);
			dwgfx.print( -1, 80, "You cannot save in this mode.", tr, tg, tb, true);
			dwgfx.print( -1, 100, "Would you like to disable the", tr, tg, tb, true);
			dwgfx.print( -1, 112, "cutscenes during the game?", tr, tg, tb, true);
		}else if (game.currentmenuname == "accessibility") {
			//if (game.currentmenuoption == 0) {
				dwgfx.bigprint( -1, 40, "Accessibility", tr, tg, tb, true);		
				if (!game.colourblindmode) {  dwgfx.print( -1, 75, "Backgrounds are ON.", tr, tg, tb, true);
				}else{ dwgfx.print( -1, 75, "Backgrounds are OFF.", tr/2, tg/2, tb/2, true); }
			//}else if (game.currentmenuoption == 1) {
				//dwgfx.bigprint( -1, 40, "Screen Effects", tr, tg, tb, true);		
			  //dwgfx.print( -1, 75, "Disables screen shakes and flashes.", tr, tg, tb, true);
				if (!game.noflashingmode) {  dwgfx.print( -1, 85, "Screen Effects are ON.", tr, tg, tb, true);
				}else{ dwgfx.print( -1, 85, "Screen Effects are OFF.", tr/2, tg/2, tb/2, true); }
			/*}else if (game.currentmenuoption == 2) {
				dwgfx.bigprint( -1, 40, "Invincibility", tr, tg, tb, true);		
			  dwgfx.print( -1, 75, "Provided to help disabled gamers", tr, tg, tb, true);
			  dwgfx.print( -1, 85, "explore the game. Can cause glitches.", tr, tg, tb, true);
				if (map.invincibility) {  dwgfx.print( -1, 105, "Invincibility is ON.", tr, tg, tb, true);
				}else{ dwgfx.print( -1, 105, "Invincibility is off.", tr/2, tg/2, tb/2, true); }
			}else if (game.currentmenuoption == 3) {
				dwgfx.bigprint( -1, 40, "Game Speed", tr, tg, tb, true);		
			  dwgfx.print( -1, 75, "May be useful for disabled gamers", tr, tg, tb, true);
			  dwgfx.print( -1, 85, "using one switch devices.", tr, tg, tb, true);
				/*
				if (stage.frameRate==30) {  dwgfx.print( -1, 105, "Game speed is normal.", tr/2, tg/2, tb/2, true);
				}else if (stage.frameRate==24){ dwgfx.print( -1, 105, "Game speed is at 80%", tr, tg, tb, true); 
				}else if (stage.frameRate==18){ dwgfx.print( -1, 105, "Game speed is at 60%", tr, tg, tb, true); 
				}else if (stage.frameRate==12){ dwgfx.print( -1, 105, "Game speed is at 40%", tr, tg, tb, true); }
				*/
			//}
		}else if (game.currentmenuname == "playint1" || game.currentmenuname == "playint2") {
				dwgfx.print( -1, 25, "Who do you want to play", tr, tg, tb, true);
				dwgfx.print( -1, 35, "the level with?", tr, tg, tb, true);
		}else if (game.currentmenuname == "playmodes") {
			if (!game.mobilemenu) {
				if (game.currentmenuoption == 0) {
					dwgfx.bigprint( -1, 30, "Time Trials", tr, tg, tb, true);
					dwgfx.print( -1, 65, "Replay any level in the game in", tr, tg, tb, true);
					dwgfx.print( -1, 75, "a competitive time trial mode.", tr, tg, tb, true);
					
					/*
					if (swfStage.frameRate < 30 || map.invincibility) {
						dwgfx.print( -1, 105, "Time Trials are not available", tr, tg, tb, true);
						dwgfx.print( -1, 115, "with slowdown or invincibility.", tr, tg, tb, true);
					}
					*/
				}else if (game.currentmenuoption == 1) {
					dwgfx.bigprint( -1, 30, "Intermissions", tr, tg, tb, true);
					dwgfx.print( -1, 65, "Replay the intermission levels.", tr, tg, tb, true);
					
					if (!game.unlock[15] && !game.unlock[16]) {
						dwgfx.print( -1, 95, "TO UNLOCK: Complete the", tr, tg, tb, true);
						dwgfx.print( -1, 105, "intermission levels in-game.", tr, tg, tb, true);
					}
				}else if (game.currentmenuoption == 2) {
					dwgfx.bigprint( -1, 30, "No Death Mode", tr, tg, tb, true);
					dwgfx.print( -1, 65, "Play the entire game", tr, tg, tb, true);
					dwgfx.print( -1, 75, "without dying once.", tr, tg, tb, true);
					
					/*
					if (swfStage.frameRate < 30 || map.invincibility) {
						dwgfx.print( -1, 105, "No death mode is not available", tr, tg, tb, true);
						dwgfx.print( -1, 115, "with slowdown or invincibility.", tr, tg, tb, true);
					}else 
					*/
					if (!game.unlock[17]) {
						dwgfx.print( -1, 105, "TO UNLOCK: Achieve an S-rank or", tr, tg, tb, true);
						dwgfx.print( -1, 115, "above in at least 4 time trials.", tr, tg, tb, true);
					}
				}else if (game.currentmenuoption == 3) {
					dwgfx.bigprint( -1, 30, "Flip Mode", tr, tg, tb, true);
					dwgfx.print( -1, 65, "Flip the entire game vertically.", tr, tg, tb, true);
					dwgfx.print( -1, 75, "Compatible with other game modes.", tr, tg, tb, true);
					
					if (game.unlock[18]) {
						if (dwgfx.setflipmode) {  dwgfx.print( -1, 105, "Currently ENABLED!", tr, tg, tb, true);
						}else{ dwgfx.print( -1, 105, "Currently Disabled.", tr/2, tg/2, tb/2, true); }
					}else{
						dwgfx.print( -1, 105, "TO UNLOCK: Complete the game.", tr, tg, tb, true);
					}
				}
			}
		}else if (game.currentmenuname == "flipmode_mobile") {
			dwgfx.bigprint( -1, 30, "Flip Mode", tr, tg, tb, true);
			dwgfx.print( -1, 65, "Flip the entire game vertically.", tr, tg, tb, true);
			dwgfx.print( -1, 75, "Compatible with other game modes.", tr, tg, tb, true);
			
			if (game.unlock[18]) {
				if (dwgfx.setflipmode) {  dwgfx.print( -1, 105, "Currently ENABLED!", tr, tg, tb, true);
				}else{ dwgfx.print( -1, 105, "Currently Disabled.", tr/2, tg/2, tb/2, true); }
			}else{
				dwgfx.print( -1, 105, "TO UNLOCK: Complete the game.", tr, tg, tb, true);
			}
		}else if (game.currentmenuname == "youwannaquit") {
			dwgfx.print( -1, 75, "Are you sure you want to quit?", tr, tg, tb, true);
		}else if (game.currentmenuname == "continue") {
			dwgfx.crewframedelay--;
			if (dwgfx.crewframedelay <= 0) {
				dwgfx.crewframedelay = 8;
				dwgfx.crewframe = (dwgfx.crewframe + 1) % 2;
			}
			if (game.currentmenuoption == 0) {
				//Show teleporter save info 
				dwgfx.drawpixeltextbox(25, 65-20, 270, 90, 34,12, 65, 185, 207,0,4);
				
				dwgfx.bigprint(-1, 20, "Tele Save", tr, tg, tb, true);
				dwgfx.print(0, 80-20, game.tele_currentarea, 25, 255 - (help.glow / 2), 255 - (help.glow / 2), true);
				for (i = 0; i < 6; i++) {
					dwgfx.drawcrewman(169-(3*42)+(i*42), 95-20, i, game.tele_crewstats[i], help, true);
				}
				dwgfx.print(160 - 84, 132-20, game.tele_gametime, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
				dwgfx.print(160 + 40, 132-20, help.number(game.tele_trinkets), 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
						
				dwgfx.drawspritesetcol(50, 126-20, 50, 18, help);
				dwgfx.drawspritesetcol(175, 126-20, 22, 18, help);
			}else if (game.currentmenuoption == 1) {
				//Show quick save info
			  dwgfx.drawpixeltextbox(25, 65-20, 270, 90, 34,12, 65, 185, 207,0,4);
				
				dwgfx.bigprint(-1, 20, "Quick Save", tr, tg, tb, true);
				dwgfx.print(0, 80-20, game.quick_currentarea, 25, 255 - (help.glow / 2), 255 - (help.glow / 2), true);
				for (i = 0; i < 6; i++) {
					dwgfx.drawcrewman(169-(3*42)+(i*42), 95-20, i, game.quick_crewstats[i], help, true);
				}
				dwgfx.print(160 - 84, 132-20, game.quick_gametime, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
				dwgfx.print(160 + 40, 132-20, help.number(game.quick_trinkets), 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
						
				dwgfx.drawspritesetcol(50, 126-20, 50, 18, help);
				dwgfx.drawspritesetcol(175, 126-20, 22, 18, help);
			}
		}else if (game.currentmenuname == "gameover" || game.currentmenuname == "gameover2") {
			dwgfx.bigprint( -1, 25, "GAME OVER", tr, tg, tb, true, 3);
			
			dwgfx.crewframedelay--;
			if (dwgfx.crewframedelay <= 0) {
				dwgfx.crewframedelay = 8;
				dwgfx.crewframe = (dwgfx.crewframe + 1) % 2;
			}
			for (i = 0; i < 6; i++) {
				dwgfx.drawcrewman(169-(3*42)+(i*42), 68, i, game.crewstats[i], help, true);
			}
			tempstring = "You rescued " + help.number(game.crewrescued()) + " crewmates";
			dwgfx.print(0, 100, tempstring, tr, tg, tb, true);
			
			tempstring = "and found " + help.number(game.trinkets) + " trinkets.";
			dwgfx.print(0, 110, tempstring, tr, tg, tb, true);
			
			tempstring = "You managed to reach:";
			dwgfx.print(0, 145, tempstring, tr, tg, tb, true);
			dwgfx.print(0, 155, game.hardestroom, tr, tg, tb, true);
			
			if (game.crewrescued() == 1) {        tempstring = "Keep trying! You'll get there!";	
			}else if (game.crewrescued() == 2) {  tempstring = "Nice one!";	
			}else if (game.crewrescued() == 3) {  tempstring = "Wow! Congratulations!";	
			}else if (game.crewrescued() == 4) {  tempstring = "Incredible!";	
			}else if (game.crewrescued() == 5) {  tempstring = "Unbelievable! Well done!";	
			}else if (game.crewrescued() == 6) {  tempstring = "Er, how did you do that?";	
			}
			
			dwgfx.print(0, 190, tempstring, tr, tg, tb, true);
		}else if (game.currentmenuname == "nodeathmodecomplete" || game.currentmenuname == "nodeathmodecomplete2") {
			dwgfx.bigprint( -1, 8, "WOW", tr, tg, tb, true, 4);
			
			dwgfx.crewframedelay--;
			if (dwgfx.crewframedelay <= 0) {
				dwgfx.crewframedelay = 8;
				dwgfx.crewframe = (dwgfx.crewframe + 1) % 2;
			}
			for (i = 0; i < 6; i++) {
				dwgfx.drawcrewman(169-(3*42)+(i*42), 68, i, game.crewstats[i], help, true);
			}
			tempstring = "You rescued all the crewmates!";
			dwgfx.print(0, 100, tempstring, tr, tg, tb, true);
			
			tempstring = "And you found " + help.number(game.trinkets) + " trinkets.";
			dwgfx.print(0, 110, tempstring, tr, tg, tb, true);
			
			dwgfx.print(0, 160, "A new trophy has been awarded and", tr, tg, tb, true);
			dwgfx.print(0, 170, "placed in the secret lab to", tr, tg, tb, true);
			dwgfx.print(0, 180, "acknowledge your achievement!", tr, tg, tb, true);
		}else if (game.currentmenuname == "timetrialcomplete" || game.currentmenuname == "timetrialcomplete2"
		       || game.currentmenuname == "timetrialcomplete3" || game.currentmenuname == "timetrialcomplete4") {
			dwgfx.bigprint( -1, 20, "Results", tr, tg, tb, true, 3);
			
			tempstring = game.resulttimestring(help) + " / " + game.partimestring(help);
			
			dwgfx.drawspritesetcol(30, 80-15, 50, 22, help);
			dwgfx.print(65, 80-15, "TIME TAKEN:", 255, 255, 255);
			dwgfx.print(65, 90-15, tempstring, tr, tg, tb);
			if (game.timetrialresulttime <= game.timetrialpar) {
				dwgfx.print(220, 85-15, "+1 Rank!", 255, 255, 255);
			}
			
			tempstring = String(game.deathcounts);
			dwgfx.drawspritesetcol(30-4, 80+20-4, 12, 22, help);
			dwgfx.print(65, 80+20, "NUMBER OF DEATHS:", 255, 255, 255);
			dwgfx.print(65, 90+20, tempstring, tr, tg, tb);
			if (game.deathcounts == 0) {
				dwgfx.print(220, 85+20, "+1 Rank!", 255, 255, 255);
			}
			
			tempstring = String(game.trinkets) + " of " + String(game.timetrialshinytarget);
			dwgfx.drawspritesetcol(30, 80+55, 22, 22, help);
			dwgfx.print(65, 80+55, "SHINY TRINKETS:", 255, 255, 255);
			dwgfx.print(65, 90+55, tempstring, tr, tg, tb);
			if (game.trinkets >= game.timetrialshinytarget) {
				dwgfx.print(220, 85+55, "+1 Rank!", 255, 255, 255);
			}
			
			if (game.currentmenuname == "timetrialcomplete2" || game.currentmenuname == "timetrialcomplete3") {
				dwgfx.bigprint( 100, 175, "Rank:", tr, tg, tb, false, 2);
			}
			
			if (game.currentmenuname == "timetrialcomplete3"){
			  switch(game.timetrialrank) {
					case 0:	dwgfx.bigprint( 195, 165, "B", 255, 255, 255, false, 4); break;
					case 1:	dwgfx.bigprint( 195, 165, "A", 255, 255, 255, false, 4); break;
					case 2:	dwgfx.bigprint( 195, 165, "S", 255, 255, 255, false, 4); break;
					case 3:	dwgfx.bigprint( 195, 165, "V", 255, 255, 255, false, 4); break;
				}
			}
	  }else if (game.currentmenuname == "unlockmenutrials") {
			/*
			dwgfx.bigprint( -1, 30, "Unlock Time Trials", tr, tg, tb, true);
			dwgfx.print( -1, 65, "You can unlock each time", tr, tg, tb, true);
			dwgfx.print( -1, 75, "trial seperately.", tr, tg, tb, true);
			*/
	  }else if (game.currentmenuname == "timetrials") {
			/*
			if (game.currentmenuoption == 0) {
				if(game.unlock[9]){
					dwgfx.bigprint( -1, 30, "Space Station 1", tr, tg, tb, true);
					if (game.besttimes[0] == -1) {
					  dwgfx.print( -1, 75, "Not yet attempted", tr, tg, tb, true);	
					}else {
						dwgfx.print( 16, 65, "BEST TIME  ", tr, tg, tb);	
						dwgfx.print( 16, 75, "BEST SHINY ", tr, tg, tb);	
						dwgfx.print( 16, 85, "BEST LIVES ", tr, tg, tb);	
						dwgfx.print( 110, 65, game.timetstring(game.besttimes[0], help), tr, tg, tb);	
						dwgfx.print( 110, 75, String(game.besttrinkets[0])+"/2", tr, tg, tb);	
						dwgfx.print( 110, 85, String(game.bestlives[0]), tr, tg, tb);	
						
						
						dwgfx.print( 170, 65, "PAR TIME    1:15", tr, tg, tb);	
						dwgfx.print( 170, 85, "Best Rank", tr, tg, tb);	
						switch(game.bestrank[0]) {
							case 0:	dwgfx.bigprint( 275, 82, "B", 225, 225, 225); break;
							case 1:	dwgfx.bigprint( 275, 82, "A", 225, 225, 225); break;
							case 2:	dwgfx.bigprint( 275, 82, "S", 225, 225, 225); break;
							case 3:	dwgfx.bigprint( 275, 82, "V", 225, 225, 225); break;
						}
					}
					
				}else {
					dwgfx.bigprint( -1, 30, "???", tr, tg, tb, true);
					dwgfx.print( -1, 60, "TO UNLOCK:", tr, tg, tb, true);
					dwgfx.print( -1, 75, "Rescue Violet", tr, tg, tb, true);
					dwgfx.print( -1, 85, "Find three trinkets", tr, tg, tb, true);
				}
			}else if (game.currentmenuoption == 1) {
				if(game.unlock[10]){
					dwgfx.bigprint( -1, 30, "The Laboratory", tr, tg, tb, true);
					if (game.besttimes[1] == -1) {
					  dwgfx.print( -1, 75, "Not yet attempted", tr, tg, tb, true);	
					}else {
						dwgfx.print( 16, 65, "BEST TIME  ", tr, tg, tb);	
						dwgfx.print( 16, 75, "BEST SHINY ", tr, tg, tb);	
						dwgfx.print( 16, 85, "BEST LIVES ", tr, tg, tb);	
						dwgfx.print( 110, 65, game.timetstring(game.besttimes[1], help), tr, tg, tb);	
						dwgfx.print( 110, 75, String(game.besttrinkets[1])+"/4", tr, tg, tb);	
						dwgfx.print( 110, 85, String(game.bestlives[1]), tr, tg, tb);	
						
						
						dwgfx.print( 170, 65, "PAR TIME    2:45", tr, tg, tb);	
						dwgfx.print( 170, 85, "Best Rank", tr, tg, tb);	
						switch(game.bestrank[1]) {
							case 0:	dwgfx.bigprint( 275, 82, "B", 225, 225, 225); break;
							case 1:	dwgfx.bigprint( 275, 82, "A", 225, 225, 225); break;
							case 2:	dwgfx.bigprint( 275, 82, "S", 225, 225, 225); break;
							case 3:	dwgfx.bigprint( 275, 82, "V", 225, 225, 225); break;
						}
					}
					
				}else {
					dwgfx.bigprint( -1, 30, "???", tr, tg, tb, true);
					dwgfx.print( -1, 60, "TO UNLOCK:", tr, tg, tb, true);
					dwgfx.print( -1, 75, "Rescue Victoria", tr, tg, tb, true);
					dwgfx.print( -1, 85, "Find six trinkets", tr, tg, tb, true);
				}
			}else if (game.currentmenuoption == 2) {
				if(game.unlock[11]){
					dwgfx.bigprint( -1, 30, "The Tower", tr, tg, tb, true);
					if (game.besttimes[2] == -1) {
					  dwgfx.print( -1, 75, "Not yet attempted", tr, tg, tb, true);	
					}else {
						dwgfx.print( 16, 65, "BEST TIME  ", tr, tg, tb);	
						dwgfx.print( 16, 75, "BEST SHINY ", tr, tg, tb);	
						dwgfx.print( 16, 85, "BEST LIVES ", tr, tg, tb);	
						dwgfx.print( 110, 65, game.timetstring(game.besttimes[2], help), tr, tg, tb);	
						dwgfx.print( 110, 75, String(game.besttrinkets[2])+"/2", tr, tg, tb);	
						dwgfx.print( 110, 85, String(game.bestlives[2]), tr, tg, tb);	
						
						
						dwgfx.print( 170, 65, "PAR TIME    1:45", tr, tg, tb);	
						dwgfx.print( 170, 85, "Best Rank", tr, tg, tb);	
						switch(game.bestrank[2]) {
							case 0:	dwgfx.bigprint( 275, 82, "B", 225, 225, 225); break;
							case 1:	dwgfx.bigprint( 275, 82, "A", 225, 225, 225); break;
							case 2:	dwgfx.bigprint( 275, 82, "S", 225, 225, 225); break;
							case 3:	dwgfx.bigprint( 275, 82, "V", 225, 225, 225); break;
						}
					}
					
				}else {
					dwgfx.bigprint( -1, 30, "???", tr, tg, tb, true);
					dwgfx.print( -1, 60, "TO UNLOCK:", tr, tg, tb, true);
					dwgfx.print( -1, 75, "Rescue Vermilion", tr, tg, tb, true);
					dwgfx.print( -1, 85, "Find nine trinkets", tr, tg, tb, true);
				}
			}else if (game.currentmenuoption == 3) {
				if(game.unlock[12]){
					dwgfx.bigprint( -1, 30, "Space Station 2", tr, tg, tb, true);
					if (game.besttimes[3] == -1) {
					  dwgfx.print( -1, 75, "Not yet attempted", tr, tg, tb, true);	
					}else {
						dwgfx.print( 16, 65, "BEST TIME  ", tr, tg, tb);	
						dwgfx.print( 16, 75, "BEST SHINY ", tr, tg, tb);	
						dwgfx.print( 16, 85, "BEST LIVES ", tr, tg, tb);	
						dwgfx.print( 110, 65, game.timetstring(game.besttimes[3], help), tr, tg, tb);	
						dwgfx.print( 110, 75, String(game.besttrinkets[3])+"/5", tr, tg, tb);	
						dwgfx.print( 110, 85, String(game.bestlives[3]), tr, tg, tb);	
						
						
						dwgfx.print( 170, 65, "PAR TIME    3:20", tr, tg, tb);	
						dwgfx.print( 170, 85, "Best Rank", tr, tg, tb);	
						switch(game.bestrank[3]) {
							case 0:	dwgfx.bigprint( 275, 82, "B", 225, 225, 225); break;
							case 1:	dwgfx.bigprint( 275, 82, "A", 225, 225, 225); break;
							case 2:	dwgfx.bigprint( 275, 82, "S", 225, 225, 225); break;
							case 3:	dwgfx.bigprint( 275, 82, "V", 225, 225, 225); break;
						}
					}
					
				}else {
					dwgfx.bigprint( -1, 30, "???", tr, tg, tb, true);
					dwgfx.print( -1, 60, "TO UNLOCK:", tr, tg, tb, true);
					dwgfx.print( -1, 75, "Rescue Vitellary", tr, tg, tb, true);
					dwgfx.print( -1, 85, "Find twelve trinkets", tr, tg, tb, true);
				}
			}else if (game.currentmenuoption == 4) {
				if(game.unlock[13]){
					dwgfx.bigprint( -1, 30, "The Warp Zone", tr, tg, tb, true);
					if (game.besttimes[4] == -1) {
					  dwgfx.print( -1, 75, "Not yet attempted", tr, tg, tb, true);	
					}else {
						dwgfx.print( 16, 65, "BEST TIME  ", tr, tg, tb);	
						dwgfx.print( 16, 75, "BEST SHINY ", tr, tg, tb);	
						dwgfx.print( 16, 85, "BEST LIVES ", tr, tg, tb);	
						dwgfx.print( 110, 65, game.timetstring(game.besttimes[4], help), tr, tg, tb);	
						dwgfx.print( 110, 75, String(game.besttrinkets[4])+"/1", tr, tg, tb);	
						dwgfx.print( 110, 85, String(game.bestlives[4]), tr, tg, tb);	
						
						
						dwgfx.print( 170, 65, "PAR TIME    2:00", tr, tg, tb);	
						dwgfx.print( 170, 85, "Best Rank", tr, tg, tb);	
						switch(game.bestrank[4]) {
							case 0:	dwgfx.bigprint( 275, 82, "B", 225, 225, 225); break;
							case 1:	dwgfx.bigprint( 275, 82, "A", 225, 225, 225); break;
							case 2:	dwgfx.bigprint( 275, 82, "S", 225, 225, 225); break;
							case 3:	dwgfx.bigprint( 275, 82, "V", 225, 225, 225); break;
						}
					}
					
				}else {
					dwgfx.bigprint( -1, 30, "???", tr, tg, tb, true);
					dwgfx.print( -1, 60, "TO UNLOCK:", tr, tg, tb, true);
					dwgfx.print( -1, 75, "Rescue Verdigris", tr, tg, tb, true);
					dwgfx.print( -1, 85, "Find fifteen trinkets", tr, tg, tb, true);
				}
			}else if (game.currentmenuoption == 5) {
				if(game.unlock[14]){
					dwgfx.bigprint( -1, 30, "The Final Level", tr, tg, tb, true);
					if (game.besttimes[5] == -1) {
					  dwgfx.print( -1, 75, "Not yet attempted", tr, tg, tb, true);	
					}else {
						dwgfx.print( 16, 65, "BEST TIME  ", tr, tg, tb);	
						dwgfx.print( 16, 75, "BEST SHINY ", tr, tg, tb);	
						dwgfx.print( 16, 85, "BEST LIVES ", tr, tg, tb);	
						dwgfx.print( 110, 65, game.timetstring(game.besttimes[5], help), tr, tg, tb);	
						dwgfx.print( 110, 75, String(game.besttrinkets[5])+"/1", tr, tg, tb);	
						dwgfx.print( 110, 85, String(game.bestlives[5]), tr, tg, tb);	
						
						
						dwgfx.print( 170, 65, "PAR TIME    2:15", tr, tg, tb);	
						dwgfx.print( 170, 85, "Best Rank", tr, tg, tb);	
						switch(game.bestrank[5]) {
							case 0:	dwgfx.bigprint( 275, 82, "B", 225, 225, 225); break;
							case 1:	dwgfx.bigprint( 275, 82, "A", 225, 225, 225); break;
							case 2:	dwgfx.bigprint( 275, 82, "S", 225, 225, 225); break;
							case 3:	dwgfx.bigprint( 275, 82, "V", 225, 225, 225); break;
						}
					}
					
				}else {
					dwgfx.bigprint( -1, 30, "???", tr, tg, tb, true);
					dwgfx.print( -1, 60, "TO UNLOCK:", tr, tg, tb, true);
					dwgfx.print( -1, 75, "Complete the game", tr, tg, tb, true);
					dwgfx.print( -1, 85, "Find eighteen trinkets", tr, tg, tb, true);
				}
			}
			*/
		}else if (game.currentmenuname == "gamecompletecontinue") {
			dwgfx.bigprint( -1, 25, "Congratulations!", tr, tg, tb, true, 2);
			
			dwgfx.print( -1, 45, "Your save files have been updated.", tr, tg, tb, true);
			
			dwgfx.print( -1, 110, "If you want to keep exploring", tr, tg, tb, true);
			dwgfx.print( -1, 120, "the game, select CONTINUE", tr, tg, tb, true);
			dwgfx.print( -1, 130, "from the play menu.", tr, tg, tb, true);
		}else if (game.currentmenuname == "unlockmenu") {
			/*
			dwgfx.bigprint( -1, 25, "Unlock Play Modes", tr, tg, tb, true, 2);
			
			dwgfx.print( -1, 55, "From here, you may unlock parts", tr, tg, tb, true);
			dwgfx.print( -1, 65, "of the game that are normally", tr, tg, tb, true);
			dwgfx.print( -1, 75, "unlocked as you play.", tr, tg, tb, true);
			*/
		}else if (game.currentmenuname == "unlocktimetrial") {
			dwgfx.bigprint( -1, 45, "Congratulations!", tr, tg, tb, true, 2);
			
			dwgfx.print( -1, 125, "Your have unlocked", tr, tg, tb, true);
			dwgfx.print( -1, 135, "a new Time Trial.", tr, tg, tb, true);
		}else if (game.currentmenuname == "unlocktimetrials") {
			dwgfx.bigprint( -1, 45, "Congratulations!", tr, tg, tb, true, 2);
			
			dwgfx.print( -1, 125, "Your have unlocked some", tr, tg, tb, true);
			dwgfx.print( -1, 135, "new Time Trials.", tr, tg, tb, true);
		}else if (game.currentmenuname == "unlocknodeathmode") {
			dwgfx.bigprint( -1, 45, "Congratulations!", tr, tg, tb, true, 2);
			
			dwgfx.print( -1, 125, "Your have unlocked", tr, tg, tb, true);
			dwgfx.print( -1, 135, "No Death Mode.", tr, tg, tb, true);
		}else if (game.currentmenuname == "unlockflipmode") {
			dwgfx.bigprint( -1, 45, "Congratulations!", tr, tg, tb, true, 2);
			
			dwgfx.print( -1, 125, "Your have unlocked", tr, tg, tb, true);
			dwgfx.print( -1, 135, "Flip Mode.", tr, tg, tb, true);
		}else if (game.currentmenuname == "unlockintermission") {
			dwgfx.bigprint( -1, 45, "Congratulations!", tr, tg, tb, true, 2);
			
			dwgfx.print( -1, 125, "Your have unlocked", tr, tg, tb, true);
			dwgfx.print( -1, 135, "the intermission levels.", tr, tg, tb, true);
		}
			
		/*
		switch(game.mainmenu) {
			case 0:
				dwgfx.print(5, 115, "[ NEW GAME ]", tr, tg, tb, true);		
			break;
			case 1:
				if (game.telesummary == "") {
					dwgfx.print(5, 115, "[ no teleporter save ]", tr/3, tg/3, tb/3, true);		
				}else {
					dwgfx.print(5, 115, "[ RESTORE FROM LAST TELEPORTER ]", tr, tg, tb, true);		
					dwgfx.print(5, 125, game.telesummary, tr, tg, tb, true);	
				}
			break;
			case 2:
				if (game.quicksummary == "") {
					dwgfx.print(5, 115, "[ no quicksave ]", tr/3, tg/3, tb/3, true);		
				}else {
					dwgfx.print(5, 115, "[ RESTORE FROM LAST QUICKSAVE ]", tr, tg, tb, true);	
					dwgfx.print(5, 125, game.quicksummary, tr, tg, tb, true);		
				}
			break;
		}
		*/
		
		tr = tr * .8;
		tg = tg * .8;
		tb = tb * .8;
		if (tr < 0) tr = 0; if(tr>255) tr=255;
		if (tg < 0) tg = 0; if(tg>255) tg=255;
		if (tb < 0) tb = 0; if(tb>255) tb=255;
		if (game.currentmenuname == "timetrials" || game.currentmenuname == "unlockmenutrials") {
			dwgfx.drawmenu(game, tr, tg, tb, help, 15);
		}else if (game.currentmenuname == "unlockmenu") {
			dwgfx.drawmenu(game, tr, tg, tb, help, 15);
		}else if (game.currentmenuname == "playmodes") {
			dwgfx.drawmenu(game, tr, tg, tb, help, 20);
		}else if (game.currentmenuname == "mainmenu") {
			dwgfx.drawmenu(game, tr, tg, tb, help, 15);
		}else if (game.currentmenuname == "playerworlds"){
      dwgfx.drawmenu(game, tr, tg, tb, help, 15);
    }else if (game.currentmenuname == "levellist"){
      //dwgfx.drawlevelmenu(game, tr, tg, tb, 5);
			dwgfx.drawmenu(game, tr, tg, tb, help, 5);
    }else if (game.currentmenuname == "fullscreenmenu"){
      dwgfx.drawmenu(game, tr, tg, tb, help, 15);
    }else{
		  dwgfx.drawmenu(game, tr, tg, tb, help);
		}
		//dwgfx.print(5, 228, "Left/Right to Choose, V to Select", tr, tg, tb, true);	
	}
	
	if (game.test) {
		dwgfx.print(5, 5, game.teststring, 196, 196, 255, false);
	}

	dwgfx.drawfade();
	
	if (game.showloadingnotice && dwgfx.fademode == 1) {
		dwgfx.print(-1, 120, "LOADING", 196, 196, 196, true);
	}
	
	if (game.flashlight > 0 && !game.noflashingmode) {
		game.flashlight--;
		dwgfx.flashlight();
	}
	
	if (game.screenshake > 0  && !game.noflashingmode) {
		game.screenshake--;
		dwgfx.screenshake();
	}else{
	  dwgfx.render();
	}
	//dwgfx.backbuffer.unlock();
}

public function gamecompleterender(key:KeyPoll, dwgfx:dwgraphicsclass, game:gameclass, map:mapclass, obj:entityclass, help:helpclass):void {
  dwgfx.drawbutton(game, help);
	
	//dwgfx.backbuffer.lock();
	dwgfx.cls(dwgfx.tower_bgdarkcol[map.colstate]);
	
	if(!game.colourblindmode) dwgfx.drawtowerbackgroundsolo(map);
	//dwgfx.drawtowermap(map);
	
	for (i = 0; i < 6; i++) {
	  //dwgfx.drawsprite((160-96)+ i * 32, 10, 23, 96+(i*10)+(Math.random()*16), 196-(help.glow)-(Math.random()*16), 255 - (help.glow*2));
	}
	
	tr = map.r - (help.glow / 4) - Math.random() * 4;
	tg = map.g - (help.glow / 4) - Math.random() * 4;
	tb = map.b - (help.glow / 4) - Math.random() * 4;
	if (tr < 0) tr = 0; if(tr>255) tr=255;
	if (tg < 0) tg = 0; if(tg>255) tg=255;
	if (tb < 0) tb = 0; if(tb>255) tb=255;
	
	
	//rendering starts... here!
	
	if (dwgfx.onscreen(220 + game.creditposition)) {
		temp = 220 + game.creditposition;
	  dwgfx.drawsprite((160 - 96) + 0 * 32, temp, 23, tr, tg, tb);
		dwgfx.drawsprite((160 - 96) + 1 * 32, temp, 23, tr, tg, tb);
		dwgfx.drawsprite((160 - 96) + 2 * 32, temp, 23, tr, tg, tb);
		dwgfx.drawsprite((160 - 96) + 3 * 32, temp, 23, tr, tg, tb);
		dwgfx.drawsprite((160 - 96) + 4 * 32, temp, 23, tr, tg, tb);
		dwgfx.drawsprite((160 - 96) + 5 * 32, temp, 23, tr, tg, tb);
	}
	
	if (dwgfx.onscreen(290 + game.creditposition)) dwgfx.bigprint( -1, 290 + game.creditposition, "Starring", tr, tg, tb, true, 2);
	
	if (dwgfx.onscreen(320 + game.creditposition)) {
		dwgfx.drawcrewman(70, 320 + game.creditposition, 0, true, help);
		dwgfx.print(100, 330 + game.creditposition, "Captain Viridian", tr, tg, tb);
	}
	if (dwgfx.onscreen(350 + game.creditposition)) {
		dwgfx.drawcrewman(70, 350 + game.creditposition, 1, true, help);
		dwgfx.print(100, 360 + game.creditposition, "Doctor Violet", tr, tg, tb);
	}
	if (dwgfx.onscreen(380 + game.creditposition)) {
		dwgfx.drawcrewman(70, 380 + game.creditposition, 2, true, help);
		dwgfx.print(100, 390 + game.creditposition, "Professor Vitellary", tr, tg, tb);
	}
	if (dwgfx.onscreen(410 + game.creditposition)) {
		dwgfx.drawcrewman(70, 410 + game.creditposition, 3, true, help);
		dwgfx.print(100, 420 + game.creditposition, "Officer Vermilion", tr, tg, tb);
	}
	if (dwgfx.onscreen(440 + game.creditposition)) {
		dwgfx.drawcrewman(70, 440 + game.creditposition, 4, true, help);
		dwgfx.print(100, 450 + game.creditposition, "Chief Verdigris", tr, tg, tb);
	}
	if (dwgfx.onscreen(470 + game.creditposition)) {
		dwgfx.drawcrewman(70, 470 + game.creditposition, 5, true, help);
		dwgfx.print(100, 480 + game.creditposition, "Doctor Victoria", tr, tg, tb);
	}
	
	if (dwgfx.onscreen(540 + game.creditposition)) dwgfx.bigprint( -1, 540 + game.creditposition, "Credits", tr, tg, tb, true, 3);
		
	if (dwgfx.onscreen(600 + game.creditposition)) {
		dwgfx.print(40, 600 + game.creditposition, "Created by", tr, tg, tb);
		dwgfx.bigprint(60, 610 + game.creditposition, "Terry Cavanagh", tr, tg, tb);
	}
	
	if (dwgfx.onscreen(640 + game.creditposition)) {
		dwgfx.print(40, 640 + game.creditposition, "With Music by", tr, tg, tb);
		dwgfx.bigprint(60, 650 + game.creditposition, "Magnus Palsson", tr, tg, tb);
	}
	
	if (dwgfx.onscreen(680 + game.creditposition)) {
		dwgfx.print(40, 680 + game.creditposition, "Rooms Named by", tr, tg, tb);
		dwgfx.bigprint(60, 690 + game.creditposition, "Bennett Foddy", tr, tg, tb);
	}
	
		
	if (dwgfx.onscreen(720 + game.creditposition)) {
		dwgfx.print(40, 720 + game.creditposition, "Beta Testing by", tr, tg, tb);
		dwgfx.bigprint(60, 730 + game.creditposition, "Sam Kaplan", tr, tg, tb);
		dwgfx.bigprint(60, 750 + game.creditposition, "Pauli Kohberger", tr, tg, tb);
	}
	
	if (dwgfx.onscreen(780 + game.creditposition)) {
		dwgfx.print(40, 780 + game.creditposition, "Ending Picture by", tr, tg, tb);
		dwgfx.bigprint(60, 790 + game.creditposition, "Pauli Kohberger", tr, tg, tb);
	}
	
	
	if (dwgfx.onscreen(870 + game.creditposition)) dwgfx.bigprint( -1, 870 + game.creditposition, "Patrons", tr, tg, tb, true, 3);
	
	if (dwgfx.onscreen(910 + game.creditposition)) dwgfx.print(-1, 910 + game.creditposition,"Anders Ekermo", tr, tg, tb, true);
if (dwgfx.onscreen(920 + game.creditposition)) dwgfx.print(-1, 920 + game.creditposition,"Andreas K|mper", tr, tg, tb, true);
if (dwgfx.onscreen(930 + game.creditposition)) dwgfx.print(-1, 930 + game.creditposition,"Anthony Burch", tr, tg, tb, true);
if (dwgfx.onscreen(940 + game.creditposition)) dwgfx.print(-1, 940 + game.creditposition,"Bennett Foddy", tr, tg, tb, true);
if (dwgfx.onscreen(950 + game.creditposition)) dwgfx.print(-1, 950 + game.creditposition,"Brendan O'Sullivan", tr, tg, tb, true);
if (dwgfx.onscreen(960 + game.creditposition)) dwgfx.print(-1, 960 + game.creditposition,"Christopher Armstrong", tr, tg, tb, true);
if (dwgfx.onscreen(970 + game.creditposition)) dwgfx.print(-1, 970 + game.creditposition,"Daniel Benmergui", tr, tg, tb, true);
if (dwgfx.onscreen(980 + game.creditposition)) dwgfx.print(-1, 980 + game.creditposition,"David Pittman", tr, tg, tb, true);
if (dwgfx.onscreen(990 + game.creditposition)) dwgfx.print(-1, 990 + game.creditposition,"Ian Bogost", tr, tg, tb, true);
if (dwgfx.onscreen(1000 + game.creditposition)) dwgfx.print(-1, 1000 + game.creditposition,"Jaz McDougall", tr, tg, tb, true);
if (dwgfx.onscreen(1010 + game.creditposition)) dwgfx.print(-1, 1010 + game.creditposition,"John Faulkenbury", tr, tg, tb, true);
if (dwgfx.onscreen(1020 + game.creditposition)) dwgfx.print(-1, 1020 + game.creditposition,"Jonathan Whiting", tr, tg, tb, true);
if (dwgfx.onscreen(1030 + game.creditposition)) dwgfx.print(-1, 1030 + game.creditposition,"Kyle Pulver", tr, tg, tb, true);
if (dwgfx.onscreen(1040 + game.creditposition)) dwgfx.print(-1, 1040 + game.creditposition,"Markus Persson", tr, tg, tb, true);
if (dwgfx.onscreen(1050 + game.creditposition)) dwgfx.print(-1, 1050 + game.creditposition,"Nathan Ostgard", tr, tg, tb, true);
if (dwgfx.onscreen(1060 + game.creditposition)) dwgfx.print(-1, 1060 + game.creditposition,"Nick Easler", tr, tg, tb, true);
if (dwgfx.onscreen(1070 + game.creditposition)) dwgfx.print(-1, 1070 + game.creditposition,"Stephen Lavelle", tr, tg, tb, true);

if (dwgfx.onscreen(1090 + game.creditposition)) dwgfx.print( -1, 1090 + game.creditposition, "and", tr, tg, tb, true);
			
if (dwgfx.onscreen(1110 + game.creditposition)) dwgfx.print(-1, 1110 + game.creditposition,"Adam Wendt", tr, tg, tb, true);
if (dwgfx.onscreen(1110 + game.creditposition)) dwgfx.print(-1, 1120 + game.creditposition,"Andreas J{rgensen", tr, tg, tb, true);
if (dwgfx.onscreen(1120 + game.creditposition)) dwgfx.print(-1, 1130 + game.creditposition,"}ngel Louzao Penalva", tr, tg, tb, true);
if (dwgfx.onscreen(1130 + game.creditposition)) dwgfx.print(-1, 1140 + game.creditposition,"Ashley Burton", tr, tg, tb, true);
if (dwgfx.onscreen(1140 + game.creditposition)) dwgfx.print(-1, 1150 + game.creditposition,"Aubrey Hesselgren", tr, tg, tb, true);
if (dwgfx.onscreen(1150 + game.creditposition)) dwgfx.print(-1, 1160 + game.creditposition,"Bradley Rose", tr, tg, tb, true);
if (dwgfx.onscreen(1160 + game.creditposition)) dwgfx.print(-1, 1170 + game.creditposition,"Brendan Urquhart", tr, tg, tb, true);
if (dwgfx.onscreen(1170 + game.creditposition)) dwgfx.print(-1, 1180 + game.creditposition,"Chris Ayotte", tr, tg, tb, true);
if (dwgfx.onscreen(1180 + game.creditposition)) dwgfx.print(-1, 1190 + game.creditposition,"Christopher Zamanillo", tr, tg, tb, true);
if (dwgfx.onscreen(1190 + game.creditposition)) dwgfx.print(-1, 1200 + game.creditposition,"Daniel Schuller", tr, tg, tb, true);
if (dwgfx.onscreen(1200 + game.creditposition)) dwgfx.print(-1, 1210 + game.creditposition,"Hybrid Mind Studios", tr, tg, tb, true);
if (dwgfx.onscreen(1210 + game.creditposition)) dwgfx.print(-1, 1220 + game.creditposition,"Emilie McGinley", tr, tg, tb, true);
if (dwgfx.onscreen(1220 + game.creditposition)) dwgfx.print(-1, 1230 + game.creditposition,"Francisco Solares", tr, tg, tb, true);
if (dwgfx.onscreen(1230 + game.creditposition)) dwgfx.print(-1, 1240 + game.creditposition,"Hal Helms", tr, tg, tb, true);
if (dwgfx.onscreen(1240 + game.creditposition)) dwgfx.print(-1, 1250 + game.creditposition,"Hayden Scott-Baron", tr, tg, tb, true);
if (dwgfx.onscreen(1250 + game.creditposition)) dwgfx.print(-1, 1260 + game.creditposition,"Hermit Games", tr, tg, tb, true);
if (dwgfx.onscreen(1260 + game.creditposition)) dwgfx.print(-1, 1270 + game.creditposition,"Ido Yehieli", tr, tg, tb, true);
if (dwgfx.onscreen(1270 + game.creditposition)) dwgfx.print(-1, 1280 + game.creditposition,"Jade Vault Games", tr, tg, tb, true);
if (dwgfx.onscreen(1280 + game.creditposition)) dwgfx.print(-1, 1290 + game.creditposition,"James Andrews", tr, tg, tb, true);
if (dwgfx.onscreen(1290 + game.creditposition)) dwgfx.print(-1, 1300 + game.creditposition,"James Riley", tr, tg, tb, true);
if (dwgfx.onscreen(1300 + game.creditposition)) dwgfx.print(-1, 1310 + game.creditposition,"James Hsieh", tr, tg, tb, true);
if (dwgfx.onscreen(1310 + game.creditposition)) dwgfx.print(-1, 1320 + game.creditposition,"Jasper Byrne", tr, tg, tb, true);
if (dwgfx.onscreen(1320 + game.creditposition)) dwgfx.print(-1, 1330 + game.creditposition,"Jedediah Baker", tr, tg, tb, true);
if (dwgfx.onscreen(1330 + game.creditposition)) dwgfx.print(-1, 1340 + game.creditposition,"Jens Bergensten", tr, tg, tb, true);
if (dwgfx.onscreen(1340 + game.creditposition)) dwgfx.print(-1, 1350 + game.creditposition,"Jeremy J. Penner", tr, tg, tb, true);
if (dwgfx.onscreen(1350 + game.creditposition)) dwgfx.print(-1, 1360 + game.creditposition,"Jeremy Peterson", tr, tg, tb, true);
if (dwgfx.onscreen(1360 + game.creditposition)) dwgfx.print(-1, 1370 + game.creditposition,"Jim McGinley", tr, tg, tb, true);
if (dwgfx.onscreen(1370 + game.creditposition)) dwgfx.print(-1, 1380 + game.creditposition,"Jonathan Cartwright", tr, tg, tb, true);
if (dwgfx.onscreen(1380 + game.creditposition)) dwgfx.print(-1, 1390 + game.creditposition,"John Nesky", tr, tg, tb, true);
if (dwgfx.onscreen(1390 + game.creditposition)) dwgfx.print(-1, 1400 + game.creditposition,"Jos Yule", tr, tg, tb, true);
if (dwgfx.onscreen(1400 + game.creditposition)) dwgfx.print(-1, 1410 + game.creditposition,"Jose Flores", tr, tg, tb, true);
if (dwgfx.onscreen(1410 + game.creditposition)) dwgfx.print(-1, 1420 + game.creditposition,"Josh Bizeau", tr, tg, tb, true);
if (dwgfx.onscreen(1420 + game.creditposition)) dwgfx.print(-1, 1430 + game.creditposition,"Joshua Buergel", tr, tg, tb, true);
if (dwgfx.onscreen(1430 + game.creditposition)) dwgfx.print(-1, 1440 + game.creditposition,"Joshua Hochner", tr, tg, tb, true);
if (dwgfx.onscreen(1440 + game.creditposition)) dwgfx.print(-1, 1450 + game.creditposition,"Kurt Ostfeld", tr, tg, tb, true);
if (dwgfx.onscreen(1450 + game.creditposition)) dwgfx.print(-1, 1460 + game.creditposition, "Magnus Palsson", tr, tg, tb, true);
if (dwgfx.onscreen(1460 + game.creditposition)) dwgfx.print(-1, 1470 + game.creditposition,"Mark Neschadimenko", tr, tg, tb, true);
if (dwgfx.onscreen(1470 + game.creditposition)) dwgfx.print(-1, 1480 + game.creditposition,"Matt Antonellis", tr, tg, tb, true);
if (dwgfx.onscreen(1480 + game.creditposition)) dwgfx.print(-1, 1490 + game.creditposition,"Matthew Reppert", tr, tg, tb, true);
if (dwgfx.onscreen(1490 + game.creditposition)) dwgfx.print(-1, 1500 + game.creditposition,"Michael Falkensteiner", tr, tg, tb, true);
if (dwgfx.onscreen(1500 + game.creditposition)) dwgfx.print(-1, 1510 + game.creditposition,"Michael Vendittelli", tr, tg, tb, true);
if (dwgfx.onscreen(1510 + game.creditposition)) dwgfx.print(-1, 1520 + game.creditposition,"Mike Kasprzak", tr, tg, tb, true);
if (dwgfx.onscreen(1520 + game.creditposition)) dwgfx.print(-1, 1530 + game.creditposition,"Mitchel Stein", tr, tg, tb, true);
if (dwgfx.onscreen(1530 + game.creditposition)) dwgfx.print(-1, 1540 + game.creditposition,"Sean Murray", tr, tg, tb, true);
if (dwgfx.onscreen(1540 + game.creditposition)) dwgfx.print(-1, 1550 + game.creditposition,"Simon Michael", tr, tg, tb, true);
if (dwgfx.onscreen(1550 + game.creditposition)) dwgfx.print(-1, 1560 + game.creditposition,"Simon Schmid", tr, tg, tb, true);
if (dwgfx.onscreen(1560 + game.creditposition)) dwgfx.print(-1, 1570 + game.creditposition,"Stephen Maxwell", tr, tg, tb, true);
if (dwgfx.onscreen(1570 + game.creditposition)) dwgfx.print(-1, 1580 + game.creditposition,"Swing Swing Submarine", tr, tg, tb, true);
if (dwgfx.onscreen(1580 + game.creditposition)) dwgfx.print(-1, 1590 + game.creditposition,"Tam Toucan", tr, tg, tb, true);
if (dwgfx.onscreen(1590 + game.creditposition)) dwgfx.print(-1, 1600 + game.creditposition,"Terry Dooher", tr, tg, tb, true);
if (dwgfx.onscreen(1600 + game.creditposition)) dwgfx.print(-1, 1610 + game.creditposition,"Tim W.", tr, tg, tb, true);
if (dwgfx.onscreen(1610 + game.creditposition)) dwgfx.print( -1, 1620 + game.creditposition, "Timothy Bragan", tr, tg, tb, true);

if (dwgfx.onscreen(1760 + game.creditposition)) dwgfx.bigprint( -1, 1760 + game.creditposition, "Thanks for playing!", tr, tg, tb, true, 2);

	if (game.test) {
		dwgfx.print(5, 5, game.teststring, 196, 196, 255, false);
	}

	dwgfx.drawfade();
	
	if (game.flashlight > 0 && !game.noflashingmode) {
		game.flashlight--;
		dwgfx.flashlight();
	}
	
	if (game.screenshake > 0 && !game.noflashingmode) {
		game.screenshake--;
		dwgfx.screenshake();
	}else{
	  dwgfx.render();
	}
	//dwgfx.backbuffer.unlock();
}

public function gamecompleterender2(key:KeyPoll, dwgfx:dwgraphicsclass, game:gameclass, obj:entityclass, help:helpclass):void {
  dwgfx.drawbutton(game, help);
	
	//dwgfx.backbuffer.lock();
	dwgfx.cls(0x000000);
	
	dwgfx.drawimage(10, 0, 0);
	
	for (j = 0; j < 30; j++) {
	  for (i = 0; i < 40; i++) {
			if (j == game.creditposy) {
			  if (i > game.creditposx) {
				  dwgfx.drawfillrect(i * 8, j * 8, 8, 8, 0, 0, 0);	
				}
			}
			
			if (j > game.creditposy) {
			  dwgfx.drawfillrect(i * 8, j * 8, 8, 8, 0, 0, 0);	
			}
		}
	}
	
	if (game.test) {
		dwgfx.print(5, 5, game.teststring, 196, 196, 255, false);
	}

	dwgfx.drawfade();
	
	if (game.flashlight > 0 && !game.noflashingmode) {
		game.flashlight--;
		dwgfx.flashlight();
	}
	
	if (game.screenshake > 0 && !game.noflashingmode) {
		game.screenshake--;
		dwgfx.screenshake();
	}else{
	  dwgfx.render();
	}
	//dwgfx.backbuffer.unlock();
}

public function gamerender(key:KeyPoll, dwgfx:dwgraphicsclass, game:gameclass, map:mapclass,
													 obj:entityclass, help:helpclass):void {
  
  dwgfx.drawbutton(game, help);
	
	//dwgfx.backbuffer.lock();
	
	if(!game.blackout){
		if(!game.colourblindmode) dwgfx.drawbackground(map.background, map);
		if (map.final_colormode) {
			dwgfx.drawfinalmap(map);
		}else{
			dwgfx.drawmap(map);
		}
		
		if(!game.completestop){
			for (i = 0; i < obj.nentity; i++) {
				//Is this entity on the ground? (needed for jumping)
				if (obj.entities[i].active) {
					if (obj.entities[i].rule == 0 || obj.entities[i].rule >= 6 || obj.entities[i].type == 55) {
						if (obj.entitycollidefloor(map, i)) {obj.entities[i].onground = 2;
						}else {	obj.entities[i].onground--;}
						
						if (obj.entitycollideroof(map, i)) {obj.entities[i].onroof = 2;
						}else { obj.entities[i].onroof--; }				
					}
				}
				
				//Animate the entities
				obj.animateentities(i, game, help);
			}
		}
		
		dwgfx.drawentities(map, obj, help);
	}else {
		dwgfx.cls(0x000000);	
	}
	
	/*for(i=0; i<obj.nblocks; i++){
    if (obj.blocks[i].active) {
			dwgfx.backbuffer.fillRect(obj.blocks[i].rect, 0xDDDDDD);
    }
  }*/
	//dwgfx.drawminimap(game, map);
	
	if (map.extrarow == 0 || (map.custommode && map.roomname != "")) {
		dwgfx.drawfillrect(dwgfx.footerrect.x, dwgfx.footerrect.y, dwgfx.footerrect.width, dwgfx.footerrect.height, 0);
		if (map.finallevel) {
			map.glitchname = map.getglitchname(game.roomx, game.roomy);
	    dwgfx.print(5, 231, map.glitchname, 196, 196, 255 - help.glow, true);
		}else{
	    dwgfx.print(5, 231, map.roomname, 196, 196, 255 - help.glow, true);
		}
	}
	
	if (map.roomtexton) {
		//Draw room text!
		for (i = 0; i < map.roomtextnumlines; i++) {
			dwgfx.print(map.roomtextx[i]*8, (map.roomtexty[i]*8), map.roomtext[i], 196, 196, 255 - help.glow);
		}
	}
	
	if(map.custommode && !map.custommodeforreal){
		//Return to level editor
		dwgfx.bprint(5, 5, "[Press ENTER to return to editor]", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);
	}
	
	dwgfx.cutscenebars();
	dwgfx.drawfade();
	
	dwgfx.drawgui(help);
	if (dwgfx.flipmode) {
		if (game.advancetext) dwgfx.print(5, 228, "- Tap screen to advance text -", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);	
	}else{
	  if (game.advancetext) dwgfx.print(5, 5, "- Tap screen to advance text -", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);	
	}
	
	if(!game.mobilemenu){
		if (game.readytotele > 100 && !game.advancetext && game.hascontrol && !script.running && !game.intimetrial) {
			i = obj.getplayer();
			if(dwgfx.flipmode){
				dwgfx.print(5, 20, "- Press ENTER to Teleport -", game.readytotele - 20 - (help.glow / 2), game.readytotele - 20 - (help.glow / 2), game.readytotele, true);	
			}else {
				dwgfx.print(5, 210, "- Press ENTER to Teleport -", game.readytotele - 20 - (help.glow / 2), game.readytotele - 20 - (help.glow / 2), game.readytotele, true);	
			}
		}
	}
	
	if (game.swnmode) {
		if (game.swngame == 0) {
			tempstring = help.timestring(game.swntimer);
			dwgfx.bigprint( -1, 20, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);	
		}else if (game.swngame == 1) {
			if (game.swnmessage == 0) {
				tempstring = help.timestring(game.swntimer);
				dwgfx.print( 10, 10, "Current Time", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);	
				dwgfx.bigprint( 25, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false, 2);	
				tempstring = help.timestring(game.swnrecord);
				dwgfx.print( 240, 10, "Best Time", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);	
				dwgfx.bigrprint( 300, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false, 2);	
				
				switch(game.swnbestrank) {
					case 0: dwgfx.print( -1, 204, "Next Trophy at 5 seconds", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true); break;
					case 1: dwgfx.print( -1, 204, "Next Trophy at 10 seconds", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true); break;
					case 2: dwgfx.print( -1, 204, "Next Trophy at 15 seconds", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true); break;
					case 3: dwgfx.print( -1, 204, "Next Trophy at 20 seconds", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true); break;
					case 4: dwgfx.print( -1, 204, "Next Trophy at 30 seconds", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true); break;
					case 5: dwgfx.print( -1, 204, "Next Trophy at 1 minute", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true); break;
					case 6: dwgfx.print( -1, 204, "All Trophies collected!", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true); break;
				}
			}else if (game.swnmessage == 1) {
				tempstring = help.timestring(game.swntimer);
				dwgfx.print( 10, 10, "Current Time", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);	
				dwgfx.bigprint( 25, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false, 2);	
				tempstring = help.timestring(game.swnrecord);
				if (int(game.deathseq / 5) % 2 == 1) {
					dwgfx.print( 240, 10, "Best Time", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);	
					dwgfx.bigrprint( 300, 24, tempstring, 128 - (help.glow), 220 - (help.glow), 128 - (help.glow / 2), false, 2);	
				
					dwgfx.bigprint( -1, 200, "New Record!", 128 - (help.glow), 220 - (help.glow), 128 - (help.glow / 2), true, 2);	
				}
			}else if (game.swnmessage >= 2) {
				game.swnmessage--;
				if (game.swnmessage == 2) game.swnmessage = 0;
				tempstring = help.timestring(game.swntimer);
				dwgfx.print( 10, 10, "Current Time", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);	
				dwgfx.bigprint( 25, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false, 2);	
				tempstring = help.timestring(game.swnrecord);
				dwgfx.print( 240, 10, "Best Time", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false);	
				dwgfx.bigrprint( 300, 24, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), false, 2);	
				
				if (int(game.swnmessage / 5) % 2 == 1) {
					dwgfx.bigprint( -1, 200, "New Trophy!", 220 - (help.glow), 128 - (help.glow), 128 - (help.glow / 2), true, 2);	
				}
			}
			
			if(!game.mobilemenu){
				dwgfx.print( 20, 228, "[Press ENTER to stop]", 160 - (help.glow / 2), 160 - (help.glow / 2), 160 - (help.glow / 2), true);	
			}
		}else if(game.swngame==2){
			if (int(game.swndelay / 15) % 2 == 1 || game.swndelay >= 120) {
				if (dwgfx.flipmode) {
					dwgfx.bigprint( -1, 30, "Survive for", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);	
			    dwgfx.bigprint( -1, 10, "60 seconds!", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);	
				}else {
					dwgfx.bigprint( -1, 10, "Survive for", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);	
			    dwgfx.bigprint( -1, 30, "60 seconds!", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);	
				}
			}
		}else if(game.swngame==7){
			if (game.swndelay >= 60) {
				dwgfx.bigprint( -1, 20, "SUPER GRAVITRON", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);	
				
				tempstring = help.timestring(game.swnrecord);
				dwgfx.print( 240, 190, "Best Time", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);	
				dwgfx.bigrprint( 300, 205, tempstring, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);	
			}else	if (int(game.swndelay / 10) % 2 == 1) {
				dwgfx.bigprint( -1, 20, "SUPER GRAVITRON", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 2);	
				dwgfx.bigprint( -1, 200, "GO!", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 3);		
			}
		}	
	}
	
	if (game.intimetrial && dwgfx.fademode==0) {
		//Draw countdown!
		if (game.timetrialcountdown > 0) {
			if (game.timetrialcountdown < 30) {
				game.resetgameclock();
				if (int(game.timetrialcountdown / 4) % 2 == 0) dwgfx.bigprint( -1, 100, "Go!", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 4);
			}else if (game.timetrialcountdown < 60) {
				dwgfx.bigprint( -1, 100, "1", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 4);
			}else if (game.timetrialcountdown < 90) {
				dwgfx.bigprint( -1, 100, "2", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 4);
			}else if (game.timetrialcountdown < 120) {
				dwgfx.bigprint( -1, 100, "3", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 4);
			}
		}else {
			//Draw OSD stuff	
			dwgfx.bprint(6, 18, "TIME :",  255,255,255);
			dwgfx.bprint(6, 30, "DEATH:",  255, 255, 255);
			dwgfx.bprint(6, 42, "SHINY:",  255,255,255);
				
			if(game.timetrialparlost){
			  dwgfx.bprint(56, 18, game.timestring(help),  196, 80, 80);
			}else {
			  dwgfx.bprint(56, 18, game.timestring(help),  196, 196, 196);
			}
			if(game.deathcounts>0){
			  dwgfx.bprint(56, 30, String(game.deathcounts),  196, 80, 80);
			}else {
			  dwgfx.bprint(56, 30, String(game.deathcounts),  196, 196, 196);
			}
			if(game.trinkets<game.timetrialshinytarget){
			  dwgfx.bprint(56, 42, String(game.trinkets) + " of " +String(game.timetrialshinytarget),  196, 80, 80);
			}else {
			  dwgfx.bprint(56, 42, String(game.trinkets) + " of " +String(game.timetrialshinytarget),  196, 196, 196);
			}
			
			if(game.timetrialparlost){
				dwgfx.bprint(195, 214, "PAR TIME:",  80, 80, 80);
				dwgfx.bprint(275, 214, game.partimestring(help),  80, 80, 80);
			}else {
				dwgfx.bprint(195, 214, "PAR TIME:",  255, 255, 255);
				dwgfx.bprint(275, 214, game.partimestring(help),  196, 196, 196);
			}
		}
	}
	
	if (game.activeactivity > -1) {
		//dwgfx.backbuffer.fillRect(new Rectangle(0, 0, 320, 18), 0x000000);
		game.activity_lastprompt = obj.blocks[game.activeactivity].prompt;
		game.activity_r = obj.blocks[game.activeactivity].r;
		game.activity_g = obj.blocks[game.activeactivity].g;
		game.activity_b = obj.blocks[game.activeactivity].b;
		if(game.act_fade<5) game.act_fade=5;
		if(game.act_fade<10){
		  game.act_fade++;
		}
		dwgfx.drawtextbox(16, 4, 36, 3, game.activity_r*(game.act_fade/10), game.activity_g*(game.act_fade/10), game.activity_b*(game.act_fade/10));
		dwgfx.print(5, 12, game.activity_lastprompt, game.activity_r*(game.act_fade/10), game.activity_g*(game.act_fade/10), game.activity_b*(game.act_fade/10), true);
	}else {
		if(game.act_fade>5){
			dwgfx.drawtextbox(16, 4, 36, 3, game.activity_r*(game.act_fade/10), game.activity_g*(game.act_fade/10), game.activity_b*(game.act_fade/10));
		dwgfx.print(5, 12, game.activity_lastprompt, game.activity_r*(game.act_fade/10), game.activity_g*(game.act_fade/10), game.activity_b*(game.act_fade/10), true);
			game.act_fade--;
		}
	}
	
	if (obj.trophytext > 0) {
		dwgfx.drawtrophytext(obj, help);
		obj.trophytext--;
	}
	
	/*
	 * For creating graphical buttons!
	dwgfx.drawmobilebutton(game, 20, 20, 8*7, dwgfx.b_size, "BACK", 65, 185, 207);
	dwgfx.drawmobilebutton(game, 20, 80, 8*7, dwgfx.b_size, "BACK", 65*1.2, 185*1.2, 207*1.2);
	 *
	 * /
	
	//dwgfx.rprint(5, 231, String(game.coins), 255 - help.glow/2, 255 - help.glow/2, 196, true);
	//dwgfx.drawhuetile(311, 230, 48, 1);
	
	//Level complete image
	//if (game.state >= 3007) {
	//	dwgfx.drawimage(0, 0, 12, true);
	//}	
	
	//state changes
	
	/*
	game.test = true;
	if (game.teststring != String(game.state)) trace(game.state);
	game.teststring = String(game.state);
	*/
	
	//Detail entity info for debuging
	/*
	for (i = 0; i < obj.nentity; i++) {
		game.tempstring = String(obj.entities[i].type) +", (" + String(obj.entities[i].xp) + "," + String(obj.entities[i].yp) + ")";
		game.tempstring += " state:" +obj.entities[i].state + ", delay:" + obj.entities[i].statedelay;
		dwgfx.print(5, 5 + i * 8, game.tempstring, 255, 255, 255);
	}
	*/
	
	/*
	game.test = true;
	game.teststring = String(int(obj.entities[obj.getplayer()].xp)) + "," + String(int(obj.entities[obj.getplayer()].yp));
	game.teststring += "   [" + String(game.roomx) + "," + String(game.roomy) + "]";
	*/
	
	//game.test = true;
	//game.teststring = "Current room deaths: " + String(game.currentroomdeaths);
	
	if (game.test) {
		dwgfx.print(5, 5, game.teststring, 196, 196, 255, false);
	}

	if (game.flashlight > 0 && !game.noflashingmode) {
		game.flashlight--;
		dwgfx.flashlight();
	}
	
	if (game.screenshake > 0 && !game.noflashingmode) {
		game.screenshake--;
		dwgfx.screenshake();
	}else{
	  dwgfx.render();
	}
	
	//dwgfx.backbuffer.unlock();
}

public function maprender(key:KeyPoll, dwgfx:dwgraphicsclass, game:gameclass, map:mapclass,
													 obj:entityclass, help:helpclass):void {

  dwgfx.drawbutton(game, help);
	
	//dwgfx.backbuffer.lock();
	
	
	//dwgfx.drawgui(help);
	
	//draw screen alliteration
	//Roomname:
	dwgfx.drawfillrect(0, 0, 320, 12, 0, 0, 0);
	temp = map.area(game.roomx, game.roomy);
	if (temp < 2 && !map.custommode && dwgfx.fademode == 0) {
		if (game.roomx >= 102 && game.roomx <= 104 && game.roomy >= 110 && game.roomy <= 111) {
			dwgfx.print(5, 2, "The Ship", 196, 196, 255 - help.glow, true);
		}else{
	    dwgfx.print(5, 2, "Dimension VVVVVV", 196, 196, 255 - help.glow, true);
		}
	}else {
		if (map.finallevel) {
			map.glitchname = map.getglitchname(game.roomx, game.roomy);
	    dwgfx.print(5, 2, map.glitchname, 196, 196, 255 - help.glow, true);
		}else{
	    dwgfx.print(5, 2, map.roomname, 196, 196, 255 - help.glow, true);
		}	
	}
	
	//Background color
	dwgfx.drawfillrect(0, 12, 320, 240, 10, 24, 26);
	
	dwgfx.crewframedelay--;
	if (dwgfx.crewframedelay <= 0) {
		dwgfx.crewframedelay = 8;
		dwgfx.crewframe = (dwgfx.crewframe + 1) % 2;
	}
		
	//Menubar:
	//dwgfx.drawtextbox( -10, 212, 42, 3, 65, 185, 207);
	dwgfx.drawfillrect(0, 212, 320, 24, 11, 31, 35);
	dwgfx.drawfillrect(0, 212, 320, 2, 65, 185, 207);
	dwgfx.drawfillrect(0, 215, 320, 1, 65, 185, 207);
	dwgfx.drawfillrect(0, 230, 320, 1, 65, 185, 207);
	dwgfx.drawfillrect(0, 232, 320, 2, 65, 185, 207);
	
	switch(game.menupage) {
		case 0:
			if (game.mobilemenu) {
				dwgfx.drawmobilebutton(game, 30 - 8, 220, 56, dwgfx.b_size, "MAP", 65, 185, 207);
				if (game.insecretlab) { dwgfx.drawmobilebutton(game, 103, 220, 56, dwgfx.b_size, "GRAV", 32, 92, 104);
				}else if (obj.flags[67] == 1 && !map.custommode) { dwgfx.drawmobilebutton(game, 103, 220, 56, dwgfx.b_size, "SHIP", 32, 92, 104);
				}else{ dwgfx.drawmobilebutton(game, 103, 220, 56, dwgfx.b_size, "CREW", 32, 92, 104);}
				dwgfx.drawmobilebutton(game, 185-4, 220, 56, dwgfx.b_size, "STATS", 32, 92, 104);
				dwgfx.drawmobilebutton(game, 258, 220, 56, dwgfx.b_size, "QUIT", 32, 92, 104);
			}else {
				dwgfx.print(30 - 8, 220, "[MAP]", 196, 196, 255 - help.glow);
				if (game.insecretlab) { dwgfx.print(103, 220, "GRAV", 64, 64, 64);
				}else if (obj.flags[67] == 1 && !map.custommode) { dwgfx.print(103, 220, "SHIP", 64,64,64);
				}else{ dwgfx.print(103, 220, "CREW", 64,64,64);}
				dwgfx.print(185-4, 220, "STATS", 64,64,64);
				dwgfx.print(258, 220, "SAVE", 64, 64, 64);	
			}
			
			if (map.finalmode || (map.custommode && !map.customshowmm)) {
				//draw the map image
				dwgfx.drawpixeltextbox(35, 16, 250, 190, 32,24, 65, 185, 207,4,0);
				dwgfx.drawimage(1, 40, 21, false);
				
				for (j = 0; j < 20; j++) {
					for (i = 0; i < 20; i++) {
						dwgfx.drawimage(2, 40 + (i * 12), 21 + (j * 9), false);
					}
				}
				dwgfx.print(-1, 105, "NO SIGNAL", 245, 245, 245, true);
			}else if(map.custommode){
				//draw the map image
				dwgfx.drawcustompixeltextbox(35 + map.custommmxoff, 16 + map.custommmyoff, map.custommmxsize + 10, map.custommmysize + 10, (map.custommmxsize + 10) / 8, (map.custommmysize + 10) / 8, 65, 185, 207, 4, 0);
				dwgfx.drawpartimage(dwgfx.customminimap as Texture, 40 + map.custommmxoff, 21 + map.custommmyoff, map.custommmxsize, map.custommmysize);
				
				//Black out here
				if(map.customzoom==4){
					for (j = 0; j < map.customheight; j++){
						for (i = 0; i < map.customwidth; i++){
							if(map.explored[i+(j*20)]==0){
								//Draw the fog of war on the map
								dwgfx.drawimage(2, map.custommmxoff+40 + (i * 48), map.custommmyoff+21 + (j * 36), false);
								dwgfx.drawimage(2, map.custommmxoff+40 + 12 + (i * 48), map.custommmyoff+21 + (j * 36), false);
								dwgfx.drawimage(2, map.custommmxoff+40 + (i * 48), map.custommmyoff+21 + 9 + (j * 36), false);
								dwgfx.drawimage(2, map.custommmxoff + 40 + 12 + (i * 48), map.custommmyoff + 21 + 9 + (j * 36), false);
								
								dwgfx.drawimage(2, map.custommmxoff+40 + (i * 48) + 24, map.custommmyoff+21 + (j * 36), false);
								dwgfx.drawimage(2, map.custommmxoff+40 + 12 + (i * 48) + 24, map.custommmyoff+21 + (j * 36), false);
								dwgfx.drawimage(2, map.custommmxoff+40 + (i * 48) + 24, map.custommmyoff+ 21 + 9 + (j * 36), false);
								dwgfx.drawimage(2, map.custommmxoff + 40 + 12 + (i * 48) + 24, map.custommmyoff + 21 + 9 + (j * 36), false);
								
								dwgfx.drawimage(2, map.custommmxoff+40 + (i * 48), map.custommmyoff+21 + (j * 36)+18, false);
								dwgfx.drawimage(2, map.custommmxoff+40 + 12 + (i * 48), map.custommmyoff+21 + (j * 36)+18, false);
								dwgfx.drawimage(2, map.custommmxoff+40 + (i * 48), map.custommmyoff+21 + 9 + (j * 36)+18, false);
								dwgfx.drawimage(2, map.custommmxoff + 40 + 12 + (i * 48), map.custommmyoff + 21 + 9 + (j * 36) + 18, false);
								
								dwgfx.drawimage(2, map.custommmxoff+40 + (i * 48) + 24, map.custommmyoff+21 + (j * 36)+18, false);
								dwgfx.drawimage(2, map.custommmxoff+40 + 12 + (i * 48) + 24, map.custommmyoff+21 + (j * 36)+18, false);
								dwgfx.drawimage(2, map.custommmxoff+40 + (i * 48) + 24, map.custommmyoff+21 + 9 + (j * 36)+18, false);
								dwgfx.drawimage(2, map.custommmxoff+40 + 12 + (i * 48) + 24, map.custommmyoff+21 + 9+ (j * 36)+18, false);
							}
						}
					}
				}else if(map.customzoom==2){
					for (j = 0; j < map.customheight; j++){
						for (i = 0; i < map.customwidth; i++){
							if (map.explored[i + (j * 20)] == 0) {
								//Draw the fog of war on the map
								dwgfx.drawimage(2, map.custommmxoff+40 + (i * 24), map.custommmyoff+21 + (j * 18), false);
								dwgfx.drawimage(2, map.custommmxoff+40 + 12 + (i * 24), map.custommmyoff+21 + (j * 18), false);
								dwgfx.drawimage(2, map.custommmxoff+40 + (i * 24), map.custommmyoff+21 + 9 + (j * 18), false);
								dwgfx.drawimage(2, map.custommmxoff+40 + 12 + (i * 24), map.custommmyoff+21 + 9+ (j * 18), false);
							}
						}
					}
				}else{
					for (j = 0; j < map.customheight; j++){
						for (i = 0; i < map.customwidth; i++){
							if(map.explored[i+(j*20)]==0){
								//Draw the fog of war on the map
								dwgfx.drawimage(2, map.custommmxoff+40 + (i * 12), map.custommmyoff+21 + (j * 9), false);
							}
						}
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
				
				//normal size maps
				if(map.customzoom==4){
					if(map.cursorstate==1){
						if (int(map.cursordelay / 4) % 2 == 0){
							dwgfx.drawrect(40 + ((game.roomx - 100) * 48) +map.custommmxoff, 21 + ((game.roomy - 100) * 36)+map.custommmyoff , 48 , 36 , 255,255,255);
							dwgfx.drawrect(40 + ((game.roomx - 100) * 48) + 2+map.custommmxoff, 21 + ((game.roomy - 100) * 36) + 2+map.custommmyoff, 48 - 4, 36 - 4, 255,255,255);
						}
					}else if (map.cursorstate == 2){
						if (int(map.cursordelay / 15) % 2 == 0){
							dwgfx.drawrect(40 + ((game.roomx - 100) * 48) + 2+map.custommmxoff, 21 + ((game.roomy - 100) * 36) + 2+map.custommmyoff, 48 - 4, 36 - 4, 16, 245 - (help.glow), 245 - (help.glow));
						}
					}
				}else if(map.customzoom==2){
					if(map.cursorstate==1){
						if (int(map.cursordelay / 4) % 2 == 0){
							dwgfx.drawrect(40 + ((game.roomx - 100) * 24)+map.custommmxoff , 21 + ((game.roomy - 100) * 18)+map.custommmyoff , 24 , 18 , 255,255,255);
							dwgfx.drawrect(40 + ((game.roomx - 100) * 24) + 2+map.custommmxoff, 21 + ((game.roomy - 100) * 18) + 2+map.custommmyoff, 24 - 4, 18 - 4, 255,255,255);
						}
					}else if (map.cursorstate == 2){
						if (int(map.cursordelay / 15) % 2 == 0){
							dwgfx.drawrect(40 + ((game.roomx - 100) * 24) + 2+map.custommmxoff, 21 + ((game.roomy - 100) * 18) + 2+map.custommmyoff, 24 - 4, 18 - 4, 16, 245 - (help.glow), 245 - (help.glow));
						}
					}
				}else{
					if(map.cursorstate==1){
						if (int(map.cursordelay / 4) % 2 == 0){
							dwgfx.drawrect(40 + ((game.roomx - 100) * 12)+map.custommmxoff , 21 + ((game.roomy - 100) * 9)+map.custommmyoff , 12 , 9 , 255,255,255);
							dwgfx.drawrect(40 + ((game.roomx - 100) * 12) + 2+map.custommmxoff, 21 + ((game.roomy - 100) * 9) + 2+map.custommmyoff, 12 - 4, 9 - 4, 255,255,255);
						}
					}else if (map.cursorstate == 2){
						if (int(map.cursordelay / 15) % 2 == 0){
							dwgfx.drawrect(40 + ((game.roomx - 100) * 12) + 2+map.custommmxoff, 21 + ((game.roomy - 100) * 9) + 2+map.custommmyoff, 12 - 4, 9 - 4, 16, 245 - (help.glow), 245 - (help.glow));
						}
					}
				}				
      }else {
				//draw the map image
				dwgfx.drawpixeltextbox(35, 16, 250, 190, 32,24, 65, 185, 207,4,0);
				dwgfx.drawimage(1, 40, 21, false);
				
				//black out areas we can't see yet
				for (j = 0; j < 20; j++) {
					for (i = 0; i < 20; i++) {
						if(map.explored[i+(j*20)]==0){
							//dwgfx.drawfillrect(10 + (i * 12), 21 + (j * 9), 12, 9, 16, 16, 16);
							dwgfx.drawimage(2, 40 + (i * 12), 21 + (j * 9), false);
						}
					}
				}
				//draw the coordinates
				if (game.roomx == 109) {
					//tower!instead of room y, scale map.ypos
					/*if (map.ypos > (0.57 * (680 * 8))) {
						i = int(map.ypos - (0.57 * (680 * 8)));
						i = int((i / (0.43 * (680 * 8)))*9);
						dwgfx.drawrect(40 + ((game.roomx - 100) * 12) + 2, 21 + i + 2, 12 - 4, 9 - 4, 16, 245 - (help.glow * 2), 245 - (help.glow * 2));		
					}*/
					if (map.cursorstate == 0) {
						map.cursordelay++;
						if (map.cursordelay > 10) {	map.cursorstate = 1; map.cursordelay = 0; }
					}else if (map.cursorstate == 1) {
						map.cursordelay++;
						if (int(map.cursordelay / 4) % 2 == 0) {
							dwgfx.drawrect(40 + ((game.roomx - 100) * 12) , 21 , 12, 180, 255,255,255);		
							dwgfx.drawrect(40 + ((game.roomx - 100) * 12) + 2 , 21  + 2, 12 - 4, 180 - 4, 255,255,255);		
						}
						if (map.cursordelay > 30) map.cursorstate = 2;
					}else if (map.cursorstate == 2) {
						map.cursordelay++;
						if (int(map.cursordelay / 15) % 2 == 0) {
							dwgfx.drawrect(40 + ((game.roomx - 100) * 12) + 2 , 21  + 2, 12 - 4, 180 - 4,16, 245 - (help.glow), 245 - (help.glow));				
						}
					}
				}else {
					if (map.cursorstate == 0) {
						map.cursordelay++;
						if (map.cursordelay > 10) {	map.cursorstate = 1; map.cursordelay = 0; }
					}else if (map.cursorstate == 1) {
						map.cursordelay++;
						if (int(map.cursordelay / 4) % 2 == 0) {
							dwgfx.drawrect(40 + ((game.roomx - 100) * 12) , 21 + ((game.roomy - 100) * 9) , 12 , 9 , 255,255,255);		
							dwgfx.drawrect(40 + ((game.roomx - 100) * 12) + 2, 21 + ((game.roomy - 100) * 9) + 2, 12 - 4, 9 - 4, 255,255,255);		
						}
						if (map.cursordelay > 30) map.cursorstate = 2;
					}else if (map.cursorstate == 2) {
						map.cursordelay++;
						if (int(map.cursordelay / 15) % 2 == 0) {
					    dwgfx.drawrect(40 + ((game.roomx - 100) * 12) + 2, 21 + ((game.roomy - 100) * 9) + 2, 12 - 4, 9 - 4, 16, 245 - (help.glow), 245 - (help.glow));		
						}
					}
				}
				
				//draw legend details
				for (i = 0; i < map.numteleporters; i++) {
					if (map.showteleporters && map.explored[map.teleporters[i].x + (20 * map.teleporters[i].y)] > 0) {
						temp = 1126 + map.explored[map.teleporters[i].x + (20 * map.teleporters[i].y)];
						if (dwgfx.flipmode) temp += 3;
						dwgfx.drawtile(40 + 3 + (map.teleporters[i].x * 12), 22 + (map.teleporters[i].y * 9), temp);
					}else if(map.showtargets && map.explored[map.teleporters[i].x+(20*map.teleporters[i].y)]==0){
						temp = 1126 + map.explored[map.teleporters[i].x + (20 * map.teleporters[i].y)];
						if (dwgfx.flipmode) temp += 3;
						dwgfx.drawtile(40 + 3 + (map.teleporters[i].x * 12), 22 + (map.teleporters[i].y * 9), temp);
					}
					//dwgfx.drawtile(40+3 + (map.teleporters[i].x * 12), 22 + (map.teleporters[i].y * 9), 1086); //for shiny trinkets, do later
				}
				
				if (map.showtrinkets) {
					for (i = 0; i < map.numshinytrinkets; i++) {
						if (obj.collect[i] == 0) {
							temp = 1086;
							if (dwgfx.flipmode) temp += 3;
							dwgfx.drawtile(40 + 3 + (map.shinytrinkets[i].x * 12), 22 + (map.shinytrinkets[i].y * 9),	temp);
						}
					}
				}
			}
	  break;
		case 1:		  			
			if (game.insecretlab) {
				if (game.mobilemenu) {
					dwgfx.drawmobilebutton(game, 30 - 8, 220, 56, dwgfx.b_size, "MAP", 32, 92, 104);
					dwgfx.drawmobilebutton(game, 103, 220, 56, dwgfx.b_size, "GRAV", 65, 185, 207);
					dwgfx.drawmobilebutton(game, 185-4, 220, 56, dwgfx.b_size, "STATS", 32, 92, 104);
					dwgfx.drawmobilebutton(game, 258, 220, 56, dwgfx.b_size, "QUIT", 32, 92, 104);
				}else {
					dwgfx.print(30, 220, "MAP", 64,64,64);
					dwgfx.print(103-8, 220, "[GRAV]", 196, 196, 255 - help.glow);
					dwgfx.print(185-4, 220, "STATS", 64,64,64);
					dwgfx.print(258, 220, "SAVE", 64, 64, 64);	
				}
				
				if (dwgfx.flipmode) {
					dwgfx.print(0, 174, "SUPER GRAVITRON HIGHSCORE", 196, 196, 255 - help.glow, true);
					
					tempstring = help.timestring(game.swnrecord);
					dwgfx.print( 240, 124, "Best Time", 196, 196, 255 - help.glow, true);	
					dwgfx.bigrprint( 300, 94, tempstring, 196, 196, 255 - help.glow, true, 2);	
					
					switch(game.swnbestrank) {
						case 0: dwgfx.print( -1, 40, "Next Trophy at 5 seconds", 196, 196, 255 - help.glow, true); break;
						case 1: dwgfx.print( -1, 40, "Next Trophy at 10 seconds", 196, 196, 255 - help.glow, true); break;
						case 2: dwgfx.print( -1, 40, "Next Trophy at 15 seconds", 196, 196, 255 - help.glow, true); break;
						case 3: dwgfx.print( -1, 40, "Next Trophy at 20 seconds", 196, 196, 255 - help.glow, true); break;
						case 4: dwgfx.print( -1, 40, "Next Trophy at 30 seconds", 196, 196, 255 - help.glow, true); break;
						case 5: dwgfx.print( -1, 40, "Next Trophy at 1 minute", 196, 196, 255 - help.glow, true); break;
						case 6: dwgfx.print( -1, 40, "All Trophies collected!", 196, 196, 255 - help.glow, true); break;
					}
				}else{
					dwgfx.print(0, 40, "SUPER GRAVITRON HIGHSCORE", 196, 196, 255 - help.glow, true);
					
					tempstring = help.timestring(game.swnrecord);
					dwgfx.print( 240, 90, "Best Time", 196, 196, 255 - help.glow, true);	
					dwgfx.bigrprint( 300, 104, tempstring, 196, 196, 255 - help.glow, true, 2);	
					
					switch(game.swnbestrank) {
						case 0: dwgfx.print( -1, 174, "Next Trophy at 5 seconds", 196, 196, 255 - help.glow, true); break;
						case 1: dwgfx.print( -1, 174, "Next Trophy at 10 seconds", 196, 196, 255 - help.glow, true); break;
						case 2: dwgfx.print( -1, 174, "Next Trophy at 15 seconds", 196, 196, 255 - help.glow, true); break;
						case 3: dwgfx.print( -1, 174, "Next Trophy at 20 seconds", 196, 196, 255 - help.glow, true); break;
						case 4: dwgfx.print( -1, 174, "Next Trophy at 30 seconds", 196, 196, 255 - help.glow, true); break;
						case 5: dwgfx.print( -1, 174, "Next Trophy at 1 minute", 196, 196, 255 - help.glow, true); break;
						case 6: dwgfx.print( -1, 174, "All Trophies collected!", 196, 196, 255 - help.glow, true); break;
					}
				}
			}else if (obj.flags[67] == 1 && !map.custommode) {
				if (game.mobilemenu) {
					dwgfx.drawmobilebutton(game, 30 - 8, 220, 56, dwgfx.b_size, "MAP", 32, 92, 104);
					dwgfx.drawmobilebutton(game, 103, 220, 56, dwgfx.b_size, "SHIP", 65, 185, 207);
					dwgfx.drawmobilebutton(game, 185-4, 220, 56, dwgfx.b_size, "STATS", 32, 92, 104);
					dwgfx.drawmobilebutton(game, 258, 220, 56, dwgfx.b_size, "QUIT", 32, 92, 104);
				}else {
					dwgfx.print(30, 220, "MAP", 64,64,64);
					dwgfx.print(103-8, 220, "[SHIP]", 196, 196, 255 - help.glow);
					dwgfx.print(185-4, 220, "STATS", 64,64,64);
					dwgfx.print(258, 220, "SAVE", 64, 64, 64);	
				}
				
				if (game.mobilemenu) {
					dwgfx.print(0, 80, "Warp to the ship?", 255 - (help.glow * 2), 255 - (help.glow * 2), 255 - help.glow, true);
					
					if(help.slowsine%8>4){
						dwgfx.drawmobilebutton(game, 160 + 4 - 24, 110, 8 * 6, dwgfx.b_size, "WARP", 65, 185, 207);
					}else {
						dwgfx.drawmobilebutton(game, 160 + 4 - 24, 110, 8 * 6, dwgfx.b_size, "WARP", 65*1.2, 185*1.2, 207*1.2);
					}
					
				}else{
					dwgfx.print(0, 105, "Press ACTION to warp to the ship.", 196, 196, 255 - help.glow, true);
				}
			}else if(map.custommode){
				if (game.mobilemenu) {
					dwgfx.drawmobilebutton(game, 30 - 8, 220, 56, dwgfx.b_size, "MAP", 32, 92, 104);
					dwgfx.drawmobilebutton(game, 103, 220, 56, dwgfx.b_size, "CREW", 65, 185, 207);
					dwgfx.drawmobilebutton(game, 185-4, 220, 56, dwgfx.b_size, "STATS", 32, 92, 104);
					dwgfx.drawmobilebutton(game, 258, 220, 56, dwgfx.b_size, "QUIT", 32, 92, 104);
				}else {
					dwgfx.print(30, 220, "MAP", 64,64,64);
					dwgfx.print(103-8, 220, "[CREW]", 196, 196, 255 - help.glow);
					dwgfx.print(185-4, 220, "STATS", 64,64,64);
					dwgfx.print(258, 220, "SAVE", 64, 64, 64);
				}
				
				if (dwgfx.flipmode){
					dwgfx.bigprint( -1, 220-45, editor.ListOfMetaData[game.playcustomlevel].title, 196, 196, 255 - help.glow, true);
					dwgfx.print( -1, 220-70, "by " + editor.ListOfMetaData[game.playcustomlevel].creator, 196, 196, 255 - help.glow, true);
					dwgfx.print( -1, 220-80, editor.ListOfMetaData[game.playcustomlevel].website, 196, 196, 255 - help.glow, true);
					dwgfx.print( -1, 220-100, editor.ListOfMetaData[game.playcustomlevel].Desc1, 196, 196, 255 - help.glow, true);
					dwgfx.print( -1, 220-110, editor.ListOfMetaData[game.playcustomlevel].Desc2, 196, 196, 255 - help.glow, true);
					dwgfx.print( -1, 220 - 120, editor.ListOfMetaData[game.playcustomlevel].Desc3, 196, 196, 255 - help.glow, true);
					
					if(map.customcrewmates-game.crewmates==1){
						dwgfx.print(1,220-165, help.number(int(map.customcrewmates-game.crewmates))+ " crewmate remains", 196, 196, 255 - help.glow, true);
					}else if(map.customcrewmates-game.crewmates>0){
						dwgfx.print(1,220-165, help.number(int(map.customcrewmates-game.crewmates))+ " crewmates remain", 196, 196, 255 - help.glow, true);
					}
				}else{
					dwgfx.bigprint( -1, 45, editor.ListOfMetaData[game.playcustomlevel].title, 196, 196, 255 - help.glow, true);
					dwgfx.print( -1, 70, "by " + editor.ListOfMetaData[game.playcustomlevel].creator, 196, 196, 255 - help.glow, true);
					dwgfx.print( -1, 80, editor.ListOfMetaData[game.playcustomlevel].website, 196, 196, 255 - help.glow, true);
					dwgfx.print( -1, 100, editor.ListOfMetaData[game.playcustomlevel].Desc1, 196, 196, 255 - help.glow, true);
					dwgfx.print( -1, 110, editor.ListOfMetaData[game.playcustomlevel].Desc2, 196, 196, 255 - help.glow, true);
					dwgfx.print( -1, 120, editor.ListOfMetaData[game.playcustomlevel].Desc3, 196, 196, 255 - help.glow, true);
					
					if(map.customcrewmates-game.crewmates==1){
						dwgfx.print(1,165, help.number(int(map.customcrewmates-game.crewmates))+ " crewmate remains", 196, 196, 255 - help.glow, true);
					}else if(map.customcrewmates-game.crewmates>0){
						dwgfx.print(1,165, help.number(int(map.customcrewmates-game.crewmates))+ " crewmates remain", 196, 196, 255 - help.glow, true);
					}
				}
			}else{ 
				if (game.mobilemenu) {
					dwgfx.drawmobilebutton(game, 30 - 8, 220, 56, dwgfx.b_size, "MAP", 32, 92, 104);
					dwgfx.drawmobilebutton(game, 103, 220, 56, dwgfx.b_size, "CREW", 65, 185, 207);
					dwgfx.drawmobilebutton(game, 185-4, 220, 56, dwgfx.b_size, "STATS", 32, 92, 104);
					dwgfx.drawmobilebutton(game, 258, 220, 56, dwgfx.b_size, "QUIT", 32, 92, 104);
				}else {
					dwgfx.print(30, 220, "MAP", 64,64,64);
					dwgfx.print(103-8, 220, "[CREW]", 196, 196, 255 - help.glow);
					dwgfx.print(185-4, 220, "STATS", 64,64,64);
					dwgfx.print(258, 220, "SAVE", 64, 64, 64);						
				}
				
				if (dwgfx.flipmode) {
					for (i = 0; i < 3; i++) {
						dwgfx.drawcrewman(16, 32 + (i * 64), 2-i, game.crewstats[2-i], help);
						if (game.crewstats[(2-i)]) {
							dwgfx.printcrewname(44, 32 + (i * 64)+4+10, 2-i);
							dwgfx.printcrewnamestatus(44, 32 + (i * 64)+4, 2-i);
						}else {
							dwgfx.printcrewnamedark(44, 32 + (i * 64)+4+10, 2-i);
							dwgfx.print(44, 32 + (i * 64) + 4, "Missing...", 64,64,64);
						}
						
						dwgfx.drawcrewman(16+160, 32 + (i * 64), (2-i)+3, game.crewstats[(2-i)+3], help);
						if (game.crewstats[(2-i)+3]) {
							dwgfx.printcrewname(44+160, 32 + (i * 64)+4+10, (2-i)+3);
							dwgfx.printcrewnamestatus(44+160, 32 + (i * 64)+4, (2-i)+3);
						}else {
							dwgfx.printcrewnamedark(44+160, 32 + (i * 64)+4+10, (2-i)+3);
							dwgfx.print(44+160, 32 + (i * 64) + 4, "Missing...", 64,64,64);
						}
					}
				}else{
					for (i = 0; i < 3; i++) {
						dwgfx.drawcrewman(16, 32 + (i * 64), i, game.crewstats[i], help);
						if (game.crewstats[i]) {
							dwgfx.printcrewname(44, 32 + (i * 64)+4, i);
							dwgfx.printcrewnamestatus(44, 32 + (i * 64)+4+10, i);
						}else {
							dwgfx.printcrewnamedark(44, 32 + (i * 64)+4, i);
							dwgfx.print(44, 32 + (i * 64) + 4 + 10, "Missing...", 64,64,64);
						}
						
						dwgfx.drawcrewman(16+160, 32 + (i * 64), i+3, game.crewstats[i+3], help);
						if (game.crewstats[i+3]) {
							dwgfx.printcrewname(44+160, 32 + (i * 64)+4, i+3);
							dwgfx.printcrewnamestatus(44+160, 32 + (i * 64)+4+10, i+3);
						}else {
							dwgfx.printcrewnamedark(44+160, 32 + (i * 64)+4, i+3);
							dwgfx.print(44+160, 32 + (i * 64) + 4 + 10, "Missing...", 64,64,64);
						}
					}
				}
			}
	  break;
		case 2:
	    if (game.mobilemenu) {
				dwgfx.drawmobilebutton(game, 30 - 8, 220, 56, dwgfx.b_size, "MAP", 32, 92, 104);
				if (game.insecretlab) { dwgfx.drawmobilebutton(game, 103, 220, 56, dwgfx.b_size, "GRAV", 32, 92, 104);
				}else if (obj.flags[67] == 1 && !map.custommode) { dwgfx.drawmobilebutton(game, 103, 220, 56, dwgfx.b_size, "SHIP", 32, 92, 104);
				}else{ dwgfx.drawmobilebutton(game, 103, 220, 56, dwgfx.b_size, "CREW", 32, 92, 104);}
				dwgfx.drawmobilebutton(game, 185-4, 220, 56, dwgfx.b_size, "STATS", 65, 185, 207);
				dwgfx.drawmobilebutton(game, 258, 220, 56, dwgfx.b_size, "QUIT", 32, 92, 104);
			}else {
				dwgfx.print(30, 220, "MAP", 64,64,64);
				if (game.insecretlab) { dwgfx.print(103, 220, "GRAV", 64, 64, 64);
				}else if (obj.flags[67] == 1 && !map.custommode) { dwgfx.print(103, 220, "SHIP", 64,64,64);
				}else{ dwgfx.print(103, 220, "CREW", 64,64,64);}
				dwgfx.print(185-12, 220, "[STATS]", 196, 196, 255 - help.glow);
				dwgfx.print(258, 220, "SAVE", 64, 64, 64);	
			}
		
      if (map.custommode) {
			  if (dwgfx.flipmode){
					dwgfx.print(0, 164, "[Trinkets found]", 196, 196, 255 - help.glow, true);
					dwgfx.print(0, 152, help.number(game.trinkets) + " out of " + help.number(map.customtrinkets), 96, 96, 96, true);
					
					dwgfx.print(0, 114, "[Number of Deaths]", 196, 196, 255 - help.glow, true);
					dwgfx.print(0, 102, String(game.deathcounts),  96, 96, 96, true);
					
					dwgfx.print(0, 64, "[Time Taken]", 196, 196, 255 - help.glow, true);
					dwgfx.print(0, 52, game.timestring(help),  96, 96, 96, true);
				}else{
					dwgfx.print(0, 52, "[Trinkets found]", 196, 196, 255 - help.glow, true);
					dwgfx.print(0, 64, help.number(game.trinkets) + " out of " + help.number(map.customtrinkets), 96, 96, 96, true);
					
					dwgfx.print(0, 102, "[Number of Deaths]", 196, 196, 255 - help.glow, true);
					dwgfx.print(0, 114, String(game.deathcounts),  96, 96, 96, true);
					
					dwgfx.print(0, 152, "[Time Taken]", 196, 196, 255 - help.glow, true);
					dwgfx.print(0, 164, game.timestring(help),  96, 96, 96, true);
				}
			}else{
				if (dwgfx.flipmode) {
					dwgfx.print(0, 164, "[Trinkets found]", 196, 196, 255 - help.glow, true);
					dwgfx.print(0, 152, help.number(game.trinkets) + " out of Twenty", 96,96,96, true);
					
					dwgfx.print(0, 114, "[Number of Deaths]", 196, 196, 255 - help.glow, true);
					dwgfx.print(0, 102, String(game.deathcounts),  96,96,96, true);
					
					dwgfx.print(0, 64, "[Time Taken]", 196, 196, 255 - help.glow, true);
					dwgfx.print(0, 52, game.timestring(help),  96, 96, 96, true);
				}else{	
					dwgfx.print(0, 52, "[Trinkets found]", 196, 196, 255 - help.glow, true);
					dwgfx.print(0, 64, help.number(game.trinkets) + " out of Twenty", 96,96,96, true);
					
					dwgfx.print(0, 102, "[Number of Deaths]", 196, 196, 255 - help.glow, true);
					dwgfx.print(0, 114, String(game.deathcounts),  96,96,96, true);
					
					dwgfx.print(0, 152, "[Time Taken]", 196, 196, 255 - help.glow, true);
					dwgfx.print(0, 164, game.timestring(help),  96, 96, 96, true);
				}
			}
	  break;
		case 3:
			if (game.mobilemenu) {
				dwgfx.drawmobilebutton(game, 30 - 8, 220, 56, dwgfx.b_size, "MAP", 32, 92, 104);
				if (game.insecretlab) { dwgfx.drawmobilebutton(game, 103, 220, 56, dwgfx.b_size, "GRAV", 32, 92, 104);
				}else if (obj.flags[67] == 1 && !map.custommode) { dwgfx.drawmobilebutton(game, 103, 220, 56, dwgfx.b_size, "SHIP", 32, 92, 104);
				}else{ dwgfx.drawmobilebutton(game, 103, 220, 56, dwgfx.b_size, "CREW", 32, 92, 104);}
				dwgfx.drawmobilebutton(game, 185-4, 220, 56, dwgfx.b_size, "STATS", 32, 92, 104);
				dwgfx.drawmobilebutton(game, 258, 220, 56, dwgfx.b_size, "QUIT", 65, 185, 207);
			}else {
				dwgfx.print(30, 220, "MAP", 64,64,64);
				if (game.insecretlab) { dwgfx.print(103, 220, "GRAV", 64, 64, 64);
				}else if (obj.flags[67] == 1 && !map.custommode) { dwgfx.print(103, 220, "SHIP", 64,64,64);
				}else{ dwgfx.print(103, 220, "CREW", 64,64,64);}
				dwgfx.print(185-4, 220, "STATS", 64,64,64);
				dwgfx.print(258 - 8, 220, "[SAVE]", 196, 196, 255 - help.glow);					
			}
			
			dwgfx.print(0, 80, "Quit to main menu?", 255 - (help.glow * 2), 255 - (help.glow * 2), 255 - help.glow, true);
			
			if(help.slowsine%8>4){
				dwgfx.drawmobilebutton(game, 160 + 4 - 24, 110, 8 * 6, dwgfx.b_size, "QUIT", 65, 185, 207);
			}else {
				dwgfx.drawmobilebutton(game, 160 + 4 - 24, 110, 8 * 6, dwgfx.b_size, "QUIT", 65*1.2, 185*1.2, 207*1.2);
			}
			
			if (dwgfx.flipmode) {
				dwgfx.print(0, 186, "(Note: The game is autosaved", 146, 146, 180, true);
				dwgfx.print(0, 174, "at every checkpoint.)", 146, 146, 180, true);
			}else{
				dwgfx.print(0, 174, "(Note: The game is autosaved", 146, 146, 180, true);
				dwgfx.print(0, 186, "at every checkpoint.)", 146, 146, 180, true);
			}
			
			/*
			if (game.inintermission) {
				dwgfx.print(0, 115, "Cannot Save in Level Replay", 146, 146, 180, true);
			}else if (game.nodeathmode) {
				dwgfx.print(0, 115, "Cannot Save in No Death Mode", 146, 146, 180, true);
			}else if (game.intimetrial) {
				dwgfx.print(0, 115, "Cannot Save in Time Trial", 146, 146, 180, true);
			}else if (game.insecretlab) {
				dwgfx.print(0, 115, "Cannot Save in Secret Lab", 146, 146, 180, true);
			}else if (map.custommode){
				if (game.gamesaved){
					dwgfx.print(0, 36, "Game saved ok!", 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2), true);
					
					dwgfx.drawpixeltextbox(25, 65, 270, 90, 34, 12, 65, 185, 207, 0, 4);
					
					if (dwgfx.flipmode){
						dwgfx.print(0, 122, game.customleveltitle, 25, 255 - (help.glow / 2), 255 - (help.glow / 2), true);
						dwgfx.print(160 - 84, 78, game.savetime, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
						dwgfx.print(160 + 40, 78, help.number(game.savetrinkets), 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
						
						dwgfx.drawspritesetcol(50, 74, 50, 18, help);
						dwgfx.drawspritesetcol(175, 74, 22, 18, help);
					}else{
						dwgfx.print(0, 90, game.customleveltitle, 25, 255 - (help.glow / 2), 255 - (help.glow / 2), true);
						dwgfx.print(160 - 84, 132, game.savetime, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
						dwgfx.print(160 + 40, 132, help.number(game.savetrinkets), 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
						
						dwgfx.drawspritesetcol(50, 126, 50, 18, help);
						dwgfx.drawspritesetcol(175, 126, 22, 18, help);
					}
				}else{
					dwgfx.print(0, 80, "[Press ACTION to save your game]", 255 - (help.glow * 2), 255 - (help.glow * 2), 255 - help.glow, true);
				}
			}else {
				if (dwgfx.flipmode) {
					dwgfx.print(0, 186, "(Note: The game is autosaved", 146, 146, 180, true);
					dwgfx.print(0, 174, "at every teleporter.)", 146, 146, 180, true);
				}else{
					dwgfx.print(0, 174, "(Note: The game is autosaved", 146, 146, 180, true);
					dwgfx.print(0, 186, "at every teleporter.)", 146, 146, 180, true);
				}
				
				if (game.gamesaved) {
					dwgfx.print(0, 36, "Game saved ok!", 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2), true);
					
					dwgfx.drawpixeltextbox(25, 65, 270, 90, 34,12, 65, 185, 207,0,4);
					
					if (dwgfx.flipmode) {
						dwgfx.print(0, 132, game.savearea, 25, 255 - (help.glow / 2), 255 - (help.glow / 2), true);
						for (i = 0; i < 6; i++) {
							dwgfx.drawcrewman(169-(3*42)+(i*42), 98, i, game.crewstats[i], help, true);
						}
						dwgfx.print(160 - 84, 78, game.savetime, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
						dwgfx.print(160 + 40, 78, help.number(game.savetrinkets), 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
						
						dwgfx.drawspritesetcol(50, 74, 50, 18, help);
						dwgfx.drawspritesetcol(175, 74, 22, 18, help);	
					}else{
						dwgfx.print(0, 80, game.savearea, 25, 255 - (help.glow / 2), 255 - (help.glow / 2), true);
						for (i = 0; i < 6; i++) {
							dwgfx.drawcrewman(169-(3*42)+(i*42), 95, i, game.crewstats[i], help, true);
						}
						dwgfx.print(160 - 84, 132, game.savetime, 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
						dwgfx.print(160 + 40, 132, help.number(game.savetrinkets), 255 - (help.glow / 2), 255 - (help.glow / 2), 255 - (help.glow / 2));
						
						dwgfx.drawspritesetcol(50, 126, 50, 18, help);
						dwgfx.drawspritesetcol(175, 126, 22, 18, help);
					}
				}else{
					dwgfx.print(0, 80, "[Press ACTION to save your game]", 255 - (help.glow * 2), 255 - (help.glow * 2), 255 - help.glow, true);
					
					if (game.quicksummary != "") {
						if (dwgfx.flipmode) {
							dwgfx.print(0, 110, "Last Save:", 164 - (help.glow / 4), 164 - (help.glow / 4), 164, true);
							dwgfx.print(0, 100, game.quicksummary, 164  - (help.glow / 4), 164 - (help.glow / 4), 164, true);
						}else{
							dwgfx.print(0, 100, "Last Save:", 164 - (help.glow / 4), 164 - (help.glow / 4), 164, true);
							dwgfx.print(0, 110, game.quicksummary, 164  - (help.glow / 4), 164 - (help.glow / 4), 164, true);
						}
					}
				}
			}
			*/
	  break;
		case 10:
			
			if(game.mobilemenu){
				dwgfx.drawmobilebutton(game, 128, 220, 56, dwgfx.b_size, "QUIT", 65, 185, 207);
			}else {
				dwgfx.print(128, 220, "[ QUIT ]", 196, 196, 255 - help.glow);
			}
	    
			if (dwgfx.flipmode) {
				if (game.intimetrial || game.insecretlab || game.nodeathmode || game.menukludge){
				  dwgfx.print(0, 135, "Return to main menu?", 196, 196, 255 - help.glow, true);
			  }else {
				  dwgfx.print(0, 142, "Do you want to quit? You will", 196, 196, 255 - help.glow, true);
				  dwgfx.print(0, 130, "lose any unsaved progress.", 196, 196, 255 - help.glow, true);
				}
				
				if(help.slowsine%8>4){
					dwgfx.drawmobilebutton(game, 160 + 4 - 24, 110, 8 * 6, dwgfx.b_size, "QUIT", 65, 185, 207);
				}else {
					dwgfx.drawmobilebutton(game, 160 + 4 - 24, 110, 8 * 6, dwgfx.b_size, "QUIT", 65*1.2, 185*1.2, 207*1.2);
				}
				//dwgfx.print(80-16, 88, "[ NO, KEEP PLAYING ]", 196, 196, 255 - help.glow);
				//dwgfx.print(80 + 32, 76, "yes, quit to menu",  96, 96, 96);
			}else {	
				
				if (game.intimetrial || game.insecretlab || game.nodeathmode || game.menukludge) {
					dwgfx.print(0, 80, "Return to main menu?", 196, 196, 255 - help.glow, true);
				}else {
				  dwgfx.print(0, 76, "Do you want to quit? You will", 196, 196, 255 - help.glow, true);
				  dwgfx.print(0, 88, "lose any unsaved progress.", 196, 196, 255 - help.glow, true);
			  }
				
				if(help.slowsine%8>4){
					dwgfx.drawmobilebutton(game, 160 + 4 - 24, 110, 8 * 6, dwgfx.b_size, "QUIT", 65, 185, 207);
				}else {
					dwgfx.drawmobilebutton(game, 160 + 4 - 24, 110, 8 * 6, dwgfx.b_size, "QUIT", 65*1.2, 185*1.2, 207*1.2);
				}
				//dwgfx.print(80-16, 130, "[ NO, KEEP PLAYING ]", 196, 196, 255 - help.glow);
				//dwgfx.print(80 + 32, 142, "yes, quit to menu",  96, 96, 96);
			
			}
	  break;
		case 11:
			
			if(game.mobilemenu){
				dwgfx.drawmobilebutton(game, 128, 220, 56, dwgfx.b_size, "QUIT", 65, 185, 207);
			}else {
				dwgfx.print(128, 220, "[ QUIT ]", 196, 196, 255 - help.glow);
			}
	    
	    
			if (dwgfx.flipmode) {
				if (game.intimetrial || game.insecretlab || game.nodeathmode || game.menukludge){
				  dwgfx.print(0, 135, "Return to main menu?", 196, 196, 255 - help.glow, true);
			  }else {
				  dwgfx.print(0, 142, "Do you want to quit? You will", 196, 196, 255 - help.glow, true);
				  dwgfx.print(0, 130, "lose any unsaved progress.", 196, 196, 255 - help.glow, true);
				}
				
				dwgfx.print(80, 88, "no, keep playing", 96,96,96);
				dwgfx.print(80+32-16, 76, "[ YES, QUIT TO MENU ]",  196, 196, 255 - help.glow);
			}else{	
				if (game.intimetrial || game.insecretlab || game.nodeathmode || game.menukludge) {
					dwgfx.print(0, 80, "Return to main menu?", 196, 196, 255 - help.glow, true);
				}else {
				  dwgfx.print(0, 76, "Do you want to quit? You will", 196, 196, 255 - help.glow, true);
				  dwgfx.print(0, 88, "lose any unsaved progress.", 196, 196, 255 - help.glow, true);
			  }
				
				dwgfx.print(80, 130, "no, keep playing", 96,96,96);
				dwgfx.print(80+32-16, 142, "[ YES, QUIT TO MENU ]", 196, 196, 255 - help.glow);
			}
	  break;
		case 20:
			if(game.mobilemenu){
				dwgfx.drawmobilebutton(game, 128, 220, 11*8, dwgfx.b_size, "GRAVITRON", 65, 185, 207);
			}else {
				dwgfx.print(128, 220, "[ GRAVITRON ]", 196, 196, 255 - help.glow, true);
			}
	    
			if (dwgfx.flipmode) {
				dwgfx.print(0, 76, "the secret laboratory?", 196, 196, 255 - help.glow, true);			
				dwgfx.print(0, 88, "Do you want to return to", 196, 196, 255 - help.glow, true);	
				//dwgfx.print(80-16, 142, "[ NO, KEEP PLAYING ]", 196, 196, 255 - help.glow);
				//dwgfx.print(80 + 32, 130, "yes, return",  96, 96, 96);
				if(help.slowsine%8>4){
					dwgfx.drawmobilebutton(game, 160 + 4 - 24, 110, 8 * 6, dwgfx.b_size, "QUIT", 65, 185, 207);
				}else {
					dwgfx.drawmobilebutton(game, 160 + 4 - 24, 110, 8 * 6, dwgfx.b_size, "QUIT", 65*1.2, 185*1.2, 207*1.2);
				}
			}else {
				dwgfx.print(0, 76, "Do you want to return to", 196, 196, 255 - help.glow, true);
				dwgfx.print(0, 88, "the secret laboratory?", 196, 196, 255 - help.glow, true);		
				//dwgfx.print(80-16, 130, "[ NO, KEEP PLAYING ]", 196, 196, 255 - help.glow);
				//dwgfx.print(80 + 32, 142, "yes, return",  96, 96, 96);		
				if(help.slowsine%8>4){
					dwgfx.drawmobilebutton(game, 160 + 4 - 24, 110, 8 * 6, dwgfx.b_size, "QUIT", 65, 185, 207);
				}else {
					dwgfx.drawmobilebutton(game, 160 + 4 - 24, 110, 8 * 6, dwgfx.b_size, "QUIT", 65*1.2, 185*1.2, 207*1.2);
				}
			}
	  break;
		case 21:
			if(game.mobilemenu){
				dwgfx.drawmobilebutton(game, 128, 220, 11*8, dwgfx.b_size, "GRAVITRON", 65, 185, 207);
			}else {
				dwgfx.print(128, 220, "[ GRAVITRON ]", 196, 196, 255 - help.glow, true);
			}
	    
	    if (dwgfx.flipmode) {
				dwgfx.print(0, 76, "the secret laboratory?", 196, 196, 255 - help.glow, true);			
				dwgfx.print(0, 88, "Do you want to return to", 196, 196, 255 - help.glow, true);	
				dwgfx.print(80, 142, "no, keep playing", 96, 96, 96);
				dwgfx.print(80 + 32-16, 130, "[ YES, RETURN ]",  196, 196, 255 - help.glow);	
			}else {
				dwgfx.print(0, 76, "Do you want to return to", 196, 196, 255 - help.glow, true);
				dwgfx.print(0, 88, "the secret laboratory?", 196, 196, 255 - help.glow, true);			
				dwgfx.print(80, 130, "no, keep playing", 96, 96, 96);
				dwgfx.print(80 + 32-16, 142, "[ YES, RETURN ]",  196, 196, 255 - help.glow);	
			}
		break;
		case 30:
			if (game.mobilemenu) {
				dwgfx.drawmobilebutton(game, 30 - 8, 220, 56, dwgfx.b_size, "MAP", 32, 92, 104);
				if (game.insecretlab) { dwgfx.drawmobilebutton(game, 103, 220, 56, dwgfx.b_size, "GRAV", 32, 92, 104);
				}else if (obj.flags[67] == 1 && !map.custommode) { dwgfx.drawmobilebutton(game, 103, 220, 56, dwgfx.b_size, "SHIP", 32, 92, 104);
				}else{ dwgfx.drawmobilebutton(game, 103, 220, 56, dwgfx.b_size, "CREW", 32, 92, 104);}
				dwgfx.drawmobilebutton(game, 185-4, 220, 56, dwgfx.b_size, "STATS", 32, 92, 104);
				dwgfx.drawmobilebutton(game, 258, 220, 56, dwgfx.b_size, "QUIT", 32, 92, 104);
			}
			
			if (dwgfx.flipmode) {
				dwgfx.drawmobilebutton(game, 160 - (12 * 8)-20, 156, 8 * 8, dwgfx.b_size, "SWIPE", 32, 92, 104);
				dwgfx.drawmobilebutton(game, 160 - (4 * 8), 156, 8 * 8, dwgfx.b_size, "SIDES", 32, 92, 104);
				dwgfx.drawmobilebutton(game, 160 + (4 * 8)+20, 156, 8 * 8, dwgfx.b_size, "D-PAD", 32, 92, 104);
				
				switch(game.controlstyle) {
					case 0:
						dwgfx.print(0, 52, "Current mode: VVVVVV Style", 196, 196, 255 - help.glow, true);
						dwgfx.print(0, 88, "Swipe on the left side to move", 196, 196, 255 - help.glow, true);
						dwgfx.print(0, 76, "Tap on the right side to flip", 196, 196, 255 - help.glow, true);
						dwgfx.drawmobilebutton(game, 160 - (12 * 8)-20, 156, 8 * 8, dwgfx.b_size, "SWIPE", 65, 185, 207);
					break;
					case 1:
						dwgfx.print(0, 52, "Current mode: Super Gravitron Style", 196, 196, 255 - help.glow, true);
						dwgfx.print(0, 88, "Touch the left or right edges to move", 196, 196, 255 - help.glow, true);
						dwgfx.print(0, 76, "Touch both edges together to flip", 196, 196, 255 - help.glow, true);
						dwgfx.drawmobilebutton(game, 160 - (4 * 8), 156, 8 * 8, dwgfx.b_size, "SIDES", 65, 185, 207);
					break;
					case 2:
						dwgfx.print(0, 52, "Current mode: Button Style", 196, 196, 255 - help.glow, true);
						dwgfx.print(0, 88, "Use the virtual buttons to move", 196, 196, 255 - help.glow, true);
						dwgfx.print(0, 76, "Tap anywhere else to flip", 196, 196, 255 - help.glow, true);
						dwgfx.drawmobilebutton(game, 160 + (4 * 8)+20, 156, 8 * 8, dwgfx.b_size, "D-PAD", 65, 185, 207);
					break;
				}
			}else {	
				dwgfx.drawmobilebutton(game, 160 - (12 * 8)-20, 156, 8 * 8, dwgfx.b_size, "SWIPE", 32, 92, 104);
				dwgfx.drawmobilebutton(game, 160 - (4 * 8), 156, 8 * 8, dwgfx.b_size, "SIDES", 32, 92, 104);
				dwgfx.drawmobilebutton(game, 160 + (4 * 8)+20, 156, 8 * 8, dwgfx.b_size, "D-PAD", 32, 92, 104);
				/*
				dwgfx.print(25, 189, "SENSITIVITY", 196, 196, 255 - help.glow);
				dwgfx.drawfillrect(200 - 80, 190, 160, 5, 32, 92, 104);
				dwgfx.drawfillrect(200 - 80 + int((155 * 50) / 100) - 1, 190 - 4 - 1, 5 + 2, 5 + 8 + 2, 255, 255, 255);
				dwgfx.drawfillrect(200 - 80 + int((155 * game.controlsensitivity) / 100), 190 - 4, 5, 5 + 8, 65, 185, 207);
				*/
				switch(game.controlstyle) {
					case 0:
						dwgfx.print(0, 52, "Current mode: VVVVVV Style", 196, 196, 255 - help.glow, true);
						dwgfx.print(0, 76, "Swipe on the left side to move", 196, 196, 255 - help.glow, true);
						dwgfx.print(0, 88, "Tap on the right side to flip", 196, 196, 255 - help.glow, true);
						dwgfx.drawmobilebutton(game, 160 - (12 * 8)-20, 156, 8 * 8, dwgfx.b_size, "SWIPE", 65, 185, 207);
					break;
					case 1:
						dwgfx.print(0, 52, "Current mode: Super Gravitron Style", 196, 196, 255 - help.glow, true);
						dwgfx.print(0, 76, "Touch the left or right edges to move", 196, 196, 255 - help.glow, true);
						dwgfx.print(0, 88, "Touch both edges together to flip", 196, 196, 255 - help.glow, true);
						dwgfx.drawmobilebutton(game, 160 - (4 * 8), 156, 8 * 8, dwgfx.b_size, "SIDES", 65, 185, 207);
					break;
					case 2:
						dwgfx.print(0, 52, "Current mode: Button Style", 196, 196, 255 - help.glow, true);
						dwgfx.print(0, 76, "Use the virtual buttons to move", 196, 196, 255 - help.glow, true);
						dwgfx.print(0, 88, "Tap anywhere else to flip", 196, 196, 255 - help.glow, true);
						dwgfx.drawmobilebutton(game, 160 + (4 * 8)+20, 156, 8 * 8, dwgfx.b_size, "D-PAD", 65, 185, 207);
					break;
				}
				
				dwgfx.print(0, 124, "Alternate controls:", 196, 196, 255 - help.glow, true);
			}
	  break;
	}

	dwgfx.drawfade();
	
	if (game.test) {
		dwgfx.print(5, 5, game.teststring, 196, 196, 255, false);
	}

	if (game.flashlight > 0 && !game.noflashingmode) {
		game.flashlight--;
		dwgfx.flashlight();
	}
	
	if (dwgfx.resumegamemode) {
		dwgfx.menuoffset += 25;
		if (map.extrarow) {
			if (dwgfx.menuoffset >= 230) {
				dwgfx.menuoffset = 230;
				//go back to gamemode!
				game.mapheld = true;
				game.gamestate = GAMEMODE;
			}
		}else{
			if (dwgfx.menuoffset >= 240) {
				dwgfx.menuoffset = 240;
				//go back to gamemode!
				game.mapheld = true;
				game.gamestate = GAMEMODE;
			}
		}
		dwgfx.menuoffrender();
	}else if (dwgfx.menuoffset > 0) {
		dwgfx.menuoffset -= 25;
		if (dwgfx.menuoffset < 0) dwgfx.menuoffset = 0;
		dwgfx.menuoffrender();
	}else{
		if (game.screenshake > 0 && !game.noflashingmode) {
			game.screenshake--;
			dwgfx.screenshake();
		}else{
			dwgfx.render();
		}
	}
	
	//dwgfx.backbuffer.unlock();
}

public function towerrender(key:KeyPoll, dwgfx:dwgraphicsclass, game:gameclass, map:mapclass,
											  		obj:entityclass, help:helpclass):void {
  dwgfx.drawbutton(game, help);
  //Couple of changes for the towermode
	
	//dwgfx.backbuffer.lock();
	dwgfx.cls(dwgfx.tower_bgdarkcol[map.colstate]);
	
	if (!game.colourblindmode) {
		dwgfx.drawtowerbackground(map);
	  dwgfx.drawtowermap(game, map);
	}else {
	  dwgfx.drawtowermap_nobackground(map);
	}
	
	if(!game.completestop){
		for (i = 0; i < obj.nentity; i++) {
			//Is this entity on the ground? (needed for jumping)
			if (obj.entities[i].active) {
				if (obj.entities[i].rule == 0 || obj.entities[i].rule >= 6 || obj.entities[i].type == 55) {
					if (obj.entitycollidefloor(map, i)) {obj.entities[i].onground = 2;
					}else {	obj.entities[i].onground--;}
					
					if (obj.entitycollideroof(map, i)) {obj.entities[i].onroof = 2;
					}else { obj.entities[i].onroof--; }				
				}
			}
			
			//Animate the entities
			obj.animateentities(i, game, help);
		}
	}
	
	dwgfx.drawtowerentities(map, obj, help);
	
	dwgfx.drawtowerspikes(map);
	
	
	/*for(i=0; i<obj.nblocks; i++){
    if (obj.blocks[i].active) {
			dwgfx.backbuffer.fillRect(obj.blocks[i].rect, 0xDDDDDD);
    }
  }*/
	dwgfx.cutscenebars();
	
	dwgfx.drawgui(help);	
	if (dwgfx.flipmode) {
		if (game.advancetext) dwgfx.print(5, 228, "- Tap screen to advance text -", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);	
	}else{
	  if (game.advancetext) dwgfx.print(5, 5, "- Tap screen to advance text -", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);	
	}
	
	dwgfx.drawfillrect(dwgfx.footerrect.x, dwgfx.footerrect.y, dwgfx.footerrect.width, dwgfx.footerrect.height, 0);
	dwgfx.print(5, 231, map.roomname, 196, 196, 255 - help.glow, true);
	
	//dwgfx.rprint(5, 231, String(game.coins), 255 - help.glow/2, 255 - help.glow/2, 196, true);
	//dwgfx.drawhuetile(311, 230, 48, 1);
	
	if (game.intimetrial && dwgfx.fademode==0) {
		//Draw countdown!
		if (game.timetrialcountdown > 0) {
			if (game.timetrialcountdown < 30) {
				game.resetgameclock();
				if (int(game.timetrialcountdown / 4) % 2 == 0) dwgfx.bigprint( -1, 100, "Go!", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 4);
			}else if (game.timetrialcountdown < 60) {
				dwgfx.bigprint( -1, 100, "1", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 4);
			}else if (game.timetrialcountdown < 90) {
				dwgfx.bigprint( -1, 100, "2", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 4);
			}else if (game.timetrialcountdown < 120) {
				dwgfx.bigprint( -1, 100, "3", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true, 4);
			}
		}else {
			//Draw OSD stuff	
			dwgfx.bprint(6, 18, "TIME :",  255,255,255);
			dwgfx.bprint(6, 30, "DEATH:",  255, 255, 255);
			dwgfx.bprint(6, 42, "SHINY:",  255,255,255);
				
			if(game.timetrialparlost){
			  dwgfx.bprint(56, 18, game.timestring(help),  196, 80, 80);
			}else {
			  dwgfx.bprint(56, 18, game.timestring(help),  196, 196, 196);
			}
			if(game.deathcounts>0){
			  dwgfx.bprint(56, 30, String(game.deathcounts),  196, 80, 80);
			}else {
			  dwgfx.bprint(56, 30, String(game.deathcounts),  196, 196, 196);
			}
			if(game.trinkets<game.timetrialshinytarget){
			  dwgfx.bprint(56, 42, String(game.trinkets) + " of " +String(game.timetrialshinytarget),  196, 80, 80);
			}else {
			  dwgfx.bprint(56, 42, String(game.trinkets) + " of " +String(game.timetrialshinytarget),  196, 196, 196);
			}
			
			if(game.timetrialparlost){
				dwgfx.bprint(195, 214, "PAR TIME:",  80, 80, 80);
				dwgfx.bprint(275, 214, game.partimestring(help),  80, 80, 80);
			}else {
				dwgfx.bprint(195, 214, "PAR TIME:",  255, 255, 255);
				dwgfx.bprint(275, 214, game.partimestring(help),  196, 196, 196);
			}
		}
	}
	
	dwgfx.drawfade();
	
	if (game.test) {
		dwgfx.print(5, 5, game.teststring, 196, 196, 255, false);
	}

	if (game.flashlight > 0 && !game.noflashingmode) {
		game.flashlight--;
		dwgfx.flashlight();
	}
	
	dwgfx.render();
	//dwgfx.backbuffer.unlock();
}

public function teleporterrender(key:KeyPoll, dwgfx:dwgraphicsclass, game:gameclass, map:mapclass,
													       obj:entityclass, help:helpclass):void {
	
	dwgfx.drawbutton(game, help);
	//dwgfx.backbuffer.lock();
	
	//draw screen alliteration
	//Roomname:
	dwgfx.drawfillrect(0, 0, 320, 12, 0, 0, 0);
	temp = map.area(game.roomx, game.roomy);
	if (temp < 2 && !map.custommode && dwgfx.fademode==0) {
		if (game.roomx >= 102 && game.roomx <= 104 && game.roomy >= 110 && game.roomy <= 111) {
			dwgfx.print(5, 2, "The Ship", 196, 196, 255 - help.glow, true);
		}else{
	    dwgfx.print(5, 2, "Dimension VVVVVV", 196, 196, 255 - help.glow, true);
		}
	}else {
		dwgfx.print(5, 2, map.roomname, 196, 196, 255 - help.glow, true);
	}
	
	//Background color
	dwgfx.drawfillrect(0, 12, 320, 240, 10, 24, 26);
	
	//draw the map image
	dwgfx.drawpixeltextbox(35, 16, 250, 190, 32,24, 65, 185, 207,4,0);
	dwgfx.drawimage(1, 40, 21, false);
	//black out areas we can't see yet
	for (j = 0; j < 20; j++) {
		for (i = 0; i < 20; i++) {
			if(map.explored[i+(j*20)]==0){
				//dwgfx.drawfillrect(10 + (i * 12), 21 + (j * 9), 12, 9, 16, 16, 16);
				dwgfx.drawimage(2, 40 + (i * 12), 21 + (j * 9), false);
			}
		}
	}
	
	//draw the coordinates //current
	if (game.roomx == 109) {
		//tower!instead of room y, scale map.ypos
		dwgfx.drawrect(40 + ((game.roomx - 100) * 12) + 2, 21  + 2, 12 - 4, 180 - 4, 16, 245 - (help.glow * 2), 245 - (help.glow * 2));		
	}else{
		dwgfx.drawrect(40 + ((game.roomx - 100) * 12) + 2, 21 + ((game.roomy - 100) * 9) + 2, 12 - 4, 9 - 4, 16, 245 - (help.glow * 2), 245 - (help.glow * 2));		
	}
	
	if (game.useteleporter) {
		//Draw the chosen destination coordinate!
		
		//draw the coordinates //destination
		tempx = map.teleporters[game.teleport_to_teleporter].x; 
		tempy = map.teleporters[game.teleport_to_teleporter].y;
		dwgfx.drawrect(40 + (tempx * 12) + 1, 21 + (tempy * 9) + 1, 12 - 2, 9 - 2, 245 - (help.glow * 2), 16, 16);		
		dwgfx.drawrect(40 + (tempx * 12) + 3, 21 + (tempy * 9) + 3, 12 - 6, 9 - 6, 245 - (help.glow * 2), 16, 16);		
	}

	//draw legend details
	for (i = 0; i < map.numteleporters; i++) {
		if (map.showteleporters && map.explored[map.teleporters[i].x + (20 * map.teleporters[i].y)] > 0) {
			temp = 1126 + map.explored[map.teleporters[i].x + (20 * map.teleporters[i].y)];
			if (dwgfx.flipmode) temp += 3;
			dwgfx.drawtile(40 + 3 + (map.teleporters[i].x * 12), 22 + (map.teleporters[i].y * 9), temp);
		}else if(map.showtargets && map.explored[map.teleporters[i].x+(20*map.teleporters[i].y)]==0){
			temp = 1126 + map.explored[map.teleporters[i].x + (20 * map.teleporters[i].y)];
			if (dwgfx.flipmode) temp += 3;
			dwgfx.drawtile(40 + 3 + (map.teleporters[i].x * 12), 22 + (map.teleporters[i].y * 9), temp);
		}
		//dwgfx.drawtile(40+3 + (map.teleporters[i].x * 12), 22 + (map.teleporters[i].y * 9), 1086); //for shiny trinkets, do later
	}
				
			if (map.showtrinkets) {
			  for (i = 0; i < map.numshinytrinkets; i++) {
				  if (obj.collect[i] == 0) {
						temp = 1086;
      			if (dwgfx.flipmode) temp += 3;
						dwgfx.drawtile(40 + 3 + (map.shinytrinkets[i].x * 12), 22 + (map.shinytrinkets[i].y * 9),	temp);
				  }
				}
			}
	
	if (game.useteleporter && ((help.slowsine%16)>8)) {
		//colour in the legend
		temp = 1128;
    if (dwgfx.flipmode) temp += 3;
		dwgfx.drawtile(40 + 3 + (tempx * 12), 22 + (tempy * 9), temp);	
	}
	
	dwgfx.cutscenebars();
	
	
	if (game.useteleporter) {
		//Instructions!
		if (game.mobilemenu) {
			dwgfx.drawmobilebutton(game, 5+10, 220-10, 64, dwgfx.b_size, "LEFT", 65, 185, 207);
			dwgfx.drawmobilebutton(game, 320 - 5 - 64, 220-10, 64, dwgfx.b_size, "RIGHT", 65, 185, 207);
			dwgfx.drawmobilebutton(game, 160 - 40+4, 220, 64+16, dwgfx.b_size, "TELEPORT", 65, 185, 207);
		}else{
			dwgfx.print(5, 210, "Press Left/Right to choose a Teleporter", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
			dwgfx.print(5, 225, "Press ENTER to Teleport", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
		}
	}
	
	dwgfx.drawgui(help);
	
	if (dwgfx.flipmode) {
		if (game.advancetext) dwgfx.print(5, 228, "- Tap screen to advance text -", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);	
	}else{
	  if (game.advancetext) dwgfx.print(5, 5, "- Tap screen to advance text -", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);	
	}
	
	
	if (game.test) {
		dwgfx.print(5, 5, game.teststring, 196, 196, 255, false);
	}

	if (game.flashlight > 0 && !game.noflashingmode) {
		game.flashlight--;
		dwgfx.flashlight();
	}
	
	if (dwgfx.resumegamemode) {
		dwgfx.menuoffset += 25;
		if (map.extrarow) {
			if (dwgfx.menuoffset >= 230) {
				dwgfx.menuoffset = 230;
				//go back to gamemode!
				game.mapheld = true;
				game.gamestate = GAMEMODE;
			}
		}else{
			if (dwgfx.menuoffset >= 240) {
				dwgfx.menuoffset = 240;
				//go back to gamemode!
				game.mapheld = true;
				game.gamestate = GAMEMODE;
			}
		}
		dwgfx.menuoffrender();
	}else if (dwgfx.menuoffset > 0) {
		dwgfx.menuoffset -= 25;
		if (dwgfx.menuoffset < 0) dwgfx.menuoffset = 0;
		dwgfx.menuoffrender();
	}else{
		if (game.screenshake > 0 && !game.noflashingmode) {
			game.screenshake--;
			dwgfx.screenshake();
		}else{
			dwgfx.render();
		}
	}
	
	//dwgfx.backbuffer.unlock();
}

public function controltutorialrender(key:KeyPoll, dwgfx:dwgraphicsclass, game:gameclass, map:mapclass,
													       obj:entityclass, help:helpclass):void {
	
	dwgfx.drawbutton(game, help);
	//dwgfx.backbuffer.lock();
	
	//Background color
	dwgfx.drawfillrect(0, 0, 320, 240, 10, 24, 26);
	
	for (i = 0; i < obj.nentity; i++) {
		//Is this entity on the ground? (needed for jumping)
		if (obj.entities[i].active) {
			if (obj.entities[i].rule == 0 || obj.entities[i].rule >= 6 || obj.entities[i].type == 55) {
				if (obj.entitycollidefloor(map, i)) {obj.entities[i].onground = 2;
				}else {	obj.entities[i].onground--;}
				
				if (obj.entitycollideroof(map, i)) {obj.entities[i].onroof = 2;
				}else { obj.entities[i].onroof--; }				
			}
		}
		
		//Animate the entities
		obj.animateentities(i, game, help);
	}
	
	dwgfx.drawentities(map, obj, help);
	
	dwgfx.drawtextbox(160-120, 2, 30, 3, 174, 174, 174);
	dwgfx.print(5, 10, "-= TOUCHSCREEN CONTROLS =-", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
	
	if(game.controltutorialstate>=3 &&game.controltutorialstate<=6){	
		dwgfx.print(5, 195 + 8, "Swipe and hold on the left side", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
		dwgfx.print(5, 205 + 8, "of the screen to move", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
	}
	
	if(game.controltutorialstate>=7 &&game.controltutorialstate<=11){	
	  dwgfx.print(5, 200 + 8, "Tap on the right to flip", 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2), true);
	}
	
	if (game.controltutorialstate >= 13) {
		if(help.slowsine%8>4){
			dwgfx.drawmobilebutton(game, 160 + 4 - 24, 210, 8 * 7, dwgfx.b_size, "START", 65, 185, 207);
		}else {
			dwgfx.drawmobilebutton(game, 160 + 4 - 24, 210, 8 * 7, dwgfx.b_size, "START", 65*1.2, 185*1.2, 207*1.2);
		}
	}
	
	dwgfx.drawmobileimage(0, 0, 70 + ((100 - game.controllerp1) * 3), true);
	
	//Draw touchpoints
	if(!game.press_left && !game.press_right){
		//- 1 Left hand off
		dwgfx.drawmobilehands(1, 56, 106 + ((100 - game.controllerp1) * 3));
	}else if(game.press_left){		
		//- 2 Left hand moving left
		if (game.controllerp3 <= 8) {
			if(help.slowsine%16>8){ dwgfx.drawrect(116, 120, 16, 16, 255, 255, 255);
			}else {	dwgfx.drawrect(116+2, 120+2, 16-4, 16-4, 255, 255, 255); }
			dwgfx.drawmobilehands(3, 56, 106);
			dwgfx.drawmobileimage(7, 136, 120);
		}else{
			if(help.slowsine%16>8){ dwgfx.drawrect(104, 118, 16, 16, 255, 255, 255);
			}else {	dwgfx.drawrect(104+2, 118+2, 16-4, 16-4, 255, 255, 255); }
			dwgfx.drawmobilehands(2, 56, 106);
			dwgfx.drawmobileimage(7, 124, 120);
		}
	}else if(game.press_right){
		//- 3 Left hand moving right
		if (game.controllerp3 <= 8) {
			if(help.slowsine%16>8){ dwgfx.drawrect(104, 118, 16, 16, 255, 255, 255);
			}else {	dwgfx.drawrect(104+2, 118+2, 16-4, 16-4, 255, 255, 255); }
			dwgfx.drawmobilehands(2, 56, 106);
			dwgfx.drawmobileimage(8, 84, 120);
		}else{
			if(help.slowsine%16>8){ dwgfx.drawrect(116, 120, 16, 16, 255, 255, 255);
			}else {	dwgfx.drawrect(116+2, 120+2, 16-4, 16-4, 255, 255, 255); }
			dwgfx.drawmobilehands(3, 56, 106);
			dwgfx.drawmobileimage(8, 96, 120);
		}
	}
	
	if(game.controltutorialstate>=7){	
		if (game.controllerp2 > 0) {
			if(help.slowsine%16>8){ dwgfx.drawrect(188, 120, 16, 16, 255, 255, 255);
			}else {	dwgfx.drawrect(188+2, 120+2, 16-4, 16-4, 255, 255, 255); }
			dwgfx.drawmobilehands(6, 193, 106);	
		}else{
			dwgfx.drawmobilehands(4, 193, 106 + ((100 - game.controllerp1) * 3));
		}
	}
	
	if (game.test) {
		dwgfx.print(5, 5, game.teststring, 196, 196, 255, false);
	}

	if (game.flashlight > 0 && !game.noflashingmode) {
		game.flashlight--;
		dwgfx.flashlight();
	}
	
	if (game.screenshake > 0 && !game.noflashingmode) {
		game.screenshake--;
		dwgfx.screenshake();
	}else{
		dwgfx.render();
	}
	
	//dwgfx.backbuffer.unlock();
}