package {
	import flash.display.*;
	import flash.geom.*;
  import flash.events.*;
  import flash.net.*;
	
	public class edlevelclass{
		public function edlevelclass():void {
			clear();
    }
		
		public function clear():void {
			tileset = 0; tilecol = 0;
			roomname = "";
			warpdir = 0;
			platx1 = 0; platy1 = 0; platx2 = 320; platy2 = 240; platv = 4;
			enemyx1 = 0; enemyy1 = 0; enemyx2 = 320; enemyy2 = 240; enemytype = 0;
			directmode = 0;
		}
		
		public var tileset:int, tilecol:int;
		public var roomname:String;
		public var warpdir:int;
		public var platx1:int, platy1:int, platx2:int, platy2:int, platv:int;
		public var enemyx1:int, enemyy1:int, enemyx2:int, enemyy2:int, enemytype:int;
		public var directmode:int;
	}
};