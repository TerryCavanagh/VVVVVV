package {
	import flash.geom.*;
  import flash.events.*;
  import flash.net.*;
	import starling.display.Image;
	import starling.textures.RenderTexture;
	
	public class editor {
		public static function init():void {
			for (var i:int = 0 ; i < 3000; i++) {
				edentity.push(new edentitiesclass);
			}
			
			for (i = 0; i < 500; i++) {
				hooklist.push("");
				sb.push("");
			}
			
			for (i = 0; i < 400; i++) {
				level.push(new edlevelclass);
			}
			
			//m_editorSavePath = FileSystemUtils::GetInstance().GetUserDocuments();
      m_editorSavePath = "";
			
			maxwidth=20;
			maxheight = 20;
			
			//We create a blank map
			for (var j:int = 0; j < 30 * maxwidth; j++){
				for (i = 0; i < 40 * maxheight; i++){
					contents.push(int(0));
				}
			}
			
			for (j = 0; j < 30; j++){
				for (i = 0; i < 40; i++){
					swapmap.push(int(0));
				}
			}
			
			for (i = 0; i < 30 * maxheight; i++){
				vmult.push(int(i * 40 * maxwidth));
			}
			
			reset();
			
			playerlevel.push(new XML( new lvl_pyramid ));             playerlevel_spikyness.push(int(1));
			playerlevel.push(new XML( new lvl_seasons ));             playerlevel_spikyness.push(int(1));
			playerlevel.push(new XML( new lvl_victuals ));            playerlevel_spikyness.push(int(1));
			playerlevel.push(new XML( new lvl_4kvvvv ));              playerlevel_spikyness.push(int(2));
			playerlevel.push(new XML( new lvl_quantumtunnel12 ));     playerlevel_spikyness.push(int(2));
			playerlevel.push(new XML( new lvl_variationventure ));    playerlevel_spikyness.push(int(2));
			playerlevel.push(new XML( new lvl_vertexvortex ));        playerlevel_spikyness.push(int(2));
			playerlevel.push(new XML( new lvl_333333_easy ));         playerlevel_spikyness.push(int(3));
			playerlevel.push(new XML( new lvl_linewrap ));            playerlevel_spikyness.push(int(3));
			playerlevel.push(new XML( new lvl_soulsearching ));       playerlevel_spikyness.push(int(3));
			playerlevel.push(new XML( new lvl_towerofpower ));        playerlevel_spikyness.push(int(3));
			playerlevel.push(new XML( new lvl_varietyshow ));         playerlevel_spikyness.push(int(3));
			playerlevel.push(new XML( new lvl_a_new_dimension ));     playerlevel_spikyness.push(int(4));
			playerlevel.push(new XML( new lvl_roadtrip ));            playerlevel_spikyness.push(int(4));
			playerlevel.push(new XML( new lvl_the_dual_challenge ));  playerlevel_spikyness.push(int(4));
			playerlevel.push(new XML( new lvl_vertiginousviridian )); playerlevel_spikyness.push(int(4));
			playerlevel.push(new XML( new lvl_vvvvvvgoldenspiral ));  playerlevel_spikyness.push(int(5));
			
			getDirectoryData();
			
			/*
			trace("loading level data:");
			getDirectoryData();
			for (i = 0; i < ListOfMetaData.length; i++) {
				trace(ListOfMetaData[i].title, ListOfMetaData[i].creator);
			}
			
			trace("done!");
			*/
		}
		
    // comparison, not case sensitive.
		public static function compare_nocase(first:String, second:String):Boolean {
		  //Not used
			//For alphabetical sorting of two strings
			return true;
		}
		
		public static function getDirectoryData():void {
			ListOfMetaData.length = 0;
			
			for (var i:int = 0; i < playerlevel.length; i++) {
        ListOfMetaData.push(getLevelMetaData(i));
			}
			/*
			var ts:String = "";
			for(i = 0; i < ListOfMetaData.length; i++){
        for(var k:int = 0; k < ListOfMetaData.length; k++){
					if (compare_nocase(ListOfMetaData[i].title, ListOfMetaData[k].title)) {
						ts = ListOfMetaData[i];
						ListOfMetaData[i] = ListOfMetaData[k];
						ListOfMetaData[k] = ts;
					}
        }
      } 
			*/
	  }
		
		public static function getLevelMetaData(t:int):LevelMetaData {
			var temp:LevelMetaData = new LevelMetaData;
			
			temp.timeCreated = playerlevel[t].Data.MetaData.Created.text();
			temp.creator = playerlevel[t].Data.MetaData.Creator.text();
			temp.title = playerlevel[t].Data.MetaData.Title.text();
			temp.timeModified = playerlevel[t].Data.MetaData.Modified.text();
			temp.modifier = playerlevel[t].Data.MetaData.Modifiers.text();
			temp.Desc1 = playerlevel[t].Data.MetaData.Desc1.text();
			temp.Desc2 = playerlevel[t].Data.MetaData.Desc2.text();
			temp.Desc3 = playerlevel[t].Data.MetaData.Desc3.text();
			temp.website = playerlevel[t].Data.MetaData.website.text();
			
			temp.filename = String(t);
			temp.file_num = t;
			temp.version = 2; //All these files are version 2
			
			return temp;
		}
		
		public static function reset():void {
			version = 2; //New smaller format change is 2
			
			mapwidth=5;
			mapheight = 5;
			
			editordata.title = "Untitled Level";
			editordata.creator = "Unknown";
			Desc1="";
			Desc2="";
			Desc3="";
			website = "";
			
			roomnamehide=0;
			zmod=false;
			xmod=false;
			spacemod=false;
			spacemenu=0;
			shiftmenu=false;
			shiftkey=false;
			saveandquit=false;
			note="";
			notedelay=0;
			roomnamemod=false;
			textentry=false;
			savemod=false;
			loadmod=false;
			//deletekeyheld = false;
			
			titlemod=false;
			creatormod=false;
			desc1mod=false;
			desc2mod=false;
			desc3mod=false;
			websitemod=false;
			settingsmod=false;
			warpmod=false; //Two step process
			warpent = -1;
			
			boundarymod=0;
			boundarytype=0;
			boundx1=0;
			boundx2=0;
			boundy1=0;
			boundy2 = 0;
			
			scripttextmod=false;
			scripttextent=0;
			scripttexttype = 0;
			
			drawmode=0;
			dmtile=0;
			dmtileeditor=0;
			entcol = 0;
			
			tilex=0;
			tiley=0;
			levx=0;
			levy=0;
			keydelay=0;
			lclickdelay=0;
			savekey=false;
			loadkey=false;
			updatetiles=true;
			changeroom=true;
			levmusic = 0;
			
			entframe=0;
			entframedelay = 0;
			
			numtrinkets=0;
			numcrewmates = 0;
			
			editordata.numedentities=0;
			levmusic = 0;
			
			roomtextmod=false;
			roomtextent = 0;
			
			for (var j:int = 0; j < maxheight; j++){
				for (var i:int = 0; i < maxwidth; i++){
					level[i + (j * maxwidth)].tileset = 0;
					level[i + (j * maxwidth)].tilecol = (i + j) % 32;
					level[i + (j * maxwidth)].roomname = ""					
					level[i + (j * maxwidth)].warpdir = 0;
					level[i + (j * maxwidth)].platx1 = 0;
					level[i + (j * maxwidth)].platy1 = 0;
					level[i + (j * maxwidth)].platx2 = 320;
					level[i + (j * maxwidth)].platy2 = 240;
					level[i + (j * maxwidth)].platv = 4;
					level[i + (j * maxwidth)].enemyx1 = 0;
					level[i + (j * maxwidth)].enemyy1 = 0					
					level[i + (j * maxwidth)].enemyx2 = 320;
					level[i + (j * maxwidth)].enemyy2 = 240;
				}
			}
			
			for (j = 0; j < 30 * maxwidth; j++){
				for (i = 0; i < 40 * maxheight; i++) {
					contents[i + (j * 30 * maxwidth)] = 0;
				}
			}
			
			if(numhooks>0){
				for (i = 0; i < numhooks; i++) {
					removehook(hooklist[i]);
				}
			}
			
			for (i = 0; i < 500; i++){
				sb[i]="";
			}
			
			for (i = 0; i < 500; i++){
				hooklist[i] = "";
			}
			
			clearscriptbuffer();
			sblength=1;
			sbx=0;
			sby=0;
			pagey=0;
			scripteditmod=false;
			sbscript="null";
			scripthelppage=0;
			scripthelppagedelay = 0;
			
			hookmenupage=0;
			hookmenu=0;
			numhooks=0;
			customscript.length = 0;
		}
		
		public static function gethooks():void {
			//Scan through the script and create a hooks list based on it
			numhooks=0;
			var tstring:String;
			var tstring2:String;
			for (var i:int = 0; i < customscript.length; i++){
				tstring = customscript[i];
				if(tstring.length-1>=0){
					tstring = tstring.substr(tstring.length - 1, 1);
				}else{
					tstring="";
				}
				
				if(tstring==":"){
					tstring2="";
					tstring=customscript[i];
					tstring2 = tstring.substr(0, tstring.length - 1);
					hooklist[numhooks] = tstring2;
					numhooks++;
				}
			}
		}
		
		public static function loadhookineditor(t:String):void{
			//Find hook t in the scriptclass, then load it into the editor
			clearscriptbuffer();
			
			var tstring:String;
			
			var removemode:Boolean = false;
			
			for (var i:int = 0; i < customscript.length; i++){
				if (customscript[i] == t + ":") {
					removemode=true;
				}else if (removemode) {
					tstring = customscript[i];					
					if (tstring != "") {
						tstring = tstring[tstring.length() - 1];
					}
					if (tstring == ":"){
						//this is a hook
						removemode=false;
					}else{
						//load in this line
						sb[sblength - 1] = customscript[i];
						sblength++;
					}
				}
			}
			if(sblength>1) sblength--;
		}
		
		public static function addhooktoscript(t:String):void{
			//Adds hook+the scriptbuffer to the end of the scriptclass
			removehookfromscript(t);
			customscript.push(t + ":");
			if (sblength >= 1) {
				for (var i:int = 0; i < sblength; i++){
					customscript.push(sb[i]);
				}
			}
		}
		
		public static function removehookfromscript(t:String):void {
			//Find hook t in the scriptclass, then removes it (and any other code with it)
			var tstring:String;
			var removemode:Boolean = false;
			
			for (var i:int = 0; i < customscript.length; i++) {
				if (customscript[i] == t + ":") {
					removemode=true;
					//Remove this line
					for (var j:int = i; j < customscript.length - 1; j++){
						customscript[j] = customscript[j + 1];
					}
					customscript.pop();
					i--;
				}else if(removemode){
					//If this line is not the start of a new hook, remove it!
					tstring = customscript[i];
					tstring = tstring.substr(tstring.length - 1, 1);
					if(tstring==":"){
						//this is a hook
						removemode=false;
					}else{
						//Remove this line
						for (j = i; j < customscript.length - 1; j++) {
							customscript[j] = customscript[j + 1];
						}
						customscript.pop();
						
						i--;
					}
				}
			}
		}
		
		public static function removehook(t:String):void {
			//Check the hooklist for the hook t. If it's there, remove it from here and the script
			for (var i:int = 0; i < numhooks; i++) {
				if (hooklist[i] == t) {
					removehookfromscript(t);
					for (var j:int = i; j < numhooks; j++) {
						hooklist[j]=hooklist[j+1];
					}
					
					hooklist[numhooks] = "";
					numhooks--;
					i--;
				}
			}
		}
		
		public static function addhook(t:String):void{
			//Add an empty function to the list in both editor and script
			removehook(t);
			hooklist[numhooks] = t;
			numhooks++;
			addhooktoscript(t);
		}
		
		public static function checkhook(t:String):Boolean {
			//returns true if hook t already is in the list
			for (var i:int = 0; i < numhooks; i++) {
				if(hooklist[i]==t) return true;
			}
			return false;
		}
		
		public static function clearscriptbuffer():void{
			for (var i:int = 0; i < sblength + 1; i++) {
				sb[i] = "";
			}
			sblength=1;
		}
		
		public static function removeline(t:int):void{
			//Remove line t from the script
			if (sblength > 0) {
				if (sblength == t) {
					sblength--;
				}else{
					for (var i:int = t; i < sblength; i++){
						sb[i] = sb[i + 1];
					}
					sb[sblength] = "";
					sblength--;
				}
			}
		}
		
		public static function insertline(t:int):void{
			//insert a blank line into script at line t
			for (var i:int = sblength; i >= t; i--) {
				sb[i + 1] = sb[i];
			}
			sb[t] = "";
			sblength++;
		}
		
		public static function loadlevel(rxi:int, ryi:int):void {
			//Set up our buffer array to be picked up by mapclass
			rxi -= 100;
			ryi -= 100;
			if (rxi < 0) rxi += mapwidth;
			if (ryi < 0) ryi += mapheight;
			if (rxi >= mapwidth) rxi -= mapwidth;
			if (ryi >= mapheight) ryi -= mapheight;
			
			for (var j:int = 0; j < 30; j++){
				for (var i:int = 0; i < 40; i++) {
					swapmap[i + (j * 40)] = contents[i + (rxi * 40) + vmult[j + (ryi * 30)]];
				}
			}
		}
		
		public static function getlevelcol(t:int):int{
			if (level[t].tileset == 0) {  //Space Station
				return level[t].tilecol;
			}else if (level[t].tileset == 1) {   //Outside
				return 32 + level[t].tilecol;
			}else if (level[t].tileset == 2) {   //Lab
				return 40 + level[t].tilecol;
			}else if (level[t].tileset == 3) {   //Warp Zone				
				return 46 + level[t].tilecol;
			}else if (level[t].tileset == 4) {  //Ship
				return 52 + level[t].tilecol;
			}
			return 0;
		}
		
		public static function getenemycol(t:int):int{
			switch(t){
				//RED
				case 3:	case 7:	case 12: case 23: case 28: case 34:	case 42: case 48:	case 58:
					return 6;
				break;
				//GREEN
				case 5:	case 9:	case 22: case 25:	case 29: case 31:	case 38: case 46:	case 52: case 53:
					return 7;
				break;
				//BLUE
			  case 1:	case 6: case 14: case 27: case 33: case 44:	case 50: case 57:	
				  return 12;
				break;
				//YELLOW
				case 4:	case 17: case 24:	case 30: case 37:	case 45: case 51:	case 55:
					return 9;
				break;
				//PURPLE
			  case 2:	case 11: case 15:	case 19: case 32:	case 36: case 49:
				  return 20;
				break;
				//CYAN
				case 8:	case 10: case 13:	case 18: case 26:	case 35: case 41:	case 47: case 54:
					return 11;
				break;
				//PINK
				case 16: case 20:	case 39: case 43:	case 56:
					return 8;
				break;
				//ORANGE
				case 21: case 40:
					return 17;
				break;
				default:
					return 6;
				break;
			}
			return 0;
		}
		
		public static function getwarpbackground(rx:int, ry:int):int {
			var tmp:int = rx + (maxwidth * ry);
			switch(level[tmp].tileset){
				case 0: //Space Station
					switch(level[tmp].tilecol) {
						case 0:	return 3;	break;
						case 1:	return 2;	break;
						case 2:	return 1;	break;
					  case 3:	return 4;	break;
						case 4:	return 5;	break;
						case 5:	return 3;	break;
						case 6:	return 1;	break;
						case 7:	return 0; break;
						case 8:	return 5;	break;
						case 9:	return 0;	break;
						case 10: return 2; break;
						case 11: return 1; break;
						case 12: return 5; break;
						case 13: return 0; break;
						case 14: return 3; break;
						case 15: return 2; break;
						case 16: return 4; break;
						case 17: return 0; break;
						case 18: return 3; break;
						case 19: return 1; break;
						case 20: return 4; break;
						case 21: return 5; break;
						case 22: return 1; break;
						case 23: return 4; break;
						case 24: return 5; break;
						case 25: return 0; break;
						case 26: return 3; break;
						case 27: return 1; break;
						case 28: return 5; break;
						case 29: return 4; break;
						case 30: return 5; break;
						case 31: return 2; break;
						default: return 6; break;
					}
				break;
			  case 1: //Outside
					switch(level[tmp].tilecol) {
					  case 0:	return 3;	break;
						case 1:	return 1;	break;
						case 2:	return 0;	break;
						case 3:	return 2;	break;
						case 4:	return 4;	break;
						case 5:	return 5;	break;
						case 6:	return 2;	break;
						case 7:	return 4;	break;
						default: return 6; break;
					}
				break;
				case 2: //Lab
					switch(level[tmp].tilecol) {
						case 0: return 0;	break;
						case 1:	return 1;	break;
						case 2:	return 2; break;
						case 3:	return 3;	break;
						case 4: return 4;	break;
						case 5:	return 5;	break;
						case 6:	return 6;	break;
						default: return 6; break;
					}
				break;
				case 3: //Warp Zone
					switch(level[tmp].tilecol) {
						case 0:	return 0;	break;
						case 1:	return 1;	break;
						case 2:	return 2;	break;
						case 3: return 3;	break;
						case 4:	return 4;	break;
						case 5:	return 5;	break;
						case 6:	return 6; break;
						default: return 6; break;
					}
				break;
				case 4: //Ship
					switch(level[tmp].tilecol) {
						case 0: return 5;	break;
						case 1:	return 0; break;
						case 2:	return 4;	break;
						case 3:	return 2;	break;
						case 4:	return 3;	break;
						case 5:	return 1; break;
						case 6:	return 6;	break;
						default: return 6; break;
					}
				break;
				case 5: //Tower
					return 6;
				break;
				default:
					return 6;
				break;
			}
		}
		
		public static function getenemyframe(t:int):int {
			switch(t){
				case 0: return 78; break;
				case 1:	return 88; break;
				case 2:	return 36; break;
				case 3:	return 164;	break;
				case 4:	return 68; break;
				case 5:	return 48; break;
				case 6:	return 176;	break;
				case 7:	return 168;	break;
				case 8:	return 112;	break;
				case 9:	return 114;	break;
				default: return 78;	break;
			}
			return 78;
		}
		
		public static function placetile(x:int, y:int, t:int):void {
			if (x >= 0 && y >= 0 && x < mapwidth * 40 && y < mapheight * 30) {
        contents[x + (levx * 40) + vmult[y + (levy * 30)]] = t;
			}
		}
		
		public static function placetilelocal(x:int, y:int, t:int):void {
			if (x >= 0 && y >= 0 && x < 40 && y < 30) {
				contents[x + (levx * 40) + vmult[y + (levy * 30)]] = t;
			}
			updatetiles=true;
		}
		
		public static function base(x:int, y:int):int {
    //Return the base tile for the given tileset and colour
			temp = x + (y * maxwidth);
			if(level[temp].tileset==0){  //Space Station
        if (level[temp].tilecol >= 22) {
          return 483 + ((level[temp].tilecol - 22) * 3);
        }else if (level[temp].tilecol >= 11) {
          return 283 + ((level[temp].tilecol-11)*3);
        }else{
          return 83 + (level[temp].tilecol * 3);
        }
			}else if (level[temp].tileset == 1) {   //Outside
        return 480 + (level[temp].tilecol*3);
			}else if (level[temp].tileset == 2) {   //Lab
				return 280 + (level[temp].tilecol*3);
			}else if (level[temp].tileset == 3) {   //Warp Zone/Intermission
        return 80 + (level[temp].tilecol * 3);
			}else if (level[temp].tileset == 4) {   //SHIP
        return 101 + (level[temp].tilecol * 3);
			}
			return 0;
		}
		
		public static function backbase(x:int, y:int):int {
			//Return the base tile for the background of the given tileset and colour
			temp = x + (y * maxwidth);
			if (level[temp].tileset == 0) {  //Space Station
        //Pick depending on tilecol
        switch(level[temp].tilecol) {
					case 0: case 5: case 26:
            return 680; //Blue
          break;
					case 3: case 16: case 23:
            return 683; //Yellow
          break;
					case 9: case 12: case 21:
            return 686; //Greeny Cyan
          break;
					case 4: case 8: case 24: case 28: case 30:
            return 689; //Green
          break;
					case 20: case 29:
            return 692; //Orange
          break;
					case 2: case 6: case 11: case 22: case 27:
            return 695; //Red
          break;
					case 1: case 10: case 15: case 19: case 31:
            return 698; //Pink
          break;
					case 14: case 18:
            return 701; //Dark Blue
          break;
					case 7: case 13: case 17: case 25:
            return 704; //Cyan
          break;
					default:
					  return 680;
          break;
        }
			}else if (level[temp].tileset == 1) {  //outside
        return 680 + (level[temp].tilecol * 3);
			}else if (level[temp].tileset == 2) {  //Lab
        return 0;
			}else if (level[temp].tileset == 3) {  //Warp Zone/Intermission
				return 120 + (level[temp].tilecol * 3)
			} else if(level[temp].tileset==4) {  //SHIP
        return 741 + (level[temp].tilecol * 3);
			}
			return 0;
		}
		
		public static function at(x:int, y:int):int {
			if (x < 0) return at(0, y);
			if (y < 0) return at(x, 0);
			if (x >= 40) return at(39, y);
			if (y >= 30) return at(x, 29);
			
			if (x >= 0 && y >= 0 && x < 40 && y < 30) {
				return contents[x + (levx * 40) + vmult[y + (levy * 30)]];
			}
			return 0;
		}
		
		public static function freewrap(x:int, y:int):int {
			if (x < 0) return freewrap(x + (mapwidth * 40), y);
			if (y < 0) return freewrap(x, y + (mapheight * 30));
			if (x >= (mapwidth * 40)) return freewrap(x - (mapwidth * 40), y);
			if (y >= (mapheight * 30)) return freewrap(x, y - (mapheight * 30));
			
			if (x >= 0 && y >= 0 && x < (mapwidth * 40) && y < (mapheight * 30)) {
				if (contents[x + vmult[y]] == 0) {
					return 0;
				}else{
					if (contents[x + vmult[y]]>= 2 && contents[x + vmult[y]] < 80) {
						return 0;
					}
					if (contents[x + vmult[y]]>= 680) {
						return 0;
					}
				}
			}
			return 1;
		}
		
		public static function backonlyfree(x:int, y:int):int {
			//Returns 1 if tile is a background tile, 0 otherwise
			if (x < 0) return backonlyfree(0, y);
			if (y < 0) return backonlyfree(x, 0);
			if (x >= 40) return backonlyfree(39, y);
			if (y >= 30) return backonlyfree(x, 29);
			
			if (x >= 0 && y >= 0 && x < 40 && y < 30) {
				if (contents[x + (levx * 40) + vmult[y + (levy * 30)]]>= 680) {
					return 1;
				}
			}
			return 0;
		}
		
		public static function backfree(x:int, y:int):int {
			//Returns 0 if tile is not a block or background tile, 1 otherwise
			if (x < 0) return backfree(0, y);
			if (y < 0) return backfree(x, 0);
			if (x >= 40) return backfree(39, y);
			if (y >= 30) return backfree(x, 29);
			
			if (x >= 0 && y >= 0 && x < 40 && y < 30) {
				if (contents[x + (levx * 40) + vmult[y + (levy * 30)]] == 0) {
					return 0;
				}else{
					//if(contents[x+(levx*40)+vmult[y+(levy*30)]]>=2 && contents[x+(levx*40)+vmult[y+(levy*30)]]<80){
					//		return 0;
					//}
				}
			}
			return 1;
		}
		
		public static function spikefree(x:int, y:int):int {
			//Returns 0 if tile is not a block or spike, 1 otherwise
			if (x == -1) return free(0, y);
			if (y == -1) return free(x, 0);
			if (x == 40) return free(39, y);
			if (y == 30) return free(x, 29);			
			
			if (x >= 0 && y >= 0 && x < 40 && y < 30) {
				if (contents[x + (levx * 40) + vmult[y + (levy * 30)]] == 0) {
					return 0;
				}else{
					if (contents[x + (levx * 40) + vmult[y + (levy * 30)]]>= 680) {
						return 0;
					}
				}
			}
			return 1;
		}
		
		public static function free(x:int, y:int):int {
			//Returns 0 if tile is not a block, 1 otherwise
			if (x == -1) return free(0, y);
			if (y == -1) return free(x, 0);
			if (x == 40) return free(39, y);
			if (y == 30) return free(x, 29);
			
			if (x >= 0 && y >= 0 && x < 40 && y < 30) {
				if (contents[x + (levx * 40) + vmult[y + (levy * 30)]] == 0) {
					return 0;
				}else {
					if (contents[x + (levx * 40) + vmult[y + (levy * 30)]]>= 2 && contents[x + (levx * 40) + vmult[y + (levy * 30)]] < 80) {
						return 0;
					}
					if (contents[x + (levx * 40) + vmult[y + (levy * 30)]]>= 680) {
						return 0;
					}
				}
			}
			return 1;
		}
		
		public static function absfree(x:int, y:int):int {
			//Returns 0 if tile is not a block, 1 otherwise, abs on grid
			if (x >= 0 && y >= 0 && x < mapwidth * 40 && y < mapheight * 30) {
				if (contents[x + vmult[y]] == 0) {
					return 0;
				}else{
					if (contents[x + vmult[y]]>= 2 && contents[x + vmult[y]] < 80){
						return 0;
					}
					if (contents[x + vmult[y]]>= 680) {
						return 0;
					}
				}
			}
			return 1;
		}
		
		public static function match(x:int, y:int):int {
			if (free(x - 1, y) == 0 && free(x, y - 1) == 0 && free(x + 1, y) == 0 && free(x, y + 1) == 0) return 0;
			
			if (free(x - 1, y) == 0 && free(x, y - 1) == 0) return 10;
			if (free(x + 1, y) == 0 && free(x, y - 1) == 0) return 11;
			if (free(x - 1, y) == 0 && free(x, y + 1) == 0) return 12;
			if (free(x + 1, y) == 0 && free(x, y + 1) == 0) return 13;
			
			if (free(x, y - 1) == 0) return 1;
			if (free(x - 1, y) == 0) return 2;
			if (free(x, y + 1) == 0) return 3;
			if (free(x + 1, y) == 0) return 4;
			if (free(x - 1, y - 1) == 0) return 5;
			if (free(x + 1, y - 1) == 0) return 6;
			if (free(x - 1, y + 1) == 0) return 7;
			if (free(x + 1, y + 1) == 0) return 8;
			
			return 0;
		}

		public static function warpzonematch(x:int, y:int):int {
			if (free(x - 1, y) == 0 && free(x, y - 1) == 0 && free(x + 1, y) == 0 && free(x, y + 1) == 0) return 0;
			
			if (free(x - 1, y) == 0 && free(x, y - 1) == 0) return 10;
			if (free(x + 1, y) == 0 && free(x, y - 1) == 0) return 11;
			if (free(x - 1, y) == 0 && free(x, y + 1) == 0) return 12;
			if (free(x + 1, y) == 0 && free(x, y + 1) == 0) return 13;
			
			if (free(x, y - 1) == 0) return 1;
			if (free(x - 1, y) == 0) return 2;
			if (free(x, y + 1) == 0) return 3;
			if (free(x + 1, y) == 0) return 4;
			if (free(x - 1, y - 1) == 0) return 5;
			if (free(x + 1, y - 1) == 0) return 6;
			if (free(x - 1, y + 1) == 0) return 7;
			if (free(x + 1, y + 1) == 0) return 8;
			
			return 0;
		}
		
		public static function outsidematch(x:int, y:int):int {
			if (backonlyfree(x - 1, y) == 0 && backonlyfree(x + 1, y) == 0) return 2;
			if (backonlyfree(x, y - 1) == 0 && backonlyfree(x, y + 1) == 0) return 1;
			
			return 0;
		}
		
		public static function backmatch(x:int, y:int):int{
			//Returns the first position match for a border
			// 5 1 6
			// 2 X 4
			// 7 3 8
			/*
			if(at(x-1,y)>=80 && at(x,y-1)>=80) return 10;
			if(at(x+1,y)>=80 && at(x,y-1)>=80) return 11;
			if(at(x-1,y)>=80 && at(x,y+1)>=80) return 12;
			if(at(x+1,y)>=80 && at(x,y+1)>=80) return 13;
			
			if(at(x,y-1)>=80) return 1;
			if(at(x-1,y)>=80) return 2;
			if(at(x,y+1)>=80) return 3;
			if(at(x+1,y)>=80) return 4;
			if(at(x-1,y-1)>=80) return 5;
			if(at(x+1,y-1)>=80) return 6;
			if(at(x-1,y+1)>=80) return 7;
			if(at(x+1,y+1)>=80) return 8;
			*/
			if (backfree(x - 1, y) == 0 && backfree(x, y - 1) == 0 && backfree(x + 1, y) == 0 && backfree(x, y + 1) == 0) return 0;
			
			if (backfree(x - 1, y) == 0 && backfree(x, y - 1) == 0) return 10;
			if (backfree(x + 1, y) == 0 && backfree(x, y - 1) == 0) return 11;
			if (backfree(x - 1, y) == 0 && backfree(x, y + 1) == 0) return 12;
			if (backfree(x + 1, y) == 0 && backfree(x, y + 1) == 0) return 13;
			
			if (backfree(x, y - 1) == 0) return 1;
			if (backfree(x - 1, y) == 0) return 2;
			if (backfree(x, y + 1) == 0) return 3;
			if (backfree(x + 1, y) == 0) return 4;
			
			if (backfree(x - 1, y - 1) == 0) return 5;
			if (backfree(x + 1, y - 1) == 0) return 6;
			if (backfree(x - 1, y + 1) == 0) return 7;
			if (backfree(x + 1, y + 1) == 0) return 8
			
			return 0;
		}
		
		public static function edgetile(x:int, y:int):int {
			switch(match(x, y)) {
				case 14: return 0; break;
				case 10: return 80; break;
				case 11: return 82; break;
				case 12: return 160; break;
				case 13: return 162; break;
				case 1: return 81; break;
				case 2: return 120; break;
				case 3: return 161; break;
				case 4: return 122; break;
				case 5: return 42; break;
				case 6: return 41; break;
				case 7: return 2; break;
				case 8: return 1; break;
				case 0: default: return 0; break;
			}
			return 0;
		}
		
		public static function warpzoneedgetile(x:int, y:int):int{
			switch(backmatch(x, y)) {
				case 14: return 0; break;
				case 10: return 80; break;
				case 11: return 82; break;
				case 12: return 160; break;
				case 13: return 162; break;
				case 1: return 81; break;
				case 2: return 120; break;
				case 3: return 161; break;
				case 4: return 122; break;
				case 5: return 42; break;
				case 6: return 41; break;
				case 7: return 2; break;
				case 8: return 1; break;
				case 0: default: return 0; break;
			}
			return 0;
		}
		
		public static function outsideedgetile(x:int, y:int):int{
			switch(outsidematch(x,y)){
				case 2: return 0; break;
				case 1: return 1; break;
				case 0: default: return 2; break;
			}
			return 2;
		}
		
		public static function backedgetile(x:int, y:int):int {
			switch(backmatch(x, y)) {
				case 14: return 0; break;
				case 10: return 80;	break;
				case 11: return 82;	break;
				case 12: return 160; break;
				case 13: return 162; break;
				case 1: return 81; break;
				case 2: return 120;	break;
				case 3:	return 161;	break;
				case 4:	return 122;	break;
				case 5:	return 42; break;
				case 6:	return 41; break;
				case 7:	return 2;	break;
				case 8:	return 1;	break;
				case 0:	default: return 0; break;
			}
			return 0;
		}
		
		public static function labspikedir(x:int, y:int, t:int):int{
			// a slightly more tricky case
			if (free(x, y + 1) == 1) return 63 + (t * 2);
			if (free(x, y - 1) == 1) return 64 + (t * 2);
			if (free(x - 1, y) == 1) return 51 + (t * 2);
			if (free(x + 1, y) == 1) return 52 + (t * 2);
			return 63 + (t * 2);
		}
		
		public static function spikedir(x:int, y:int):int{
			if (free(x, y + 1) == 1) return 8;
			if (free(x, y - 1) == 1) return 9;
			if (free(x - 1, y) == 1) return 49;
			if (free(x + 1, y) == 1) return 50;
			return 8;
		}
		
		public static function findstartpoint(game:gameclass):void {
			//Ok! Scan the room for the closest checkpoint
			var testeditor:int = -1;
			//First up; is there a start point on this screen?
			for (var i:int = 0; i < editordata.numedentities; i++) {
        //if() on screen
        if (edentity[i].t == 16 && testeditor == -1){
          testeditor=i;
        }
			}
			
			if (testeditor == -1) {
				game.edsavex = 160;
        game.edsavey = 120;
        game.edsaverx = 100;
        game.edsavery = 100;
        game.edsavegc = 0;
        game.edsavey--;
        game.edsavedir = 1 - edentity[testeditor].p1;
			}else{
        //Start point spawn
        var tx:int = (edentity[testeditor].x - (edentity[testeditor].x % 40)) / 40;
        var ty:int = (edentity[testeditor].y - (edentity[testeditor].y % 30)) / 30;
        game.edsavex = ((edentity[testeditor].x % 40) * 8) - 4;
        game.edsavey = (edentity[testeditor].y % 30) * 8;
        game.edsaverx = 100 + tx;
        game.edsavery = 100 + ty;
        game.edsavegc = 0;
        game.edsavey--;
        game.edsavedir = 1 - edentity[testeditor].p1;
			}
		}
		
		public static function saveconvertor():void{
			//In the case of resizing breaking a level, this function can fix it
			maxwidth = 20;
			maxheight = 20;
			var oldwidth:int = 10, oldheight:int = 10;
			
			var tempcontents:Vector.<int> = new Vector.<int>;
			for (var j:int = 0; j < 30 * oldwidth; j++) {
				for (var i:int = 0; i < 40 * oldheight; i++) {
					tempcontents.push(contents[i + (j * 40 * oldwidth)]);
				}
			}
			
			contents.length = 0;
			for (j = 0; j < 30 * maxheight; j++) {
				for (i = 0; i < 40 * maxwidth; i++){
					contents.push(0);
				}
			}
			
			for (j = 0; j < 30 * oldheight; j++){
				for (i = 0; i < 40 * oldwidth; i++){
					contents[i + (j * 40 * oldwidth)] = tempcontents[i + (j * 40 * oldwidth)];
				}
			}
			
			tempcontents.length = 0;
			vmult.length = 0;
			for (i = 0; i < 30 * maxheight; i++){
				vmult.push(int(i * 40 * maxwidth));
			}
			
			for (j = 0; j < maxheight; j++) {
				for (i = 0; i < maxwidth; i++) {
					level[i + (j * maxwidth)].tilecol = (i + j) % 6;
				}
			}
			//contents.length = 0;
		}
		
		public static function findtrinket(t:int):int {
			var ttrinket:int = 0;
			for (var i:int = 0; i < editordata.numedentities; i++) {
				if (i == t) return ttrinket;
				if (edentity[i].t == 9) ttrinket++;
			}
			return 0;
		}
		
		public static function findcrewmate(t:int):int {
			var ttrinket:int = 0;
			for (var i:int = 0; i < editordata.numedentities; i++) {
				if (i == t) return ttrinket;
				if (edentity[i].t == 15) ttrinket++;
			}
			return 0;
		}
		
		public static function findwarptoken(t:int):int {
      var ttrinket:int = 0;
			for (var i:int = 0; i < editordata.numedentities; i++) {
				if (i == t) return ttrinket;
				if (edentity[i].t == 13) ttrinket++;
			}
			return 0;
		}
		
		public static function countstuff():void{
			numtrinkets = 0;
			numcrewmates = 0;
			for (var i:int = 0; i < editordata.numedentities; i++) {
				if (edentity[i].t == 9) numtrinkets++;
				if (edentity[i].t == 15) numcrewmates++;
			}
		}
		
		public static function weirdloadthing(t:int):void {
			load(t);
		}
		
		public static function load(t:int):void {
			//Need to dramatically rethink this function
			reset();
			
			version = 2;
			
			editordata.creator = playerlevel[t].Data.MetaData.Creator.text();
			editordata.title = playerlevel[t].Data.MetaData.Title.text();
			Desc1 = playerlevel[t].Data.MetaData.Desc1.text();
			Desc2 = playerlevel[t].Data.MetaData.Desc2.text();
			Desc3 = playerlevel[t].Data.MetaData.Desc3.text();
			website = playerlevel[t].Data.MetaData.website.text();
			
			mapwidth = int(playerlevel[t].Data.mapwidth.text());
			mapheight = int(playerlevel[t].Data.mapheight.text());
			levmusic = int(playerlevel[t].Data.levmusic.text());
			
			var lvlstring:String = playerlevel[t].Data.contents.text();
			var values:Array;
			values = new Array;
			for (var i:int = 0; i < contents.length; i++) contents[i] = 0;
			values = lvlstring.split(",");
			var xp:int = 0, yp:int = 0;
			for (i = 0; i < values.length; i++) {
				contents[xp + (maxwidth * 40 * yp)] = int(values[i]);
				xp++;
				if(xp == mapwidth*40){
					xp = 0;
					yp++;
				}
			}
			
			editordata.numedentities = playerlevel[t].Data.edEntities.edentity.length();
			for (i = 0; i < editordata.numedentities; i++) {
				edentity[i].x = playerlevel[t].Data.edEntities.edentity[i].@x;
				edentity[i].y = playerlevel[t].Data.edEntities.edentity[i].@y;
				edentity[i].t = playerlevel[t].Data.edEntities.edentity[i].@t;
				edentity[i].p1 = playerlevel[t].Data.edEntities.edentity[i].@p1;
				edentity[i].p2 = playerlevel[t].Data.edEntities.edentity[i].@p2;
				edentity[i].p3 = playerlevel[t].Data.edEntities.edentity[i].@p3;
				edentity[i].p4 = playerlevel[t].Data.edEntities.edentity[i].@p4;
				edentity[i].p5 = playerlevel[t].Data.edEntities.edentity[i].@p5;
				edentity[i].p6 = playerlevel[t].Data.edEntities.edentity[i].@p6;
				
				edentity[i].scriptname = playerlevel[t].Data.edEntities.edentity[i].text();
			}
			
			for (i = 0; i < playerlevel[t].Data.levelMetaData.edLevelClass.length(); i++) {
				level[i].roomname = playerlevel[t].Data.levelMetaData.edLevelClass[i].text();
				
				level[i].tileset = playerlevel[t].Data.levelMetaData.edLevelClass[i].@tileset;
				level[i].tilecol = playerlevel[t].Data.levelMetaData.edLevelClass[i].@tilecol;
				level[i].platx1 = playerlevel[t].Data.levelMetaData.edLevelClass[i].@platx1;
				level[i].platy1 = playerlevel[t].Data.levelMetaData.edLevelClass[i].@platy1;
				level[i].platx2 = playerlevel[t].Data.levelMetaData.edLevelClass[i].@platx2;
				level[i].platy2 = playerlevel[t].Data.levelMetaData.edLevelClass[i].@platy2;
				level[i].platv = playerlevel[t].Data.levelMetaData.edLevelClass[i].@platv;
				level[i].enemyx1 = playerlevel[t].Data.levelMetaData.edLevelClass[i].@enemyx1;
				level[i].enemyy1 = playerlevel[t].Data.levelMetaData.edLevelClass[i].@enemyy1;
				level[i].enemyx2 = playerlevel[t].Data.levelMetaData.edLevelClass[i].@enemyx2;
				level[i].enemyy2 = playerlevel[t].Data.levelMetaData.edLevelClass[i].@enemyy2;
				level[i].enemytype = playerlevel[t].Data.levelMetaData.edLevelClass[i].@enemytype;
				level[i].warpdir = playerlevel[t].Data.levelMetaData.edLevelClass[i].@warpdir;
				level[i].directmode = playerlevel[t].Data.levelMetaData.edLevelClass[i].@directmode;
			}
			
			lvlstring = playerlevel[t].Data.script.text();
			values = new Array;
			values = lvlstring.split("|");
			
			customscript.length = 0;
			for (i = 0; i < values.length; i++) {
				customscript.push(values[i]);
			}
			
			gethooks();
			countstuff();
			version=2;
			//saveconvertor();
		}	
		
		public static function save(_path:String):void {
			//Not used
		}
		
		public static function addedentity(xp:int, yp:int, tp:int, p1:int, p2:int, p3:int, p4:int, p5:int, p6:int):void{
			edentity[editordata.numedentities].x=xp;
			edentity[editordata.numedentities].y=yp;
			edentity[editordata.numedentities].t=tp;
			edentity[editordata.numedentities].p1=p1;
			edentity[editordata.numedentities].p2=p2;
			edentity[editordata.numedentities].p3=p3;
			edentity[editordata.numedentities].p4=p4;
			edentity[editordata.numedentities].p5=p5;
			edentity[editordata.numedentities].p6=p6;
			edentity[editordata.numedentities].scriptname = "";
			
			editordata.numedentities++;
		}
		
		public static function naddedentity(xp:int, yp:int, tp:int, p1:int, p2:int, p3:int, p4:int, p5:int, p6:int):void{
			edentity[editordata.numedentities].x=xp;
			edentity[editordata.numedentities].y=yp;
			edentity[editordata.numedentities].t=tp;
			edentity[editordata.numedentities].p1=p1;
			edentity[editordata.numedentities].p2=p2;
			edentity[editordata.numedentities].p3=p3;
			edentity[editordata.numedentities].p4=p4;
			edentity[editordata.numedentities].p5=p5;
			edentity[editordata.numedentities].p6=p6;
			edentity[editordata.numedentities].scriptname="";
		}
		
		public static function copyedentity(a:int, b:int):void {
			edentity[a].x = edentity[b].x;
			edentity[a].y = edentity[b].y;
			edentity[a].t = edentity[b].t;
			edentity[a].p1 = edentity[b].p1;
			edentity[a].p2 = edentity[b].p2;
			edentity[a].p3 = edentity[b].p3;
			edentity[a].p4 = edentity[b].p4;
			edentity[a].p5 = edentity[b].p5;
			edentity[a].p6 = edentity[b].p6;
			edentity[a].scriptname = edentity[b].scriptname;
		}

		public static function removeedentity(t:int):void {
			if (t == editordata.numedentities - 1) {
				editordata.numedentities--;
			}else{
				for (var m:int = t; m < editordata.numedentities; m++) copyedentity(m, m + 1);
				editordata.numedentities--;
			}
		}
		
		public static function edentat(xp:int, yp:int):int{
			for(var i:int=0; i<editordata.numedentities; i++){
				if (edentity[i].x == xp && edentity[i].y == yp) return i;
			}
			return -1;
		}
		
		public static function edentclear(xp:int, yp:int):Boolean {
			for (var i:int = 0; i < editordata.numedentities; i++) {
				if(edentity[i].x==xp && edentity[i].y==yp) return false;
			}
			return true;
		}
		
		public static function fillbox(dwgfx:dwgraphicsclass, x:int, y:int, x2:int, y2:int, c:int):void {
			dwgfx.drawfillrect(x, y, x2 - x, 1, c);
			dwgfx.drawfillrect(x, y2 - 1, x2 - x, 1, c);
			dwgfx.drawfillrect(x, y, 1, y2 - y, c);
			dwgfx.drawfillrect(x2 - 1, y, 1, y2 - y, c);
		}
		
		public static function fillboxabs(dwgfx:dwgraphicsclass, x:int, y:int, x2:int, y2:int, c:int):void {
			dwgfx.drawfillrect(x, y, x2, 1, c);
			dwgfx.drawfillrect(x, y + y2 - 1, x2, 1, c);
			dwgfx.drawfillrect(x, y, 1, y2, c);
			dwgfx.drawfillrect(x + x2 - 1, y, 1, y2, c);
		}
		
		public static function generatecustomminimap(dwgfx:dwgraphicsclass, map:mapclass):void {
			map.customwidth = mapwidth;
			map.customheight = mapheight;
			
			map.customzoom=1;
			if(map.customwidth<=10 && map.customheight<=10) map.customzoom=2;
			if (map.customwidth <= 5 && map.customheight <= 5) map.customzoom = 4;
			
			//Set minimap offsets
			if(map.customzoom==4){
				map.custommmxoff = 24 * (5 - map.customwidth);
				map.custommmxsize = 240 - (map.custommmxoff * 2);
				
				map.custommmyoff = 18 * (5 - map.customheight);
				map.custommmysize = 180 - (map.custommmyoff * 2);
			}else if(map.customzoom==2){
				map.custommmxoff = 12 * (10 - map.customwidth);
				map.custommmxsize = 240 - (map.custommmxoff * 2);
				
				map.custommmyoff=9*(10-map.customheight);
				map.custommmysize=180-(map.custommmyoff*2);
			}else{
				map.custommmxoff = 6 * (20 - map.customwidth);
				map.custommmxsize = 240 - (map.custommmxoff * 2);
				
				map.custommmyoff = int(4.5 * (20 - map.customheight));
				map.custommmysize = 180 - (map.custommmyoff * 2);
			}
			
			if (dwgfx.customminimap == null) {
			  dwgfx.customminimap = new RenderTexture(240, 180);	
				dwgfx.images[12] = new Image(dwgfx.customminimap);
			}
			dwgfx.customminimap.clear(dwgfx.RGB(0, 0, 0), 1.0);
			
			dwgfx.customminimap.drawBundled(function():void {
				var tm:int = 0;
				var temp:int = 0;
				//Scan over the map size
				if(mapheight<=5 && mapwidth<=5){
					//4x map
					for (var j2:int = 0; j2 < mapheight; j2++){
						for (var i2:int = 0; i2 < mapwidth; i2++) {
							//Ok, now scan over each square
							tm = 196;
							if (level[i2 + (j2 * maxwidth)].tileset == 1) tm = 96;
							
							for (var j:int = 0; j < 36; j++) {
								for (var i:int = 0; i < 48; i++) {
									temp = absfree(int(i * 0.83) + (i2 * 40), int(j * 0.83) + (j2 * 30));
									if(temp>=1){
										//Fill in this pixel
										dwgfx.drawfillrect_onimage(dwgfx.customminimap, (i2 * 48) + i, (j2 * 36) + j, 1, 1, tm, tm, tm);
									}
								}
							}
						}
					}
				}else if (mapheight <= 10 && mapwidth <= 10) {
					//2x map
					for (j2 = 0; j2 < mapheight; j2++) {
						for (i2 = 0; i2 < mapwidth; i2++) {
							//Ok, now scan over each square
							tm = 196;
							if (level[i2 + (j2 * maxwidth)].tileset == 1) tm = 96;
							
							for (j = 0; j < 18; j++) {
								for (i = 0; i < 24; i++) {
									temp = absfree(int(i * 1.6) + (i2 * 40), int(j * 1.6) + (j2 * 30));
									if (temp >= 1) {
										//Fill in this pixel
										dwgfx.drawfillrect_onimage(dwgfx.customminimap, (i2 * 24) + i, (j2 * 18) + j, 1, 1, tm, tm, tm);
									}
								}
							}
						}
					}
				}else {
					for (j2 = 0; j2 < mapheight; j2++) {
						for (i2 = 0; i2 < mapwidth; i2++) {
							//Ok, now scan over each square
							tm=196;
							if (level[i2 + (j2 * maxwidth)].tileset == 1) tm = 96;
							
							for (j = 0; j < 9; j++) {
								for (i = 0; i < 12; i++) {								
									temp = absfree(3 + (i * 3) + (i2 * 40), (j * 3) + (j2 * 30));
									if(temp>=1){
										//Fill in this pixel
										dwgfx.drawfillrect_onimage(dwgfx.customminimap, (i2 * 12) + i, (j2 * 9) + j, 1, 1, tm, tm, tm);
									}
								}
							}
						}
					}
				}
			});
		}
		
		public static function editorrender(dwgfx:dwgraphicsclass, game:gameclass, map:mapclass, obj:entityclass, help:helpclass):void {
		  //Not used
			trace("ERROR: Editorrender called");
		}
		
		public static function editorlogic(dwgfx:dwgraphicsclass, game:gameclass, obj:entityclass, music:musicclass, map:mapclass, help:helpclass):void {
			//Not used
			trace("ERROR: Editorlogic called");
		}
		
		public static function editorinput(dwgfx:dwgraphicsclass, game:gameclass, map:mapclass, obj:entityclass, help:helpclass, music:musicclass):void {
			//Not used
			trace("ERROR: Editorinput called");
		}
		
		public static var customscript:Vector.<String> = new Vector.<String>;
		
		public static var editordata:EditorDataclass = new EditorDataclass;
		public static var edentity:Vector.<edentitiesclass> = new Vector.<edentitiesclass>;
		
		public static var Desc1:String;
		public static var Desc2:String;
		public static var Desc3:String;
		public static var website:String;
		
		public static var directoryList:Vector.<String> = new Vector.<String>;
		public static var ListOfMetaData:Vector.<LevelMetaData> = new Vector.<LevelMetaData>;
		
		public static var entcol:int;
		
		public static var swapmap:Vector.<int> = new Vector.<int>;
		public static var contents:Vector.<int> = new Vector.<int>;
		public static var vmult:Vector.<int> = new Vector.<int>;
		public static var numtrinkets:int;
		public static var numcrewmates:int;
		public static var level:Vector.<edlevelclass> = new Vector.<edlevelclass>;
		
		public static var temp:int;
		public static var notedelay:int;
		public static var note:String;
		public static var keybuffer:String;
		public static var filename:String;
		
		public static var drawmode:int;
		public static var tilex:int, tiley:int;
		public static var keydelay:int, lclickdelay:int;
		public static var savekey:Boolean, loadkey:Boolean;
		public static var levx:int, levy:int;
		public static var entframe:int, entframedelay:int;
		
		public static var roomtextmod:Boolean;
		public static var roomtextent:int;
		
		public static var scripttextmod:Boolean;
		public static var scripttextent:int;
		public static var scripttexttype:int;
		
		public static var xmod:Boolean, zmod:Boolean, spacemod:Boolean, warpmod:Boolean;
		public static var roomnamemod:Boolean, textentry:Boolean, savemod:Boolean, loadmod:Boolean;
		public static var titlemod:Boolean, creatormod:Boolean, desc1mod:Boolean;
		public static var desc2mod:Boolean, desc3mod:Boolean, websitemod:Boolean;
		
		public static var roomnamehide:int;
		public static var saveandquit:Boolean;
		public static var shiftmenu:Boolean, shiftkey:Boolean;
		public static var spacemenu:int;
		public static var settingsmod:Boolean, settingskey:Boolean;
		public static var warpent:int;
		public static var updatetiles:Boolean, changeroom:Boolean;
		public static var deletekeyheld:int;
		
		public static var boundarymod:int, boundarytype:int;
		public static var boundx1:int, boundx2:int, boundy1:int, boundy2:int;
		
		public static var levmusic:int;
		public static var mapwidth:int, mapheight:int; //Actual width and height of stage
		public static var maxwidth:int, maxheight:int; //Special; the physical max the engine allows
		
		public static var version:int;
		
		public static var m_editorSavePath:String;
		
		public static var scripteditmod:Boolean;
		public static var scripthelppage:int, scripthelppagedelay:int;
		public static var sb:Vector.<String> = new Vector.<String>;
		public static var sbscript:String;
		public static var sblength:int;
		public static var sbx:int, sby:int;
		public static var pagey:int;
		
		public static var author:String;
		public static var description:String;
		public static var title:String;
		
		public static var hooklist:Vector.<String> = new Vector.<String>;
		public static var numhooks:int;
		
		public static var hookmenupage:int, hookmenu:int;
		
		//Direct Mode variables
		public static var dmtile:int;
		public static var dmtileeditor:int;
		
		public static var playerlevel:Vector.<XML> = new Vector.<XML>;
		public static var playerlevel_spikyness:Vector.<int> = new Vector.<int>;
		
		[Embed(source='../data/playerlevels/4kvvvv.vvvvvv', mimeType="application/octet-stream")]
    public static const lvl_4kvvvv:Class;
		[Embed(source='../data/playerlevels/333333_easy.vvvvvv', mimeType="application/octet-stream")]
    public static const lvl_333333_easy:Class;
		[Embed(source='../data/playerlevels/a_new_dimension.vvvvvv', mimeType="application/octet-stream")]
    public static const lvl_a_new_dimension:Class;
		[Embed(source='../data/playerlevels/linewrap.vvvvvv', mimeType="application/octet-stream")]
    public static const lvl_linewrap:Class;
		[Embed(source='../data/playerlevels/pyramid.vvvvvv', mimeType="application/octet-stream")]
    public static const lvl_pyramid:Class;
		[Embed(source='../data/playerlevels/quantumtunnel12.vvvvvv', mimeType="application/octet-stream")]
    public static const lvl_quantumtunnel12:Class;
		[Embed(source='../data/playerlevels/roadtrip.vvvvvv', mimeType="application/octet-stream")]
    public static const lvl_roadtrip:Class;
		[Embed(source='../data/playerlevels/seasons.vvvvvv', mimeType="application/octet-stream")]
    public static const lvl_seasons:Class;
		[Embed(source='../data/playerlevels/soulsearching.vvvvvv', mimeType="application/octet-stream")]
    public static const lvl_soulsearching:Class;
		[Embed(source='../data/playerlevels/the_dual_challenge.vvvvvv', mimeType="application/octet-stream")]
    public static const lvl_the_dual_challenge:Class;
		[Embed(source='../data/playerlevels/towerofpower.vvvvvv', mimeType="application/octet-stream")]
    public static const lvl_towerofpower:Class;
		[Embed(source='../data/playerlevels/variationventure.vvvvvv', mimeType="application/octet-stream")]
    public static const lvl_variationventure:Class;
		[Embed(source='../data/playerlevels/varietyshow.vvvvvv', mimeType="application/octet-stream")]
    public static const lvl_varietyshow:Class;
		[Embed(source='../data/playerlevels/vertexvortex.vvvvvv', mimeType="application/octet-stream")]
    public static const lvl_vertexvortex:Class;
		[Embed(source='../data/playerlevels/vertiginousviridian.vvvvvv', mimeType="application/octet-stream")]
    public static const lvl_vertiginousviridian:Class;
		[Embed(source='../data/playerlevels/victuals.vvvvvv', mimeType="application/octet-stream")]
    public static const lvl_victuals:Class;
		[Embed(source='../data/playerlevels/vvvvvvgoldenspiral.vvvvvv', mimeType="application/octet-stream")]
    public static const lvl_vvvvvvgoldenspiral:Class;
	}
};
