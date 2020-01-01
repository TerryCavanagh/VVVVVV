package {
	import flash.display.*;
	import flash.geom.*;
  import flash.events.*;
  import flash.net.*;
	
	public class textboxclass extends Sprite {
		public function textboxclass():void {
			firstcreate();
		}
		
		public function firstcreate():void {
			//Like clear, only it creates the actual arrays, etc
			for (iter = 0; iter < 10; iter++) {
				var t:String = new String;	t = "";
				line.push(t);
			}
			x = 0; y = 0; w = 0; h = 0; numlines = 0; lw = 0; tl = 0; tm = 0; active = false; timer = 0;
		}
		
		public function clear():void {
			//Set all values to a default, required for creating a new entity
			for (iter = 0; iter < 12; iter++) {
				line[iter]="";
			}
			textrect = new Rectangle();
			xp = 0; yp = 0; w = 0; h = 0; numlines = 1; lw = 0; tl = 0; tm = 0; active = true; timer = 0;
		}
		
		public function centerx():void { resize();	xp = 160 - (w / 2);	resize(); }
		public function centery():void { resize();	yp = 120 - (h / 2); resize();	}
		
		public function adjust():void {
			resize();
			if (xp < 10) xp = 10;
			if (yp < 10) yp = 10;
			if (xp + w > 310) xp = 310 - w;
			if (yp + h > 230) yp = 230 - h;	
			resize();
		}
		
		public function initcol(rr:int, gg:int, bb:int):void {
			tr = rr; tg = gg; tb = bb;
			r = 0; g = 0; b = 0; tl = 0.5;
		}
		
		public function setcol(rr:int, gg:int, bb:int):void {
			r = rr; g = gg; b = bb;
		}
		
		public function update():void {
			if (tm == 0) {
				tl += .1;
				if (tl >= 1) { tl = 1; tm = 1; }
				setcol(int(tr * tl), int(tg * tl), int(tb * tl));
			}else if (tm == 2) {
				tl -= .1;
				if (tl <= 0.5) { tl = 0.5; active = false; }
				setcol(int(tr * tl), int(tg * tl), int(tb * tl));
			}
			if (timer > 0) {
				timer--;
				if (timer == 0) tm = 2;
			}
		}
		
		public function remove():void {
			//tm = 2; tl = 1; //Remove mode
			tm = 2; tl = 0.4; //Remove mode
		}
		
		public function removefast():void {
			tm = 2; tl = 0.4; //Remove mode
		}
		
		public function resize():void {
			//Set the width and height to the correct sizes
			max = 0;
			for (iter = 0; iter < numlines; iter++) if (line[iter].length > max) max = line[iter].length;
			
			lw = max;
			w = (max +2) * 8;
			h = (numlines + 2) * 8;
			textrect.x = xp; textrect.y = yp;	textrect.width = w;	textrect.height = h;
		}
		
		public function addline(t:String):void {
			line[numlines] = t;
			numlines++;
			resize();
			if (numlines >= 12) numlines = 0;
		}
		
    //Fundamentals
		public var line:Array = new Array;
		public var xp:int, yp:int, lw:int, w:int, h:int, numlines:int;
		public var r:int, g:int, b:int;
		public var tr:int, tg:int, tb:int;
		public var textrect:Rectangle;
		public var active:Boolean;
		public var timer:int;
		
		public var tl:Number, tm:int;
		
		public var iter:int, max:int;
	}
}
