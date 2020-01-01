package {
	import bigroom.input.KeyPoll;
	import flash.display.*;
	import flash.geom.*;
  import flash.events.*;
  import flash.net.*;
	import flash.desktop.NativeApplication;
		
	public class gameclass extends Sprite {		
		public var GAMEMODE:int = 0;
		public var TITLEMODE:int = 1;
		public var CLICKTOSTART:int = 2;
		public var FOCUSMODE:int = 3;
		public var MAPMODE:int = 4;
		public var TELEPORTERMODE:int = 5;
		public var GAMECOMPLETE:int = 6;
		public var GAMECOMPLETE2:int = 7;
		public var EDITORMODE:int = 8;
		public var CONTROLTUTORIALMODE:int = 9;
    
		public function gameclass(dwgfx:dwgraphicsclass, map:mapclass, obj:entityclass, help:helpclass, music:musicclass):void {
			platform.init(); //Cross platform specific init
			NativeApplication.nativeApplication.addEventListener(Event.ACTIVATE, onActivate);
			NativeApplication.nativeApplication.addEventListener(Event.DEACTIVATE, onDeactivate);
			
			infocus = true; paused = false; muted = false; globalsound = 1;
			/*
			if(device.devicetype != device.MOBILE){
			  addEventListener(Event.DEACTIVATE, windowNotActive);
        addEventListener(Event.ACTIVATE, windowActive);
			}
			*/
			
			//iOS only, turn off for android
		  scores.init(stage);
				
			//gamestate = GAMEMODE;
	    hascontrol = true; jumpheld = false; advancetext = false; jumppressed = 0;
			gravitycontrol = 0; teleport = false; companion = 0; roomchange = false;
			
			edteleportent = 0; //Added in the port!
			teleport_to_new_area = false; mapheld = false; teleport_to_x = 0; teleport_to_y = 0;
			teleportscript = "";
			savemystats = false;
			menukludge = false;
			quickrestartkludge = false;
			
			tapleft = 0; tapright = 0;
			
			recording = 0; recordinit = false; playbackfinished = false;
			recordstring = "";
			
			advancetext = false; pausescript = false; completestop = false;
			activeactivity = -1; act_fade = 0;
			backgroundtext = false;
			startscript = false; newscript = "";
			inintermission = false;
			
			alarmon = false; alarmdelay = 0; blackout = false; 
			creditposx = 0; creditposy = 0; creditposdelay = 0;
			
			useteleporter = false; teleport_to_teleporter = 0;
			
			teleblock = new Rectangle();
			activetele = false; readytotele = 0;
			activity_lastprompt = ""; activity_r = 0; activity_g = 0; activity_b = 0;
			creditposition = 0;
			bestgamedeaths = -1;
			
			//Accessibility Options
			colourblindmode = false;
			noflashingmode = false;
			slowdown = 30;
			
			fullscreen = false;// true; //Assumed true at first unless overwritten at some point!
			advanced_mode = false;
			advanced_smoothing = true;
		  advanced_scaling = 0;
			
			nodeathmode = false; 
			nocutscenes = false;
			
			for (i = 0; i < 50; i++){
				customscript.push("");
			}
			customcol = 0;
			
			for (i = 0; i < 6; i++) {
				var cstats:Boolean;
				cstats = false;
				crewstats.push(cstats);
				tele_crewstats.push(false);
				quick_crewstats.push(false);
				besttimes.push( -1);
				besttrinkets.push( -1);
				bestlives.push( -1);
				bestrank.push( -1);
			}
			crewstats[0] = true;
			lastsaved = 0;
				
		  tele_gametime = "00:00";
		  tele_trinkets = 0;
		  tele_currentarea = "Error! Error!";
		  quick_gametime = "00:00";
		  quick_trinkets = 0;
		  quick_currentarea = "Error! Error!";
			
			//Menu stuff initiliased here:
			for (i = 0; i < 25; i++) {
				menuoptions.push(new String);
				menuoptions_alt.push(new String);
				menuoptionsactive.push(new Boolean);
				menuoptionslayout.push(new int(0));
				
				var nb1:Boolean, nb2:Boolean;
				nb1 = false; nb2 = false;
				unlock.push(nb1);
				unlocknotify.push(nb2);
			}
			
			for (i = 0; i < 25; i++) { unlock[i] = false; unlocknotify[i] = false; }
			
			nummenuoptions = 0; currentmenuoption = 0;
		  menuselection = "null"; currentmenuname = "null";
			menuxoff = 0; menuyoff = 0;
			menucountdown = 0; menudest="null";
			createmenu("mainmenu");
			
		  deathcounts = 0; gameoverdelay = 0;
		  frames = 0; seconds = 0; minutes = 0; hours = 0;
			gamesaved = false; 
			savetime = "00:00"; savearea = "nowhere"; savetrinkets = 0;
			
			intimetrial = false;
      timetrialcountdown = 0; timetrialshinytarget = 0;
			timetrialparlost = false; timetrialpar = 0; timetrialresulttime = 0;
			
	    totalflips = 0;
		  hardestroom = "Welcome Aboard"; hardestroomdeaths = 0; currentroomdeaths=0;
			
			sfpsmode = false; //by default, play at 30 fps
			inertia = 1.1;
			swnmode = false; swntimer = 0; swngame = 0;//Not playing sine wave ninja!
			swnstate = 0; swnstate2 = 0; swnstate3 = 0; swnstate4 = 0;  swndelay = 0; swndeaths = 0;
			supercrewmate = false; scmhurt = false; scmprogress = 0; scmmoveme = false;
			swncolstate = 0; swncoldelay = 0;
			swnrecord = 0; swnbestrank = 0; swnrank = 0; swnmessage = 0;
			
			for (i = 0; i < 200; i++) {
				customlevelstats.push("");
				customlevelscore.push(int(0));
			}
			
			clearcustomlevelstats();
			
			//Save stuff
			for (i = 0; i < 3; i++) {
				var supersave:saveclass = new saveclass();
				savestate.push(supersave);
			}
			
			telecookie = SharedObject.getLocal("vvvvvv_telesave");
			quickcookie = SharedObject.getLocal("vvvvvv_quicksave");
			levelstatscookie = SharedObject.getLocal("vvvvvv_levelstats");
			for (var i:int = 0; i < editor.playerlevel.length; i++) {
				customcookie.push(SharedObject.getLocal("vvvvvv_custom_" + String(i)));
				customcookieexists.push(false);
				customcookiesummary.push(false);
			}
			
			levelstatscookieloaded = true;
			if (levelstatscookie.data.num == undefined) {
        levelstatscookie.data.num = 0;
      }
			
			if (telecookie.data.savex == undefined) {
        telecookieexists = false; telesummary = "";
      } else {
        telecookieexists = true; telesummary = telecookie.data.summary;
      }
			
			if (quickcookie.data.savex == undefined) {
        quickcookieexists = false; quicksummary = "";
      } else {
        quickcookieexists = true; quicksummary = quickcookie.data.summary;
      }
			
			for (i = 0; i < editor.playerlevel.length; i++) {
				if(customcookie[i].data.savex == undefined) {
					customcookieexists[i] = false; customcookiesummary[i] = "";
				} else {
					customcookieexists[i] = true; customcookiesummary[i] = customcookie[i].data.summary;
				}
			}
			
			screenshake = flashlight = 0;
			
			stat_trinkets = 0;
			
			test = false; teststring = "TEST = True";
			state = 1; statedelay = 0;
			updatestate(dwgfx, map, obj, help, music);
		}	
		
		public function onActivate(e:Event):void {
			platform.callonwake();
			infocus = true; 
		} 
		
		public function onDeactivate(e:Event):void {
			infocus = false; 
		} 
		
		public function clearcustomlevelstats():void{
			//just clearing the arrays
			for (var i:int = 0; i < 200; i++) {
				customlevelstats[i] = "";
				customlevelscore[i] = 0;
			}
			numcustomlevelstats = 0;
			
			customlevelstatsloaded=false; //To ensure we don't load it where it isn't needed
		}
		
		public function updatecustomlevelstats(clevel:String, cscore:int):void{
			var tvar:int = -1;
			for (var j:int = 0; j < numcustomlevelstats; j++) {
				if (clevel == customlevelstats[j]) {
					tvar=j;
					j=numcustomlevelstats+1;
				}
			}
			
			if (tvar >= 0) {
				//update existing entry
				customlevelscore[tvar]=cscore;
			}else{
				//add a new entry
				if (numcustomlevelstats < 200) {
					customlevelstats[numcustomlevelstats]=clevel;
					customlevelscore[numcustomlevelstats]=cscore;
					numcustomlevelstats++;
				}
			}
			savecustomlevelstats();
		}
		
		public function loadcustomlevelstats():void{
      //Totally rewriten for mobile version, probably broken
			if (!customlevelstatsloaded) {
        numcustomlevelstats = levelstatscookie.data.num;
				if (numcustomlevelstats > 0) {
          if (numcustomlevelstats >= 200) numcustomlevelstats = 199;
					
					var lvlstring:String = levelstatscookie.data.customlevelscore;
					var values:Array;
					values = new Array;
					values = lvlstring.split(",");
					for (var i:int = 0; i < values.length; i++) {
            if (i < 200) customlevelscore[i] = int(values[i]);
          }
					
					lvlstring = levelstatscookie.data.customlevelstats;
					values = new Array;
					values = lvlstring.split("|");
					for(i = 0; i < values.length; i++){
            if (i < 200) customlevelstats[i] = values[i];
          }
				}
			}
		}
		
		public function savecustomlevelstats():void {
			//Rewritten for mobile
			levelstatscookie = SharedObject.getLocal("vvvvvv_levelstats");
			
			if (numcustomlevelstats >= 200) numcustomlevelstats = 199;
			levelstatscookie.data.num = numcustomlevelstats;
			
			var lvlstring:String = "";
			for (var i:int = 0; i < numcustomlevelstats; i++) {
        lvlstring = lvlstring + customlevelscore[i] + ",";
      }
			levelstatscookie.data.customlevelscore = lvlstring;
			
			lvlstring = ""
			for (i = 0; i < numcustomlevelstats; i++) {
        lvlstring = lvlstring + customlevelstats[i] + "|";
      }
			levelstatscookie.data.customlevelstats = lvlstring;
			
			levelstatscookie.flush();
			levelstatscookie.close();	
		}
		
		public function inittouchpoints(key:KeyPoll):void {
			if (key.touchPoints > 0) {
				my = key.touchy[key.touchPoints - 1];
			}
		}
		
		public function inbox(x:int, y:int, x1:int, y1:int, w:int, h:int):Boolean {
			if (x >= x1 && x <= x1 + w) {
				if (y >= y1 && y <= y1 + h) {
					return true;
				}
			}
			return false;
		}
		
		public var m_touchx:int, m_touchy:int;
		public var m_pressed:Boolean = false;
		public var b_gap:int = 40, b_size:int = 26;
		
		public function gettouchpoints_mainmenu(key:KeyPoll, dwgfx:dwgraphicsclass):void {
			//Checks for last touchpress. For main menu, check to see that you've lifted
			//your finger before continuing.
			if (!m_pressed) {
				if (key.touchPoints > 0) {
					m_pressed = true;
					m_touchx = key.touchx[key.touchPoints - 1];
					m_touchy = key.touchy[key.touchPoints - 1];
					if (dwgfx.flipmode) m_touchy = dwgfx.devicey - m_touchy;
					//These coordinates now need to be translated to actual screen coordinates
					//Gamecenter
					if(menustart && !showloadingnotice) {
						if (inbox(m_touchx, m_touchy, 0, 0, dwgfx.buttonscreen[0].width + (dwgfx.buttonxspacing * 2), dwgfx.buttonscreen[0].height + (dwgfx.buttonyspacing * 2))) {
							//Bring up game center achievements
							scores.showAchievements();
						}
					}
					
					m_touchx -= dwgfx.screen.x;
					m_touchx = m_touchx / dwgfx.screensizemultiplier;
					m_touchy = m_touchy / dwgfx.screensizemultiplier;
					
					//Ok! Now we see if we've pressed any buttons!
					
					for (i = 0; i < nummenuoptions; i++) {
						switch(menuoptionslayout[i]) {
							case 0:
								if (menuoptionsactive[i]) {
									if (inbox(m_touchx, m_touchy, 160 - 70, 140 + (i * b_gap) + menuyoff - 6, 140, b_size)) {
										press_action = true;
										currentmenuoption = i;
									}
								}
							break;
							case 1:
								if (menuoptionsactive[i]) {
									if (inbox(m_touchx, m_touchy, 160 - 70, 140 + (i * b_gap) - 10 + menuyoff - 6, 140, b_size * 2)) {
										press_action = true;
										currentmenuoption = i;
									}
								}
							break;
							case 2:
								if (menuoptionsactive[i]) {
									if (inbox(m_touchx, m_touchy, 160 - 70, 140 + (i * b_gap)+20 + menuyoff - 6, 140, b_size)) {
										press_action = true;
										currentmenuoption = i;
									}
								}
							break;
							case 3:
								if (menuoptionsactive[i]) {
									if (inbox(m_touchx, m_touchy, -8, 230 - 6, 70, b_size)) {
										press_action = true;
										currentmenuoption = i;
									}
								}
							break;
							case 4:
								if (menuoptionsactive[i]) {
									if (inbox(m_touchx, m_touchy, 160 - 90, 140 + (i * b_gap) + menuyoff - 6, 180, b_size)) {
										press_action = true;
										currentmenuoption = i;
									}
								}
							break;
							case 5:
								if (menuoptionsactive[i]) {
									if (inbox(m_touchx, m_touchy, 10, 140 + (i * 68)-10 + menuyoff - 6, 140, 40)) {
										press_action = true;
										currentmenuoption = i;
									}
								}
							break;
							case 6:
								if (menuoptionsactive[i]) {
									if (inbox(m_touchx, m_touchy, 170, 140 + ((i-3) * 68)-10 + menuyoff - 6, 140, 40)) {
										press_action = true;
										currentmenuoption = i;
									}
								}
							break;
							case 7:
								if (menuoptionsactive[i]) {
									if (inbox(m_touchx, m_touchy, -8, 230 - 6-10, 70, b_size)) {
										press_action = true;
										currentmenuoption = i;
									}
								}
							break;
							case 8:
								if (menuoptionsactive[i]) {
									if (inbox(m_touchx, m_touchy, 320 - 70, 230 - 6-10, 70, b_size)) {
										press_action = true;
										currentmenuoption = i;
									}
								}
							break;
							case 9:
								if (menuoptionsactive[i]) {
									if (inbox(m_touchx, m_touchy, 160 - 90, 140 + (i * b_gap) + menuyoff - 6, 180, b_size)) {
										press_action = true;
										currentmenuoption = i;
									}
								}
							break;
							case 10:
								if (menuoptionsactive[i]) {
									if (inbox(m_touchx, m_touchy, 160 - 70, 240 - b_gap, 140, b_size)) {
										press_action = true;
										currentmenuoption = i;
									}
								}
							break;
							case 11:
								if (menuoptionsactive[i]) {
									if (inbox(m_touchx, m_touchy, 160 - 140, 140 + (i * 65)-10 + menuyoff - 6, 280, (b_size*2)+5)) {
										press_action = true;
										currentmenuoption = i;
									}
								}
							break;
							case 12:
								if (menuoptionsactive[i]) {
									if (inbox(m_touchx, m_touchy, 320 - 70, 0, 70, b_size+10)) {
										press_action = true;
										currentmenuoption = i;
									}
								}
							break;
						}
						
						if (!menustart) press_action = true;
					}
				}
			}else {
				if (key.touchPoints == 0) {
					m_pressed = false;
				}
			}
		}
		
		
		public function gettouchpoints_gamemenu(key:KeyPoll, obj:entityclass, music:musicclass, dwgfx:dwgraphicsclass):void {
			//Buttons for in game menu!
			if (!m_pressed) {
				if (key.touchPoints > 0) {
					m_pressed = true;
					m_touchx = key.touchx[key.touchPoints - 1];
					m_touchy = key.touchy[key.touchPoints - 1];
					if (dwgfx.flipmode) m_touchy = dwgfx.devicey - m_touchy;
					
					if (inbox(m_touchx, m_touchy, dwgfx.devicex - dwgfx.buttonscreen[0].width - dwgfx.buttonxspacing, 0, dwgfx.buttonscreen[0].width + (dwgfx.buttonxspacing*2), dwgfx.buttonscreen[0].height + (dwgfx.buttonyspacing*2) )) {
						press_map = true;
					}
					if(menupage == 0){
						if (inbox(m_touchx, m_touchy, 0, 0, dwgfx.buttonscreen[1].width + (dwgfx.buttonxspacing*2), dwgfx.buttonscreen[1].height + (dwgfx.buttonyspacing*2) )) {
							menupage = 30;
							music.playef(11, 10);
						}
					}
					
					//These coordinates now need to be translated to actual screen coordinates
					m_touchx -= dwgfx.screen.x;
					m_touchx = m_touchx / dwgfx.screensizemultiplier;
					m_touchy = m_touchy / dwgfx.screensizemultiplier;
					
					//Ok! Now we see if we've pressed any buttons!
					if (inbox(m_touchx, m_touchy, 30 - 8-10, 220-20, 56+20, b_size+40)) {
						//Press map button
						//disablekludge_1 = !disablekludge_1;
						menupage = 0;
						music.playef(11, 10);
					}else if (inbox(m_touchx, m_touchy, 103-10, 220-20, 56+20, b_size+40)) {
						//Press crew button
						//disablekludge_2 = !disablekludge_2;
						menupage = 1;
						music.playef(11, 10);
					}else if (inbox(m_touchx, m_touchy, 185-4-10, 220-20, 56+20, b_size+40)) {
						//Press stats button
						//disablekludge_3 = !disablekludge_3;
						menupage = 2;
						music.playef(11, 10);
					}else if (inbox(m_touchx, m_touchy, 258-10, 220-20, 56+20, b_size+40)) {
						//Press quit button
						//disablekludge_4 = !disablekludge_4;
						menupage = 3;
						music.playef(11, 10);
					}
					
					if (menupage == 1 && !insecretlab && obj.flags[67] == 1) {
						if (inbox(m_touchx, m_touchy, 160 + 4 - 24-20, 110-20, (8 * 6)+40, b_size+40)) {
							//Warp to ship!
							press_action = true;
						}
					}
					if (menupage == 3 || menupage == 10 || menupage == 20) {
						if (inbox(m_touchx, m_touchy, 160 + 4 - 24-20, 110-20, (8 * 6)+40, b_size+40)) {
							//Press quit button
							press_action = true;
						}
					}
					if (menupage == 30) {
						//Change controls
						if (inbox(m_touchx, m_touchy, 160 - (12 * 8) - 20-10, 156-20, (8 * 8)+20, b_size+40)) {
							if(controlstyle!=0){
								controlstyle = 0;
								music.playef(11, 10);
								savemystats = true;
							}
						}
						if (inbox(m_touchx, m_touchy, 160 - (4 * 8)-20, 156-10, (8 * 8)+20, b_size+40)) {
							if(controlstyle!=1){
								controlstyle = 1;
								music.playef(11, 10);
								savemystats = true;
							}
						}
						if (inbox(m_touchx, m_touchy, 160 + (4 * 8)+20-10, 156-20, (8 * 8)+20, b_size+40)) {
							if(controlstyle!=2){
								controlstyle = 2;
								music.playef(11, 10);
								savemystats = true;
							}
						}
					}
					
				}
			}else {
				if (key.touchPoints == 0) {
					m_pressed = false;
				}
			}
		}
		
		public function gettouchpoints_teleportmenu(key:KeyPoll, music:musicclass, dwgfx:dwgraphicsclass):void {
			//Buttons for in game menu!
			if (!m_pressed) {
				if (key.touchPoints > 0) {
					m_pressed = true;
					m_touchx = key.touchx[key.touchPoints - 1];
					m_touchy = key.touchy[key.touchPoints - 1];
					if (dwgfx.flipmode) m_touchy = dwgfx.devicey - m_touchy;
					//These coordinates now need to be translated to actual screen coordinates
					m_touchx -= dwgfx.screen.x;
					m_touchx = m_touchx / dwgfx.screensizemultiplier;
					m_touchy = m_touchy / dwgfx.screensizemultiplier;
					
					//Ok! Now we see if we've pressed any buttons!
					if (inbox(m_touchx, m_touchy, 5+10-20, 220-10-20, 64+40, b_size+40)) {
						//Press left
						press_left = true;
						music.playef(11, 10);
					}else if (inbox(m_touchx, m_touchy,320 - 5 - 64-20, 220-10-20, 64+40, b_size+40)) {
						//Press right
						press_right = true;
						music.playef(11, 10);
					}else if (inbox(m_touchx, m_touchy, 160 - 40+4-20, 220-20, 64+16+40, b_size+40)) {
						//Press teleport
						press_map = true;
						music.playef(11, 10);
					}
				}
			}else {
				if (key.touchPoints == 0) {
					m_pressed = false;
				}
			}
		}
		
		public function gettouchpoints_gamecomplete(key:KeyPoll, music:musicclass, dwgfx:dwgraphicsclass):void {
			//Buttons for in game menu!
			if (!m_pressed) {
				if (key.touchPoints > 0) {
					/*
					m_touchx = key.touchx[key.touchPoints - 1];
					m_touchy = key.touchy[key.touchPoints - 1];
					if (dwgfx.flipmode) m_touchy = dwgfx.devicey - m_touchy;
					//These coordinates now need to be translated to actual screen coordinates
					m_touchx -= dwgfx.screen.x;
					m_touchx = m_touchx / dwgfx.screensizemultiplier;
					m_touchy = m_touchy / dwgfx.screensizemultiplier;
					*/
					press_action = true;
					
				}
			}else {
				if (key.touchPoints == 0) {
					m_pressed = false;
				}
			}
		}
		
		public function gettouchpoints_controltutorial(key:KeyPoll, music:musicclass, dwgfx:dwgraphicsclass):void {
			//Buttons for in game menu!
			if (!m_pressed) {
				if (key.touchPoints > 0) {
					m_pressed = true;
					m_touchx = key.touchx[key.touchPoints - 1];
					m_touchy = key.touchy[key.touchPoints - 1];
					if (dwgfx.flipmode) m_touchy = dwgfx.devicey - m_touchy;
					//These coordinates now need to be translated to actual screen coordinates
					m_touchx -= dwgfx.screen.x;
					m_touchx = m_touchx / dwgfx.screensizemultiplier;
					m_touchy = m_touchy / dwgfx.screensizemultiplier;
					
					//Ok! Now we see if we've pressed any buttons!
					if (inbox(m_touchx, m_touchy, 160 + 4 - 24-20, 200-20, (8 * 7)+40, b_size+40)) {
						//Press continue
						press_action = true;
						music.playef(11, 10);
					}
				}
			}else {
				if (key.touchPoints == 0) {
					m_pressed = false;
				}
			}
		}
		
		public function gettouchpoints(key:KeyPoll, dwgfx:dwgraphicsclass):void {
			//Iterate through touch points, find buttons that are being pressed
			switch(controlstyle) {
				case 0:
					//Swipe controls
					for (i = 0; i < key.touchPoints; i++) {
						/*if (inbox(key.touchx[i], key.touchy[i], 0, dwgfx.buttonpos[0].y - 100, dwgfx.buttonsize + dwgfx.buttonxspacing+ (dwgfx.buttonxspacing/2), dwgfx.buttonsize + 200)) {
							press_left = true;
						}else if (inbox(key.touchx[i], key.touchy[i], dwgfx.buttonpos[1].x - (dwgfx.buttonxspacing/2), dwgfx.buttonpos[1].y - 100, dwgfx.buttonsize + (dwgfx.buttonxspacing*3), dwgfx.buttonsize + 200)) {
							press_right = true;
						}else 
						*/
						if (insecretlab) {
							if (inbox(key.touchx[i],  key.touchy[i], 0, 0, dwgfx.buttonscreen[0].width + (dwgfx.buttonxspacing * 2), dwgfx.buttonscreen[0].height + (dwgfx.buttonyspacing * 2) )) {
								scores.opengamecenter();
								press_map = true;
							}
						}
						if (inbox(key.touchx[i],  key.touchy[i], dwgfx.devicex - dwgfx.buttonscreen[0].width - dwgfx.buttonxspacing, 0, dwgfx.buttonscreen[0].width + (dwgfx.buttonxspacing*2), dwgfx.buttonscreen[0].height + (dwgfx.buttonyspacing*2) )) {
							press_map = true;
						}else {
							if (key.touchid[i] != key.controlstick) {
								press_action = true;
							}
						}
					}
					
					if (key.pushleft) press_left = true;
					if (key.pushright) press_right = true;
				break;
				case 1:
				  //both sides
					for (i = 0; i < key.touchPoints; i++) {
						//if (inbox(key.touchx[i], key.touchy[i], 0, 0, dwgfx.devicex, dwgfx.devicey / 4)) {
						//	press_action = true;
						//}else 
						if (insecretlab) {
							if (inbox(key.touchx[i],  key.touchy[i], 0, 0, dwgfx.buttonscreen[0].width + (dwgfx.buttonxspacing * 2), dwgfx.buttonscreen[0].height + (dwgfx.buttonyspacing * 2) )) {
								scores.opengamecenter();
								press_map = true;
							}
						}
						if (inbox(key.touchx[i],  key.touchy[i], dwgfx.devicex - dwgfx.buttonscreen[0].width - dwgfx.buttonxspacing, 0, dwgfx.buttonscreen[0].width + (dwgfx.buttonxspacing*2), dwgfx.buttonscreen[0].height + (dwgfx.buttonyspacing*2) )) {
							press_map = true;
						}else{							
							if (inbox(key.touchx[i], key.touchy[i], 0, 0, dwgfx.devicex / 2, dwgfx.devicey)) {
								press_left = true;
							}else if (inbox(key.touchx[i], key.touchy[i], dwgfx.devicex / 2, 0, dwgfx.devicex / 2, dwgfx.devicey)) {
								press_right = true;
							}
						}
					}
					
					/*
					if (last_action > 0) {
						if (!press_left || !press_right) {
							last_action = 0;
						}else {
							press_left = false;
							press_right = false;
						}
					}
					*/
					
					//if (last_action > 0) {
						if (press_left && press_right) {
							if (last_left > 0 && last_right > 0) {
								press_left = false;
								press_right = false;
								press_action = true;
								last_action = 1;
							}else if (last_left > 0 && last_right == 0) {
								press_right = false;
								press_action = true;
							}else	if (last_left == 0 && last_right > 0) {
								press_left = false;
								press_action = true;
							}
						}
					//}
					
					if (press_left) {
						last_left = 2;
					}else {
						if (last_left > 0) last_left--;
					}
					if (press_right) {
						last_right = 2;
					}else {
						if (last_right > 0) last_right--;
					}
				break;
				case 2:
					//D-Pad controls
					for (i = 0; i < key.touchPoints; i++) {
						if (insecretlab) {
							if (inbox(key.touchx[i],  key.touchy[i], 0, 0, dwgfx.buttonscreen[0].width + (dwgfx.buttonxspacing * 2), dwgfx.buttonscreen[0].height + (dwgfx.buttonyspacing * 2) )) {
								scores.opengamecenter();
								press_map = true;
							}
						}
						if (inbox(key.touchx[i],  key.touchy[i], dwgfx.devicex - dwgfx.buttonscreen[0].width - dwgfx.buttonxspacing, 0, dwgfx.buttonscreen[0].width + (dwgfx.buttonxspacing*2), dwgfx.buttonscreen[0].height + (dwgfx.buttonyspacing*2) )) {
							press_map = true;
						}else if (inbox(key.touchx[i], key.touchy[i], 0, dwgfx.buttonpos[2].y - (dwgfx.devicey*2/3), dwgfx.buttonsize + dwgfx.buttonxspacing+ (dwgfx.buttonxspacing/2), dwgfx.devicey)) {
							press_left = true;
						}else if (inbox(key.touchx[i], key.touchy[i], dwgfx.buttonpos[3].x - (dwgfx.buttonxspacing / 2), dwgfx.buttonpos[3].y -(dwgfx.devicey*2 / 3), dwgfx.buttonsize + (dwgfx.buttonxspacing * 5), dwgfx.devicey)) {
							press_right = true;
						}else if (key.touchx[i] > dwgfx.devicex / 2) {
							press_action = true;
						}
					}
				break;
			}
		}
		
		public function windowNotActive(e:Event):void{ infocus = false; }
    public function windowActive(e:Event):void{ infocus = true; }
		
		public function customstart(obj:entityclass, music:musicclass):void{
			jumpheld = true;
			
			savex = edsavex;
			savey = edsavey;
			saverx = edsaverx;
			savery = edsavery;
			
			savegc = edsavegc;
			savedir = edsavedir; //Worldmap Start
			//savex = 6 * 8; savey = 15 * 8; saverx = 46; savery = 54; savegc = 0; savedir = 1; //Final Level Current
			savepoint = 0;
			gravitycontrol = savegc;
			
			coins = 0;
			trinkets = 0;			
      crewmates = 0;
			
			//state = 2; deathseq = -1; lifeseq = 10; //Not dead, in game initilisation state
			state = 0;
			deathseq = -1;
			lifeseq = 0;
			
			//let's teleport in!
			//state = 2500;
			//if (!nocutscenes) music.play(5);
		}
		
		public function start(obj:entityclass, music:musicclass):void {
			jumpheld = true;
		  
			savex = 232; savey = 113; saverx = 104; savery = 110; savegc = 0; savedir = 1; //Worldmap Start
			//savex = 6 * 8; savey = 15 * 8; saverx = 46; savery = 54; savegc = 0; savedir = 1; //Final Level Current
		  savepoint = 0;
		  gravitycontrol = savegc;
			
		  coins = 0; trinkets = 0; 
			
		  //state = 2; deathseq = -1; lifeseq = 10; //Not dead, in game initilisation state
		  state = 0; deathseq = -1; lifeseq = 0;
			
			//let's teleport in!
			//state = 2500;
		  if (!nocutscenes) music.play(5);
		}
		
				
		public function startspecial(t:int, obj:entityclass, music:musicclass):void {
			jumpheld = true;
			
			switch(t) {
				case 0: //Secret Lab
			    savex = 104; savey = 169; saverx = 118; savery = 106; savegc = 0; savedir = 1;
				break;
				case 1: //Intermission 1 (any)
			    savex = 80; savey = 57; saverx = 41; savery = 56; savegc = 0; savedir = 0;
				break;
				default:
				  savex = 232; savey = 113; saverx = 104; savery = 110; savegc = 0; savedir = 1; //Worldmap Start
				break;
			}
		  
		  savepoint = 0;
		  gravitycontrol = savegc;
		  coins = 0; trinkets = 0; 
		  state = 0; deathseq = -1; lifeseq = 0;
		}
				
		public function starttrial(t:int, obj:entityclass, music:musicclass):void {
			jumpheld = true;
			
			switch(t) {
				case 0: //Space Station 1
			    savex = 200; savey = 161; saverx = 113; savery = 105; savegc = 0; savedir = 1;
				break;
				case 1: //Lab
			    savex = 191; savey = 33; saverx = 102; savery = 116; savegc = 0; savedir = 1;
				break;
				case 2: //Tower
			    savex = 84; savey = 193, saverx = 108; savery = 109; savegc = 0; savedir = 1;
				break;
				case 3: //Space Station 2
			    savex = 148; savey = 38; saverx = 112; savery = 114; savegc = 1; savedir = 0;
				break;
				case 4: //Warp
			    savex = 52; savey = 73; saverx = 114; savery = 101; savegc = 0; savedir = 1;
				break;
				case 5: //Final
				  savex = 101; savey = 113; saverx = 46; savery = 54; savegc = 0; savedir = 1;
				break;
				default:
				  savex = 232; savey = 113; saverx = 104; savery = 110; savegc = 0; savedir = 1; //Worldmap Start
				break;
			}
		  
		  savepoint = 0;
		  gravitycontrol = savegc;
			
		  coins = 0; trinkets = 0; 
			
		  //state = 2; deathseq = -1; lifeseq = 10; //Not dead, in game initilisation state
		  state = 0; deathseq = -1; lifeseq = 0;
		}
		
		public function starttest(obj:entityclass, music:musicclass):void {
			jumpheld = true;
		  
		  //savex = 19*8; savey = 17*8; saverx = 50; savery = 50; savegc = 0; savedir = 1; //Generic Start
			
			//savex = 19*8; savey = 18*8; saverx = 114; savery = 101; savegc = 0; savedir = 1; //Warp Zone Start
			//savex = 20*8; savey = 18*8; saverx = 113; savery = 100; savegc = 0; savedir = 1; //Warp Zone current
			
			//savex = 6 * 8; savey = 6 * 8; saverx = 113; savery = 105; savegc = 0; savedir = 1; //Space Station Start
			//savex = 6 * 8; savey = 6 * 8; saverx = 50; savery = 42; savegc = 0; savedir = 1; //Space Station Reversal
			//savex = 16 * 8; savey = 10 * 8; saverx = 116; savery = 107; savegc = 0; savedir = 1; //Space Station Tele
			
			//savex = 5 * 8; savey = 5 * 8; saverx = 112; savery = 114; savegc = 1; savedir = 1; //Space Station 2 Start
			//savex = 22*8; savey = 10*8; saverx = 113; savery = 108; savegc = 0; savedir = 1; //Space Station 2 Tele1
			//savex = 9*8; savey = 13*8; saverx = 118; savery = 108; savegc = 0; savedir = 1; //Space Station 2 s.driller
			//savex = 10*8; savey = 22*8; saverx = 111; savery = 107; savegc = 0; savedir = 1; //Space Station 2 ending
			//savex = 24*8; savey = 26*8; saverx = 53; savery = 52; savegc = 0; savedir = 1; //Actual start
			//savex = 5 * 8; savey = 5 * 8; saverx = 117; savery = 108; savegc = 0; savedir = 1; //Space Station 2 wip
			
      //savex = 18 * 8; savey = 24 * 8; saverx = 100; savery = 116; savegc = 0; savedir = 1; //Lab outside, start
			//savex = 17 * 8; savey = 24 * 8; saverx = 107; savery = 115; savegc = 1; savedir = 1; //Lab tele
			//savex = 31 * 8; savey = 12 * 8; saverx = 49; savery = 54; savegc = 1; savedir = 1; //Lab, final bit
			//savex = 19 * 8; savey = 14 * 8; saverx = 104; savery = 102; savegc = 0; savedir = 1; //Lab, ending
			
			//savex = 20; savey = 680*8; saverx = 109; savery = 101; savegc = 0; savedir = 1; //The tower
		  //savex = 6 * 8; savey = 6 * 8; saverx = 108; savery = 109; savegc = 0; savedir = 1; //Area just before tower
		  //savex = 6 * 8; savey = 6 * 8; saverx = 110; savery = 104; savegc = 0; savedir = 1; //Area after tower
		  //savex = 70; savey = 129; saverx = 1; savery = 1; savegc = 0; savedir = 1; //The testbed
			
			//savex = 14 * 8; savey = 15 * 8; saverx = 102; savery = 111; savegc = 0; savedir = 1; //Worldmap Start
			//savex = 19 * 8; savey = 13 * 8; saverx = 114; savery = 117; savegc = 0; savedir = 1; //Warp zone entrance
			//savex = 19 * 8; savey = 13 * 8; saverx = 110; savery = 109; savegc = 0; savedir = 1; //The Elephant
			
			//savex = 16 * 8; savey = 15 * 8; saverx = 46; savery = 54; savegc = 0; savedir = 1; //Final Level Start
			//savex = 5 * 8; savey = 15 * 8; saverx = 41; savery = 52; savegc = 0; savedir = 1; //Final Level Midpoint
			//savex = (5-3) * 8; savey = (5+2) * 8; saverx = 52-2; savery = 53; savegc = 0; savedir = 1; //Final Level End
			
			//savex = 6 * 8; savey = 6 * 8; saverx = 117; savery = 107; savegc = 0; savedir = 1; //Space Station Start
			
			savex = 13 * 8; savey = 129; saverx = 102; savery = 111; savegc = 0; savedir = 1; //Worldmap Start **********
			//savex = 13 * 8; savey = (19 * 8)-22; saverx = 107; savery = 108; savegc = 0; savedir = 1; //Worldmap Start
			
			//savex = 228; savey = 129; saverx = 53; savery = 49; savegc = 0; savedir = 0; //Intermission level 2
			//savex = 80; savey = 57; saverx = 41; savery = 56; savegc = 0; savedir = 0; //Intermission level 1
			//savex = 215; savey = 33; saverx = 52-1; savery = 56; savegc = 0; savedir = 0; //Intermission level 1
			
			//savex = 104; savey = 169; saverx = 118; savery = 106; savegc = 0; savedir = 1; //Secret Lab
			//savex = 132; savey = 137; saverx = 119; savery = 107; savegc = 0; savedir = 1; //Secret Lab (beside gravitron)
			
			savepoint = 0;
		  gravitycontrol = savegc;
		  
		  coins = 0; trinkets = 0; 
			
		  state = 0; deathseq = -1; lifeseq = 10;
		
		  //music.play(1);
		}
		
		//Menu stuff first!
		public function createmenu(t:String):void {
			var tcount:int, i:int, tvar:int, j:int;
			for (i = 0; i < 25; i++) menuoptionslayout[i] = 0;
			
			currentmenuoption = 0; menuselection = "null"; currentmenuname = t;
			menuxoff = 0; menuyoff = 0;
			menucountdown = 0; menudest="null";
			
			if (mobilemenu) {
				if (t == "mainmenu") {
					showloadingnotice = false;
					menuoptions[0] = "play";
					menuoptionsactive[0] = true;
					menuoptionslayout[0] = 1;
					menuoptions[1] = "player worlds";
					menuoptionsactive[1] = true;
					menuoptionslayout[1] = 2;
					/*menuoptions[2] = "super gravitron";
					menuoptionsactive[2] = true;
					menuoptionslayout[2] = 2;
					*/
					menuoptions[2] = "options";
					menuoptionsactive[2] = true;
					menuoptionslayout[2] = 7;
					menuoptions[3] = "credits";
					menuoptionsactive[3] = true;
					menuoptionslayout[3] = 8;
					menuoptions[4] = "more games";
					menuoptionsactive[4] = true;
					menuoptionslayout[4] = 12;
					nummenuoptions = 5;
					menuxoff = -16;
					menuyoff = -50+20;
				}else if (t == "playerworlds"){
					menuoptions[0] = "play a level";
					menuoptionsactive[0] = true;
					menuoptions[1] = "level editor";
					menuoptionsactive[1] = true;
					menuoptions[2] = "< back";          menuoptionsactive[2] = true;
					menuoptionslayout[2] = 3;
					nummenuoptions = 3;
					menuxoff = -30;
					menuyoff = -40-(12*2);
				}else if (t == "quickloadlevel") {
					menuoptions[0] = "continue from save";
					menuoptionsactive[0] = true;
					menuoptionslayout[0] = 4;
					menuoptions[1] = "start from beginning";
					menuoptionsactive[1] = true;
					menuoptionslayout[1] = 4;
					menuoptions[2] = "< back";          menuoptionsactive[2] = true;
					menuoptionslayout[2] = 3;
					nummenuoptions = 3;
					menuxoff = -40;
					menuyoff = -30-(12*2);
				}else if (t == "youwannaquit") {
					menuoptions[0] = "yes, quit";     menuoptionsactive[0] = true;
					menuoptions[1] = "no, return";  menuoptionsactive[1] = true;
					nummenuoptions = 2;
					menuxoff = 0;	menuyoff = -20-(12*2);
				}else if (t == "levellist"){
					if (editor.ListOfMetaData.length == 0) {
						menuoptions[0] = "ok";
						menuoptionsactive[0] = true;
						nummenuoptions = 1;
						menuxoff = 0;
						menuyoff = -20;
					}else{
						tcount = 0;
						for (i = 0; i < editor.ListOfMetaData.length; i++) {
							if (i >= levelpage * 3 && i < (levelpage * 3) + 3) {
								//This is, er, suboptimal. Whatever, life optimisation and all that
								tvar = -1;
								for (j = 0; j < numcustomlevelstats; j++) {
									if (editor.ListOfMetaData[i].filename == customlevelstats[j]) {
										tvar = customlevelscore[j];
										j=numcustomlevelstats+1;
									}
								}
								/*
								if(tvar>=0){
									if(customlevelscore[tvar]==0){
										menuoptions[tcount] = "   " + editor.ListOfMetaData[i].title;
									}else if(customlevelscore[tvar]==1){
										menuoptions[tcount] = " * " + editor.ListOfMetaData[i].title;
									}else if(customlevelscore[tvar]==3){
										menuoptions[tcount] = "** " + editor.ListOfMetaData[i].title;
									}
								}else{
									menuoptions[tcount] = "   " + editor.ListOfMetaData[i].title;
								}
								*/
								menuoptions[tcount] = String(i);
								menuoptions_alt[tcount] = String(tvar);
								menuoptionsactive[tcount] = true;
								menuoptionslayout[tcount] = 11;
								menuoptions[tcount] = menuoptions[tcount].toLowerCase();
								tcount++;
							}
						}
						if((levelpage*3)+3<editor.ListOfMetaData.length){
							menuoptions[tcount] = "next page";
							menuoptionsactive[tcount] = true;
							menuoptionslayout[tcount] = 10;
							tcount++;
						}else{
							menuoptions[tcount] = "first page";
							menuoptionsactive[tcount] = true;
							menuoptionslayout[tcount] = 10;
							tcount++;
						}
						
						menuoptions[tcount] = "< back";          menuoptionsactive[tcount] = true;
						menuoptionslayout[tcount] = 3;
						tcount++;
						
						nummenuoptions = tcount;
						menuxoff = -90;
						menuyoff = -120;
					}
				}else if (t == "graphicoptions") {
					if (advanced_mode) {
						if(fullscreen){
							menuoptions[0] = "change to windowed mode";      menuoptionsactive[0] = true;
						}else {
							menuoptions[0] = "change to fullscreen";      menuoptionsactive[0] = true;
						}
						menuoptions[1] = "enable acceleration";      menuoptionsactive[1] = true;
						menuoptions[2] = "change scaling mode";      menuoptionsactive[2] = true;
						if (advanced_smoothing) {
							menuoptions[3] = "disable smoothing";      menuoptionsactive[3] = true;					
						}else{
							menuoptions[3] = "enable smoothing";      menuoptionsactive[3] = true;					
						}
						menuoptions[4] = "< back";          menuoptionsactive[4] = true;
						menuoptionslayout[4] = 3;
						nummenuoptions = 5;
						menuxoff = -50;	menuyoff = 16-(12*4);
					}else{
						if(fullscreen){
							menuoptions[0] = "change to windowed mode";      menuoptionsactive[0] = true;
						}else {
							menuoptions[0] = "change to fullscreen";      menuoptionsactive[0] = true;
						}
						menuoptions[1] = "disable acceleration";      menuoptionsactive[1] = true;
						menuoptions[2] = "< back";          menuoptionsactive[2] = true;
						menuoptionslayout[2] = 3;
						nummenuoptions = 3;
						menuxoff = -50;	menuyoff = 16-(12*2);
					}
				}else if (t == "options") {
					menuoptions[0] = "accessibility options";      menuoptionsactive[0] = true;
					menuoptionslayout[0] = 4;
					menuoptions[1] = "unlock play modes";  				 menuoptionsactive[1] = true;
					menuoptionslayout[1] = 4;
					menuoptions[2] = "clear data";            menuoptionsactive[2] = true;
					menuoptionslayout[2] = 4;
					menuoptions[3] = "< back";          menuoptionsactive[3] = true;
					menuoptionslayout[3] = 3;
					nummenuoptions = 4;
					menuxoff = -40;	menuyoff = -30-(12*3);
				}else if (t == "accessibility") {
					menuoptions[0] = "animated backgrounds";      menuoptionsactive[0] = true;
					menuoptionslayout[0] = 4;
					menuoptions[1] = "screen effects";     		 		menuoptionsactive[1] = true;
					menuoptionslayout[1] = 4;
					//menuoptions[2] = "invincibility";  						menuoptionsactive[2] = true;
					//menuoptions[3] = "slowdown";            			menuoptionsactive[3] = true;
					menuoptions[2] = "< back";          menuoptionsactive[2] = true;
					menuoptionslayout[2] = 3;
					nummenuoptions = 3;
					menuxoff = -40;	menuyoff = 16-(12*2);
				}else if (t == "cleardatamenu") {
					menuoptions[0] = "no! don't delete";      menuoptionsactive[0] = true;
					menuoptionslayout[0] = 4;
					menuoptions[1] = "delete everything";  				 menuoptionsactive[1] = true;
					menuoptionslayout[1] = 4;
					nummenuoptions = 2;
					menuxoff = -30;	menuyoff = 32-(12*2)-32;
				}else if (t == "setinvincibility") {
					menuoptions[0] = "no, return to options";      menuoptionsactive[0] = true;
					menuoptions[1] = "yes, delete saves";  				 menuoptionsactive[1] = true;
					nummenuoptions = 2;
					menuxoff = -30;	menuyoff = 64-(12*2);
				}else if (t == "setslowdown1") {
					menuoptions[0] = "no, return to options";      menuoptionsactive[0] = true;
					menuoptions[1] = "yes, delete saves";  				 menuoptionsactive[1] = true;
					nummenuoptions = 2;
					menuxoff = -30;	menuyoff = 64-(12*2);
				}else if (t == "setslowdown2") {
					menuoptions[0] = "normal speed";      menuoptionsactive[0] = true;
					menuoptions[1] = "80% speed";     		 		menuoptionsactive[1] = true;
					menuoptions[2] = "60% speed";  						menuoptionsactive[2] = true;
					menuoptions[3] = "40% speed";            			menuoptionsactive[3] = true;
					nummenuoptions = 4;
					menuxoff = -40;	menuyoff = 16-(12*3);
				}else if (t == "unlockmenu") {
					menuoptions[0] = "unlock time trials";           menuoptionsactive[0] = true; menuoptionslayout[0] = 9;
					menuoptions[1] = "unlock intermissions";         menuoptionsactive[1] = !unlock[16]; menuoptionslayout[1] = 9;
					menuoptions[2] = "unlock no death mode";  			 menuoptionsactive[2] = !unlock[17];menuoptionslayout[2] = 9;
					menuoptions[3] = "unlock flip mode";             menuoptionsactive[3] = !unlock[18];menuoptionslayout[3] = 9;
					menuoptions[4] = "unlock ship jukebox";          menuoptionsactive[4] = (stat_trinkets<20);menuoptionslayout[4] = 9;
					menuoptions[5] = "unlock secret lab";            menuoptionsactive[5] = !unlock[8];menuoptionslayout[5] = 9;
					
					menuoptions_alt[0] = "time trials unlocked";
					menuoptions_alt[1] = "intermissions unlocked";
					menuoptions_alt[2] = "no death mode unlocked";
					menuoptions_alt[3] = "flip mode unlocked";
					menuoptions_alt[4] = "ship jukebox unlocked";
					menuoptions_alt[5] = "secret lab unlocked";
					
					menuoptions[6] = "< back";          menuoptionsactive[6] = true;
					menuoptionslayout[6] = 3;
					
					nummenuoptions = 7;
					menuxoff = -70;	menuyoff = -55-(12*6);
				}else if (t == "credits") {
					menuoptions[0] = "next page";      menuoptionsactive[0] = true;
					menuoptions[1] = "< back";          menuoptionsactive[1] = true;
					menuoptionslayout[1] = 3;
					nummenuoptions = 2;
					menuxoff = 20;	menuyoff = 64;
				}else if (t == "credits2") {
					menuoptions[0] = "next page";      menuoptionsactive[0] = true;
					menuoptions[1] = "< back";          menuoptionsactive[1] = true;
					menuoptionslayout[1] = 3;
					nummenuoptions = 2;
					menuxoff = 20;	menuyoff = 64;
				}else if (t == "credits3") {
					menuoptions[0] = "next page";      menuoptionsactive[0] = true;
					menuoptions[1] = "< back";          menuoptionsactive[1] = true;
					menuoptionslayout[1] = 3;
					nummenuoptions = 2;
					menuxoff = 20;	menuyoff = 64;
				}else if (t == "credits4") {
					menuoptions[0] = "next page";      menuoptionsactive[0] = true;
					menuoptions[1] = "< back";          menuoptionsactive[1] = true;
					menuoptionslayout[1] = 3;
					nummenuoptions = 2;
					menuxoff = 20;	menuyoff = 64;
				}else if (t == "credits5") {
					menuoptions[0] = "next page";      menuoptionsactive[0] = true;
					menuoptions[1] = "< back";          menuoptionsactive[1] = true;
					menuoptionslayout[1] = 3;
					nummenuoptions = 2;
					menuxoff = 20;	menuyoff = 64;
				}else if (t == "credits6") {
					menuoptions[0] = "next page";      menuoptionsactive[0] = true;
					menuoptions[1] = "< back";          menuoptionsactive[1] = true;
					menuoptionslayout[1] = 3;
					nummenuoptions = 2;
					menuxoff = 20;	menuyoff = 64;
				}else if (t == "credits7") {
					menuoptions[0] = "next page";      menuoptionsactive[0] = true;
					menuoptions[1] = "< back";          menuoptionsactive[1] = true;
					menuoptionslayout[1] = 3;
					nummenuoptions = 2;
					menuxoff = 20;	menuyoff = 64;
				}else if (t == "credits8") {
					menuoptions[0] = "next page";      menuoptionsactive[0] = true;
					menuoptions[1] = "< back";          menuoptionsactive[1] = true;
					menuoptionslayout[1] = 3;
					nummenuoptions = 2;
					menuxoff = 20;	menuyoff = 64;
				}else if (t == "credits9") {
					menuoptions[0] = "first page";      menuoptionsactive[0] = true;
					menuoptions[1] = "< back";          menuoptionsactive[1] = true;
					menuoptionslayout[1] = 3;
					nummenuoptions = 2;
					menuxoff = 20;	menuyoff = 64;
				}else if (t == "play") {
					//Ok, here's where the unlock stuff comes into it:
					//First up, time trials:
					temp = 0;
					if (unlock[0] && stat_trinkets >= 3 && !unlocknotify[9]) temp++;
					if (unlock[1] && stat_trinkets >= 6 && !unlocknotify[10]) temp++;
					if (unlock[2] && stat_trinkets >= 9 && !unlocknotify[11]) temp++;
					if (unlock[3] && stat_trinkets >= 12 && !unlocknotify[12]) temp++;
					if (unlock[4] && stat_trinkets >= 15 && !unlocknotify[13]) temp++;
					if (unlock[5] && stat_trinkets >= 18 && !unlocknotify[14]) temp++;
					if (temp > 0) {
						//you've unlocked a time trial!					
						if (unlock[0] && stat_trinkets >= 3) { unlocknotify[9] = true; unlock[9] = true; }
						if (unlock[1] && stat_trinkets >= 6) { unlocknotify[10] = true; unlock[10] = true; }
						if (unlock[2] && stat_trinkets >= 9) { unlocknotify[11] = true; unlock[11] = true; }					
						if (unlock[3] && stat_trinkets >= 12) { unlocknotify[12] = true; unlock[12] = true; }					
						if (unlock[4] && stat_trinkets >= 15) { unlocknotify[13] = true; unlock[13] = true; }					
						if (unlock[5] && stat_trinkets >= 18) { unlocknotify[14] = true; unlock[14] = true; }
						
						if (temp == 1) {
							createmenu("unlocktimetrial"); savemystats = true;
						}else if (temp > 1) {
							createmenu("unlocktimetrials"); savemystats = true;
						}
					}else {
						//Alright, we haven't unlocked any time trials. How about no death mode?
						temp = 0;
						if (bestrank[0] >= 2) temp++;
						if (bestrank[1] >= 2) temp++;
						if (bestrank[2] >= 2) temp++;
						if (bestrank[3] >= 2) temp++;
						if (bestrank[4] >= 2) temp++;
						if (bestrank[5] >= 2) temp++;
						if (temp >= 4 && !unlocknotify[17]) {
							//Unlock No Death Mode
							unlocknotify[17] = true; unlock[17] = true;
							createmenu("unlocknodeathmode");
							savemystats = true;
						}else{
							//Alright then! Flip mode?
							if (unlock[5] && !unlocknotify[18]) {
								unlock[18] = true; unlocknotify[18] = true;
								createmenu("unlockflipmode");
								savemystats = true;
							}else {
								//What about the intermission levels?
								if (unlock[7] && !unlocknotify[16]) {
									unlock[16] = true; unlocknotify[16] = true;
									createmenu("unlockintermission");
									savemystats = true;
								}else {
									//ok, secret lab! no notification, but test:
									if (unlock[8]) {
										createmenu("playsecretlab");
									}else{
										menuoptions[0] = "continue";      menuoptionsactive[0] = true;
										menuoptions[1] = "play modes";  	menuoptionsactive[1] = true;
										menuoptions[2] = "new game";      menuoptionsactive[2] = true;
										menuoptions[3] = "< back";          menuoptionsactive[3] = true;
										menuoptionslayout[3] = 3;
										nummenuoptions = 4;
										menuxoff = -20;	menuyoff = -30 -(12*3);
									}
								}
							}
						}
					}
				}else if (t == "unlocktimetrial" || t == "unlocktimetrials") {
					menuoptions[0] = "continue";          menuoptionsactive[0] = true;
					nummenuoptions = 1;
					menuxoff = 20;	menuyoff = 70;
				}else if (t == "unlocknodeathmode") {
					menuoptions[0] = "continue";          menuoptionsactive[0] = true;
					nummenuoptions = 1;
					menuxoff = 20;	menuyoff = 70;
				}else if (t == "unlockintermission") {
					menuoptions[0] = "continue";          menuoptionsactive[0] = true;
					nummenuoptions = 1;
					menuxoff = 20;	menuyoff = 70;
				}else if (t == "unlockflipmode") {
					menuoptions[0] = "continue";          menuoptionsactive[0] = true;
					nummenuoptions = 1;
					menuxoff = 20;	menuyoff = 70;
				}else if (t == "playsecretlab") {
					menuoptions[0] = "continue";      menuoptionsactive[0] = true;
					menuoptions[1] = "secret lab";  	menuoptionsactive[1] = true;
					menuoptions[2] = "play modes";  	menuoptionsactive[2] = true;
					menuoptions[3] = "new game";      menuoptionsactive[3] = true;
					menuoptions[4] = "< back";          menuoptionsactive[4] = true;
					menuoptionslayout[4] = 3;
					nummenuoptions = 5;
					menuxoff = -40;	menuyoff = -35-(12*4);
				}else if (t == "newgamewarning") {
					menuoptions[0] = "start new game";      menuoptionsactive[0] = true;
					menuoptions[1] = "< back";  				 menuoptionsactive[1] = true;
					menuoptionslayout[1] = 3;
					nummenuoptions = 2;
					menuxoff = -30;	menuyoff = 64-(16*2)-32;
				}else if (t == "flipmode_mobile") {
					menuoptions[0] = "change flip mode";      menuoptionsactive[0] = true;
					menuoptions[1] = "< back";  				 menuoptionsactive[1] = true;
					menuoptionslayout[1] = 3;
					nummenuoptions = 2;
					menuxoff = -30;	menuyoff = 64-(16*2);
				}else if (t == "playmodes") {
					menuoptions[0] = "time trials";    menuoptionsactive[0] = true;
					menuoptions[1] = "intermissions";      menuoptionsactive[1] = unlock[16];
					menuoptions[2] = "no death mode";  	menuoptionsactive[2] = unlock[17];
					menuoptions[3] = "flip mode";      menuoptionsactive[3] = unlock[18];
					menuoptions[4] = "< back";      menuoptionsactive[4] = true;
					menuoptionslayout[4] = 3;
					nummenuoptions = 5;
					menuxoff = -70;	menuyoff = -35-(12*4);
				}else if (t == "intermissionmenu") {
					menuoptions[0] = "play intermission 1";      menuoptionsactive[0] = true;
					menuoptions[1] = "play intermission 2";  	menuoptionsactive[1] = true;
					menuoptions[2] = "< back";          menuoptionsactive[2] = true;
					menuoptionslayout[0] = 4; menuoptionslayout[1] = 4;
					menuoptionslayout[2] = 3;
					nummenuoptions = 3;
					menuxoff = -50;	menuyoff = -35-(12*2);
				}else if (t == "playint1") {
					menuoptions[0] = "Vitellary";      menuoptionsactive[0] = true;
					menuoptions[1] = "Vermilion";  	menuoptionsactive[1] = true;
					menuoptions[2] = "Verdigris";  	menuoptionsactive[2] = true;
					menuoptions[3] = "Victoria";  	menuoptionsactive[3] = true;
					menuoptions[4] = "< back";          menuoptionsactive[4] = true;
					menuoptionslayout[4] = 3;
					nummenuoptions = 5;
					menuxoff = -60;	menuyoff = -30-(12*4);
				}else if (t == "playint2") {
					menuoptions[0] = "Vitellary";      menuoptionsactive[0] = true;
					menuoptions[1] = "Vermilion";  	menuoptionsactive[1] = true;
					menuoptions[2] = "Verdigris";  	menuoptionsactive[2] = true;
					menuoptions[3] = "Victoria";  	menuoptionsactive[3] = true;
					menuoptions[4] = "< back";          menuoptionsactive[4] = true;
					menuoptionslayout[4] = 3;
					nummenuoptions = 5;
					menuxoff = -60;	menuyoff = -30-(12*4);
				}else if (t == "continue") {
					menuoptions[0] = "continue from teleporter";      menuoptionsactive[0] = true;
					menuoptions[1] = "continue from quicksave";  	menuoptionsactive[1] = true;
					menuoptionslayout[0] = 4; menuoptionslayout[1] = 4;
					menuoptions[2] = "< back";          menuoptionsactive[2] = true;
					menuoptionslayout[2] = 3;
					nummenuoptions = 3;
					menuxoff = -60;	menuyoff = 20-(12*2);
				}else if (t == "startnodeathmode") {
					menuoptions[0] = "disable cutscenes";      menuoptionsactive[0] = true;
					menuoptions[1] = "enable cutscenes";  	menuoptionsactive[1] = true;
					menuoptionslayout[0] = 4; menuoptionslayout[1] = 4;
					menuoptions[2] = "< back";          menuoptionsactive[2] = true;
					menuoptionslayout[2] = 3;
					nummenuoptions = 3;
					menuxoff = -60;	menuyoff = 30-(12*2);
				}else if (t == "gameover") {
					nummenuoptions = 0;
					menucountdown = 120; menudest="gameover2";
				}else if (t == "gameover2") {
					menuoptions[0] = "return to play menu";          menuoptionsactive[0] = true;
					menuoptionslayout[0] = 4;
					nummenuoptions = 1;
					menuxoff = -25;	menuyoff = 80-8;
				}else if (t == "unlockmenutrials") {
					if (unlock[9]) {  menuoptions[0] = "space station 1";                      menuoptionsactive[0] = false;
					}else{            menuoptions[0] = "space station 1";      menuoptionsactive[0] = true;}
					if (unlock[10]) {  menuoptions[1] = "the laboratory";                     menuoptionsactive[1] = false;
					}else{            menuoptions[1] = "the laboratory";            menuoptionsactive[1] = true;}
					if (unlock[11]) {  menuoptions[2] = "the tower";                     menuoptionsactive[2] = false;
					}else{            menuoptions[2] = "the tower";                menuoptionsactive[2] = true;}
					if (unlock[12]) {  menuoptions[3] = "space station 2";                     menuoptionsactive[3] = false;
					}else{            menuoptions[3] = "space station 2";            menuoptionsactive[3] = true;}
					if (unlock[13]) {  menuoptions[4] = "the warp zone";                     menuoptionsactive[4] = false;
					}else{            menuoptions[4] = "the warp zone";            menuoptionsactive[4] = true;}
					if (unlock[14]) {  menuoptions[5] = "the final level";                     menuoptionsactive[5] = false;
					}else{            menuoptions[5] = "the final level";          menuoptionsactive[5] = true;}
					
					menuoptions_alt[0] = "space station 1 unlocked";
					menuoptions_alt[1] = "the laboratory unlocked";
					menuoptions_alt[2] = "the tower unlocked";
					menuoptions_alt[3] = "space station 2 unlocked";
					menuoptions_alt[4] = "the warp zone unlocked";
					menuoptions_alt[5] = "the final level unlocked";
					
					menuoptionslayout[0] = 9;
					menuoptionslayout[1] = 9;
					menuoptionslayout[2] = 9;
					menuoptionslayout[3] = 9;
					menuoptionslayout[4] = 9;
					menuoptionslayout[5] = 9;
					
					menuoptions[6] = "< back";          menuoptionsactive[6] = true;
					menuoptionslayout[6] = 3;
					nummenuoptions = 7;
					menuxoff = -80;	menuyoff = -55-(12*6);
				}else if (t == "timetrials") {
					if (!unlock[9]) {  menuoptions[0] = "???";                      menuoptionsactive[0] = false;
					}else{            menuoptions[0] = "space station 1";      menuoptionsactive[0] = true;}
					if (!unlock[10]) {  menuoptions[1] = "???";                     menuoptionsactive[1] = false;
					}else{            menuoptions[1] = "the laboratory";            menuoptionsactive[1] = true;}
					if (!unlock[11]) {  menuoptions[2] = "???";                     menuoptionsactive[2] = false;
					}else{            menuoptions[2] = "the tower";                menuoptionsactive[2] = true;}
					if (!unlock[12]) {  menuoptions[3] = "???";                     menuoptionsactive[3] = false;
					}else{            menuoptions[3] = "space station 2";            menuoptionsactive[3] = true;}
					if (!unlock[13]) {  menuoptions[4] = "???";                     menuoptionsactive[4] = false;
					}else{            menuoptions[4] = "the warp zone";            menuoptionsactive[4] = true;}
					if (!unlock[14]) {  menuoptions[5] = "???";                     menuoptionsactive[5] = false;
					}else{            menuoptions[5] = "the final level";          menuoptionsactive[5] = true;}
					menuoptionslayout[0] = 5;
					menuoptionslayout[1] = 5;
					menuoptionslayout[2] = 5;
					menuoptionslayout[3] = 6;
					menuoptionslayout[4] = 6;
					menuoptionslayout[5] = 6;
					
					menuoptions[6] = "< back";          menuoptionsactive[6] = true;
					menuoptionslayout[6] = 3;
					nummenuoptions = 7;
					menuxoff = -80;	menuyoff = -100;
				}else if (t == "nodeathmodecomplete") {
					nummenuoptions = 0;
					menucountdown = 90; menudest = "nodeathmodecomplete2";
				}else if (t == "nodeathmodecomplete2") {
					menuoptions[0] = "return to play menu";          menuoptionsactive[0] = true;
					menuoptionslayout[0] = 4;
					nummenuoptions = 1;
					menuxoff = -25;	menuyoff = 70-8;
				}else if (t == "timetrialcomplete") {
					nummenuoptions = 0;
					menucountdown = 90; menudest="timetrialcomplete2";
				}else if (t == "timetrialcomplete2") {
					nummenuoptions = 0;
					menucountdown = 60; menudest="timetrialcomplete3";
				}else if (t == "timetrialcomplete3") {
					menuoptions[0] = "try again";  	menuoptionsactive[0] = true;
					menuoptions[1] = "< back";      menuoptionsactive[1] = true;
					menuoptionslayout[1] = 3;
					nummenuoptions = 2;
					menuxoff = -25;	menuyoff = 70;
				}else if (t == "gamecompletecontinue") {
					menuoptions[0] = "return to play menu";          menuoptionsactive[0] = true;
					menuoptionslayout[0] = 4;
					nummenuoptions = 1;
					menuxoff = -25;	menuyoff = 70-8;
				}
			}else{
				if (t == "mainmenu") {
					menuoptions[0] = "start game";
					menuoptionsactive[0] = true;
					menuoptions[1] = "player levels";
					menuoptionsactive[1] = true;
					menuoptions[2] = "graphic options";
					menuoptionsactive[2] = true;
					menuoptions[3] = "game options";
					menuoptionsactive[3] = true;
					menuoptions[4] = "view credits";
					menuoptionsactive[4] = true;
					menuoptions[5] = "quit game";
					menuoptionsactive[5] = true;
					nummenuoptions = 6;
					menuxoff = -16;
					menuyoff = -10;
				}else if (t == "playerworlds"){
					menuoptions[0] = "play a level";
					menuoptionsactive[0] = true;
					menuoptions[1] = "level editor";
					menuoptionsactive[1] = true;
					menuoptions[2] = "back to menu";
					menuoptionsactive[3] = true;
					nummenuoptions = 3;
					menuxoff = -30;
					menuyoff = -40;
				}else if (t == "quickloadlevel") {
					menuoptions[0] = "continue from save";
					menuoptionsactive[0] = true;
					menuoptions[1] = "start from beginning";
					menuoptionsactive[1] = true;
					menuoptions[2] = "back to levels";
					menuoptionsactive[2] = true;
					nummenuoptions = 3;
					menuxoff = -40;
					menuyoff = -30;
				}else if (t == "youwannaquit") {
					menuoptions[0] = "yes, quit";     menuoptionsactive[0] = true;
					menuoptions[1] = "no, return";  menuoptionsactive[1] = true;
					nummenuoptions = 2;
					menuxoff = 0;	menuyoff = -20;
				}else if (t == "levellist"){
					if (editor.ListOfMetaData.length == 0) {
						menuoptions[0] = "ok";
						menuoptionsactive[0] = true;
						nummenuoptions = 1;
						menuxoff = 0;
						menuyoff = -20;
					}else{
					  tcount = 0;
						for (i = 0; i < editor.ListOfMetaData.length; i++) {
							if (i >= levelpage * 8 && i < (levelpage * 8) + 8) {
								//This is, er, suboptimal. Whatever, life optimisation and all that
								tvar = -1;
								for (j = 0; j < numcustomlevelstats; j++) {
									if (editor.ListOfMetaData[i].filename == customlevelstats[j]) {
										tvar=j;
										j=numcustomlevelstats+1;
									}
								}
								if(tvar>=0){
									if(customlevelscore[tvar]==0){
										menuoptions[tcount] = "   " + editor.ListOfMetaData[i].title;
									}else if(customlevelscore[tvar]==1){
										menuoptions[tcount] = " * " + editor.ListOfMetaData[i].title;
									}else if(customlevelscore[tvar]==3){
										menuoptions[tcount] = "** " + editor.ListOfMetaData[i].title;
									}
								}else{
									menuoptions[tcount] = "   " + editor.ListOfMetaData[i].title;
								}
								menuoptionsactive[tcount] = true;
								menuoptions[tcount] = menuoptions[tcount].toLowerCase();
								tcount++;
							}
						}
						if((levelpage*8)+8<editor.ListOfMetaData.length){
							menuoptions[tcount] = "next page";
							menuoptionsactive[tcount] = true;
							tcount++;
						}else{
							menuoptions[tcount] = "first page";
							menuoptionsactive[tcount] = true;
							tcount++;
						}
						menuoptions[tcount] = "return to menu";
						menuoptionsactive[tcount] = true;
						tcount++;
						
						nummenuoptions = tcount;
						menuxoff = -90;
						menuyoff = 70-(tcount*10);
					}
				}else if (t == "graphicoptions") {
					if (advanced_mode) {
						if(fullscreen){
							menuoptions[0] = "change to windowed mode";      menuoptionsactive[0] = true;
						}else {
							menuoptions[0] = "change to fullscreen";      menuoptionsactive[0] = true;
						}
						menuoptions[1] = "enable acceleration";      menuoptionsactive[1] = true;
						menuoptions[2] = "change scaling mode";      menuoptionsactive[2] = true;
						if (advanced_smoothing) {
							menuoptions[3] = "disable smoothing";      menuoptionsactive[3] = true;					
						}else{
							menuoptions[3] = "enable smoothing";      menuoptionsactive[3] = true;					
						}
						menuoptions[4] = "return";                    menuoptionsactive[4] = true;
						nummenuoptions = 5;
						menuxoff = -50;	menuyoff = 16;
					}else{
						if(fullscreen){
							menuoptions[0] = "change to windowed mode";      menuoptionsactive[0] = true;
						}else {
							menuoptions[0] = "change to fullscreen";      menuoptionsactive[0] = true;
						}
						menuoptions[1] = "disable acceleration";      menuoptionsactive[1] = true;
						menuoptions[2] = "return";                    menuoptionsactive[2] = true;
						nummenuoptions = 3;
						menuxoff = -50;	menuyoff = 16;
					}
				}else if (t == "options") {
					menuoptions[0] = "accessibility options";      menuoptionsactive[0] = true;
					menuoptions[1] = "unlock play modes";  				 menuoptionsactive[1] = true;
					menuoptions[2] = "clear data";            menuoptionsactive[2] = true;
					menuoptions[3] = "return";                       menuoptionsactive[3] = true;
					nummenuoptions = 4;
					menuxoff = -40;	menuyoff = 0;
				}else if (t == "accessibility") {
					menuoptions[0] = "animated backgrounds";      menuoptionsactive[0] = true;
					menuoptions[1] = "screen effects";     		 		menuoptionsactive[1] = true;
					//menuoptions[2] = "invincibility";  						menuoptionsactive[2] = true;
					//menuoptions[3] = "slowdown";            			menuoptionsactive[3] = true;
					menuoptions[2] = "return";                    menuoptionsactive[2] = true;
					nummenuoptions = 3;
					menuxoff = -40;	menuyoff = 16;
				}else if (t == "cleardatamenu") {
					menuoptions[0] = "no! don't delete";      menuoptionsactive[0] = true;
					menuoptions[1] = "yes, delete everything";  				 menuoptionsactive[1] = true;
					nummenuoptions = 2;
					menuxoff = -30;	menuyoff = 64;
				}else if (t == "setinvincibility") {
					menuoptions[0] = "no, return to options";      menuoptionsactive[0] = true;
					menuoptions[1] = "yes, delete saves";  				 menuoptionsactive[1] = true;
					nummenuoptions = 2;
					menuxoff = -30;	menuyoff = 64;
				}else if (t == "setslowdown1") {
					menuoptions[0] = "no, return to options";      menuoptionsactive[0] = true;
					menuoptions[1] = "yes, delete saves";  				 menuoptionsactive[1] = true;
					nummenuoptions = 2;
					menuxoff = -30;	menuyoff = 64;
				}else if (t == "setslowdown2") {
					menuoptions[0] = "normal speed";      menuoptionsactive[0] = true;
					menuoptions[1] = "80% speed";     		 		menuoptionsactive[1] = true;
					menuoptions[2] = "60% speed";  						menuoptionsactive[2] = true;
					menuoptions[3] = "40% speed";            			menuoptionsactive[3] = true;
					nummenuoptions = 4;
					menuxoff = -40;	menuyoff = 16;
				}else if (t == "unlockmenu") {
					menuoptions[0] = "unlock time trials";           menuoptionsactive[0] = true;
					menuoptions[1] = "unlock intermissions";         menuoptionsactive[1] = !unlock[16];
					menuoptions[2] = "unlock no death mode";  			 menuoptionsactive[2] = !unlock[17];
					menuoptions[3] = "unlock flip mode";             menuoptionsactive[3] = !unlock[18];
					menuoptions[4] = "unlock ship jukebox";          menuoptionsactive[4] = (stat_trinkets<20);
					menuoptions[5] = "unlock secret lab";            menuoptionsactive[5] = !unlock[8];
					menuoptions[6] = "return";                       menuoptionsactive[6] = true;
					nummenuoptions = 7;
					menuxoff = -70;	menuyoff = -20;
				}else if (t == "credits") {
					menuoptions[0] = "next page";      menuoptionsactive[0] = true;
					menuoptions[1] = "return";  				 menuoptionsactive[1] = true;
					nummenuoptions = 2;
					menuxoff = 20;	menuyoff = 64;
				}else if (t == "credits2") {
					menuoptions[0] = "next page";      menuoptionsactive[0] = true;
					menuoptions[1] = "return";  				 menuoptionsactive[1] = true;
					nummenuoptions = 2;
					menuxoff = 20;	menuyoff = 64;
				}else if (t == "credits3") {
					menuoptions[0] = "next page";      menuoptionsactive[0] = true;
					menuoptions[1] = "return";  				 menuoptionsactive[1] = true;
					nummenuoptions = 2;
					menuxoff = 20;	menuyoff = 64;
				}else if (t == "credits4") {
					menuoptions[0] = "next page";      menuoptionsactive[0] = true;
					menuoptions[1] = "return";  				 menuoptionsactive[1] = true;
					nummenuoptions = 2;
					menuxoff = 20;	menuyoff = 64;
				}else if (t == "credits5") {
					menuoptions[0] = "next page";      menuoptionsactive[0] = true;
					menuoptions[1] = "return";  				 menuoptionsactive[1] = true;
					nummenuoptions = 2;
					menuxoff = 20;	menuyoff = 64;
				}else if (t == "credits6") {
					menuoptions[0] = "next page";      menuoptionsactive[0] = true;
					menuoptions[1] = "return";  				 menuoptionsactive[1] = true;
					nummenuoptions = 2;
					menuxoff = 20;	menuyoff = 64;
				}else if (t == "credits7") {
					menuoptions[0] = "next page";      menuoptionsactive[0] = true;
					menuoptions[1] = "return";  				 menuoptionsactive[1] = true;
					nummenuoptions = 2;
					menuxoff = 20;	menuyoff = 64;
				}else if (t == "credits8") {
					menuoptions[0] = "next page";      menuoptionsactive[0] = true;
					menuoptions[1] = "return";  				 menuoptionsactive[1] = true;
					nummenuoptions = 2;
					menuxoff = 20;	menuyoff = 64;
				}else if (t == "credits9") {
					menuoptions[0] = "first page";      menuoptionsactive[0] = true;
					menuoptions[1] = "return";  				 menuoptionsactive[1] = true;
					nummenuoptions = 2;
					menuxoff = 20;	menuyoff = 64;
				}else if (t == "play") {
					//Ok, here's where the unlock stuff comes into it:
					//First up, time trials:
					temp = 0;
					if (unlock[0] && stat_trinkets >= 3 && !unlocknotify[9]) temp++;
					if (unlock[1] && stat_trinkets >= 6 && !unlocknotify[10]) temp++;
					if (unlock[2] && stat_trinkets >= 9 && !unlocknotify[11]) temp++;
					if (unlock[3] && stat_trinkets >= 12 && !unlocknotify[12]) temp++;
					if (unlock[4] && stat_trinkets >= 15 && !unlocknotify[13]) temp++;
					if (unlock[5] && stat_trinkets >= 18 && !unlocknotify[14]) temp++;
					if (temp > 0) {
						//you've unlocked a time trial!					
						if (unlock[0] && stat_trinkets >= 3) { unlocknotify[9] = true; unlock[9] = true; }
						if (unlock[1] && stat_trinkets >= 6) { unlocknotify[10] = true; unlock[10] = true; }
						if (unlock[2] && stat_trinkets >= 9) { unlocknotify[11] = true; unlock[11] = true; }					
						if (unlock[3] && stat_trinkets >= 12) { unlocknotify[12] = true; unlock[12] = true; }					
						if (unlock[4] && stat_trinkets >= 15) { unlocknotify[13] = true; unlock[13] = true; }					
						if (unlock[5] && stat_trinkets >= 18) { unlocknotify[14] = true; unlock[14] = true; }
						
						if (temp == 1) {
							createmenu("unlocktimetrial"); savemystats = true;
						}else if (temp > 1) {
							createmenu("unlocktimetrials"); savemystats = true;
						}
					}else {
						//Alright, we haven't unlocked any time trials. How about no death mode?
						temp = 0;
						if (bestrank[0] >= 2) temp++;
						if (bestrank[1] >= 2) temp++;
						if (bestrank[2] >= 2) temp++;
						if (bestrank[3] >= 2) temp++;
						if (bestrank[4] >= 2) temp++;
						if (bestrank[5] >= 2) temp++;
						if (temp >= 4 && !unlocknotify[17]) {
							//Unlock No Death Mode
							unlocknotify[17] = true; unlock[17] = true;
							createmenu("unlocknodeathmode");
							savemystats = true;
						}else{
							//Alright then! Flip mode?
							if (unlock[5] && !unlocknotify[18]) {
								unlock[18] = true; unlocknotify[18] = true;
								createmenu("unlockflipmode");
								savemystats = true;
							}else {
								//What about the intermission levels?
								if (unlock[7] && !unlocknotify[16]) {
									unlock[16] = true; unlocknotify[16] = true;
									createmenu("unlockintermission");
									savemystats = true;
								}else {
									//ok, secret lab! no notification, but test:
									if (unlock[8]) {
										createmenu("playsecretlab");
									}else{
										menuoptions[0] = "continue";      menuoptionsactive[0] = true;
										menuoptions[1] = "play modes";  	menuoptionsactive[1] = true;
										menuoptions[2] = "new game";      menuoptionsactive[2] = true;
										menuoptions[3] = "return";          menuoptionsactive[3] = true;
										nummenuoptions = 4;
										menuxoff = -20;	menuyoff = -40;
									}
								}
							}
						}
					}
				}else if (t == "unlocktimetrial" || t == "unlocktimetrials") {
					menuoptions[0] = "continue";          menuoptionsactive[0] = true;
					nummenuoptions = 1;
					menuxoff = 20;	menuyoff = 70;
				}else if (t == "unlocknodeathmode") {
					menuoptions[0] = "continue";          menuoptionsactive[0] = true;
					nummenuoptions = 1;
					menuxoff = 20;	menuyoff = 70;
				}else if (t == "unlockintermission") {
					menuoptions[0] = "continue";          menuoptionsactive[0] = true;
					nummenuoptions = 1;
					menuxoff = 20;	menuyoff = 70;
				}else if (t == "unlockflipmode") {
					menuoptions[0] = "continue";          menuoptionsactive[0] = true;
					nummenuoptions = 1;
					menuxoff = 20;	menuyoff = 70;
				}else if (t == "playsecretlab") {
					menuoptions[0] = "continue";      menuoptionsactive[0] = true;
					menuoptions[1] = "secret lab";  	menuoptionsactive[1] = true;
					menuoptions[2] = "play modes";  	menuoptionsactive[2] = true;
					menuoptions[3] = "new game";      menuoptionsactive[3] = true;
					menuoptions[4] = "return";          menuoptionsactive[4] = true;
					nummenuoptions = 5;
					menuxoff = -40;	menuyoff = -30;
				}else if (t == "newgamewarning") {
					menuoptions[0] = "start new game";      menuoptionsactive[0] = true;
					menuoptions[1] = "return to menu";  				 menuoptionsactive[1] = true;
					nummenuoptions = 2;
					menuxoff = -30;	menuyoff = 64;
				}else if (t == "playmodes") {
					menuoptions[0] = "time trials";    menuoptionsactive[0] = true;
					menuoptions[1] = "intermissions";      menuoptionsactive[1] = unlock[16];
					menuoptions[2] = "no death mode";  	menuoptionsactive[2] = unlock[17];
					menuoptions[3] = "flip mode";      menuoptionsactive[3] = unlock[18];
					menuoptions[4] = "return to play menu";      menuoptionsactive[4] = true;
					nummenuoptions = 5;
					menuxoff = -70;	menuyoff = 8;
				}else if (t == "intermissionmenu") {
					menuoptions[0] = "play intermission 1";      menuoptionsactive[0] = true;
					menuoptions[1] = "play intermission 2";  	menuoptionsactive[1] = true;
					menuoptions[2] = "return to play menu";          menuoptionsactive[2] = true;
					nummenuoptions = 3;
					menuxoff = -50;	menuyoff = -35;
				}else if (t == "playint1") {
					menuoptions[0] = "Vitellary";      menuoptionsactive[0] = true;
					menuoptions[1] = "Vermilion";  	menuoptionsactive[1] = true;
					menuoptions[2] = "Verdigris";  	menuoptionsactive[2] = true;
					menuoptions[3] = "Victoria";  	menuoptionsactive[3] = true;
					menuoptions[4] = "return";          menuoptionsactive[4] = true;
					nummenuoptions = 5;
					menuxoff = -60;	menuyoff = 10;
				}else if (t == "playint2") {
					menuoptions[0] = "Vitellary";      menuoptionsactive[0] = true;
					menuoptions[1] = "Vermilion";  	menuoptionsactive[1] = true;
					menuoptions[2] = "Verdigris";  	menuoptionsactive[2] = true;
					menuoptions[3] = "Victoria";  	menuoptionsactive[3] = true;
					menuoptions[4] = "return";          menuoptionsactive[4] = true;
					nummenuoptions = 5;
					menuxoff = -60;	menuyoff = 10;
				}else if (t == "continue") {
					menuoptions[0] = "continue from teleporter";      menuoptionsactive[0] = true;
					menuoptions[1] = "continue from quicksave";  	menuoptionsactive[1] = true;
					menuoptions[2] = "return to play menu";          menuoptionsactive[2] = true;
					nummenuoptions = 3;
					menuxoff = -60;	menuyoff = 20;
				}else if (t == "startnodeathmode") {
					menuoptions[0] = "disable cutscenes";      menuoptionsactive[0] = true;
					menuoptions[1] = "enable cutscenes";  	menuoptionsactive[1] = true;
					menuoptions[2] = "return to play menu";          menuoptionsactive[2] = true;
					nummenuoptions = 3;
					menuxoff = -60;	menuyoff = 40;
				}else if (t == "gameover") {
					nummenuoptions = 0;
					menucountdown = 120; menudest="gameover2";
				}else if (t == "gameover2") {
					menuoptions[0] = "return to play menu";          menuoptionsactive[0] = true;
					nummenuoptions = 1;
					menuxoff = -25;	menuyoff = 80;
				}else if (t == "unlockmenutrials") {
					if (unlock[9]) {  menuoptions[0] = "space station 1";                      menuoptionsactive[0] = true;
					}else{            menuoptions[0] = "space station 1";      menuoptionsactive[0] = true;}
					if (unlock[10]) {  menuoptions[1] = "the laboratory";                     menuoptionsactive[1] = true;
					}else{            menuoptions[1] = "the laboratory";            menuoptionsactive[1] = true;}
					if (unlock[11]) {  menuoptions[2] = "the tower";                     menuoptionsactive[2] = true;
					}else{            menuoptions[2] = "the tower";                menuoptionsactive[2] = true;}
					if (unlock[12]) {  menuoptions[3] = "space station 2";                     menuoptionsactive[3] = true;
					}else{            menuoptions[3] = "space station 2";            menuoptionsactive[3] = true;}
					if (unlock[13]) {  menuoptions[4] = "the warp zone";                     menuoptionsactive[4] = true;
					}else{            menuoptions[4] = "the warp zone";            menuoptionsactive[4] = true;}
					if (unlock[14]) {  menuoptions[5] = "the final level";                     menuoptionsactive[5] = true;
					}else{            menuoptions[5] = "the final level";          menuoptionsactive[5] = true;}
					
					menuoptions[6] = "return to unlock menu";      menuoptionsactive[6] = true;
					nummenuoptions = 7;
					menuxoff = -80;	menuyoff = 0 - (12*6);
				}else if (t == "timetrials") {
					if (!unlock[9]) {  menuoptions[0] = "???";                      menuoptionsactive[0] = true;
					}else{            menuoptions[0] = "space station 1";      menuoptionsactive[0] = true;}
					if (!unlock[10]) {  menuoptions[1] = "???";                     menuoptionsactive[1] = true;
					}else{            menuoptions[1] = "the laboratory";            menuoptionsactive[1] = true;}
					if (!unlock[11]) {  menuoptions[2] = "???";                     menuoptionsactive[2] = true;
					}else{            menuoptions[2] = "the tower";                menuoptionsactive[2] = true;}
					if (!unlock[12]) {  menuoptions[3] = "???";                     menuoptionsactive[3] = true;
					}else{            menuoptions[3] = "space station 2";            menuoptionsactive[3] = true;}
					if (!unlock[13]) {  menuoptions[4] = "???";                     menuoptionsactive[4] = true;
					}else{            menuoptions[4] = "the warp zone";            menuoptionsactive[4] = true;}
					if (!unlock[14]) {  menuoptions[5] = "???";                     menuoptionsactive[5] = true;
					}else{            menuoptions[5] = "the final level";          menuoptionsactive[5] = true;}
					
					menuoptions[6] = "return to play menu";      menuoptionsactive[6] = true;
					nummenuoptions = 7;
					menuxoff = -80;	menuyoff = 0 - (12*6);
				}else if (t == "nodeathmodecomplete") {
					nummenuoptions = 0;
					menucountdown = 90; menudest = "nodeathmodecomplete2";
				}else if (t == "nodeathmodecomplete2") {
					menuoptions[0] = "return to play menu";          menuoptionsactive[0] = true;
					nummenuoptions = 1;
					menuxoff = -25;	menuyoff = 70;
				}else if (t == "timetrialcomplete") {
					nummenuoptions = 0;
					menucountdown = 90; menudest="timetrialcomplete2";
				}else if (t == "timetrialcomplete2") {
					nummenuoptions = 0;
					menucountdown = 60; menudest="timetrialcomplete3";
				}else if (t == "timetrialcomplete3") {
					menuoptions[0] = "return to play menu";          menuoptionsactive[0] = true;
					menuoptions[1] = "try again";  	menuoptionsactive[1] = true;
					nummenuoptions = 2;
					menuxoff = -25;	menuyoff = 70;
				}else if (t == "gamecompletecontinue") {
					menuoptions[0] = "return to play menu";          menuoptionsactive[0] = true;
					nummenuoptions = 1;
					menuxoff = -25;	menuyoff = 70;
				}
			}
		}
		
		public function loadstats(map:mapclass, dwgfx:dwgraphicsclass):void {
			statcookie = SharedObject.getLocal("vvvvvv_unlock");
			
			if (statcookie.data.stattrinkets == undefined) {
				savestats(map, dwgfx); //save with default values
			}else{
				temp_unlock = statcookie.data.unlock.slice();				
				unlock = temp_unlock.slice();
				
				temp_unlocknotify = statcookie.data.unlocknotify.slice();
				unlocknotify = temp_unlocknotify.slice();
				
				temp_besttimes = statcookie.data.besttimes.slice();
				besttimes = temp_besttimes.slice();
				temp_besttrinkets = statcookie.data.besttrinkets.slice();
				besttrinkets = temp_besttrinkets.slice();
				temp_bestlives = statcookie.data.bestlives.slice();
				bestlives = temp_bestlives.slice();				
				temp_bestrank = statcookie.data.bestrank.slice();
				bestrank = temp_bestrank.slice();				
				
				bestgamedeaths = statcookie.data.bestgamedeaths;
				
				stat_trinkets =  statcookie.data.stattrinkets;
				fullscreen =  statcookie.data.fullscreen;
				
				noflashingmode = statcookie.data.screenshakes;
				colourblindmode = statcookie.data.backgrounds;
				dwgfx.setflipmode = statcookie.data.flipmode;
				map.invincibility = statcookie.data.invincibility;
				slowdown = statcookie.data.slowdown;
				
				swnbestrank = statcookie.data.swnbestrank;
			  swnrecord = statcookie.data.swnrecord;
				
				advanced_mode = statcookie.data.advanced_mode;
				advanced_smoothing = statcookie.data.advanced_smoothing;
				advanced_scaling = statcookie.data.advanced_scaling;
				
				controlstyle = statcookie.data.controlstyle;
				controlsensitivity = statcookie.data.controlsensitivity;
				showcontroltutorial = statcookie.data.showcontroltutorial;
			}
			statcookie.close();
		}
		
		public function savestats(map:mapclass, dwgfx:dwgraphicsclass):void {
			statcookie = SharedObject.getLocal("vvvvvv_unlock");
						
			temp_unlock = unlock.slice();
			temp_unlocknotify = unlocknotify.slice();
			
			statcookie.data.unlock = temp_unlock.slice();
			statcookie.data.unlocknotify = temp_unlocknotify.slice();
			
			temp_besttimes = besttimes.slice();
			temp_besttrinkets = besttrinkets.slice();
			temp_bestlives = bestlives.slice();
			temp_bestrank = bestrank.slice();
			statcookie.data.besttimes = temp_besttimes.slice();
			statcookie.data.besttrinkets = temp_besttrinkets.slice();
			statcookie.data.bestlives = temp_bestlives.slice();
			statcookie.data.bestrank = temp_bestrank.slice();
			
			statcookie.data.bestgamedeaths = bestgamedeaths;
			
			statcookie.data.stattrinkets = stat_trinkets;
		  statcookie.data.fullscreen = fullscreen;
			
			statcookie.data.screenshakes = noflashingmode;
			statcookie.data.backgrounds = colourblindmode;
			statcookie.data.flipmode = dwgfx.setflipmode;
			statcookie.data.invincibility = map.invincibility;
			statcookie.data.slowdown = slowdown;
			
			statcookie.data.swnbestrank = swnbestrank;
			statcookie.data.swnrecord = swnrecord;
			
			statcookie.data.advanced_mode = advanced_mode;
			statcookie.data.advanced_smoothing = advanced_smoothing;
		  statcookie.data.advanced_scaling = advanced_scaling;
			
			statcookie.data.controlstyle = controlstyle;
			statcookie.data.controlsensitivity = controlsensitivity;
			statcookie.data.showcontroltutorial = showcontroltutorial;
			
      statcookie.flush()
			statcookie.close();
		}
		
		public function deletestats(map:mapclass, dwgfx:dwgraphicsclass):void {
			for (i = 0; i < 25; i++) {
				unlock[i] = false;
				unlocknotify[i] = false;
			}				
			for (i = 0; i < 6; i++) {				
				besttimes[i] = -1;
				besttrinkets[i] = -1;
				bestlives[i] = -1;
				bestrank[i] = -1;
			}			
			dwgfx.setflipmode = false;
			stat_trinkets = 0;
			controlstyle = 0;
			controlsensitivity = 50;
			savestats(map, dwgfx);
		}
		
		public function unlocknum(t:int, map:mapclass, dwgfx:dwgraphicsclass):void {
			unlock[t] = true;
			savestats(map, dwgfx);
		}
		
		public function CopyLevelData():void {
			//Not used in mobile version
		}
		
		public function loadsummary(map:mapclass, help:helpclass):void {
			quickcookie = SharedObject.getLocal("vvvvvv_quicksave");
			telecookie = SharedObject.getLocal("vvvvvv_telesave");
			
			if (telecookie.data.savex == undefined) {
        telecookieexists = false; telesummary = "";
      } else {
        telecookieexists = true; telesummary = telecookie.data.summary;
				tele_gametime = giventimestring(telecookie.data.hours, telecookie.data.minutes, telecookie.data.seconds, help);
				tele_trinkets = telecookie.data.trinkets;
				tele_currentarea = map.currentarea(map.area(telecookie.data.savex, telecookie.data.savey));
				
			  summary_crewstats = telecookie.data.crewstats.slice();
			  tele_crewstats = summary_crewstats.slice();
      }
			
			if (quickcookie.data.savex == undefined) {
        quickcookieexists = false; quicksummary = "";
      } else {
        quickcookieexists = true; quicksummary = quickcookie.data.summary;
				quick_gametime = giventimestring(quickcookie.data.hours, quickcookie.data.minutes, quickcookie.data.seconds, help);
				quick_trinkets = quickcookie.data.trinkets;
				quick_currentarea = map.currentarea(map.area(quickcookie.data.savex, quickcookie.data.savey));
				
			  summary_crewstats = quickcookie.data.crewstats.slice();
			  quick_crewstats = summary_crewstats.slice();
      }
		}
		
		public function savequick(map:mapclass, obj:entityclass, music:musicclass, help:helpclass):void {
			quickcookie = SharedObject.getLocal("vvvvvv_quicksave");
			//Save to the quicksave cookie
			quickcookieexists = true;
			//Flags, map and stats
			savestate[0].explored = map.explored.slice();
			savestate[0].flags = obj.flags.slice();
			savestate[0].crewstats = crewstats.slice();
			savestate[0].collect = obj.collect.slice();
			
			quickcookie.data.worldmap = savestate[0].explored.slice();
			quickcookie.data.flags = savestate[0].flags.slice();
			quickcookie.data.crewstats = savestate[0].crewstats.slice();
			quickcookie.data.collect = savestate[0].collect.slice();
			
			//trace(obj.flags);
			
			quickcookie.data.finalmode = map.finalmode;
			quickcookie.data.finalstretch = map.finalstretch;
			
			quickcookie.data.finalx = map.finalx;
			quickcookie.data.finaly = map.finaly;
			//Position
			quickcookie.data.savex = savex;
			quickcookie.data.savey = savey;
			quickcookie.data.saverx = saverx;
			quickcookie.data.savery = savery;
			quickcookie.data.savegc = savegc;
			quickcookie.data.savedir = savedir;
			quickcookie.data.savepoint = savepoint;
			quickcookie.data.trinkets = trinkets;
			
			quickcookie.data.currentsong = music.currentsong;
			quickcookie.data.teleportscript = teleportscript;
			
			//Special stats
			quickcookie.data.companion = companion; 
			quickcookie.data.lastsaved = lastsaved; 
			quickcookie.data.supercrewmate = supercrewmate;
			quickcookie.data.scmprogress = scmprogress;
		  quickcookie.data.scmmoveme = scmmoveme;
			
		  quickcookie.data.frames = frames; quickcookie.data.seconds = seconds; 
			quickcookie.data.minutes = minutes; quickcookie.data.hours = hours;
			
		  quickcookie.data.deathcounts = deathcounts;
			quickcookie.data.totalflips = totalflips;
		  quickcookie.data.hardestroom = hardestroom; quickcookie.data.hardestroomdeaths = hardestroomdeaths; 
			
			savearea = map.currentarea(map.area(roomx, roomy));
			quickcookie.data.summary = savearea + ", " + timestring(help);
			quicksummary = quickcookie.data.summary;
			
      quickcookie.flush()
			quickcookie.close();
		}
		
		public function customsavequick(t:int, map:mapclass, obj:entityclass, music:musicclass, help:helpclass):void {
			customcookie[t] = SharedObject.getLocal("vvvvvv_custom_" + String(t));
			
			//Flags, map and stats
			savestate[2].explored = map.explored.slice();
			savestate[2].flags = obj.flags.slice();
			savestate[2].crewstats = crewstats.slice();
			savestate[2].collect = obj.collect.slice();
			savestate[2].customcollect = obj.customcollect.slice();
			savestate[2].moods = obj.customcrewmoods.slice();
			
			customcookie[t].data.worldmap = savestate[2].explored.slice();
			customcookie[t].data.flags = savestate[2].flags.slice();
			customcookie[t].data.crewstats = savestate[2].crewstats.slice();
			customcookie[t].data.collect = savestate[2].collect.slice();
			customcookie[t].data.customcollect = savestate[2].customcollect.slice();
			customcookie[t].data.moods = savestate[2].moods.slice();
			
			//trace(obj.flags);
			
			customcookie[t].data.finalmode = map.finalmode;
			customcookie[t].data.finalstretch = map.finalstretch;
			
			customcookie[t].data.finalx = map.finalx;
			customcookie[t].data.finaly = map.finaly;
			//Position
			customcookie[t].data.savex = savex;
			customcookie[t].data.savey = savey;
			customcookie[t].data.saverx = saverx;
			customcookie[t].data.savery = savery;
			customcookie[t].data.savegc = savegc;
			customcookie[t].data.savedir = savedir;
			customcookie[t].data.savepoint = savepoint;
			customcookie[t].data.trinkets = trinkets;
			customcookie[t].data.crewmates = crewmates;
			
			customcookie[t].data.currentsong = music.currentsong;
			customcookie[t].data.teleportscript = teleportscript;
			
			//Special stats
			customcookie[t].data.companion = companion; 
			customcookie[t].data.lastsaved = lastsaved; 
			customcookie[t].data.supercrewmate = supercrewmate;
			customcookie[t].data.scmprogress = scmprogress;
		  customcookie[t].data.scmmoveme = scmmoveme;
			
		  customcookie[t].data.frames = frames; customcookie[t].data.seconds = seconds; 
			customcookie[t].data.minutes = minutes; customcookie[t].data.hours = hours;
			
		  customcookie[t].data.deathcounts = deathcounts;
			customcookie[t].data.totalflips = totalflips;
		  customcookie[t].data.hardestroom = hardestroom; customcookie[t].data.hardestroomdeaths = hardestroomdeaths; 
			
			customcookie[t].data.showminimap = map.customshowmm;
			
			savearea = map.currentarea(map.area(roomx, roomy))
			customcookie[t].data.summary = savearea + ", " + timestring(help);
			customcookiesummary[t] = customcookie[t].data.summary;
			
      customcookie[t].flush()
			customcookie[t].close();
			
			customcookieexists[t] = true; 
			customcookiesummary[t] = savearea + ", " + timestring(help);
		}
		
		public function loadquick(map:mapclass, obj:entityclass, music:musicclass):void {
			quickcookie = SharedObject.getLocal("vvvvvv_quicksave");
			//Save to the quicksave cookie
			savestate[0].explored = quickcookie.data.worldmap.slice();
			savestate[0].flags = quickcookie.data.flags.slice();
			savestate[0].crewstats = quickcookie.data.crewstats.slice();
			savestate[0].collect = quickcookie.data.collect.slice();
			
			map.explored = savestate[0].explored.slice();
			obj.flags = savestate[0].flags.slice();
			crewstats = savestate[0].crewstats.slice();
			obj.collect = savestate[0].collect.slice();
			//trace(obj.flags);
			
			map.finalmode = quickcookie.data.finalmode;
			map.finalstretch = quickcookie.data.finalstretch;
			if (map.finalmode) {
				map.final_colormode = false;	map.final_mapcol = 0;	map.final_colorframe = 0;
			}
			if (map.finalstretch) {
				map.finalstretch = true;
				map.final_colormode = true;
				map.final_mapcol = 0;
				map.final_colorframe = 1;
			}
			
			map.finalx = quickcookie.data.finalx;
			map.finaly = quickcookie.data.finaly;
			//position
			savex = quickcookie.data.savex;
			savey = quickcookie.data.savey;
			saverx = quickcookie.data.saverx;
			savery = quickcookie.data.savery;
			savegc = quickcookie.data.savegc;
			savedir = quickcookie.data.savedir;
			savepoint = quickcookie.data.savepoint;
			trinkets = quickcookie.data.trinkets;
			
			//Special stats
			companion = quickcookie.data.companion; 
			lastsaved = quickcookie.data.lastsaved;
			teleportscript = quickcookie.data.teleportscript;
			supercrewmate = quickcookie.data.supercrewmate;
			scmprogress = quickcookie.data.scmprogress;
			scmmoveme = quickcookie.data.scmmoveme;
			
		  frames = quickcookie.data.frames; seconds = quickcookie.data.seconds; 
			minutes = quickcookie.data.minutes; hours = quickcookie.data.hours;
			
		  deathcounts = quickcookie.data.deathcounts;
			totalflips = quickcookie.data.totalflips;
		  hardestroom = quickcookie.data.hardestroom; hardestroomdeaths = quickcookie.data.hardestroomdeaths; 
			
			music.play(quickcookie.data.currentsong);
			
			//Ok, now we determine some game stuff based on the above:
			map.showteleporters = true;
			if(obj.flags[12]==1) map.showtargets = true;
			if (obj.flags[42] == 1) map.showtrinkets = true;
			
			quickcookie.close();
		}
		
		public function customloadquick(t:int, map:mapclass, obj:entityclass, music:musicclass):void{
			customcookie[t] = SharedObject.getLocal("vvvvvv_custom_" + String(t));
			
			savestate[2].explored = customcookie[t].data.worldmap.slice();
			savestate[2].flags = customcookie[t].data.flags.slice();
			savestate[2].crewstats = customcookie[t].data.crewstats.slice();
			savestate[2].collect = customcookie[t].data.collect.slice();
			savestate[2].customcollect = customcookie[t].data.customcollect.slice();
			savestate[2].moods = customcookie[t].data.moods.slice();
			
			map.explored = savestate[2].explored.slice();
			obj.flags = savestate[2].flags.slice();
			crewstats = savestate[2].crewstats.slice();
			obj.collect = savestate[2].collect.slice();
			obj.customcollect = savestate[2].customcollect.slice();
			obj.customcrewmoods = savestate[2].moods.slice();
			//trace(obj.flags);
			
			map.finalmode = customcookie[t].data.finalmode;
			map.finalstretch = customcookie[t].data.finalstretch;
			if (map.finalmode) {
				map.final_colormode = false;	map.final_mapcol = 0;	map.final_colorframe = 0;
			}
			if (map.finalstretch) {
				map.finalstretch = true;
				map.final_colormode = true;
				map.final_mapcol = 0;
				map.final_colorframe = 1;
			}
			
			map.finalx = customcookie[t].data.finalx;
			map.finaly = customcookie[t].data.finaly;
			//position
			savex = customcookie[t].data.savex;
			savey = customcookie[t].data.savey;
			saverx = customcookie[t].data.saverx;
			savery = customcookie[t].data.savery;
			savegc = customcookie[t].data.savegc;
			savedir = customcookie[t].data.savedir;
			savepoint = customcookie[t].data.savepoint;
			trinkets = customcookie[t].data.trinkets;
			crewmates = customcookie[t].data.crewmates;
			
			//Special stats
			companion = customcookie[t].data.companion; 
			lastsaved = customcookie[t].data.lastsaved;
			teleportscript = customcookie[t].data.teleportscript;
			supercrewmate = customcookie[t].data.supercrewmate;
			scmprogress = customcookie[t].data.scmprogress;
			scmmoveme = customcookie[t].data.scmmoveme;
			
		  frames = customcookie[t].data.frames; seconds = customcookie[t].data.seconds; 
			minutes = customcookie[t].data.minutes; hours = customcookie[t].data.hours;
			
		  deathcounts = customcookie[t].data.deathcounts;
			totalflips = customcookie[t].data.totalflips;
		  hardestroom = customcookie[t].data.hardestroom; hardestroomdeaths = customcookie[t].data.hardestroomdeaths; 
			
			map.customshowmm = customcookie[t].data.showminimap;
			
			music.play(customcookie[t].data.currentsong);
			
			//Ok, now we determine some game stuff based on the above:
			map.showteleporters = true;
			if (obj.flags[12] == 1) map.showtargets = true;
			if (obj.flags[42] == 1) map.showtrinkets = true;
			
			customcookie[t].close();
		}
		
		public function deletequick():void {
			quicksummary = "";
			quickcookieexists = false;
			quickcookie.clear();
		}
		
		public function telegotoship():void {
			//Special function to move the telesave to the ship teleporter.
			telecookie.data.savex = 13*8;
			telecookie.data.savey = 129;
			telecookie.data.saverx = 102;
			telecookie.data.savery = 111;
			telecookie.data.savegc = 0;
			telecookie.data.savedir = 1;
			telecookie.data.savepoint = 0;
			
			telecookie.data.currentsong = 4;
			telecookie.data.companion = 0; 
			
			telecookie.data.finalmode = false;;
			telecookie.data.finalstretch = false;
		}
		
		public function savetele(map:mapclass, obj:entityclass, music:musicclass, help:helpclass):void {
			//do a quicksave instead
			savequick(map, obj, music, help);
			/*
			telecookie = SharedObject.getLocal("vvvvvv_telesave");
			//Save to the telesave cookie
			telecookieexists = true;
			//Flags, map and stats
			savestate[1].explored = map.explored.slice();
			savestate[1].flags = obj.flags.slice();
			savestate[1].crewstats = crewstats.slice();
			savestate[1].collect = obj.collect.slice();
			
			telecookie.data.worldmap = savestate[1].explored.slice();
			telecookie.data.flags = savestate[1].flags.slice();
			telecookie.data.crewstats = savestate[1].crewstats.slice();
			telecookie.data.collect = savestate[1].collect.slice();
			
			telecookie.data.finalmode = map.finalmode;
			telecookie.data.finalstretch = map.finalstretch;
			
			telecookie.data.finalx = map.finalx;
			telecookie.data.finaly = map.finaly;
			//Position
			telecookie.data.savex = savex;
			telecookie.data.savey = savey;
			telecookie.data.saverx = saverx;
			telecookie.data.savery = savery;
			telecookie.data.savegc = savegc;
			telecookie.data.savedir = savedir;
			telecookie.data.savepoint = savepoint;
			telecookie.data.trinkets = trinkets;
			
			if (music.nicechange != -1) {
				telecookie.data.currentsong = music.nicechange;
			}else{
			  telecookie.data.currentsong = music.currentsong;
			}
			telecookie.data.teleportscript = teleportscript;
			
			//Special stats
			telecookie.data.companion = companion; 
			telecookie.data.lastsaved = lastsaved;
			telecookie.data.supercrewmate = supercrewmate;
			telecookie.data.scmprogress = scmprogress;
			telecookie.data.scmmoveme = scmmoveme;
			
		  telecookie.data.frames = frames; telecookie.data.seconds = seconds; 
			telecookie.data.minutes = minutes; telecookie.data.hours = hours;
			
		  telecookie.data.deathcounts = deathcounts;
			telecookie.data.totalflips = totalflips;
		  telecookie.data.hardestroom = hardestroom; telecookie.data.hardestroomdeaths = hardestroomdeaths; 
			
			savearea = map.currentarea(map.area(roomx, roomy))
			telecookie.data.summary = savearea + ", " + timestring(help);
			telesummary = telecookie.data.summary;
			
      telecookie.flush();
			telecookie.close();
			*/
		}
		
		public function loadtele(map:mapclass, obj:entityclass, music:musicclass):void {
			telecookie = SharedObject.getLocal("vvvvvv_telesave");
			//Save to the telesave cookie
			savestate[1].explored = telecookie.data.worldmap.slice();
			savestate[1].flags = telecookie.data.flags.slice();
			savestate[1].crewstats = telecookie.data.crewstats.slice();
			savestate[1].collect = telecookie.data.collect.slice();
			
			map.explored = savestate[1].explored.slice();
			obj.flags = savestate[1].flags.slice();
			crewstats = savestate[1].crewstats.slice();
			obj.collect = savestate[1].collect.slice();
			
			map.finalmode = telecookie.data.finalmode;
			map.finalstretch = telecookie.data.finalstretch;
			if (map.finalmode) {
				map.final_colormode = false;	map.final_mapcol = 0;	map.final_colorframe = 0;
			}
			if (map.finalstretch) {
				map.finalstretch = true;
				map.final_colormode = true;
				map.final_mapcol = 0;
				map.final_colorframe = 1;
			}
			
			map.finalx = telecookie.data.finalx;
			map.finaly = telecookie.data.finaly;
			//position
			savex = telecookie.data.savex;
			savey = telecookie.data.savey;
			saverx = telecookie.data.saverx;
			savery = telecookie.data.savery;
			savegc = telecookie.data.savegc;
			savedir = telecookie.data.savedir;
			savepoint = telecookie.data.savepoint;
			trinkets = telecookie.data.trinkets;
			
			//trinkets = 19; obj.flags[41] = 0; obj.flags[42] = 0; obj.collect[18] = 0; //Testing Victoria epilogue criteria#
			
			//Special stats
			companion = telecookie.data.companion; 
			lastsaved = telecookie.data.lastsaved;
			teleportscript = telecookie.data.teleportscript;
			supercrewmate = telecookie.data.supercrewmate;
			scmprogress = telecookie.data.scmprogress;
			scmmoveme = telecookie.data.scmmoveme;
			
		  frames = telecookie.data.frames; seconds = telecookie.data.seconds; 
			minutes = telecookie.data.minutes; hours = telecookie.data.hours;
			
		  deathcounts = telecookie.data.deathcounts;
			totalflips = telecookie.data.totalflips;
		  hardestroom = telecookie.data.hardestroom; hardestroomdeaths = telecookie.data.hardestroomdeaths; 
			
			music.play(telecookie.data.currentsong);
			
			//Ok, now we determine some game stuff based on the above:
			map.showteleporters = true;
			if(obj.flags[12]==1) map.showtargets = true;
			if (obj.flags[42] == 1) map.showtrinkets = true;
			
			telecookie.close();
			
		}
		
		public function deletetele():void {
			telesummary = "";
			telecookieexists = false;
			telecookie.clear();
		}
		
		public function crewrescued():int {
			temp = 0;
			if (crewstats[0]) temp++;
			if (crewstats[1]) temp++;
			if (crewstats[2]) temp++;
			if (crewstats[3]) temp++;
			if (crewstats[4]) temp++;
			if (crewstats[5]) temp++;
			return temp;
		}
		
		public function unrescued():String {
			//Randomly return the name of an unrescued crewmate
			if (Math.random() * 100 > 50) {
				if (!crewstats[5]) return "Victoria";
				if (!crewstats[2]) return "Vitellary";
				if (!crewstats[4]) return "Verdigris";
				if (!crewstats[3]) return "Vermilion";
			}else {
				if (Math.random() * 100 > 50) {
					if (!crewstats[2]) return "Vitellary";
					if (!crewstats[4]) return "Verdigris";
					if (!crewstats[3]) return "Vermilion";
					if (!crewstats[5]) return "Victoria";
				}else {
					if (!crewstats[4]) return "Verdigris";
					if (!crewstats[3]) return "Vermilion";	
					if (!crewstats[5]) return "Victoria";
					if (!crewstats[2]) return "Vitellary";
				}
			}
			return "you";
		}
		
		public function gethardestroom(map:mapclass):void {
			if (currentroomdeaths > hardestroomdeaths) {
				hardestroomdeaths = currentroomdeaths;
				hardestroom = map.roomname;
				if (map.roomname == "glitch") {
					if (roomx == 42 && roomy == 51) {
					  hardestroom = "Rear Vindow";
				  }else if (roomx == 48 && roomy == 51) {
					  hardestroom = "On the Vaterfront";
				  }else if (roomx == 49 && roomy == 51) {
					  hardestroom = "The Untouchavles";
					}
				}else if (map.roomname == "change") {
					if (roomx == 45 && roomy == 51) hardestroom =map.specialnames[3];
				  if (roomx == 46 && roomy == 51) hardestroom =map.specialnames[4];
				  if (roomx == 47 && roomy == 51) hardestroom =map.specialnames[5];
				  if (roomx == 50 && roomy == 53) hardestroom =map.specialnames[6];
				  if (roomx == 50 && roomy == 54) hardestroom = map.specialnames[7];		
				}
			}
		}
		
		public function updatestate(dwgfx:dwgraphicsclass, map:mapclass, obj:entityclass, help:helpclass, music:musicclass):void {
			statedelay--; if(statedelay<=0) statedelay=0;
      if (statedelay <= 0) {
				switch(state){
          case 0:
            //Do nothing here! Standard game state
          break;
					case 1:
					  //Game initilisation
						state = 0;
          break;
					case 2:
					  //Opening cutscene
						advancetext = true;
						hascontrol = false;
						state = 3;
						dwgfx.createtextbox("To do: write quick", 50, 80, 164, 164, 255);
						      dwgfx.addline("intro to story!");
									//Oh no! what happen to rest of crew etc crash into dimension
					break;
					case 4:
					  //End of opening cutscene for now
						switch(controlstyle) {
							case 0: 
								dwgfx.createtextbox("  Swipe on the left side to move  ", -1, 195, 174, 174, 174);
							break;
							case 1: 
								dwgfx.createtextbox("  Touch the screen sides to move  ", -1, 195, 174, 174, 174);
							break;
							case 2: 
								dwgfx.createtextbox("  Tap the buttons to move  ", -1, 195, 174, 174, 174);
							break;
						}
						dwgfx.textboxtimer(60);
						state = 0;
					break;
					case 5:
					  //Demo over
						advancetext = true;
						hascontrol = false;
						/*dwgfx.createtextbox("   Prototype Complete    ", 50, 80, 164, 164, 255);
						      dwgfx.addline("Congrats! More Info Soon!");
						dwgfx.textboxcenter();
						*/
						
						startscript = true;	newscript="returntohub";
						obj.removetrigger(5);
						state = 6;
					break;
					case 7:
					  //End of opening cutscene for now
					  dwgfx.textboxremove();
					  hascontrol = true;
						advancetext = false;
						state = 0;
					break;
					case 8:
					  //Enter dialogue
						obj.removetrigger(8);
						if (obj.flags[13] == 0) {
							obj.changeflag(13, 1);
							if (mobilemenu) {
									dwgfx.createtextbox("  Tap the top right corner  ", -1, 155, 174, 174, 174);
											  dwgfx.addline("    to view map and crew");
							}else{
								dwgfx.createtextbox("  Press ENTER to view map  ", -1, 155, 174, 174, 174);
											dwgfx.addline("      and quicksave");
							}
							
							dwgfx.textboxtimer(60);
						}
						state = 0;
					break;
					
					case 9:
					  //Start SWN Minigame Mode B
						obj.removetrigger(9);
						
						swnmode = true;
				    swngame = 6; 
				    swndelay = 150;
    				swntimer = 60 * 30;
						
						//set the checkpoint in the middle of the screen
						savepoint = 0;
						savex = 148; 
						savey = 100;
						savegc = 0;
						saverx = roomx; savery = roomy;  
						savedir = 0;		
						
						state = 0;
					break;
					
					case 10:
					  //Start SWN Minigame Mode A
						obj.removetrigger(10);
						
						swnmode = true;
				    swngame = 4; 
				    swndelay = 150;
    				swntimer = 60 * 30;
						
						//set the checkpoint in the middle of the screen
						savepoint = 0;
						savex = 148; 
						savey = 100;
						savegc = 0;
						saverx = roomx; savery = roomy;  
						savedir = 0;		
						
						state = 0;
					break;
					
					case 11:
					  //Intermission 1 instructional textbox, depends on last saved
						dwgfx.textboxremovefast();
						dwgfx.createtextbox("   When you're NOT standing on   ", -1, 3, 174, 174, 174);
						if (dwgfx.flipmode) {
							if (lastsaved == 2) {       dwgfx.addline("   the ceiling, Vitellary will"); 
							}else if (lastsaved == 3) { dwgfx.addline("   the ceiling, Vermilion will");
							}else if (lastsaved == 4) { dwgfx.addline("   the ceiling, Verdigris will");
							}else if (lastsaved == 5) { dwgfx.addline("   the ceiling, Victoria will"); }
						}else{
							if (lastsaved == 2) {       dwgfx.addline("    the floor, Vitellary will"); 
							}else if (lastsaved == 3) { dwgfx.addline("    the floor, Vermilion will");
							}else if (lastsaved == 4) { dwgfx.addline("    the floor, Verdigris will");
							}else if (lastsaved == 5) { dwgfx.addline("    the floor, Victoria will"); }
						}
						
						      dwgfx.addline("     stop and wait for you.");
						dwgfx.textboxtimer(180);
						state = 0;
					break;
					case 12:
					  //Intermission 1 instructional textbox, depends on last saved
						obj.removetrigger(12);
						if (obj.flags[61] == 0) {
							obj.changeflag(61, 1);
							dwgfx.textboxremovefast();
							dwgfx.createtextbox("  You can't continue to the next   ", -1, 8, 174, 174, 174);
							if (lastsaved == 5) {
								dwgfx.addline("  room until she is safely across. ");
							}else {
								dwgfx.addline("  room until he is safely across.  ");
							}
							dwgfx.textboxtimer(120);
						}
						state = 0;
					break;					
					case 13:
					  //textbox removal
						obj.removetrigger(13);
						dwgfx.textboxremovefast();
						state = 0;
					break;					
					case 14:
					  //Intermission 1 instructional textbox, depends on last saved
						if (dwgfx.flipmode) {                
							                 dwgfx.createtextbox(" When you're standing on the ceiling, ", -1, 3, 174, 174, 174);
						}else{
						                      dwgfx.createtextbox(" When you're standing on the floor, ", -1, 3, 174, 174, 174);
						}
						if (lastsaved == 2) {       dwgfx.addline(" Vitellary will try to walk to you. "); 
						}else if (lastsaved == 3) { dwgfx.addline(" Vermilion will try to walk to you. ");
				    }else if (lastsaved == 4) { dwgfx.addline(" Verdigris will try to walk to you. ");
						}else if (lastsaved == 5) { dwgfx.addline(" Victoria will try to walk to you. ");}
						dwgfx.textboxtimer(280); 
						
						state = 0;
					break;
					
					case 15:
					  //leaving the naughty corner
						obj.entities[obj.getplayer()].tile = 0;
						state = 0;
					break; 
					case 16:
					  //entering the naughty corner
						if(obj.entities[obj.getplayer()].tile == 0){
						  obj.entities[obj.getplayer()].tile = 144;
							music.playef(2, 10);
						}
						state = 0;
					break;
					
					case 17:
					  //Arrow key tutorial
						/*
						obj.removetrigger(17);
						dwgfx.createtextbox(" If you prefer, you can press UP or ", -1, 195, 174, 174, 174);
						      dwgfx.addline("   DOWN instead of ACTION to flip.");
						dwgfx.textboxtimer(100);
						*/
						state = 0;
					break;
					
					case 20:
						if (obj.flags[1] == 0) {
							obj.changeflag(1, 1);
							state = 0;
							dwgfx.textboxremove();
						}
						obj.removetrigger(20);
					break;
					case 21:
						if (obj.flags[2] == 0) {
							obj.changeflag(2, 1);
							state = 0; 
							dwgfx.textboxremove();
						}
						obj.removetrigger(21);
					break;
					case 22:
						if (obj.flags[3] == 0) {
							dwgfx.textboxremovefast();
							obj.changeflag(3, 1);
							state = 0;
							
							switch(controlstyle) {
								case 0: case 2:
									dwgfx.createtextbox("  Tap on the right to flip  ", -1, 25, 174, 174, 174);
								break;
								case 1:
									dwgfx.createtextbox("  Tap both sides together to flip  ", -1, 25, 174, 174, 174);
								break;
							}
					    
							dwgfx.textboxtimer(60);
						}
						obj.removetrigger(22);
					break;
					
					case 30:
					  //Generic "run script"
						if (obj.flags[4] == 0) {
							obj.changeflag(4, 1);
							startscript = true;	newscript="firststeps";
							state = 0;
						}
						obj.removetrigger(30);
						state = 0;
					break;
					case 31:
						//state = 55;  statedelay = 50;
							state = 0; statedelay = 0;
						if (obj.flags[6] == 0) {
							obj.changeflag(6, 1);
							
							obj.changeflag(5, 1);
							startscript = true;	newscript="communicationstation";
							state = 0; statedelay = 0;
						}
						obj.removetrigger(31);
					break;
					case 32:
					  //Generic "run script"
						if (obj.flags[7] == 0) {
							obj.changeflag(7, 1);
							startscript = true;	newscript="teleporterback";
							state = 0;
						}
						obj.removetrigger(32);
						state = 0;
					break;
					case 33:
					  //Generic "run script"
						if (obj.flags[9] == 0) {
							obj.changeflag(9, 1);
							startscript = true;	newscript="rescueblue";
							state = 0;
						}
						obj.removetrigger(33);
						state = 0;
					break;
					case 34:
					  //Generic "run script"
						if (obj.flags[10] == 0) {
							obj.changeflag(10, 1);
							startscript = true;	newscript="rescueyellow";
							state = 0;
						}
						obj.removetrigger(34);
						state = 0;
					break;
					case 35:
					  //Generic "run script"
						if (obj.flags[11] == 0) {
							obj.changeflag(11, 1);
							startscript = true;	newscript="rescuegreen";
							state = 0;
						}
						obj.removetrigger(35);
						state = 0;
					break;
					case 36:
					  //Generic "run script"
						if (obj.flags[8] == 0) {
							obj.changeflag(8, 1);
							startscript = true;	newscript="rescuered";
							state = 0;
						}
						obj.removetrigger(36);
						state = 0;
					break;
					
					case 37:
					  //Generic "run script"
						if (companion == 0) {
							startscript = true;	newscript="int2_yellow";
							state = 0;
						}
						obj.removetrigger(37);
						state = 0;
					break;
					case 38:
					  //Generic "run script"
						if (companion == 0) {
							startscript = true;	newscript="int2_red";
							state = 0;
						}
						obj.removetrigger(38);
						state = 0;
					break;
					case 39:
					  //Generic "run script"
						if (companion == 0) {
							startscript = true;	newscript="int2_green";
							state = 0;
						}
						obj.removetrigger(39);
						state = 0;
					break;
					case 40:
					  //Generic "run script"
						if (companion == 0) {
							startscript = true;	newscript="int2_blue";
							state = 0;
						}
						obj.removetrigger(40);
						state = 0;
					break;
					
					case 41:
					  //Generic "run script"
						if (obj.flags[60] == 0) {
							obj.changeflag(60, 1);
							startscript = true;
							if (lastsaved == 2) { newscript = "int1yellow_2";
							}else if (lastsaved == 3) { newscript = "int1red_2";
							}else if (lastsaved == 4) { newscript = "int1green_2";
							}else if (lastsaved == 5) { newscript = "int1blue_2";}
							state = 0;
						}
						obj.removetrigger(41);
						state = 0;
					break;					
					case 42:
					  //Generic "run script"
						if (obj.flags[62] == 0) {
							obj.changeflag(62, 1);
							startscript = true;
							if (lastsaved == 2) { newscript = "int1yellow_3";
							}else if (lastsaved == 3) { newscript = "int1red_3";
							}else if (lastsaved == 4) { newscript = "int1green_3";
							}else if (lastsaved == 5) { newscript = "int1blue_3";}
							state = 0;
						}
						obj.removetrigger(42);
						state = 0;
					break;					
					case 43:
					  //Generic "run script"
						if (obj.flags[63] == 0) {
							obj.changeflag(63, 1);
							startscript = true;
							if (lastsaved == 2) { newscript = "int1yellow_4";
							}else if (lastsaved == 3) { newscript = "int1red_4";
							}else if (lastsaved == 4) { newscript = "int1green_4";
							}else if (lastsaved == 5) { newscript = "int1blue_4";}
							state = 0;
						}
						obj.removetrigger(43);
						state = 0;
					break;		
					case 44:
					  //Generic "run script"
						if (obj.flags[64] == 0) {
							obj.changeflag(64, 1);
							startscript = true;
							if (lastsaved == 2) { newscript = "int1yellow_5";
							}else if (lastsaved == 3) { newscript = "int1red_5";
							}else if (lastsaved == 4) { newscript = "int1green_5";
							}else if (lastsaved == 5) { newscript = "int1blue_5";}
							state = 0;
						}
						obj.removetrigger(44);
						state = 0;
					break;					
					case 45:
					  //Generic "run script"
						if (obj.flags[65] == 0) {
							obj.changeflag(65, 1);
							startscript = true;
							if (lastsaved == 2) { newscript = "int1yellow_6";
							}else if (lastsaved == 3) { newscript = "int1red_6";
							}else if (lastsaved == 4) { newscript = "int1green_6";
							}else if (lastsaved == 5) { newscript = "int1blue_6";}
							state = 0;
						}
						obj.removetrigger(45);
						state = 0;
					break;
					case 46:
					  //Generic "run script"
						if (obj.flags[66] == 0) {
							obj.changeflag(66, 1);
							startscript = true;
							if (lastsaved == 2) { newscript = "int1yellow_7";
							}else if (lastsaved == 3) { newscript = "int1red_7";
							}else if (lastsaved == 4) { newscript = "int1green_7";
							}else if (lastsaved == 5) { newscript = "int1blue_7";}
							state = 0;
						}
						obj.removetrigger(46);
						state = 0;
					break;
					
					case 47:
					  //Generic "run script"
						if (obj.flags[69] == 0) {
							obj.changeflag(69, 1);
							startscript = true;	newscript="trenchwarfare";
							state = 0;
						}
						obj.removetrigger(47);
						state = 0;
					break;
					case 48:
					  //Generic "run script"
						if (obj.flags[70] == 0) {
							obj.changeflag(70, 1);
							startscript = true;	newscript="trinketcollector";
							state = 0;
						}
						obj.removetrigger(48);
						state = 0;
					break;
					case 49:
					  //Start final level music
						if (obj.flags[71] == 0) {
							obj.changeflag(71, 1);
							music.niceplay(15); //Final level remix
							state = 0;
						}
						obj.removetrigger(49);
						state = 0;
					break;
					
					case 50:
					  music.playef(15, 10);
					  dwgfx.createtextbox("Help! Can anyone hear", 35, 15, 255, 134, 255);
						      dwgfx.addline("this message?");
						dwgfx.textboxtimer(60);
						state++; statedelay = 100;
					break;
					case 51:
					  music.playef(15, 10);
					  dwgfx.createtextbox("Verdigris? Are you out", 30, 12, 255, 134, 255);
						      dwgfx.addline("there? Are you ok?");
						dwgfx.textboxtimer(60);
						state++; statedelay = 100;
					break;
					case 52:
					  music.playef(15, 10);
					  dwgfx.createtextbox("Please help us! We've crashed", 5, 22, 255, 134, 255);
						      dwgfx.addline("and need assistance!");
						dwgfx.textboxtimer(60);
						state++; statedelay = 100;
					break;
					case 53:
					  music.playef(15, 10);
					  dwgfx.createtextbox("Hello? Anyone out there?", 40, 15, 255, 134, 255);
						dwgfx.textboxtimer(60);
						state++; statedelay = 100;
					break;
					case 54:
					  music.playef(15, 10);
					  dwgfx.createtextbox("This is Doctor Violet from the", 5, 8, 255, 134, 255);
						      dwgfx.addline("D.S.S. Souleye! Please respond!");
						dwgfx.textboxtimer(60);
						state++; statedelay = 100;
					break;
					case 55:
					  music.playef(15, 10);
					  dwgfx.createtextbox("Please... Anyone...", 45, 14, 255, 134, 255);
						dwgfx.textboxtimer(60);
						state++; statedelay = 100;
					break;
					case 56:
					  music.playef(15, 10);
					  dwgfx.createtextbox("Please be alright, everyone...", 25, 18, 255, 134, 255);
						dwgfx.textboxtimer(60);
						state=50; statedelay = 100;
					break;
					
					
					case 80: 
					  //Used to return to menu from the game	  
						if(dwgfx.fademode == 1)	state++;
					break;
					case 81: 					  
						gamestate = 1; dwgfx.fademode = 4;
						music.play(6); dwgfx.backgrounddrawn = false; map.tdrawback = true;
						dwgfx.flipmode = false;
						createmenu("mainmenu");
						state = 0;
					break;
					
					case 82: 
					  //Time Trial Complete!
						obj.removetrigger(82);
						hascontrol = false;
						timetrialresulttime = seconds + (minutes * 60);
						timetrialrank = 0;
						if (timetrialresulttime <= timetrialpar) timetrialrank++;
						if (trinkets >= timetrialshinytarget) timetrialrank++;
						if (deathcounts == 0) timetrialrank++;
						
						if (timetrialresulttime < besttimes[timetriallevel] || besttimes[timetriallevel]==-1) {
						  besttimes[timetriallevel] = timetrialresulttime;
						}
						if (trinkets > besttrinkets[timetriallevel] || besttrinkets[timetriallevel]==-1) {
							besttrinkets[timetriallevel] = trinkets;
						}
						if (deathcounts < bestlives[timetriallevel] || bestlives[timetriallevel]==-1) {
							bestlives[timetriallevel] = deathcounts;
						}
						if (timetrialrank > bestrank[timetriallevel] || bestrank[timetriallevel]==-1) {
							bestrank[timetriallevel] = timetrialrank;
							scores.reportAchievement(6 + timetriallevel);
						}
						
						savestats(map, dwgfx);
						
						dwgfx.fademode = 2;	music.fadeout();
						state++;
					break;
					case 83: 
					  frames--;
						if(dwgfx.fademode == 1)	state++;
					break;
					case 84: 					  
					  dwgfx.flipmode = false;
						gamestate = 1; dwgfx.fademode = 4;
						dwgfx.backgrounddrawn = true; map.tdrawback = true;
						createmenu("timetrialcomplete");
						state = 0;
					break;
					
					
					case 85: 
					  //Cutscene skip version of final level change
					  obj.removetrigger(85);
						//Init final stretch
						state++; music.playef(9, 10);
						music.play(2);
						obj.flags[72] = 1;
						
						screenshake = 10;
						flashlight = 5;
						map.finalstretch = true;
						map.warpx = false; map.warpy = false;
						map.background = 6;
						
						map.final_colormode = true; map.final_colorframe = 1;						
						
						state = 0;
					break;
					
					//From 90-100 are run scripts for the eurogamer expo only, remove later
					case 90:
					  //Generic "run script"
						startscript = true;	newscript="startexpolevel_station1";
						obj.removetrigger(90);
						state = 0;
					break;
					case 91:
					  //Generic "run script"
						startscript = true;	newscript="startexpolevel_lab";
						obj.removetrigger(91);
						state = 0;
					break;
					case 92:
					  //Generic "run script"
						startscript = true;	newscript="startexpolevel_warp";
						obj.removetrigger(92);
						state = 0;
					break;
					case 93:
					  //Generic "run script"
						startscript = true;	newscript="startexpolevel_tower";
						obj.removetrigger(93);
						state = 0;
					break;
					case 94:
					  //Generic "run script"
						startscript = true;	newscript="startexpolevel_station2";
						obj.removetrigger(94);
						state = 0;
					break;
					case 95:
					  //Generic "run script"
						startscript = true;	newscript="startexpolevel_final";
						obj.removetrigger(95);
						state = 0;
					break;
					
					case 96: 
					  //Used to return to gravitron to game	  
						if(dwgfx.fademode == 1)	state++;
					break;
					case 97: 					  
						gamestate = 0; dwgfx.fademode = 4;
						startscript = true;	newscript="returntolab";
						state = 0;
					break;
					
					case 100:
					  //
						//                       Meeting crewmate in the warpzone
						//
						obj.removetrigger(100);
						if (obj.flags[4] == 0) {
							obj.changeflag(4, 1);
							state++;
						}						
					break;
					case 101:
					  i = obj.getplayer();
						hascontrol = false;
						if (obj.entities[i].onroof > 0 && gravitycontrol == 1) {
							gravitycontrol = 0;	music.playef(1, 10);
						}
						if (obj.entities[i].onground > 0) {
							state++;
						}
					break;
					case 102:
					  companion = 6; 
						i = obj.getcompanion(6);	obj.entities[i].tile = 0;	obj.entities[i].state = 1;
						
						advancetext = true;	hascontrol = false;
						
						dwgfx.createtextbox("Captain! I've been so worried!", 60, 90, 164, 255, 164);
						state++; music.playef(12, 10);
					break;
					case 104:						
						dwgfx.createtextbox("I'm glad you're ok!", 135, 152, 164, 164, 255);
						state++; music.playef(11, 10);
						dwgfx.textboxactive();
					break;
					case 106:						
						dwgfx.createtextbox("I've been trying to find a", 74, 70, 164, 255, 164);
						      dwgfx.addline("way out, but I keep going");
									dwgfx.addline("around in circles...");
						state++; music.playef(2, 10);
						dwgfx.textboxactive();
						i = obj.getcompanion(6);
						obj.entities[i].tile = 54;	obj.entities[i].state = 0;
					break;
					case 108:						
						dwgfx.createtextbox("Don't worry! I have a", 125, 152, 164, 164, 255);
						      dwgfx.addline("teleporter key!");
						state++; music.playef(11, 10);
						dwgfx.textboxactive();
					break;
					case 110:						
					  i = obj.getcompanion(6);	obj.entities[i].tile = 0;	obj.entities[i].state = 1;
						dwgfx.createtextbox("Follow me!", 185, 154, 164, 164, 255);
						state++; music.playef(11, 10);
						dwgfx.textboxactive();
					break;
					case 112:
					  dwgfx.textboxremove();
					  hascontrol = true;
						advancetext = false;
						
						state = 0;
					break;
					
					case 115:
					  //
						//                       Test script for space station, totally delete me!
						//
					  i = obj.getplayer();
						hascontrol = false;
						state++;
					break;
					case 116:
						advancetext = true;	hascontrol = false;
						
						dwgfx.createtextbox("Sorry Eurogamers! Teleporting around", 60 - 20, 200, 255, 64, 64);
						      dwgfx.addline("the map doesn't work in this version!");
						dwgfx.textboxcenterx();
						state++;
					break;
					case 118:
					  dwgfx.textboxremove();
					  hascontrol = true;
						advancetext = false;
						
						state = 0;
					break;
					
					case 120:
					  //
						//                       Meeting crewmate in the space station
						//
						obj.removetrigger(120);
						if (obj.flags[5] == 0) {
							obj.changeflag(5, 1);
							state++;
						}						
					break;
					case 121:
					  i = obj.getplayer();
						hascontrol = false;
						if (obj.entities[i].onground > 0 && gravitycontrol == 0) {
							gravitycontrol = 1;	music.playef(1, 10);
						}
						if (obj.entities[i].onroof > 0) {
							state++;
						}
					break;
					case 122:
					  companion = 7; 
						i = obj.getcompanion(7);	obj.entities[i].tile = 6;	obj.entities[i].state = 1;
						
						advancetext = true;	hascontrol = false;
						
						dwgfx.createtextbox("Captain! You're ok!", 60-10, 90-40, 255, 255, 134);
						state++; music.playef(14, 10);
					break;
					case 124:						
						dwgfx.createtextbox("I've found a teleporter, but", 60-20, 90 - 40, 255, 255, 134);
						      dwgfx.addline("I can't get it to go anywhere...");
						state++; music.playef(2, 10);
						dwgfx.textboxactive();
						i = obj.getcompanion(7);	//obj.entities[i].tile = 66;	obj.entities[i].state = 0;
					break;
					case 126:						
						dwgfx.createtextbox("I can help with that!", 125, 152-40, 164, 164, 255);
						state++; music.playef(11, 10);
						dwgfx.textboxactive();
					break;
					case 128:						
						dwgfx.createtextbox("I have the teleporter", 130, 152-35, 164, 164, 255);
						      dwgfx.addline("codex for our ship!");
						state++; music.playef(11, 10);
						dwgfx.textboxactive();
					break;
					
					case 130:						
						dwgfx.createtextbox("Yey! Let's go home!", 60-30, 90-35, 255, 255, 134);
						state++; music.playef(14, 10);
						dwgfx.textboxactive();
						i = obj.getcompanion(7);	obj.entities[i].tile = 6;	obj.entities[i].state = 1;
					break;
					case 132:
					  dwgfx.textboxremove();
					  hascontrol = true;
						advancetext = false;
						
						state = 0;
					break;
					
					case 200:
					  //Init final stretch
						state++; music.playef(9, 10);
						//music.play(2);
						obj.flags[72] = 1;
						
						screenshake = 10;
						flashlight = 5;
						map.finalstretch = true;
						map.warpx = false; map.warpy = false;
						map.background = 6;
						
						map.final_colormode = true; map.final_colorframe = 1;
						
						startscript = true;	newscript="finalterminal_finish";
						state = 0;
					break;
					
					case 300:
            startscript = true;
            newscript="custom_"+customscript[0];
            obj.removetrigger(300);
            state = 0;
            break;
        case 301:
            startscript = true;
            newscript="custom_"+customscript[1];
            obj.removetrigger(301);
            state = 0;
            break;
        case 302:
            startscript = true;
            newscript="custom_"+customscript[2];
            obj.removetrigger(302);
            state = 0;
            break;
        case 303:
            startscript = true;
            newscript="custom_"+customscript[3];
            obj.removetrigger(303);
            state = 0;
            break;
        case 304:
            startscript = true;
            newscript="custom_"+customscript[4];
            obj.removetrigger(304);
            state = 0;
            break;
        case 305:
            startscript = true;
            newscript="custom_"+customscript[5];
            obj.removetrigger(305);
            state = 0;
            break;
        case 306:
            startscript = true;
            newscript="custom_"+customscript[6];
            obj.removetrigger(306);
            state = 0;
            break;
        case 307:
            startscript = true;
            newscript="custom_"+customscript[7];
            obj.removetrigger(307);
            state = 0;
            break;
        case 308:
            startscript = true;
            newscript="custom_"+customscript[8];
            obj.removetrigger(308);
            state = 0;
            break;
        case 309:
            startscript = true;
            newscript="custom_"+customscript[9];
            obj.removetrigger(309);
            state = 0;
            break;
        case 310:
            startscript = true;
            newscript="custom_"+customscript[10];
            obj.removetrigger(310);
            state = 0;
            break;
        case 311:
            startscript = true;
            newscript="custom_"+customscript[11];
            obj.removetrigger(311);
            state = 0;
            break;
        case 312:
            startscript = true;
            newscript="custom_"+customscript[12];
            obj.removetrigger(312);
            state = 0;
            break;
        case 313:
            startscript = true;
            newscript="custom_"+customscript[13];
            obj.removetrigger(313);
            state = 0;
            break;
        case 314:
            startscript = true;
            newscript="custom_"+customscript[14];
            obj.removetrigger(314);
            state = 0;
            break;
        case 315:
            startscript = true;
            newscript="custom_"+customscript[15];
            obj.removetrigger(315);
            state = 0;
            break;
        case 316:
            startscript = true;
            newscript="custom_"+customscript[16];
            obj.removetrigger(316);
            state = 0;
            break;
        case 317:
            startscript = true;
            newscript="custom_"+customscript[17];
            obj.removetrigger(317);
            state = 0;
            break;
        case 318:
            startscript = true;
            newscript="custom_"+customscript[18];
            obj.removetrigger(318);
            state = 0;
            break;
        case 319:
            startscript = true;
            newscript="custom_"+customscript[19];
            obj.removetrigger(319);
            state = 0;
            break;
        case 320:
            startscript = true;
            newscript="custom_"+customscript[20];
            obj.removetrigger(320);
            state = 0;
            break;
        case 321:
            startscript = true;
            newscript="custom_"+customscript[21];
            obj.removetrigger(321);
            state = 0;
            break;
        case 322:
            startscript = true;
            newscript="custom_"+customscript[22];
            obj.removetrigger(322);
            state = 0;
            break;
        case 323:
            startscript = true;
            newscript="custom_"+customscript[23];
            obj.removetrigger(323);
            state = 0;
            break;
        case 324:
            startscript = true;
            newscript="custom_"+customscript[24];
            obj.removetrigger(324);
            state = 0;
            break;
        case 325:
            startscript = true;
            newscript="custom_"+customscript[25];
            obj.removetrigger(325);
            state = 0;
            break;
        case 326:
            startscript = true;
            newscript="custom_"+customscript[26];
            obj.removetrigger(326);
            state = 0;
            break;
        case 327:
            startscript = true;
            newscript="custom_"+customscript[27];
            obj.removetrigger(327);
            state = 0;
            break;
        case 328:
            startscript = true;
            newscript="custom_"+customscript[28];
            obj.removetrigger(328);
            state = 0;
            break;
        case 329:
            startscript = true;
            newscript="custom_"+customscript[29];
            obj.removetrigger(329);
            state = 0;
            break;
        case 330:
            startscript = true;
            newscript="custom_"+customscript[30];
            obj.removetrigger(330);
            state = 0;
            break;
        case 331:
            startscript = true;
            newscript="custom_"+customscript[31];
            obj.removetrigger(331);
            state = 0;
            break;
        case 332:
            startscript = true;
            newscript="custom_"+customscript[32];
            obj.removetrigger(332);
            state = 0;
            break;
        case 333:
            startscript = true;
            newscript="custom_"+customscript[33];
            obj.removetrigger(333);
            state = 0;
            break;
        case 334:
            startscript = true;
            newscript="custom_"+customscript[34];
            obj.removetrigger(334);
            state = 0;
            break;
        case 335:
            startscript = true;
            newscript="custom_"+customscript[35];
            obj.removetrigger(335);
            state = 0;
            break;
        case 336:
            startscript = true;
            newscript="custom_"+customscript[36];
            obj.removetrigger(336);
            state = 0;
            break;
					
					case 1000:
						dwgfx.showcutscenebars = true;
						hascontrol = false;
						completestop = true;
						state++; statedelay = 15;
					break;
					case 1001:
					  //Found a trinket!
					  advancetext = true;
						state++;
						if (dwgfx.flipmode) {
							dwgfx.createtextbox("        Congratulations!       ", 50, 105, 174, 174, 174);
										dwgfx.addline("");
										dwgfx.addline("You have found a shiny trinket!");
							dwgfx.textboxcenterx();
							
							if (map.custommode) {
								dwgfx.createtextbox(" " + help.number(trinkets) + " out of " + help.number(map.customtrinkets), 50, 65, 174, 174, 174);
							}else{
								dwgfx.createtextbox(" " + help.number(trinkets) + " out of Twenty ", 50, 65, 174, 174, 174);
							}
							dwgfx.textboxcenterx();
						}else{
							dwgfx.createtextbox("        Congratulations!       ", 50, 85, 174, 174, 174);
										dwgfx.addline("");
										dwgfx.addline("You have found a shiny trinket!");
							dwgfx.textboxcenterx();
							
							if (map.custommode) {
								dwgfx.createtextbox(" " + help.number(trinkets) + " out of " + help.number(map.customtrinkets), 50, 135, 174, 174, 174);
							}else{
								dwgfx.createtextbox(" " + help.number(trinkets) + " out of Twenty ", 50, 135, 174, 174, 174);
							}
							dwgfx.textboxcenterx();
						}
					break;
					case 1003:
					  dwgfx.textboxremove();
					  hascontrol = true;
						advancetext = false; completestop = false;
						state = 0;
						//music.play(music.resumesong);
						music.musicfadein = 90;
						dwgfx.showcutscenebars = false;
					break;
					
					case 1010:
            dwgfx.showcutscenebars = true;
            hascontrol = false;
            completestop = true;
            state++;
            statedelay = 15;
            break;
        case 1011:
            //Found a trinket!
            advancetext = true;
            state++;
            if (dwgfx.flipmode)
            {
                dwgfx.createtextbox("        Congratulations!       ", 50, 105, 174, 174, 174);
                dwgfx.addline("");
                dwgfx.addline("You have found a lost crewmate!");
                dwgfx.textboxcenterx();

                if(int(map.customcrewmates-crewmates)==0)
                {
                    dwgfx.createtextbox("     All crewmates rescued!    ", 50, 135, 174, 174, 174);
                }
                else if(map.customcrewmates-crewmates==1)
                {
                    dwgfx.createtextbox("    " + help.number(int(map.customcrewmates-crewmates))+ " remains    ", 50, 135, 174, 174, 174);
                }
                else
                {
                    dwgfx.createtextbox("     " + help.number(int(map.customcrewmates-crewmates))+ " remain    ", 50, 135, 174, 174, 174);
                }
                dwgfx.textboxcenterx();

            }
            else
            {
                dwgfx.createtextbox("        Congratulations!       ", 50, 85, 174, 174, 174);
                dwgfx.addline("");
                dwgfx.addline("You have found a lost crewmate!");
                dwgfx.textboxcenterx();

                if(int(map.customcrewmates-crewmates)==0)
                {
                    dwgfx.createtextbox("     All crewmates rescued!    ", 50, 135, 174, 174, 174);
                }
                else if(map.customcrewmates-crewmates==1)
                {
                    dwgfx.createtextbox("    " + help.number(int(map.customcrewmates-crewmates))+ " remains    ", 50, 135, 174, 174, 174);
                }
                else
                {
                    dwgfx.createtextbox("     " + help.number(int(map.customcrewmates-crewmates))+ " remain    ", 50, 135, 174, 174, 174);
                }
                dwgfx.textboxcenterx();
            }
            break;
        case 1013:
            dwgfx.textboxremove();
            hascontrol = true;
            advancetext = false;
            completestop = false;
            state = 0;

            if(map.customcrewmates-crewmates==0)
            {
                if(map.custommodeforreal)
                {
                    dwgfx.fademode = 2;
                    if(!muted && editor.levmusic>0) music.musicfadein = 90;
                    if(editor.levmusic>0) music.fadeout();
                    state=1014;
                }
                else
                {
                    gamestate = EDITORMODE;
                    dwgfx.backgrounddrawn=false;
                    if(!muted && editor.levmusic>0) music.musicfadein = 90;
                    if(editor.levmusic>0) music.fadeout();
                }
            }
            else
            {
                if(!muted && editor.levmusic>0) music.musicfadein = 90;
            }
            dwgfx.showcutscenebars = false;
            break;
        case 1014:
            frames--;
            if(dwgfx.fademode == 1)	state++;
            break;
        case 1015:
            dwgfx.flipmode = false;
            gamestate = TITLEMODE;
            dwgfx.fademode = 4;
            music.play(6);
            dwgfx.backgrounddrawn = true;
            map.tdrawback = true;
            //Update level stats
            if(map.customcrewmates-crewmates==0)
            {
                //Finished level
                if(map.customtrinkets-trinkets==0)
                {
                    //and got all the trinkets!
                    updatecustomlevelstats(customlevelfilename, 3);
                }
                else
                {
                    updatecustomlevelstats(customlevelfilename, 1);
                }
            }
            createmenu("levellist");
            state = 0;
            break;
						
					case 2000:
						//Game Saved!
						if (intimetrial || nodeathmode || inintermission) {
							state = 0;
						}else{
							savetele(map, obj, music, help);
							if (dwgfx.flipmode) {
								dwgfx.createtextbox("    Game Saved    ", -1, 202, 174, 174, 174);
								dwgfx.textboxtimer(25);
							}else{
								dwgfx.createtextbox("    Game Saved    ", -1, 12, 174, 174, 174);
								dwgfx.textboxtimer(25);
							}
							state = 0;
						}
					break;
					
					case 2500:
					
						music.play(5);
					  //Activating a teleporter (appear)
						state++; statedelay = 15;
						flashlight = 5;
						screenshake = 90;
						music.playef(9, 10);
					break;
					case 2501:
					  //Activating a teleporter 2
						state++; statedelay = 0;
						flashlight = 5; screenshake = 0;
						//we're done here!
						music.playef(10, 10);
					break;
					case 2502:
					  //Activating a teleporter 2
						state++; statedelay = 5;
						
						i = obj.getplayer(); 
						obj.entities[i].colour = 0;
						obj.entities[i].invis = false;
						
						obj.entities[i].xp = obj.entities[obj.getteleporter()].xp+44;
						obj.entities[i].yp = obj.entities[obj.getteleporter()].yp+44;
						obj.entities[i].ay = -6;
						obj.entities[i].ax = 6;
						obj.entities[i].vy = -6;
						obj.entities[i].vx = 6;
						
						i = obj.getteleporter();
						obj.entities[i].tile = 1;
						obj.entities[i].colour = 101;
					break;
					case 2503:
						state++; 
						i = obj.getplayer(); 
						obj.entities[i].xp += 10;
					break;
					case 2504:
						state++; 
						i = obj.getplayer(); 
						//obj.entities[i].xp += 10;
					break;
					case 2505:
						state++; 
						i = obj.getplayer(); 
						obj.entities[i].xp += 8;
					break;
					case 2506:
						state++; 
						i = obj.getplayer(); 
						obj.entities[i].xp += 6;
					break;
					case 2507:
						state++; 
						i = obj.getplayer(); 
						//obj.entities[i].xp += 4;
					break;
					case 2508:
						state++; 
						i = obj.getplayer(); 
						obj.entities[i].xp += 2;
					break;
					case 2509:
						state++; statedelay = 15;
						i = obj.getplayer(); 
						obj.entities[i].xp += 1;
					break;
					case 2510:			
						advancetext = true;	hascontrol = false;
						dwgfx.createtextbox("Hello?", 125+24, 152-20, 164, 164, 255);
						state++; music.playef(11, 10);
						dwgfx.textboxactive();
					break;
					case 2512:			
						advancetext = true;	hascontrol = false;
						dwgfx.createtextbox("Is anyone there?", 125+8, 152-24, 164, 164, 255);
						state++; music.playef(11, 10);
						dwgfx.textboxactive();
					break;
					case 2514:								 
					  dwgfx.textboxremove();
					  hascontrol = true;
						advancetext = false;
						
						state = 0;
						music.play(3); 
					break;
					
					
					case 3000:
					  //Activating a teleporter (long version for level complete)
						state++; statedelay = 30;
						flashlight = 5;
						screenshake = 90;
						music.playef(9, 10);
					break;
					case 3001:
					  //Activating a teleporter 2
						state++; statedelay = 15;
						flashlight = 5;
						music.playef(9, 10);
					break;
					case 3002:
					  //Activating a teleporter 2
						state++; statedelay = 15;
						flashlight = 5;
						music.playef(9, 10);
					break;
					case 3003:
					  //Activating a teleporter 2
						state++; statedelay = 15;
						flashlight = 5;
						music.playef(9, 10);
					break;
					case 3004:
					  //Activating a teleporter 2
						state++; statedelay = 0;
						flashlight = 5; screenshake = 0;
						//we're done here!
						music.playef(10, 10);
					break;
					case 3005:
					  //Activating a teleporter 2
						state++; statedelay = 50;
						//testing!
						//state = 3006; //Warp Zone
						//state = 3020; //Space Station
						switch(companion) {
							case 6: state = 3006; break; //Warp Zone
							case 7: state = 3020; break; //Space Station
							case 8: state = 3040; break; //Lab
							case 9: state = 3060; break; //Tower
							case 10: state = 3080; break; //Intermission 2
							case 11: state = 3085; break; //Intermission 1
						}
						
						i = obj.getplayer(); 
						obj.entities[i].colour = 0;
						obj.entities[i].invis = true;
						
						i = obj.getcompanion(companion); 
						if(i>-1){
						  obj.entities[i].active = false;
						}
						
						i = obj.getteleporter();
						obj.entities[i].tile = 1;
						obj.entities[i].colour = 100;
					break;
					
					case 3006:
					  //Level complete! (warp zone)
						unlocknum(4, map, dwgfx);
						lastsaved = 4;
						music.play(0);
						state++; statedelay = 75;
						
						if (dwgfx.flipmode) { dwgfx.createtextbox("", -1, 180, 165, 165, 255); 
						}else{ dwgfx.createtextbox("", -1, 12, 165, 165, 255); }
						//dwgfx.addline("      Level Complete!      ");
						dwgfx.addline("                                   ");
						dwgfx.addline("");
						dwgfx.addline("");
						dwgfx.textboxcenterx();
						
/*												advancetext = true;
						hascontrol = false;
						state = 3;
						dwgfx.createtextbox("To do: write quick", 50, 80, 164, 164, 255);
						      dwgfx.addline("intro to story!");*/
					break;
					case 3007:
					  state++; statedelay = 45;
						
						if (dwgfx.flipmode) { dwgfx.createtextbox("", -1, 104, 175,174,174);
						}else { dwgfx.createtextbox("", -1, 64+8+16, 175,174,174); }
						dwgfx.addline("     You have rescued  ");
						dwgfx.addline("      a crew member!   ");
						dwgfx.addline("");
						dwgfx.textboxcenterx();
					break;
					case 3008:
					  state++; statedelay = 45;
						
						temp = 6 - crewrescued();
						if (temp == 1) {
							tempstring = "  One remains  ";
							if (dwgfx.flipmode) { dwgfx.createtextbox(tempstring, -1, 72, 174, 174, 174);
						  }else { dwgfx.createtextbox(tempstring, -1, 128+16, 174, 174, 174); }
						}else if (temp > 0) {
							tempstring = "  " + help.number(temp) + " remain  ";
							if (dwgfx.flipmode) { dwgfx.createtextbox(tempstring, -1, 72, 174, 174, 174);
						  }else { dwgfx.createtextbox(tempstring, -1, 128+16, 174, 174, 174); }
						}else {
							if (dwgfx.flipmode) { dwgfx.createtextbox("  All Crew Members Rescued!  ", -1, 72, 174, 174, 174);
						  }else { dwgfx.createtextbox("  All Crew Members Rescued!  ", -1, 128+16, 174, 174, 174); }
						}
						dwgfx.textboxcenterx();
					break;
					case 3009:
					  state++; statedelay = 0;
						
						if (dwgfx.flipmode) { dwgfx.createtextbox(" Tap screen to continue ", -1, 20, 164, 164, 255);
						}else { dwgfx.createtextbox(" Tap screen to continue ", -1, 196, 164, 164, 255); }
						dwgfx.textboxcenterx();
					break;
					case 3010:
					  if (jumppressed) {
							state++; statedelay = 30;
							dwgfx.textboxremove();
						}
					break;	
					case 3011:
						state = 3070; statedelay = 0;
					break;
					
					case 3020:
					  //Level complete! (Space Station 2)
						unlocknum(3, map, dwgfx);
						lastsaved = 2;
						music.play(0);
						state++; statedelay = 75;
						
						
						if (dwgfx.flipmode) { dwgfx.createtextbox("", -1, 180, 165, 165, 255); 
						}else{ dwgfx.createtextbox("", -1, 12, 165, 165, 255); }
						//dwgfx.addline("      Level Complete!      ");
						dwgfx.addline("                                   ");
						dwgfx.addline("");
						dwgfx.addline("");
						dwgfx.textboxcenterx();
						
/*												advancetext = true;
						hascontrol = false;
						state = 3;
						dwgfx.createtextbox("To do: write quick", 50, 80, 164, 164, 255);
						      dwgfx.addline("intro to story!");*/
					break;
					case 3021:
					  state++; statedelay = 45;
						
						if (dwgfx.flipmode) { dwgfx.createtextbox("", -1, 104, 174,175,174);
						}else { dwgfx.createtextbox("", -1, 64+8+16, 174,175,174); }
						dwgfx.addline("     You have rescued  ");
						dwgfx.addline("      a crew member!   ");
						dwgfx.addline("");
						dwgfx.textboxcenterx();
					break;
					case 3022:
					  state++; statedelay = 45;
						
						temp = 6 - crewrescued();
						if (temp == 1) {
							tempstring = "  One remains  ";
							if (dwgfx.flipmode) { dwgfx.createtextbox(tempstring, -1, 72, 174, 174, 174);
						  }else { dwgfx.createtextbox(tempstring, -1, 128+16, 174, 174, 174); }
						}else if (temp > 0) {
							tempstring = "  " + help.number(temp) + " remain  ";
							if (dwgfx.flipmode) { dwgfx.createtextbox(tempstring, -1, 72, 174, 174, 174);
						  }else { dwgfx.createtextbox(tempstring, -1, 128+16, 174, 174, 174); }
						}else {
							if (dwgfx.flipmode) { dwgfx.createtextbox("  All Crew Members Rescued!  ", -1, 72, 174, 174, 174);
						  }else { dwgfx.createtextbox("  All Crew Members Rescued!  ", -1, 128+16, 174, 174, 174); }
						}
						dwgfx.textboxcenterx();
					break;
					case 3023:
					  state++; statedelay = 0;
						
						if (dwgfx.flipmode) { dwgfx.createtextbox(" Tap screen to continue ", -1, 20, 164, 164, 255);
						}else { dwgfx.createtextbox(" Tap screen to continue ", -1, 196, 164, 164, 255); }
						dwgfx.textboxcenterx();
					break;
					case 3024:
					  if (jumppressed) {
							state++; statedelay = 30;
							dwgfx.textboxremove();
						}
					break;	
					case 3025:
						state = 3070; statedelay = 0;
					break;
					
					case 3040:
					  //Level complete! (Lab)
						unlocknum(1, map, dwgfx);
						lastsaved = 5;
						music.play(0);
						state++; statedelay = 75;
						
						if (dwgfx.flipmode) { dwgfx.createtextbox("", -1, 180, 165, 165, 255); 
						}else{ dwgfx.createtextbox("", -1, 12, 165, 165, 255); }
						//dwgfx.addline("      Level Complete!      ");
						dwgfx.addline("                                   ");
						dwgfx.addline("");
						dwgfx.addline("");
						dwgfx.textboxcenterx();
						
/*												advancetext = true;
						hascontrol = false;
						state = 3;
						dwgfx.createtextbox("To do: write quick", 50, 80, 164, 164, 255);
						      dwgfx.addline("intro to story!");*/
					break;
					case 3041:
					  state++; statedelay = 45;
						
						if (dwgfx.flipmode) { dwgfx.createtextbox("", -1, 104, 174,174,175);
						}else { dwgfx.createtextbox("", -1, 64+8+16, 174,174,175); }
						dwgfx.addline("     You have rescued  ");
						dwgfx.addline("      a crew member!   ");
						dwgfx.addline("");
						dwgfx.textboxcenterx();
					break;
					case 3042:
					  state++; statedelay = 45;
						
						temp = 6 - crewrescued();
						if (temp == 1) {
							tempstring = "  One remains  ";
							if (dwgfx.flipmode) { dwgfx.createtextbox(tempstring, -1, 72, 174, 174, 174);
						  }else { dwgfx.createtextbox(tempstring, -1, 128+16, 174, 174, 174); }
						}else if (temp > 0) {
							tempstring = "  " + help.number(temp) + " remain  ";
							if (dwgfx.flipmode) { dwgfx.createtextbox(tempstring, -1, 72, 174, 174, 174);
						  }else { dwgfx.createtextbox(tempstring, -1, 128+16, 174, 174, 174); }
						}else {
							if (dwgfx.flipmode) { dwgfx.createtextbox("  All Crew Members Rescued!  ", -1, 72, 174, 174, 174);
						  }else { dwgfx.createtextbox("  All Crew Members Rescued!  ", -1, 128+16, 174, 174, 174); }
						}
						dwgfx.textboxcenterx();
					break;
					case 3043:
					  state++; statedelay = 0;
						
						if (dwgfx.flipmode) { dwgfx.createtextbox(" Tap screen to continue ", -1, 20, 164, 164, 255);
						}else { dwgfx.createtextbox(" Tap screen to continue ", -1, 196, 164, 164, 255); }
						dwgfx.textboxcenterx();
					break;
					case 3044:
					  if (jumppressed) {
							state++; statedelay = 30;
							dwgfx.textboxremove();
						}
					break;	
					case 3045:
						state = 3070; statedelay = 0;
					break;
					
					case 3050:
					  //Level complete! (Space Station 1)
						unlocknum(0, map, dwgfx);
						lastsaved = 1;
						music.play(0);
						state++; statedelay = 75;
						
						
						if (dwgfx.flipmode) { dwgfx.createtextbox("", -1, 180, 165, 165, 255); 
						}else{ dwgfx.createtextbox("", -1, 12, 165, 165, 255); }
						//dwgfx.addline("      Level Complete!      ");
						dwgfx.addline("                                   ");
						dwgfx.addline("");
						dwgfx.addline("");
						dwgfx.textboxcenterx();
						
/*												advancetext = true;
						hascontrol = false;
						state = 3;
						dwgfx.createtextbox("To do: write quick", 50, 80, 164, 164, 255);
						      dwgfx.addline("intro to story!");*/
					break;
					case 3051:
					  state++; statedelay = 45;
						
						if (dwgfx.flipmode) { dwgfx.createtextbox("", -1, 104, 175,175,174);
						}else { dwgfx.createtextbox("", -1, 64+8+16, 175,175,174); }
						dwgfx.addline("     You have rescued  ");
						dwgfx.addline("      a crew member!   ");
						dwgfx.addline("");
						dwgfx.textboxcenterx();
					break;
					case 3052:
					  state++; statedelay = 45;
						
						temp = 6 - crewrescued();
						if (temp == 1) {
							tempstring = "  One remains  ";
							if (dwgfx.flipmode) { dwgfx.createtextbox(tempstring, -1, 72, 174, 174, 174);
						  }else { dwgfx.createtextbox(tempstring, -1, 128+16, 174, 174, 174); }
						}else if (temp > 0) {
							tempstring = "  " + help.number(temp) + " remain  ";
							if (dwgfx.flipmode) { dwgfx.createtextbox(tempstring, -1, 72, 174, 174, 174);
						  }else { dwgfx.createtextbox(tempstring, -1, 128+16, 174, 174, 174); }
						}else {
							if (dwgfx.flipmode) { dwgfx.createtextbox("  All Crew Members Rescued!  ", -1, 72, 174, 174, 174);
						  }else { dwgfx.createtextbox("  All Crew Members Rescued!  ", -1, 128+16, 174, 174, 174); }
						}
						dwgfx.textboxcenterx();
					break;
					case 3053:
					  state++; statedelay = 0;
						
						if (dwgfx.flipmode) { dwgfx.createtextbox(" Tap screen to continue ", -1, 20, 164, 164, 255);
						}else { dwgfx.createtextbox(" Tap screen to continue ", -1, 196, 164, 164, 255); }
						dwgfx.textboxcenterx();
					break;
					case 3054:
					  if (jumppressed) {
							state++; statedelay = 30;
							dwgfx.textboxremove();
							crewstats[1] = 0; //Set violet's rescue script to 0 to make the next bit easier
							teleportscript = "";
						}
					break;	
					case 3055:
					  dwgfx.fademode = 2;
					  state++; statedelay = 10;
					break;	
					case 3056:
					  if(dwgfx.fademode==1){
							startscript = true;	
							if (nocutscenes) {
								newscript="bigopenworldskip";
							}else{
							  newscript = "bigopenworld";
							}
					    state = 0;
						}
					break;
					
										
					case 3060:
					  //Level complete! (Tower)
						unlocknum(2, map, dwgfx);
						lastsaved = 3;
						music.play(0);
						state++; statedelay = 75;
						
						if (dwgfx.flipmode) { dwgfx.createtextbox("", -1, 180, 165, 165, 255); 
						}else{ dwgfx.createtextbox("", -1, 12, 165, 165, 255); }
						//dwgfx.addline("      Level Complete!      ");
						dwgfx.addline("                                   ");
						dwgfx.addline("");
						dwgfx.addline("");
						dwgfx.textboxcenterx();
						
/*												advancetext = true;
						hascontrol = false;
						state = 3;
						dwgfx.createtextbox("To do: write quick", 50, 80, 164, 164, 255);
						      dwgfx.addline("intro to story!");*/
					break;
					case 3061:
					  state++; statedelay = 45;
						
						if (dwgfx.flipmode) { dwgfx.createtextbox("", -1, 104, 175,174,175);
						}else { dwgfx.createtextbox("", -1, 64+8+16, 175,174,175); }
						dwgfx.addline("     You have rescued  ");
						dwgfx.addline("      a crew member!   ");
						dwgfx.addline("");
						dwgfx.textboxcenterx();
					break;
					case 3062:
					  state++; statedelay = 45;
						
						temp = 6 - crewrescued();
						if (temp == 1) {
							tempstring = "  One remains  ";
							if (dwgfx.flipmode) { dwgfx.createtextbox(tempstring, -1, 72, 174, 174, 174);
						  }else { dwgfx.createtextbox(tempstring, -1, 128+16, 174, 174, 174); }
						}else if (temp > 0) {
							tempstring = "  " + help.number(temp) + " remain  ";
							if (dwgfx.flipmode) { dwgfx.createtextbox(tempstring, -1, 72, 174, 174, 174);
						  }else { dwgfx.createtextbox(tempstring, -1, 128+16, 174, 174, 174); }
						}else {
							if (dwgfx.flipmode) { dwgfx.createtextbox("  All Crew Members Rescued!  ", -1, 72, 174, 174, 174);
						  }else { dwgfx.createtextbox("  All Crew Members Rescued!  ", -1, 128+16, 174, 174, 174); }
						}
						dwgfx.textboxcenterx();
					break;
					case 3063:
					  state++; statedelay = 0;
						
						if (dwgfx.flipmode) { dwgfx.createtextbox(" Tap screen to continue ", -1, 20, 164, 164, 255);
						}else { dwgfx.createtextbox(" Tap screen to continue ", -1, 196, 164, 164, 255); }
						dwgfx.textboxcenterx();
					break;
					case 3064:
					  if (jumppressed) {
							state++; statedelay = 30;
							dwgfx.textboxremove();
						}
					break;	
					case 3065:
						state = 3070; statedelay = 0;
					break;
					
					
					case 3070:
						dwgfx.fademode = 2;
						state++;
					break;
					case 3071:
					  if (dwgfx.fademode == 1) state++;
					break;
					case 3072:
					  //Ok, we need to adjust some flags based on who've we've rescued. Some of there conversation options
						//change depending on when they get back to the ship.	
						if (lastsaved == 2) {
							if (crewstats[3]) obj.flags[25] = 1;
							if (crewstats[4]) obj.flags[26] = 1;
							if (crewstats[5]) obj.flags[24] = 1;
						}else if (lastsaved == 3) {
							if (crewstats[2]) obj.flags[50] = 1;
							if (crewstats[4]) obj.flags[49] = 1;
							if (crewstats[5]) obj.flags[48] = 1;
						}else if (lastsaved == 4) {
							if (crewstats[2]) obj.flags[54] = 1;
							if (crewstats[3]) obj.flags[55] = 1;
							if (crewstats[5]) obj.flags[56] = 1;
						}else if (lastsaved == 5) {
							if (crewstats[2]) obj.flags[37] = 1;
							if (crewstats[3]) obj.flags[38] = 1;
							if (crewstats[4]) obj.flags[39] = 1;
						}
					  //We're pitch black now, make a decision
						companion = 0;
						if (crewrescued() == 6) {
							startscript = true;	newscript="startlevel_final";
					    state = 0;
						}else if (crewrescued() == 4) {
							companion = 11;
				      supercrewmate = true;
				      scmprogress = 0;
				
							startscript = true;	newscript = "intermission_1";
							obj.flags[19] = 1;
							if (lastsaved == 2) obj.flags[32] = 1;
							if (lastsaved == 3) obj.flags[35] = 1;
							if (lastsaved == 4) obj.flags[34] = 1;
							if (lastsaved == 5) obj.flags[33] = 1;
					    state = 0;
						}else if (crewrescued() == 5) {
							startscript = true;	newscript = "intermission_2";
							obj.flags[20] = 1;
							if (lastsaved == 2) obj.flags[32] = 1;
							if (lastsaved == 3) obj.flags[35] = 1;
							if (lastsaved == 4) obj.flags[34] = 1;
							if (lastsaved == 5) obj.flags[33] = 1;
					    state = 0;
						}else {
							startscript = true;	newscript="regularreturn";
					    state = 0;
						}
					break;
					
					case 3080:
					  //returning from an intermission, very like 3070
						if (inintermission) {
							dwgfx.fademode = 2;
							companion = 0; 
							state=3100;
						}else{
							unlocknum(7, map, dwgfx);
							dwgfx.fademode = 2;
							companion = 0;
							state++;
						}
					break;
					case 3081:
					  if (dwgfx.fademode == 1) state++;
					break;
					case 3082:
					  map.finalmode = false;
						startscript = true;	newscript="regularreturn";
					  state = 0;
					break;
					
					case 3085:
					  //returning from an intermission, very like 3070
						//return to menu from here 
						if (inintermission) {
							companion = 0;
							supercrewmate = false;
							state++;
							dwgfx.fademode = 2;	music.fadeout();
						  state=3100;
						}else{
							unlocknum(6, map, dwgfx);
							dwgfx.fademode = 2;
							companion = 0;
							supercrewmate = false;
							state++;
						}
					break;
					case 3086:
					  if (dwgfx.fademode == 1) state++;
					break;
					case 3087:
					  map.finalmode = false;
						startscript = true;	newscript="regularreturn";
					  state = 0;
					break;
					
					case 3100:
						if(dwgfx.fademode == 1)	state++;
					break;
					case 3101:
					  dwgfx.flipmode = false;
						gamestate = 1; dwgfx.fademode = 4;
						dwgfx.backgrounddrawn = true; map.tdrawback = true;
						createmenu("play");
						music.play(6);
						state = 0;
					break;
					
					//startscript = true;	newscript="returntohub";
					//state = 0;
					
					/*case 3025:
						if (recording == 1) {
							//if recording the input, output it to debug here
							trace(recordstring);
							help.toclipboard(recordstring);
						}
						test = true; teststring = recordstring;
					  dwgfx.createtextbox("   Congratulations!    ", 50, 80, 164, 164, 255);
						      dwgfx.addline("");
									dwgfx.addline("Your play of this level has");
									dwgfx.addline("been copied to the clipboard.");
									dwgfx.addline("");
									dwgfx.addline("Please consider pasting and");
									dwgfx.addline("sending it to me! Even if you");
									dwgfx.addline("made a lot of mistakes - knowing");
									dwgfx.addline("exactly where people are having");
									dwgfx.addline("trouble is extremely useful!");
						dwgfx.textboxcenter();
						state = 0;
					break;*/
					
					case 3500:
						music.fadeout();
						state++; statedelay = 120;
						//state = 3511; //testing
					break;
					case 3501:
					  //Game complete! 
						scores.reportAchievement(scores.vvvvvvgamecomplete);
						unlocknum(5, map, dwgfx);
						crewstats[0] = true;
						state++; statedelay = 75;
						music.play(7);
						
						if (dwgfx.flipmode) { dwgfx.createtextbox("", -1, 180, 164, 165, 255); 
						}else{ dwgfx.createtextbox("", -1, 12, 164, 165, 255); }
						dwgfx.addline("                                   ");
						dwgfx.addline("");
						dwgfx.addline("");
						dwgfx.textboxcenterx();
					break;
					case 3502:
					  state++; statedelay = 45+15;
						
						if (dwgfx.flipmode) { dwgfx.createtextbox("  All Crew Members Rescued!  ", -1, 175-24, 0, 0, 0);
						}else{ dwgfx.createtextbox("  All Crew Members Rescued!  ", -1, 64, 0, 0, 0);}
						savetime = timestring(help);
					break;
					case 3503:
					  state++; statedelay = 45;
						
						tempstring = help.number(trinkets);
						if (dwgfx.flipmode) {
							dwgfx.createtextbox("Trinkets Found:", 48, 155-24, 0,0,0);
							dwgfx.createtextbox(tempstring, 180, 155-24, 0, 0, 0);
						}else{
							dwgfx.createtextbox("Trinkets Found:", 48, 84, 0,0,0);
							dwgfx.createtextbox(tempstring, 180, 84, 0, 0, 0);
						}
					break;
					case 3504:
					  state++; statedelay = 45+15;
						
						tempstring = String(savetime);
						if (dwgfx.flipmode) { 
						  dwgfx.createtextbox("   Game Time:", 64, 143-24, 0,0,0);
						  dwgfx.createtextbox(tempstring, 180, 143-24, 0, 0, 0);							
						}else{
						  dwgfx.createtextbox("   Game Time:", 64, 96, 0,0,0);
						  dwgfx.createtextbox(tempstring, 180, 96, 0, 0, 0);
						}
					break;
					case 3505:
					  state++; statedelay = 45;
						
						if (dwgfx.flipmode) { 
							dwgfx.createtextbox(" Total Flips:", 64, 116-24, 0,0,0);
							dwgfx.createtextbox(String(totalflips), 180, 116-24, 0, 0, 0);							
						}else{
							dwgfx.createtextbox(" Total Flips:", 64, 123, 0,0,0);
							dwgfx.createtextbox(String(totalflips), 180, 123, 0, 0, 0);
						}
					break;
					case 3506:
					  state++; statedelay = 45+15;
						
						if (dwgfx.flipmode) { 
							dwgfx.createtextbox("Total Deaths:", 64, 104-24, 0,0,0);
							dwgfx.createtextbox(String(deathcounts), 180, 104-24, 0, 0, 0);							
						}else{
							dwgfx.createtextbox("Total Deaths:", 64, 135, 0,0,0);
							dwgfx.createtextbox(String(deathcounts), 180, 135, 0, 0, 0);
						}
					break;
					case 3507:
					  state++; statedelay = 45+15;
						
						if (dwgfx.flipmode) { 
							tempstring = "Hardest Room (with " + String(hardestroomdeaths) + " deaths)";
							dwgfx.createtextbox(tempstring, -1, 81-24, 0,0,0);
							dwgfx.createtextbox(hardestroom, -1, 69-24, 0, 0, 0);							
						}else{
							tempstring = "Hardest Room (with " + String(hardestroomdeaths) + " deaths)";
							dwgfx.createtextbox(tempstring, -1, 158, 0,0,0);
							dwgfx.createtextbox(hardestroom, -1, 170, 0, 0, 0);
						}
					break;
					case 3508:
					  state++; statedelay = 0;
						
						if (dwgfx.flipmode) { 
						  dwgfx.createtextbox(" Tap screen to continue ", -1, 20, 164, 164, 255);
						}else{
						  dwgfx.createtextbox(" Tap screen to continue ", -1, 196, 164, 164, 255);
						}
						dwgfx.textboxcenterx();
					break;
					case 3509:
					  if (jumppressed) {
							state++; statedelay = 30;
							dwgfx.textboxremove();
						}
					break;	
					case 3510:
					  //Save stats and stuff here
						if (obj.flags[73] == 0) {
							//flip mode complete
							scores.reportAchievement(scores.vvvvvvgamecompleteflip);
							unlock[19] = true;
						}
						
					  if (bestgamedeaths == -1) {
							bestgamedeaths = deathcounts;
						}else {
							if (deathcounts < bestgamedeaths) {
								bestgamedeaths = deathcounts;
							}
						}
						
						if (bestgamedeaths > -1) {
							if (bestgamedeaths <= 500) {
							  scores.reportAchievement(scores.vvvvvvcomplete500);
							}
							if (bestgamedeaths <= 250) {
								scores.reportAchievement(scores.vvvvvvcomplete250);
							}
							if (bestgamedeaths <= 100) {
								scores.reportAchievement(scores.vvvvvvcomplete100);
							}
							if (bestgamedeaths <= 50) {
								scores.reportAchievement(scores.vvvvvvcomplete50);
							}
						}
						
						savestats(map, dwgfx);
	  				if (nodeathmode) {
							scores.reportAchievement(scores.vvvvvvmaster); //bloody hell
							unlock[20] = true;
							state = 3520; statedelay = 0;
					  }else{
					    statedelay = 120;
						  state++;
				    }
					break;
					case 3511:
					  //Activating a teleporter (long version for level complete)
						i = obj.getplayer(); obj.entities[i].colour = 102;
						
						obj.flags[67] = 1;
						
						state++; statedelay = 30;
						flashlight = 5;
						screenshake = 90;
						music.playef(9, 10);
					break;
					case 3512:
					  //Activating a teleporter 2
						state++; statedelay = 15;
						flashlight = 5;
						music.playef(9, 10);
					break;
					case 3513:
					  //Activating a teleporter 2
						state++; statedelay = 15;
						flashlight = 5;
						music.playef(9, 10);
					break;
					case 3514:
					  //Activating a teleporter 2
						state++; statedelay = 15;
						flashlight = 5;
						music.playef(9, 10);
					break;
					case 3515:
					  //Activating a teleporter 2
						state++; statedelay = 0;
						flashlight = 5; screenshake = 0;
						
						i = obj.getplayer(); obj.entities[i].colour = 0;
						obj.entities[i].invis = true;
								
						//we're done here!
						music.playef(10, 10);
						statedelay = 60;	
					break;
					case 3516:
					  dwgfx.fademode = 2;
						state++;
					break;
					case 3517:
					  if (dwgfx.fademode == 1) {
						  state++; statedelay = 30;	
						}						
					break;
					case 3518:
					  dwgfx.fademode = 4;
						state = 0; statedelay = 30;	
						//music.play(5);
						//music.play(10);
						
					  map.finalmode = false;
					  map.final_colormode = false;	map.final_mapcol = 0;	map.final_colorframe = 0;
						map.finalstretch = false;
						map.finalx = 100; map.finaly = 100; 
						
						dwgfx.cutscenebarspos = 320;
					
						teleport_to_new_area = true;
						teleportscript = "gamecomplete";
					break;
					  
					case 3520:
					  //NO DEATH MODE COMPLETE JESUS
						hascontrol = false;
						crewstats[0] = true;
						
						dwgfx.fademode = 2;	
						state++;
					break;
					case 3521: 
					  if(dwgfx.fademode == 1)	state++;
					break;
					case 3522: 					  
					  dwgfx.flipmode = false;
						gamestate = 1; dwgfx.fademode = 4;
						dwgfx.backgrounddrawn = true; map.tdrawback = true;
						createmenu("nodeathmodecomplete");
						state = 0;
					break;
					
					case 4000:
					  //Activating a teleporter (short version)
						state++; statedelay = 10;
						flashlight = 5;
						screenshake = 10;
						music.playef(9, 10);
					break;
					case 4001:
					  //Activating a teleporter 2
						state++; statedelay = 0;
						flashlight = 5; screenshake = 0;
						//we're done here!
						music.playef(10, 10);
					break;
					case 4002:
					  //Activating a teleporter 2
						state++; statedelay = 10;
						//testing!
						//state = 3006; //Warp Zone
						//state = 3020; //Space Station
						//state = 3040; //Lab
						
						i = obj.getplayer(); 
						obj.entities[i].colour = 0;
						obj.entities[i].invis = true;
						
						i = obj.getteleporter();
				    if(i>-1){
							obj.entities[i].tile = 1;
							obj.entities[i].colour = 100;
						}
					break;
					case 4003:
					  state = 0; statedelay = 0;
						teleport_to_new_area = true;
					break;
					
					case 4010:
						//Activating a teleporter (default appear)
						state++; statedelay = 15;
						flashlight = 5; screenshake = 90;
						music.playef(9, 10);
					break;
					case 4011:
					  //Activating a teleporter 2
						state++; statedelay = 0; flashlight = 5; screenshake = 0;	music.playef(10, 10);
					break;
					case 4012:
					  //Activating a teleporter 2
						state++; statedelay = 5;
						
						i = obj.getplayer(); j = obj.getteleporter();
						if (j != -1) {
						  obj.entities[i].xp = obj.entities[j].xp+44;
						  obj.entities[i].yp = obj.entities[j].yp+44;
						  obj.entities[j].tile = 2; obj.entities[j].colour = 101;
						}
						obj.entities[i].colour = 0; obj.entities[i].invis = false; obj.entities[i].dir = 1;
						
						obj.entities[i].ay = -6; obj.entities[i].ax = 6;
						obj.entities[i].vy = -6; obj.entities[i].vx = 6;
					break;
					case 4013: state++; i = obj.getplayer(); obj.entities[i].xp += 10; break;
					case 4014: state++; i = obj.getplayer(); obj.entities[i].xp += 10; break;
					case 4015: state++; i = obj.getplayer(); obj.entities[i].xp += 8; break;
					case 4016: state++; i = obj.getplayer(); obj.entities[i].xp += 6; break;
					case 4017: state++; i = obj.getplayer(); obj.entities[i].xp += 3; break;
					case 4018: state++; statedelay = 15;	i = obj.getplayer(); 	obj.entities[i].xp += 1;	break;
					case 4019: 
					  if (intimetrial || nodeathmode || inintermission) {
						}else{
              savetele(map, obj, music, help);
						}
						i = obj.getteleporter();
						activetele = true;
						teleblock.x = obj.entities[i].xp - 32;	teleblock.y = obj.entities[i].yp - 32;
						teleblock.width = 160;							    teleblock.height = 160;
					  hascontrol = true;	advancetext = false;	state = 0; 
					break;
					
					case 4020:
						//Activating a teleporter (default appear)
						state++; statedelay = 15;
						flashlight = 5; screenshake = 90;
						music.playef(9, 10);
					break;
					case 4021:
					  //Activating a teleporter 2
						state++; statedelay = 0; flashlight = 5; screenshake = 0;	music.playef(10, 10);
					break;
					case 4022:
					  //Activating a teleporter 2
						state++; statedelay = 5;
						
						i = obj.getplayer(); j = obj.getteleporter();
						if (j != -1) {
						  obj.entities[i].xp = obj.entities[j].xp+44;
						  obj.entities[i].yp = obj.entities[j].yp+44;
						  obj.entities[j].tile = 2; obj.entities[j].colour = 101;
						}
						obj.entities[i].colour = 0; obj.entities[i].invis = false; obj.entities[i].dir = 1;
						
						obj.entities[i].ay = -6; obj.entities[i].ax = 6;
						obj.entities[i].vy = -6; obj.entities[i].vx = 6;
					break;
					case 4023: state++; i = obj.getplayer(); obj.entities[i].xp += 12; break;
					case 4024: state++; i = obj.getplayer(); obj.entities[i].xp += 12; break;
					case 4025: state++; i = obj.getplayer(); obj.entities[i].xp += 10; break;
					case 4026: state++; i = obj.getplayer(); obj.entities[i].xp += 8; break;
					case 4027: state++; i = obj.getplayer(); obj.entities[i].xp += 5; break;
					case 4028: state++; statedelay = 15;	i = obj.getplayer(); 	obj.entities[i].xp += 2;	break;
					case 4029: hascontrol = true;	advancetext = false;	state = 0; break;
					
					case 4030:
						//Activating a teleporter (default appear)
						state++; statedelay = 15;
						flashlight = 5; screenshake = 90;
						music.playef(9, 10);
					break;
					case 4031:
					  //Activating a teleporter 2
						state++; statedelay = 0; flashlight = 5; screenshake = 0;	music.playef(10, 10);
					break;
					case 4032:
					  //Activating a teleporter 2
						state++; statedelay = 5;
						
						i = obj.getplayer(); j = obj.getteleporter();
						if (j != -1) {
						  obj.entities[i].xp = obj.entities[j].xp+44;
						  obj.entities[i].yp = obj.entities[j].yp+44;
						  obj.entities[j].tile = 2; obj.entities[j].colour = 101;
						}
						obj.entities[i].colour = 0; obj.entities[i].invis = false; obj.entities[i].dir = 0;
						
						obj.entities[i].ay = -6; obj.entities[i].ax = -6;
						obj.entities[i].vy = -6; obj.entities[i].vx = -6;
					break;
					case 4033: state++; i = obj.getplayer(); obj.entities[i].xp -= 12; break;
					case 4034: state++; i = obj.getplayer(); obj.entities[i].xp -= 12; break;
					case 4035: state++; i = obj.getplayer(); obj.entities[i].xp -= 10; break;
					case 4036: state++; i = obj.getplayer(); obj.entities[i].xp -= 8; break;
					case 4037: state++; i = obj.getplayer(); obj.entities[i].xp -= 5; break;
					case 4038: state++; statedelay = 15;	i = obj.getplayer(); 	obj.entities[i].xp -= 2;	break;
					case 4039: hascontrol = true;	advancetext = false;	state = 0; break;
					
					case 4040:
						//Activating a teleporter (default appear)
						state++; statedelay = 15;
						flashlight = 5; screenshake = 90;
						music.playef(9, 10);
					break;
					case 4041:
					  //Activating a teleporter 2
						state++; statedelay = 0; flashlight = 5; screenshake = 0;	music.playef(10, 10);
					break;
					case 4042:
					  //Activating a teleporter 2
						state++; statedelay = 5;
						
						i = obj.getplayer(); j = obj.getteleporter();
						if (j != -1) {
						  obj.entities[i].xp = obj.entities[j].xp+44;
						  obj.entities[i].yp = obj.entities[j].yp+44;
						  obj.entities[j].tile = 2; obj.entities[j].colour = 101;
						}
						obj.entities[i].colour = 0; obj.entities[i].invis = false; obj.entities[i].dir = 1;
						
						obj.entities[i].ay = -6; obj.entities[i].ax = 6;
						obj.entities[i].vy = -6; obj.entities[i].vx = 6;
					break;
					case 4043: state++; i = obj.getplayer(); obj.entities[i].xp += 12; obj.entities[i].yp -= 15;  break;
					case 4044: state++; i = obj.getplayer(); obj.entities[i].xp += 12; obj.entities[i].yp -= 10;  break;
					case 4045: state++; i = obj.getplayer(); obj.entities[i].xp += 12; obj.entities[i].yp -= 10;  break;
					case 4046: state++; i = obj.getplayer(); obj.entities[i].xp += 8; obj.entities[i].yp -= 8;  break;
					case 4047: state++; i = obj.getplayer(); obj.entities[i].xp += 6; obj.entities[i].yp -= 8; break;
					case 4048: state++; statedelay = 15;	i = obj.getplayer(); 	obj.entities[i].xp += 3;	break;
					case 4049: hascontrol = true;	advancetext = false;	state = 0; break;
					
					case 4050:
						//Activating a teleporter (default appear)
						state++; statedelay = 15;
						flashlight = 5; screenshake = 90;
						music.playef(9, 10);
					break;
					case 4051:
					  //Activating a teleporter 2
						state++; statedelay = 0; flashlight = 5; screenshake = 0;	music.playef(10, 10);
					break;
					case 4052:
					  //Activating a teleporter 2
						state++; statedelay = 5;
						
						i = obj.getplayer(); j = obj.getteleporter();
						if (j != -1) {
						  obj.entities[i].xp = obj.entities[j].xp+44;
						  obj.entities[i].yp = obj.entities[j].yp+44;
						  obj.entities[j].tile = 2; obj.entities[j].colour = 101;
						}
						obj.entities[i].colour = 0; obj.entities[i].invis = false; obj.entities[i].dir = 1;
						
						obj.entities[i].ay = -6; obj.entities[i].ax = 6;
						obj.entities[i].vy = -6; obj.entities[i].vx = 6;
					break;
					case 4053: state++; i = obj.getplayer(); obj.entities[i].xp += 4; obj.entities[i].yp -= 15;  break;
					case 4054: state++; i = obj.getplayer(); obj.entities[i].xp += 4; obj.entities[i].yp -= 10;  break;
					case 4055: state++; i = obj.getplayer(); obj.entities[i].xp += 4; obj.entities[i].yp -= 10;  break;
					case 4056: state++; i = obj.getplayer(); obj.entities[i].xp += 4; obj.entities[i].yp -= 8;  break;
					case 4057: state++; i = obj.getplayer(); obj.entities[i].xp += 2; obj.entities[i].yp -= 8; break;
					case 4058: state++; statedelay = 15;	i = obj.getplayer(); 	obj.entities[i].xp += 1;	break;
					case 4059: hascontrol = true;	advancetext = false;	state = 0; break;
					
					case 4060:
						//Activating a teleporter (default appear)
						state++; statedelay = 15;
						flashlight = 5; screenshake = 90;
						music.playef(9, 10);
					break;
					case 4061:
					  //Activating a teleporter 2
						state++; statedelay = 0; flashlight = 5; screenshake = 0;	music.playef(10, 10);
					break;
					case 4062:
					  //Activating a teleporter 2
						state++; statedelay = 5;
						
						i = obj.getplayer(); j = obj.getteleporter();
						if (j != -1) {
						  obj.entities[i].xp = obj.entities[j].xp+44;
						  obj.entities[i].yp = obj.entities[j].yp+44;
						  obj.entities[j].tile = 2; obj.entities[j].colour = 101;
						}
						obj.entities[i].colour = 0; obj.entities[i].invis = false; obj.entities[i].dir = 0;
						
						obj.entities[i].ay = -6; obj.entities[i].ax = -6;
						obj.entities[i].vy = -6; obj.entities[i].vx = -6;
					break;
					case 4063: state++; i = obj.getplayer(); obj.entities[i].xp -= 28; obj.entities[i].yp -= 8; break;
					case 4064: state++; i = obj.getplayer(); obj.entities[i].xp -= 28; obj.entities[i].yp -= 8; break;
					case 4065: state++; i = obj.getplayer(); obj.entities[i].xp -= 25; break;
					case 4066: state++; i = obj.getplayer(); obj.entities[i].xp -= 25; break;
					case 4067: state++; i = obj.getplayer(); obj.entities[i].xp -= 20; break;
					case 4068: state++; statedelay = 15;	i = obj.getplayer(); 	obj.entities[i].xp -= 16;	break;
					case 4069: hascontrol = true;	advancetext = false;	state = 0; break;
					
					
					case 4070:
						//Activating a teleporter (special for final script, player has colour changed to match rescued crewmate)
						state++; statedelay = 15;
						flashlight = 5; screenshake = 90;
						music.playef(9, 10);
					break;
					case 4071:
					  //Activating a teleporter 2
						state++; statedelay = 0; flashlight = 5; screenshake = 0;	music.playef(10, 10);
					break;
					case 4072:
					  //Activating a teleporter 2
						state++; statedelay = 5;
						
						i = obj.getplayer(); j = obj.getteleporter();
						if (j != -1) {
						  obj.entities[i].xp = obj.entities[j].xp+44;
						  obj.entities[i].yp = obj.entities[j].yp+44;
						  obj.entities[j].tile = 2; obj.entities[j].colour = 101;
						}
						obj.entities[i].invis = false; obj.entities[i].dir = 1;
						obj.entities[i].colour = obj.crewcolour(lastsaved);
						
						obj.entities[i].ay = -6; obj.entities[i].ax = 6;
						obj.entities[i].vy = -6; obj.entities[i].vx = 6;
					break;
					case 4073: state++; i = obj.getplayer(); obj.entities[i].xp += 10; break;
					case 4074: state++; i = obj.getplayer(); obj.entities[i].xp += 10; break;
					case 4075: state++; i = obj.getplayer(); obj.entities[i].xp += 8; break;
					case 4076: state++; i = obj.getplayer(); obj.entities[i].xp += 6; break;
					case 4077: state++; i = obj.getplayer(); obj.entities[i].xp += 3; break;
					case 4078: state++; statedelay = 15;	i = obj.getplayer(); 	obj.entities[i].xp += 1;	break;
					case 4079: 
					  state = 0; 
						startscript = true;	newscript = "finallevel_teleporter";
					break;
					
					case 4080:
						//Activating a teleporter (default appear)
						state++; statedelay = 15;
						flashlight = 5; screenshake = 90;
						music.playef(9, 10);
					break;
					case 4081:
					  //Activating a teleporter 2
						state++; statedelay = 0; flashlight = 5; screenshake = 0;	music.playef(10, 10);
					break;
					case 4082:
					  //Activating a teleporter 2
						state++; statedelay = 5;
						
						i = obj.getplayer(); j = obj.getteleporter();
						if (j != -1) {
						  obj.entities[i].xp = obj.entities[j].xp+44;
						  obj.entities[i].yp = obj.entities[j].yp+44;
						  obj.entities[j].tile = 2; obj.entities[j].colour = 101;
						}
						obj.entities[i].colour = 0; obj.entities[i].invis = false; obj.entities[i].dir = 1;
						
						obj.entities[i].ay = -6; obj.entities[i].ax = 6;
						obj.entities[i].vy = -6; obj.entities[i].vx = 6;
					break;
					case 4083: state++; i = obj.getplayer(); obj.entities[i].xp += 10; break;
					case 4084: state++; i = obj.getplayer(); obj.entities[i].xp += 10; break;
					case 4085: state++; i = obj.getplayer(); obj.entities[i].xp += 8; break;
					case 4086: state++; i = obj.getplayer(); obj.entities[i].xp += 6; break;
					case 4087: state++; i = obj.getplayer(); obj.entities[i].xp += 3; break;
					case 4088: state++; statedelay = 15;	i = obj.getplayer(); 	obj.entities[i].xp += 1;	break;
					case 4089: 
						startscript = true;	newscript = "gamecomplete_ending";
					  state = 0; 
					break;
					
					case 4090:
						//Activating a teleporter (default appear)
						state++; statedelay = 15;
						flashlight = 5; screenshake = 90;
						music.playef(9, 10);
					break;
					case 4091:
					  //Activating a teleporter 2
						state++; statedelay = 0; flashlight = 5; screenshake = 0;	music.playef(10, 10);
					break;
					case 4092:
					  //Activating a teleporter 2
						state++; statedelay = 5;
						
						i = obj.getplayer(); j = obj.getteleporter();
						if (j != -1) {
						  obj.entities[i].xp = obj.entities[j].xp+44;
						  obj.entities[i].yp = obj.entities[j].yp+44;
						  obj.entities[j].tile = 2; obj.entities[j].colour = 101;
						}
						obj.entities[i].colour = 0; obj.entities[i].invis = false; obj.entities[i].dir = 1;
						
						obj.entities[i].ay = -6; obj.entities[i].ax = 6;
						obj.entities[i].vy = -6; obj.entities[i].vx = 6;
					break;
					case 4093: state++; i = obj.getplayer(); obj.entities[i].xp += 10; break;
					case 4094: state++; i = obj.getplayer(); obj.entities[i].xp += 10; break;
					case 4095: state++; i = obj.getplayer(); obj.entities[i].xp += 8; break;
					case 4096: state++; i = obj.getplayer(); obj.entities[i].xp += 6; break;
					case 4097: state++; i = obj.getplayer(); obj.entities[i].xp += 3; break;
					case 4098: state++; statedelay = 15;	i = obj.getplayer(); 	obj.entities[i].xp += 1;	break;
					case 4099: 
					  if (nocutscenes) {
							startscript = true;	newscript = "levelonecompleteskip";
						}else{
						  startscript = true;	newscript = "levelonecomplete_ending";
						}
					  state = 0; 
					break;
				}
			}
		}
		
		public function lifesequence(obj:entityclass):void{
			if (lifeseq > 0) {
				i = obj.getplayer();
				obj.entities[i].invis = false;
				if (lifeseq == 2) obj.entities[i].invis = true;
				if (lifeseq == 6) obj.entities[i].invis = true;
				if (lifeseq >= 8) obj.entities[i].invis = true;
				if (lifeseq > 5) gravitycontrol = savegc;
				
				lifeseq--;
				if (lifeseq <= 0) { obj.entities[i].invis = false; }
			}
		}
		
		public function resetgameclock():void {
			frames = 0;
			seconds = 0;
			minutes = 0;
			hours = 0;
		}
		
		public function gameclock():void {
			frames++;
			if (frames >= 30) {
				frames -= 30;
				seconds++;
				if (seconds >= 60) {
					seconds -= 60;
					minutes++;
					if (minutes >= 60) {
						minutes -= 60;
						hours++;
					}
				}
			}
		}
		
		public function giventimestring(hrs:int, min:int, sec:int, help:helpclass):String {
			tempstring = "";
			if (hrs > 0) {
				tempstring += String(hrs) + ":";
			}
			tempstring += help.twodigits(min) + ":" + help.twodigits(sec);
			return tempstring;
		}
		
		public function timestring(help:helpclass):String {
			tempstring = "";
			if (hours > 0) {
				tempstring += String(hours) + ":";
			}
			tempstring += help.twodigits(minutes) + ":" + help.twodigits(seconds);
			return tempstring;
		}
		
		public function partimestring(help:helpclass):String {
			//given par time in seconds:
			tempstring = "";
			if (timetrialpar >= 60) {
				tempstring = help.twodigits(int((timetrialpar - (timetrialpar % 60)) / 60)) + ":" + help.twodigits(timetrialpar % 60);
			}else {
				tempstring = "00:" + help.twodigits(timetrialpar);
			}
			return tempstring;
		}
		
		public function resulttimestring(help:helpclass):String {
			//given result time in seconds:
			tempstring = "";
			if (timetrialresulttime > 60) {
				tempstring = help.twodigits(int((timetrialresulttime - (timetrialresulttime % 60)) / 60)) + ":" 
				           + help.twodigits(timetrialresulttime % 60);
			}else {
				tempstring = "00:" + help.twodigits(timetrialresulttime);
			}
			return tempstring;
		}
		
		public function timetstring(t:int, help:helpclass):String {
			//given par time in seconds:
			tempstring = "";
			if (t >= 60) {
				tempstring = help.twodigits(int((t - (t % 60)) / 60)) + ":" + help.twodigits(t % 60);
			}else {
				tempstring = "00:" + help.twodigits(t);
			}
			return tempstring;
		}
		
		
		public function deathsequence(map:mapclass, obj:entityclass, music:musicclass):void {
			if (supercrewmate && scmhurt) {
				i = obj.getscm();
			}else{
				i = obj.getplayer();
			}
			obj.entities[i].colour = 1;

			obj.entities[i].invis = false;
			if (deathseq == 30) {
				if (nodeathmode) {
					music.fadeout();
					gameoverdelay = 60;
				}
				deathcounts++;
				music.playef(2,10);
				obj.entities[i].invis = true;
				if (map.finalmode) {
				  map.roomdeathsfinal[roomx - 41 + (20 * (roomy - 48))]++;
				  currentroomdeaths = map.roomdeathsfinal[roomx - 41 + (20 * (roomy - 48))];
				}else{
				  map.roomdeaths[roomx - 100 + (20*(roomy - 100))]++;
				  currentroomdeaths = map.roomdeaths[roomx - 100 + (20 * (roomy - 100))];
				}
			}
			if (deathseq == 25) obj.entities[i].invis = true;
			if (deathseq == 20) obj.entities[i].invis = true;
			if (deathseq == 16) obj.entities[i].invis = true;
			if (deathseq == 14) obj.entities[i].invis = true;
			if (deathseq == 12) obj.entities[i].invis = true;
			if (deathseq < 10) obj.entities[i].invis = true;
			if (!nodeathmode) {
				if (deathseq <= 1) obj.entities[i].invis = false;
			}else {
				gameoverdelay--;
			}
		}
		
		public function initteleportermode(map:mapclass):void {
			//Set the teleporter variable to the right position!
			teleport_to_teleporter = 0;
			
			for (i = 0; i < map.numteleporters; i++) {
				if (roomx == map.teleporters[i].x + 100 && roomy == map.teleporters[i].y + 100) {
					teleport_to_teleporter = i;
				}
			}
		}
		
		public function swnpenalty():void {
			//set the SWN clock back to the closest 5 second interval
			if (swntimer <= 150) { swntimer += 8; if (swntimer > 150) swntimer = 150;
			}else if (swntimer <= 300) { swntimer += 8; if (swntimer > 300) swntimer = 300;
			}else if (swntimer <= 450) { swntimer += 8; if (swntimer > 450) swntimer = 450;
			}else if (swntimer <= 600) { swntimer += 8; if (swntimer > 600) swntimer = 600;
			}else if (swntimer <= 750) { swntimer += 8; if (swntimer > 750) swntimer = 750;
			}else if (swntimer <= 900) { swntimer += 8; if (swntimer > 900) swntimer = 900;
			}else if (swntimer <= 1050) { swntimer += 8; if (swntimer > 1050) swntimer = 1050;
			}else if (swntimer <= 1200) {	swntimer += 8; if (swntimer > 1200) swntimer = 1200;
			}else if (swntimer <= 1350) {	swntimer += 8; if (swntimer > 1350) swntimer = 1350;
			}else if (swntimer <= 1500) {	swntimer += 8; if (swntimer > 1500) swntimer = 1500;
			}else if (swntimer <= 1650) {	swntimer += 8; if (swntimer > 1650) swntimer = 1650;
			}else if (swntimer <= 1800) { swntimer += 8; if (swntimer > 1800) swntimer = 1800;
			}else if (swntimer <= 2100) {	swntimer += 8; if (swntimer > 2100) swntimer = 2100;
			}else if (swntimer <= 2400) {	swntimer += 8; if (swntimer > 2400) swntimer = 2400;
			}
		}
		
		//Game room control
		public var door_left:int, door_right:int, door_up:int, door_down:int;
		public var roomx:int, roomy:int, roomchangedir:int;
		public var temp:int, i:int, j:int, k:int;
		
		//Save points
		public var savex:int, savey:int, saverx:int, savery:int;
		public var savegc:int, savedir:int;
		
		//Game save variables: two cookies, one for quicksaves and one for teleporters
		public var telecookie:SharedObject, quickcookie:SharedObject, statcookie:SharedObject;
		public var customcookie:Vector.<SharedObject> = new Vector.<SharedObject>;
		public var customcookieexists:Vector.<Boolean> = new Vector.<Boolean>;
		public var customcookiesummary:Vector.<String> = new Vector.<String>;
		public var telecookieexists:Boolean, quickcookieexists:Boolean;
		public var telesummary:String, quicksummary:String;
		public var tele_crewstats:Array = new Array();
		public var tele_gametime:String;
		public var tele_trinkets:int;
		public var tele_currentarea:String;
		public var quick_crewstats:Array = new Array();
		public var quick_gametime:String;
		public var quick_trinkets:int;
		public var quick_currentarea:String;
		public var summary_crewstats:Array;// = new Array();
		
		public var savestate:Array = new Array();
		
    //Added for port
    public var edsavex:int, edsavey:int, edsaverx:int, edsavery:int;
    public var edsavegc:int, edsavedir:int;
		
		//State Logic stuff
		public var state:int, statedelay:int;
		
		public var gamestate:int;
    public var hascontrol:Boolean, jumpheld:Boolean, jumppressed:int;
		public var gravitycontrol:int;
		
		public var infocus:Boolean, paused:Boolean;
		public var muted:Boolean; 
		public var mutebutton:int;
		public var globalsound:int;
		
		public var mx:int, my:int;
		public var screenshake:int, flashlight:int;
		public var test:Boolean, teststring:String, tempstring:String;
		public var advancetext:Boolean, pausescript:Boolean;
		
		public var deathseq:int, lifeseq:int;
		
		public var coins:int, trinkets:int, crewmates:int, trinketcollect:int;
		public var savepoint:int, teleport:Boolean, teleportxpos:int;
		public var edteleportent:int;
		public var completestop:Boolean;
		
		public var inertia:Number;
		
		public var companion:int, roomchange:Boolean;
		public var teleblock:Rectangle, activetele:Boolean, readytotele:int;
		public var activeactivity:int, act_fade:int;
		public var activity_lastprompt:String, activity_r:int, activity_g:int, activity_b:int;
		public var backgroundtext:Boolean;
		
		public var press_left:Boolean, press_right:Boolean, press_action:Boolean, press_map:Boolean;
		
		//Some stats:
		public var totalflips:int;
		public var hardestroom:String, hardestroomdeaths:int, currentroomdeaths:int;
		
		//Menu interaction stuff
		public var mapheld:Boolean; //Is the map key being held down? 
		public var menupage:int;
		public var crewstats:Array = new Array();
		public var lastsaved:int;
		public var deathcounts:int;
		public var frames:int, seconds:int, minutes:int, hours:int;
		public var gamesaved:Boolean;
		public var savetime:String, savearea:String, savetrinkets:int;
		public var startscript:Boolean, newscript:String;
		
		public var mainmenu:int, menustart:Boolean;
		
	  //teleporting
		public var teleport_to_new_area:Boolean;
		public var teleport_to_x:int, teleport_to_y:int;
		public var teleportscript:String;
		public var useteleporter:Boolean;
		public var teleport_to_teleporter:int;
		
		//Screenrecording stuff, for beta/trailer
		public var recording:int;
		public var recordstring:String;
		public var combomode:Boolean, combolen:int, comboaction:String;
		public var currentaction:String, recordinit:Boolean;
		
		public var alarmon:Boolean, alarmdelay:int;
		public var blackout:Boolean;
		
		public var playback:Array = new Array();
		public var playbackpos:int, playbacksize:int;
		public var playmove:int, playcombo:int;
		public var playbackfinished:Boolean;
		
		//functions for recording:
		public function initplayback():void {
			playback = new Array();
				
		  playback = recordstring.split(",");
			playbacksize = playback.length;
			playbackpos = 0;
		}
		
		//Main Menu Variables
		public var menuoptions:Vector.<String> = new Vector.<String>;
		public var menuoptions_alt:Vector.<String> = new Vector.<String>;
		public var menuoptionsactive:Vector.<Boolean> = new Vector.<Boolean>;
		public var menuoptionslayout:Vector.<int> = new Vector.<int>;
		public var nummenuoptions:int, currentmenuoption:int;
		public var menuselection:String, currentmenuname:String, previousmenuname:String;
		public var menuxoff:int, menuyoff:int;
		
		public var menucountdown:int, menudest:String;
		
		public var creditposx:int, creditposy:int, creditposdelay:int;
		
		//60 fps mode!
		public var sfpsmode:Boolean;
		
		//Sine Wave Ninja Minigame
		public var swnmode:Boolean; 
		public var swngame:int, swnstate:int, swnstate2:int, swnstate3:int, swnstate4:int, swndelay:int, swndeaths:int;
		public var swntimer:int, swncolstate:int, swncoldelay:int;
		public var swnrecord:int, swnbestrank:int, swnrank:int, swnmessage:int;
		
		//SuperCrewMate Stuff
		public var supercrewmate:Boolean, scmhurt:Boolean, scmprogress:int, scmmoveme:Boolean;
		
		//Accessibility Options
		public var colourblindmode:Boolean;
		public var noflashingmode:Boolean;
		public var slowdown:int;
		
		public var nodeathmode:Boolean;
		public var gameoverdelay:int;
		public var nocutscenes:Boolean;
		
		//Time Trials
		public var intimetrial:Boolean, timetrialparlost:Boolean;
		public var timetrialcountdown:int, timetrialshinytarget:int, timetriallevel:int;
		public var timetrialpar:int, timetrialresulttime:int, timetrialrank:int;
		
		public var creditposition:int;
		public var insecretlab:Boolean;
		
		public var inintermission:Boolean;
		
		//Tap sensors
		public var tapleft:int, tapright:int;
		
		//Stats
		public var unlock:Array = new Array();
		public var unlocknotify:Array = new Array();
		public var temp_unlock:Array;
		public var temp_unlocknotify:Array;
		public var stat_trinkets:int;
		public var fullscreen:Boolean;
		public var bestgamedeaths:int;
		
		public var stat_screenshakes:Boolean;
		public var stat_backgrounds:Boolean;
		public var stat_flipmode:Boolean;
		public var stat_invincibility:Boolean;
		public var stat_slowdown:int;
		
		public var besttimes:Array = new Array();
		public var besttrinkets:Array = new Array();
		public var bestlives:Array = new Array();
		public var bestrank:Array = new Array();
		public var temp_besttimes:Array;
		public var temp_besttrinkets:Array;
		public var temp_bestlives:Array;
		public var temp_bestrank:Array;
		
		public var savemystats:Boolean;
		
		public var menukludge:Boolean;
		public var quickrestartkludge:Boolean;
		
		public var advanced_mode:Boolean;
		public var advanced_smoothing:Boolean;
		public var advanced_scaling:int;
		
		public var fullscreentoggleheld:Boolean = false;
		
		//Custom stuff
		public var levelstatscookie:SharedObject;
		public var levelstatscookieloaded:Boolean;
    public var customscript:Vector.<String> = new Vector.<String>;
    public var customcol:int, levelpage:int, playcustomlevel:int;
    public var customleveltitle:String;
    public var customlevelfilename:String;
		
		public var customlevelstats:Vector.<String> = new Vector.<String>; //string array containing level filenames
    public var customlevelscore:Vector.<int> = new Vector.<int>;
    public var numcustomlevelstats:int;
    public var customlevelstatsloaded:Boolean;
		
		//Control types!
		//0 is swipe, 1 is both sides, 2 is d-pad
		public var controlstyle:int = 0;
		public var last_left:int = 0, last_right:int = 0;
		public var last_action:int = 0;
		public var controlsensitivity:int = 50;
		
		//Mobile menu options
		public var mobilemenu:Boolean = true;
		public var autoquicksave:Boolean = false;
		public var mobilequicksave_thisroom:Boolean = false;
		public var controltutorialstate:int = 0;
		public var controltutorialstatedelay:int = 0;
		public var controllerp1:int = 0;
		public var controllerp2:int = 0;
		public var controllerp3:int = 0;
		
		public var showloadingnotice:Boolean = false;
		
		public var disablekludge_1:Boolean = false;
		public var disablekludge_2:Boolean = false;
		public var disablekludge_3:Boolean = false;
		public var disablekludge_4:Boolean = false;
		
		public var showcontroltutorial:int = 0;
		
		public var platform:platformclass = new platformclass;
		public var scores:scoreclass = new scoreclass;
	}
}