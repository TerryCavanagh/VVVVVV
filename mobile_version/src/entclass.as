package {
	import flash.geom.*;
	
	public class entclass{
		public function entclass():void {
			clear();
		}
		
		public function clear():void {
			//Set all values to a default, required for creating a new entity
			active = false; invis = false;
			type = 0;  size = 0; tile = 0; rule = 0;
			state = 0; statedelay = 0; life = 0; colour = 0; para = 0;
			behave = 0; animate = 0;
			
			xp = 0; yp = 0; ax = 0; ay = 0; vx = 0; vy = 0;
			w = 16; h = 16; cx = 0; cy = 0;
			newxp = 0; newyp = 0;
			
			x1 = 0; y1 = 0; x2 = 320; y2 = 240;
			
			jumping = false; gravity = false; onground = 0; onroof = 0; jumpframe = 0;
			
	    onentity = 0; harmful = false; onwall = 0; onxwall = 0; onywall = 0; isplatform = false;
			
			framedelay = 0; drawframe = 0; walkingframe = 0; dir = 0; actionframe = 0;
		}
		
		public function outside():Boolean {
			//returns true if any point of the entity is outside the map. adjusts velocity for a clean collision.
			if (xp < x1){
				xp = x1;
				return true;
			}
			if (yp < y1) {
				yp = y1;
				return true;
			}
			if (xp + w > x2) {
				xp = x2 - w;
				return true;
			}
			if (yp + h > y2) {
				yp = y2 - h;
				return true;
			}
			return false;
		}
		
    //Fundamentals
		public var active:Boolean, invis:Boolean;
		public var type:int, size:int, tile:int, rule:int;
		public var state:int, statedelay:int;
		public var behave:int, animate:int, para:int;
		public var life:int, colour:int;
		//Position and velocity
		public var xp:Number, yp:Number, oldxp:Number, oldyp:Number;
		public var ax:Number, ay:Number, vx:Number, vy:Number;
		public var cx:int, cy:int, w:int, h:int;
    public var newxp:Number, newyp:Number; //For collision functions
		public var isplatform:Boolean;
		public var x1:int, y1:int, x2:int, y2:int;
		//Collision Rules
		public var onentity:int;
		public var harmful:Boolean;
		public var onwall:int, onxwall:int, onywall:int;
		
		//Platforming specific
		public var jumping:Boolean, gravity:Boolean, onground:int, onroof:int;
		public var jumpframe:int;
		//Animation
		public var framedelay:int, drawframe:int, walkingframe:int, dir:int, actionframe:int;
	}
}
