package{
	import flash.display.*;
	import flash.geom.*;
  import flash.events.*;
  import flash.net.*;
	
	public class dwgraphicsclass extends Sprite {
		public var BUTTON_BLANK:int = 0;
		public var BUTTON_MENU:int = 1;
		public var BUTTON_BACK:int = 2;
		public var BUTTON_TALK:int = 3;
		public var BUTTON_USE:int = 4;
		public var BUTTON_TELEPORT:int = 5;
		public var BUTTON_GAMECENTER:int = 6;
		public var BUTTON_CONTROLS:int = 7;
		public var BUTTON_DPAD:int = 8;
		
		public function init():void {
			//We initialise a few things
			updatebackground = true; 
			tiles_rect=new Rectangle(0,0,8,8);
			sprites_rect=new Rectangle(0,0,32,32);
			bfont_rect=new Rectangle(0,0,8,8);
			bfontmask_rect=new Rectangle(0,0,9,9);
			bg_rect = new Rectangle(0, 0, 320, 240);
			footerrect = new Rectangle(0, 230, 320, 10);
			prect = new Rectangle(0, 0, 4, 4);
			line_rect = new Rectangle();
			tl = new Point(0, 0); tpoint = new Point(0, 0); trect = new Rectangle();
			ct = new ColorTransform(0, 0, 0, 1, 255, 255, 255, 1); //Set to white			
			madpoint = new Point;
			madrect = new Rectangle;
			
			tele_rect = new Rectangle(0, 0, 96, 96); 
			trinketcolset = false;
			
			showcutscenebars = false;
		  cutscenebarspos = 0;
			
			flipmode = false; setflipmode = false;
			flipmatrix.scale(1, -1);
			flipmatrix.translate(0, 240);
			flipfontmatrix.scale(1, -1);	flipfontmatrix.translate(0, 8);
			flipfontmatrix2.scale(1, -1);	flipfontmatrix2.translate(0, 9);
			
			//Background inits
			for (i = 0; i < 50; i++) {
				var s:Rectangle = new Rectangle(Math.random() * 320, Math.random() * 240, 2, 2);
				var s2:int = 4+(Math.random()*4);
				stars.push(s); starsspeed.push(s2);
				
				var bb:Rectangle;
				var bvx:int = 0;	var bvy:int = 0;
				if(Math.random()*100 > 50){
				  bvx = 9 - (Math.random() * 19); if (bvx > -6 && bvx < 6) bvx = 6;
					 bvx = bvx * 1.5;
					bb = new Rectangle(Math.random() * 320, Math.random() * 240, 32, 12);
  			}else {
				  bvy = 9 - (Math.random() * 19); if (bvy > -6 && bvy < 6) bvy = 6;
					 bvy = bvy * 1.5;
					bb = new Rectangle(Math.random() * 320, Math.random() * 240, 12, 32);
				}
				var bint:Number = 0.5 + ((Math.random() * 100) / 200);
				backboxes.push(bb); 
				backboxvx.push(bvx); backboxvy.push(bvy);
				backboxint.push(bint);
			}
			backboxrect = new Rectangle();
			backoffset = 0; backgrounddrawn = false;
			
			warpskip = 0;
			warpfcol = 0x000000; warpbcol = 0x000000;
		  warprect = new Rectangle(0, 0, 0, 0);
			
			spcol = 0; spcoldel = 0;
			
			crewframe = 0; crewframedelay = 4;
			menuoffset = 0; resumegamemode = false;
			
  		//Textboxes!
			for (i = 0; i < 30; i++) {
				var t:textboxclass = new textboxclass;
				textbox.push(t);
			}
			ntextbox = 0;
			
			//Fading stuff
			for (i = 0; i < 15; i++) {
				fadebars.push(0);
			}
			fadeamount = 0;
			fademode = 0;
		  
		  bigbuffer = new BitmapData(320, 32, true, 0x000000);
			bigbufferscreen = new Bitmap(bigbuffer);
			bigbufferscreen.width = 640;
      bigbufferscreen.height = 20;
			
			towerbuffer = new BitmapData(320, 240, true, 0x000000);
			frontbuffer = new BitmapData(320, 240, true, 0x000000);
			
			menubuffer = new BitmapData(320, 240, false, 0x000000);  //yeah yeah I know this is lazy
			
			backbuffer=new BitmapData(320, 240,false,0x000000);
		  screenbuffer = new BitmapData(320,240,false,0x000000);
			
		  temptile = new BitmapData(8, 8, false, 0x000000);
		  tempsprite = new BitmapData(32, 32, false, 0x000000);
			screen = new Bitmap(screenbuffer);
			
			//screen.width = 640;//320;//;640;
      //screen.height = 480;// 240;//480;
			screen.smoothing = true;
			
			addChild(screen);		
			
			buttonscreen.push(new Bitmap(buttonimg[0]));
			buttonscreen.push(new Bitmap(buttonimg[1]));
			buttonscreen.push(new Bitmap(buttonimg[2]));
			buttonscreen.push(new Bitmap(buttonimg[3]));
			
			/*
			if (device.deviceresolution == device.IPAD) {
				buttonsize = 90;
				buttonyspacing = buttonsize / 3;
				buttonxspacing = 40;	
				buttonscreen[0].width = 92*3; buttonscreen[0].height = buttonsize;
				buttonscreen[1].width = 92*3; buttonscreen[1].height = buttonsize;
				buttonscreen[2].width = 40*3; buttonscreen[2].height = 40*3;
				buttonscreen[3].width = 40 * 3; buttonscreen[3].height = 40 * 3;
				buttonydiff = buttonscreen[2].height / 2;
			}else if (device.deviceresolution == device.ANDROID) {
			*/
				
				/*
			}else {
				buttonsize = 30;
				buttonyspacing = buttonsize / 3;
				buttonxspacing = 20;	
				buttonscreen[0].width = 92; buttonscreen[0].height = buttonsize;
				buttonscreen[1].width = 92; buttonscreen[1].height = buttonsize;
				buttonscreen[2].width = 40; buttonscreen[2].height = 40;
				buttonscreen[3].width = 40; buttonscreen[3].height = 40;
				buttonydiff = buttonscreen[2].height / 2;
			}
			*/
			
			/*buttonscreen.push(new Bitmap(buttonimg[1]));
			buttonscreen[1].width = buttonsize; buttonscreen[1].height = buttonsize;
			buttonscreen.push(new Bitmap(buttonimg[2]));
			buttonscreen[2].width = buttonsize; buttonscreen[2].height = buttonsize;
			buttonscreen.push(new Bitmap(buttonimg[3]));
			buttonscreen[3].width = buttonsize; buttonscreen[3].height = buttonsize;
			*/
			
			for (i = 0; i < 10; i++) {
				buttonlerp.push(int(0));
				buttonstate.push(int(0));
				currentbuttontype.push(int(-1));
				newbuttontype.push(int(-1));
				buttonframe.push(int(-1));
			}
			
			buttonactive.push(false);
			buttonactive.push(false);
			buttonactive.push(false);
			buttonactive.push(false);
			buttonhighlight.push(0); buttonhighlight.push(0);	buttonhighlight.push(0); buttonhighlight.push(0);
			buttonpos.push(new Point(0, 0)); buttonpos.push(new Point(0, 0));
			buttonpos.push(new Point(0, 0)); buttonpos.push(new Point(0, 0));
			//So, 0-4 are buffers that can be overwritten. 5+ contains graphics for each button.
			
			initbuttonpositions();
			
			/*
			buttonscreen[0].x = buttonxspacing;
			buttonscreen[0].y = device.yres - buttonsize-buttonyspacing-buttonydiff;
			buttonpos.push(new Point(buttonscreen[0].x, buttonscreen[0].y));
			
			buttonscreen[1].x = buttonxspacing + buttonsize + buttonxspacing;
			buttonscreen[1].y = device.yres - buttonsize-buttonyspacing;
			buttonpos.push(new Point(buttonscreen[1].x, buttonscreen[1].y));
			
			buttonscreen[2].x = device.xres - buttonxspacing - buttonsize;
			buttonscreen[2].y = buttonsize / 4;
			buttonpos.push(new Point(buttonscreen[2].x, buttonscreen[2].y));
			
			buttonscreen[3].x = device.xres - buttonxspacing - buttonsize - buttonxspacing - buttonsize;
			buttonscreen[3].y = device.yres - buttonsize-buttonyspacing;
			buttonpos.push(new Point(buttonscreen[3].x, buttonscreen[3].y));
			*/
			drawonscreenbutton(0, -1);
			drawonscreenbutton(1, -1);
			drawonscreenbutton(2, -1);
			drawonscreenbutton(3, -1);
			addChild(buttonscreen[0]);
			addChild(buttonscreen[1]);
			addChild(buttonscreen[2]);
			addChild(buttonscreen[3]);
			/*
			drawonscreenbutton(0, 0);
	    drawonscreenbutton(1, 0);
	    drawonscreenbutton(2, 0);
	    drawonscreenbutton(3, 0);
			showarrows();
			*/
    }
		
		public function initbuttonpositions():void {
			devicex = device.xres;
			devicey = device.yres;
			
			buttonsize = device.yres / (32 / 3);
			buttonyspacing = buttonsize / 3;
			buttonxspacing = (buttonsize * 3) / 3;	
			buttonscreen[0].width = (buttonsize * 46) / 15; buttonscreen[0].height = buttonsize;
			buttonscreen[1].width = (buttonsize * 46) / 15; buttonscreen[1].height = buttonsize;
			buttonscreen[2].width = (buttonsize * 5) / 3; buttonscreen[2].height = (buttonsize * 5) / 3;
			buttonscreen[3].width = (buttonsize * 5) / 3; buttonscreen[3].height = (buttonsize * 5) / 3;
			buttonydiff = buttonscreen[2].height / 2;
			
			buttonscreen[0].x = device.xres + 1;
			buttonscreen[0].y = 0;
			buttonpos[0].setTo(buttonscreen[0].x, buttonscreen[0].y);
			
			buttonscreen[1].x = 0;
			buttonscreen[1].y = 0;
			buttonpos[1].setTo(buttonscreen[1].x, buttonscreen[1].y);
			
			buttonscreen[2].x = (buttonxspacing / 2);
			buttonscreen[2].y = device.yres - buttonsize-buttonyspacing - buttonydiff;
			buttonpos[2].setTo(buttonscreen[2].x, buttonscreen[2].y);
			
			buttonscreen[3].x = (buttonxspacing/2) + buttonsize + buttonxspacing;
			buttonscreen[3].y = device.yres - buttonsize-buttonyspacing;
			buttonpos[3].setTo(buttonscreen[3].x, buttonscreen[3].y);
		}
		
		public function mobile_changebutton(t:int):void {
			if (newbuttontype[0] != t) {
				if (currentbuttontype[0] != t) {
					newbuttontype[0] = t;
					buttonstate[0] = 1;
				}
			}
		}
		
		public function mobile_changeleftbutton(t:int):void {
			if (newbuttontype[1] != t) {
				if (currentbuttontype[1] != t) {
					newbuttontype[1] = t;
					buttonstate[1] = 1;
				}
			}
		}
		
		public function mobile_hidearrows():void {
			if (newbuttontype[2] != BUTTON_BLANK) {
				if (currentbuttontype[2] != BUTTON_BLANK) {
					newbuttontype[2] = BUTTON_BLANK;
					buttonstate[2] = 1;
				}
			}
		}
		
		public function mobile_showarrows(t:int):void {
			if (t == 2) {
				if (newbuttontype[2] != BUTTON_DPAD) {
					if (currentbuttontype[2] != BUTTON_DPAD) {
						newbuttontype[2] = BUTTON_DPAD;
						buttonstate[2] = 1;
					}
				}
			}
		}
		
		public function drawbutton(game:gameclass, help:helpclass):void {
			//Called every frame, this function controls what buttons appear and when,
			//and lerps them in and out in a nice way.
			//buttonstate[0]:
			// 0 - Normal, slide in if not in
			// 1 - Sliding out, change to new button state
			for (i = 0; i < 3; i++){
				switch(buttonstate[i]) {
					case 0:
						if (buttonlerp[i] > 60) {
							buttonlerp[i] = 60;
						}
						if (buttonlerp[i] > 0) {
							buttonlerp[i] -= 20;
							if (buttonlerp[i] < 0) {
								buttonlerp[i] = 0;
							}
						}
					break;
					case 1:
						if (buttonlerp[i] < 100) {
							buttonlerp[i] += 20;
						}
						if (buttonlerp[i] >= 60) buttonlerp[i] = 100;
						
						if (buttonlerp[i] >= 100) {
							buttonlerp[i] = 100;
							buttonstate[i] = 0;
							currentbuttontype[i] = newbuttontype[i];
							newbuttontype[i] = -1;
						}
					break;
				}
			}
			
			switch(currentbuttontype[2]) {
				case BUTTON_BLANK:
					changebuttonframe(2, -1);
					changebuttonframe(3, -1);
				break;
				case BUTTON_DPAD:
					changebuttonframe(2, 11);
					changebuttonframe(3, 12);
					
					if (game.press_left) { 
						changebuttonpos(2, buttonxspacing/2, devicey - buttonsize-buttonyspacing - buttonydiff+10);
					}else{
						changebuttonpos(2, buttonxspacing/2, devicey - buttonsize-buttonyspacing - buttonydiff);
					}
					
					if (game.press_right) { 
						changebuttonpos(3, (buttonxspacing/2)+ buttonsize + buttonxspacing, devicey - buttonsize-buttonyspacing - buttonydiff+10);
					}else{
						changebuttonpos(3, (buttonxspacing/2) + buttonsize + buttonxspacing, devicey - buttonsize-buttonyspacing - buttonydiff);
					}
				break;
			}
			
			switch(currentbuttontype[1]) {
				case BUTTON_BLANK:
					//Blank
					changebuttonframe(1, -1);
					changebuttonpos(1, 0 - ((buttonscreen[1].width * (buttonlerp[1])) / 100), 0);
				break;
				case BUTTON_CONTROLS:
					//Controls button
					if (game.menupage >= 30) {
						changebuttonframe(1, 9);	
					}else{
						changebuttonframe(1, 8);
					}
					if (flipmode) {
						changebuttonpos(1, 0 - ((buttonscreen[1].width * (buttonlerp[1])) / 100), devicey-buttonyspacing-buttonscreen[1].height);	
					}else {
						changebuttonpos(1, 0 - ((buttonscreen[1].width * (buttonlerp[1])) / 100), buttonyspacing);	
					}
					
				break;
				case BUTTON_GAMECENTER:
					changebuttonframe(1, 10);
					changebuttonpos(1, 0 - ((buttonscreen[1].width * (buttonlerp[1])) / 100), 5);
				break;
			}
			
			switch(currentbuttontype[0]) {
				case BUTTON_BLANK:
					//Blank
					changebuttonframe(0, -1);
					changebuttonpos(0, devicex - ((buttonscreen[0].width * (100-buttonlerp[0])) / 100), 0);
				break;
				case BUTTON_MENU:
					//Menu button
				  changebuttonframe(0, 0);
					changebuttonpos(0, devicex - ((buttonscreen[0].width * (100-buttonlerp[0])) / 100), 0);
				break;
				case BUTTON_BACK:
					//Back button
					changebuttonframe(0, 1);
					if (flipmode) {
						changebuttonpos(0, devicex - ((buttonscreen[0].width * (100 - buttonlerp[0])) / 100), devicey - buttonyspacing - buttonscreen[0].height);
					}else{
						changebuttonpos(0, devicex - ((buttonscreen[0].width * (100 - buttonlerp[0])) / 100), buttonyspacing);
					}
				break;
				case BUTTON_USE:
					//Use button
					if (help.slowsine % 16 > 8) {
						changebuttonframe(0, 4);
					}else {
						changebuttonframe(0, 5);
					}
					changebuttonpos(0, devicex - ((buttonscreen[0].width * (100-buttonlerp[0])) / 100), buttonyspacing);
				break;
				case BUTTON_TALK:
					//Talk button
					if (help.slowsine % 16 > 8) {
						changebuttonframe(0, 2);
					}else {
						changebuttonframe(0, 3);
					}
					changebuttonpos(0, devicex - ((buttonscreen[0].width * (100-buttonlerp[0])) / 100), buttonyspacing);
				break;
				case BUTTON_TELEPORT:
					//Teleport button
					if (help.slowsine % 16 > 8) {
						changebuttonframe(0, 6);
					}else {
						changebuttonframe(0, 7);
					}
					changebuttonpos(0, devicex - ((buttonscreen[0].width * (100-buttonlerp[0])) / 100), buttonyspacing);
				break;
			}
			
			/*
			if (game.press_left) { drawonscreenbutton(0, 1);		
			}else{ drawonscreenbutton(0, 0);}
			
			if (game.press_right) { drawonscreenbutton(1, 1);		
			}else { drawonscreenbutton(1, 0); }
			
			if (game.press_map) { drawonscreenbutton(2, 1);		
			}else { drawonscreenbutton(2, 0); }
			*/
			//Draw circle pad!
	//if (key.controlstick != -1) {
	/*
		backbuffer.copyPixels(dwgfx.buttonimg[13], dwgfx.buttonimg[13].rect, new Point(key.controlstick_x - 18+18, key.controlstick_y - 18+18));
		for (i = 0; i < key.touchPoints; i++) {
			if (key.touchid[i] == key.controlstick) {
				dwgfx.backbuffer.copyPixels(dwgfx.buttonimg[14], dwgfx.buttonimg[14].rect, new Point(key.touchx[i] - 18, key.touchy[i] - 18));
			}
		}
		*/
	//}
		}
		
		public function addbutton():void {
			var t:BitmapData = new BitmapData(buffer.width, buffer.height, true, 0x00000000);
			t.copyPixels(buffer, new Rectangle(0,0,buffer.width, buffer.height), new Point(0,0));
			buttonimg.push(t);
		}
		
		public function drawonscreenbutton(t:int, t2:int):void {
			//Draw button t with frame t2
			if (t2 == -1) {
				buttonimg[t].fillRect(buttonimg[t].rect, 0x00000000);
			}
			if (buttonframe[t] != t2) {
				buttonframe[t] = t2;
				buttonimg[t].fillRect(buttonimg[t].rect, 0x00000000);
				buttonimg[t].copyPixels(buttonimg[4 + t2], buttonimg[t].rect, tl);
			}
		}
		
		public function changebuttonframe(t:int, t2:int):void {
			//Draw button t with frame t2
			if (buttonframe[t] != t2) {
				buttonframe[t] = t2;
				if (t2 == -1) {
					buttonimg[t].fillRect(buttonimg[t].rect, 0x00000000);
				}else{
					buttonimg[t].fillRect(buttonimg[t].rect, 0x00000000);
					buttonimg[t].copyPixels(buttonimg[4 + t2], buttonimg[t].rect, tl);
				}
			}
		}
		
		public function changebuttonpos(t:int, xp:int, yp:int):void {
			buttonscreen[t].x = xp;
			buttonscreen[t].y = yp;
		}
		
		public function changebuttonxpos(t:int, xp:int):void {
			buttonscreen[t].x = xp;
		}
		
		public function showarrows():void {
		  if (buttonactive[0] == false) {
				buttonhighlight[0] = 120;
				buttonactive[0] = true;
				addChild(buttonscreen[0]);
				addChild(buttonscreen[1]);
				addChild(buttonscreen[2]);
			}
		}
		
		public function hidearrows():void {
		  if (buttonactive[0] == true) {
				buttonactive[0] = false;
				removeChild(buttonscreen[0]);
				removeChild(buttonscreen[1]);
				removeChild(buttonscreen[2]);
			}
		}
		
		
		public function drawspritesetcol(x:int, y:int, t:int, c:int, help:helpclass):void {
		  tpoint.x = x; tpoint.y = y;
			setcol(c, help); 
			sprites[t].colorTransform(sprites_rect, ct);
			backbuffer.copyPixels(sprites[t], sprites_rect, tpoint);
		}
		
		
		public function makebfont():void {
			for (var j:Number = 0; j < 16; j++) {
				for (var i:Number = 0; i < 16; i++) {
					var t:BitmapData = new BitmapData(8, 8, true, 0x000000);
					var t2:BitmapData = new BitmapData(8, 8, true, 0x000000);
					var temprect:Rectangle = new Rectangle(i * 8, j * 8, 8, 8);	
					t.copyPixels(buffer, temprect, tl);
					t2.draw(t, flipfontmatrix);
					bfont.push(t);
					flipbfont.push(t2);
				}
			}
			
			//Ok, now we work out the lengths (this data string cortesy of a program I wrote!)
			for (i = 0; i < 256; i++) bfontlen.push(6);
			var maprow:Array;
			var tstring:String="4,3,5,7,6,7,6,3,4,4,7,7,3,5,2,5,6,5,6,6,6,6,6,6,6,6,2,3,5,5,5,6,7,6,6,6,6,5,5,6,6,3,6,6,5,7,7,6,6,6,6,6,5,6,7,7,7,7,5,4,5,4,5,6,4,6,6,6,6,5,5,6,6,3,6,6,5,7,7,6,6,6,6,6,5,6,7,7,7,7,5,5,3,5,6,4";
					
			maprow = new Array();
			maprow = tstring.split(",");
			for(var k:int = 0; k < 96; k++) {
				bfontlen[k + 32] = 8;// int(maprow[k]);
			}	
		}
		
		public function makebfontmask():void {
			for (var j:Number = 0; j < 16; j++) {
				for (var i:Number = 0; i < 16; i++) {
					var t:BitmapData = new BitmapData(9, 9, true, 0x000000);
					var t2:BitmapData = new BitmapData(9, 9, true, 0x000000);
					var temprect:Rectangle = new Rectangle(i * 9, j * 9, 9, 9);	
					t.copyPixels(buffer, temprect, tl);
					t2.draw(t, flipfontmatrix2);
					bfontmask.push(t);
					flipbfontmask.push(t2);
				}
			}
		}
		
		public function makeentcolourarray():void {
			for (var j:Number = 0; j < 60; j++) {
				for (var i:Number = 0; i < 12; i++) {
					var t:BitmapData = new BitmapData(8, 8, true, 0x000000);
					var temprect:Rectangle = new Rectangle(i * 8, j * 8, 8, 8);	
					t.copyPixels(buffer, temprect, tl);
					entcolours.push(t);
				}
			}
		}
		
		public function maketilearray():void {
			for (var j:Number = 0; j < 30; j++) {
				for (var i:Number = 0; i < 40; i++) {
					var t:BitmapData = new BitmapData(8, 8, true, 0x000000);
					var temprect:Rectangle = new Rectangle(i * 8, j * 8, 8, 8);	
					t.copyPixels(buffer, temprect, tl);
					tiles.push(t);
				}
			}
		}
		
		public function maketile2array():void {
			for (var j:Number = 0; j < 30; j++) {
				for (var i:Number = 0; i < 40; i++) {
					var t:BitmapData = new BitmapData(8, 8, true, 0x000000);
					var temprect:Rectangle = new Rectangle(i * 8, j * 8, 8, 8);	
					t.copyPixels(buffer, temprect, tl);
					tiles2.push(t);
				}
			}
		}
		
		public function maketile3array():void {
			for (var j:Number = 0; j < 30; j++) {
				for (var i:Number = 0; i < 30; i++) {
					var t:BitmapData = new BitmapData(8, 8, true, 0x000000);
					var temprect:Rectangle = new Rectangle(i * 8, j * 8, 8, 8);	
					t.copyPixels(buffer, temprect, tl);
					tiles3.push(t);
				}
			}
		}
		
		public function makespritearray():void {
			for (var j:Number = 0; j < 16; j++) {
				for (var i:Number = 0; i < 12; i++) {
					var t:BitmapData = new BitmapData(32, 32, true, 0x000000);
					var temprect:Rectangle = new Rectangle(i * 32, j * 32, 32, 32);	
					t.copyPixels(buffer, temprect, tl);
					sprites.push(t);
				}
			}
		}
		
		public function makeflipspritearray():void {
			for (var j:Number = 0; j < 16; j++) {
				for (var i:Number = 0; i < 12; i++) {
					var t:BitmapData = new BitmapData(32, 32, true, 0x000000);
					var temprect:Rectangle = new Rectangle(i * 32, j * 32, 32, 32);	
					t.copyPixels(buffer, temprect, tl);
					flipsprites.push(t);
				}
			}
		}
		
		public function maketelearray():void {
			for (var i:Number = 0; i < 10; i++) {
			  var t:BitmapData = new BitmapData(96, 96, true, 0x000000);
				var temprect:Rectangle = new Rectangle(i * 96, 0, 96, 96);	
				t.copyPixels(buffer, temprect, tl);
				tele.push(t);
			}
		}
		
		public function drawmobilebutton(game:gameclass, xp:int, yp:int, wp:int, hp:int, t:String, cr:int, cg:int, cb:int, xoff:int = -8, yoff:int = -10):void {
			xp = xp + xoff;
			yp = yp + yoff;
			drawfillrect(xp + 4, yp + 4, wp, hp, cr * 0.25, cg * 0.25, cb * 0.25);
			drawfillrect(xp, yp, wp, hp, cr, cg, cb);
			drawfillrect(xp + 2, yp + 2, wp - 4, hp - 4, cr * 0.75, cg * 0.75, cb * 0.75);
			xp = xp + (wp / 2) - (t.length * 4);
			print(xp, yp + 9, t, 255, 255, 255);
		}
		
		public function drawmenu(game:gameclass, cr:int, cg:int, cb:int, help:helpclass, division:int = 30):void {
			if (game.mobilemenu) {
				for (i = 0; i < game.nummenuoptions; i++) {
					/*
					if (i == game.currentmenuoption) {
						//Draw it highlighted
						if (game.menuoptionsactive[i]) {
							tempstring = game.menuoptions[i];	tempstring = "[ " + tempstring.toUpperCase() + " ]";
							print(110 + (i * division) - 16 +game.menuxoff, 140 + (i * 12) +game.menuyoff, tempstring, cr, cg, cb);
						}else {
							tempstring = game.menuoptions[i];	tempstring = "[ " + tempstring + " ]";
							//Draw it in gray
							print(110 + (i * division) - 16 +game.menuxoff, 140 + (i * 12)+game.menuyoff, tempstring, 128, 128, 128);
						}
					}else {
					*/
					//Draw it normally
					if (game.menuoptionslayout[i] == 0) {
						//Normal Buttons
						if (game.menuoptionsactive[i]) {
							drawfillrect(160 - 70 + 4, 140 + (i * b_gap) + game.menuyoff - 6 + 4, 140, b_size, cr*0.25, cg*0.25, cb*0.25);
							drawfillrect(160 - 70, 140 + (i * b_gap) + game.menuyoff - 6, 140, b_size, cr, cg, cb);
							drawfillrect(160 - 70 + 2, 140 + (i * b_gap) + game.menuyoff - 6 + 2, 140 - 4, b_size - 4, cr*0.75, cg*0.75, cb*0.75);
							print(110 + (i * division) +game.menuxoff, 140 + (i * b_gap) + game.menuyoff+3, game.menuoptions[i], 255, 255, 255, true);
						}else {
							drawfillrect(160 - 70 + 4, 140 + (i * b_gap) + game.menuyoff - 6 + 4, 140, b_size, 128*0.25, 128*0.25, 128*0.25);
							drawfillrect(160 - 70, 140 + (i * b_gap) + game.menuyoff - 6, 140, b_size, 128, 128, 128);
							drawfillrect(160 - 70 + 2, 140 + (i * b_gap) + game.menuyoff - 6 + 2, 140 - 4, b_size - 4, 128*0.75, 128*0.75, 128*0.75);
							print(110 + (i * division) +game.menuxoff, 140 + (i * b_gap) + game.menuyoff+3, "LOCKED", 255, 255, 255, true);
						}
					}else if (game.menuoptionslayout[i] == 1) {
						//Super big button for main menu play 
						drawfillrect(160 - 70 + 4, 140 + (i * b_gap) -10+ game.menuyoff - 6 + 4, 140, b_size*2, cr*0.25, cg*0.25, cb*0.25);
						drawfillrect(160 - 70, 140 + (i * b_gap)-10 + game.menuyoff - 6, 140, b_size*2, cr, cg, cb);
						drawfillrect(160 - 70 + 2, 140 + (i * b_gap)-10 + game.menuyoff - 6 + 2, 140 - 4, (b_size*2) - 4, cr*0.75, cg*0.75, cb*0.75);
						bigprint(110 + (i * division) +game.menuxoff, 140 + (i * b_gap) -10 + game.menuyoff + 10, game.menuoptions[i], 255, 255, 255, true);
					}else if (game.menuoptionslayout[i] == 2) {
						//Main menu: Player levels and super gravitron
						if (game.menuoptionsactive[i]) {
							drawfillrect(160 - 70 + 4, 140 + (i * b_gap)+20 + game.menuyoff - 6 + 4, 140, b_size, cr*0.25, cg*0.25, cb*0.25);
							drawfillrect(160 - 70, 140 + (i * b_gap)+20 + game.menuyoff - 6, 140, b_size, cr, cg, cb);
							drawfillrect(160 - 70 + 2, 140 + (i * b_gap) +20+ game.menuyoff - 6 + 2, 140 - 4, b_size - 4, cr*0.75, cg*0.75, cb*0.75);
							print(110 + (i * division) +game.menuxoff, 140 + (i * b_gap) + 20 + game.menuyoff+3, game.menuoptions[i], 255, 255, 255, true);
						}else {
							drawfillrect(160 - 70 + 4, 140 + (i * b_gap)+20 + game.menuyoff - 6 + 4, 140, 20, 128*0.25, 128*0.25, 128*0.25);
							drawfillrect(160 - 70, 140 + (i * b_gap)+20 + game.menuyoff - 6, 140, 20, 128, 128, 128);
							drawfillrect(160 - 70 + 2, 140 + (i * b_gap) +20+ game.menuyoff - 6 + 2, 140 - 4, 20 - 4, 128*0.75, 128*0.75, 128*0.75);
							print(110 + (i * division) +game.menuxoff, 140 + (i * b_gap) + 20 + game.menuyoff+3, "LOCKED", 255, 255, 255, true);
						}
					}else if (game.menuoptionslayout[i] == 3) {
						//Back Button
						drawfillrect(-4, 230 - 6 + 4, 70, b_size, cr*0.10, cg*0.10, cb*0.10);
						drawfillrect(-8, 230 - 6, 70, b_size, cr*0.5, cg*0.5, cb*0.5);
						drawfillrect(-8+2, 230 - 6 + 2, 70 - 4, b_size - 4, cr*0.4, cg*0.4, cb*0.4);
						print(8, 230, game.menuoptions[i], 196, 196, 196);
					}else if (game.menuoptionslayout[i] == 4) {
						//Normal Buttons except wider for longer text
						if (game.menuoptionsactive[i]) {
							drawfillrect(160 - 90 + 4, 140 + (i * b_gap) + game.menuyoff - 6 + 4, 180, b_size, cr*0.25, cg*0.25, cb*0.25);
							drawfillrect(160 - 90, 140 + (i * b_gap) + game.menuyoff - 6, 180, b_size, cr, cg, cb);
							drawfillrect(160 - 90 + 2, 140 + (i * b_gap) + game.menuyoff - 6 + 2, 180 - 4, b_size - 4, cr*0.75, cg*0.75, cb*0.75);
							print(110 + (i * division) +game.menuxoff, 140 + (i * b_gap) + game.menuyoff+3, game.menuoptions[i], 255, 255, 255, true);
						}else {
							drawfillrect(160 - 90 + 4, 140 + (i * b_gap) + game.menuyoff - 6 + 4, 180, b_size, 128*0.25, 128*0.25, 128*0.25);
							drawfillrect(160 - 90, 140 + (i * b_gap) + game.menuyoff - 6, 180, b_size, 128, 128, 128);
							drawfillrect(160 - 90 + 2, 140 + (i * b_gap) + game.menuyoff - 6 + 2, 180 - 4, b_size - 4, 128*0.75, 128*0.75, 128*0.75);
							print(110 + (i * division) +game.menuxoff, 140 + (i * b_gap) + game.menuyoff+3, "LOCKED", 255, 255, 255, true);
						}
					}else if (game.menuoptionslayout[i] == 5) {
						//Time trial button, left
						
						if (game.menuoptionsactive[i]) {
							drawfillrect(10, 140 + (i * 68) -10+ game.menuyoff - 6 + 4, 140, 40, cr*0.25, cg*0.25, cb*0.25);
							drawfillrect(10, 140 + (i * 68)-10 + game.menuyoff - 6, 140, 40, cr, cg, cb);
							drawfillrect(12, 140 + (i * 68)-10 + game.menuyoff - 6 + 2, 140 - 4, 40 - 4, cr*0.75, cg*0.75, cb*0.75);
							print(18, 140 + (i * 68) -10 + game.menuyoff + 20, game.menuoptions[i], 255, 255, 255);
							
							if (game.besttimes[i] == -1) {
								print( 18, 140 + (i * 68) -10 + game.menuyoff + 3, "Not attempted", 196, 196, 196, false);	
							}else {
								print( 18, 140 + (i * 68) -10 + game.menuyoff + 3, game.timetstring(game.besttimes[i], help), 196, 196, 196);	
								
								print( 18+72, 140 + (i * 68) -10 + game.menuyoff + 3, "Rank: ", 196, 196, 196);	
								switch(game.bestrank[i]) {
									case 0:	print( 18+116, 140 + (i * 68) -10 + game.menuyoff + 3, "B", 255, 255, 255); break;
									case 1:	print( 18+116, 140 + (i * 68) -10 + game.menuyoff + 3, "A", 255, 255, 255); break;
									case 2:	print( 18+116, 140 + (i * 68) -10 + game.menuyoff + 3, "S", 255, 255, 255); break;
									case 3:	print( 18+116, 140 + (i  * 68) -10 + game.menuyoff + 3, "V", 255, 255, 255); break;
								}
							}
						}else {
							drawfillrect(10, 140 + (i * 68) -10+ game.menuyoff - 6 + 4, 140, 40, 128*0.25, 128*0.25, 128*0.25);
							drawfillrect(10, 140 + (i * 68)-10 + game.menuyoff - 6, 140, 40, 128, 128, 128);
							drawfillrect(12, 140 + (i * 68)-10 + game.menuyoff - 6 + 2, 140 - 4, 40 - 4, 128*0.75, 128*0.75, 128*0.75);
							
							print( 18, 140 + (i * 68) -10 + game.menuyoff + 12, "Locked...", 255, 255, 255, false);	
						}
						
					}else if (game.menuoptionslayout[i] == 6) {
						//Time trial button, right
						
						if (game.menuoptionsactive[i]) {
							drawfillrect(170, 140 + ((i-3) * 68) -10+ game.menuyoff - 6 + 4, 140, 40, cr*0.25, cg*0.25, cb*0.25);
							drawfillrect(170, 140 + ((i-3) * 68)-10 + game.menuyoff - 6, 140, 40, cr, cg, cb);
							drawfillrect(172, 140 + ((i-3) * 68)-10 + game.menuyoff - 6 + 2, 140 - 4, 40 - 4, cr*0.75, cg*0.75, cb*0.75);
							print(178, 140 + ((i - 3) * 68) -10 + game.menuyoff + 20, game.menuoptions[i], 255, 255, 255);
							
							if (game.besttimes[i] == -1) {
								print( 178, 140 + ((i - 3) * 68) -10 + game.menuyoff + 3, "Not attempted", 196, 196, 196, false);	
							}else {
								print( 178, 140 + ((i - 3) * 68) -10 + game.menuyoff + 3, game.timetstring(game.besttimes[i], help), 196, 196, 196);	
								
								print( 178+72, 140 + ((i - 3) * 68) -10 + game.menuyoff + 3, "Rank: ", 196, 196, 196);	
								switch(game.bestrank[i]) {
									case 0:	print( 178+116, 140 + ((i - 3) * 68) -10 + game.menuyoff + 3, "B", 255, 255, 255); break;
									case 1:	print( 178+116, 140 + ((i - 3) * 68) -10 + game.menuyoff + 3, "A", 255, 255, 255); break;
									case 2:	print( 178+116, 140 + ((i - 3) * 68) -10 + game.menuyoff + 3, "S", 255, 255, 255); break;
									case 3:	print( 178+116, 140 + ((i - 3) * 68) -10 + game.menuyoff + 3, "V", 255, 255, 255); break;
								}
							}
						}else {
							drawfillrect(170, 140 + ((i-3) * 68) -10+ game.menuyoff - 6 + 4, 140, 40, 128*0.25, 128*0.25, 128*0.25);
							drawfillrect(170, 140 + ((i-3) * 68)-10 + game.menuyoff - 6, 140, 40, 128, 128, 128);
							drawfillrect(172, 140 + ((i - 3) * 68) - 10 + game.menuyoff - 6 + 2, 140 - 4, 40 - 4, 128 * 0.75, 128 * 0.75, 128 * 0.75);
							
							print( 178, 140 + ((i - 3) * 68) -10 + game.menuyoff + 12, "Locked...", 255, 255, 255, false);	
						}
					}else if (game.menuoptionslayout[i] == 7) {
						//Options button on the main menu - don't draw anything, just have a detection zone
					}else if (game.menuoptionslayout[i] == 8) {
						//credits button on the main menu - don't draw anything, just have a detection zone
					}else if (game.menuoptionslayout[i] == 9) {
						//For unlocking and relocking things
						if (game.menuoptionsactive[i]) {
							drawfillrect(160 - 90 + 4, 140 + (i * b_gap) + game.menuyoff - 6 + 4, 180, b_size, cr*0.25, cg*0.25, cb*0.25);
							drawfillrect(160 - 90, 140 + (i * b_gap) + game.menuyoff - 6, 180, b_size, cr, cg, cb);
							drawfillrect(160 - 90 + 2, 140 + (i * b_gap) + game.menuyoff - 6 + 2, 180 - 4, b_size - 4, cr*0.75, cg*0.75, cb*0.75);
							print(110 + (i * division) +game.menuxoff, 140 + (i * b_gap) + game.menuyoff+3, game.menuoptions[i], 255, 255, 255, true);
						}else {
							print(110 + (i * division) +game.menuxoff, 140 + (i * b_gap) + game.menuyoff+3, game.menuoptions_alt[i], 255, 255, 255, true);
						}
					}else if (game.menuoptionslayout[i] == 10) {
						//Player level menu next page button
						if (game.menuoptionsactive[i]) {
							drawfillrect(160 - 70 + 4, 240 - b_gap + 4, 140, b_size, cr*0.25, cg*0.25, cb*0.25);
							drawfillrect(160 - 70, 240 - b_gap, 140, b_size, cr, cg, cb);
							drawfillrect(160 - 70 + 2, 240 - b_gap + 2, 140 - 4, b_size - 4, cr*0.75, cg*0.75, cb*0.75);
							print(110 + (i * division) +game.menuxoff, 240 - b_gap+3+6, game.menuoptions[i], 255, 255, 255, true);
						}else {
							drawfillrect(160 - 70 + 4, 240 - b_gap + 4, 140, 20, 128*0.25, 128*0.25, 128*0.25);
							drawfillrect(160 - 70, 240 - b_gap, 140, 20, 128, 128, 128);
							drawfillrect(160 - 70 + 2, 240 - b_gap + 2, 140 - 4, 20 - 4, 128*0.75, 128*0.75, 128*0.75);
							print(110 + (i * division) +game.menuxoff, 240 - b_gap +3+6, "LOCKED", 255, 255, 255, true);
						}
					}else if (game.menuoptionslayout[i] == 11) {
						//Player level
						drawfillrect(160 - 140 + 4, 140 + (i * 65) -10+ game.menuyoff - 6 + 4, 280, (b_size*2)+5, cr*0.25, cg*0.25, cb*0.25);
						drawfillrect(160 - 140, 140 + (i * 65)-10 + game.menuyoff - 6, 280, (b_size*2)+5, cr, cg, cb);
						drawfillrect(160 - 140 + 2, 140 + (i * 65)-10 + game.menuyoff - 6 + 2, 280 - 4, (b_size*2) - 4+5, cr*0.75, cg*0.75, cb*0.75);
						print(120, 140 + (i * 65) -10 + game.menuyoff, editor.ListOfMetaData[int(game.menuoptions[i])].title, 255, 255, 255);
						print(120, 140 + (i * 65) -10 + game.menuyoff + 10, editor.ListOfMetaData[int(game.menuoptions[i])].creator, 196, 196, 196);
						
						print(120, 140 + (i * 65) -10 + game.menuyoff + 35+3, "spikyness: ", 196, 196, 196);
						for (var tiii:int = 0; tiii < editor.playerlevel_spikyness[int(game.menuoptions[i])]; tiii++){
							drawtile(120 + (8 * 11)+(8*tiii), 140 + (i * 65) -10 + game.menuyoff + 35+3, 8);
						}
						
						if (int(game.menuoptions_alt[i]) == 3) {
							drawsprite(280-20, 140 + (i * 65) -10 + game.menuyoff + 30-3, 188, 255, 255, 255);
							drawsprite(280, 140 + (i * 65) -10 + game.menuyoff + 30, 184, 255, 255, 255);
						}else	if (int(game.menuoptions_alt[i]) == 1) {
							drawsprite(280, 140 + (i * 65) -10 + game.menuyoff + 30, 184, 255, 255, 255);
						}
						
						drawplayerlevelimage(int(game.menuoptions[i]), 160 - 140 + 4+2, 140 + (i * 65) -10 + game.menuyoff - 6 + 4+2);
					}else if (game.menuoptionslayout[i] == 12) {
						//more games button on the main menu - don't draw anything, just have a detection zone
					}
				}
			}else {
				for (i = 0; i < game.nummenuoptions; i++) {
					if (i == game.currentmenuoption) {
						//Draw it highlighted
						if (game.menuoptionsactive[i]) {
							tempstring = game.menuoptions[i];	tempstring = "[ " + tempstring.toUpperCase() + " ]";
							print(110 + (i * division) - 16 +game.menuxoff, 140 + (i * 12) +game.menuyoff, tempstring, cr, cg, cb);
						}else {
							tempstring = game.menuoptions[i];	tempstring = "[ " + tempstring + " ]";
							//Draw it in gray
							print(110 + (i * division) - 16 +game.menuxoff, 140 + (i * 12)+game.menuyoff, tempstring, 128, 128, 128);
						}
					}else {
						//Draw it normally
						if (game.menuoptionsactive[i]) {
							print(110 + (i * division) +game.menuxoff, 140 + (i * 12)+game.menuyoff, game.menuoptions[i], cr, cg, cb);
						}else {
							//Draw it in gray
							print(110 + (i * division) +game.menuxoff, 140 + (i * 12)+game.menuyoff, game.menuoptions[i], 128, 128, 128);
						}
					}
				}
			}
		}
		
		public function drawlevelmenu(game:gameclass, cr:int, cg:int, cb:int, division:int = 30):void {
			for (var i:int = 0; i < game.nummenuoptions; i++){
				if (i == game.currentmenuoption){
					if(game.nummenuoptions-i<=2){
						//Draw it highlighted
						if (game.menuoptionsactive[i]) {
							tempstring = game.menuoptions[i];	tempstring = "[ " + tempstring.toUpperCase() + " ]";
					    print(110 + (i * division) - 16 +game.menuxoff, 140 + 8 + (i * 12) +game.menuyoff, tempstring, cr, cg, cb);
						}else{
							tempstring = game.menuoptions[i];
							tempstring = "[ " + tempstring + " ]";
							//Draw it in gray
							print(110 + (i * division) - 16 +game.menuxoff, 140 + 8 + (i * 12) + game.menuyoff, tempstring, 128, 128, 128);
						}
					}else{
						//Draw it highlighted
						if (game.menuoptionsactive[i]){
							tempstring = game.menuoptions[i];	tempstring = "[ " + tempstring.toUpperCase() + " ]";
					    print(110 + (i * division) - 16 +game.menuxoff, 140 + (i * 12) +game.menuyoff, tempstring, cr, cg, cb);
						}else{
							tempstring = game.menuoptions[i];
							tempstring = "[ " + tempstring + " ]";
							//Draw it in gray
							print(110 + (i * division) - 16 +game.menuxoff, 140 + (i * 12)+game.menuyoff, tempstring, 128, 128, 128);
						}
					}
				}else{
					if(game.nummenuoptions-i<=2){
						//Draw it normally
						if (game.menuoptionsactive[i]){
							print(110 + (i * division) +game.menuxoff, 140+8 + (i * 12)+game.menuyoff, game.menuoptions[i], cr, cg, cb);
						}else{
							//Draw it in gray
							print(110 + (i * division) +game.menuxoff, 140+8 + (i * 12)+game.menuyoff, game.menuoptions[i], 128, 128, 128);
						}
					}else{
						//Draw it normally
						if (game.menuoptionsactive[i]){
							print(110 + (i * division) +game.menuxoff, 140 + (i * 12)+game.menuyoff, game.menuoptions[i], cr, cg, cb);
						}else{
							//Draw it in gray
							print(110 + (i * division) +game.menuxoff, 140 + (i * 12)+game.menuyoff, game.menuoptions[i], 128, 128, 128);
						}
					}
				}
			}
		}
		
		//Fade functions
		public function processfade():void {
			if (fademode > 1) {
				if (fademode == 2) {
					//prepare fade out
					for (i = 0; i < 15; i++) {
						fadebars[i] = -int(Math.random() * 12) * 8;
					}
					fadeamount = 0;
					fademode = 3;
				}else if (fademode == 3) {
					fadeamount += 24;
					if (fadeamount > 416) {
						fademode = 1; //faded
					}
				}else if (fademode == 4) {
					//prepare fade in
					for (i = 0; i < 15; i++) {
						fadebars[i] = 320 + int(Math.random() * 12) * 8;
					}
					fadeamount = 416;
					fademode = 5;
				}else if (fademode == 5) {
					fadeamount -= 24;
					if (fadeamount <= 0) {
						fademode = 0; //normal
					}
				}
			}
		}
		
		public function drawfade():void {
			if (fademode == 1) {
				backbuffer.fillRect(backbuffer.rect, 0x000000);
			}else if(fademode==3){
			  for (i = 0; i < 15; i++) {
					setmadrect(fadebars[i], i * 16, fadeamount, 16);
				  backbuffer.fillRect(madrect, 0x000000);
				}
			}else if(fademode==5){
			  for (i = 0; i < 15; i++) {
					setmadrect(fadebars[i]-fadeamount, i * 16, 500, 16);
				  backbuffer.fillRect(madrect, 0x000000);
				}
			}
		}
		
		public function setwarprect(a:int, b:int, c:int, d:int):void {
			warprect.x = a; warprect.y = b;
			warprect.width = c; warprect.height = d;
		}
		
		public function createtextbox(t:String, xp:int, yp:int, r:int = 255, g:int = 255, b:int = 255):void {
			if(ntextbox == 0) {
				//If there are no active textboxes, Z=0;
				m = 0; ntextbox++;
			}else {
				/*i = 0; m = -1;
				while (i < ntextbox) {
					if (!textbox[i].active) {	m = i; i = ntextbox;}
					i++;
				}
				if (m == -1) {m = ntextbox; ntextbox++;}
				*/
				m = ntextbox; ntextbox++;
			}
			
			if(m<20){
				textbox[m].clear();
				textbox[m].line[0] = t;
				textbox[m].xp = xp;
				if (xp == -1) textbox[m].xp = 160 - (((t.length / 2) + 1) * 8);
				textbox[m].yp = yp;
				textbox[m].initcol(r, g, b);
				textbox[m].resize();
			}
		}
		
		public function textboxcleanup():void {
			i = ntextbox - 1; while (i >= 0 && !textbox[i].active) { ntextbox--; i--; }
		}
		
		public function textboxcenter():void {
			textbox[m].centerx(); textbox[m].centery();
		}
		
		public function textboxcenterx():void {
			textbox[m].centerx(); 
		}
		
		public function textboxwidth():int {
			return textbox[m].w; 
		}
		
		public function textboxmove(xo:int, yo:int):void {
			textbox[m].xp += xo; textbox[m].yp += yo;
		}
				
		public function textboxmoveto(xo:int):void {
			textbox[m].xp = xo;
		}
		
		public function textboxcentery():void {
		  textbox[m].centery();
		}
		
		public function textboxadjust():void {
		  textbox[m].adjust();
		}
		
		public function addline(t:String):void {
		  textbox[m].addline(t);
		}
		
		public function textboxtimer(t:int):void {
		  textbox[m].timer=t;
		}
		
		public function textboxremove():void {
			//Remove all textboxes
			for (i = 0; i < ntextbox; i++) {
				textbox[i].remove();
			}
		}
		
		public function textboxremovefast():void {
			//Remove all textboxes
			for (i = 0; i < ntextbox; i++) {
				textbox[i].removefast();
			}
		}
		
		public function textboxactive():void {
			//Remove all but the most recent textbox
			for (i = 0; i < ntextbox; i++) {
				if (m != i) textbox[i].remove();
			}
		}
		
		public function setmadrect(x:int, y:int, w:int, h:int):void {
			madrect.x = x; madrect.y = y; madrect.width = w; madrect.height = h;
		}
		
		public function drawtextbox(x:int, y:int, w:int, h:int, r:int, g:int, b:int):void {
			//given these parameters, draw a textbox
			madrect.x = x; madrect.y = y; madrect.width = w*8; madrect.height = h*8;
			backbuffer.fillRect(madrect, RGB(r / 6, g / 6, b / 6));
			drawcoloredtile(x, y, 40, r, g, b);
			drawcoloredtile(x + (w*8) - 8, y, 42, r, g, b);
			drawcoloredtile(x, y + (h*8) - 8, 45, r, g, b);
			drawcoloredtile(x + (w*8) - 8, y + (h*8) - 8, 47, r, g, b);
			
			for (k = 0; k < w-2; k++) {
				drawcoloredtile(x + 8 + (k * 8), y, 41, r, g, b);
				drawcoloredtile(x + 8 + (k * 8), y + (h * 8) - 8, 46, r, g, b);
			}
		
			for (k = 0; k < h-2; k++) {
				drawcoloredtile(x, y + 8 + (k * 8), 43, r, g, b);
				drawcoloredtile(x + (w * 8) - 8, y + 8 + (k * 8), 44, r, g, b);
			}
		}
		
		public function drawpixeltextbox(x:int, y:int, w:int, h:int, w2:int, h2:int, r:int, g:int, b:int, xo:int=0, yo:int=0):void {
			//given these parameters, draw a textbox with a pixel width
			madrect.x = x; madrect.y = y; madrect.width = w; madrect.height = h;
			backbuffer.fillRect(madrect, RGB(r / 6, g / 6, b / 6));
			
			for (k = 0; k < w2-2; k++) {
				drawcoloredtile(x + 8-xo + (k * 8), y, 41, r, g, b);
				drawcoloredtile(x + 8-xo + (k * 8), y + (h) - 8, 46, r, g, b);
			}
		
			for (k = 0; k < h2-2; k++) {
				drawcoloredtile(x, y + 8-yo + (k * 8), 43, r, g, b);
				drawcoloredtile(x + (w) - 8, y + 8-yo + (k * 8), 44, r, g, b);
			}
			
			drawcoloredtile(x, y, 40, r, g, b);
			drawcoloredtile(x + (w) - 8, y, 42, r, g, b);
			drawcoloredtile(x, y + (h) - 8, 45, r, g, b);
			drawcoloredtile(x + (w) - 8, y + (h) - 8, 47, r, g, b);
		}
		
		public function drawcustompixeltextbox(x:int, y:int, w:int, h:int, w2:int, h2:int, r:int, g:int, b:int, xo:int=0, yo:int=0):void {
			//given these parameters, draw a textbox with a pixel width
			
			//madrect.x = x; madrect.y = y; madrect.w = w; madrect.h = h;
			//backbuffer.fillRect(madrect, RGB(r / 6, g / 6, b / 6));
			madrect.x = x; madrect.y = y; madrect.width = w; madrect.height = h;
			backbuffer.fillRect(madrect, RGB(r / 6, g / 6, b / 6));
			
			for (k = 0; k < w2-2; k++){
				drawcoloredtile(x + 8-xo + (k * 8), y, 41, r, g, b);
				drawcoloredtile(x + 8-xo + (k * 8), y + (h) - 8, 46, r, g, b);
			}
			
			drawcoloredtile(x + (w) - 16, y, 41, r, g, b);
			drawcoloredtile(x + (w) - 16, y + (h) - 8, 46, r, g, b);
			drawcoloredtile(x + (w) - 24, y, 41, r, g, b);
			drawcoloredtile(x + (w) - 24, y + (h) - 8, 46, r, g, b);
			
			for (k = 0; k < h2-2; k++){
				drawcoloredtile(x, y + 8-yo + (k * 8), 43, r, g, b);
				drawcoloredtile(x + (w) - 8, y + 8-yo + (k * 8), 44, r, g, b);
			}
			
			drawcoloredtile(x, y + (h) - 16, 43, r, g, b);
			drawcoloredtile(x + (w) - 8, y + (h) - 16, 44, r, g, b);
			drawcoloredtile(x, y + (h) - 24, 43, r, g, b);
			drawcoloredtile(x + (w) - 8, y + (h) - 24, 44, r, g, b);
			
			drawcoloredtile(x, y, 40, r, g, b);
			drawcoloredtile(x + (w) - 8, y, 42, r, g, b);
			drawcoloredtile(x, y + (h) - 8, 45, r, g, b);
			drawcoloredtile(x + (w) - 8, y + (h) - 8, 47, r, g, b);
		}
		
		public function drawpartimage(t:int, xp:int, yp:int, wp:int, hp:int):void {
			tpoint.x = xp; tpoint.y = yp;
			madrect.x = 0; madrect.y = 0; madrect.width = wp; madrect.height = hp;
			
			backbuffer.copyPixels(images[t], madrect, tpoint);
		}
		
		public function cutscenebars():void {
			if (showcutscenebars) {
				cutscenebarspos += 25;
				if (cutscenebarspos >= 360) cutscenebarspos = 360;
				setmadrect(0, 0, cutscenebarspos, 16);
				backbuffer.fillRect(madrect, 0x000000);
				setmadrect(360-cutscenebarspos, 224, cutscenebarspos, 16);
				backbuffer.fillRect(madrect, 0x000000);
			}else {
				//disappearing
				if (cutscenebarspos > 0) {
					cutscenebarspos -= 25;
					//draw
					setmadrect(0, 0, cutscenebarspos, 16);
					backbuffer.fillRect(madrect, 0x000000);
					setmadrect(360-cutscenebarspos, 224, cutscenebarspos, 16);
				  backbuffer.fillRect(madrect, 0x000000);
				}
			}
		}
		
		public function drawgui(help:helpclass):void {
			textboxcleanup();
			//Draw all the textboxes to the screen
			for (i = 0; i<ntextbox; i++) {
				//This routine also updates the textboxs
				textbox[i].update();
				if (textbox[i].active) {
					if (textbox[i].r == 0 && textbox[i].g == 0 && textbox[i].b == 0) {
						if(flipmode){
							for (j = 0; j < textbox[i].numlines; j++) {
								print(textbox[i].xp + 8, textbox[i].yp + (textbox[i].numlines*8) - (j * 8), textbox[i].line[j], 196, 196, 255 - help.glow);
							} 
						}else {
							for (j = 0; j < textbox[i].numlines; j++) {
								print(textbox[i].xp + 8, textbox[i].yp + 8 + (j * 8), textbox[i].line[j], 196, 196, 255 - help.glow);
							}
						}
					}else{
						backbuffer.fillRect(textbox[i].textrect, RGB(textbox[i].r/6, textbox[i].g/6, textbox[i].b / 6));
						
						drawcoloredtile(textbox[i].xp, textbox[i].yp, 40, textbox[i].r, textbox[i].g, textbox[i].b);
						drawcoloredtile(textbox[i].xp+textbox[i].w-8, textbox[i].yp, 42, textbox[i].r, textbox[i].g, textbox[i].b);
						drawcoloredtile(textbox[i].xp, textbox[i].yp+textbox[i].h-8, 45, textbox[i].r, textbox[i].g, textbox[i].b);
						drawcoloredtile(textbox[i].xp+textbox[i].w-8, textbox[i].yp+textbox[i].h-8, 47, textbox[i].r, textbox[i].g, textbox[i].b);
						for (k = 0; k < textbox[i].lw; k++) {
							drawcoloredtile(textbox[i].xp + 8 + (k * 8), textbox[i].yp, 41, textbox[i].r, textbox[i].g, textbox[i].b);
							drawcoloredtile(textbox[i].xp + 8 + (k * 8), textbox[i].yp+textbox[i].h-8, 46, textbox[i].r, textbox[i].g, textbox[i].b);
						}
						for (k = 0; k < textbox[i].numlines; k++) {
							drawcoloredtile(textbox[i].xp, textbox[i].yp + 8 + (k * 8), 43, textbox[i].r, textbox[i].g, textbox[i].b);
							drawcoloredtile(textbox[i].xp + textbox[i].w-8, textbox[i].yp + 8 + (k * 8), 44, textbox[i].r, textbox[i].g, textbox[i].b);
						}
						
						if(flipmode){
							for (j = 0; j < textbox[i].numlines; j++) {
								print(textbox[i].xp + 8, textbox[i].yp  + (textbox[i].numlines*8) - (j * 8), textbox[i].line[j], textbox[i].r, textbox[i].g, textbox[i].b);
							} 
						}else {
							for (j = 0; j < textbox[i].numlines; j++) {
								print(textbox[i].xp + 8, textbox[i].yp +8 + (j * 8), textbox[i].line[j], textbox[i].r, textbox[i].g, textbox[i].b);
							}
						}
					}
					
					if ((textbox[i].yp == 12 || textbox[i].yp == 180) && textbox[i].r == 165) {
						if (flipmode) {	drawimage(5, 0, 180, true);
						}else{ drawimage(0, 0, 12, true);	}
					}else if ((textbox[i].yp == 12 || textbox[i].yp == 180) && textbox[i].g == 165) {
						if (flipmode) {	drawimage(6, 0, 180, true);
						}else{ drawimage(4, 0, 12, true);	}
					}
					if (flipmode) {
						if (textbox[i].r == 175 && textbox[i].g == 175) {
							//purple guy
							drawsprite(80 - 6, 64 + 48 + 4, 6, 220- help.glow/4 - (Math.random()*20), 120- help.glow/4, 210 - help.glow/4);
						}else if (textbox[i].r == 175 && textbox[i].b == 175) {
							//red guy
							drawsprite(80 - 6, 64 + 48+ 4, 6, 255 - help.glow/8, 70 - help.glow/4, 70 - help.glow / 4);
						}else if (textbox[i].r == 175) {
							//green guy
							drawsprite(80 - 6, 64 + 48 + 4, 6, 120 - help.glow / 4 - (Math.random() * 20), 220 - help.glow / 4, 120 - help.glow / 4);
						}else if (textbox[i].g == 175) {
							//yellow guy
							drawsprite(80 - 6, 64 + 48+ 4, 6, 220- help.glow/4 - (Math.random()*20), 210 - help.glow/4, 120- help.glow/4);
						}else if (textbox[i].b == 175) {
							//blue guy
							drawsprite(80 - 6, 64 + 48+ 4, 6, 75, 75, 255- help.glow/4 - (Math.random()*20));
						}
					}else{
						if (textbox[i].r == 175 && textbox[i].g == 175) {
							//purple guy
							drawsprite(80 - 6, 64 + 32 + 4, 0, 220- help.glow/4 - (Math.random()*20), 120- help.glow/4, 210 - help.glow/4);
						}else if (textbox[i].r == 175 && textbox[i].b == 175) {
							//red guy
							drawsprite(80 - 6, 64 + 32 + 4, 0, 255 - help.glow/8, 70 - help.glow/4, 70 - help.glow / 4);
						}else if (textbox[i].r == 175) {
							//green guy
							drawsprite(80 - 6, 64 + 32 + 4, 0, 120 - help.glow / 4 - (Math.random() * 20), 220 - help.glow / 4, 120 - help.glow / 4);
						}else if (textbox[i].g == 175) {
							//yellow guy
							drawsprite(80 - 6, 64 + 32 + 4, 0, 220- help.glow/4 - (Math.random()*20), 210 - help.glow/4, 120- help.glow/4);
						}else if (textbox[i].b == 175) {
							//blue guy
							drawsprite(80 - 6, 64 + 32 + 4, 0, 75, 75, 255- help.glow/4 - (Math.random()*20));
						}
					}
				}
			}
		}
		
		public function RGBA(red:Number,green:Number,blue:Number):Number{
			return (blue | (green << 8) | (red << 16)) + 0xFF000000;
		}
		
		public function RGB(red:Number,green:Number,blue:Number):Number{
			return (blue | (green << 8) | (red << 16))
		}
		
	  public function RGBf(red:Number, green:Number, blue:Number):Number {
			red = (red+128) / 3; green = (green+128) / 3; blue = (blue+128) / 3;
			return (blue | (green << 8) | (red << 16))
		}
		
		public function addmobileimage():void {
			var t:BitmapData = new BitmapData(buffer.width, buffer.height, true, 0x000000);
			setmadrect(0, 0, buffer.width, buffer.height);
			t.copyPixels(buffer, madrect, tl);
			mobileimages.push(t);
		}
		
		public function addimage():void {
			var t:BitmapData = new BitmapData(buffer.width, buffer.height, true, 0x000000);
			setmadrect(0, 0, buffer.width, buffer.height);
			t.copyPixels(buffer, madrect, tl);
			images.push(t);
		}
		
		public function addplayerlevelimage():void {
			var t:BitmapData = new BitmapData(buffer.width, buffer.height, true, 0x000000);
			setmadrect(0, 0, buffer.width, buffer.height);
			t.copyPixels(buffer, madrect, tl);
			playerlevelimages.push(t);
		}
		
		public function addbackground():void {
			var t:BitmapData = new BitmapData(160, 144, true, 0x000000);
			t.copyPixels(buffer, bg_rect, tl);
			backgrounds.push(t);
		}
		
		public function drawtowerbackground(map:mapclass):void {
			if (map.bypos < 0) map.bypos += 120 * 8;
			
			if (map.scrolldir == 1) map.tdrawback = true;
			
			if (map.tdrawback) {
				//Draw the whole thing; needed for every colour cycle!
				for (j = 0; j < 30; j++) {
					for (i = 0; i < 40; i++) {			
						temp = map.tower.backat(i, j, map.bypos);
						drawtowertile3(i * 8, (j * 8) - (map.bypos % 8), temp, map.colstate);
					}
				}
				
				backbuffer.copyPixels(towerbuffer, towerbuffer.rect, tl, null, null, false);
				
				map.tdrawback = false;
			}else {
				//just update the bottom
				towerbuffer.scroll(0, -map.bscroll);
				for (i = 0; i < 40; i++) {			
					temp = map.tower.backat(i, 0, map.bypos);
					drawtowertile3(i * 8, -(map.bypos % 8), temp, map.colstate);
				}
				
				backbuffer.copyPixels(towerbuffer, towerbuffer.rect, tl, null, null, false);
			}
		}
		
		public function drawtowerbackgroundsolo(map:mapclass):void {
			if (map.bypos < 0) map.bypos += 120 * 8;
			
			if (map.tdrawback) {
				//Draw the whole thing; needed for every colour cycle!
				for (j = 0; j < 31; j++) {
					for (i = 0; i < 40; i++) {			
						temp = map.tower.backat(i, j, map.bypos);
						drawtowertile3(i * 8, (j * 8) - (map.bypos % 8), temp, map.colstate);
					}
				}
				
				backbuffer.copyPixels(towerbuffer, towerbuffer.rect, tl, null, null, false);
				
				map.tdrawback = false;
			}else {
				//just update the bottom
				towerbuffer.scroll(0, -map.bscroll);
				for (i = 0; i < 40; i++) {			
					temp = map.tower.backat(i, 0, map.bypos);
					drawtowertile3(i * 8, -(map.bypos % 8), temp, map.colstate);
				}
				
				backbuffer.copyPixels(towerbuffer, towerbuffer.rect, tl, null, null, false);
			}
		}
		
		public function drawtowermap(map:mapclass):void {
			for (j = 0; j < 30; j++) {
				for (i = 0; i < 40; i++) {			
					temp = map.tower.at(i, j, map.ypos);
					if (temp > 0) drawtile3(i * 8, (j * 8) - (map.ypos % 8), temp, map.colstate);
				}
			}
		}
		
		public function drawtowermap_nobackground(map:mapclass):void {
			for (j = 0; j < 30; j++) {
				for (i = 0; i < 40; i++) {			
					temp = map.tower.at(i, j, map.ypos);
					if (temp > 0 && temp<28) drawtile3(i * 8, (j * 8) - (map.ypos % 8), temp, map.colstate);
				}
			}
		}
		
		public function drawtowerspikes(map:mapclass):void {
			for (i = 0; i < 40; i++) {			
				drawtile3(i * 8, -8+map.spikeleveltop, 9, map.colstate);
				drawtile3(i * 8, 230-map.spikelevelbottom, 8, map.colstate);
			}
		}
		
		public function drawtowerentities(map:mapclass, obj:entityclass, help:helpclass):void {
			//Update line colours!
			if (linedelay <= 0) {
				linestate++;
				if (linestate >= 10) linestate = 0;
				linedelay = 2;
			}else {
				linedelay--;
			}
			
			var j:int, k:int;
			for (var i:int = 0; i < obj.nentity; i++) {
				if (!obj.entities[i].invis && obj.entities[i].active) {
					if (obj.entities[i].size == 0) {      // Sprites
						tpoint.x = obj.entities[i].xp; tpoint.y = obj.entities[i].yp-map.ypos;
						setcol(obj.entities[i].colour, help); 
						sprites[obj.entities[i].drawframe].colorTransform(sprites_rect, ct);
						backbuffer.copyPixels(sprites[obj.entities[i].drawframe], sprites_rect, tpoint);
						//screenwrapping!
						if (!map.minitowermode) {
							if ( map.ypos >= 500 && map.ypos <= 5000) { //The "wrapping" area of the tower
								if (tpoint.x < 0) {
									tpoint.x += 320;
									backbuffer.copyPixels(sprites[obj.entities[i].drawframe], sprites_rect, tpoint);
								}
								if (tpoint.x > 300) {
									tpoint.x -= 320;
									backbuffer.copyPixels(sprites[obj.entities[i].drawframe], sprites_rect, tpoint);
								}
							}
						}
					}else if (obj.entities[i].size == 1) { // Tiles
					  tpoint.x = obj.entities[i].xp; tpoint.y = obj.entities[i].yp-map.ypos;
					  backbuffer.copyPixels(tiles[obj.entities[i].drawframe], tiles_rect, tpoint);
					}else if (obj.entities[i].size == 2) { // Special: Moving platform, 4 tiles
					  tpoint.x = obj.entities[i].xp; tpoint.y = obj.entities[i].yp-map.ypos;
					  backbuffer.copyPixels(tiles[obj.entities[i].drawframe], tiles_rect, tpoint);
						tpoint.x += 8; backbuffer.copyPixels(tiles[obj.entities[i].drawframe], tiles_rect, tpoint);
						tpoint.x += 8; backbuffer.copyPixels(tiles[obj.entities[i].drawframe], tiles_rect, tpoint);
						tpoint.x += 8; backbuffer.copyPixels(tiles[obj.entities[i].drawframe], tiles_rect, tpoint);
					}else if (obj.entities[i].size == 3) { // Big chunky pixels!
					  prect.x = obj.entities[i].xp; prect.y = obj.entities[i].yp-map.ypos;
						//A seperate index of colours, for simplicity
						if(obj.entities[i].colour==1){
						  backbuffer.fillRect(prect, RGB(196 - (Math.random() * 64), 10, 10));
						}else if (obj.entities[i].colour == 2) {
							backbuffer.fillRect(prect, RGB(160- help.glow/2 - (Math.random()*20), 200- help.glow/2, 220 - help.glow));
						}
					}else if (obj.entities[i].size == 4) { // Small pickups
					  drawhuetile(obj.entities[i].xp, obj.entities[i].yp-map.ypos, obj.entities[i].tile, obj.entities[i].colour);
					}else if (obj.entities[i].size == 5) { //Horizontal Line
						line_rect.x = obj.entities[i].xp; line_rect.y = obj.entities[i].yp-map.ypos;
						line_rect.width = obj.entities[i].w; line_rect.height = 1;
						drawgravityline(i, obj);
					}else if (obj.entities[i].size == 6) { //Vertical Line
						line_rect.x = obj.entities[i].xp; line_rect.y = obj.entities[i].yp-map.ypos;
						line_rect.width = 1; line_rect.height = obj.entities[i].h;
						drawgravityline(i, obj);
					}
				}
			}
		}
		
		public function drawbackground(t:int, map:mapclass):void {
			switch(t) {
				case 1:
				  //Starfield
				  backbuffer.fillRect(backbuffer.rect, 0x000000);
					for (i = 0; i < 50; i++) {
						if (starsspeed[i] <= 6) {
						  backbuffer.fillRect(stars[i], 0x222222);	
						}else {
							backbuffer.fillRect(stars[i], 0x555555);
						}
						stars[i].x -= starsspeed[i];
						if (stars[i].x < -10) {
							stars[i].x += 340;
							stars[i].y = Math.random() * 240;
							starsspeed[i] = 4+(Math.random()*4);
						}
					}
				break;
				case 2:
				  //Lab
					switch(rcol) {
						//Akward ordering to match tileset
						case 0: bcol2 = RGB(0, 16*backboxint[i], 16*backboxint[i]);	break; //Cyan
						case 1: bcol2 = RGB(16*backboxint[i], 0, 0);	break;  //Red
						case 2: bcol2 = RGB(16*backboxint[i], 0, 16*backboxint[i]);	break; //Purple
						case 3: bcol2 = RGB(0, 0, 16*backboxint[i]);	break;  //Blue
						case 4: bcol2 = RGB(16*backboxint[i], 16*backboxint[i], 0);	break; //Yellow
						case 5: bcol2 = RGB(0, 16 * backboxint[i], 0);	break;  //Green
						case 6: 
						  //crazy case
							spcoldel--;
							if (spcoldel <= 0) {
								spcoldel = 15;
								spcol++;
								if (spcol >= 12) spcol = 0;
							}
							switch(spcol) {
								case 0: bcol2 = RGB(0, 16*backboxint[i], 16*backboxint[i]);	break; //Cyan
								case 1: bcol2 = RGB(0, (spcoldel+1)*backboxint[i], 16*backboxint[i]);	break; //Cyan
								case 2: bcol2 = RGB(0, 0, 16*backboxint[i]);	break;  //Blue
								case 3: bcol2 = RGB((16-spcoldel)*backboxint[i], 0, 16*backboxint[i]);	break;  //Blue
								case 4: bcol2 = RGB(16*backboxint[i], 0, 16*backboxint[i]);	break; //Purple
								case 5: bcol2 = RGB(16*backboxint[i], 0, (spcoldel+1)*backboxint[i]);	break; //Purple
								case 6: bcol2 = RGB(16*backboxint[i], 0, 0);	break;  //Red
								case 7: bcol2 = RGB(16*backboxint[i], (16-spcoldel)*backboxint[i], 0);	break;  //Red
								case 8: bcol2 = RGB(16*backboxint[i], 16*backboxint[i], 0);	break; //Yellow
								case 9: bcol2 = RGB((spcoldel+1)*backboxint[i], 16*backboxint[i], 0);	break; //Yellow
								case 10: bcol2 = RGB(0, 16 * backboxint[i], 0);	break;  //Green
								case 11: bcol2 = RGB(0, 16 * backboxint[i], (16-spcoldel)*backboxint[i]);	break;  //Green
							}
						break;
					}
				  backbuffer.fillRect(backbuffer.rect, bcol2);
					for (i = 0; i < 18; i++) {
						switch(rcol) {
							//Akward ordering to match tileset
							case 0: bcol = RGB(16, 128*backboxint[i], 128*backboxint[i]);	break; //Cyan
							case 1: bcol = RGB(128*backboxint[i], 16, 16);	break;  //Red
							case 2: bcol = RGB(128*backboxint[i], 16, 128*backboxint[i]);	break; //Purple
							case 3: bcol = RGB(16, 16, 128*backboxint[i]);	break;  //Blue
							case 4: bcol = RGB(128*backboxint[i], 128*backboxint[i], 16);	break; //Yellow
							case 5: bcol = RGB(16, 128 * backboxint[i], 16);	break;  //Green
							case 6: 
						  //crazy case
							switch(spcol) {
								case 0: bcol = RGB(16, 128*backboxint[i], 128*backboxint[i]);	break; //Cyan
								case 1: bcol = RGB(16, ((spcoldel+1)*8)*backboxint[i], 128*backboxint[i]);	break; //Cyan
								case 2: bcol = RGB(16, 16, 128*backboxint[i]);	break;  //Blue
								case 3: bcol = RGB((128-(spcoldel*8))*backboxint[i], 16, 128*backboxint[i]);	break;  //Blue
								case 4: bcol = RGB(128*backboxint[i], 16, 128*backboxint[i]);	break; //Purple
								case 5: bcol = RGB(128*backboxint[i], 16, ((spcoldel+1)*8)*backboxint[i]);	break; //Purple
								case 6: bcol = RGB(128*backboxint[i], 16, 16);	break;  //Red
								case 7: bcol = RGB(128*backboxint[i], (128-(spcoldel*8))*backboxint[i], 16);	break;  //Red
								case 8: bcol = RGB(128*backboxint[i], 128*backboxint[i], 16);	break; //Yellow
								case 9: bcol = RGB(((spcoldel+1)*8)*backboxint[i], 128*backboxint[i], 16);	break; //Yellow
								case 10: bcol = RGB(16, 128 * backboxint[i], 16);	break;  //Green
								case 11: bcol = RGB(16, 128 * backboxint[i], (128-(spcoldel*8))*backboxint[i]);	break;  //Green
							}
						break;
						}
						backbuffer.fillRect(backboxes[i], bcol);	
						
						backboxrect.x = backboxes[i].x + 1; backboxrect.y = backboxes[i].y + 1;
						backboxrect.width = backboxes[i].width - 2; backboxrect.height = backboxes[i].height - 2;
						backbuffer.fillRect(backboxrect, bcol2);
						
						backboxes[i].x += backboxvx[i];
						backboxes[i].y += backboxvy[i];
						if (backboxes[i].x < -40) { backboxes[i].x = 320; backboxes[i].y = Math.random() * 240; }
					  if (backboxes[i].x > 320) { backboxes[i].x = -32; backboxes[i].y = Math.random() * 240; }
						if (backboxes[i].y < -40) { backboxes[i].y = 240; backboxes[i].x = Math.random() * 320; }
						if (backboxes[i].y > 260) { backboxes[i].y = -32; backboxes[i].x = Math.random() * 320; }
					}
				break;
				case 3: //Warp zone (horizontal)
				  backoffset+=3; if (backoffset >= 16) backoffset -= 16;
					
				  if (backgrounddrawn) {
						towerbuffer.scroll( -3, 0);
						for (j = 0; j < 15; j++) {
							temp = 680 + (rcol * 3); 
							drawtowertile(317 - backoffset, (j * 16), temp+40);  //20*16 = 320
							drawtowertile(317 - backoffset + 8, (j * 16), temp + 41);
							drawtowertile(317 - backoffset, (j * 16) + 8, temp + 80);
							drawtowertile(317 - backoffset + 8, (j * 16) + 8, temp + 81);
						}
					}else {
						//draw the whole thing for the first time!
						backoffset = 0;
						towerbuffer.fillRect(towerbuffer.rect, 0x000000);
						for (j = 0; j < 15; j++) {
							for (i = 0; i < 21; i++) {			
								temp = 680 + (rcol * 3);
								drawtowertile((i * 16) - backoffset, (j * 16), temp+40);
								drawtowertile((i * 16) - backoffset + 8, (j * 16), temp + 41);
								drawtowertile((i * 16) - backoffset, (j * 16) + 8, temp + 80);
								drawtowertile((i * 16) - backoffset + 8, (j * 16) + 8, temp + 81);
							}
						}
						backgrounddrawn = true;
					}
					
					backbuffer.copyPixels(towerbuffer, towerbuffer.rect, tl);
				break;
				case 4: //Warp zone (vertical)
				  backoffset+=3; if (backoffset >= 16) backoffset -= 16;
					
				  if (backgrounddrawn) {
						towerbuffer.scroll(0, -3);
						for (i = 0; i < 21; i++) {			
							temp = 760 + (rcol * 3);
							drawtowertile((i * 16), 237 - backoffset, temp + 40); //14*17=240 - 3
							drawtowertile((i * 16) + 8, 237 - backoffset, temp + 41);
							drawtowertile((i * 16), 237 - backoffset + 8, temp + 80);
							drawtowertile((i * 16) + 8, 237 - backoffset + 8, temp + 81);
						}
					}else {
						//draw the whole thing for the first time!
						backoffset = 0;
						towerbuffer.fillRect(towerbuffer.rect, 0x000000);
						for (j = 0; j < 15; j++) {
							for (i = 0; i < 21; i++) {			
								temp = 760 + (rcol * 3);
								drawtowertile((i * 16), (j * 16)- backoffset, temp+40);
								drawtowertile((i * 16)+ 8, (j * 16)- backoffset, temp + 41);
								drawtowertile((i * 16), (j * 16)- backoffset + 8, temp + 80);
								drawtowertile((i * 16)+ 8, (j * 16)- backoffset + 8, temp + 81);
							}
						}
						backgrounddrawn = true;
					}
					
					backbuffer.copyPixels(towerbuffer, towerbuffer.rect, tl);
				break;
				case 5:
				  //Warp zone, central
					switch(rcol) {
						//Akward ordering to match tileset
						case 0: warpbcol = 0x0A100E; warpfcol = 0x102221; break; //Cyan
						case 1: warpbcol = 0x11090B; warpfcol = 0x221011; break; //Red
						case 2: warpbcol = 0x0F0A10; warpfcol = 0x221022; break; //Purple
						case 3: warpbcol = 0x0A0B10; warpfcol = 0x101022; break; //Blue
						case 4: warpbcol = 0x100D0A; warpfcol = 0x221E10; break; //Yellow
						case 5: warpbcol = 0x0D100A; warpfcol = 0x142210; break; //Green
						case 6: warpbcol = 0x0A0A0A; warpfcol = 0x121212; break; //Gray
					}
					
				  backoffset += 1; 
					if (backoffset >= 16) {
						backoffset -= 16;
						warpskip = (warpskip + 1) % 2;
					}
					
					for (i = 10 ; i >= 0; i--) {
						temp = (i << 4) + backoffset;
						setwarprect(160 - temp, 120 - temp, temp * 2, temp * 2);
						if (i % 2 == warpskip) {
							backbuffer.fillRect(warprect, warpbcol);
						}else {
							backbuffer.fillRect(warprect, warpfcol);
						}
					}
				break;
				case 6:
				  //Final Starfield
				  backbuffer.fillRect(backbuffer.rect, 0x000000);
					for (i = 0; i < 50; i++) {
						if (starsspeed[i] <= 8) {
						  backbuffer.fillRect(stars[i], 0x222222);	
						}else {
							backbuffer.fillRect(stars[i], 0x555555);
						}
						stars[i].y -= starsspeed[i];
						if (stars[i].y < -10) {
							stars[i].y += 260;
							stars[i].x = Math.random() * 320;
							starsspeed[i] = 5+(Math.random()*5);
						}
					}
				break;
				case 7:
				  //Static, unscrolling section of the tower
					for (j = 0; j < 30; j++) { for (i = 0; i < 40; i++) {	drawtile3(i * 8, j * 8, map.tower.backat(i, j, 200), 15);	}	}
				break;
				case 8:
				  //Static, unscrolling section of the tower
					for (j = 0; j < 30; j++) { for (i = 0; i < 40; i++) {	drawtile3(i * 8, j * 8, map.tower.backat(i, j, 200), 10);	}	}
				break;
				case 9:
				  //Static, unscrolling section of the tower
					for (j = 0; j < 30; j++) { for (i = 0; i < 40; i++) {	drawtile3(i * 8, j * 8, map.tower.backat(i, j, 600), 0);	}	}
				break;
				default:
				  backbuffer.fillRect(backbuffer.rect, 0x000000);
					//backbuffer.copyPixels(backgrounds[t], bg_rect, tl);
				break;
			}
		}
		
		public function drawimage(t:int, xp:int, yp:int, cent:Boolean=false):void {
			if (cent) {
				tpoint.x = 160 - int(images[t].width / 2); tpoint.y = yp;
				trect.x = 0; trect.y = 0; trect.width = images[t].width; trect.height = images[t].height;
				backbuffer.copyPixels(images[t], trect, tpoint);
			}else {
				tpoint.x = xp; tpoint.y = yp;
				trect.x = 0; trect.y = 0; trect.width = images[t].width; trect.height = images[t].height;
			  backbuffer.copyPixels(images[t], trect, tpoint);
			}
		}
		
		public function drawmobileimage(t:int, xp:int, yp:int, cent:Boolean=false):void {
			if (cent) {
				tpoint.x = 160 - int(mobileimages[t].width / 2); tpoint.y = yp;
				trect.x = 0; trect.y = 0; trect.width = mobileimages[t].width; trect.height = mobileimages[t].height;
				backbuffer.copyPixels(mobileimages[t], trect, tpoint);
			}else {
				tpoint.x = xp; tpoint.y = yp;
				trect.x = 0; trect.y = 0; trect.width = mobileimages[t].width; trect.height = mobileimages[t].height;
			  backbuffer.copyPixels(mobileimages[t], trect, tpoint);
			}
		}
		
		public function drawmobilehands(t:int, xp:int, yp:int):void {
			tpoint.x = xp; tpoint.y = yp;
			trect.x = 0; trect.y = 0; trect.width = mobileimages[t].width; trect.height = 70;
			backbuffer.copyPixels(mobileimages[t], trect, tpoint);
		}
		
		public function drawplayerlevelimage(t:int, xp:int, yp:int):void {
			tpoint.x = xp; tpoint.y = yp;
			trect.x = 0; trect.y = 0; trect.width = playerlevelimages[t].width; trect.height = playerlevelimages[t].height;
			backbuffer.copyPixels(playerlevelimages[t], trect, tpoint);
		}
		
		public function drawimagecol(t:int, xp:int, yp:int, r:int, g:int, b:int, cent:Boolean = false):void {
			setcolreal(RGB(r,g,b)); 
			if (cent) {
				tpoint.x = 160 - int(images[t].width / 2); tpoint.y = yp;
				trect.x = 0; trect.y = 0; trect.width = images[t].width; trect.height = images[t].height;
				images[t].colorTransform(trect, ct);
				backbuffer.copyPixels(images[t], trect, tpoint);
			}else {
				tpoint.x = xp; tpoint.y = yp;
				trect.x = 0; trect.y = 0; trect.width = images[t].width; trect.height = images[t].height;
			  images[t].colorTransform(trect, ct);
				backbuffer.copyPixels(images[t], trect, tpoint);
			}
		}
		
		public function crewcolour(t:int):int {
			//given crewmate t, return colour in setcol
			if (t == 0) return 0;
			if (t == 1) return 20;
			if (t == 2) return 14;
			if (t == 3) return 15;
			if (t == 4) return 13;
			if (t == 5) return 16;
			return 0;
		}
		
		public function setcol(t:int, help:helpclass):void {
		  //Setup predefinied colours as per our zany palette
			switch(t) {
				//Player Normal
				case 0: ct.color = RGB(160- help.glow/2 - (Math.random()*20), 200- help.glow/2, 220 - help.glow); break;
				//Player Hurt
				case 1:	ct.color = RGB(196 - (Math.random() * 64), 10, 10); break;
				//Enemies and stuff
				case 2: ct.color = RGB(225-(help.glow/2), 75, 30); break;
				case 3: //Trinket
					if (!trinketcolset) {
						trinketr = 200 - (Math.random() * 64);
						trinketg = 200 - (Math.random() * 128);
						trinketb = 164 + (Math.random() * 60);
						trinketcolset = true;
					}
				  ct.color = RGB(trinketr, trinketg, trinketb);
				break;
				case 4: //Inactive savepoint
				  temp = (help.glow/2) + (Math.random() * 8);
				  ct.color = RGB(80 + temp, 80 + temp, 80 + temp);
				break;
				case 5: //Active savepoint
				  ct.color = RGB(164+(Math.random()*64),164+(Math.random()*64), 255-(Math.random()*64))
				break;
				case 6: //Enemy : Red
				  ct.color = RGB(250 - help.glow/2, 60- help.glow/2, 60 - help.glow/2); 
				break;
				case 7: //Enemy : Green
				  ct.color = RGB(100 - help.glow/2 - (Math.random()*30), 250 - help.glow/2, 100 - help.glow/2 - (Math.random()*30)); 
				break;
				case 8: //Enemy : Purple
				  ct.color = RGB(250 - help.glow/2, 20, 128 - help.glow/2 + (Math.random()*30)); 
				break;
				case 9: //Enemy : Yellow
				  ct.color = RGB(250 - help.glow/2, 250 - help.glow/2, 20); 
				break;
				case 10: //Warp point (white)
				  ct.color = RGB(255 - (Math.random() * 64), 255 - (Math.random() * 64), 255 - (Math.random() * 64));
				break;
				case 11: //Enemy : Cyan
				  ct.color = RGB(20, 250 - help.glow/2, 250 - help.glow/2); 
				break;
				case 12: //Enemy : Blue
				  ct.color = RGB(90- help.glow/2, 90 - help.glow/2, 250 - help.glow/2); 
				break;
				//Crew Members
				//green
				case 13: ct.color = RGB(120- help.glow/4 - (Math.random()*20), 220 - help.glow/4, 120- help.glow/4); break;
				//Yellow
				case 14: ct.color = RGB(220- help.glow/4 - (Math.random()*20), 210 - help.glow/4, 120- help.glow/4); break;
				//pink
				case 15: ct.color = RGB(255 - help.glow/8, 70 - help.glow/4, 70 - help.glow / 4); break;
				//Blue
				case 16: ct.color = RGB(75, 75, 255- help.glow/4 - (Math.random()*20)); break;
				
				
				case 17: //Enemy : Orange
				  ct.color = RGB(250 - help.glow/2, 130 - help.glow/2, 20); 
				break;
				case 18: //Enemy : Gray
				  ct.color = RGB(130- help.glow/2, 130 - help.glow/2, 130 - help.glow/2); 
				break;
				case 19: //Enemy : Dark gray
				  ct.color = RGB(60- help.glow/8, 60 - help.glow/8, 60 - help.glow/8); 
				break;
				//Purple
				case 20: ct.color = RGB(220 - help.glow / 4 - (Math.random() * 20), 120 - help.glow / 4, 210 - help.glow / 4); break;
				
				case 21: //Enemy : Light Gray
				  ct.color = RGB(180- help.glow/2, 180 - help.glow/2, 180 - help.glow/2); 
				break;				
				case 22: //Enemy : Indicator Gray
				  ct.color = RGB(230- help.glow/2, 230- help.glow/2, 230- help.glow/2); 
				break;			
				case 23: //Enemy : Indicator Gray
				  ct.color = RGB(255- help.glow/2 - (Math.random() * 40) , 255- help.glow/2 - (Math.random() * 40), 255- help.glow/2 - (Math.random() * 40)); 
				break;
				
				//Trophies
				//cyan
				case 30: ct.color = RGBf(160, 200, 220); break;
				//Purple
				case 31: ct.color = RGBf(220, 120, 210); break;
				//Yellow
				case 32: ct.color = RGBf(220, 210, 120); break;
				//red
				case 33: ct.color = RGBf(255, 70, 70); break;
				//green
				case 34: ct.color = RGBf(120, 220, 120); break;
				//Blue
				case 35: ct.color = RGBf(75, 75, 255); break;
				//Gold
				case 36:  ct.color = RGB(180, 120, 20); break;				
				case 37: //Trinket
					if (!trinketcolset) {
						trinketr = 200 - (Math.random() * 64);
						trinketg = 200 - (Math.random() * 128);
						trinketb = 164 + (Math.random() * 60);
						trinketcolset = true;
					}
				  ct.color = RGBf(trinketr, trinketg, trinketb);
				break;
				//Silver
				case 38:  ct.color = RGBf(196, 196, 196); break;		
				//Bronze
				case 39:  ct.color = RGBf(128, 64, 10); break;		
				//Awesome
				case 40: //Teleporter in action!
				  temp = Math.random() * 150;  
				  if(temp<33){
				    ct.color = RGBf(255 - (Math.random() * 64), 64 + (Math.random() * 64), 64 + (Math.random() * 64));
					}else if (temp < 66) {
						ct.color = RGBf(64 + (Math.random() * 64), 255 - (Math.random() * 64), 64 + (Math.random() * 64));
					}else if (temp < 100) {
						ct.color = RGBf(64 + (Math.random() * 64), 64 + (Math.random() * 64), 255 - (Math.random() * 64));
					}else {
						ct.color = RGBf(164+(Math.random()*64),164+(Math.random()*64), 255-(Math.random()*64))
					}
				break;
				
				case 100: //Inactive Teleporter
				  temp = (help.glow/2) + (Math.random() * 8);
				  ct.color = RGB(42 + temp, 42 + temp, 42 + temp);
				break;
				case 101: //Active Teleporter
				  ct.color = RGB(164+(Math.random()*64),164+(Math.random()*64), 255-(Math.random()*64))
				break;
				case 102: //Teleporter in action!
				  temp = Math.random() * 150;  
				  if(temp<33){
				    ct.color = RGB(255 - (Math.random() * 64), 64 + (Math.random() * 64), 64 + (Math.random() * 64));
					}else if (temp < 66) {
						ct.color = RGB(64 + (Math.random() * 64), 255 - (Math.random() * 64), 64 + (Math.random() * 64));
					}else if (temp < 100) {
						ct.color = RGB(64 + (Math.random() * 64), 64 + (Math.random() * 64), 255 - (Math.random() * 64));
					}else {
						ct.color = RGB(164+(Math.random()*64),164+(Math.random()*64), 255-(Math.random()*64))
					}
				break;
				
				default: ct.color = 0xFFFFFF;	break;
			}
		}
		
		public function setcolreal(t:int):void {
			ct.color = t;
		}

		public function drawcoloredtile(x:int, y:int, t:int, r:int, g:int, b:int):void {
			tpoint.x = x; tpoint.y = y;
			setcolreal(RGB(r,g,b)); 
			tiles[t].colorTransform(tiles_rect, ct);
			backbuffer.copyPixels(tiles[t], tiles_rect, tpoint);
		}
		
		public function drawhuetile(x:int, y:int, t:int, c:int):void {
			tpoint.x = x; tpoint.y = y;
			switch(c) {
				case 0:setcolreal(RGB(250-(Math.random()*32), 250-(Math.random()*32), 10)); break;
				case 1:setcolreal(RGB(250-(Math.random()*32), 250-(Math.random()*32), 10)); break;
				
				default:setcolreal(RGB(250-(Math.random()*32), 250-(Math.random()*32), 10)); break;
			}
			
			tiles[t].colorTransform(tiles_rect, ct);
			backbuffer.copyPixels(tiles[t], tiles_rect, tpoint);
		}
		
		public function drawcrewman(x:int, y:int, t:int, act:Boolean, help:helpclass, noshift:Boolean=false):void {
			if (!act) {
				if (noshift) {
					if (flipmode) { drawspritesetcol(x, y, 14, 19, help);				
					}else{ drawspritesetcol(x, y, 12, 19, help); }
				}else{
					if (flipmode) { drawspritesetcol(x - 8, y, 14, 19, help);			
				  }else{ drawspritesetcol(x - 8, y, 12, 19, help); } 
				}
			}else {
				if (flipmode) crewframe += 6;
				
				switch(t) {
					case 0: drawspritesetcol(x, y, crewframe, 0, help);	break;
					case 1: drawspritesetcol(x, y, crewframe, 20, help);	break;
					case 2: drawspritesetcol(x, y, crewframe, 14, help);	break;
					case 3: drawspritesetcol(x, y, crewframe, 15, help);	break;
					case 4: drawspritesetcol(x, y, crewframe, 13, help);	break;
					case 5: drawspritesetcol(x, y, crewframe, 16, help);	break;
				}
        
				if (flipmode) crewframe -= 6;
			}
		}
		
		public function drawtrophytext(obj:entityclass, help:helpclass):void {
			if (obj.trophytext < 15) {
				temp = (196 * obj.trophytext) / 15;	temp2 = (196 * obj.trophytext) / 15; temp3 = ((255 - help.glow) * obj.trophytext) / 15; 
			}else { temp = 196;	temp2 = 196; temp3 = 255 - help.glow; }
			switch(obj.trophytype) {
				case 1:
				  print( -1, 6, "SPACE STATION 1 MASTERED", temp, temp2, temp3, true);
					print( -1, 16, "Obtain a V Rank in this Time Trial", temp, temp2, temp3, true);
				break;
				case 2:
				  print( -1, 6, "LABORATORY MASTERED", temp, temp2, temp3, true);
					print( -1, 16, "Obtain a V Rank in this Time Trial", temp, temp2, temp3, true);
				break;
				case 3:
				  print( -1, 6, "THE TOWER MASTERED", temp, temp2, temp3, true);
					print( -1, 16, "Obtain a V Rank in this Time Trial", temp, temp2, temp3, true);
				break;
				case 4:
				  print( -1, 6, "SPACE STATION 2 MASTERED", temp, temp2, temp3, true);
					print( -1, 16, "Obtain a V Rank in this Time Trial", temp, temp2, temp3, true);
				break;
				case 5:
				  print( -1, 6, "WARP ZONE MASTERED", temp, temp2, temp3, true);
					print( -1, 16, "Obtain a V Rank in this Time Trial", temp, temp2, temp3, true);
				break;
				case 6:
				  print( -1, 6, "FINAL LEVEL MASTERED", temp, temp2, temp3, true);
					print( -1, 16, "Obtain a V Rank in this Time Trial", temp, temp2, temp3, true);
				break;
				case 7:
				  print( -1, 6, "GAME COMPLETE", temp, temp2, temp3, true);
					print( -1, 16, "Complete the game", temp, temp2, temp3, true);
				break;
				case 8:
				  print( -1, 6, "FLIP MODE COMPLETE", temp, temp2, temp3, true);
					print( -1, 16, "Complete the game in flip mode", temp, temp2, temp3, true);
				break;
				case 9:
					print( -1, 11, "Win with less than 50 deaths", temp, temp2, temp3, true);
				break;
				case 10:
					print( -1, 11, "Win with less than 100 deaths", temp, temp2, temp3, true);
				break;
				case 11:
					print( -1, 11, "Win with less than 250 deaths", temp, temp2, temp3, true);
				break;
				case 12:
					print( -1, 11, "Win with less than 500 deaths", temp, temp2, temp3, true);
				break;
				case 13:
					print( -1, 11, "Last 5 seconds on the Super Gravitron", temp, temp2, temp3, true);
				break;
				case 14:
					print( -1, 11, "Last 10 seconds on the Super Gravitron", temp, temp2, temp3, true);
				break;
				case 15:
					print( -1, 11, "Last 15 seconds on the Super Gravitron", temp, temp2, temp3, true);
				break;
				case 16:
					print( -1, 11, "Last 20 seconds on the Super Gravitron", temp, temp2, temp3, true);
				break;
				case 17:
					print( -1, 11, "Last 30 seconds on the Super Gravitron", temp, temp2, temp3, true);
				break;
				case 18:
					print( -1, 11, "Last 1 minute on the Super Gravitron", temp, temp2, temp3, true);
				break;
				case 20:
				  print( -1, 6, "MASTER OF THE UNIVERSE", temp, temp2, temp3, true);
					print( -1, 16, "Complete the game in no death mode", temp, temp2, temp3, true);
				break;
			}
		}
		
		public function drawsprite(x:int, y:int, t:int, r:int, g:int, b:int):void {
			tpoint.x = x; tpoint.y = y;
			setcolreal(RGB(r,g,b)); 
			sprites[t].colorTransform(sprites_rect, ct);
			backbuffer.copyPixels(sprites[t], sprites_rect, tpoint);
		}
		
		public function drawgravityline(t:int, obj:entityclass):void {
			if (obj.entities[t].life == 0) {
				/*
				switch(linestate) {
					case 0: backbuffer.fillRect(line_rect, RGB(0, 200, 0)); break;
					case 1: backbuffer.fillRect(line_rect, RGB(16, 245, 0)); break;
					case 2: backbuffer.fillRect(line_rect, RGB(0, 245, 16)); break;
					case 3: backbuffer.fillRect(line_rect, RGB(16, 200, 0)); break;
					case 4: backbuffer.fillRect(line_rect, RGB(24, 255, 16)); break;
					case 5: backbuffer.fillRect(line_rect, RGB(16, 235, 0)); break;
					case 6: backbuffer.fillRect(line_rect, RGB(0, 164, 16)); break;
					case 7: backbuffer.fillRect(line_rect, RGB(16, 245, 24)); break;
					case 8: backbuffer.fillRect(line_rect, RGB(0, 255, 16)); break;
					case 9: backbuffer.fillRect(line_rect, RGB(96, 245, 96)); break;
				}
				*/
				switch(linestate) {
					case 0: backbuffer.fillRect(line_rect, RGB(200 - 20, 200 - 20, 200 - 20)); break;
					case 1: backbuffer.fillRect(line_rect, RGB(225 - 30, 245 - 30, 245 - 30)); break;
					case 2: backbuffer.fillRect(line_rect, RGB(245 - 30, 245 - 30, 225 - 30)); break;
					case 3: backbuffer.fillRect(line_rect, RGB(164 - 10, 200 - 20, 200 - 20)); break;
					case 4: backbuffer.fillRect(line_rect, RGB(224 - 20, 255 - 30, 196 - 20)); break;
					case 5: backbuffer.fillRect(line_rect, RGB(205 - 20, 235 - 30, 196 - 20)); break;
					case 6: backbuffer.fillRect(line_rect, RGB(164 - 10, 164 - 10, 164 - 10)); break;
					case 7: backbuffer.fillRect(line_rect, RGB(225 - 30, 245 - 30, 205 - 20)); break;
					case 8: backbuffer.fillRect(line_rect, RGB(205 - 20, 255 - 30, 225 - 30)); break;
					case 9: backbuffer.fillRect(line_rect, RGB(245 - 30, 245 - 30, 245 - 30)); break;
				}
			}else{
				backbuffer.fillRect(line_rect, RGB(96, 96, 96));
			}
		}
		
		public function drawentities(map:mapclass, obj:entityclass, help:helpclass):void {
			//Update line colours!
			if (linedelay <= 0) {
				linestate++;
				if (linestate >= 10) linestate = 0;
				linedelay = 2;
			}else {
				linedelay--;
			}
			
			var j:int, k:int;
			for (var i:int = obj.nentity - 1; i >= 0; i--) {
				if (!obj.entities[i].invis && obj.entities[i].active) {
					if (obj.entities[i].size == 0) {      // Sprites
						if (flipmode) {
							tpoint.x = obj.entities[i].xp; tpoint.y = obj.entities[i].yp;
							setcol(obj.entities[i].colour, help); 
							flipsprites[obj.entities[i].drawframe].colorTransform(sprites_rect, ct);
							backbuffer.copyPixels(flipsprites[obj.entities[i].drawframe], sprites_rect, tpoint);
							if (map.warpx) {
								//screenwrapping!
								if (tpoint.x < 0) {
									tpoint.x += 320;
									backbuffer.copyPixels(flipsprites[obj.entities[i].drawframe], sprites_rect, tpoint);
								}
								if (tpoint.x > 300) {
									tpoint.x -= 320;
									backbuffer.copyPixels(flipsprites[obj.entities[i].drawframe], sprites_rect, tpoint);
								}
							}else if (map.warpy) {
								if (tpoint.y < 0) {
									tpoint.y += 230;
									backbuffer.copyPixels(flipsprites[obj.entities[i].drawframe], sprites_rect, tpoint);
								}
								if (tpoint.y > 210) {
									tpoint.y -= 230;
									backbuffer.copyPixels(flipsprites[obj.entities[i].drawframe], sprites_rect, tpoint);
								}
							}
						}else{
							tpoint.x = obj.entities[i].xp; tpoint.y = obj.entities[i].yp;
							setcol(obj.entities[i].colour, help); 
							sprites[obj.entities[i].drawframe].colorTransform(sprites_rect, ct);
							backbuffer.copyPixels(sprites[obj.entities[i].drawframe], sprites_rect, tpoint);
							if (map.warpx) {
								//screenwrapping!
								if (tpoint.x < 0) {
									tpoint.x += 320;
									backbuffer.copyPixels(sprites[obj.entities[i].drawframe], sprites_rect, tpoint);
								}
								if (tpoint.x > 300) {
									tpoint.x -= 320;
									backbuffer.copyPixels(sprites[obj.entities[i].drawframe], sprites_rect, tpoint);
								}
							}else if (map.warpy) {
								if (tpoint.y < 0) {
									tpoint.y += 230;
									backbuffer.copyPixels(sprites[obj.entities[i].drawframe], sprites_rect, tpoint);
								}
								if (tpoint.y > 210) {
									tpoint.y -= 230;
									backbuffer.copyPixels(sprites[obj.entities[i].drawframe], sprites_rect, tpoint);
								}
							}
						}
					}else if (obj.entities[i].size == 1) { // Tiles
					  tpoint.x = obj.entities[i].xp; tpoint.y = obj.entities[i].yp;
					  backbuffer.copyPixels(tiles[obj.entities[i].drawframe], tiles_rect, tpoint);
					}else if (obj.entities[i].size == 2) { // Special: Moving platform, 4 tiles
					  tpoint.x = obj.entities[i].xp; tpoint.y = obj.entities[i].yp;
						if(map.custommode){
							backbuffer.copyPixels(entcolours[obj.entities[i].drawframe], tiles_rect, tpoint);
							tpoint.x += 8; backbuffer.copyPixels(entcolours[obj.entities[i].drawframe], tiles_rect, tpoint);
							tpoint.x += 8; backbuffer.copyPixels(entcolours[obj.entities[i].drawframe], tiles_rect, tpoint);
							tpoint.x += 8; backbuffer.copyPixels(entcolours[obj.entities[i].drawframe], tiles_rect, tpoint);
						}else{
							backbuffer.copyPixels(tiles[obj.entities[i].drawframe], tiles_rect, tpoint);
							tpoint.x += 8; backbuffer.copyPixels(tiles[obj.entities[i].drawframe], tiles_rect, tpoint);
							tpoint.x += 8; backbuffer.copyPixels(tiles[obj.entities[i].drawframe], tiles_rect, tpoint);
							tpoint.x += 8; backbuffer.copyPixels(tiles[obj.entities[i].drawframe], tiles_rect, tpoint);
						}
					}else if (obj.entities[i].size == 3) { // Big chunky pixels!
					  prect.x = obj.entities[i].xp; prect.y = obj.entities[i].yp;
						//A seperate index of colours, for simplicity
						if(obj.entities[i].colour==1){
						  backbuffer.fillRect(prect, RGB(196 - (Math.random() * 64), 10, 10));
						}else if (obj.entities[i].colour == 2) {
							backbuffer.fillRect(prect, RGB(160- help.glow/2 - (Math.random()*20), 200- help.glow/2, 220 - help.glow));
						}
					}else if (obj.entities[i].size == 4) { // Small pickups
					  drawhuetile(obj.entities[i].xp, obj.entities[i].yp, obj.entities[i].tile, obj.entities[i].colour);
					}else if (obj.entities[i].size == 5) { //Horizontal Line
						line_rect.x = obj.entities[i].xp; line_rect.y = obj.entities[i].yp;
						line_rect.width = obj.entities[i].w; line_rect.height = 1;
						drawgravityline(i, obj);
					}else if (obj.entities[i].size == 6) { //Vertical Line
						line_rect.x = obj.entities[i].xp; line_rect.y = obj.entities[i].yp;
						line_rect.width = 1; line_rect.height = obj.entities[i].h;
						drawgravityline(i, obj);
					}else if (obj.entities[i].size == 7) { //Teleporter
					  drawtele(obj.entities[i].xp, obj.entities[i].yp, obj.entities[i].drawframe, obj.entities[i].colour, help);
					}else if (obj.entities[i].size == 8) { // Special: Moving platform, 8 tiles
					  tpoint.x = obj.entities[i].xp; tpoint.y = obj.entities[i].yp;
						if (map.custommode) {
							backbuffer.copyPixels(entcolours[obj.entities[i].drawframe], tiles_rect, tpoint);
							tpoint.x += 8; backbuffer.copyPixels(entcolours[obj.entities[i].drawframe], tiles_rect, tpoint);
							tpoint.x += 8; backbuffer.copyPixels(entcolours[obj.entities[i].drawframe], tiles_rect, tpoint);
							tpoint.x += 8; backbuffer.copyPixels(entcolours[obj.entities[i].drawframe], tiles_rect, tpoint);
							tpoint.x += 8; backbuffer.copyPixels(entcolours[obj.entities[i].drawframe], tiles_rect, tpoint);
							tpoint.x += 8; backbuffer.copyPixels(entcolours[obj.entities[i].drawframe], tiles_rect, tpoint);
							tpoint.x += 8; backbuffer.copyPixels(entcolours[obj.entities[i].drawframe], tiles_rect, tpoint);
							tpoint.x += 8; backbuffer.copyPixels(entcolours[obj.entities[i].drawframe], tiles_rect, tpoint);
						}else{
							backbuffer.copyPixels(tiles[obj.entities[i].drawframe], tiles_rect, tpoint);
							tpoint.x += 8; backbuffer.copyPixels(tiles[obj.entities[i].drawframe], tiles_rect, tpoint);
							tpoint.x += 8; backbuffer.copyPixels(tiles[obj.entities[i].drawframe], tiles_rect, tpoint);
							tpoint.x += 8; backbuffer.copyPixels(tiles[obj.entities[i].drawframe], tiles_rect, tpoint);
							tpoint.x += 8; backbuffer.copyPixels(tiles[obj.entities[i].drawframe], tiles_rect, tpoint);
							tpoint.x += 8; backbuffer.copyPixels(tiles[obj.entities[i].drawframe], tiles_rect, tpoint);
							tpoint.x += 8; backbuffer.copyPixels(tiles[obj.entities[i].drawframe], tiles_rect, tpoint);
							tpoint.x += 8; backbuffer.copyPixels(tiles[obj.entities[i].drawframe], tiles_rect, tpoint);
						}
					}else if (obj.entities[i].size == 9) {      // Really Big Sprite! (2x2)
						if (flipmode) {
							setcol(obj.entities[i].colour, help); 
							
							tpoint.x = obj.entities[i].xp; tpoint.y = obj.entities[i].yp;
							flipsprites[obj.entities[i].drawframe].colorTransform(sprites_rect, ct);
							backbuffer.copyPixels(flipsprites[obj.entities[i].drawframe], sprites_rect, tpoint);
							
							tpoint.x = obj.entities[i].xp+32; tpoint.y = obj.entities[i].yp;
							flipsprites[obj.entities[i].drawframe+1].colorTransform(sprites_rect, ct);
							backbuffer.copyPixels(flipsprites[obj.entities[i].drawframe+1], sprites_rect, tpoint);
							
							tpoint.x = obj.entities[i].xp; tpoint.y = obj.entities[i].yp+32;
							flipsprites[obj.entities[i].drawframe+12].colorTransform(sprites_rect, ct);
							backbuffer.copyPixels(flipsprites[obj.entities[i].drawframe+12], sprites_rect, tpoint);
							
							tpoint.x = obj.entities[i].xp+32; tpoint.y = obj.entities[i].yp+32;
							flipsprites[obj.entities[i].drawframe+13].colorTransform(sprites_rect, ct);
							backbuffer.copyPixels(flipsprites[obj.entities[i].drawframe + 13], sprites_rect, tpoint);
						}else{
							setcol(obj.entities[i].colour, help); 
							
							tpoint.x = obj.entities[i].xp; tpoint.y = obj.entities[i].yp;
							sprites[obj.entities[i].drawframe].colorTransform(sprites_rect, ct);
							backbuffer.copyPixels(sprites[obj.entities[i].drawframe], sprites_rect, tpoint);
							
							tpoint.x = obj.entities[i].xp+32; tpoint.y = obj.entities[i].yp;
							sprites[obj.entities[i].drawframe+1].colorTransform(sprites_rect, ct);
							backbuffer.copyPixels(sprites[obj.entities[i].drawframe+1], sprites_rect, tpoint);
							
							tpoint.x = obj.entities[i].xp; tpoint.y = obj.entities[i].yp+32;
							sprites[obj.entities[i].drawframe+12].colorTransform(sprites_rect, ct);
							backbuffer.copyPixels(sprites[obj.entities[i].drawframe+12], sprites_rect, tpoint);
							
							tpoint.x = obj.entities[i].xp+32; tpoint.y = obj.entities[i].yp+32;
							sprites[obj.entities[i].drawframe+13].colorTransform(sprites_rect, ct);
							backbuffer.copyPixels(sprites[obj.entities[i].drawframe + 13], sprites_rect, tpoint);
						}
					}else if (obj.entities[i].size == 10) {      // 2x1 Sprite
						if (flipmode) {
							setcol(obj.entities[i].colour, help); 
							
							tpoint.x = obj.entities[i].xp; tpoint.y = obj.entities[i].yp;
							flipsprites[obj.entities[i].drawframe].colorTransform(sprites_rect, ct);
							backbuffer.copyPixels(flipsprites[obj.entities[i].drawframe], sprites_rect, tpoint);
							
							tpoint.x = obj.entities[i].xp+32; tpoint.y = obj.entities[i].yp;
							flipsprites[obj.entities[i].drawframe+1].colorTransform(sprites_rect, ct);
							backbuffer.copyPixels(flipsprites[obj.entities[i].drawframe + 1], sprites_rect, tpoint);
						}else{
							setcol(obj.entities[i].colour, help); 
							
							tpoint.x = obj.entities[i].xp; tpoint.y = obj.entities[i].yp;
							sprites[obj.entities[i].drawframe].colorTransform(sprites_rect, ct);
							backbuffer.copyPixels(sprites[obj.entities[i].drawframe], sprites_rect, tpoint);
							
							tpoint.x = obj.entities[i].xp+32; tpoint.y = obj.entities[i].yp;
							sprites[obj.entities[i].drawframe+1].colorTransform(sprites_rect, ct);
							backbuffer.copyPixels(sprites[obj.entities[i].drawframe + 1], sprites_rect, tpoint);
						}
					}else if (obj.entities[i].size == 11) { //The fucking elephant
						setcol(obj.entities[i].colour, help); 
						images[3].colorTransform(images[3].rect, ct);
					  drawimage(3, obj.entities[i].xp, obj.entities[i].yp);
					}else if (obj.entities[i].size == 12) {      // Regular sprites that don't wrap
						if (flipmode) {
							//forget this for a minute; 
							tpoint.x = obj.entities[i].xp; tpoint.y = obj.entities[i].yp;
							setcol(obj.entities[i].colour, help); 
							flipsprites[obj.entities[i].drawframe].colorTransform(sprites_rect, ct);
							backbuffer.copyPixels(flipsprites[obj.entities[i].drawframe], sprites_rect, tpoint);
							
							//if we're outside the screen, we need to draw indicators
							if (obj.entities[i].xp < -20 && obj.entities[i].vx > 0) {
								if (obj.entities[i].xp < -100) {
									tpoint.x = -5 + (int(( -obj.entities[i].xp) / 10)); 
								}else{
								  tpoint.x = 5; 
								}
								
								tpoint.y = tpoint.y+4;
								setcol(23, help); 
							  tiles[1167].colorTransform(tiles_rect, ct);
					      backbuffer.copyPixels(tiles[1167], tiles_rect, tpoint);
							}else if (obj.entities[i].xp > 340 && obj.entities[i].vx < 0) {
								if (obj.entities[i].xp > 420) {
									tpoint.x = 320 - (int(( obj.entities[i].xp-320) / 10)); 
								}else{
								  tpoint.x = 310; 
								}
								
								tpoint.y = tpoint.y+4;
								setcol(23, help); 
							  tiles[1166].colorTransform(tiles_rect, ct);
					      backbuffer.copyPixels(tiles[1166], tiles_rect, tpoint);
							}
						}else{
							tpoint.x = obj.entities[i].xp; tpoint.y = obj.entities[i].yp;
							setcol(obj.entities[i].colour, help); 
							sprites[obj.entities[i].drawframe].colorTransform(sprites_rect, ct);
							backbuffer.copyPixels(sprites[obj.entities[i].drawframe], sprites_rect, tpoint);
							
							//if we're outside the screen, we need to draw indicators
							
							if (obj.entities[i].xp < -20 && obj.entities[i].vx > 0) {
								if (obj.entities[i].xp < -100) {
									tpoint.x = -5 + (int(( -obj.entities[i].xp) / 10)); 
								}else{
								  tpoint.x = 5; 
								}
								
								tpoint.y = tpoint.y+4;
								setcol(23, help); 
							  tiles[1167].colorTransform(tiles_rect, ct);
					      backbuffer.copyPixels(tiles[1167], tiles_rect, tpoint);
							}else if (obj.entities[i].xp > 340 && obj.entities[i].vx < 0) {
								if (obj.entities[i].xp > 420) {
									tpoint.x = 320 - (int(( obj.entities[i].xp-320) / 10)); 
								}else{
								  tpoint.x = 310; 
								}
								
								tpoint.y = tpoint.y+4;
								setcol(23, help); 
							  tiles[1166].colorTransform(tiles_rect, ct);
					      backbuffer.copyPixels(tiles[1166], tiles_rect, tpoint);
							}
						}
					}else if (obj.entities[i].size == 13) {      // Special for epilogue: huge hero!
						if (flipmode) {
							scaleMatrix = new Matrix();
			        scaleMatrix.scale(6, 6);
  		        bigbuffer.fillRect(bigbuffer.rect, 0x000000);
							
							tpoint.x = obj.entities[i].xp; tpoint.y = obj.entities[i].yp;
							setcol(obj.entities[i].colour, help); 
							flipsprites[obj.entities[i].drawframe].colorTransform(sprites_rect, ct);
							bigbuffer.copyPixels(flipsprites[obj.entities[i].drawframe], sprites_rect, new Point(0, 0));			
							
							scaleMatrix.translate(obj.entities[i].xp, obj.entities[i].yp);
							backbuffer.draw(bigbufferscreen, scaleMatrix);
							scaleMatrix.translate(-obj.entities[i].xp, -obj.entities[i].yp);
						}else {
							scaleMatrix = new Matrix();
			        scaleMatrix.scale(6, 6);
  		        bigbuffer.fillRect(bigbuffer.rect, 0x000000);
							
							tpoint.x = obj.entities[i].xp; tpoint.y = obj.entities[i].yp;
							setcol(obj.entities[i].colour, help); 
							sprites[obj.entities[i].drawframe].colorTransform(sprites_rect, ct);
							bigbuffer.copyPixels(sprites[obj.entities[i].drawframe], sprites_rect, new Point(0, 0));			
							
							scaleMatrix.translate(obj.entities[i].xp, obj.entities[i].yp);
							backbuffer.draw(bigbufferscreen, scaleMatrix);
							scaleMatrix.translate(-obj.entities[i].xp, -obj.entities[i].yp);
						}
					}
				}
			}
		}
		
		public function drawbuffertile(x:int, y:int, t:int):void {
			tpoint.x = x; tpoint.y = y;
			buffer.copyPixels(tiles[t], tiles_rect, tpoint);
		}
		
		public function drawforetile(x:int, y:int, t:int):void {
			tpoint.x = x; tpoint.y = y;
			frontbuffer.copyPixels(tiles[t], tiles_rect, tpoint);
		}
		
		public function drawforetile2(x:int, y:int, t:int):void {
			tpoint.x = x; tpoint.y = y;
			frontbuffer.copyPixels(tiles2[t], tiles_rect, tpoint);
		}
		
		public function drawforetile3(x:int, y:int, t:int, off:int):void {
			tpoint.x = x; tpoint.y = y;
			frontbuffer.copyPixels(tiles3[t+(off*30)], tiles_rect, tpoint);
		}
		
		
		public function drawtele(x:int, y:int, t:int, c:int, help:helpclass):void {
			tpoint.x = x; tpoint.y = y;
			
			setcolreal(RGB(16,16,16)); 
			tele[0].colorTransform(tele_rect, ct);
			backbuffer.copyPixels(tele[0], tele_rect, tpoint);
			
			setcol(c, help); 
			if (t > 9) t = 8; if (t < 0) t = 0;
			tele[t].colorTransform(tele_rect, ct);
			backbuffer.copyPixels(tele[t], tele_rect, tpoint);
		}
		
		public function drawtile(x:int, y:int, t:int):void {
			tpoint.x = x; tpoint.y = y;
			backbuffer.copyPixels(tiles[t], tiles_rect, tpoint);
		}
		
		public function drawtile2(x:int, y:int, t:int):void {
			tpoint.x = x; tpoint.y = y;
			backbuffer.copyPixels(tiles2[t], tiles_rect, tpoint);
		}
		
		public function drawtile3(x:int, y:int, t:int, off:int):void {
			tpoint.x = x; tpoint.y = y;
			backbuffer.copyPixels(tiles3[t+(off*30)], tiles_rect, tpoint);
		}
		
		public function drawtowertile3(x:int, y:int, t:int, off:int):void {
			tpoint.x = x; tpoint.y = y;
			towerbuffer.copyPixels(tiles3[t+(off*30)], tiles_rect, tpoint);
		}
		
		public function drawtowertile(x:int, y:int, t:int):void {
			tpoint.x = x; tpoint.y = y;
			towerbuffer.copyPixels(tiles2[t], tiles_rect, tpoint, null, null, true);
		}
		
		public function drawmap(map:mapclass):void {
			if (!foregrounddrawn) {
				frontbuffer.fillRect(frontbuffer.rect, 0x000000);
				if(map.tileset==0){
					for (j = 0; j < 29+map.extrarow; j++) {
						for (i = 0; i < 40; i++) {			
							if(map.contents[i + map.vmult[j]]>0) drawforetile(i * 8, j * 8, map.contents[i + map.vmult[j]]);
						}
					}
				}else if (map.tileset == 1) {
					for (j = 0; j < 29+map.extrarow; j++) {
						for (i = 0; i < 40; i++) {			
							if(map.contents[i + map.vmult[j]]>0) drawforetile2(i * 8, j * 8, map.contents[i + map.vmult[j]]);
						}
					}
				}else if (map.tileset == 2) {
					for (j = 0; j < 29+map.extrarow; j++) {
						for (i = 0; i < 40; i++) {			
							if(map.contents[i + map.vmult[j]]>0) drawforetile3(i * 8, j * 8, map.contents[i + map.vmult[j]],map.rcol);
						}
					}
				}
				foregrounddrawn=true;
			}
			
			backbuffer.copyPixels(frontbuffer, frontbuffer.rect, tl);
		}
		
		public function drawfinalmap(map:mapclass):void {
			//Update colour cycling for final level
			if (map.final_colormode) {
				map.final_aniframedelay--;
				if(map.final_aniframedelay==0) foregrounddrawn=false;
				if (map.final_aniframedelay <= 0) {
					map.final_aniframedelay = 2;
					map.final_aniframe++;
					if (map.final_aniframe >= 4) map.final_aniframe = 0;
				}
			}
			
			if (!foregrounddrawn) {
				frontbuffer.fillRect(frontbuffer.rect, 0x000000);
				if(map.tileset==0){
					for (j = 0; j < 29+map.extrarow; j++) {
						for (i = 0; i < 40; i++) {			
							if(map.contents[i + map.vmult[j]]>0) drawforetile(i * 8, j * 8, map.finalat(i,j));
						}
					}
				}else if (map.tileset == 1) {
					for (j = 0; j < 29+map.extrarow; j++) {
						for (i = 0; i < 40; i++) {			
							if(map.contents[i + map.vmult[j]]>0) drawforetile2(i * 8, j * 8, map.finalat(i,j));
						}
					}
				}
			  foregrounddrawn=true;
			}
			
			backbuffer.copyPixels(frontbuffer, frontbuffer.rect, tl);
		}
		
		public function drawmaptileat(xp:int, yp:int, mapx:int, mapy:int, map:mapclass):void {
			if (mapx < 100) mapx = 119;	if (mapy < 100) mapy = 119;
			if (mapx > 119) mapx = 100;	if (mapy > 119) mapy = 100;
		  if (map.explored[(mapx - 100) + ((mapy - 100) * 20)] == 1) {
				madrect.x = (mapx - 100)*12; madrect.y = (mapy - 100)*9; madrect.width = 12; madrect.height = 9;
				madpoint.x = xp; madpoint.y = yp;
				backbuffer.copyPixels(images[1], madrect, madpoint);
			}else {
				drawimage(2, xp, yp);
			}
		}
		
		public function drawminimap(game:gameclass, map:mapclass):void {
			drawfillrect(272, 8, 40, 31, 196, 196, 196);
			drawfillrect(273, 9, 38, 29, 164,164,164);
			for (j = 0; j < 3; j++){
			  for (i = 0; i < 3; i++) {
					drawmaptileat(274 + (i * 12), 10 + (j * 9), game.roomx - 1 + i, game.roomy - 1 + j, map);
				}
			}
		}

		public function drawrect(x:int, y:int, w:int, h:int, r:int, g:int, b:int):void {
			//Draw the retangle indicated by that object
			madrect.x = x; madrect.y = y;
			madrect.width = w; madrect.height = 1;
			backbuffer.fillRect(madrect, RGB(r,g,b));
			
			madrect.width = 1; madrect.height = h;
			backbuffer.fillRect(madrect, RGB(r,g,b));
			
			madrect.x = x + w - 1;
			madrect.width = 1; madrect.height = h;
			backbuffer.fillRect(madrect, RGB(r,g,b));
			
			madrect.x = x; madrect.y = y + h - 1;
			madrect.width = w; madrect.height = 1;
			backbuffer.fillRect(madrect, RGB(r,g,b));
		}
		
		public function drawfillrect(x:int, y:int, w:int, h:int, r:int, g:int, b:int):void {
			//Draw the retangle indicated by that object
			madrect.x = x; madrect.y = y;
			madrect.width = w; madrect.height = h;
			backbuffer.fillRect(madrect, RGB(r,g,b));
		}
		
		public function printcrewname(x:int, y:int, t:int):void {
			//Print the name of crew member t in the right colour
			switch(t) {
				case 0: print(x, y, "Viridian", 16, 240, 240); break;
				case 1: print(x, y, "Violet", 240, 16, 240); break;
				case 2: print(x, y, "Vitellary", 240, 240, 16); break;
				case 3: print(x, y, "Vermilion", 240, 16, 16); break;
				case 4: print(x, y, "Verdigris", 16, 240, 16); break;
				case 5: print(x, y, "Victoria", 16, 16, 240); break;
			}
		}
		
		public function printcrewnamestatus(x:int, y:int, t:int):void {
			//Print the status of crew member t in the right colour
			switch(t) {
				case 0: print(x, y, "(that's you!)", 12, 140, 140); break;
				case 1: print(x, y, "Rescued!", 140, 12, 140); break;
				case 2: print(x, y, "Rescued!", 140, 140, 12); break;
				case 3: print(x, y, "Rescued!", 140, 12, 12); break;
				case 4: print(x, y, "Rescued!", 12, 140, 12); break;
				case 5: print(x, y, "Rescued!", 12, 12, 140); break;
			}
		}
		
		public function printcrewnamedark(x:int, y:int, t:int):void {
			//Print the name of crew member t as above, but in black and white
			switch(t) {
				case 0: print(x, y, "Viridian", 128,128,128); break;
				case 1: print(x, y, "Violet", 128,128,128); break;
				case 2: print(x, y, "Vitellary", 128,128,128); break;
				case 3: print(x, y, "Vermilion", 128,128,128); break;
				case 4: print(x, y, "Verdigris", 128,128,128); break;
				case 5: print(x, y, "Victoria", 128,128,128); break;
			}
		}
		
		public function printmask(x:int, y:int, t:String, cen:Boolean = false):void {			
			if (cen) x = 160 - (len(t) / 2);
			bfontpos = 0;
			for (var i:int = 0; i < t.length; i++) {
				cur = t.charCodeAt(i);
				tpoint.x = x + bfontpos-1; tpoint.y = y;
				if (flipmode) {
				  backbuffer.copyPixels(flipbfontmask[cur], bfont_rect, tpoint);
				}else{
				  backbuffer.copyPixels(bfontmask[cur], bfont_rect, tpoint);
				}
				bfontpos+=bfontlen[cur];
			}
		}
		
		public function print(x:int, y:int, t:String, r:int, g:int, b:int, cen:Boolean = false):void {
			if (r < 0) r = 0; if (g < 0) g = 0; if (b < 0) b = 0;
			if (r > 255) r = 255; if (g > 255) g = 255; if (b > 255) b = 255;
			ct.color = RGB(r, g, b);
			if (cen) x = 160 - (len(t) / 2);
			bfontpos = 0;
			for (var i:int = 0; i < t.length; i++) {
				cur = t.charCodeAt(i);
				tpoint.x = x + bfontpos; tpoint.y = y;
				if (flipmode) {
				  flipbfont[cur].colorTransform(bfont_rect, ct);
				  backbuffer.copyPixels(flipbfont[cur], bfont_rect, tpoint);
				}else {
				  bfont[cur].colorTransform(bfont_rect, ct);
				  backbuffer.copyPixels(bfont[cur], bfont_rect, tpoint);
				}
				bfontpos+=bfontlen[cur];
			}
		}
		
		public function rprint(x:int, y:int, t:String, r:int, g:int, b:int, cen:Boolean = false):void {
			if (r < 0) r = 0; if (g < 0) g = 0; if (b < 0) b = 0;
			if (r > 255) r = 255; if (g > 255) g = 255; if (b > 255) b = 255;
			ct.color = RGB(r, g, b);
			if (cen) x = 308 - (len(t));
			bfontpos = 0;
			for (var i:int = 0; i < t.length; i++) {
				cur = t.charCodeAt(i);
				tpoint.x = x + bfontpos; tpoint.y = y;
				if (flipmode) {
			   	flipbfont[cur].colorTransform(bfont_rect, ct);
				  backbuffer.copyPixels(flipbfont[cur], bfont_rect, tpoint);
				}else{
				  bfont[cur].colorTransform(bfont_rect, ct);
				  backbuffer.copyPixels(bfont[cur], bfont_rect, tpoint);
				}
				bfontpos+=bfontlen[cur];
			}
		}
		
		public function printoff(x:int, y:int, t:String, r:int, g:int, b:int, cen:Boolean = false):void {
			if (r < 0) r = 0; if (g < 0) g = 0; if (b < 0) b = 0;
			if (r > 255) r = 255; if (g > 255) g = 255; if (b > 255) b = 255;
			ct.color = RGB(r, g, b);
			if (cen) x = (160 - (len(t) / 2))+x;
			bfontpos = 0;
			for (var i:int = 0; i < t.length; i++) {
				cur = t.charCodeAt(i);
				tpoint.x = x + bfontpos; tpoint.y = y;
				if (flipmode) {
				  flipbfont[cur].colorTransform(bfont_rect, ct);
					backbuffer.copyPixels(flipbfont[cur], bfont_rect, tpoint);
				}else{
				  bfont[cur].colorTransform(bfont_rect, ct);
				  backbuffer.copyPixels(bfont[cur], bfont_rect, tpoint);
				}
				bfontpos+=bfontlen[cur];
			}
		}
		
		public function bprint(x:int, y:int, t:String, r:int, g:int, b:int, cen:Boolean=false):void {			
			//printmask(x, y, t, cen);
			print(x, y - 1, t, 0, 0, 0, cen);
			if (cen) {
				printoff(-1, y, t, 0, 0, 0, cen); //kludge :i
				printoff(1, y, t, 0, 0, 0, cen);
			}else{
				print(x-1, y, t, 0, 0, 0, cen);
				print(x+1, y, t, 0, 0, 0, cen);
			}
			print(x, y+1, t, 0, 0, 0, cen);
			
			print(x, y, t, r, g, b, cen);
		}
		
		public function printmasktemptile(x:int, y:int, t:String, cen:Boolean = false):void {			
			if (cen) x = 160- (len(t) / 2);
			bfontpos = 0;
			for (var i:int = 0; i < t.length; i++) {
				cur = t.charCodeAt(i);
				tpoint.x = x + bfontpos - 1; tpoint.y = y;
				if (flipmode) {
					temptile.copyPixels(flipbfontmask[cur], bfont_rect, tpoint);
				}else{
				  temptile.copyPixels(bfontmask[cur], bfont_rect, tpoint);
				}
				bfontpos+=bfontlen[cur];
			}
		}
		
		public function printtemptile(x:int, y:int, t:String, r:int, g:int, b:int, cen:Boolean = false):void {
			if (r < 0) r = 0; if (g < 0) g = 0; if (b < 0) b = 0;
			if (r > 255) r = 255; if (g > 255) g = 255; if (b > 255) b = 255;
			ct.color = RGB(r, g, b);
			if (cen) x = 160 - (len(t) / 2);
			bfontpos = 0;
			for (var i:int = 0; i < t.length; i++) {
				cur = t.charCodeAt(i);
				tpoint.x = x + bfontpos; tpoint.y = y;
				if (flipmode) {
				  flipbfont[cur].colorTransform(bfont_rect, ct);
					temptile.copyPixels(flipbfont[cur], bfont_rect, tpoint);
				}else{
				  bfont[cur].colorTransform(bfont_rect, ct);
				  temptile.copyPixels(bfont[cur], bfont_rect, tpoint);
				}
				bfontpos+=bfontlen[cur];
			}
		}
		
		public function onscreen(t:int):Boolean {
			if (t >= -40 && t <= 280) return true;
			return false;
		}
		
		public function bigrprint(x:int, y:int, t:String, r:int, g:int, b:int, cen:Boolean = false, sc:Number = 2):void {
			if (r < 0) r = 0; if (g < 0) g = 0; if (b < 0) b = 0;
			if (r > 255) r = 255; if (g > 255) g = 255; if (b > 255) b = 255;
			ct.color = RGB(r, g, b);
			
			scaleMatrix = new Matrix();
			scaleMatrix.scale(sc, sc);
			
  		bigbuffer.fillRect(bigbuffer.rect, 0x000000);
			
			x = x / sc;
			
			x -= (len(t));
			
			//if (r < -1) r = -1; if (g < 0) g = 0; if (b < 0) b = 0;?
			//if (r > 255) r = 255; if (g > 255) g = 255; if (b > 255) b = 255;
			ct.color = RGB(r, g, b);
			if(sc==2){
			  if (cen) x = 80 - (len(t)/2);
			}else if (sc == 3) {
				if (cen) x = 160/3 - (len(t) / 2);
			}else if (sc == 4) {
				if (cen) x = 40 - (len(t) / 2);
			}
			bfontpos = 0;
			for (i = 0; i < t.length; i++) {
				cur = t.charCodeAt(i);
				if (flipmode) {
				  flipbfont[cur].colorTransform(bfont_rect, ct);
				  bigbuffer.copyPixels(flipbfont[cur], bfont_rect, new Point(x + bfontpos, 0));
				}else {
				  bfont[cur].colorTransform(bfont_rect, ct);
				  bigbuffer.copyPixels(bfont[cur], bfont_rect, new Point(x + bfontpos, 0));
				}
				bfontpos+=bfontlen[cur];
			}
			
			scaleMatrix.translate(0, y);
			backbuffer.draw(bigbufferscreen, scaleMatrix);
			scaleMatrix.translate(0, -y);
		}
		
		public function bigprint(x:int, y:int, t:String, r:int, g:int, b:int, cen:Boolean = false, sc:Number = 2):void {
			if (r < 0) r = 0; if (g < 0) g = 0; if (b < 0) b = 0;
			if (r > 255) r = 255; if (g > 255) g = 255; if (b > 255) b = 255;
			ct.color = RGB(r, g, b);
			
			scaleMatrix = new Matrix();
			scaleMatrix.scale(sc, sc);
			
  		bigbuffer.fillRect(bigbuffer.rect, 0x000000);
			
			x = x / sc;
			
			//if (r < -1) r = -1; if (g < 0) g = 0; if (b < 0) b = 0;?
			//if (r > 255) r = 255; if (g > 255) g = 255; if (b > 255) b = 255;
			ct.color = RGB(r, g, b);
			if(sc==2){
			  if (cen) x = 80 - (len(t)/2);
			}else if (sc == 3) {
				if (cen) x = 160/3 - (len(t) / 2);
			}else if (sc == 4) {
				if (cen) x = 40 - (len(t) / 2);
			}
			bfontpos = 0;
			for (var ti:int = 0; ti < t.length; ti++) {
				cur = t.charCodeAt(ti);
				if (flipmode) {
				  flipbfont[cur].colorTransform(bfont_rect, ct);
				  bigbuffer.copyPixels(flipbfont[cur], bfont_rect, new Point(x + bfontpos, 0));
				}else {
				  bfont[cur].colorTransform(bfont_rect, ct);
				  bigbuffer.copyPixels(bfont[cur], bfont_rect, new Point(x + bfontpos, 0));
				}
				bfontpos+=bfontlen[cur];
			}
			
			scaleMatrix.translate(0, y);
			backbuffer.draw(bigbufferscreen, scaleMatrix);
			scaleMatrix.translate(0, -y);
		}
		
		public function bprinttemptile(x:int, y:int, t:String, r:int, g:int, b:int, cen:Boolean=false):void {			
			printmasktemptile(x, y, t, cen);
			printtemptile(x, y, t, r, g, b, cen);
		}
		
		public function len(t:String):int {
			bfontpos = 0;
		  for (var i:int = 0; i < t.length; i++) {
				cur = t.charCodeAt(i);
			  bfontpos+=bfontlen[cur];
			}
			return bfontpos;
		}
		
		public function flashlight():void {
			backbuffer.fillRect(backbuffer.rect, 0xBBBBBB);
		}
		
		public function screenshake():void {
			screenbuffer.lock();
			if(flipmode){
			  tpoint.x = int((Math.random() * 7) - 4); tpoint.y = int((Math.random() * 7) - 4);
			  flipmatrix.translate(tpoint.x, tpoint.y);
			  screenbuffer.draw(backbuffer, flipmatrix);
			  flipmatrix.translate(-tpoint.x, -tpoint.y);
			}else{
			  screenbuffer.copyPixels(backbuffer, backbuffer.rect, tl, null, null, false);
			  tpoint.x = (Math.random() * 7) - 4; tpoint.y = (Math.random() * 7) - 4;
			  screenbuffer.copyPixels(backbuffer, backbuffer.rect, tpoint, null, null, false);
			}
			screenbuffer.unlock();
			
			backbuffer.lock();
			backbuffer.fillRect(backbuffer.rect, 0x000000);
			backbuffer.unlock();
		}
		
		public function render():void {
			screenbuffer.lock();
			if(flipmode){
			  screenbuffer.draw(backbuffer, flipmatrix);
			}else{
			  screenbuffer.copyPixels(backbuffer, backbuffer.rect, tl, null, null, false);
			}
			screenbuffer.unlock();
			
			backbuffer.lock();
			backbuffer.fillRect(backbuffer.rect, 0x000000);
			backbuffer.unlock();
		}
		
		public function menuoffrender():void {
			screenbuffer.lock();
			screenbuffer.copyPixels(menubuffer, menubuffer.rect, tl, null, null, false);
			
			if(flipmode){
			  flipmatrix.translate(0, menuoffset);
			  screenbuffer.draw(backbuffer, flipmatrix);
			  flipmatrix.translate(0, -menuoffset);
			}else {
				tpoint.x = 0;
				tpoint.y = menuoffset;
			  screenbuffer.copyPixels(backbuffer, backbuffer.rect, tpoint, null, null, false);	
			}
			
			screenbuffer.unlock();
			
			backbuffer.lock();
			backbuffer.fillRect(backbuffer.rect, 0x000000);
			backbuffer.unlock();
		}
		
		public var backgrounds:Vector.<BitmapData> = new Vector.<BitmapData>;
		public var images:Vector.<BitmapData> = new Vector.<BitmapData>;
		public var playerlevelimages:Vector.<BitmapData> = new Vector.<BitmapData>;
		public var mobileimages:Vector.<BitmapData> = new Vector.<BitmapData>;
		public var tele:Vector.<BitmapData> = new Vector.<BitmapData>;
	  public var tele_rect:Rectangle;
		public var tiles:Vector.<BitmapData> = new Vector.<BitmapData>;
		public var tiles2:Vector.<BitmapData> = new Vector.<BitmapData>;
		public var tiles3:Vector.<BitmapData> = new Vector.<BitmapData>;
		public var sprites:Vector.<BitmapData> = new Vector.<BitmapData>;
		public var flipsprites:Vector.<BitmapData> = new Vector.<BitmapData>;
		public var entcolours:Vector.<BitmapData> = new Vector.<BitmapData>;
		public var bfont:Vector.<BitmapData> = new Vector.<BitmapData>;
		public var bfontmask:Vector.<BitmapData> = new Vector.<BitmapData>;
		public var flipbfont:Vector.<BitmapData> = new Vector.<BitmapData>;
		public var flipbfontmask:Vector.<BitmapData> = new Vector.<BitmapData>;
		public var bfontlen:Vector.<int> = new Vector.<int>;
		public var bfontpos:int;
		public var cur:int;
		public var ct:ColorTransform;
	  public var tiles_rect:Rectangle;
	  public var sprites_rect:Rectangle;
	  public var bfont_rect:Rectangle;
	  public var bfontmask_rect:Rectangle;
		public var images_rect:Rectangle;
	  public var tl:Point; //The point at (0,0)
	  public var bg_rect:Rectangle, prect:Rectangle;
		public var line_rect:Rectangle;
		
		public var temp:int;
		public var temp2:int;
		public var temp3:int;
		public var tempstring:String;
		public var alphamult:uint;
		public var stemp:String;
		public var buffer:BitmapData;
		public var i:int, j:int, k:int, m:int;
		public var tpoint:Point, trect:Rectangle;		
		public var madrect:Rectangle, madpoint:Point;
		
		public var temptile:BitmapData;
		public var tempsprite:BitmapData;
		public var footerrect:Rectangle;
		//Actual backgrounds
		public var backbuffer:BitmapData;
		public var frontbuffer:BitmapData;
		public var towerbuffer:BitmapData;
		public var screenbuffer:BitmapData;
		public var menubuffer:BitmapData;
		public var screen:Bitmap;
		public var updatebackground:Boolean;
		//Textbox Stuff
		public var ntextbox:int;
		public var textbox:Vector.<textboxclass> = new Vector.<textboxclass>;
		//Cutscene bars
		public var showcutscenebars:Boolean;
		public var cutscenebarspos:int;
		//Background Stuff
		public var stars:Vector.<Rectangle> = new Vector.<Rectangle>;
		public var starsspeed:Vector.<int> = new Vector.<int>;
		
		//special for weird lab rooms
		public var spcol:int, spcoldel:int;
		//Menu stuff
		public var crewframe:int, crewframedelay:int;
		public var menuoffset:int, resumegamemode:Boolean;
		//More Background Stuff
		public var backboxes:Vector.<Rectangle> = new Vector.<Rectangle>;
		public var backboxvx:Vector.<int> = new Vector.<int>;
		public var backboxvy:Vector.<int> = new Vector.<int>;
		public var backboxint:Vector.<Number> = new Vector.<Number>;
		public var bcol:int, bcol2:int, rcol:int;
		public var backboxrect:Rectangle;
		public var linestate:int, linedelay:int;
		public var backoffset:int, backgrounddrawn:Boolean;
		public var foregrounddrawn:Boolean;
		
		public var warpskip:int;
		public var warpfcol:int, warpbcol:int;
		public var warprect:Rectangle;
		//Fade effect
		public var fademode:int;
		public var fadeamount:int;
		public var fadebars:Vector.<int> = new Vector.<int>;
		//Single trinket colour
		public var trinketcolset:Boolean, trinketr:int, trinketg:int, trinketb:int;
		//Flipmode Stuff
		public var flipmode:Boolean, setflipmode:Boolean;
		public var flipmatrix:Matrix = new Matrix();
		public var flipfontmatrix:Matrix = new Matrix();
		public var flipfontmatrix2:Matrix = new Matrix();
		
		public var scaleMatrix:Matrix = new Matrix();
		public var bigbuffer:BitmapData;
		public var bigbufferscreen:Bitmap;
		
		//Mobile stuff
		public var screensizemultiplier:Number;
		
		public var screenoffx:int, screenoffy:int;
		
		public var buttonimg:Vector.<BitmapData> = new Vector.<BitmapData>;
	  public var buttonscreen:Vector.<Bitmap> = new Vector.<Bitmap>;
		public var buttonactive:Vector.<Boolean> = new Vector.<Boolean>;
		public var buttonhighlight:Vector.<int> = new Vector.<int>;
		
		public var buttonpos:Vector.<Point> = new Vector.<Point>;
		public var currentbuttontype:Vector.<int> = new Vector.<int>;
		public var newbuttontype:Vector.<int> = new Vector.<int>;
		public var buttonstate:Vector.<int> = new Vector.<int>;
		public var buttonlerp:Vector.<int> = new Vector.<int>;
		public var buttonframe:Vector.<int> = new Vector.<int>;
		public var buttonsize:int, buttonyspacing:int, buttonxspacing:int, buttonydiff:int;
		
		public var b_gap:int = 40, b_size:int = 26;
		public var devicex:int, devicey:int;
	}
}