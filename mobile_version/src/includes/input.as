public function recordinput(key:KeyPoll, dwgfx:dwgraphicsclass, game:gameclass, map:mapclass,
                          obj:entityclass, help:helpclass, music:musicclass):void {
  //record your input and add it to the record string
	//Keys are:
	//0 - nothing
	//1 - left
	//2 - right
	//3 - left+right
	//4 - flip
	//5 - left+flip
	//6 - right+flip
	//7 - left+right+flip
	//8 - Map/teleport
	
	if (!game.recordinit) {
		//Init recording
		game.recordinit = true;
		game.combomode = false; game.comboaction = "x"; game.combolen = 0; 
		game.currentaction = "x";
	}
	
	game.press_left = false; game.press_right = false; game.press_action = false; game.press_map = false;
	
	if (key.isDown(Keyboard.LEFT)) game.press_left = true;
	if (key.isDown(Keyboard.RIGHT)) game.press_right = true;
	if (key.isDown(90) || key.isDown(32) || key.isDown(86)
	   || key.isDown(Keyboard.UP) || key.isDown(Keyboard.DOWN)) game.press_action = true;
	if (key.isDown(Keyboard.ENTER) || key.isDown(Keyboard.BACK)) game.press_map = true;
	
	if (game.press_map) {
		game.currentaction = "8";
	}else{
		if (game.press_left) {
			if (game.press_right) {
				if (game.press_action) { game.currentaction = "7";
				}else {	game.currentaction = "3";
				}
			}else {
				if (game.press_action) { game.currentaction = "5";
				}else {	game.currentaction = "1";
				}
			}
		}else {
			if (game.press_right) {
				if (game.press_action) { game.currentaction = "6";
				}else {	game.currentaction = "2";
				}
			}else {
				if (game.press_action) { game.currentaction = "4";
				}else {	game.currentaction = "0";
				}
			}
		}
	}
	
	if (game.comboaction != game.currentaction) {
		//we're doing something different, output and move on
		if (game.comboaction == "x") {
			//init
			game.recordstring += String(game.savex) + "," + String(game.savey) + ","
			                    +String(game.saverx) + "," + String(game.savery) +"," + String(game.savegc) + ",";
		}else{
		  game.recordstring += String(game.combolen+1) + "," + game.comboaction + ",";
		}
		
		game.combolen = 1;
		game.comboaction = game.currentaction;
	}else {
		//add to the combo!
		game.combolen++;
	}
	
	if (game.currentaction == "8") {
		//teleporter - we can't combo this!
		game.recordstring += "1," + game.currentaction + ",";
	}
	
	gameinput(key, dwgfx, game, map, obj, help, music);
}

