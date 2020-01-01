package {
	import flash.display.*;
	import flash.geom.*;
  import flash.events.*;
  import flash.net.*;
	
	public class edentitiesclass {
		public function edentitiesclass():void {
			clear();
    }
		
		public function clear():void{
			x = 0; y = 0; t = 0;
			p1 = 0; p2 = 0; p3 = 0; p4 = 0; p5 = 0; p6 = 0;
			scriptname = "";
    }
		
		public var x:int, y:int, t:int;
		public var scriptname:String;
		public var p1:int, p2:int, p3:int, p4:int, p5:int, p6:int;
	}
};
