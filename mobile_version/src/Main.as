package {
	import flash.geom.*;
	import flash.net.*;
	import flash.display.Screen;
	import flash.display.StageAlign;
	import flash.display.StageQuality;
	import flash.display.StageScaleMode;
	import flash.system.Capabilities;
	import starling.core.Starling;
	import starling.display.Image;
	import starling.text.BitmapFont;
	import starling.text.TextField;
	import starling.core.StatsDisplay;
	import starling.events.Event;
	import starling.utils.Color;
	import starling.textures.*;
	import starling.display.*;
	import flash.filesystem.File;
	import starling.events.KeyboardEvent;
	import flash.ui.Keyboard;
	import starling.utils.AssetManager;
	import bigroom.input.KeyPoll; 
	//import com.mesmotronic.ane.AndroidFullScreen; //This doesn't seem to be needed anymore
	import flash.display.StageDisplayState;
	import flash.media.*;
	
	
	import flash.utils.getTimer;
	import flash.utils.Timer;
  import flash.events.TimerEvent;
	
	public class Main extends Sprite{
		static public var BLOCK:Number = 0;
    static public var TRIGGER:Number = 1;
		static public var DAMAGE:Number = 2;
		
  	include "includes/logic.as";
  	include "includes/input.as";
  	include "includes/render.as";
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
		
		public var addedtwice:Boolean = false;
		
		public function Main() {
			super();	
			
			if (stage) gameinit();
			else addEventListener(Event.ADDED_TO_STAGE, gameinit);
		}
		
		private function gameinit(e:Event = null):void {
			removeEventListener(Event.ADDED_TO_STAGE, gameinit);
			// entry point
			if (addedtwice) return; //I don't think I actually need this, but... I have a hunch
			addedtwice = true;
			
			//Ok: quick security check to make sure it doesn't get posted about
			key = new KeyPoll(Starling.current.nativeStage);
			
			//immersivemode = AndroidFullScreen.isSupported;
			stage.addEventListener(Event.RESIZE, function(e:Event):void{
				androidresize();
			});
			
			device.deviceresolution = device.ANDROID;
			
			editor.init(); //Load in all the levels
			
			//General game variables				
			obj.init();
			help.init();
			
			/*obj.createblock(BLOCK,20,200,128,16);
			obj.createblock(BLOCK,130,170,128,16);
			obj.createblock(BLOCK,240,150,128,16);
			obj.createblock(BLOCK,240,20,128,16);
			obj.createblock(BLOCK,130,40,128,16);
			obj.createblock(BLOCK,20,60,128,16);*/
			
			//Input
			key.definestickrange(device.xres / 2, 0, 6);
			
			SoundMixer.soundTransform = new SoundTransform(1);
			
			music.currentsong = -1; music.musicfade = 0;//no music, no amb
			music.initefchannels(); music.currentefchan = 0;
			music.nicechange = -1;
			
			music.numplays = 0;
			music.musicchan.push(new music_1());                // 0: Level Complete
			music.musicchan.push(new vmaintheme_hq());          // 1: VVVVVV Main Theme  (Pushing Forward)
			music.musicchan.push(new vtempo_hq());              // 2: VVVVVV Tempo Theme (Positive Force)
			music.musicchan.push(new vpfa_hq());                // 3: Potential for Anything
			music.musicchan.push(new passionforexploring());    // 4: UU Brothers Instrumental
			music.musicchan.push(new souleye_intermission());   // 5: Jingle: Intermission
			music.musicchan.push(new presentingvvvvvv());       // 6: Jingle: Menu Loop
			music.musicchan.push(new music_2());                // 7: Jingle: Game Complete
			music.musicchan.push(new configmegamix_hq());       // 8: Config Megamix
			music.musicchan.push(new posreverse());             // 9: Tempo Theme, Reversed
			music.musicchan.push(new poppot());                 // 10: Extra
			music.musicchan.push(new pipedream_hq());           // 11: Highscore
			music.musicchan.push(new pressurecooker_hq());      // 12: Pressure Cooker
			music.musicchan.push(new pacedenergy());            // NEW 13: Paced Energy
			music.musicchan.push(new piercingthesky());         // NEW 14: Piercing the Sky
			music.musicchan.push(new predestinedfateremix());   // NEW 15: Predestined Fate Remix
			
			music.efchan.push(new ef_0());   
			music.efchan.push(new ef_1());   
			music.efchan.push(new ef_2());   
			music.efchan.push(new ef_3());   
			music.efchan.push(new ef_4());   
			music.efchan.push(new ef_5());   
			music.efchan.push(new ef_6());   
			music.efchan.push(new ef_7());   
			music.efchan.push(new ef_8());   
			music.efchan.push(new ef_9());   
			music.efchan.push(new ef_10());   
			music.efchan.push(new ef_11());   
			music.efchan.push(new ef_12());   
			music.efchan.push(new ef_13());   
			music.efchan.push(new ef_14());   
			music.efchan.push(new ef_15());   
			music.efchan.push(new ef_16());   
			music.efchan.push(new ef_17());  
			music.efchan.push(new ef_18());  
			music.efchan.push(new ef_19()); 
			music.efchan.push(new ef_20()); 
			music.efchan.push(new ef_21()); 
			music.efchan.push(new ef_22()); 
			music.efchan.push(new ef_23()); 
			music.efchan.push(new ef_24()); 
			music.efchan.push(new ef_25()); 
			music.efchan.push(new ef_26()); 
			music.efchan.push(new ef_27()); 
				
			/*
				Graphics Init
			*/
			dwgfx.init(stage);
			//Load assets
			dwgfx.starlingassets = new AssetManager();
			dwgfx.starlingassets.enqueue(EmbeddedAssets);
			
			Starling.current.nativeStage.scaleMode = StageScaleMode.NO_SCALE;
			Starling.current.nativeStage.align = StageAlign.TOP_LEFT;
			Starling.current.nativeStage.quality = StageQuality.LOW;
			
			//TO DO: orientation code
			game = new gameclass(dwgfx, map, obj, help, music);
				
			map.ypos = (700-29) * 8;
			map.bypos = map.ypos / 2;
			map.cameramode = 0;
			
			
			///Test Start:
			//-- Comment this for real start
			/*
			gamestate = GAMEMODE;
			/*map.finalmode = true; //Enable final level mode
			//map.finalx = 41; map.finaly = 52; //Midpoint
			//map.finalx = 48; map.finaly = 52; //Just before the tower
			map.finalx = 46; map.finaly = 54; //Current
			//map.finalstretch = true;
			map.final_colormode = true;
			map.final_mapcol = 0;
			map.final_colorframe = 0;
			*/
			/*
			game.starttest(obj, music);
			obj.createentity(game, game.savex, game.savey, 0); //In this game, constant, never destroyed
			map.gotoroom(game.saverx, game.savery, dwgfx, game, obj, music);
			music.play(1);
			*/
			//game.crewstats[1] = true;
			//game.crewstats[2] = true;
			//game.crewstats[3] = true;
			//game.crewstats[4] = true;
			//game.crewstats[5] = true;
			//script.load("intro");
			
			//crew member test
			//obj.createentity(game, game.savex - 10, game.savey - 10, 14); 
			//game.companion = 6; //different rules for different members
			
			
			
			//--
			//Uncomment this if testing the final level
			/*
				map.finalmode = true; //Enable final level mode
				map.finalx = 46; map.finaly = 54; //Current
				//map.finalx = 41; map.finaly = 52; //Midpoint
				map.final_colormode = false;	map.final_mapcol = 0;	map.final_colorframe = 0;
			*/
			//And this if after the midpoint:
			/*
				map.finalx = 52; map.finaly = 53; //Current
				map.finalstretch = true;
				map.final_colormode = true;
				map.final_mapcol = 0;
				map.final_colorframe = 0;
				//map.background = 6;
			*/
			//-- Jumping right in
			/*
			game.gamestate = GAMEMODE;
			game.starttest(obj, music);
			//game.loadquick(map, obj, music);
			obj.createentity(game, game.savex, game.savey, 0); //In this game, constant, never destroyed
			map.gotoroom(game.saverx, game.savery, dwgfx, game, obj, music);
			music.play(4);
			//fscommand("quit");
			
			game.crewstats[1] = true;
			//game.crewstats[2] = true;
			game.crewstats[3] = true;
			game.crewstats[4] = true;
			//game.crewstats[5] = true;
			
			map.showtargets = true;
			map.showteleporters = true;
			//map.showtrinkets = true;
			
			//obj.flags[34] = 1;
			//obj.flags[20] = 1;				
			//obj.flags[67] = 1;			//Game complete
			for (i = 0; i < 20; i++) {
				obj.collect[i] = true;
			}
			game.trinkets = 19; obj.collect[18] = false;
			game.stat_trinkets = 20;
			//obj.altstates = 1;
			
			//initilise map info
			for (j = 0; j < 20; j++) {
				for (i = 0; i < 20; i++) {
					map.explored[i + (j * 20)] = 1;
				}
			}
			*/
			//obj.entities[obj.getplayer()].size = 13;
			
			//game.gamestate = 6;
			
			//game.intimetrail = true; game.timetrialcountdown = 0;
			//game.nodeathmode = true;
			//dwgfx.flipmode = true;
			//game.nocutscenes = true;
			//map.invincibility = true;
			//stage.frameRate = 24;
			//game.colourblindmode = true;
			//game.noflashingmode = true;
			//for intermission 2 test
			//game.lastsaved = 3;
			/*
			game.swnmode = true;
			game.swngame = 2; 
			game.swndelay = 120;
			game.swntimer = 60 * 30;
			*/
			//script.load("intermission_2");
			
			
			//for intermission 1 test
			/*
			game.companion = 11;
			game.supercrewmate = true;
			game.scmprogress = 0;
			game.scmprogress = 10;			
			game.lastsaved = 4;
			music.play(8);
			*/
			//game.sfpsmode = true; //Run at 60 FPS
			//stage.frameRate = 60; // doesn't work, it's a bit more complex than this - will have to do a double sized window instead...
			
			//dwgfx.flipmode = true;
			
			//Testing some unlock stuff
			/*
			game.unlock[9] = true;  //Space Station Intro Time Trial
			game.unlock[10] = true; //Lab Time Trial
			game.unlock[11] = true; //Tower Time Trial
			game.unlock[12] = true; //Space Station 2 Time Trial
			game.unlock[13] = true; //Warp Zone Time Trial
			game.unlock[14] = true; //Final Level Time Trial
			
			game.unlock[17] = true; //No death mode
			game.unlock[18] = true; //Flip Mode
			
			*/
			//-- Normal start, with no fancy stuff!
			
			/*
			game.gamestate = CLICKTOSTART;
			dwgfx.createtextbox(" Click to Start ", 96, 107, 164, 164, 255);
			dwgfx.textboxcenter();
			*/
			//No click to start any more! Instead go right to the menu	
			
			//INIT GRAPHIC MODE HERE
			game.loadstats(map, dwgfx);
			//updategraphicsmode(game, dwgfx);
			
			game.gamestate = TITLEMODE;
				
			game.menustart = false;
			game.mainmenu = 0;
			
			/*if (game.quicksummary != "") {
				game.mainmenu = 2;
			}
			if (game.telesummary != "") {
				game.mainmenu = 1;
			}*/ //what the hell is that for
			
			
			/*
			game.gamestate = 6;
			game.menustart = true;
			map.ypos = (700-29) * 8;
			map.bypos = map.ypos / 2;
			map.cameramode = 0;
			*/
			//--Everything after this is screen recording
			
			//ok, screenrecording here
			//don't record anything (comment both next parts)
			
			game.recording = 0;
			
			if(game.recording==1){
				trace("warning! recording input!");
			}else if(game.recording==2){
				trace("playing back input!");
				game.recordstring = "blahblahblah";
				help.toclipboard(game.recordstring);
				
				game.initplayback();
				game.playbackpos = 5; 
				game.savex = game.playback[0]; game.savey = game.playback[1]; 
				game.saverx = game.playback[2]; game.savery = game.playback[3]; 
				game.savegc = game.playback[4]; game.savedir = 1; 
			}
			
			//addEventListener(Event.ENTER_FRAME, mainloop);
			
			dwgfx.starlingassets.loadQueue(function(ratio:Number):void {
				trace(ratio);
				if (ratio == 1){
					waitforassetstoload();
				}
			});
		}
		
		public function waitforassetstoload():void {
			//Wait till this has loaded the texture before processing
			dwgfx.button_texture.push(new RenderTexture(92, 30));
			dwgfx.button_texture.push(new RenderTexture(92, 30));
			dwgfx.button_texture.push(new RenderTexture(40, 40));
			dwgfx.button_texture.push(new RenderTexture(40, 40));
			dwgfx.addbutton("mobilebuttons/map");
			dwgfx.addbutton("mobilebuttons/back");
			dwgfx.addbutton("mobilebuttons/talk_1");
			dwgfx.addbutton("mobilebuttons/talk_2");
			dwgfx.addbutton("mobilebuttons/use_1");
			dwgfx.addbutton("mobilebuttons/use_2");
			dwgfx.addbutton("mobilebuttons/teleport_1");
			dwgfx.addbutton("mobilebuttons/teleport_2");
			dwgfx.addbutton("mobilebuttons/controls_1");
			dwgfx.addbutton("mobilebuttons/controls_2");
			dwgfx.addbutton("mobilebuttons/gamecenter");
			dwgfx.addbutton("mobilebuttons/button_left");
			dwgfx.addbutton("mobilebuttons/button_right");
			dwgfx.initbuttonstuff();
			
			//We load all our graphics in:
			dwgfx.maketilearray();
			dwgfx.maketile2array();
			dwgfx.maketile3array();
			dwgfx.makespritearray();
			dwgfx.makeflipspritearray();
	    dwgfx.maketelearray();
			dwgfx.makeentcolourarray();
			
			//Load in the images
			dwgfx.addimage("levelcomplete"); // 0
			dwgfx.addimage("minimap"); // 1 (this is the minimap!)
			dwgfx.addimage("covered"); // 2
			dwgfx.addimage("elephant"); // 3
			dwgfx.addimage("gamecomplete"); // 4
			dwgfx.addimage("fliplevelcomplete"); // 5
			dwgfx.addimage("flipgamecomplete"); // 6
			dwgfx.addimage("site"); // 7
			dwgfx.addimage("site2"); // 8
			dwgfx.addimage("site3"); // 9
			dwgfx.addimage("ending"); // 10
			
			dwgfx.addimage("minimap"); // Minimap
			dwgfx.addimage_rendertexture("minimap"); // Minimap
			
			dwgfx.addmobileimage("controls/touchscreen");
			dwgfx.addmobileimage("controls/lefthand_off");
			dwgfx.addmobileimage("controls/lefthand_near");
			dwgfx.addmobileimage("controls/lefthand_far");
			dwgfx.addmobileimage("controls/righthand_off");
			dwgfx.addmobileimage("controls/righthand_near");
			dwgfx.addmobileimage("controls/righthand_far");
			dwgfx.addmobileimage("controls/arrowleft");
			dwgfx.addmobileimage("controls/arrowright");
			
			dwgfx.addplayerlevelimage("playerlevels/playerlevel_0");
			dwgfx.addplayerlevelimage("playerlevels/playerlevel_1");
			dwgfx.addplayerlevelimage("playerlevels/playerlevel_2");
			dwgfx.addplayerlevelimage("playerlevels/playerlevel_3");
			dwgfx.addplayerlevelimage("playerlevels/playerlevel_4");
			dwgfx.addplayerlevelimage("playerlevels/playerlevel_5");
			dwgfx.addplayerlevelimage("playerlevels/playerlevel_6");
			dwgfx.addplayerlevelimage("playerlevels/playerlevel_7");
			dwgfx.addplayerlevelimage("playerlevels/playerlevel_8");
			dwgfx.addplayerlevelimage("playerlevels/playerlevel_9");
			dwgfx.addplayerlevelimage("playerlevels/playerlevel_10");
			dwgfx.addplayerlevelimage("playerlevels/playerlevel_11");
			dwgfx.addplayerlevelimage("playerlevels/playerlevel_12");
			dwgfx.addplayerlevelimage("playerlevels/playerlevel_13");
			dwgfx.addplayerlevelimage("playerlevels/playerlevel_14");
			dwgfx.addplayerlevelimage("playerlevels/playerlevel_15");
			dwgfx.addplayerlevelimage("playerlevels/playerlevel_16");
			
			var c64fontbm:BitmapFont = new BitmapFont(dwgfx.starlingassets.getTexture("c64/c64_0"), 
			                             XML(new c64font_xml)); 
			TextField.registerCompositor(c64fontbm, "c64");
			
			_timer.addEventListener(TimerEvent.TIMER, mainloop);
			_timer.start();
			
			androidresize();
		}
		
		public function sitelock():Boolean {
			//we don't care about this on mobile
			return true;
		}
		
		public function input():void {
			if (!game.infocus) {
			}else {
				switch(game.gamestate){
					case TITLEMODE:
						titleinput(key, dwgfx, map, game, obj, help, music);		
					break;
					case GAMEMODE:
						if (map.towermode) {
							if(game.recording==1){
							  recordinput(key, dwgfx, game, map, obj, help, music);
							}else {
								gameinput(key, dwgfx, game, map, obj, help, music);
							}
						}else{
							if (game.recording == 1) {
								recordinput(key, dwgfx, game, map, obj, help, music);
							}else {
								if (script.running) {
									script.run(key, dwgfx, game, map, obj, help, music);
								}
							  gameinput(key, dwgfx, game, map, obj, help, music);
							}
						}
					break;
					case CONTROLTUTORIALMODE:
						controltutorialinput(key, dwgfx, game, map, obj, help, music);
					break;
					case MAPMODE:
						if (game.recording == 1) {
							//recordinput(key, dwgfx, game, map, obj, help, music); //will implement this later if it's actually needed
						}else{
						  mapinput(key, dwgfx, game, map, obj, help, music);
						}
					break;
				  case TELEPORTERMODE:
						if (game.recording == 1) {
							recordinput(key, dwgfx, game, map, obj, help, music);
						}else {
							if(game.useteleporter) {
						    teleporterinput(key, dwgfx, game, map, obj, help, music);
							}else {
								if (script.running) {
									script.run(key, dwgfx, game, map, obj, help, music);
								}
							  gameinput(key, dwgfx, game, map, obj, help, music);
							}
						}
					break;
					case GAMECOMPLETE:
						gamecompleteinput(key, dwgfx, game, map, obj, help, music);		
					break;
					case GAMECOMPLETE2:
						gamecompleteinput2(key, dwgfx, game, map, obj, help, music);		
					break;
					case CLICKTOSTART:
						if (key.click) {			
							dwgfx.textboxremove();
						}
					break;
				}
				
				//Mute button
				if (key.isDown(77) && game.mutebutton<=0) {
					game.mutebutton = 8; if (game.muted) { game.muted = false;	}else { game.muted = true;}
				}
				if(game.mutebutton>0) game.mutebutton--;
				
			}
			//Stupid event listeners!
			if (key.hasclicked) key.click = false;
		}
		
		public function logic():void {
			if (!game.infocus) {
				if (game.globalsound > 0) {
					game.globalsound = 0;
					SoundMixer.soundTransform = new SoundTransform(0);
				}					
				music.processmusic();
				help.updateglow();
			}else {
				switch(game.gamestate){
					case TITLEMODE:
						titlelogic(key, dwgfx, game, obj, help, music);
					break;
					case GAMEMODE:
						if (map.towermode) {
							towerlogic(key, dwgfx, game, map, obj, help, music);
						}else{
							gamelogic(key, dwgfx, game, map, obj, help, music);
						}
					break;
					case MAPMODE:
						maplogic(key, dwgfx, game, map, obj, help, music);
					break;
					case CONTROLTUTORIALMODE:
						controltutoriallogic(key, dwgfx, game, map, obj, help, music);
					break;
				  case TELEPORTERMODE:
						maplogic(key, dwgfx, game, map, obj, help, music);
					break;
					case GAMECOMPLETE:
						gamecompletelogic(key, dwgfx, game, map, obj, help, music);
					break;
					case GAMECOMPLETE2:
						gamecompletelogic2(key, dwgfx, game, map, obj, help, music);
					break;
					case CLICKTOSTART:
						help.updateglow();
						if (dwgfx.ntextbox == 0) {
							//music.play(6);
							map.ypos = (700-29) * 8;
							map.bypos = map.ypos / 2;
							map.cameramode = 0;
							
							game.gamestate = TITLEMODE;
						}
					break;
				}
				if (game.platform.wakeupcall > 0) {
					game.platform.wakeupcall--;
					if (game.platform.wakeupcall == 0) {
						//if(immersivemode){
						//	AndroidFullScreen.immersiveMode();
						//}
					}
				}
				
				music.processmusic();
				dwgfx.processfade();
				game.gameclock();
				dwgfx.trinketcolset = false; //Reset this in every frame, used for a new standard random colour each frame
				if (game.savemystats) {
					game.savemystats = false;
					game.savestats(map, dwgfx);
				}
				
				if (game.muted) {
					if (game.globalsound == 1) {
					  game.globalsound = 0; SoundMixer.soundTransform = new SoundTransform(0);
					}
				}
				
				if (!game.muted && game.globalsound == 0) {
					game.globalsound = 1; SoundMixer.soundTransform = new SoundTransform(1);
				}
			}
		}
		
		public function dorender():void {
			dwgfx.backbuffer.drawBundled(function():void {
				if (!game.infocus) {
					//dwgfx.backbuffer.lock();
					/*
					dwgfx.bprint(5, 110, "Game paused", 196 - help.glow, 255 - help.glow, 196 - help.glow, true);
					dwgfx.bprint(5, 120, "[click to resume]", 196 - help.glow, 255 - help.glow, 196 - help.glow, true);
					dwgfx.bprint(5, 230, "Press M to mute in game", 164 - help.glow, 196 - help.glow, 164 - help.glow, true);
					*/
					dwgfx.render();
					//dwgfx.backbuffer.unlock();
				}else {
					switch(game.gamestate){
						case TITLEMODE:
							titlerender(key, dwgfx, map, game, obj, help);
						break;
						case GAMEMODE:
							if (map.towermode) {
								towerrender(key, dwgfx, game, map, obj, help);
							}else{
								gamerender(key, dwgfx, game, map, obj, help);
							}
						break;
						case CONTROLTUTORIALMODE:
							controltutorialrender(key, dwgfx, game, map, obj, help);
						break;
						case MAPMODE:
							maprender(key, dwgfx, game, map, obj, help);
						break;
						case TELEPORTERMODE:
							teleporterrender(key, dwgfx, game, map, obj, help);
						break;
						case GAMECOMPLETE:
							gamecompleterender(key, dwgfx, game, map, obj, help);
						break;
						case GAMECOMPLETE2:
							gamecompleterender2(key, dwgfx, game, obj, help);
						break;
						case CLICKTOSTART:
							//dwgfx.backbuffer.lock();
							//dwgfx.bprint(5, 115, "[Click to start]", 196 - help.glow, 196 - help.glow, 255 - help.glow, true);
							dwgfx.drawgui(help);
							dwgfx.render();
							//dwgfx.backbuffer.unlock();
						break;
					}
				}
			});
		}
		
		public function mainloop(e:TimerEvent):void {
			_current = getTimer();
			if (_last < 0) _last = _current;
			_delta += _current - _last;
			_last = _current;
			if (_delta >= _rate){
				_delta %= _skip;
				while (_delta >= _rate){
					_delta -= _rate;
					input();
					logic();
					if (key.hasclicked) key.click = false;
				}
				dorender();
				e.updateAfterEvent();
			}
		}
		
		public function androidresize():void {			
			/*if(immersivemode){
				AndroidFullScreen.stage = Starling.current.nativeStage; // Set this to your app's stage
				AndroidFullScreen.fullScreen();
				
				device.xres = AndroidFullScreen.immersiveWidth;
				device.yres = AndroidFullScreen.immersiveHeight;
			}else {*/
				Starling.current.nativeStage.displayState = StageDisplayState.FULL_SCREEN_INTERACTIVE;
				if(!device.localtesting){
					//device.xres = flash.system.Capabilities.screenResolutionX;
					//device.yres = flash.system.Capabilities.screenResolutionY;
					device.xres = Screen.mainScreen.safeArea.width;
					device.yres = Screen.mainScreen.safeArea.height;
				}else{
				  device.xres = 1280;
				  device.yres = 800;
				}
			//}
			
			if (device.xres < device.yres) {
				//Switch them!
				var t:int = device.yres;
				device.yres = device.xres;
				device.xres = t;
			}
			key.definestickrange(device.xres / 2, 0, 6);
			
			dwgfx.updatescreen(device.xres, device.yres);
			if(dwgfx.buttonsready) dwgfx.initbuttonpositions();
		}
		
		public function updategraphicsmode(game:gameclass, dwgfx:dwgraphicsclass):void {
			//ghost of a function that once was
		}
		
		// Timer information (a shout out to ChevyRay for the implementation)
		public static const TARGET_FPS:Number = 30; // the fixed-FPS we want the game to run at
		private var	_rate:Number = 1000 / TARGET_FPS; // how long (in seconds) each frame is
		private var	_skip:Number = _rate * 10; // this tells us to allow a maximum of 10 frame skips
		private var	_last:Number = -1;
		private var	_current:Number = 0;
		private var	_delta:Number = 0;
		private var	_timer:Timer = new Timer(4);
		
		
		public var dwgfx:dwgraphicsclass = new dwgraphicsclass();
		public var music:musicclass = new musicclass();
		public var help:helpclass = new helpclass();
		public var map:mapclass = new mapclass();
		public var game:gameclass;
		public var obj:entityclass = new entityclass();
		public var key:KeyPoll;
		public var script:scriptclass = new scriptclass();
		
		public var slogo:MovieClip;
		public var logoposition:Matrix;
		public var pixel:uint; public var pixel2:uint;
		public var pi:uint, pj:uint;
		public var i:int, j:int, k:int, temp:int, tempx:int, tempy:int, tempstring:String;
		public var tr:int, tg:int, tb:int, t:int;
		public var tvel:Number;
		public var immersivemode:Boolean;
		
		[Embed(source="../data/c64.fnt", mimeType="application/octet-stream")]
		public static const c64font_xml:Class;
		
				//Music
		[Embed(source = '../data/music/levelcomplete.mp3')]	private var music_1:Class;
		[Embed(source = '../data/music/endgame.mp3')]	private var music_2:Class;
		//Sound effects
		[Embed(source = '../data/sounds/jump.mp3')]	private var ef_0:Class;
		[Embed(source = '../data/sounds/jump2.mp3')]	private var ef_1:Class;
		[Embed(source = '../data/sounds/hurt.mp3')]	private var ef_2:Class;
		[Embed(source = '../data/sounds/souleyeminijingle.mp3')]	private var ef_3:Class;
		[Embed(source = '../data/sounds/coin.mp3')]	private var ef_4:Class;
		[Embed(source = '../data/sounds/save.mp3')]	private var ef_5:Class;
		[Embed(source = '../data/sounds/crumble.mp3')]	private var ef_6:Class;
		[Embed(source = '../data/sounds/vanish.mp3')]	private var ef_7:Class;
		[Embed(source = '../data/sounds/blip.mp3')]	private var ef_8:Class;
		[Embed(source = '../data/sounds/preteleport.mp3')]	private var ef_9:Class;
		[Embed(source = '../data/sounds/teleport.mp3')]	private var ef_10:Class;
		[Embed(source = '../data/sounds/crew1.mp3')]	private var ef_11:Class;
		[Embed(source = '../data/sounds/crew2.mp3')]	private var ef_12:Class;
		[Embed(source = '../data/sounds/crew3.mp3')]	private var ef_13:Class;
		[Embed(source = '../data/sounds/crew4.mp3')]	private var ef_14:Class;
		[Embed(source = '../data/sounds/crew5.mp3')]	private var ef_15:Class;
		[Embed(source = '../data/sounds/crew6.mp3')]	private var ef_16:Class;
		[Embed(source = '../data/sounds/terminal.mp3')]	private var ef_17:Class;
		[Embed(source = '../data/sounds/gamesaved.mp3')]	private var ef_18:Class;
		[Embed(source = '../data/sounds/crashing.mp3')]	private var ef_19:Class;
		[Embed(source = '../data/sounds/blip2.mp3')]	private var ef_20:Class;
		[Embed(source = '../data/sounds/countdown.mp3')]	private var ef_21:Class;
		[Embed(source = '../data/sounds/go.mp3')]	private var ef_22:Class;
		[Embed(source = '../data/sounds/crash.mp3')]	private var ef_23:Class;
		[Embed(source = '../data/sounds/combine.mp3')]	private var ef_24:Class;
		[Embed(source = '../data/sounds/newrecord.mp3')]	private var ef_25:Class;
		[Embed(source = '../data/sounds/trophy.mp3')]	private var ef_26:Class;
		[Embed(source = '../data/sounds/rescue.mp3')]	private var ef_27:Class;
	}
}	