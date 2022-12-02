package{
	import starling.display.*;
	import starling.events.*;
	import starling.textures.*;
	import flash.geom.*;
  import flash.net.*;
	import starling.core.*;
	import starling.text.BitmapFont;
	import starling.text.TextField;
	import starling.utils.AssetManager;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	
	public class dwgraphicsclass {
		public var screenwidth:int = 320;
		public var screenheight:int = 240;
		
		public var BUTTON_BLANK:int = 0;
		public var BUTTON_MENU:int = 1;
		public var BUTTON_BACK:int = 2;
		public var BUTTON_TALK:int = 3;
		public var BUTTON_USE:int = 4;
		public var BUTTON_TELEPORT:int = 5;
		public var BUTTON_GAMECENTER:int = 6;
		public var BUTTON_CONTROLS:int = 7;
		public var BUTTON_DPAD:int = 8;
		
		public function init(_stage:Stage):void {
			starstage = _stage;
			starstage.touchable = false;
			
			backbuffer = new RenderTexture(screenwidth, screenheight, false);
			frontbuffer_meshbatch = new MeshBatch();
			towerbufferforeground_meshbatch = new MeshBatch();
			towerbufferbackground_meshbatch = new MeshBatch();
			warpzonevertical_meshbatch = new MeshBatch();
			warpzonehorizontal_meshbatch = new MeshBatch();
			towerbufferstatic_meshbatch = new MeshBatch();
			menubuffer = new RenderTexture(screenwidth, screenheight, false);
			menubuffer_image = new Image(menubuffer);
			menubuffer_image.touchable = false;
			menubuffer_image.textureSmoothing = TextureSmoothing.NONE;
			screenmenubuffer = new RenderTexture(screenwidth, screenheight, false);
			screenmenubuffer_image = new Image(screenmenubuffer);
			screenmenubuffer_image.touchable = false;
			screenmenubuffer_image.textureSmoothing = TextureSmoothing.NONE;
			
			screen = new Image(backbuffer);
			screen.touchable = false;
			screen.textureSmoothing = TextureSmoothing.BILINEAR;
			smallscreen = new Image(backbuffer);
			smallscreen.touchable = false;
			smallscreen.textureSmoothing = TextureSmoothing.NONE;
			starstage.addChild(screen);
			
			tquad = new Quad(1, 1, RGB(255, 255, 255));
			
			ttf = new TextField(320, 120, "");
			ttf.touchable = false;
			ttf.format.font = "c64";
			ttf.format.color = 0xFFFFFF;
			ttf.format.size = BitmapFont.NATIVE_SIZE;
			ttf.format.horizontalAlign = "left";
			ttf.format.verticalAlign = "top";
			tposition = new Matrix();
			
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
			
			trinketcolset = false;
			
			showcutscenebars = false;
		  cutscenebarspos = 0;
			
			flipmode = false; setflipmode = false;
			flipmatrix.scale(1, -1);
			flipmatrix.translate(0, 240);
			flipfontmatrix.scale(1, -1);	flipfontmatrix.translate(0, 8);
			flipfontmatrix2.scale(1, -1);	flipfontmatrix2.translate(0, 9);
			
			//Android optimisations
			var tbg:Array = [0x19090A, 0x190C0A, 0x190F0A, 0x191309, 0x191609, 0x191909,
											 0x171909, 0x151909, 0x131909, 0x111909, 0x0F1909, 0x0E190C,
											 0x0D190F, 0x0B1813, 0x0A1816, 0x091819, 0x0A1519, 0x0C1219,
											 0x0E0F19, 0x100C19, 0x110919, 0x130919, 0x140919, 0x160919,
											 0x170919, 0x190919, 0x190916, 0x190913, 0x190910, 0x19090D];
			for (i = 0; i < tbg.length; i++){
				tower_bgcol.push(tbg[i]);
			}
			
			tbg = [0x0A0304, 0x0A0504, 0x0A0604, 0x0A0703, 0x0A0803, 0x0A0A03, 
						 0x090A03, 0x080A03, 0x070A03, 0x070A03, 0x060A03, 0x050A05, 
						 0x050A06, 0x040907, 0x040908, 0x03090A, 0x04080A, 0x05070A, 
						 0x05060A, 0x06040A, 0x07030A, 0x07030A, 0x08030A, 0x08030A, 
						 0x09030A, 0x0A030A, 0x0A0309, 0x0A0307, 0x0A0306, 0x0A0305];
			for (i = 0; i < tbg.length; i++){
				tower_bgdarkcol.push(tbg[i]);
			}
			
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
		  
			
			//var devicexres:int = int(Capabilities.screenResolutionX);
			//var deviceyres:int = int(Capabilities.screenResolutionY);
			//updatescreen(starstage.stageWidth, starstage.stageHeight);
			
			//Starling.current.showStats = true;
		}
		
		public function initbuttonstuff():void {
			button_image_width.push(0); button_image_height.push(0);
			button_image_width.push(0); button_image_height.push(0);
			button_image_width.push(0); button_image_height.push(0);
			button_image_width.push(0); button_image_height.push(0);
			button_image.push(new Image(button_texture[0]));
			button_image.push(new Image(button_texture[1]));
			button_image.push(new Image(button_texture[2]));
			button_image.push(new Image(button_texture[3]));
			button_image[0].touchable = false; button_image[0].textureSmoothing = TextureSmoothing.NONE;
			button_image[1].touchable = false; button_image[1].textureSmoothing = TextureSmoothing.NONE;
			button_image[2].touchable = false; button_image[2].textureSmoothing = TextureSmoothing.NONE;
			button_image[3].touchable = false; button_image[3].textureSmoothing = TextureSmoothing.NONE;
			
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
			
			drawonscreenbutton(0, -1);
			drawonscreenbutton(1, -1);
			drawonscreenbutton(2, -1);
			drawonscreenbutton(3, -1);
			starstage.addChild(button_image[0]);
			starstage.addChild(button_image[1]);
			starstage.addChild(button_image[2]);
			starstage.addChild(button_image[3]);
			
			buttonsready = true;
		}
		
		public function updatescreen(w:int, h:int):void {
			starstage.stageWidth = w;
			starstage.stageHeight = h;
			
			Starling.current.viewPort = new Rectangle(0, 0, w, h);
			
			// set rectangle dimensions for viewPort:
			var stretchscalex:Number = w / screenwidth;
			var stretchscaley:Number = h / screenheight;
			screensizemultiplier = Math.min(stretchscalex, stretchscaley);
			
			//Never mess with starling's viewport for VVVVVV - instead, mess with the
			//screen image
			screen.width = screenwidth * screensizemultiplier;
			screen.height = screenheight * screensizemultiplier;
			
			screen.x = (w / 2) - (screenwidth * screensizemultiplier / 2);
		}
		
		public function initbuttonpositions():void {
		  devicex = device.xres;
			devicey = device.yres;
			
			buttonsize = devicey / (32 / 3);
			buttonyspacing = buttonsize / 3;
			buttonxspacing = (buttonsize * 3) / 3;
			
			button_image[0].width = (buttonsize * 46) / 15; button_image[0].height = buttonsize;
			button_image[1].width = (buttonsize * 46) / 15; button_image[1].height = buttonsize;
			button_image[2].width = (buttonsize * 5) / 3; button_image[2].height = (buttonsize * 5) / 3;
			button_image[3].width = (buttonsize * 5) / 3; button_image[3].height = (buttonsize * 5) / 3;
			
			button_image_width[0] = int(button_image[0].width);
			button_image_width[1] = int(button_image[1].width);
			button_image_width[2] = int(button_image[2].width);
			button_image_width[3] = int(button_image[3].width);
			button_image_height[0] = int(button_image[0].height);
			button_image_height[1] = int(button_image[1].height);
			button_image_height[2] = int(button_image[2].height);
			button_image_height[3] = int(button_image[3].height);
			
			
			buttonydiff = button_image_height[2] / 2;
			
			button_image[0].x = devicex + 1;
			button_image[0].y = 0;
			buttonpos[0].setTo(button_image[0].x, button_image[0].y);
			
			button_image[1].x = 0;
			button_image[1].y = 0;
			buttonpos[1].setTo(button_image[1].x, button_image[1].y);
			
			button_image[2].x = (buttonxspacing / 2);
			button_image[2].y = devicey - buttonsize-buttonyspacing - buttonydiff;
			buttonpos[2].setTo(button_image[2].x, button_image[2].y);
			
			button_image[3].x = (buttonxspacing/2) + buttonsize + buttonxspacing ;
			button_image[3].y = devicey - buttonsize-buttonyspacing;
			buttonpos[3].setTo(button_image[3].x, button_image[3].y);
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
						changebuttonpos(2, (buttonxspacing/2), devicey - buttonsize-buttonyspacing - buttonydiff+10);
					}else{
						changebuttonpos(2, (buttonxspacing/2), devicey - buttonsize-buttonyspacing - buttonydiff);
					}
					
					if (game.press_right) { 
						changebuttonpos(3, (buttonxspacing/2) + buttonsize + buttonxspacing, devicey - buttonsize-buttonyspacing - buttonydiff+10);
					}else{
						changebuttonpos(3, (buttonxspacing/2) + buttonsize + buttonxspacing, devicey - buttonsize-buttonyspacing - buttonydiff);
					}
				break;
			}
			
			switch(currentbuttontype[1]) {
				case BUTTON_BLANK:
					//Blank
					changebuttonframe(1, -1);
					changebuttonpos(1, 0 - ((button_image_width[1] * (buttonlerp[1])) / 100), 0);
				break;
				case BUTTON_CONTROLS:
					//Controls button
					if (game.menupage >= 30) {
						changebuttonframe(1, 9);	
					}else{
						changebuttonframe(1, 8);
					}
					if (flipmode) {
						changebuttonpos(1, 0 - ((button_image_width[1] * (buttonlerp[1])) / 100), devicey - buttonyspacing - button_image_height[1]);	
					}else {
						changebuttonpos(1, 0 - ((button_image_width[1] * (buttonlerp[1])) / 100), buttonyspacing);	
					}
					
				break;
				case BUTTON_GAMECENTER:
					changebuttonframe(1, 10);
					changebuttonpos(1, 0 - ((button_image_width[1] * (buttonlerp[1])) / 100), 5);
				break;
			}
			
			switch(currentbuttontype[0]) {
				case BUTTON_BLANK:
					//Blank
					changebuttonframe(0, -1);
					changebuttonpos(0, devicex - ((button_image_width[0] * (100-buttonlerp[0])) / 100), 0);
				break;
				case BUTTON_MENU:
					//Menu button
				  changebuttonframe(0, 0);
					changebuttonpos(0, devicex - ((button_image_width[0] * (100-buttonlerp[0])) / 100), 0);
				break;
				case BUTTON_BACK:
					//Back button
					changebuttonframe(0, 1);
					if (flipmode) {
						changebuttonpos(0, devicex - ((button_image_width[0] * (100 - buttonlerp[0])) / 100), devicey - buttonyspacing - button_image_height[0]);
					}else{
						changebuttonpos(0, devicex - ((button_image_width[0] * (100 - buttonlerp[0])) / 100), buttonyspacing);
					}
				break;
				case BUTTON_USE:
					//Use button
					if (help.slowsine % 16 > 8) {
						changebuttonframe(0, 4);
					}else {
						changebuttonframe(0, 5);
					}
					changebuttonpos(0, devicex - ((button_image_width[0] * (100-buttonlerp[0])) / 100), buttonyspacing);
				break;
				case BUTTON_TALK:
					//Talk button
					if (help.slowsine % 16 > 8) {
						changebuttonframe(0, 2);
					}else {
						changebuttonframe(0, 3);
					}
					changebuttonpos(0, devicex - ((button_image_width[0] * (100-buttonlerp[0])) / 100), buttonyspacing);
				break;
				case BUTTON_TELEPORT:
					//Teleport button
					if (help.slowsine % 16 > 8) {
						changebuttonframe(0, 6);
					}else {
						changebuttonframe(0, 7);
					}
					changebuttonpos(0, devicex - ((button_image_width[0] * (100-buttonlerp[0])) / 100), buttonyspacing);
				break;
			}
		}
		
		public function addbutton(imgname:String):void {
			var sourcetexture:Texture = starlingassets.getTexture(imgname);
			var tmpimage:Image = new Image(sourcetexture);
			
			var newtexture:RenderTexture = new RenderTexture(int(tmpimage.texture.width), int(tmpimage.texture.height));
			newtexture.draw(tmpimage);
			
			sourcetexture.dispose();
			tmpimage.dispose();
			
			button_texture.push(newtexture);
		}
		
		public function drawonscreenbutton(t:int, t2:int):void {
			//Draw button t with frame t2
			if (t2 == -1) {
				button_texture[t].clear();
			}
			if (buttonframe[t] != t2) {
				buttonframe[t] = t2;
				button_texture[t].clear();
				var tempimg:Image = new Image(button_texture[4 + t2]);
				button_texture[t].draw(tempimg);
			}
		}
		
		public function changebuttonframe(t:int, t2:int):void {
			//Draw button t with frame t2
			if (buttonframe[t] != t2) {
				buttonframe[t] = t2;
				if (t2 == -1) {
					button_texture[t].clear();
				}else{
					button_texture[t].clear();
					var tempimg:Image = new Image(button_texture[4 + t2]);
					button_texture[t].draw(tempimg);
				}
			}
		}
		
		public function changebuttonpos(t:int, xp:int, yp:int):void {
		  button_image[t].x = xp;
			button_image[t].y = yp;
		}
		
		public function changebuttonxpos(t:int, xp:int):void {
		  button_image[t].x = xp;
		}
		
		public function showarrows():void {
		  if (buttonactive[0] == false) {
				buttonhighlight[0] = 120;
				buttonactive[0] = true;
				starstage.addChild(button_image[0]);
				starstage.addChild(button_image[1]);
				starstage.addChild(button_image[2]);
			}
		}
		
		public function hidearrows():void {
		  if (buttonactive[0] == true) {
				buttonactive[0] = false;
				starstage.removeChild(button_image[0]);
				starstage.removeChild(button_image[1]);
				starstage.removeChild(button_image[2]);
			}
		}
		
		public function drawspritesetcol(x:int, y:int, t:int, c:int, help:helpclass):void {
			setcol(c, help);
			sprites[t].color = ct.color;
			tposition.identity();
			tposition.translate(x, y);
			backbuffer.draw(sprites[t], tposition);
		}
		
		public function makeentcolourarray():void {
			var sourcetexture:Texture = starlingassets.getTexture("entcolours");
			for (var j:int = 0; j < 60; j++) {
				for (var i:int = 0; i < 12; i++) {
					var tiletex:Texture = Texture.fromTexture(sourcetexture, new Rectangle(i * 8, j * 8, 8, 8));
					var t:Image = new Image(tiletex);
					t.textureSmoothing = TextureSmoothing.NONE;
					t.touchable = false;
					entcolours.push(t);
				}
			}
		}
		
		public function maketilearray():void {
			var sourcetexture:Texture = starlingassets.getTexture("tiles");
		  for (var j:int = 0; j < 30; j++) {
				for (var i:int = 0; i < 40; i++) {
					var tiletex:Texture = Texture.fromTexture(sourcetexture, new Rectangle(i * 8, j * 8, 8, 8));
					var tile:Image = new Image(tiletex);
					tile.textureSmoothing = TextureSmoothing.NONE;
					tile.touchable = false;
					tiles.push(tile);
				}
			}
		}
		
		public function maketile2array():void {
			var sourcetexture:Texture = starlingassets.getTexture("tiles2");
		  for (var j:int = 0; j < 30; j++) {
				for (var i:int = 0; i < 40; i++) {
					var tiletex:Texture = Texture.fromTexture(sourcetexture, new Rectangle(i * 8, j * 8, 8, 8));
					var tile:Image = new Image(tiletex);
					tile.textureSmoothing = TextureSmoothing.NONE;
					tile.touchable = false;
					tiles2.push(tile);
				}
			}
		}
		
		public function maketile3array():void {
		  var sourcetexture:Texture = starlingassets.getTexture("tiles3");
		  for (var j:int = 0; j < 30; j++) {
				for (var i:int = 0; i < 30; i++) {
					var tiletex:Texture = Texture.fromTexture(sourcetexture, new Rectangle(i * 8, j * 8, 8, 8));
					var tile:Image = new Image(tiletex);
					tile.textureSmoothing = TextureSmoothing.NONE;
					tile.touchable = false;
					tiles3.push(tile);
				}
			}
		}
		
		public function makespritearray():void {
		  var sourcetexture:Texture = starlingassets.getTexture("sprites");
		  for (var j:int = 0; j < 16; j++) {
				for (var i:int = 0; i < 12; i++) {
					var tiletex:Texture = Texture.fromTexture(sourcetexture, new Rectangle(i * 32, j * 32, 32, 32));
					var t:Image = new Image(tiletex);
					t.textureSmoothing = TextureSmoothing.NONE;
					t.touchable = false;
					sprites.push(t);
				}
			}
			
			//Sprites also need to load flash bitmapdatas, which are used for pixel perfect hittests
			var b:Bitmap = new img_bitmapsprites();
			var buffer:BitmapData = b.bitmapData;
			
			for (j = 0; j < 16; j++) {
				for (i = 0; i < 12; i++) {
					var tb:BitmapData = new BitmapData(32, 32, true, 0x000000);
					var temprect:Rectangle = new Rectangle(i * 32, j * 32, 32, 32);	
					tb.copyPixels(buffer, temprect, tl);
					sprites_bitmap.push(tb);
				}
			}
		}
		
		public function makeflipspritearray():void {
			var sourcetexture:Texture = starlingassets.getTexture("flipsprites");
		  for (var j:int = 0; j < 16; j++) {
				for (var i:int = 0; i < 12; i++) {
					var tiletex:Texture = Texture.fromTexture(sourcetexture, new Rectangle(i * 32, j * 32, 32, 32));
					var t:Image = new Image(tiletex);
					t.textureSmoothing = TextureSmoothing.NONE;
					t.touchable = false;
					flipsprites.push(t);
				}
			}
			
			
			//Sprites also need to load flash bitmapdatas, which are used for pixel perfect hittests
			var b:Bitmap = new img_bitmapflipsprites();
			var buffer:BitmapData = b.bitmapData;
			
			for (j = 0; j < 16; j++) {
				for (i = 0; i < 12; i++) {
					var tb:BitmapData = new BitmapData(32, 32, true, 0x000000);
					var temprect:Rectangle = new Rectangle(i * 32, j * 32, 32, 32);	
					tb.copyPixels(buffer, temprect, tl);
				  flipsprites_bitmap.push(tb);
				}
			}
		}
		
		public function maketelearray():void {
			var sourcetexture:Texture = starlingassets.getTexture("teleporter");
			for (var i:int = 0; i < 10; i++) {
			  var tiletex:Texture = Texture.fromTexture(sourcetexture, new Rectangle(i * 96, j * 96, 96, 96));
				var t:Image = new Image(tiletex);
				t.textureSmoothing = TextureSmoothing.NONE;
				t.touchable = false;
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
			trace("dwgfx.drawlevelmenu() is not implemented yet");
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
				cls(0x000000);
			}else if(fademode==3){
			  for (i = 0; i < 15; i++) {
					drawfillrect(fadebars[i], i * 16, fadeamount, 16, 0, 0, 0);
				}
			}else if(fademode==5){
			  for (i = 0; i < 15; i++) {
					drawfillrect(fadebars[i]-fadeamount, i * 16, 500, 16, 0, 0, 0);
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
			
			if (m < 20) {
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
			drawfillrect(x, y, w * 8, h * 8, r / 6, g / 6, b / 6);
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
		
		public function drawpixeltextbox(x:int, y:int, w:int, h:int, w2:int, h2:int, r:int, g:int, b:int, xo:int = 0, yo:int = 0):void {
			//given these parameters, draw a textbox with a pixel width
			drawfillrect(x, y, w, h, r / 6, g / 6, b / 6);
			
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
		
		public function drawcustompixeltextbox(x:int, y:int, w:int, h:int, w2:int, h2:int, r:int, g:int, b:int, xo:int = 0, yo:int = 0):void {
			//given these parameters, draw a textbox with a pixel width
			drawfillrect(x, y, w, h, r / 6, g / 6, b / 6);
			
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
		
		public var subtex:Texture;
		public var subimage:Image;
		public function drawpartimage(t:Texture, xp:int, yp:int, wp:int, hp:int):void {
			// Acquire SubTexture and build an Image from it.
			trect.x = 0;
			trect.y = 0;
			trect.width = wp;
			trect.height = hp;
			
			if (subtex == null) {
				subtex = Texture.fromTexture(t, trect);
				subimage = new Image(subtex); // alloc. avoidable with pooling?
				subimage.touchable = false;
				subimage.textureSmoothing = TextureSmoothing.NONE;
				
				tposition.identity();
				tposition.translate(xp, yp);
				backbuffer.draw(subimage, tposition);
				return;
			}else {
				//dispose of the old one
				//No memory leaks, but this is throwing away a texture every call, which might
				//get expensive? Ideally you'd reuse an image here if it hasn't changed.
				//Or, maybe easier, make a function that grabs the chunk we need and keeps it somewhere
				//else, hardcoding around the problem. Let's see if this is a problem in practice, though.
				subtex.dispose();
				subimage.dispose();
				
				subtex = Texture.fromTexture(t, trect);
				subimage = new Image(subtex);
				subimage.touchable = false;
				subimage.textureSmoothing = TextureSmoothing.NONE;
				
				tposition.identity();
				tposition.translate(xp, yp);
				backbuffer.draw(subimage, tposition);
				return;
			}
		}
		
		public function cutscenebars():void {
			if (showcutscenebars) {
				cutscenebarspos += 25;
				if (cutscenebarspos >= 360) cutscenebarspos = 360;
				drawfillrect(0, 0, cutscenebarspos, 16, 0, 0, 0);
				drawfillrect(360 - cutscenebarspos, 224, cutscenebarspos, 16, 0, 0, 0);
			}else {
				//disappearing
				if (cutscenebarspos > 0) {
					cutscenebarspos -= 25;
					//draw					
					drawfillrect(0, 0, cutscenebarspos, 16, 0, 0, 0);
					drawfillrect(360 - cutscenebarspos, 224, cutscenebarspos, 16, 0, 0, 0);
				}
			}
		}
		
		//For android: this function draws textboxes to a buffer first, where they're later copied to the screen
		//buffer is a 320x500 image. X coordinates aren't changed, but y coordinates are
		public var textbox_ybuffer:int;
		public var textbox_ybufferheight:int = 0;
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
					}else {
					  drawfillrect(textbox[i].textrect.x, textbox[i].textrect.y, textbox[i].textrect.width, textbox[i].textrect.height, textbox[i].r/6, textbox[i].g/6, textbox[i].b / 6);
						
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
		
		public function addmobileimage(imgname:String):void {
		  var sourcetexture:Texture = starlingassets.getTexture(imgname);
			var t:Image = new Image(sourcetexture);
			t.touchable = false;
			t.textureSmoothing = TextureSmoothing.NONE;
			mobileimages.push(t);
		}
		
		public function addimage(imgname:String):void {
		  var sourcetexture:Texture = starlingassets.getTexture(imgname);
			var t:Image = new Image(sourcetexture);
			t.touchable = false;
			t.textureSmoothing = TextureSmoothing.NONE;
			images.push(t);
		}
		
		public function addimage_rendertexture(imgname:String):void {
		  customminimap = starlingassets.getTexture(imgname) as RenderTexture;
			var t:Image = new Image(customminimap);
			t.touchable = false;
			t.textureSmoothing = TextureSmoothing.NONE;
			images.push(t);
		}
		
		public function addplayerlevelimage(imgname:String):void {
		  var sourcetexture:Texture = starlingassets.getTexture(imgname);
			var t:Image = new Image(sourcetexture);
			t.touchable = false;
			t.textureSmoothing = TextureSmoothing.NONE;
			playerlevelimages.push(t);
		}
		
		public function addbackground(imgname:String):void {
			//Don't think this is actually used in VVVVVV
			var sourcetexture:Texture = starlingassets.getTexture(imgname);
			var t:Image = new Image(sourcetexture);
			t.touchable = false;
			t.textureSmoothing = TextureSmoothing.NONE;
			backgrounds.push(t);
		}
		
		public var towerx_pix:int;
		public var towery_pix:int;
		public var tower_bgcol:Vector.<int> = new Vector.<int>;
		public var tower_bgdarkcol:Vector.<int> = new Vector.<int>;
		public var lasttowercolstate:int = -1;
		public var lasttowercolstate_front:int = -1;
		public var maptdrawfront:Boolean = true;
		public var forcetowerupdate:int = 10;
		public var forcefronttowerupdate:int = 10;
		public var towerfront_lastypos:int = 0;
		public var tower_lastypos:int = 0;
		
		public var forcewarpzonehorizontalupdate:int = 2;
		public var forcewarpzoneverticalupdate:int = 1;
		public var forcetowerstaticupdate:int = 1;
		
		public function forcescreenupdates():void {
		  if (forcetowerupdate < 3) forcetowerupdate = 3;
			if (forcefronttowerupdate < 3) forcefronttowerupdate = 3;
			if (forcetowerstaticupdate < 3) forcetowerstaticupdate = 3;
			if (forcewarpzoneverticalupdate < 1) forcewarpzoneverticalupdate = 1;
			if (forcewarpzonehorizontalupdate < 1) forcewarpzonehorizontalupdate = 1;
		}
		
		public function drawtowerbackground(map:mapclass):void {
			if (map.bypos < 0) {
				map.bypos += 120 * 8;
				forcetowerupdate += 5;
			}
			
			if (map.scrolldir == 1) {
				if (map.tdrawback || lasttowercolstate != map.colstate || forcetowerupdate > 0) {
					//Draw the whole thing; needed for every colour cycle!
					towerbufferbackground_meshbatch.y = 0;
					tower_lastypos = map.bypos;
					
					towerbufferbackground_meshbatch.clear();			
					//Draw the whole thing; needed for every colour cycle!
					for (j = 0; j < 40; j++) {
						for (i = 0; i < 40; i++) {			
							temp = map.tower.backat(i, j, map.bypos);
							drawtowertile3(i * 8, (j * 8) - (map.bypos % 8), temp, map.colstate);
						}
					}
					
					backbuffer.draw(towerbufferbackground_meshbatch);
					
					map.tdrawback = false;
					lasttowercolstate = map.colstate;
					if (forcetowerupdate > 0) forcetowerupdate--;
				}else {
					//just scroll down a bit
					towerbufferbackground_meshbatch.y = -int(map.bypos - tower_lastypos);
					backbuffer.draw(towerbufferbackground_meshbatch);
					
					if (towerbufferbackground_meshbatch.y < -80) {
						towerbufferbackground_meshbatch.y = 0;
						map.tdrawback = true;	
					}
				}
			}else{			
				if (map.tdrawback || lasttowercolstate != map.colstate || forcetowerupdate > 0) {
					//Draw the whole thing; needed for every colour cycle!
					towerbufferbackground_meshbatch.y = 0;
					tower_lastypos = map.bypos;
					
					towerbufferbackground_meshbatch.clear();			
					//Draw the whole thing; needed for every colour cycle!
					for (j = -10; j < 30; j++) {
						for (i = 0; i < 40; i++) {			
							temp = map.tower.backat(i, j, map.bypos);
							drawtowertile3(i * 8, (j * 8) - (map.bypos % 8), temp, map.colstate);
						}
					}
					
					backbuffer.draw(towerbufferbackground_meshbatch);
					
					map.tdrawback = false;
					lasttowercolstate = map.colstate;
					if (forcetowerupdate > 0) forcetowerupdate--;
				}else {
					//just scroll down a bit
					towerbufferbackground_meshbatch.y = -int(map.bypos - tower_lastypos);
					backbuffer.draw(towerbufferbackground_meshbatch);
					
					if (towerbufferbackground_meshbatch.y > 80) {
						towerbufferbackground_meshbatch.y = 0;
						map.tdrawback = true;	
					}
				}
			}
		}
		
		public function drawtowerbackgroundsolo(map:mapclass):void {
			if (map.bypos < 0) {
				map.bypos += 120 * 8;
				forcetowerupdate += 5;
			}
			
			if (map.scrolldir == 1) map.tdrawback = true;
			
			if (map.tdrawback || lasttowercolstate != map.colstate || forcetowerupdate > 0) {
				//Draw the whole thing; needed for every colour cycle!
				towerbufferbackground_meshbatch.y = 0;
				tower_lastypos = map.bypos;
				
				towerbufferbackground_meshbatch.clear();			
				//Draw the whole thing; needed for every colour cycle!
				for (j = -10; j < 31; j++) {
					for (i = 0; i < 40; i++) {			
						temp = map.tower.backat(i, j, map.bypos);
						drawtowertile3(i * 8, (j * 8) - (map.bypos % 8), temp, map.colstate);
					}
				}
				
				backbuffer.draw(towerbufferbackground_meshbatch);
				
				map.tdrawback = false;
				lasttowercolstate = map.colstate;
				if (forcetowerupdate > 0) forcetowerupdate--;
			}else {
				//just scroll down a bit
				towerbufferbackground_meshbatch.y = -int(map.bypos - tower_lastypos);
				backbuffer.draw(towerbufferbackground_meshbatch);
				
				if (towerbufferbackground_meshbatch.y > 80) {
					towerbufferbackground_meshbatch.y = 0;
				  map.tdrawback = true;	
				}
			}
		}
		
		public function drawtowermap(game:gameclass, map:mapclass):void {
			if (game.deathseq > 0) { forcefronttowerupdate = 20;	}
			
			if (map.scrolldir == 1) {
				if (maptdrawfront || lasttowercolstate_front != map.colstate || forcefronttowerupdate > 0) {
					towerbufferforeground_meshbatch.y = 0;
					towerfront_lastypos = map.ypos;
					towerbufferforeground_meshbatch.clear();
					
					for (j = 0; j < 40; j++) {
						for (i = 0; i < 40; i++) {
							temp = map.tower.at(i, j, map.ypos);
							if (temp > 0) drawtile3_batch(i * 8, (j * 8) - (map.ypos % 8), temp, map.colstate);
						}
					}
					
					backbuffer.draw(towerbufferforeground_meshbatch);
					
					maptdrawfront = false;
					lasttowercolstate_front = map.colstate;
					if (forcefronttowerupdate > 0) forcefronttowerupdate--;
				}else {
					//just scroll down a bit
					towerbufferforeground_meshbatch.y = -int(map.ypos - towerfront_lastypos);
					backbuffer.draw(towerbufferforeground_meshbatch);
					
					if (towerbufferforeground_meshbatch.y < -80) {
						towerbufferforeground_meshbatch.y = 0;
						maptdrawfront = true;	
					}
				}
			}else{
				if (maptdrawfront || lasttowercolstate_front != map.colstate || forcefronttowerupdate > 0) {
					towerbufferforeground_meshbatch.y = 0;
					towerfront_lastypos = map.ypos;
					towerbufferforeground_meshbatch.clear();
					
					for (j = -10; j < 30; j++) {
						for (i = 0; i < 40; i++) {
							temp = map.tower.at(i, j, map.ypos);
							if (temp > 0) drawtile3_batch(i * 8, (j * 8) - (map.ypos % 8), temp, map.colstate);
						}
					}
					
					backbuffer.draw(towerbufferforeground_meshbatch);
					
					maptdrawfront = false;
					lasttowercolstate_front = map.colstate;
					if (forcefronttowerupdate > 0) forcefronttowerupdate--;
				}else {
					//just scroll down a bit
					towerbufferforeground_meshbatch.y = -int(map.ypos - towerfront_lastypos);
					backbuffer.draw(towerbufferforeground_meshbatch);
					
					if (towerbufferforeground_meshbatch.y > 80) {
						towerbufferforeground_meshbatch.y = 0;
						maptdrawfront = true;	
					}
				}
			}
		}
		
		public function drawtowermap_nobackground(map:mapclass):void {
			towerbufferforeground_meshbatch.clear();
			
			for (j = 0; j < 30; j++) {
				for (i = 0; i < 40; i++) {			
					temp = map.tower.at(i, j, map.ypos);
					if (temp > 0 && temp<28) drawtile3_batch(i * 8, (j * 8) - (map.ypos % 8), temp, map.colstate);
				}
			}
			
			backbuffer.draw(towerbufferforeground_meshbatch);
		}
		
		public function drawtowerspikes(map:mapclass):void {
			for (i = 0; i < 40; i++) {			
				drawtile3(i * 8, -8 + map.spikeleveltop, 9, map.colstate);
				drawtile3(i * 8, 230 - map.spikelevelbottom, 8, map.colstate);
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
						tposition.identity();
						tposition.translate(obj.entities[i].xp, obj.entities[i].yp - map.ypos);
						
						setcol(obj.entities[i].colour, help); 
						sprites[obj.entities[i].drawframe].color = ct.color;
						backbuffer.draw(sprites[obj.entities[i].drawframe], tposition);
						//screenwrapping!
						if (!map.minitowermode) {
							if ( map.ypos >= 500 && map.ypos <= 5000) { //The "wrapping" area of the tower
								if (tpoint.x < 0) {
									tposition.identity();
									tposition.translate(obj.entities[i].xp + 320, obj.entities[i].yp - map.ypos);
									backbuffer.draw(sprites[obj.entities[i].drawframe], tposition);
								}
								if (tpoint.x > 300) {
									tposition.identity();
									tposition.translate(obj.entities[i].xp - 320, obj.entities[i].yp - map.ypos);
									backbuffer.draw(sprites[obj.entities[i].drawframe], tposition);
								}
							}
						}
					}else if (obj.entities[i].size == 1) { // Tiles						
						tposition.identity();
						tposition.translate(obj.entities[i].xp, obj.entities[i].yp - map.ypos);
						backbuffer.draw(tiles[obj.entities[i].drawframe], tposition);
					}else if (obj.entities[i].size == 2) { // Special: Moving platform, 4 tiles
						tposition.identity();
						tposition.translate(obj.entities[i].xp, obj.entities[i].yp - map.ypos);
						backbuffer.draw(tiles[obj.entities[i].drawframe], tposition);
						
						tposition.identity();
						tposition.translate(obj.entities[i].xp + 8, obj.entities[i].yp - map.ypos);
						backbuffer.draw(tiles[obj.entities[i].drawframe], tposition);
						
						tposition.identity();
						tposition.translate(obj.entities[i].xp + 16, obj.entities[i].yp - map.ypos);
						backbuffer.draw(tiles[obj.entities[i].drawframe], tposition);
						
						tposition.identity();
						tposition.translate(obj.entities[i].xp + 24, obj.entities[i].yp - map.ypos);
					}else if (obj.entities[i].size == 3) { // Big chunky pixels!
					  prect.x = obj.entities[i].xp; prect.y = obj.entities[i].yp-map.ypos;
						//A seperate index of colours, for simplicity
						if (obj.entities[i].colour == 1) {
							drawfillrect(prect.x, prect.y, prect.width, prect.height, 196 - (Math.random() * 64), 10, 10);
						}else if (obj.entities[i].colour == 2) {
							drawfillrect(prect.x, prect.y, prect.width, prect.height, 160- help.glow/2 - (Math.random()*20), 200- help.glow/2, 220 - help.glow);
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
					drawfillrect(0, 0, 320, 240, 0, 0, 0);
					for (i = 0; i < 50; i++) {
						if (starsspeed[i] <= 6) {
							drawfillrect(stars[i].x, stars[i].y, stars[i].width, stars[i].height, 34, 34, 34);
						}else {
							drawfillrect(stars[i].x, stars[i].y, stars[i].width, stars[i].height, 85, 85, 85);
						}
						stars[i].x -= starsspeed[i];
						if (stars[i].x < -10) {
							stars[i].x += 340;
							stars[i].y = Math.random() * 240;
							starsspeed[i] = 4 + (Math.random()*4);
						}
					}
				break;
				case 2:
					//Lab
					i = 0;
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
								case 0: bcol2 = RGB(0, 16 * backboxint[i], 16 * backboxint[i]);	break; //Cyan
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
					cls(bcol2);
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
						drawfillrect(backboxes[i].x, backboxes[i].y, backboxes[i].width, backboxes[i].height, bcol);	
						
						backboxrect.x = backboxes[i].x + 1; backboxrect.y = backboxes[i].y + 1;
						backboxrect.width = backboxes[i].width - 2; backboxrect.height = backboxes[i].height - 2;
						drawfillrect(backboxrect.x, backboxrect.y, backboxrect.width, backboxrect.height, bcol2);
						
						backboxes[i].x += backboxvx[i];
						backboxes[i].y += backboxvy[i];
						if (backboxes[i].x < -40) { backboxes[i].x = 320; backboxes[i].y = Math.random() * 240; }
					  if (backboxes[i].x > 320) { backboxes[i].x = -32; backboxes[i].y = Math.random() * 240; }
						if (backboxes[i].y < -40) { backboxes[i].y = 240; backboxes[i].x = Math.random() * 320; }
						if (backboxes[i].y > 260) { backboxes[i].y = -32; backboxes[i].x = Math.random() * 320; }
					}
				break;
				case 3: //Warp zone (horizontal)
					backoffset += 3; if (backoffset >= 16) backoffset -= 16;
					
					if (forcewarpzonehorizontalupdate > 0) {
						warpzonehorizontal_meshbatch.x = 0;	
						warpzonehorizontal_meshbatch.clear();
						for (j = 0; j < 15; j++) {
							for (i = -1; i < 21; i++) {			
								temp = 680 + (rcol * 3);
								tposition.identity();	tposition.translate(int((i * 16) - backoffset), int(j * 16));
								warpzonehorizontal_meshbatch.addMesh(tiles2[temp+40], tposition);
								tposition.identity();	tposition.translate(int((i * 16) - backoffset + 8), int((j * 16)));
								warpzonehorizontal_meshbatch.addMesh(tiles2[temp + 41], tposition);
								tposition.identity();	tposition.translate(int((i * 16) - backoffset), int((j * 16) + 8));
								warpzonehorizontal_meshbatch.addMesh(tiles2[temp + 80], tposition);
								tposition.identity();	tposition.translate(int((i * 16) - backoffset + 8), int((j * 16) + 8));
								warpzonehorizontal_meshbatch.addMesh(tiles2[temp + 81], tposition);
							}
						}
						
						backbuffer.draw(warpzonehorizontal_meshbatch);
						forcewarpzonehorizontalupdate--;
					}else {
						warpzonehorizontal_meshbatch.x = backoffset;
						backbuffer.draw(warpzonehorizontal_meshbatch);
					}
					
				break;
				case 4: //Warp zone (vertical)
					backoffset += 3; 
					if (backoffset >= 16) backoffset -= 16;
					
				  if (forcewarpzoneverticalupdate > 0) {
						warpzonevertical_meshbatch.y = 0;	
						warpzonevertical_meshbatch.clear();
						for (j = -1; j < 16; j++) {
							for (i = 0; i < 21; i++) {			
								temp = 760 + (rcol * 3);
								tposition.identity();	tposition.translate(int((i * 16)), int((j * 16) - backoffset));
								warpzonevertical_meshbatch.addMesh(tiles2[temp+40], tposition);
								tposition.identity();	tposition.translate(int((i * 16) + 8), int((j * 16) - backoffset));
								warpzonevertical_meshbatch.addMesh(tiles2[temp + 41], tposition);
								tposition.identity();	tposition.translate(int((i * 16)), int((j * 16) - backoffset + 8));
								warpzonevertical_meshbatch.addMesh(tiles2[temp + 80], tposition);
								tposition.identity();	tposition.translate(int((i * 16) + 8), int((j * 16) - backoffset + 8));
								warpzonevertical_meshbatch.addMesh(tiles2[temp + 81], tposition);
							}
						}
						
						backbuffer.draw(warpzonevertical_meshbatch);
						forcewarpzoneverticalupdate--;
					}else {
						warpzonevertical_meshbatch.y = backoffset;
						backbuffer.draw(warpzonevertical_meshbatch);
					}
					
					//backbuffer.copyPixels(towerbuffer, towerbuffer.rect, tl);
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
					
					for (i = 10; i >= 0; i--) {
						temp = (i << 4) + backoffset;
						setwarprect(160 - temp, 120 - temp, temp * 2, temp * 2);
						if (i % 2 == warpskip) {
							drawfillrect(warprect.x, warprect.y, warprect.width, warprect.height, warpbcol);
						}else {
							drawfillrect(warprect.x, warprect.y, warprect.width, warprect.height, warpfcol);
						}
					}
				break;
				case 6:
				  //Final Starfield
					cls(0x000000);
					for (i = 0; i < 50; i++) {
						if (starsspeed[i] <= 8) {
							drawfillrect(stars[i].x, stars[i].y, stars[i].width, stars[i].height, 0x222222);
						}else {
							drawfillrect(stars[i].x, stars[i].y, stars[i].width, stars[i].height, 0x555555);
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
					if (forcetowerstaticupdate > 0) {
						towerbufferstatic_meshbatch.clear();
						
						for (j = 0; j < 30; j++) { 
							for (i = 0; i < 40; i++) {	
								tposition.identity();
								tposition.translate(int(i * 8), int(j * 8));
								towerbufferstatic_meshbatch.addMesh(tiles3[map.tower.backat(i, j, 200) + (15 * 30)], tposition);
							}	
						}
						
						backbuffer.draw(towerbufferstatic_meshbatch);
						forcetowerstaticupdate--;
					}else {
						backbuffer.draw(towerbufferstatic_meshbatch);
					}
				break;
				case 8:
				  //Static, unscrolling section of the tower
					if (forcetowerstaticupdate > 0) {
						towerbufferstatic_meshbatch.clear();
						
						for (j = 0; j < 30; j++) { 
							for (i = 0; i < 40; i++) {	
								tposition.identity();
								tposition.translate(int(i * 8), int(j * 8));
								towerbufferstatic_meshbatch.addMesh(tiles3[map.tower.backat(i, j, 200) + (15 * 10)], tposition);
							}	
						}
						
						backbuffer.draw(towerbufferstatic_meshbatch);
						forcetowerstaticupdate--;
					}else {
						backbuffer.draw(towerbufferstatic_meshbatch);
					}
				break;
				case 9:
				  //Static, unscrolling section of the tower
					if (forcetowerstaticupdate > 0) {
						towerbufferstatic_meshbatch.clear();
						
						for (j = 0; j < 30; j++) { 
							for (i = 0; i < 40; i++) {	
								tposition.identity();
								tposition.translate(int(i * 8), int(j * 8));
								towerbufferstatic_meshbatch.addMesh(tiles3[map.tower.backat(i, j, 600)], tposition);
							}	
						}
						
						backbuffer.draw(towerbufferstatic_meshbatch);
						forcetowerstaticupdate--;
					}else {
						backbuffer.draw(towerbufferstatic_meshbatch);
					}
				break;
				default:
					drawfillrect(0, 0, 320, 240, 0, 0, 0);
				break;
			}
		}
		
		public function textbox_drawimage(t:int, xp:int, yp:int, cent:Boolean=false):void {
			trace("dwgfx.textbox_drawimage() is not implemented yet");
		}
		
		public function drawimage(t:int, xp:int, yp:int, cent:Boolean=false):void {
			if (cent) {
				tposition.identity();
				tposition.translate(160 - int(images[t].width / 2), yp);
			  backbuffer.draw(images[t], tposition);
			}else {
				tposition.identity();
				tposition.translate(xp, yp);
				backbuffer.draw(images[t], tposition);
			}
		}
		
		public function drawmobileimage(t:int, xp:int, yp:int, cent:Boolean=false):void {
			if (cent) {
				tposition.identity();
				tposition.translate(160 - int(mobileimages[t].width / 2), yp);
			  backbuffer.draw(mobileimages[t], tposition);
			}else {
				tposition.identity();
				tposition.translate(xp, yp);
				backbuffer.draw(mobileimages[t], tposition);
			}
		}
		
		public function drawmobilehands(t:int, xp:int, yp:int):void {
			tposition.identity();
			tposition.translate(xp, yp);
			backbuffer.draw(mobileimages[t], tposition);
		}
		
		public function drawplayerlevelimage(t:int, xp:int, yp:int):void {
			tposition.identity();
			tposition.translate(xp, yp);
			backbuffer.draw(playerlevelimages[t], tposition);
		}
		
		public function drawimagecol(t:int, xp:int, yp:int, r:int, g:int, b:int, cent:Boolean = false):void {
			if (cent) {
				tposition.identity();
				tposition.translate(160 - int(images[t].width / 2), yp);
				images[t].color = RGB(r, g, b);
			  backbuffer.draw(images[t], tposition);
			}else {
				tposition.identity();
				tposition.translate(xp, yp);
				images[t].color = RGB(r, g, b);
				backbuffer.draw(images[t], tposition);
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
		
		public function textbox_drawcoloredtile(x:int, y:int, t:int, r:int, g:int, b:int):void {
			trace("dwgfx.textbox_drawcoloredtile() is not implemented yet");
		}
		
		public function drawcoloredtile(x:int, y:int, t:int, r:int, g:int, b:int):void {
			tposition.identity();
			tposition.translate(x, y);
			
			tiles[t].color = RGB(r, g, b); 
			backbuffer.draw(tiles[t], tposition);
		}
		
		public function drawhuetile(x:int, y:int, t:int, c:int):void {
			tposition.identity();
			tposition.translate(x, y);
			switch(c) {
				case 0: tiles[t].color = RGB(250-(Math.random()*32), 250-(Math.random()*32), 10); break;
				case 1: tiles[t].color = RGB(250-(Math.random()*32), 250-(Math.random()*32), 10); break;
				
				default: tiles[t].color = RGB(250-(Math.random()*32), 250-(Math.random()*32), 10); break;
			}
			
			backbuffer.draw(tiles[t], tposition);
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
			sprites[t].color = RGB(r, g, b);
			tposition.identity();
			tposition.translate(x, y);
			backbuffer.draw(sprites[t], tposition);
		}
		
		public function textbox_drawsprite(x:int, y:int, t:int, r:int, g:int, b:int):void {
			trace("dwgfx.textbox_drawsprite() is not implemented yet");
		}
		
		public function drawgravityline(t:int, obj:entityclass):void {
			if (obj.entities[t].life == 0) {
				switch(linestate) {
					case 0: drawfillrect(line_rect.x, line_rect.y, line_rect.width, line_rect.height, 200 - 20, 200 - 20, 200 - 20); break;
					case 1: drawfillrect(line_rect.x, line_rect.y, line_rect.width, line_rect.height, 225 - 30, 245 - 30, 245 - 30); break;
					case 2: drawfillrect(line_rect.x, line_rect.y, line_rect.width, line_rect.height, 245 - 30, 245 - 30, 225 - 30); break;
					case 3: drawfillrect(line_rect.x, line_rect.y, line_rect.width, line_rect.height, 164 - 10, 200 - 20, 200 - 20); break;
					case 4: drawfillrect(line_rect.x, line_rect.y, line_rect.width, line_rect.height, 224 - 20, 255 - 30, 196 - 20); break;
					case 5: drawfillrect(line_rect.x, line_rect.y, line_rect.width, line_rect.height, 205 - 20, 235 - 30, 196 - 20); break;
					case 6: drawfillrect(line_rect.x, line_rect.y, line_rect.width, line_rect.height, 164 - 10, 164 - 10, 164 - 10); break;
					case 7: drawfillrect(line_rect.x, line_rect.y, line_rect.width, line_rect.height, 225 - 30, 245 - 30, 205 - 20); break;
					case 8: drawfillrect(line_rect.x, line_rect.y, line_rect.width, line_rect.height, 205 - 20, 255 - 30, 225 - 30); break;
					case 9: drawfillrect(line_rect.x, line_rect.y, line_rect.width, line_rect.height, 245 - 30, 245 - 30, 245 - 30); break;
				}
			}else{
				drawfillrect(line_rect.x, line_rect.y, line_rect.width, line_rect.height, 96, 96, 96);
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
							tposition.identity();
							tposition.translate(int(obj.entities[i].xp), int(obj.entities[i].yp));
							setcol(obj.entities[i].colour, help); 
							flipsprites[obj.entities[i].drawframe].color = ct.color;
							backbuffer.draw(flipsprites[obj.entities[i].drawframe], tposition);
							if (map.warpx) {
								//screenwrapping!
								if (tpoint.x < 0) {
									tposition.identity();
									tposition.translate(int(obj.entities[i].xp) + 320, int(obj.entities[i].yp));
									backbuffer.draw(flipsprites[obj.entities[i].drawframe], tposition);
								}
								if (tpoint.x > 300) {
									tposition.identity();
									tposition.translate(int(obj.entities[i].xp) - 320, int(obj.entities[i].yp));
									backbuffer.draw(flipsprites[obj.entities[i].drawframe], tposition);
								}
							}else if (map.warpy) {
								if (tpoint.y < 0) {
									tposition.identity();
									tposition.translate(int(obj.entities[i].xp), int(obj.entities[i].yp) + 230);
									backbuffer.draw(flipsprites[obj.entities[i].drawframe], tposition);
								}
								if (tpoint.y > 210) {
									tposition.identity();
									tposition.translate(int(obj.entities[i].xp), int(obj.entities[i].yp) - 230);
									backbuffer.draw(flipsprites[obj.entities[i].drawframe], tposition);
								}
							}
						}else {
							tposition.identity();
							tposition.translate(int(obj.entities[i].xp), int(obj.entities[i].yp));
							setcol(obj.entities[i].colour, help); 
							sprites[obj.entities[i].drawframe].color = ct.color;
							backbuffer.draw(sprites[obj.entities[i].drawframe], tposition);
							if (map.warpx) {
								//screenwrapping!
								if (tpoint.x < 0) {
									tposition.identity();
									tposition.translate(int(obj.entities[i].xp) + 320, int(obj.entities[i].yp));
									backbuffer.draw(sprites[obj.entities[i].drawframe], tposition);
								}
								if (tpoint.x > 300) {
									tposition.identity();
									tposition.translate(int(obj.entities[i].xp) - 320, int(obj.entities[i].yp));
									backbuffer.draw(sprites[obj.entities[i].drawframe], tposition);
								}
							}else if (map.warpy) {
								if (tpoint.y < 0) {
									tposition.identity();
									tposition.translate(int(obj.entities[i].xp), int(obj.entities[i].yp) + 230);
									backbuffer.draw(sprites[obj.entities[i].drawframe], tposition);
								}
								if (tpoint.y > 210) {
									tposition.identity();
									tposition.translate(int(obj.entities[i].xp), int(obj.entities[i].yp) - 230);
									backbuffer.draw(sprites[obj.entities[i].drawframe], tposition);
								}
							}
						}
					}else if (obj.entities[i].size == 1) { // Tiles
						tposition.identity();
						tposition.translate(int(obj.entities[i].xp), int(obj.entities[i].yp));
						backbuffer.draw(tiles[obj.entities[i].drawframe], tposition);
					}else if (obj.entities[i].size == 2) { // Special: Moving platform, 4 tiles
					  tposition.identity();	tposition.translate(int(obj.entities[i].xp), int(obj.entities[i].yp));
						if (map.custommode) {
							backbuffer.draw(entcolours[obj.entities[i].drawframe], tposition);	
							tposition.identity();	tposition.translate(int(obj.entities[i].xp) + 8, int(obj.entities[i].yp));
							backbuffer.draw(entcolours[obj.entities[i].drawframe], tposition);	
							tposition.identity();	tposition.translate(int(obj.entities[i].xp) + 16, int(obj.entities[i].yp));
							backbuffer.draw(entcolours[obj.entities[i].drawframe], tposition)
							tposition.identity();	tposition.translate(int(obj.entities[i].xp) + 24, int(obj.entities[i].yp));
							backbuffer.draw(entcolours[obj.entities[i].drawframe], tposition)
						}else {
							backbuffer.draw(tiles[obj.entities[i].drawframe], tposition);	
							tposition.identity();	tposition.translate(int(obj.entities[i].xp) + 8, int(obj.entities[i].yp));
							backbuffer.draw(tiles[obj.entities[i].drawframe], tposition);	
							tposition.identity();	tposition.translate(int(obj.entities[i].xp) + 16, int(obj.entities[i].yp));
							backbuffer.draw(tiles[obj.entities[i].drawframe], tposition)
							tposition.identity();	tposition.translate(int(obj.entities[i].xp) + 24, int(obj.entities[i].yp));
							backbuffer.draw(tiles[obj.entities[i].drawframe], tposition)
						}
					}else if (obj.entities[i].size == 3) { // Big chunky pixels!
					  prect.x = int(obj.entities[i].xp); prect.y = int(obj.entities[i].yp);
						//A seperate index of colours, for simplicity
						if (obj.entities[i].colour == 1) {
							drawfillrect(prect.x, prect.y, prect.width, prect.height, 196 - (Math.random() * 64), 10, 10);
						}else if (obj.entities[i].colour == 2) {
							drawfillrect(prect.x, prect.y, prect.width, prect.height, 160 - help.glow / 2 - (Math.random() * 20), 200 - help.glow / 2, 220 - help.glow);
						}
					}else if (obj.entities[i].size == 4) { // Small pickups
					  drawhuetile(int(obj.entities[i].xp), int(obj.entities[i].yp), obj.entities[i].tile, obj.entities[i].colour);
					}else if (obj.entities[i].size == 5) { //Horizontal Line
						line_rect.x = int(obj.entities[i].xp); line_rect.y = int(obj.entities[i].yp);
						line_rect.width = obj.entities[i].w; line_rect.height = 1;
						drawgravityline(i, obj);
					}else if (obj.entities[i].size == 6) { //Vertical Line
						line_rect.x = int(obj.entities[i].xp); line_rect.y = int(obj.entities[i].yp);
						line_rect.width = 1; line_rect.height = obj.entities[i].h;
						drawgravityline(i, obj);
					}else if (obj.entities[i].size == 7) { //Teleporter
					  drawtele(int(obj.entities[i].xp), int(obj.entities[i].yp), obj.entities[i].drawframe, obj.entities[i].colour, help);
					}else if (obj.entities[i].size == 8) { // Special: Moving platform, 8 tiles
					  tposition.identity();	tposition.translate(int(obj.entities[i].xp), int(obj.entities[i].yp));
						if (map.custommode) {
							backbuffer.draw(entcolours[obj.entities[i].drawframe], tposition);	
							tposition.identity();	tposition.translate(int(obj.entities[i].xp + 8), int(obj.entities[i].yp));
							backbuffer.draw(entcolours[obj.entities[i].drawframe], tposition);	
							tposition.identity();	tposition.translate(int(obj.entities[i].xp + 16), int(obj.entities[i].yp));
							backbuffer.draw(entcolours[obj.entities[i].drawframe], tposition);	
							tposition.identity();	tposition.translate(int(obj.entities[i].xp + 24), int(obj.entities[i].yp));
							backbuffer.draw(entcolours[obj.entities[i].drawframe], tposition);	
							tposition.identity();	tposition.translate(int(obj.entities[i].xp + 32), int(obj.entities[i].yp));
							backbuffer.draw(entcolours[obj.entities[i].drawframe], tposition);	
							tposition.identity();	tposition.translate(int(obj.entities[i].xp + 40), int(obj.entities[i].yp));
							backbuffer.draw(entcolours[obj.entities[i].drawframe], tposition);	
							tposition.identity();	tposition.translate(int(obj.entities[i].xp + 48), int(obj.entities[i].yp));
							backbuffer.draw(entcolours[obj.entities[i].drawframe], tposition);	
							tposition.identity();	tposition.translate(int(obj.entities[i].xp + 56), int(obj.entities[i].yp));
							backbuffer.draw(entcolours[obj.entities[i].drawframe], tposition);	
						}else{
							backbuffer.draw(tiles[obj.entities[i].drawframe], tposition);	
							tposition.identity();	tposition.translate(int(obj.entities[i].xp + 8), int(obj.entities[i].yp));
							backbuffer.draw(tiles[obj.entities[i].drawframe], tposition);	
							tposition.identity();	tposition.translate(int(obj.entities[i].xp + 16), int(obj.entities[i].yp));
							backbuffer.draw(tiles[obj.entities[i].drawframe], tposition);	
							tposition.identity();	tposition.translate(int(obj.entities[i].xp + 24), int(obj.entities[i].yp));
							backbuffer.draw(tiles[obj.entities[i].drawframe], tposition);	
							tposition.identity();	tposition.translate(int(obj.entities[i].xp + 32), int(obj.entities[i].yp));
							backbuffer.draw(tiles[obj.entities[i].drawframe], tposition);	
							tposition.identity();	tposition.translate(int(obj.entities[i].xp + 40), int(obj.entities[i].yp));
							backbuffer.draw(tiles[obj.entities[i].drawframe], tposition);	
							tposition.identity();	tposition.translate(int(obj.entities[i].xp + 48), int(obj.entities[i].yp));
							backbuffer.draw(tiles[obj.entities[i].drawframe], tposition);	
							tposition.identity();	tposition.translate(int(obj.entities[i].xp + 56), int(obj.entities[i].yp));
							backbuffer.draw(tiles[obj.entities[i].drawframe], tposition);	
						}
					}else if (obj.entities[i].size == 9) {      // Really Big Sprite! (2x2)
						if (flipmode) {
							setcol(obj.entities[i].colour, help);
							
							tposition.identity();
							tposition.translate(int(obj.entities[i].xp), int(obj.entities[i].yp));
							flipsprites[obj.entities[i].drawframe].color = ct.color;
							backbuffer.draw(flipsprites[obj.entities[i].drawframe], tposition);
							
							tposition.identity();
							tposition.translate(int(obj.entities[i].xp) + 32, int(obj.entities[i].yp));
							flipsprites[obj.entities[i].drawframe + 1].color = ct.color;
							backbuffer.draw(flipsprites[obj.entities[i].drawframe + 1], tposition);
							
							tposition.identity();
							tposition.translate(int(obj.entities[i].xp), int(obj.entities[i].yp) + 32);
							flipsprites[obj.entities[i].drawframe + 12].color = ct.color;
							backbuffer.draw(flipsprites[obj.entities[i].drawframe + 12], tposition);
							
							tposition.identity();
							tposition.translate(int(obj.entities[i].xp) + 32, int(obj.entities[i].yp) + 32);
							flipsprites[obj.entities[i].drawframe + 13].color = ct.color;
							backbuffer.draw(flipsprites[obj.entities[i].drawframe + 13], tposition);							
						}else{
							setcol(obj.entities[i].colour, help);
							
							tposition.identity();
							tposition.translate(int(obj.entities[i].xp), int(obj.entities[i].yp));
							sprites[obj.entities[i].drawframe].color = ct.color;
							backbuffer.draw(sprites[obj.entities[i].drawframe], tposition);
							
							tposition.identity();
							tposition.translate(int(obj.entities[i].xp) + 32, int(obj.entities[i].yp));
							sprites[obj.entities[i].drawframe + 1].color = ct.color;
							backbuffer.draw(sprites[obj.entities[i].drawframe + 1], tposition);
							
							tposition.identity();
							tposition.translate(int(obj.entities[i].xp), int(obj.entities[i].yp) + 32);
							sprites[obj.entities[i].drawframe + 12].color = ct.color;
							backbuffer.draw(sprites[obj.entities[i].drawframe + 12], tposition);
							
							tposition.identity();
							tposition.translate(int(obj.entities[i].xp) + 32, int(obj.entities[i].yp) + 32);
							sprites[obj.entities[i].drawframe + 13].color = ct.color;
							backbuffer.draw(sprites[obj.entities[i].drawframe + 13], tposition);		
						}
					}else if (obj.entities[i].size == 10) {      // 2x1 Sprite
						if (flipmode) {
							setcol(obj.entities[i].colour, help);
							
							tposition.identity();
							tposition.translate(int(obj.entities[i].xp), int(obj.entities[i].yp));
							flipsprites[obj.entities[i].drawframe].color = ct.color;
							backbuffer.draw(flipsprites[obj.entities[i].drawframe], tposition);
							
							tposition.identity();
							tposition.translate(int(obj.entities[i].xp) + 32, int(obj.entities[i].yp));
							flipsprites[obj.entities[i].drawframe + 1].color = ct.color;
							backbuffer.draw(flipsprites[obj.entities[i].drawframe + 1], tposition);
						}else{
							setcol(obj.entities[i].colour, help);
							
							tposition.identity();
							tposition.translate(int(obj.entities[i].xp), int(obj.entities[i].yp));
							sprites[obj.entities[i].drawframe].color = ct.color;
							backbuffer.draw(sprites[obj.entities[i].drawframe], tposition);
							
							tposition.identity();
							tposition.translate(int(obj.entities[i].xp) + 32, int(obj.entities[i].yp));
							sprites[obj.entities[i].drawframe + 1].color = ct.color;
							backbuffer.draw(sprites[obj.entities[i].drawframe + 1], tposition);
						}
					}else if (obj.entities[i].size == 11) { //The fucking elephant
						setcol(obj.entities[i].colour, help);
						images[3].color = ct.color;
					  drawimage(3, int(obj.entities[i].xp), int(obj.entities[i].yp));
					}else if (obj.entities[i].size == 12) {      // Regular sprites that don't wrap
						if (flipmode) {
							//forget this for a minute; 
							tpoint.x = int(obj.entities[i].xp); tpoint.y = int(obj.entities[i].yp);
							setcol(obj.entities[i].colour, help); 
							flipsprites[obj.entities[i].drawframe].color = ct.color;
							tposition.identity();
							tposition.translate(tpoint.x, tpoint.y);
							backbuffer.draw(flipsprites[obj.entities[i].drawframe], tposition);
							
							//if we're outside the screen, we need to draw indicators
							if (obj.entities[i].xp < -20 && obj.entities[i].vx > 0) {
								if (obj.entities[i].xp < -100) {
									tpoint.x = -5 + (int(( -obj.entities[i].xp) / 10)); 
								}else{
								  tpoint.x = 5; 
								}
								
								tpoint.y = tpoint.y+4;
								setcol(23, help); 
							  tiles[1167].color = ct.color;
								tposition.identity();
								tposition.translate(tpoint.x, tpoint.y);
								
								backbuffer.draw(tiles[1167], tposition);
							}else if (obj.entities[i].xp > 340 && obj.entities[i].vx < 0) {
								if (obj.entities[i].xp > 420) {
									tpoint.x = 320 - (int(( obj.entities[i].xp-320) / 10)); 
								}else{
								  tpoint.x = 310; 
								}
								
								tpoint.y = tpoint.y+4;
								setcol(23, help); 
							  tiles[1166].color = ct.color;
								
								tposition.identity();
								tposition.translate(tpoint.x, tpoint.y);
								
								backbuffer.draw(tiles[1166], tposition);
							}
						}else{
							tpoint.x = int(obj.entities[i].xp); tpoint.y = int(obj.entities[i].yp);
							setcol(obj.entities[i].colour, help); 
							sprites[obj.entities[i].drawframe].color = ct.color;
							tposition.identity();
							tposition.translate(tpoint.x, tpoint.y);
							backbuffer.draw(sprites[obj.entities[i].drawframe], tposition);
							
							//if we're outside the screen, we need to draw indicators
							
							if (obj.entities[i].xp < -20 && obj.entities[i].vx > 0) {
								if (obj.entities[i].xp < -100) {
									tpoint.x = -5 + (int(( -obj.entities[i].xp) / 10)); 
								}else{
								  tpoint.x = 5; 
								}
								
								tpoint.y = tpoint.y+4;
								setcol(23, help); 
							  tiles[1167].color = ct.color;
								tposition.identity();
								tposition.translate(tpoint.x, tpoint.y);
								
								backbuffer.draw(tiles[1167], tposition);
							}else if (obj.entities[i].xp > 340 && obj.entities[i].vx < 0) {
								if (obj.entities[i].xp > 420) {
									tpoint.x = 320 - (int(( obj.entities[i].xp-320) / 10)); 
								}else{
								  tpoint.x = 310; 
								}
								
								tpoint.y = tpoint.y+4;
								setcol(23, help); 
							  tiles[1166].color = ct.color;
								
								tposition.identity();
								tposition.translate(tpoint.x, tpoint.y);
								
								backbuffer.draw(tiles[1166], tposition);
							}
						}
					}else if (obj.entities[i].size == 13) {      // Special for epilogue: huge hero!
						if (flipmode) {
							tposition.identity();
							tposition.scale(6, 6);
							tposition.translate(int(obj.entities[i].xp), int(obj.entities[i].yp));
							
							setcol(obj.entities[i].colour, help); 
							flipsprites[obj.entities[i].drawframe].color = ct.color;
							backbuffer.draw(flipsprites[obj.entities[i].drawframe], tposition);
						}else {
							tposition.identity();
							tposition.scale(6, 6);
							tposition.translate(int(obj.entities[i].xp), int(obj.entities[i].yp));
							
							setcol(obj.entities[i].colour, help); 
							sprites[obj.entities[i].drawframe].color = ct.color;
							backbuffer.draw(sprites[obj.entities[i].drawframe], tposition);
						}
					}
				}
			}
		}
		
		public function drawbuffertile(x:int, y:int, t:int):void {
			trace("dwgfx.drawbuffertile() is not implemented yet");
		}
		
		public function drawforetile(x:int, y:int, t:int):void {
			tposition.identity();
			tposition.translate(x, y);
			frontbuffer_meshbatch.addMesh(tiles[t], tposition);
		}
		
		public function drawforetile2(x:int, y:int, t:int):void {
			tposition.identity();
			tposition.translate(x, y);
			frontbuffer_meshbatch.addMesh(tiles2[t], tposition);
		}
		
		public function drawforetile3(x:int, y:int, t:int, off:int):void {
			tposition.identity();
			tposition.translate(x, y);
			frontbuffer_meshbatch.addMesh(tiles3[t+(off*30)], tposition);
		}
		
		public function drawtele(x:int, y:int, t:int, c:int, help:helpclass):void {
			tele[0].color = RGB(16, 16, 16);
			tposition.identity();
			tposition.translate(x, y);
			backbuffer.draw(tele[0], tposition);
			
			setcol(c, help);
			if (t > 9) t = 8; if (t < 0) t = 0;
			tele[t].color = ct.color;
			
			tposition.identity();
			tposition.translate(x, y);
			backbuffer.draw(tele[t], tposition);
		}
		
		public function drawtile(x:int, y:int, t:int):void {
			tposition.identity();
			tposition.translate(x, y);
			backbuffer.draw(tiles[t], tposition);
		}
		
		public function drawtile2(x:int, y:int, t:int):void {
			tposition.identity();
			tposition.translate(x, y);
			backbuffer.draw(tiles2[t], tposition);
		}
		
		public function drawtile3(x:int, y:int, t:int, off:int):void {
			tposition.identity();
			tposition.translate(x, y);
			backbuffer.draw(tiles3[t + (off * 30)], tposition);
		}
		
		public function drawtile3_batch(x:int, y:int, t:int, off:int):void {
			tposition.identity();
			tposition.translate(x, y);
			towerbufferforeground_meshbatch.addMesh(tiles3[t + (off * 30)], tposition);
		}
		
		public function drawtowertile3(x:int, y:int, t:int, off:int):void {
			tposition.identity();
			tposition.translate(x, y);
			towerbufferbackground_meshbatch.addMesh(tiles3[t+(off*30)], tposition);
		}
		
		public function drawtowertile(x:int, y:int, t:int):void {
			tposition.identity();
			tposition.translate(x, y);
			backbuffer.draw(tiles2[t], tposition);
		}
		
		public function drawmap(map:mapclass):void {
			if (!foregrounddrawn) {
				frontbuffer_meshbatch.clear();
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
				foregrounddrawn = true;
			}
			
			backbuffer.draw(frontbuffer_meshbatch);
		}
		
		public function drawfinalmap(map:mapclass):void {
			//Update colour cycling for final level
			if (map.final_colormode) {
				map.final_aniframedelay--;
				if (map.final_aniframedelay == 0) foregrounddrawn = false;
				if (map.final_aniframedelay <= 0) {
					map.final_aniframedelay = 2;
					map.final_aniframe++;
					if (map.final_aniframe >= 4) map.final_aniframe = 0;
				}
			}
			
			if (!foregrounddrawn) {
				frontbuffer_meshbatch.clear();
				if(map.tileset==0){
					for (j = 0; j < 29 + map.extrarow; j++) {
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
			
			backbuffer.draw(frontbuffer_meshbatch);
		}
		
		public function drawmaptileat(xp:int, yp:int, mapx:int, mapy:int, map:mapclass):void {
			//Not used anymore, phew
			trace("dwgfx.drawmaptileat() is not implemented yet");
		}
		
		public function drawminimap(game:gameclass, map:mapclass):void {
			//Not used anymore, phew
			trace("dwgfx.drawminimap() is not implemented yet");
		}
		
		public function drawrect(x:int, y:int, w:int, h:int, r:int, g:int, b:int):void {
			//Draw the retangle indicated by that object
			tquad.x = x; tquad.y = y;
			tquad.width = w; tquad.height = 1;
			tquad.color = RGB(r, g, b);
			backbuffer.draw(tquad);
			
			tquad.width = 1; tquad.height = h;
			backbuffer.draw(tquad);
			
			tquad.x = x + w - 1;
			backbuffer.draw(tquad);
			
			tquad.x = x; tquad.y = y + h - 1;
			tquad.width = w; tquad.height = 1;
			backbuffer.draw(tquad);
		}
		
		public function cls(c:int):void {
		  drawfillrect(0, 0, screenwidth, screenheight, c);
		}
		
		public function drawfillrect(x:int, y:int, w:int, h:int, r:int, g:int = -1, b:int = -1):void {
			//Draw the retangle indicated by that object
			tquad.x = x;
			tquad.y = y;
			tquad.width = w;
			tquad.height = h;
			if (b == -1) {
				tquad.color = r;
			}else{
				tquad.color = RGB(r, g, b);
			}
			
			backbuffer.draw(tquad);
		}
		
		public function drawfillrect_onimage(img:RenderTexture, x:int, y:int, w:int, h:int, r:int, g:int = -1, b:int = -1):void {
			//Draw the retangle indicated by that object
			tquad.x = x;
			tquad.y = y;
			tquad.width = w;
			tquad.height = h;
			if (b == -1) {
				tquad.color = r;
			}else{
				tquad.color = RGB(r, g, b);
			}
			
			img.draw(tquad);
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
			trace("dwgfx.printmask() is not implemented yet");
		}
		
		public function textbox_print(x:int, y:int, t:String, r:int, g:int, b:int, cen:Boolean = false):void {
			trace("dwgfx.textbox_print() is not implemented yet");
		}
		
		public function print(x:int, y:int, t:String, r:int, g:int, b:int, cen:Boolean = false):void {
			if (r < 0) r = 0; if (g < 0) g = 0; if (b < 0) b = 0;
			if (r > 255) r = 255; if (g > 255) g = 255; if (b > 255) b = 255;
			if (cen) {
				x = 0;
				ttf.format.horizontalAlign = "center";
			}else {
				ttf.format.horizontalAlign = "left";	
			}
			
			ttf.format.color = RGB(r, g, b);
			ttf.text = t;
			
			if (flipmode) {
				tposition.identity();
				tposition.scale(1, -1);
				tposition.translate(x, y + 8);
				backbuffer.draw(ttf, tposition);
			}else{
				tposition.identity();
				tposition.translate(x, y);
				backbuffer.draw(ttf, tposition);
			}
		}
		
		public function rprint(x:int, y:int, t:String, r:int, g:int, b:int, cen:Boolean = false):void {
			if (r < 0) r = 0; if (g < 0) g = 0; if (b < 0) b = 0;
			if (r > 255) r = 255; if (g > 255) g = 255; if (b > 255) b = 255;
			ttf.format.horizontalAlign = "right";
			x = x - 320;
			
			ttf.format.color = RGB(r, g, b);
			ttf.text = t;
			
			if (flipmode) {
				tposition.identity();
				tposition.scale(1, -1);
				tposition.translate(x, y + 8);
				backbuffer.draw(ttf, tposition);
			}else {
				tposition.identity();
				tposition.translate(x, y);
				backbuffer.draw(ttf, tposition);
			}
		}
		
		public function printoff(x:int, y:int, t:String, r:int, g:int, b:int, cen:Boolean = false):void {
		  if (r < 0) r = 0; if (g < 0) g = 0; if (b < 0) b = 0;
			if (r > 255) r = 255; if (g > 255) g = 255; if (b > 255) b = 255;
			if (cen) {
				ttf.format.horizontalAlign = "center";
			}else {
				ttf.format.horizontalAlign = "left";	
			}
			
			ttf.format.color = RGB(r, g, b);
			ttf.text = t;
			
			if (flipmode) {
				tposition.identity();
				tposition.scale(1, -1);
				tposition.translate(x, y + 8);
				backbuffer.draw(ttf, tposition);
			}else {
				tposition.identity();
				tposition.translate(x, y);
				backbuffer.draw(ttf, tposition);
			}
		}
		
		public function bprint(x:int, y:int, t:String, r:int, g:int, b:int, cen:Boolean = false):void {			
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
			trace("dwgfx.printmasktemptile() is not implemented yet");
		}
		
		public function printtemptile(x:int, y:int, t:String, r:int, g:int, b:int, cen:Boolean = false):void {
			trace("dwgfx.printtemptile() is not implemented yet");
		}
		
		public function onscreen(t:int):Boolean {
			if (t >= -40 && t <= 280) return true;
			return false;
		}
		
		public function bigrprint(x:int, y:int, t:String, r:int, g:int, b:int, cen:Boolean = false, sc:Number = 2):void {
			if (r < 0) r = 0; if (g < 0) g = 0; if (b < 0) b = 0;
			if (r > 255) r = 255; if (g > 255) g = 255; if (b > 255) b = 255;
			ttf.format.horizontalAlign = "right";
			x = x - 320;
			
			ttf.format.color = RGB(r, g, b);
			ttf.format.size = BitmapFont.NATIVE_SIZE * sc;
			ttf.text = t;
			
			if (flipmode) {
				tposition.identity();
				tposition.scale(1, -1);
				tposition.translate(x, y + (8 * sc));
				backbuffer.draw(ttf, tposition);
			}else {
				tposition.identity();
				tposition.translate(x, y);
				backbuffer.draw(ttf, tposition);
			}
			ttf.format.size = BitmapFont.NATIVE_SIZE;
		}
		
		public function bigprint(x:int, y:int, t:String, r:int, g:int, b:int, cen:Boolean = false, sc:Number = 2):void {
		  if (r < 0) r = 0; if (g < 0) g = 0; if (b < 0) b = 0;
			if (r > 255) r = 255; if (g > 255) g = 255; if (b > 255) b = 255;
			if (cen) {
				x = 0;
				ttf.format.horizontalAlign = "center";
			}else {
				ttf.format.horizontalAlign = "left";	
			}
			
			ttf.format.color = RGB(r, g, b);
			ttf.format.size = BitmapFont.NATIVE_SIZE * sc;
			ttf.text = t;
			
			if (flipmode) {
				tposition.identity();
				tposition.scale(1, -1);
				tposition.translate(x, y + (8 * sc));
				backbuffer.draw(ttf, tposition);
			}else {
				tposition.identity();
				tposition.translate(x, y);
				backbuffer.draw(ttf, tposition);
			}
			
			ttf.format.size = BitmapFont.NATIVE_SIZE;
		}
		
		
		public function len(t:String):int {
			//For VVVVVV, we're using a fixed width font, so...
			return t.length * 8;
		}
		
		public function flashlight():void {
			drawfillrect(0, 0, 320, 240, 187, 187, 187);
		}
		
		public function screenshake():void {
			screenmenubuffer.draw(smallscreen);
			
			if (flipmode) {
				tposition.identity();
				tposition.scale(1, -1);
				tposition.translate(int((Math.random() * 7) - 4), screenheight + int((Math.random() * 7) - 4));
				drawfillrect(0, 0, screenwidth, screenheight, 0, 0, 0);
				backbuffer.draw(screenmenubuffer_image, tposition);
			}else{
				tposition.identity();
				tposition.translate(int((Math.random() * 7) - 4), int((Math.random() * 7) - 4));
				
				backbuffer.draw(screenmenubuffer_image, tposition);
			}
		}
		
		public function render():void {
			if (flipmode) {
				tposition.identity();
				tposition.scale(1, -1);
				tposition.translate(0, screenheight);
				
				screenmenubuffer.drawBundled(function():void {
					screenmenubuffer.draw(smallscreen);
				});
				
				backbuffer.draw(screenmenubuffer_image, tposition);
			}
		}
		
		public function clear_menubuffer():void {
			tquad.x = 0; tquad.y = 0;
			tquad.width = screenwidth;
			tquad.height = screenheight;
			tquad.color = RGB(0, 0, 0);
			
			menubuffer.drawBundled(function():void {
				menubuffer.draw(tquad);
			});
			screenmenubuffer.drawBundled(function():void {
				screenmenubuffer.draw(tquad);
			});
		}
		
		public function setup_menubuffer():void {
			menubuffer.draw(smallscreen);
		}
		
		public function menuoffrender():void {
			//The backbuffer currently contains the menu, so we save this...
			screenmenubuffer.draw(smallscreen);
			
			//Then we draw the old in-game screen instead
			backbuffer.draw(menubuffer_image);
			
			//Finally, we draw the menu back on top of this, offset.
			if (flipmode) {
				tposition.identity();
				tposition.scale(1, -1);
				tposition.translate(0, screenheight + menuoffset);	
				backbuffer.draw(screenmenubuffer_image, tposition);
			}else {
				tposition.identity();
				tposition.translate(0, menuoffset);
				backbuffer.draw(screenmenubuffer_image, tposition);
			}
		}
		
		public var tquad:Quad;
		public var ttf:TextField;
		public var tposition:Matrix;
		public var starlingassets:AssetManager;
		public var starstage:Stage
		
		public var backgrounds:Vector.<Image> = new Vector.<Image>;
		public var images:Vector.<Image> = new Vector.<Image>;
		public var playerlevelimages:Vector.<Image> = new Vector.<Image>;
		public var mobileimages:Vector.<Image> = new Vector.<Image>;
		public var tele:Vector.<Image> = new Vector.<Image>;
		public var tiles:Vector.<Image> = new Vector.<Image>;
		public var tiles2:Vector.<Image> = new Vector.<Image>;
		public var tiles3:Vector.<Image> = new Vector.<Image>;
		public var sprites:Vector.<Image> = new Vector.<Image>;
		public var flipsprites:Vector.<Image> = new Vector.<Image>;
		public var sprites_bitmap:Vector.<BitmapData> = new Vector.<BitmapData>;
		public var flipsprites_bitmap:Vector.<BitmapData> = new Vector.<BitmapData>;
		public var entcolours:Vector.<Image> = new Vector.<Image>;
		public var customminimap:RenderTexture;
		
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
		public var i:int, j:int, k:int, m:int;
		public var tpoint:Point, trect:Rectangle;		
		public var madrect:Rectangle, madpoint:Point;
		
		public var temptile:RenderTexture;
		public var tempsprite:RenderTexture;
		public var footerrect:Rectangle;
		
		//Actual backgrounds
		public var backbuffer:RenderTexture;
		public var frontbuffer_meshbatch:MeshBatch;
		public var menubuffer:RenderTexture;
		public var menubuffer_image:Image;
		public var screenmenubuffer:RenderTexture;
		public var screenmenubuffer_image:Image;
		public var towerbufferforeground_meshbatch:MeshBatch;
		public var towerbufferbackground_meshbatch:MeshBatch;
		public var towerbufferstatic_meshbatch:MeshBatch;
		public var warpzonehorizontal_meshbatch:MeshBatch;
		public var warpzonevertical_meshbatch:MeshBatch;
		public var screen:Image;
		public var smallscreen:Image;
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
		
		//Mobile stuff
		public var screensizemultiplier:Number;
		
		public var screenoffx:int, screenoffy:int;
		
		public var buttonsready:Boolean = false;
		public var button_texture:Vector.<RenderTexture> = new Vector.<RenderTexture>;
	  public var button_image:Vector.<Image> = new Vector.<Image>;
	  public var button_image_width:Vector.<int> = new Vector.<int>;
		public var button_image_height:Vector.<int> = new Vector.<int>;
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
		
    [Embed(source = "../data/bitmapsprites.png")]
    public static const img_bitmapsprites:Class;
		
		[Embed(source = "../data/bitmapflipsprites.png")]
    public static const img_bitmapflipsprites:Class;
	}
}