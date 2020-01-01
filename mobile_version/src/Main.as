package{
	import flash.display.*;
	import flash.geom.*;
  import flash.events.*;
  import flash.net.*;
	import flash.media.*;
	import flash.text.*;
  import flash.ui.ContextMenu;
  import flash.ui.ContextMenuItem;
  import flash.events.ContextMenuEvent;
	import flash.ui.Keyboard;
  import flash.ui.Mouse;
	import bigroom.input.KeyPoll; 
	import flash.system.fscommand;  
  import flash.system.Capabilities;  
	import flash.utils.getTimer;
	import flash.utils.Timer;
	//import com.mesmotronic.ane.AndroidFullScreen;
	
	//import com.sociodox.theminer.TheMiner; //Profiler
  
	//Real value
	//[SWF(width = "1136", height = "768", frameRate = "30", backgroundColor = "#000000")] //Set the size and color of the Flash file
	//Big frame for big androids!
	//[SWF(width = "3000", height = "2000", frameRate = "30", backgroundColor = "#000000")] //Set the size and color of the Flash file
	[SWF(frameRate = "30", backgroundColor = "#000000")] //Set the size and color of the Flash file
	//iPad test
	//[SWF(width = "1024", height = "768", frameRate="60", backgroundColor = "#000000")] //Set the size and color of the Flash file
	//iPhone test
	//[SWF(width = "480", height = "320", frameRate="30", backgroundColor = "#000000")] //Set the size and color of the Flash file
	//[SWF(width = "960", height = "640", frameRate="30", backgroundColor = "#000000")] //Set the size and color of the Flash file
	
	public class Main extends Sprite {
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
		
		public function Main():void {
			if (stage) gameinit();
			else addEventListener(Event.ADDED_TO_STAGE, gameinit);
		}
		
		private function gameinit(e:Event = null):void {
			removeEventListener(Event.ADDED_TO_STAGE, gameinit);
			// entry point
			var tempbmp:Bitmap;
			
			this.mouseEnabled = false;
      this.mouseChildren = false;
			
			//Ok: quick security check to make sure it doesn't get posted about
			if (sitelock()) {
				key = new KeyPoll(stage);
				
				//ANDROID SPECIFIC CODE HERE
				/*
				if (AndroidFullScreen.isSupported) {
					if (!AndroidFullScreen.immersiveMode()){
						stage.displayState = StageDisplayState.FULL_SCREEN_INTERACTIVE;
						device.xres = flash.system.Capabilities.screenResolutionX;
						device.yres = flash.system.Capabilities.screenResolutionY;
						immersivemode = false;
					}else {
						device.xres = stage.stageWidth;
						device.yres = stage.stageHeight;
						immersivemode = true;
					}	
				}else {
				*/
					stage.displayState = StageDisplayState.FULL_SCREEN_INTERACTIVE;
					device.xres = flash.system.Capabilities.screenResolutionX;
					device.yres = flash.system.Capabilities.screenResolutionY;
					immersivemode = false;
				//}
				
				/*
				if (immersivemode) {
					stage.addEventListener(Event.RESIZE, androidresize);
				}
				*/
				/*
				trace("NOTE: REMEMBER TO FIX RESOLUTION SUPPORT BEFORE BUILDING");
				device.xres = 1024;
				device.yres = 768;
				device.deviceresolution = device.IPAD;
				*/
				//Testing:Let's be an ipad
				/*
				device.xres = 1024;
				device.yres = 768;
				device.deviceresolution = device.IPAD;
				*/
				/*
				device.xres = 480;
				device.yres = 320;
				device.deviceresolution = device.IPHONE;
				*/
				/*
				device.xres = 960;
				device.yres = 640;
				device.deviceresolution = device.IPHONE;
				*/
				
				editor.init(); //Load in all the levels
				
				if (device.xres < device.yres) {
					//Switch them!
					t = device.yres;
					device.yres = device.xres;
					device.xres = t;
				}				
				
				if (device.yres < 768) {
				  device.deviceresolution = device.IPHONE;
				}else {
					device.deviceresolution = device.IPAD;
				}
				
				//Manually enable androids here! Mostly just changed buttons sizes
			//	device.deviceresolution = device.ANDROID;
				
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
				//First we init the class and add its display list to the main display list
				
				dwgfx.buttonimg.push(new BitmapData(92, 30, true, 0x00000000));
				dwgfx.buttonimg.push(new BitmapData(92, 30, true, 0x00000000));
				dwgfx.buttonimg.push(new BitmapData(40, 40, true, 0x00000000));
				dwgfx.buttonimg.push(new BitmapData(40, 40, true, 0x00000000));
				tempbmp = new im_button_0(); dwgfx.buffer = tempbmp.bitmapData; dwgfx.addbutton();
				tempbmp = new im_button_1(); dwgfx.buffer = tempbmp.bitmapData; dwgfx.addbutton();
				tempbmp = new im_button_2(); dwgfx.buffer = tempbmp.bitmapData; dwgfx.addbutton();
				tempbmp = new im_button_3(); dwgfx.buffer = tempbmp.bitmapData; dwgfx.addbutton();
				tempbmp = new im_button_4(); dwgfx.buffer = tempbmp.bitmapData; dwgfx.addbutton();
				tempbmp = new im_button_5(); dwgfx.buffer = tempbmp.bitmapData; dwgfx.addbutton();
				tempbmp = new im_button_6(); dwgfx.buffer = tempbmp.bitmapData; dwgfx.addbutton();
				tempbmp = new im_button_7(); dwgfx.buffer = tempbmp.bitmapData; dwgfx.addbutton();
				tempbmp = new im_button_8(); dwgfx.buffer = tempbmp.bitmapData; dwgfx.addbutton();
				tempbmp = new im_button_9(); dwgfx.buffer = tempbmp.bitmapData; dwgfx.addbutton();
				tempbmp = new im_button_10(); dwgfx.buffer = tempbmp.bitmapData; dwgfx.addbutton();
				tempbmp = new im_button_11(); dwgfx.buffer = tempbmp.bitmapData; dwgfx.addbutton();
				tempbmp = new im_button_12(); dwgfx.buffer = tempbmp.bitmapData; dwgfx.addbutton();
				
				dwgfx.init();
				
				//We load all our graphics in:
				tempbmp = new im_tiles();   	 dwgfx.buffer = tempbmp.bitmapData;	dwgfx.maketilearray();
				tempbmp = new im_tiles2();   	 dwgfx.buffer = tempbmp.bitmapData;	dwgfx.maketile2array();
				tempbmp = new im_tiles3();   	 dwgfx.buffer = tempbmp.bitmapData;	dwgfx.maketile3array();
				tempbmp = new im_sprites();	   dwgfx.buffer = tempbmp.bitmapData;	dwgfx.makespritearray();
				tempbmp = new im_flipsprites();	   dwgfx.buffer = tempbmp.bitmapData;	dwgfx.makeflipspritearray();
				tempbmp = new im_bfont();	     dwgfx.buffer = tempbmp.bitmapData;	dwgfx.makebfont();
				tempbmp = new im_bfontmask();	 dwgfx.buffer = tempbmp.bitmapData;	dwgfx.makebfontmask();
				tempbmp = new im_teleporter(); dwgfx.buffer = tempbmp.bitmapData;	dwgfx.maketelearray();
				tempbmp = new im_entcolours(); dwgfx.buffer = tempbmp.bitmapData;	dwgfx.makeentcolourarray();
				//Load in the images
				tempbmp = new im_image0();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addimage(); // 0
				tempbmp = new im_image1();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addimage(); // 1 (this is the minimap!)
				tempbmp = new im_image2();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addimage(); // 2
				tempbmp = new im_image3();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addimage(); // 3
				tempbmp = new im_image4();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addimage(); // 4
				tempbmp = new im_image5();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addimage(); // 5
				tempbmp = new im_image6();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addimage(); // 6
				tempbmp = new im_image7();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addimage(); // 7
				tempbmp = new im_image8();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addimage(); // 8
				tempbmp = new im_image9();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addimage(); // 9
				tempbmp = new im_image10();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addimage(); // 10
				
				tempbmp = new im_image1();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addimage(); // Minimap
				tempbmp = new im_image1();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addimage(); // Minimap
				
				tempbmp = new im_mobileimage1();	dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addmobileimage();
				tempbmp = new im_mobileimage2();	dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addmobileimage();
				tempbmp = new im_mobileimage3();	dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addmobileimage();
				tempbmp = new im_mobileimage4();	dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addmobileimage();
				tempbmp = new im_mobileimage5();	dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addmobileimage();
				tempbmp = new im_mobileimage6();	dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addmobileimage();
				tempbmp = new im_mobileimage7();	dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addmobileimage();
				tempbmp = new im_mobileimage8();	dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addmobileimage();
				tempbmp = new im_mobileimage9();	dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addmobileimage();				
				
				tempbmp = new im_imgplayerlevel0();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addplayerlevelimage();
				tempbmp = new im_imgplayerlevel1();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addplayerlevelimage();
				tempbmp = new im_imgplayerlevel2();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addplayerlevelimage();
				tempbmp = new im_imgplayerlevel3();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addplayerlevelimage();
				tempbmp = new im_imgplayerlevel4();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addplayerlevelimage();
				tempbmp = new im_imgplayerlevel5();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addplayerlevelimage();
				tempbmp = new im_imgplayerlevel6();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addplayerlevelimage();
				tempbmp = new im_imgplayerlevel7();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addplayerlevelimage();
				tempbmp = new im_imgplayerlevel8();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addplayerlevelimage();
				tempbmp = new im_imgplayerlevel9();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addplayerlevelimage();
				tempbmp = new im_imgplayerlevel10();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addplayerlevelimage();
				tempbmp = new im_imgplayerlevel11();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addplayerlevelimage();
				tempbmp = new im_imgplayerlevel12();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addplayerlevelimage();
				tempbmp = new im_imgplayerlevel13();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addplayerlevelimage();
				tempbmp = new im_imgplayerlevel14();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addplayerlevelimage();
				tempbmp = new im_imgplayerlevel15();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addplayerlevelimage();
				tempbmp = new im_imgplayerlevel16();	  dwgfx.buffer = tempbmp.bitmapData;	dwgfx.addplayerlevelimage();
				
				//Now that the graphics are loaded, init the background buffer
				dwgfx.buffer=new BitmapData(320,240,false,0x000000);
				
				stage.scaleMode = StageScaleMode.NO_SCALE;
				stage.align = StageAlign.TOP_LEFT;
				stage.quality = StageQuality.LOW;
				
				//EXACT FIT
				dwgfx.screensizemultiplier = device.yres / 240;
				dwgfx.screen.width = 320 * dwgfx.screensizemultiplier;
        dwgfx.screen.height = device.yres;
				
				dwgfx.screen.x = (device.xres / 2) - (320 * dwgfx.screensizemultiplier / 2);
				dwgfx.screen.y = 0;
				
				dwgfx.initbuttonpositions();
				
				//PIXEL PERFECT
				/*
				dwgfx.screensizemultiplier = int(device.yres / 240);
				dwgfx.screen.width = 320 * dwgfx.screensizemultiplier;
        dwgfx.screen.height = 240 * dwgfx.screensizemultiplier;
				
				dwgfx.screenoffx = (device.xres / 2) - (320 * dwgfx.screensizemultiplier / 2);
				dwgfx.screenoffy = (device.yres / 2) - (240 * dwgfx.screensizemultiplier / 2);
				dwgfx.screen.x = dwgfx.screenoffx;
				dwgfx.screen.y = dwgfx.screenoffy;
				*/
				
				addChild(dwgfx);  
				//stage.addChild(new TheMiner()); //Profiler
				
				
				//Iphone orientation fix
				var startOrientation:String = stage.orientation;
				if (startOrientation == StageOrientation.DEFAULT || startOrientation == StageOrientation.UPSIDE_DOWN){
					//stage.setOrientation(StageOrientation.ROTATED_RIGHT);
					stage.setAspectRatio(StageAspectRatio.LANDSCAPE);
				}else{
					stage.setOrientation(startOrientation);
				}                    
        stage.addEventListener(StageOrientationEvent.ORIENTATION_CHANGING, orientationChangeListener);
				
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
				
				// start the tick-timer, which updates roughly every 4 milliseconds
			  _timer.addEventListener(TimerEvent.TIMER, mainloop);
			  _timer.start();
			}else {
				dwgfx.init();
				addChild(dwgfx);
				//We load the font in:
				tempbmp = new im_bfont();	dwgfx.buffer = tempbmp.bitmapData;	dwgfx.makebfont();
				tempbmp = new im_bfontmask();	dwgfx.buffer = tempbmp.bitmapData;	dwgfx.makebfontmask();
				//Now that the graphics are loaded, init the background buffer
				dwgfx.buffer = new BitmapData(320, 240, false, 0x000000);
				
				addEventListener(Event.ENTER_FRAME, lockedloop);
			}
		}
		
		public function visit_distractionware(e:Event):void{
      var distractionware_link:URLRequest = new URLRequest( "http://www.distractionware.com" );
      navigateToURL( distractionware_link, "_blank" );
    }
		
		public function visit_sponsor(e:Event):void{
      var sponsor_link:URLRequest = new URLRequest( "http://www.kongregate.com/?gamereferral=dontlookback" );
      navigateToURL( sponsor_link, "_blank" );
    }
		
		public function visit_sponsor_logo():void{
      var sponsor_link:URLRequest = new URLRequest( "http://www.kongregate.com/?gamereferral=dontlookback" );
      navigateToURL( sponsor_link, "_blank" );
    }
		
		/*
		public function androidresize(e:Event):void {
			if (immersivemode) {
				device.xres = stage.stageWidth;
				device.yres = stage.stageHeight;
			}else{
				device.xres = flash.system.Capabilities.screenResolutionX;
				device.yres = flash.system.Capabilities.screenResolutionY;
			}
			if (device.xres < device.yres) {
				//Switch them!
				t = device.yres;
				device.yres = device.xres;
				device.xres = t;
			}
			
			key.definestickrange(device.xres / 2, 0, 6);
			
			dwgfx.screensizemultiplier = device.yres / 240;
			dwgfx.screen.width = 320 * dwgfx.screensizemultiplier;
			dwgfx.screen.height = device.yres;
			
			dwgfx.screen.x = (device.xres / 2) - (320 * dwgfx.screensizemultiplier / 2);
			dwgfx.screen.y = 0;
			
			dwgfx.initbuttonpositions();
		}
		*/
		
		public function orientationChangeListener(e:StageOrientationEvent):void{
      if (e.afterOrientation == StageOrientation.DEFAULT || e.afterOrientation ==  StageOrientation.UPSIDE_DOWN) {
        e.preventDefault();
      }
    }
		
		public function lockedloop(e:Event):void {
		  dwgfx.backbuffer.lock();
			
  		dwgfx.bprint(5, 110, "Sorry! This game can only be", 196-help.glow, 196-help.glow, 255-help.glow, true);
			dwgfx.bprint(5, 120, "played on thelettervsixtim.es", 196-help.glow, 196-help.glow, 255-help.glow, true);
			dwgfx.render();
	    dwgfx.backbuffer.unlock();
			
			help.updateglow();
		}
		
		public function setstage(w:int, h:int):void {
			//stage.stageWidth = w;
			//stage.stageHeight = h;
		}
		
		public function sitelock():Boolean {
			//No preloader for Kong version
			var currUrl:String = stage.loaderInfo.url.toLowerCase();
			//chat.kongregate.com
			if ((currUrl.indexOf("ile:///") <= 0) || (currUrl.indexOf("http") == 0)){
			//if ((currUrl.indexOf("distractionware.com/games") <= 0) && (currUrl.indexOf("thelettervsixtim.es/secretarea") <= 0)){
				return true;
				//return false;
			}else{
				return true;
			}
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
						if(immersivemode){
							//AndroidFullScreen.immersiveMode();
						}
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
		
		public function render():void {
			if (!game.infocus) {
				dwgfx.backbuffer.lock();
				/*
				dwgfx.bprint(5, 110, "Game paused", 196 - help.glow, 255 - help.glow, 196 - help.glow, true);
				dwgfx.bprint(5, 120, "[click to resume]", 196 - help.glow, 255 - help.glow, 196 - help.glow, true);
				dwgfx.bprint(5, 230, "Press M to mute in game", 164 - help.glow, 196 - help.glow, 164 - help.glow, true);
				*/
				dwgfx.render();
				dwgfx.backbuffer.unlock();
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
						gamecompleterender(key, dwgfx, game, obj, help);
					break;
					case GAMECOMPLETE2:
						gamecompleterender2(key, dwgfx, game, obj, help);
					break;
					case CLICKTOSTART:
						dwgfx.backbuffer.lock();
				    //dwgfx.bprint(5, 115, "[Click to start]", 196 - help.glow, 196 - help.glow, 255 - help.glow, true);
						dwgfx.drawgui(help);
						dwgfx.render();
						dwgfx.backbuffer.unlock();
					break;
				}
			}
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
				render();
				e.updateAfterEvent();
			}
		}
		
		public function setzoom(t:int, dwgfx:dwgraphicsclass):void {
			/*
			switch(t) {
				case 1:
				  dwgfx.screen.width = 320;
		      dwgfx.screen.height = 240;
		      dwgfx.screen.x = (640 - 320) / 2;
					dwgfx.screen.y = (480 - 240) / 2;
				break;		
				case 2:
				  dwgfx.screen.width = 640;
		      dwgfx.screen.height = 480;
		      dwgfx.screen.x = 0;
					dwgfx.screen.y = 0;
				break;
				case 3:
				  dwgfx.screen.width = 960;
		      dwgfx.screen.height = 720;
		      dwgfx.screen.x = (640 - 960) / 2;
					dwgfx.screen.y = (480 - 720) / 2;
				break;
				case 4:
				  dwgfx.screen.width = 1280;
		      dwgfx.screen.height = 960;
		      dwgfx.screen.x = (640 - 1280) / 2;
					dwgfx.screen.y = (480 - 960) / 2;
				break;
			}		
			*/
		}
		
		public function updategraphicsmode(game:gameclass, dwgfx:dwgraphicsclass):void {
			/*
		  swfStage = stage;
			
			if (game.advanced_mode) {	//advanced graphics mode
				//Screen Smooting
				dwgfx.screen.smoothing = game.advanced_smoothing;
				
				//Scaling
				if(game.advanced_scaling==0){
					swfStage.scaleMode = StageScaleMode.SHOW_ALL;
				  setzoom(2, dwgfx);
				}else{
			    swfStage.scaleMode = StageScaleMode.NO_SCALE;  //Turn Scaling off
				  setzoom(game.advanced_scaling, dwgfx);
				}
				 
				stage.fullScreenSourceRect = null;
				//Fullscreen
				if (game.fullscreen) {
				  stage.displayState = StageDisplayState.FULL_SCREEN;
					Mouse.hide();
				}else {
					stage.displayState = StageDisplayState.NORMAL;
					Mouse.show();
				}
			}else{
				swfStage.scaleMode = StageScaleMode.SHOW_ALL;
				if (game.fullscreen) {
					dwgfx.screen.smoothing = true;
					stage.fullScreenSourceRect = new Rectangle(0, 0, 640, 480);
					stage.displayState = StageDisplayState.FULL_SCREEN;
					Mouse.hide();
				}else {
					dwgfx.screen.smoothing = false;
					stage.displayState = StageDisplayState.NORMAL;
					Mouse.show();
				}
			}
			*/
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
		public var swfStage:Stage;
		public var immersivemode:Boolean;
		//Embedded resources:		
		//Graphics
		[Embed(source = '../data/graphics/tiles.png')]	private var im_tiles:Class;
		[Embed(source = '../data/graphics/tiles2.png')]	private var im_tiles2:Class;
		[Embed(source = '../data/graphics/tiles3.png')]	private var im_tiles3:Class;
		[Embed(source = '../data/graphics/sprites.png')]	private var im_sprites:Class;
		[Embed(source = '../data/graphics/flipsprites.png')]	private var im_flipsprites:Class;
		[Embed(source = '../data/graphics/font.png')]	private var im_bfont:Class;
		[Embed(source = '../data/graphics/fontmask.png')]	private var im_bfontmask:Class;
		[Embed(source = '../data/graphics/teleporter.png')]	private var im_teleporter:Class;
		[Embed(source = '../data/graphics/entcolours.png')]	private var im_entcolours:Class;
		
		[Embed(source = '../data/graphics/levelcomplete.png')]	private var im_image0:Class;
		[Embed(source = '../data/graphics/minimap.png')]	private var im_image1:Class;
		[Embed(source = '../data/graphics/covered.png')]	private var im_image2:Class;
		[Embed(source = '../data/graphics/elephant.png')]	private var im_image3:Class;
		[Embed(source = '../data/graphics/gamecomplete.png')]	private var im_image4:Class;
		[Embed(source = '../data/graphics/fliplevelcomplete.png')]	private var im_image5:Class;
		[Embed(source = '../data/graphics/flipgamecomplete.png')]	private var im_image6:Class;
		[Embed(source = '../data/graphics/site.png')]	private var im_image7:Class;
		[Embed(source = '../data/graphics/site2.png')]	private var im_image8:Class;
		[Embed(source = '../data/graphics/site3.png')]	private var im_image9:Class;
		[Embed(source = '../data/graphics/ending.png')]	private var im_image10:Class;
		
		[Embed(source = '../data/graphics/controls/touchscreen.png')]	private var im_mobileimage1:Class;
		[Embed(source = '../data/graphics/controls/lefthand_off.png')]	private var im_mobileimage2:Class;
		[Embed(source = '../data/graphics/controls/lefthand_near.png')]	private var im_mobileimage3:Class;
		[Embed(source = '../data/graphics/controls/lefthand_far.png')]	private var im_mobileimage4:Class;
		[Embed(source = '../data/graphics/controls/righthand_off.png')]	private var im_mobileimage5:Class;
		[Embed(source = '../data/graphics/controls/righthand_near.png')]	private var im_mobileimage6:Class;
		[Embed(source = '../data/graphics/controls/righthand_far.png')]	private var im_mobileimage7:Class;
		[Embed(source = '../data/graphics/controls/arrowleft.png')]	private var im_mobileimage8:Class;
		[Embed(source = '../data/graphics/controls/arrowright.png')]	private var im_mobileimage9:Class;
		//Playerlevel images
		[Embed(source = '../data/graphics/playerlevels/playerlevel_0.png')]	private var im_imgplayerlevel0:Class;
		[Embed(source = '../data/graphics/playerlevels/playerlevel_1.png')]	private var im_imgplayerlevel1:Class;
		[Embed(source = '../data/graphics/playerlevels/playerlevel_2.png')]	private var im_imgplayerlevel2:Class;
		[Embed(source = '../data/graphics/playerlevels/playerlevel_3.png')]	private var im_imgplayerlevel3:Class;
		[Embed(source = '../data/graphics/playerlevels/playerlevel_4.png')]	private var im_imgplayerlevel4:Class;
		[Embed(source = '../data/graphics/playerlevels/playerlevel_5.png')]	private var im_imgplayerlevel5:Class;
		[Embed(source = '../data/graphics/playerlevels/playerlevel_6.png')]	private var im_imgplayerlevel6:Class;
		[Embed(source = '../data/graphics/playerlevels/playerlevel_7.png')]	private var im_imgplayerlevel7:Class;
		[Embed(source = '../data/graphics/playerlevels/playerlevel_8.png')]	private var im_imgplayerlevel8:Class;
		[Embed(source = '../data/graphics/playerlevels/playerlevel_9.png')]	private var im_imgplayerlevel9:Class;
		[Embed(source = '../data/graphics/playerlevels/playerlevel_10.png')]	private var im_imgplayerlevel10:Class;
		[Embed(source = '../data/graphics/playerlevels/playerlevel_11.png')]	private var im_imgplayerlevel11:Class;
		[Embed(source = '../data/graphics/playerlevels/playerlevel_12.png')]	private var im_imgplayerlevel12:Class;
		[Embed(source = '../data/graphics/playerlevels/playerlevel_13.png')]	private var im_imgplayerlevel13:Class;
		[Embed(source = '../data/graphics/playerlevels/playerlevel_14.png')]	private var im_imgplayerlevel14:Class;
		[Embed(source = '../data/graphics/playerlevels/playerlevel_15.png')]	private var im_imgplayerlevel15:Class;
		[Embed(source = '../data/graphics/playerlevels/playerlevel_16.png')]	private var im_imgplayerlevel16:Class;
		//iOS Buttons
		[Embed(source = '../data/graphics/mobilebuttons/map.png')]	private var im_button_0:Class;
		[Embed(source = '../data/graphics/mobilebuttons/back.png')]	private var im_button_1:Class;
		[Embed(source = '../data/graphics/mobilebuttons/talk_1.png')]	private var im_button_2:Class;
		[Embed(source = '../data/graphics/mobilebuttons/talk_2.png')]	private var im_button_3:Class;
		[Embed(source = '../data/graphics/mobilebuttons/use_1.png')]	private var im_button_4:Class;
		[Embed(source = '../data/graphics/mobilebuttons/use_2.png')]	private var im_button_5:Class;
		[Embed(source = '../data/graphics/mobilebuttons/teleport_1.png')]	private var im_button_6:Class;
		[Embed(source = '../data/graphics/mobilebuttons/teleport_2.png')]	private var im_button_7:Class;
		[Embed(source = '../data/graphics/mobilebuttons/controls_1.png')]	private var im_button_8:Class;
		[Embed(source = '../data/graphics/mobilebuttons/controls_2.png')]	private var im_button_9:Class;
		[Embed(source = '../data/graphics/mobilebuttons/gamecenter.png')]	private var im_button_10:Class;
		[Embed(source = '../data/graphics/mobilebuttons/button_left.png')]	private var im_button_11:Class;
		[Embed(source = '../data/graphics/mobilebuttons/button_right.png')]	private var im_button_12:Class;
		
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