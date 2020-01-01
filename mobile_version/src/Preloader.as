//Big thanks to Muku for his help with working out how to do a preloader in FlashDevelop!

package {	
	import flash.display.*;
	import flash.geom.*;
  import flash.events.*;
  import flash.net.*;
	import flash.utils.getDefinitionByName;
	import flash.system.fscommand;
  import flash.ui.ContextMenu;
  import flash.ui.ContextMenuItem;
	//import com.kongregate.as3.client.KongregateAPI;
	
	//Mochi and Kongregate stuff is more or less ready here
	
	public dynamic class Preloader extends MovieClip {
		public function Preloader() {
			fscommand("trapallkeys", "true");
			if (stage.root.loaderInfo.url.search(/.swf$/) >= 0) {
				//fscommand("showmenu", "false");
			}
			stage.showDefaultContextMenu = (stage.root.loaderInfo.url.search(/.swf$/) >= 0);
			
			//For the offline version, manually change this
			if (checksite()) {
				adson = false;
			}else{
			  adson = true;
			}
			adson = false;
						
			//Let's assume the kong API is clever enough to do its own checking
			//var kongregate:KongregateAPI = new KongregateAPI();
      //this.addChild ( kongregate );  
			
			//stage.showDefaultContextMenu = false;
			
			
			//show c64 intro (30, 0), set to (-10, 100) to not show
		  //transition = -10;	fakepercent = 100;
			transition = 30;	fakepercent = 0;
			
			
		  var rc_menu:ContextMenu = new ContextMenu();
			rc_menu.hideBuiltInItems();
      this.contextMenu = rc_menu;
			ct = new ColorTransform(0, 0, 0, 1, 255, 255, 255, 1); //Set to white			
			
			darkcol = 0x000000; lightcol = 0x000000; curcol = 0;
	  	offset = 0; coltimer = 0;
			
			frontrect = new Rectangle(30, 20, 260, 200);
			temprect = new Rectangle(0, 0, 320, 24);
			
			tl = new Point(0, 0);
			tpoint = new Point(0, 0);
			bfont_rect=new Rectangle(0,0,8,8);
			var tempbmp:Bitmap;
			tempbmp = new im_bfont();	 buffer = tempbmp.bitmapData;
			makebfont();
			
			backbuffer=new BitmapData(320, 240,false,0x000000);
		  screenbuffer = new BitmapData(320, 240, false, 0x000000);
		  screen = new Bitmap(screenbuffer);
			screen.width = 640;//320;//;640;
      screen.height = 480;// 240;//480;				

			addChild(screen);  
			
			addEventListener(Event.ENTER_FRAME, checkFrame);
			
			if (!adson) {
				/*
			  loading = new im_loading();
			  loading.x = 320 - (loading.width / 2);
			  loading.y = 316;
			  addChild(loading);
				*/
			}
			
			showctp = false;
			startgame = false;
		}
		
    public function visit_distractionware(e:Event):void{
      var distractionware_link:URLRequest = new URLRequest( "http://www.distractionware.com" );
      navigateToURL( distractionware_link, "_blank" );
    }
		
		public function visit_sponsor(e:Event):void{
      var sponsor_link:URLRequest = new URLRequest( "http://www.kongregate.com/?gamereferral=dontlookback" );
      navigateToURL( sponsor_link, "_blank" );
    }
		
		public function print(x:int, y:int, t:String, r:int, g:int, b:int, cen:Boolean = false):void {
			if (r < 0) r = 0; if (g < 0) g = 0; if (b < 0) b = 0;
			if (r > 255) r = 255; if (g > 255) g = 255; if (b > 255) b = 255;
			ct.color = RGB(r, g, b);
			if (cen) x = x - (len(t));
			bfontpos = 0;
			for (var i:int = 0; i < t.length; i++) {
				cur = t.charCodeAt(i);
				tpoint.x = x + bfontpos; tpoint.y = y;
			  bfont[cur].colorTransform(bfont_rect, ct);
				backbuffer.copyPixels(bfont[cur], bfont_rect, tpoint);
				bfontpos+=bfontlen[cur];
			}
		}
		
		public function checkFrame(e:Event):void {
			var p:Number = this.loaderInfo.bytesLoaded / this.loaderInfo.bytesTotal;
				
			//if (stage.root.loaderInfo.url.search(/.swf$/) >= 0) p = 0;  //Not for the demo!
		  transition = -10;	fakepercent = 100;
			
			if (transition < 30) transition--;
			if(transition>=30){
				if (int(p * 100) >= fakepercent) fakepercent++;
				if (fakepercent >= 100) {
					fakepercent = 100;
					startgame = true;
				}
				
				offset = (offset + 4 + int(Math.random()*5))%32;
				coltimer--;
				if (coltimer <= 0) {
					curcol = (curcol + int(Math.random() * 5)) % 6;	
					coltimer = 8;
				}
				switch(curcol) {
					case 0:
						lightcol = 0xBF596F;
						darkcol = 0x883E53;
					break;
					case 1:
						lightcol = 0x6CBC5C;
						darkcol = 0x508640;
					break;
					case 2:
						lightcol = 0x5D57AA;
						darkcol = 0x2F2F6C;
					break;
					case 3:
						lightcol = 0xB7BA5E;
						darkcol = 0x848342;
					break;
					case 4:
						lightcol = 0x5790AA;
						darkcol = 0x2F5B6C;
					break;
					case 5:
						lightcol = 0x9061B1;
						darkcol = 0x583D71;
					break;
				}
					
				for (var i:int = 0; i < 18; i++) {
					temprect.y = (i * 16)	-offset;
					if (i % 2 == 0) {
						backbuffer.fillRect(temprect, lightcol);
					}else{
						backbuffer.fillRect(temprect, darkcol);
					}
				}
				
				backbuffer.fillRect(frontrect, 0x3E31A2);
				
				
				tempstring = "LOADING... " + String(int(fakepercent))+"%";
				print(282, 204, tempstring, 124, 112, 218, true);
				
				//Render
				screenbuffer.lock();
				screenbuffer.copyPixels(backbuffer, backbuffer.rect, tl, null, null, false);
				screenbuffer.unlock();
				
				backbuffer.lock();
				backbuffer.fillRect(backbuffer.rect, 0x000000);
				backbuffer.unlock();
				if (currentFrame >= totalFrames){
					if (startgame) {
						transition = 29;
					}
				}
			}else if (transition <= -10) {
				if (currentFrame >= totalFrames){
				  startup();
				}
			}else if (transition < 5) {
				backbuffer.fillRect(backbuffer.rect, 0x000000);
			  //Render
				screenbuffer.lock();
				screenbuffer.copyPixels(backbuffer, backbuffer.rect, tl, null, null, false);
				screenbuffer.unlock();
				
				backbuffer.lock();
				backbuffer.fillRect(backbuffer.rect, 0x000000);
				backbuffer.unlock();
			}else if (transition < 20) {
				temprect.y = 0;
				temprect.height = 240;
				backbuffer.fillRect(temprect, 0x000000);
				backbuffer.fillRect(frontrect, 0x3E31A2);
				
				tempstring = "LOADING... 100%";
				print(282, 204, tempstring, 124, 112, 218, true);
			  //Render
				screenbuffer.lock();
				screenbuffer.copyPixels(backbuffer, backbuffer.rect, tl, null, null, false);
				screenbuffer.unlock();
				
				backbuffer.lock();
				backbuffer.fillRect(backbuffer.rect, 0x000000);
				backbuffer.unlock();
			}
		}
		
		private function startup():void {
			// hide loader
			//stop();
			removeChild(screen);
			removeEventListener(Event.ENTER_FRAME, checkFrame);
			//loaderInfo.removeEventListener(ProgressEvent.PROGRESS, progress);
			var mainClass:Class = getDefinitionByName("Main") as Class;
			addChild(new mainClass() as DisplayObject);
			//stage.removeChild(this);
		}
		
		public function checksite():Boolean {
			//Returns true if on a site that doesn't use mochiads
			var currUrl:String = stage.loaderInfo.url.toLowerCase();
			//chat.kongregate.com
			if ((currUrl.indexOf("distractionware.com") <= 0) && 
			    (currUrl.indexOf("flashgamelicense.com") <= 0) &&
					(currUrl.indexOf("kongregate.com") <= 0) && 
					(currUrl.indexOf("chat.kongregate.com") <= 0)){
				//return true;
				return false;
			}else{
				return true;
			}
		}
		
		public function len(t:String):int {
			bfontpos = 0;
		  for (var i:int = 0; i < t.length; i++) {
				cur = t.charCodeAt(i);
			  bfontpos+=bfontlen[cur];
			}
			return bfontpos;
		}
		
		public function RGB(red:Number,green:Number,blue:Number):Number{
			return (blue | (green << 8) | (red << 16))
		}
		
		public function makebfont():void {
			for (var j:Number = 0; j < 16; j++) {
				for (var i:Number = 0; i < 16; i++) {
					var t:BitmapData = new BitmapData(8, 8, true, 0x000000);
					var t2emprect:Rectangle = new Rectangle(i * 8, j * 8, 8, 8);	
					t.copyPixels(buffer, t2emprect, tl);					
					bfont.push(t);
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
		
		public var darkcol:int, lightcol:int, curcol:int, coltimer:int;
		public var offset:int;
		
		public var buffer:BitmapData;
		public var backbuffer:BitmapData;
		public var screenbuffer:BitmapData;
		public var screen:Bitmap;
		
		public var frontrect:Rectangle;
		public var temprect:Rectangle;
		
		public var showctp:Boolean;
		public var startgame:Boolean;
		public var adson:Boolean;
		
		[Embed(source = '../data/graphics/font.png')]	private var im_bfont:Class;
		public var bfontlen:Array = new Array();
		public var bfont:Array = new Array();
	  public var bfont_rect:Rectangle;
		public var tl:Point, tpoint:Point;		
		public var bfontpos:int;
		public var cur:int;
		public var ct:ColorTransform;
		
		public var tempstring:String;
		public var fakepercent:int;
		
		public var transition:int;
		
		public var statcookie:SharedObject;
	}
}