public function titleinput(key:KeyPoll, dwgfx:dwgraphicsclass, map:mapclass, game:gameclass, obj:entityclass, help:helpclass, music:musicclass):void {
	game.inittouchpoints(key);
	
	game.press_left = false; game.press_right = false; game.press_action = false; game.press_map = false;
	
	if ((key.isDown(15) || key.isDown(17)) && key.isDown(70) && !game.fullscreentoggleheld) {
		//Toggle fullscreen
		game.fullscreentoggleheld = true;
		if (game.fullscreen) {game.fullscreen = false;
		}else {game.fullscreen = true;}
		updategraphicsmode(game, dwgfx);
			
		game.savestats(map, dwgfx);
	}
	
	if ((key.isDown(15) || key.isDown(17)) && key.isDown(70)) {
		game.fullscreentoggleheld = true;
	}else {
		game.fullscreentoggleheld = false;
	}
	
	if ((key.isDown(15) || key.isDown(17)) && key.isDown(70)) {
		game.fullscreentoggleheld = true;
	}else {
		game.fullscreentoggleheld = false;
	}
	
	if (dwgfx.flipmode) {
		if (key.isDown(Keyboard.LEFT) || key.isDown(Keyboard.DOWN) || key.isDown(65) ||  key.isDown(83)) game.press_left = true;
		if (key.isDown(Keyboard.RIGHT) || key.isDown(Keyboard.UP)  || key.isDown(68) ||  key.isDown(87)) game.press_right = true;
	}else{
		if (key.isDown(Keyboard.LEFT) || key.isDown(Keyboard.UP) || key.isDown(65) ||  key.isDown(87)) game.press_left = true;
		if (key.isDown(Keyboard.RIGHT) || key.isDown(Keyboard.DOWN)  || key.isDown(68) ||  key.isDown(83)) game.press_right = true;
	}
	if (key.isDown(90) || key.isDown(32) || key.isDown(86)) game.press_action = true;
	   //|| key.isDown(Keyboard.UP) || key.isDown(Keyboard.DOWN)) game.press_action = true; //on menus, up and down don't work as action
	if (key.isDown(Keyboard.ENTER) || key.isDown(Keyboard.BACK)) game.press_map = true;

	game.gettouchpoints_mainmenu(key, dwgfx);
	
	//In the menu system, all keypresses are single taps rather than holds. Therefore this test has to be done for all presses
	if (!game.press_action && !game.press_left && !game.press_right) game.jumpheld = false;
	if (!game.press_map) game.mapheld = false;
	
	if (!game.jumpheld && dwgfx.fademode==0) {
  	if (game.press_action || game.press_left || game.press_right || game.press_map){
		  game.jumpheld = true;
  	}
		
		/*
		if (game.press_left) {
			game.mainmenu--;
		}else if (game.press_right) {
			game.mainmenu++;
		}
		
		if (game.mainmenu < 0) game.mainmenu = 2;
		if (game.mainmenu > 2 ) game.mainmenu = 0;
		*/
		
				
		/*
		if (game.press_action) {
			if (!game.menustart) {
				game.menustart = true;
				music.play(6);
				music.playef(18, 10);
				game.screenshake = 10;
				game.flashlight = 5;
			}else{
				if(game.mainmenu==0){
					dwgfx.fademode = 2;
				}else if (game.mainmenu == 1) {
					if (game.telesummary != "") {
						dwgfx.fademode = 2;
					}
				}else if (game.mainmenu == 2) {
					if (game.quicksummary != "") {
						dwgfx.fademode = 2;
					}
				}
			}
	  }
		*/
		
	  if (key.isDown(27) && game.currentmenuname != "youwannaquit" && game.menustart) { 
			music.playef(11, 10);
			game.previousmenuname = game.currentmenuname;
			game.createmenu("youwannaquit");
			map.nexttowercolour();
		}
		
		if(game.menustart){
			if (game.press_left) {
				game.currentmenuoption--;
			}else if (game.press_right) {
				game.currentmenuoption++;
			}
		}
		
		if (game.currentmenuoption < 0) game.currentmenuoption = game.nummenuoptions-1;
		if (game.currentmenuoption >= game.nummenuoptions ) game.currentmenuoption = 0;

		if (game.press_action) {
			if (!game.menustart) {
				game.menustart = true;
				music.play(6);
				music.playef(18, 10);
				game.screenshake = 10;
				game.flashlight = 5;
				map.colstate = 10;
				map.nexttowercolour();
				
				game.loadstats(map, dwgfx);
				if (game.slowdown == 0) game.slowdown = 30;
				//stage.frameRate = game.slowdown;
			}else{
				if (game.currentmenuname == "mainmenu") {
					if (game.mobilemenu) {
						if (game.currentmenuoption == 0) {
							//Play
							if (game.telesummary == "" && game.quicksummary == "") {
								//No saves exist, just start a new game
								game.mainmenu = 0;  dwgfx.fademode = 2;
							}else {
								//Bring you to the normal playmenu
								music.playef(11, 10);
								game.createmenu("play");
								map.nexttowercolour();
							}
						}else if (game.currentmenuoption == 1) {
							/*
							//Bring you to the normal playmenu
							music.playef(11, 10);
							game.createmenu("playerworlds");
							map.nexttowercolour();
							*/
							music.playef(11, 10);
							game.levelpage=0;
							editor.getDirectoryData();
							game.loadcustomlevelstats(); //Should only load a file if it's needed
							game.createmenu("levellist");
							map.nexttowercolour();
						/*}else if (game.currentmenuoption == 2) {
							//Options
							music.playef(11, 10);
							game.createmenu("supergravitron");
							map.nexttowercolour();*/
						}else if (game.currentmenuoption == 2) {
							//Credits
							music.playef(11, 10);
							game.createmenu("options");
							map.nexttowercolour();
						}else if (game.currentmenuoption == 3) {
							//Credits
							music.playef(11, 10);
							game.createmenu("credits");
							map.nexttowercolour();
						}else if (game.currentmenuoption == 4) {
							//More games (external link)
							music.playef(11, 10);
							var distractionware_link:URLRequest = new URLRequest( "http://distractionware.com/games/ios/" );
							//var distractionware_link:URLRequest = new URLRequest( "http://distractionware.com/games/android/" );
							navigateToURL( distractionware_link, "_blank" );
						}
					}else{
						if (game.currentmenuoption == 0) {
							//Play
							if (game.telesummary == "" && game.quicksummary == "") {
								//No saves exist, just start a new game
								game.mainmenu = 0;  dwgfx.fademode = 2;
							}else {
								//Bring you to the normal playmenu
								music.playef(11, 10);
								game.createmenu("play");
								map.nexttowercolour();
							}
						/*}else if (game.currentmenuoption == 1) {
							//Options
							music.playef(11, 10);
							game.createmenu("graphicoptions");
							map.nexttowercolour();
							*/
						}else if (game.currentmenuoption == 1){
							//Bring you to the normal playmenu
							music.playef(11, 10);
							game.createmenu("playerworlds");
							map.nexttowercolour();
						}else if (game.currentmenuoption == 2) {
							//Options
													//Options
													music.playef(11, 10);
													game.createmenu("graphicoptions");
													map.nexttowercolour();
						}else if (game.currentmenuoption == 3) {
							//Options
													//Options
													music.playef(11, 10);
													game.createmenu("options");
													map.nexttowercolour();
						}else if (game.currentmenuoption == 4) {
							//Credits
													//Credits
													music.playef(11, 10);
													game.createmenu("credits");
													map.nexttowercolour();
						}/*else if (game.currentmenuoption == 4) {
							//bye!
							music.playef(2, 10);
							game.mainmenu = 100;  dwgfx.fademode = 2;
						}*/
					}
				}else if(game.currentmenuname=="levellist"){
					if(game.currentmenuoption==game.nummenuoptions-1){
						//go back to menu
						music.playef(11, 10);
						game.createmenu("mainmenu");
						map.nexttowercolour();
					}else if(game.currentmenuoption==game.nummenuoptions-2){
						//next page
						music.playef(11, 10);
						if((game.levelpage*3)+3>=editor.ListOfMetaData.length){
							game.levelpage=0;
						}else{
							game.levelpage++;
						}
						game.createmenu("levellist");
						game.currentmenuoption=game.nummenuoptions-2;
						map.nexttowercolour();
					}else{
						//Ok, launch the level!
						//PLAY CUSTOM LEVEL HOOK
						music.playef(11, 10);
						game.playcustomlevel = (game.levelpage * 3) + game.currentmenuoption;
						game.customleveltitle = editor.ListOfMetaData[game.playcustomlevel].title;
						game.customlevelfilename = editor.ListOfMetaData[game.playcustomlevel].filename;
						
						//DETERMINE IF THERE'S A SAVED GAME HERE
						/*
						TiXmlDocument doc((game.saveFilePath+editor.ListOfMetaData[game.playcustomlevel].filename+".vvv").c_str());
						if (!doc.LoadFile()){
						*/
						if (game.customcookieexists[game.playcustomlevel]) {
							music.playef(11, 10);
						  game.createmenu("quickloadlevel");
						  map.nexttowercolour();
						}else {
							game.showloadingnotice = true;
							game.mainmenu = 22;
							dwgfx.fademode = 2;
						}
						//}else{
						 // music.playef(11, 10);
						 // game.createmenu("quickloadlevel");
						 // map.nexttowercolour();
						//}
					}
				}else if(game.currentmenuname=="quickloadlevel"){
					if (game.currentmenuoption == 0) {//continue save
						game.showloadingnotice = true;
						game.mainmenu = 23;
						dwgfx.fademode = 2;
					}else if (game.currentmenuoption == 1) {
						game.showloadingnotice = true;
						game.mainmenu = 22;
						dwgfx.fademode = 2;
					}else if(game.currentmenuoption==2){
						music.playef(11, 10);
						game.levelpage=0;
						game.createmenu("levellist");
						map.nexttowercolour();
					}
				}else if(game.currentmenuname=="playerworlds"){
					if(game.currentmenuoption==0){
						music.playef(11, 10);
						game.levelpage=0;
						editor.getDirectoryData();
						game.loadcustomlevelstats(); //Should only load a file if it's needed
						game.createmenu("levellist");
						map.nexttowercolour();
					}else if(game.currentmenuoption==1){
						//LEVEL EDITOR HOOK
						music.playef(11, 10);
						game.mainmenu = 20;
						dwgfx.fademode = 2;
						editor.filename="";
					}else if(game.currentmenuoption==2){
						//back
						music.playef(11, 10);
						game.createmenu("mainmenu");
						map.nexttowercolour();
					}
				}else if(game.currentmenuname=="errornostart"){
					music.playef(11, 10);
					game.createmenu("mainmenu");
					map.nexttowercolour();
				}else if (game.currentmenuname == "graphicoptions") {
					if (game.advanced_mode) {
						if (game.currentmenuoption == 0) {
							//toggle fullscreen
							music.playef(11, 10);
							if (game.fullscreen) {game.fullscreen = false;
							}else {game.fullscreen = true;}
							updategraphicsmode(game, dwgfx);
								
							game.savestats(map, dwgfx);
							game.createmenu("graphicoptions");
						}else if (game.currentmenuoption == 1) {
							//enable acceleration: if in fullscreen, go back to window first
							music.playef(11, 10);
							game.advanced_mode = false;
							if (game.fullscreen) {
								game.fullscreen = false;
								updategraphicsmode(game, dwgfx);
								game.fullscreen = true;
							}
							updategraphicsmode(game, dwgfx);
								
							game.savestats(map, dwgfx);
							game.createmenu("graphicoptions");
						  game.currentmenuoption = 1;
						}else if (game.currentmenuoption == 2) {
							//change scaling mode
							music.playef(11, 10);
							game.advanced_scaling = (game.advanced_scaling + 1) % 5;
							updategraphicsmode(game, dwgfx);
								
							game.savestats(map, dwgfx);
							game.createmenu("graphicoptions");
					  	game.currentmenuoption = 2;
						}else if (game.currentmenuoption == 3) {
							//change smoothing
							music.playef(11, 10);
							game.advanced_smoothing = !game.advanced_smoothing;
							updategraphicsmode(game, dwgfx);
								
							game.savestats(map, dwgfx);
							game.createmenu("graphicoptions");
						  game.currentmenuoption = 3;
						}else{
							//back
							music.playef(11, 10);
							game.createmenu("mainmenu");
							map.nexttowercolour();
						}
					}else{
						if (game.currentmenuoption == 0) {
							//toggle fullscreen
							music.playef(11, 10);
							if (game.fullscreen) {game.fullscreen = false;
							}else {game.fullscreen = true;}
							updategraphicsmode(game, dwgfx);
								
							game.savestats(map, dwgfx);
							game.createmenu("graphicoptions");
						}else if (game.currentmenuoption == 1) {
							//disable acceleration: if in fullscreen, go back to window first
							music.playef(11, 10);
							game.advanced_mode = true;
							if (game.fullscreen) {
								game.fullscreen = false;
								updategraphicsmode(game, dwgfx);
								game.fullscreen = true;
							}
							updategraphicsmode(game, dwgfx);
								
							game.savestats(map, dwgfx);
							game.createmenu("graphicoptions");
						  game.currentmenuoption = 1;
						}else{
							//back
							music.playef(11, 10);
							game.createmenu("mainmenu");
							map.nexttowercolour();
						}
					}
			  }else if (game.currentmenuname == "youwannaquit") {
					if (game.currentmenuoption == 0) {
						//bye!
						music.playef(2, 10);
						game.mainmenu = 100;  dwgfx.fademode = 2;
					}else{
					  music.playef(11, 10);
						game.createmenu(game.previousmenuname);
						map.nexttowercolour();
					}
				}else if (game.currentmenuname == "setinvincibility") {
					if (game.currentmenuoption == 0) {
						//back
						music.playef(11, 10);
						game.createmenu("accessibility");
						game.currentmenuoption = 2;
						map.nexttowercolour();
					}else {
						map.invincibility = !map.invincibility;
						game.deletequick(); game.deletetele();
						game.savestats(map, dwgfx);
						music.playef(11, 10);
						game.createmenu("accessibility");
						game.currentmenuoption = 2;
						map.nexttowercolour();
					}
				}else if (game.currentmenuname == "setslowdown1") {
					if (game.currentmenuoption == 0) {
						//back
						music.playef(11, 10);
						game.createmenu("accessibility");
						game.currentmenuoption = 3;
						map.nexttowercolour();
					}else {
						//change game speed
						game.deletequick(); game.deletetele();
						game.createmenu("setslowdown2");
						map.nexttowercolour();	
				    music.playef(11, 10);
					}
				}else if (game.currentmenuname == "setslowdown2") {
					if (game.currentmenuoption == 0) {
						//back
						//stage.frameRate = 30; 
						game.slowdown = 30;
						game.savestats(map, dwgfx);
						music.playef(11, 10);
						game.createmenu("accessibility");
						game.currentmenuoption = 3;
						map.nexttowercolour();
					}else if (game.currentmenuoption == 1) {
						//stage.frameRate = 24;
						game.slowdown = 24;
						game.savestats(map, dwgfx);
						music.playef(11, 10);
						game.createmenu("accessibility");
						game.currentmenuoption = 3;
						map.nexttowercolour();
					}else if (game.currentmenuoption == 2) {
						//stage.frameRate = 18; 
						game.slowdown = 18;
						game.savestats(map, dwgfx);
						music.playef(11, 10);
						game.createmenu("accessibility");
						game.currentmenuoption = 3;
						map.nexttowercolour();
					}else if (game.currentmenuoption == 3) {
						//stage.frameRate = 12; 
						game.slowdown = 12;
						game.savestats(map, dwgfx);
						music.playef(11, 10);
						game.createmenu("accessibility");
						game.currentmenuoption = 3;
						map.nexttowercolour();
					}
				}else if (game.currentmenuname == "accessibility") {
					if (game.currentmenuoption == 0) {
						//disable animated backgrounds
				    game.colourblindmode = !game.colourblindmode;
						game.savestats(map, dwgfx);
						map.tdrawback = true;
						music.playef(11, 10);
					}else if (game.currentmenuoption == 1) {
						//disable screeneffects
				    game.noflashingmode = !game.noflashingmode;
						game.savestats(map, dwgfx);
						if (!game.noflashingmode) {
						  music.playef(18, 10);	game.screenshake = 10; game.flashlight = 5;
						}
					/*}else if (game.currentmenuoption == 2) {
						//invincibility
						if (!map.invincibility) {
						  game.createmenu("setinvincibility");
						  map.nexttowercolour();	
						}else {
						  map.invincibility = !map.invincibility;
						}
				    music.playef(11, 10);
					}else if (game.currentmenuoption == 3) {
						//change game speed
						game.createmenu("setslowdown1");
						map.nexttowercolour();	
				    music.playef(11, 10);
						*/
					}else if (game.currentmenuoption == 2) {
						//back
						music.playef(11, 10);
						game.createmenu("options");
						map.nexttowercolour();
					}else{
					  //Can't do yet! play sad sound
						music.playef(2, 10);
					}
				}else if (game.currentmenuname == "options") {
					if (game.currentmenuoption == 0) {
						//accessibility options
						music.playef(11, 10);
						game.createmenu("accessibility");
						map.nexttowercolour();
					}else if (game.currentmenuoption == 1) {
						//unlock play options
						music.playef(11, 10);
						game.createmenu("unlockmenu");
						map.nexttowercolour();
					}else if (game.currentmenuoption == 2) {
						//clear data menu
						music.playef(11, 10);
						game.createmenu("cleardatamenu");
						map.nexttowercolour();
					}else if (game.currentmenuoption == 3) {
						//back
						music.playef(11, 10);
						game.createmenu("mainmenu");
						map.nexttowercolour();
					}else{
					  //Can't do yet! play sad sound
						music.playef(2, 10);
					}
				}else if (game.currentmenuname == "unlockmenutrials") {
					if (game.currentmenuoption == 0) { 	//unlock 1
						if (game.unlock[9]) {
						  game.unlock[9] = false; game.unlocknotify[9] = false;
						  music.playef(11, 10);	game.savestats(map, dwgfx);	game.createmenu("unlockmenutrials"); game.currentmenuoption = 0;	
						}else{
						  game.unlock[9] = true; game.unlocknotify[9] = true;
						  music.playef(11, 10);	game.savestats(map, dwgfx);	game.createmenu("unlockmenutrials"); game.currentmenuoption = 0;
						}
					}else if (game.currentmenuoption == 1) { 	//unlock 2
						if (game.unlock[10]) {
							game.unlock[10] = false; game.unlocknotify[10] = false;
						  music.playef(11, 10);	game.savestats(map, dwgfx);	game.createmenu("unlockmenutrials");  game.currentmenuoption = 1;
						}else{
						  game.unlock[10] = true; game.unlocknotify[10] = true;
						  music.playef(11, 10);	game.savestats(map, dwgfx);	game.createmenu("unlockmenutrials");  game.currentmenuoption = 1;
						}
					}else if (game.currentmenuoption == 2) { 	//unlock 3
						if (game.unlock[11]) {
							game.unlock[11] = false; game.unlocknotify[11] = false;
						  music.playef(11, 10);	game.savestats(map, dwgfx);	game.createmenu("unlockmenutrials");  game.currentmenuoption = 2;
						}else{
						  game.unlock[11] = true; game.unlocknotify[11] = true;
						  music.playef(11, 10);	game.savestats(map, dwgfx);	game.createmenu("unlockmenutrials");  game.currentmenuoption = 2;
						}
					}else if (game.currentmenuoption == 3) { 	//unlock 4
						if (game.unlock[12]) {
							game.unlock[12] = false; game.unlocknotify[12] = false;
						  music.playef(11, 10);	game.savestats(map, dwgfx);	game.createmenu("unlockmenutrials");  game.currentmenuoption = 3;
						}else{
						  game.unlock[12] = true; game.unlocknotify[12] = true;
						  music.playef(11, 10);	game.savestats(map, dwgfx);	game.createmenu("unlockmenutrials");  game.currentmenuoption = 3;
						}
					}else if (game.currentmenuoption == 4) { 	//unlock 5
						if (game.unlock[13]) {
							game.unlock[13] = false; game.unlocknotify[13] = false;
						  music.playef(11, 10);	game.savestats(map, dwgfx);	game.createmenu("unlockmenutrials");  game.currentmenuoption = 4;
						}else{
						  game.unlock[13] = true; game.unlocknotify[13] = true;
						  music.playef(11, 10);	game.savestats(map, dwgfx);	game.createmenu("unlockmenutrials");  game.currentmenuoption = 4;
						}
					}else if (game.currentmenuoption == 5) { 	//unlock 6
						if (game.unlock[14]) {
							 game.unlock[14] = false; game.unlocknotify[14] = false;
						  music.playef(11, 10);	game.savestats(map, dwgfx);	game.createmenu("unlockmenutrials");  game.currentmenuoption = 5;
						}else{
						  game.unlock[14] = true; game.unlocknotify[14] = true;
						  music.playef(11, 10);	game.savestats(map, dwgfx);	game.createmenu("unlockmenutrials");  game.currentmenuoption = 5;
						}
					}else if (game.currentmenuoption == 6) { 	//back
						//back
						music.playef(11, 10);
						game.createmenu("unlockmenu");
						map.nexttowercolour();
					}
				}else if (game.currentmenuname == "unlockmenu") {
					if (game.currentmenuoption == 0) {
						//unlock time trials separately...
						music.playef(11, 10);
						game.createmenu("unlockmenutrials");
						map.nexttowercolour();
					}else if (game.currentmenuoption == 1) {
						//unlock intermissions
						if (game.unlock[16]) {
							music.playef(11, 10);
							game.unlock[16] = false; game.unlocknotify[16] = false;
							game.unlock[6] = false; game.unlock[7] = false;
							game.savestats(map, dwgfx);
							game.createmenu("unlockmenu");  game.currentmenuoption = 1;
						}else{
							music.playef(11, 10);
							game.unlock[16] = true; game.unlocknotify[16] = true;
							game.unlock[6] = true; game.unlock[7] = true;
							game.savestats(map, dwgfx);
							game.createmenu("unlockmenu");  game.currentmenuoption = 1;
						}
					}else if (game.currentmenuoption == 2) {
						//unlock no death mode
						if (game.unlock[17]) {
							music.playef(11, 10);
							game.unlock[17] = false; game.unlocknotify[17] = false;
							game.savestats(map, dwgfx);
							game.createmenu("unlockmenu");  game.currentmenuoption = 2;
						}else{
							music.playef(11, 10);
							game.unlock[17] = true; game.unlocknotify[17] = true;
							game.savestats(map, dwgfx);
							game.createmenu("unlockmenu");  game.currentmenuoption = 2;
						}
					}else if (game.currentmenuoption == 3) {
						//unlock flip mode
						if (game.unlock[18]) {
							music.playef(11, 10);
							game.unlock[18] = false; game.unlocknotify[18] = false;
							game.savestats(map, dwgfx);
							game.createmenu("unlockmenu");  game.currentmenuoption = 3;
						}else{
							music.playef(11, 10);
							game.unlock[18] = true; game.unlocknotify[18] = true;
							game.savestats(map, dwgfx);
							game.createmenu("unlockmenu");  game.currentmenuoption = 3;
						}
					}else if (game.currentmenuoption == 4) {
						//unlock jukebox
						if (game.stat_trinkets == 20) {
							music.playef(11, 10);
							game.stat_trinkets = 0;
							game.savestats(map, dwgfx);
							game.createmenu("unlockmenu");  game.currentmenuoption = 4;
						}else{
							music.playef(11, 10);
							game.stat_trinkets = 20;
							game.savestats(map, dwgfx);
							game.createmenu("unlockmenu");  game.currentmenuoption = 4;
						}
					}else if (game.currentmenuoption == 5) {
						//unlock secret lab
						if (game.unlock[8]) {
							music.playef(11, 10);
							game.unlock[8] = false; game.unlocknotify[8] = false;
							game.savestats(map, dwgfx);
							game.createmenu("unlockmenu");   game.currentmenuoption = 5;
						}else {
							music.playef(11, 10);
							game.unlock[8] = true; game.unlocknotify[8] = true;
							game.savestats(map, dwgfx);
							game.createmenu("unlockmenu");   game.currentmenuoption = 5;
						}
					}else{
						//back
						music.playef(11, 10);
						game.createmenu("mainmenu");
						map.nexttowercolour();
					}
				}else if (game.currentmenuname == "credits") {
					if (game.currentmenuoption == 0) {
						//next page
						music.playef(11, 10);
						game.createmenu("credits2");
						map.nexttowercolour();
					}else{
						//back
						music.playef(11, 10);
						game.createmenu("mainmenu");
						map.nexttowercolour();
					}
				}else if (game.currentmenuname == "credits2") {
					if (game.currentmenuoption == 0) {
						//next page
						music.playef(11, 10);
						game.createmenu("credits3");
						map.nexttowercolour();
					}else{
						//back
						music.playef(11, 10);
						game.createmenu("mainmenu");
						map.nexttowercolour();
					}
				}else if (game.currentmenuname == "credits3") {
					if (game.currentmenuoption == 0) {
						//next page
						music.playef(11, 10);
						game.createmenu("credits4");
						map.nexttowercolour();
					}else{
						//back
						music.playef(11, 10);
						game.createmenu("mainmenu");
						map.nexttowercolour();
					}
				}else if (game.currentmenuname == "credits4") {
					if (game.currentmenuoption == 0) {
						//next page
						music.playef(11, 10);
						game.createmenu("credits5");
						map.nexttowercolour();
					}else{
						//back
						music.playef(11, 10);
						game.createmenu("mainmenu");
						map.nexttowercolour();
					}
				}else if (game.currentmenuname == "credits5") {
					if (game.currentmenuoption == 0) {
						//next page
						music.playef(11, 10);
						game.createmenu("credits6");
						map.nexttowercolour();
					}else{
						//back
						music.playef(11, 10);
						game.createmenu("mainmenu");
						map.nexttowercolour();
					}
				}else if (game.currentmenuname == "credits6") {
					if (game.currentmenuoption == 0) {
						//next page
						music.playef(11, 10);
						game.createmenu("credits7");
						map.nexttowercolour();
					}else{
						//back
						music.playef(11, 10);
						game.createmenu("mainmenu");
						map.nexttowercolour();
					}
				}else if (game.currentmenuname == "credits7") {
					if (game.currentmenuoption == 0) {
						//next page
						music.playef(11, 10);
						game.createmenu("credits8");
						map.nexttowercolour();
					}else{
						//back
						music.playef(11, 10);
						game.createmenu("mainmenu");
						map.nexttowercolour();
					}
				}else if (game.currentmenuname == "credits8") {
					if (game.currentmenuoption == 0) {
						//next page
						music.playef(11, 10);
						game.createmenu("credits9");
						map.nexttowercolour();
					}else{
						//back
						music.playef(11, 10);
						game.createmenu("mainmenu");
						map.nexttowercolour();
					}
				}else if (game.currentmenuname == "credits9") {
					if (game.currentmenuoption == 0) {
						//first page
						music.playef(11, 10);
						game.createmenu("credits");
						map.nexttowercolour();
					}else{
						//back
						music.playef(11, 10);
						game.createmenu("mainmenu");
						map.nexttowercolour();
						music.niceplay(6);
					}
				}else if (game.currentmenuname == "play") {
					if (game.currentmenuoption == 0) {
						//continue
						//right, this depends on what saves you've got
						if (game.telesummary == "") {
							//You at least have a quicksave, or you couldn't have gotten here
							game.mainmenu = 2; dwgfx.fademode = 2;
						}else if (game.quicksummary == "") {
							//You at least have a telesave, or you couldn't have gotten here
							game.mainmenu = 1; dwgfx.fademode = 2;
						}else {
							//go to a menu!
							/*
						  music.playef(11, 10);
							game.loadsummary(map, help); //Prepare save slots to display
					  	game.createmenu("continue");
						  map.settowercolour(3);
							*/
							game.mainmenu = 2; dwgfx.fademode = 2;
						}
					}else if (game.currentmenuoption == 1) {
						//play modes
						music.playef(11, 10);
						game.createmenu("playmodes");
						map.nexttowercolour();
					}else if (game.currentmenuoption == 2) {
						//newgame
						music.playef(11, 10);
						game.createmenu("newgamewarning");
						map.nexttowercolour();
					}else if (game.currentmenuoption == 3) {
						//back
						music.playef(11, 10);
						game.createmenu("mainmenu");
						map.nexttowercolour();
					}
				}else if (game.currentmenuname == "playsecretlab") {
					//as above, except with an extra option...
					if (game.currentmenuoption == 0) {
						//continue
						//right, this depends on what saves you've got
						if (game.telesummary == "") {
							//You at least have a quicksave, or you couldn't have gotten here
							game.mainmenu = 2; dwgfx.fademode = 2;
						}else if (game.quicksummary == "") {
							//You at least have a telesave, or you couldn't have gotten here
							game.mainmenu = 1; dwgfx.fademode = 2;
						}else {
							//go to a menu!
							/*
						  music.playef(11, 10);
							game.loadsummary(map, help); //Prepare save slots to display
					  	game.createmenu("continue");
						  map.settowercolour(3);
							*/
							game.mainmenu = 2; dwgfx.fademode = 2;
						}
					}else if (game.currentmenuoption == 1) {
						game.mainmenu = 11;  dwgfx.fademode = 2;
					}else if (game.currentmenuoption == 2) {
						//play modes
						music.playef(11, 10);
						game.createmenu("playmodes");
						map.nexttowercolour();
					}else if (game.currentmenuoption == 3) {
						//newgame
						music.playef(11, 10);
						game.createmenu("newgamewarning");
						map.nexttowercolour();
					}else if (game.currentmenuoption == 4) {
						//back
						music.playef(11, 10);
						game.createmenu("mainmenu");
						map.nexttowercolour();
					}
				}else if (game.currentmenuname == "newgamewarning") {
					if (game.currentmenuoption == 0) {
						//yep
						game.mainmenu = 0;  dwgfx.fademode = 2;
						game.deletequick(); game.deletetele();
					}else{
						//back
						music.playef(11, 10);
						game.createmenu("play");
						map.nexttowercolour();
					}
				}else if (game.currentmenuname == "cleardatamenu") {
					if (game.currentmenuoption == 0) {
						//back
						music.playef(11, 10);
						game.createmenu("options");
						map.nexttowercolour();
					}else{
						//yep
						music.playef(23, 10);
						game.deletequick(); game.deletetele();
						game.deletestats(map, dwgfx);
						game.flashlight = 5; game.screenshake = 15;
						game.createmenu("mainmenu");
						map.nexttowercolour();
					}
				}else if (game.currentmenuname == "flipmode_mobile") {
					if (game.currentmenuoption == 0 && game.unlock[18]) { //enable/disable flip mode
						music.playef(18, 10);	game.screenshake = 10; game.flashlight = 5;
						dwgfx.setflipmode = !dwgfx.setflipmode;
						game.savemystats = true;
					}else if (game.currentmenuoption == 1) {
						//back
						music.playef(11, 10);
						game.createmenu("play");
						map.nexttowercolour();
					}else{
					  //Can't do yet! play sad sound
						music.playef(2, 10);
					}
				}else if (game.currentmenuname == "playmodes") {
					if (game.currentmenuoption == 0  && game.slowdown == 30 && !map.invincibility) { //go to the time trial menu
						music.playef(11, 10);
						game.createmenu("timetrials");
						map.nexttowercolour();
					}else if (game.currentmenuoption == 1 && game.unlock[16]) {
						//intermission mode menu
						music.playef(11, 10);
						game.createmenu("intermissionmenu");
						map.nexttowercolour();
					}else if (game.currentmenuoption == 2 && game.unlock[17] && game.slowdown == 30 && !map.invincibility) { //start a game in no death mode
						music.playef(11, 10);
						game.createmenu("startnodeathmode");
						map.nexttowercolour();
					}else if (game.currentmenuoption == 3 && game.unlock[18]) { //enable/disable flip mode
						music.playef(11, 10);
						game.createmenu("flipmode_mobile");
						map.nexttowercolour();
					}else if (game.currentmenuoption == 4) {
						//back
						music.playef(11, 10);
						game.createmenu("play");
						map.nexttowercolour();
					}else{
					  //Can't do yet! play sad sound
						music.playef(2, 10);
					}
				}else if (game.currentmenuname == "startnodeathmode") {
					if (game.currentmenuoption == 0) { //start no death mode, disabling cutscenes
						game.mainmenu = 10;  dwgfx.fademode = 2;
					}else if (game.currentmenuoption == 1) {
						game.mainmenu = 9;  dwgfx.fademode = 2;
					}else if (game.currentmenuoption == 2) {
						//back
						music.playef(11, 10);
						game.createmenu("play");
						map.nexttowercolour();
					}
				}else if (game.currentmenuname == "continue") {
					if (game.currentmenuoption == 0) {
						game.mainmenu = 1; dwgfx.fademode = 2;
					}else if (game.currentmenuoption == 1) {
						game.mainmenu = 2; dwgfx.fademode = 2;
					}else if (game.currentmenuoption == 2) {
						//back
						music.playef(11, 10);
						game.createmenu("play");
						map.nexttowercolour();
					}
				}else if (game.currentmenuname == "intermissionmenu") {
					if (game.currentmenuoption == 0) {
						music.playef(11, 10);
						music.play(6);
						game.createmenu("playint1");
						map.nexttowercolour();
					}else if (game.currentmenuoption == 1) {
						music.playef(11, 10);
						music.play(6);
						game.createmenu("playint2");
						map.nexttowercolour();
					}else if (game.currentmenuoption == 2) {
						//back
						music.playef(11, 10);
						game.createmenu("play");
						map.nexttowercolour();
					}
				}else if (game.currentmenuname == "playint1") {
					if (game.currentmenuoption == 0) {
						game.mainmenu = 12; dwgfx.fademode = 2;
					}else if (game.currentmenuoption == 1) {
						game.mainmenu = 13; dwgfx.fademode = 2;
					}else if (game.currentmenuoption == 2) {
						game.mainmenu = 14; dwgfx.fademode = 2;
					}else if (game.currentmenuoption == 3) {
						game.mainmenu = 15; dwgfx.fademode = 2;
					}else if (game.currentmenuoption == 4) {
						//back
						music.playef(11, 10);
						game.createmenu("play");
						map.nexttowercolour();
					}
				}else if (game.currentmenuname == "playint2") {
					if (game.currentmenuoption == 0) {
						game.mainmenu = 16; dwgfx.fademode = 2;
					}else if (game.currentmenuoption == 1) {
						game.mainmenu = 17; dwgfx.fademode = 2;
					}else if (game.currentmenuoption == 2) {
						game.mainmenu = 18; dwgfx.fademode = 2;
					}else if (game.currentmenuoption == 3) {
						game.mainmenu = 19; dwgfx.fademode = 2;
					}else if (game.currentmenuoption == 4) {
						//back
						music.playef(11, 10);
						game.createmenu("play");
						map.nexttowercolour();
					}
				}else if (game.currentmenuname == "gameover2") {
					//back
					music.playef(11, 10);
					music.play(6);
					game.createmenu("mainmenu");
					map.nexttowercolour();
				}else if (game.currentmenuname == "unlocktimetrials" || game.currentmenuname == "unlocktimetrial") {
					//back
					music.playef(11, 10);
					game.createmenu("play");
					map.nexttowercolour();
				}else if (game.currentmenuname == "unlocknodeathmode" || game.currentmenuname == "unlockintermission"
				           || game.currentmenuname == "unlockflipmode") {
					//back
					music.playef(11, 10);
					game.createmenu("play");
					map.nexttowercolour();
				}else if (game.currentmenuname == "timetrials") {
					if (game.currentmenuoption == 0 && game.unlock[9]) { //space station 1
						game.mainmenu = 3;  dwgfx.fademode = 2;
					}else if (game.currentmenuoption == 1 && game.unlock[10]) { //lab
						game.mainmenu = 4;  dwgfx.fademode = 2;
					}else if (game.currentmenuoption == 2 && game.unlock[11]) { //tower
						game.mainmenu = 5;  dwgfx.fademode = 2;
					}else if (game.currentmenuoption == 3 && game.unlock[12]) { //station 2
						game.mainmenu = 6;  dwgfx.fademode = 2;
					}else if (game.currentmenuoption == 4 && game.unlock[13]) { //warp
						game.mainmenu = 7;  dwgfx.fademode = 2;
					}else if (game.currentmenuoption == 5 && game.unlock[14]) { //final
						game.mainmenu = 8;  dwgfx.fademode = 2;
					}else if (game.currentmenuoption == 6) { //go to the time trial menu
						//back
						music.playef(11, 10);
						game.createmenu("play");
						map.nexttowercolour();
					}else {
					  //Can't do yet! play sad sound
						music.playef(2, 10);
					}
				}else if (game.currentmenuname == "timetrialcomplete3") {
					if (game.currentmenuoption == 1) {
						//back
						music.playef(11, 10);
					  music.play(6);
						game.createmenu("play");
						map.nexttowercolour();
					}else if (game.currentmenuoption == 0) {
						//duplicate the above based on given time trial level!
						if (game.timetriallevel == 0) { //space station 1
							game.mainmenu = 3;  dwgfx.fademode = 2;
						}else if (game.timetriallevel == 1) { //lab
							game.mainmenu = 4;  dwgfx.fademode = 2;
						}else if (game.timetriallevel == 2) { //tower
							game.mainmenu = 5;  dwgfx.fademode = 2;
						}else if (game.timetriallevel == 3) { //station 2
							game.mainmenu = 6;  dwgfx.fademode = 2;
						}else if (game.timetriallevel == 4) { //warp
							game.mainmenu = 7;  dwgfx.fademode = 2;
						}else if (game.timetriallevel == 5) { //final
							game.mainmenu = 8;  dwgfx.fademode = 2;
						}
					}
				}else if (game.currentmenuname == "gamecompletecontinue" || game.currentmenuname == "nodeathmodecomplete2") {
					if (game.currentmenuoption == 0) {
						music.play(6);
						music.playef(11, 10);
						game.createmenu("play");
						map.nexttowercolour();
					}
				}
			}
	  }
		
	}
	
	if (dwgfx.fademode == 1) script.startgamemode(game.mainmenu, key, dwgfx, game, map, obj, help, music);
}

public function gameinput(key:KeyPoll, dwgfx:dwgraphicsclass, game:gameclass, map:mapclass,
                          obj:entityclass, help:helpclass, music:musicclass):void {
	game.inittouchpoints(key);
	
	if(!script.running){
	  game.press_left = false; game.press_right = false; game.press_action = false; game.press_map = false;
	}
	
	if ((key.isDown(15) || key.isDown(17)) && key.isDown(70) && !game.fullscreentoggleheld) {
		//Toggle fullscreen
		game.fullscreentoggleheld = true;
		if (game.fullscreen) {game.fullscreen = false;
		}else {game.fullscreen = true;}
		updategraphicsmode(game, dwgfx);
			
		game.savestats(map, dwgfx);
	}
	
	if ((key.isDown(15) || key.isDown(17)) && key.isDown(70)) {
		game.fullscreentoggleheld = true;
	}else {
		game.fullscreentoggleheld = false;
	}
	
	if (game.recording == 2 && !game.playbackfinished) {
		//playback!
		//record your input and add it to the record string
		//Keys are:
		//0 - nothing
		//1 - left
		//2 - right
		//3 - left+right
		//4 - flip
		//5 - left+flip
		//6 - right+flip
		//7 - left+right+flip
		//8 - Map/teleport
		if (!game.recordinit) {
		  //Init recording
		  game.recordinit = true;
		  game.combomode = false; 
			game.playmove = game.playback[game.playbackpos+1]; game.playcombo = game.playback[game.playbackpos]; 
	  }
		
		if (game.playcombo <= 0) {
			//move on to the next action
			game.playbackpos += 2;
			game.playmove = game.playback[game.playbackpos + 1]; game.playcombo = game.playback[game.playbackpos];
			if (game.playcombo > 1) game.playcombo--;
		}
		
		if (game.playcombo >= 1) {
			game.playcombo--;
			if (game.playmove == 1 || game.playmove == 3 || game.playmove == 5 || game.playmove == 7){
				game.press_left = true;
			}
			if (game.playmove == 2 || game.playmove == 3 || game.playmove == 6 || game.playmove == 7){
				game.press_right = true;
			}
			if (game.playmove == 4 || game.playmove == 5 || game.playmove == 6 || game.playmove == 7){
				game.press_action = true;
			}
			if (game.playmove == 8){
				game.press_map = true;
				//game.playbackfinished = true;
				
				trace("finished!");
			}
		}		
	}else {
		if(!script.running){
			if (key.isDown(Keyboard.LEFT) || key.isDown(65)) game.press_left = true;
			if (key.isDown(Keyboard.RIGHT) || key.isDown(68)) game.press_right = true;
			if (key.isDown(90) || key.isDown(32) || key.isDown(86)
			 || key.isDown(Keyboard.UP) || key.isDown(Keyboard.DOWN) || key.isDown(87) || key.isDown(83)) game.press_action = true;
			if (key.isDown(Keyboard.ENTER) || key.isDown(Keyboard.BACK)) game.press_map = true;
			
			game.gettouchpoints(key, dwgfx);
		}
	}	
	
	if (game.advancetext) {
		if (game.pausescript) {
			game.press_action = false;
			if (key.isDown(90) || key.isDown(32) || key.isDown(86)
			 || key.isDown(Keyboard.UP) || key.isDown(Keyboard.DOWN) || key.isDown(87) || key.isDown(83)) game.press_action = true;
			 
			 //Iterate through touch points, find buttons that are being pressed
			switch(game.controlstyle) {
				case 0:
					
					if (key.touchPoints > 0) {
						game.press_action = true;
					}
					/*for (i = 0; i < key.touchPoints; i++) {
						//if (key.touchid[i] != key.controlstick) {
							game.press_action = true;
						//}
					}*/
				break;
				//To do: make these better
				case 1:
					if (key.touchPoints > 0) {
						game.press_action = true;
					}
				break;
				case 2:
					if (key.touchPoints > 0) {
						game.press_action = true;
					}
				break;
			}
		}
		
		if (game.press_action && !game.jumpheld) {
			if (game.pausescript) {
				game.pausescript = false; game.hascontrol = true;
			  game.jumpheld = true;
			}else{
			  game.state++;
			  game.jumpheld = true;
			}
		}
	}
	
	if (!game.press_map) game.mapheld = false;
	
	/*
	if (key.isDown("1".charCodeAt(0))) {
		dwgfx.screen.width = 640;
		dwgfx.screen.height = 480;
		setstage(640,480);
	}
	if (key.isDown("2".charCodeAt(0))) {
		dwgfx.screen.width = 960;
		dwgfx.screen.height = 720;
		setstage(960,720);
	}
	if (key.isDown("3".charCodeAt(0))) {
		dwgfx.screen.width = 1280;
		dwgfx.screen.height = 960;
		setstage(1280,960);
	}
	*/
	/*game.test = true;
	game.teststring = String(game.inertia);
	if (key.isDown("1".charCodeAt(0))) game.inertia = 0.5;
	if (key.isDown("2".charCodeAt(0))) game.inertia = 0.6;
	if (key.isDown("3".charCodeAt(0))) game.inertia = 0.7;
	if (key.isDown("4".charCodeAt(0))) game.inertia = 0.8;
	if (key.isDown("5".charCodeAt(0))) game.inertia = 0.9;
	if (key.isDown("6".charCodeAt(0))) game.inertia = 1;
	if (key.isDown("7".charCodeAt(0))) game.inertia = 1.1;
	if (key.isDown("8".charCodeAt(0))) game.inertia = 1.2;
	if (key.isDown("9".charCodeAt(0))) game.inertia = 1.3;
	if (key.isDown("0".charCodeAt(0))) game.inertia = 1.4;*/	
	
	if(!game.mobilemenu){
		if (game.intimetrial && dwgfx.fademode == 1 && game.quickrestartkludge) {
			//restart the time trial
			game.quickrestartkludge = false;
			script.startgamemode(game.timetriallevel + 3, key, dwgfx, game, map, obj, help, music);
			game.deathseq = -1;
			game.completestop = false;
		}
	}
	
	//Entity type 0 is player controled
	for (i = 0; i < obj.nentity; i++) {
	  if (obj.entities[i].rule == 0) {
			if (game.hascontrol && game.deathseq == -1 && game.lifeseq <= 5) {
				/*
				if (key.isDown(8)) {
					script.load("returntohub");
				}
				*/
				/*
				if (key.isDown(27)) {
					game.state = 0;
					dwgfx.textboxremove();
					
					map.tdrawback = true;
					music.haltdasmusik();
					game.gamestate = TITLEMODE;
				}
				*/
				
				if (game.press_map && !game.mapheld) {
					game.mapheld = true;
					if (game.activetele && game.readytotele > 20 && !game.intimetrial) {
						if(!dwgfx.flipmode) obj.flags[73] = 1; //Flip mode test
            if(int(Math.abs(obj.entities[i].vx))<=1 && int(obj.entities[i].vy)==0){
							//wait! space station 2 debug thingy
							if (game.teleportscript != "") {
								
								//trace(game.recordstring);
								//We're teleporting! Yey!						
								game.activetele = false;
								game.hascontrol = false;
								music.fadeout(); //Uncomment this when it's working!
								
								i = obj.getplayer(); obj.entities[i].colour = 102;
								
								i = obj.getteleporter();
								obj.entities[i].tile = 6;
								obj.entities[i].colour = 102;
								//which teleporter script do we use? it depends on the companion!
								game.state = 4000; game.statedelay = 0;
							}else if (game.companion == 0) { 
								//Alright, normal teleporting
								game.gamestate = 5;
								dwgfx.menuoffset = 240; //actually this should count the roomname
								if (map.extrarow) dwgfx.menuoffset -= 10;
								dwgfx.menubuffer.copyPixels(dwgfx.screenbuffer, dwgfx.screenbuffer.rect, dwgfx.tl, null, null, false);
								dwgfx.resumegamemode = false;
								
								game.useteleporter = true;
								game.initteleportermode(map);
							}else {
								//trace(game.recordstring);
								//We're teleporting! Yey!						
								game.activetele = false;
								game.hascontrol = false;
								music.fadeout(); //Uncomment this when it's working!
								
								i = obj.getplayer(); obj.entities[i].colour = 102;
								i = obj.getcompanion(game.companion); if(i>-1) obj.entities[i].colour = 102;
								
								i = obj.getteleporter();
								obj.entities[i].tile = 6;
								obj.entities[i].colour = 102;
								//which teleporter script do we use? it depends on the companion!
								game.state = 3000; game.statedelay = 0;
							}
						}
					}else if (game.activeactivity > -1) {
					  if(int(Math.abs(obj.entities[i].vx))<=1 && int(obj.entities[i].vy)==0){
						  script.load(obj.blocks[game.activeactivity].script);
							obj.removeblock(game.activeactivity);
						}
					}else if (game.swnmode == 1 && game.swngame == 1) {
						//quitting the super gravitron
						game.mapheld = true;
						//Quit menu, same conditions as in game menu
						game.gamestate = MAPMODE;
						game.gamesaved = false; dwgfx.resumegamemode = false;
						game.menupage = 20; // The Map Page
						dwgfx.menubuffer.copyPixels(dwgfx.screenbuffer, dwgfx.screenbuffer.rect, dwgfx.tl, null, null, false);
						dwgfx.menuoffset = 240; //actually this should count the roomname
						if (map.extrarow) dwgfx.menuoffset -= 10;
					}else if (game.intimetrial && dwgfx.fademode == 0) {
						if (game.mobilemenu) {
							//Quit menu in time trial 12345
							game.mapheld = true;
							//Quit menu, same conditions as in game menu
							game.gamestate = MAPMODE;
							game.gamesaved = false; dwgfx.resumegamemode = false;
							game.menupage = 10; // The Map Page
							dwgfx.menubuffer.copyPixels(dwgfx.screenbuffer, dwgfx.screenbuffer.rect, dwgfx.tl, null, null, false);
							dwgfx.menuoffset = 240; //actually this should count the roomname
							if (map.extrarow) dwgfx.menuoffset -= 10;
						}else{
							//Quick restart of time trial
							script.hardreset(key, dwgfx, game, map, obj, help, music);
							if (dwgfx.setflipmode) dwgfx.flipmode = true;
							dwgfx.fademode = 2; game.completestop = true;
							music.fadeout(); game.intimetrial = true;
							game.quickrestartkludge = true;
						}
					}else if (dwgfx.fademode==0){
						//Normal map screen, do transition later
						game.gamestate = MAPMODE;
						map.cursordelay = 0; map.cursorstate = 0;
						game.gamesaved = false; dwgfx.resumegamemode = false;
						game.menupage = 0; // The Map Page
						dwgfx.menubuffer.copyPixels(dwgfx.screenbuffer, dwgfx.screenbuffer.rect, dwgfx.tl, null, null, false);
						dwgfx.menuoffset = 240; //actually this should count the roomname
						if (map.extrarow) dwgfx.menuoffset -= 10;
					}
				}
				
				if (key.isDown(27) && (!map.custommode || map.custommodeforreal)) {
					game.mapheld = true;
					//Quit menu, same conditions as in game menu
					game.gamestate = MAPMODE;
					game.gamesaved = false; dwgfx.resumegamemode = false;
					game.menupage = 10; // The Map Page
					dwgfx.menubuffer.copyPixels(dwgfx.screenbuffer, dwgfx.screenbuffer.rect, dwgfx.tl, null, null, false);
					dwgfx.menuoffset = 240; //actually this should count the roomname
					if (map.extrarow) dwgfx.menuoffset -= 10;
				}
				
				if (key.isDown("R".charCodeAt(0)) && game.deathseq<=0 && map.invincibility) {
					game.deathseq = 30;
				}
				
				
				if (game.press_left) {game.tapleft++;
				}else { if (game.tapleft <= 4 && game.tapleft > 0) {if (obj.entities[i].vx < 0) obj.entities[i].vx = 0;}game.tapleft = 0;	}
				if (game.press_right) {game.tapright++;
				}else { if (game.tapright <= 4 && game.tapright > 0) {if (obj.entities[i].vx > 0) obj.entities[i].vx = 0;}game.tapright = 0;}
				
				
				if(game.press_left){
					//obj.entities[i].vx = -4;
					obj.entities[i].ax = -3;
					obj.entities[i].dir = 0;
				}else if (game.press_right){
				  //obj.entities[i].vx = 4;
					obj.entities[i].ax = 3;
				  obj.entities[i].dir = 1;
				}else{
					//obj.entities[i].vx = 0;
				}
				
				if (!game.press_action) {
					game.jumppressed = 0;
					game.jumpheld = false;
				}
				
        if (game.press_action && !game.jumpheld) {
					game.jumppressed = 5;
          game.jumpheld = true;
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
      }else {
				//Simple detection of keypresses outside player control, will probably scrap this (expand on
				//advance text function)
				if (!game.press_action) {
					game.jumppressed = 0;
					game.jumpheld = false;
				}
				
        if (game.press_action && !game.jumpheld) {
					game.jumppressed = 5;
          game.jumpheld = true;
				}
			}
		}
	}
}

public function mapinput(key:KeyPoll, dwgfx:dwgraphicsclass, game:gameclass, map:mapclass,
                          obj:entityclass, help:helpclass, music:musicclass):void {
	
	game.inittouchpoints(key);
	
	game.press_left = false; game.press_right = false; game.press_action = false; game.press_map = false;
	
	if ((key.isDown(15) || key.isDown(17)) && key.isDown(70) && !game.fullscreentoggleheld) {
		//Toggle fullscreen
		game.fullscreentoggleheld = true;
		if (game.fullscreen) {game.fullscreen = false;
		}else {game.fullscreen = true;}
		updategraphicsmode(game, dwgfx);
			
		game.savestats(map, dwgfx);
	}
	
	if ((key.isDown(15) || key.isDown(17)) && key.isDown(70)) {
		game.fullscreentoggleheld = true;
	}else {
		game.fullscreentoggleheld = false;
	}
	
  if(dwgfx.menuoffset==0){
		if (dwgfx.flipmode) {
			if (key.isDown(Keyboard.LEFT) || key.isDown(Keyboard.DOWN) || key.isDown(65) ||  key.isDown(83)) game.press_left = true;
			if (key.isDown(Keyboard.RIGHT) || key.isDown(Keyboard.UP) || key.isDown(68) ||  key.isDown(87)) game.press_right = true;
		}else{
			if (key.isDown(Keyboard.LEFT) || key.isDown(Keyboard.UP) || key.isDown(65) ||  key.isDown(87)) game.press_left = true;
			if (key.isDown(Keyboard.RIGHT) || key.isDown(Keyboard.DOWN) || key.isDown(68) ||  key.isDown(83)) game.press_right = true;
		}
		if (key.isDown(90) || key.isDown(32) || key.isDown(86)) game.press_action = true;
		if (game.menupage < 9) {
			if (key.isDown(Keyboard.ENTER) || key.isDown(Keyboard.BACK)) game.press_map = true;
		  if (key.isDown(27)) { game.mapheld = true;  game.menupage = 10; }
		}else{
		  if (key.isDown(Keyboard.ENTER) || key.isDown(27)) game.press_map = true;
			if (key.isDown(Keyboard.BACK)) game.press_map = true; 
		}
		
		game.gettouchpoints_gamemenu(key, obj, music, dwgfx);
		
		//In the menu system, all keypresses are single taps rather than holds. Therefore this test has to be done for all presses
	  if (!game.press_action && !game.press_left && !game.press_right) game.jumpheld = false;
	  if (!game.press_map && !key.isDown(27)) game.mapheld = false;
	}else {
		game.mapheld = true; game.jumpheld = true;
	}
	
	/*
	if (key.isDown(Keyboard.T)) {
		game.menupage = 30;
	}
	*/
	
	if (!game.mapheld) {
		if (game.mobilemenu) {
			if (game.press_map) {
				//Normal map screen, do transition later
				dwgfx.resumegamemode = true;
			}	
		}else{
			if (game.press_map && ((game.menupage < 10) || game.menupage == 30)) {
				//Normal map screen, do transition later
				dwgfx.resumegamemode = true;
			}	
		}
	}
	
  if (dwgfx.fademode == 1) {
		dwgfx.menubuffer.fillRect(dwgfx.menubuffer.rect, 0x000000);
		dwgfx.resumegamemode = true;
		obj.removeallblocks();
		game.menukludge = false;
		if (game.menupage >= 20) {
		  game.state = 96; game.statedelay = 0;
		}else{
		  game.state = 80; game.statedelay = 0;
		}
	}
	
	if (!game.jumpheld) {
  	if (game.press_action || game.press_left || game.press_right || game.press_map){
		  game.jumpheld = true;
  	}
		
		if (game.press_left) {
			game.menupage--;
		}else if (game.press_right) {
			game.menupage++;
		}
		
		if (game.menupage == 1 && obj.flags[67] == 1 && game.press_action && !game.insecretlab) {
			//Warp back to the ship
			dwgfx.resumegamemode = true;		
			
			  game.teleport_to_x = 2;
		    game.teleport_to_y = 11;
				
				//trace(game.recordstring);
				//We're teleporting! Yey!						
				game.activetele = false;
				game.hascontrol = false;
				
				i = obj.getplayer(); obj.entities[i].colour = 102;
				
				//which teleporter script do we use? it depends on the companion!
				game.state = 4000; game.statedelay = 0;
		}
		
		if (game.mobilemenu) {
			if (game.menupage == 3 && game.press_action) {
				//quit to menu
				if (dwgfx.fademode == 0) {
					if (game.intimetrial || game.insecretlab || game.nodeathmode) game.menukludge = true;
					script.hardreset(key, dwgfx, game, map, obj, help, music);
					if(dwgfx.setflipmode) dwgfx.flipmode = true;
					dwgfx.fademode = 2;
					music.fadeout();
				}
			}
		}else{
			if (game.menupage == 3 && !game.gamesaved && game.press_action && !game.intimetrial
			&& !game.nodeathmode && !game.insecretlab && !game.inintermission) {
				game.flashlight = 5;
				game.screenshake = 10;
				music.playef(18, 10);
				game.gamesaved = true;
				
				game.savetime = game.timestring(help); game.savearea = map.currentarea(map.area(game.roomx, game.roomy)); 
				game.savetrinkets = game.trinkets;
				
				if (game.roomx >= 102 && game.roomx <= 104 && game.roomy >= 110 && game.roomy <= 111) game.savearea = "The Ship";
				
				if(map.custommodeforreal){
					game.customsavequick(editor.ListOfMetaData[game.playcustomlevel].file_num, map, obj, music, help);
				}else{
					game.savequick(map, obj, music, help);
				}
				
			}
		}
		
		if (game.mobilemenu) {
			if (game.menupage == 20 && game.press_action) {
				//quit to menu
				if (dwgfx.fademode == 0) {
					game.swnmode = false;
					dwgfx.fademode = 2;
					music.fadeout();
				}
			}
			
			if (game.menupage == 10 && game.press_action) {
				//return to game
				if (dwgfx.fademode == 0) {
					if (game.intimetrial || game.insecretlab || game.nodeathmode) game.menukludge = true;
					script.hardreset(key, dwgfx, game, map, obj, help, music);
					if(dwgfx.setflipmode) dwgfx.flipmode = true;
					dwgfx.fademode = 2;
					music.fadeout();
				}
			}
		}else{
			if (game.menupage == 20 && game.press_action) {
				//return to game
				dwgfx.resumegamemode = true;
			}
			if (game.menupage == 21 && game.press_action) {
				//quit to menu
				if (dwgfx.fademode == 0) {
					game.swnmode = false;
					dwgfx.fademode = 2;
					music.fadeout();
				}
			}
			
			if (game.menupage == 10 && game.press_action) {
				//return to game
				dwgfx.resumegamemode = true;
			}
			if (game.menupage == 11 && game.press_action) {
				//quit to menu
				if (dwgfx.fademode == 0) {
					if (game.intimetrial || game.insecretlab || game.nodeathmode) game.menukludge = true;
					script.hardreset(key, dwgfx, game, map, obj, help, music);
					if(dwgfx.setflipmode) dwgfx.flipmode = true;
					dwgfx.fademode = 2;
					music.fadeout();
				}
			}
		}
		
		if (game.menupage < 0) game.menupage = 3;
		if (game.menupage > 3 && game.menupage < 9) game.menupage = 0;
		
		if (game.menupage == 9) game.menupage = 11;
		if (game.menupage == 12) game.menupage = 10;
		
		if (game.menupage == 19) game.menupage = 21;
		if (game.menupage == 22) game.menupage = 20;
		
	}	
}

public function teleporterinput(key:KeyPoll, dwgfx:dwgraphicsclass, game:gameclass, map:mapclass,
                          obj:entityclass, help:helpclass, music:musicclass):void {
	
	game.inittouchpoints(key);
	
	game.press_left = false; game.press_right = false; game.press_action = false; game.press_map = false;
	if ((key.isDown(15) || key.isDown(17)) && key.isDown(70) && !game.fullscreentoggleheld) {
		//Toggle fullscreen
		game.fullscreentoggleheld = true;
		if (game.fullscreen) {game.fullscreen = false;
		}else {game.fullscreen = true;}
		updategraphicsmode(game, dwgfx);
			
		game.savestats(map, dwgfx);
	}
	
	if ((key.isDown(15) || key.isDown(17)) && key.isDown(70)) {
		game.fullscreentoggleheld = true;
	}else {
		game.fullscreentoggleheld = false;
	}
	
  if(dwgfx.menuoffset==0){
		if (key.isDown(Keyboard.LEFT)|| key.isDown(65)) game.press_left = true;
		if (key.isDown(Keyboard.RIGHT) || key.isDown(68)) game.press_right = true;
		if (key.isDown(90) || key.isDown(32) || key.isDown(86)
			|| key.isDown(Keyboard.UP) || key.isDown(Keyboard.DOWN)||  key.isDown(87)||  key.isDown(83)) game.press_action = true;
		if (key.isDown(Keyboard.ENTER) || key.isDown(Keyboard.BACK)) game.press_map = true;
		
		game.gettouchpoints_teleportmenu(key, music, dwgfx);
		
		//In the menu system, all keypresses are single taps rather than holds. Therefore this test has to be done for all presses
	  if (!game.press_action && !game.press_left && !game.press_right) game.jumpheld = false;
	  if (!game.press_map) game.mapheld = false;
	}else {
		game.mapheld = true; game.jumpheld = true;
	}
	
	if (!game.jumpheld) {
  	if (game.press_action || game.press_left || game.press_right || game.press_map){
		  game.jumpheld = true;
  	}
		
		if (game.press_left) {
			game.teleport_to_teleporter--;
			if (game.teleport_to_teleporter < 0) game.teleport_to_teleporter = map.numteleporters - 1;
			tempx = map.teleporters[game.teleport_to_teleporter].x; 
			tempy = map.teleporters[game.teleport_to_teleporter].y;
			while (map.explored[tempx + (20 * tempy)] == 0) {
				game.teleport_to_teleporter--;
				if (game.teleport_to_teleporter < 0) game.teleport_to_teleporter = map.numteleporters - 1;
				tempx = map.teleporters[game.teleport_to_teleporter].x; 
				tempy = map.teleporters[game.teleport_to_teleporter].y;
			}
		}else if (game.press_right) {
			game.teleport_to_teleporter++;
			if (game.teleport_to_teleporter >= map.numteleporters) game.teleport_to_teleporter = 0;
			tempx = map.teleporters[game.teleport_to_teleporter].x; 
			tempy = map.teleporters[game.teleport_to_teleporter].y;
			while (map.explored[tempx + (20 * tempy)] == 0) {
			  game.teleport_to_teleporter++;
			  if (game.teleport_to_teleporter >= map.numteleporters) game.teleport_to_teleporter = 0;
				tempx = map.teleporters[game.teleport_to_teleporter].x; 
				tempy = map.teleporters[game.teleport_to_teleporter].y;
			}
		}
		
		if (game.press_map) {
			tempx = map.teleporters[game.teleport_to_teleporter].x; 
			tempy = map.teleporters[game.teleport_to_teleporter].y;
			if (game.roomx == tempx + 100 && game.roomy == tempy + 100) {
				//cancel!
			  dwgfx.resumegamemode = true;
			}else {
				//teleport
			  dwgfx.resumegamemode = true;		
				game.teleport_to_x = tempx;
		    game.teleport_to_y = tempy;
				
				//trace(game.recordstring);
				//We're teleporting! Yey!						
				game.activetele = false;
				game.hascontrol = false;
				
				i = obj.getplayer(); obj.entities[i].colour = 102;
				
				i = obj.getteleporter();
				obj.entities[i].tile = 6;
				obj.entities[i].colour = 102;
				//which teleporter script do we use? it depends on the companion!
				game.state = 4000; game.statedelay = 0;
			}
		}
	}	
}

public function gamecompleteinput(key:KeyPoll, dwgfx:dwgraphicsclass, game:gameclass, map:mapclass,
                                  obj:entityclass, help:helpclass, music:musicclass):void {
	
	game.inittouchpoints(key);
	
	game.press_left = false; game.press_right = false; game.press_action = false; game.press_map = false;
if ((key.isDown(15) || key.isDown(17)) && key.isDown(70) && !game.fullscreentoggleheld) {
		//Toggle fullscreen
		game.fullscreentoggleheld = true;
		if (game.fullscreen) {game.fullscreen = false;
		}else {game.fullscreen = true;}
		updategraphicsmode(game, dwgfx);
			
		game.savestats(map, dwgfx);
	}
	
	if ((key.isDown(15) || key.isDown(17)) && key.isDown(70)) {
		game.fullscreentoggleheld = true;
	}else {
		game.fullscreentoggleheld = false;
	}
	
	game.gettouchpoints_gamecomplete(key, music, dwgfx);
	
  if (game.press_action) {
		game.creditposition -= 6;
		if (game.creditposition <= -1650) {
			if(dwgfx.fademode==0){
			  dwgfx.fademode = 2;	
			}
			game.creditposition = -1650;
		}else {
      map.bypos += 6; map.bscroll = +6;
		}
		game.press_action = true;
	}
	if (key.isDown(Keyboard.ENTER) || key.isDown(Keyboard.BACK)) game.press_map = true;
	//if (key.isDown(27)) { game.mapheld = true;  game.menupage = 10; }
	
	if (!game.mapheld) {
		if(game.press_map){
			//Return to game
			if(dwgfx.fademode==0){
			  dwgfx.fademode = 2;	
			}
		}	
	}	
}

public function gamecompleteinput2(key:KeyPoll, dwgfx:dwgraphicsclass, game:gameclass, map:mapclass,
                                  obj:entityclass, help:helpclass, music:musicclass):void {
	
	game.inittouchpoints(key);
	
	game.press_left = false; game.press_right = false; game.press_action = false; game.press_map = false;
	
	if ((key.isDown(15) || key.isDown(17)) && key.isDown(70) && !game.fullscreentoggleheld) {
		//Toggle fullscreen
		game.fullscreentoggleheld = true;
		if (game.fullscreen) {game.fullscreen = false;
		}else {game.fullscreen = true;}
		updategraphicsmode(game, dwgfx);
			
		game.savestats(map, dwgfx);
	}
	
	if ((key.isDown(15) || key.isDown(17)) && key.isDown(70)) {
		game.fullscreentoggleheld = true;
	}else {
		game.fullscreentoggleheld = false;
	}
	
	if ((key.isDown(15) || key.isDown(17)) && key.isDown(70)) {
		game.fullscreentoggleheld = true;
	}else {
		game.fullscreentoggleheld = false;
	}
	
	game.gettouchpoints_gamecomplete(key, music, dwgfx);
	
  if (game.press_action) {
		game.creditposx++;
		if (game.creditposy >= 30) {
			if(dwgfx.fademode==0){
			  dwgfx.fademode = 2;	music.fadeout();
			}
		}
		game.press_action = true;
	}
	if (key.isDown(Keyboard.ENTER) || key.isDown(Keyboard.BACK)) game.press_map = true;
	//if (key.isDown(27)) { game.mapheld = true;  game.menupage = 10; }
		
	if (!game.mapheld) {
		if(game.press_map){
			//Return to game
			if(dwgfx.fademode==0){
			  dwgfx.fademode = 2;	music.fadeout();
			}
		}	
	}	
}

public function controltutorialinput(key:KeyPoll, dwgfx:dwgraphicsclass, game:gameclass, map:mapclass,
                          obj:entityclass, help:helpclass, music:musicclass):void {
	if (game.controltutorialstate >= 13) {
		if (key.isDown(90) || key.isDown(32) || key.isDown(86)
	   || key.isDown(Keyboard.UP) || key.isDown(Keyboard.DOWN)) game.press_action = true;
		 
		game.gettouchpoints_controltutorial(key, music, dwgfx);
		if (game.press_action) {
			game.gamestate = game.GAMEMODE;
		}
	}
}


