package {
	public class tmap {
		public static function init():void {
			for (var i:int = 0; i < 30; i++) {
			  contents.push("");
			}
			
			index = 0;
		}
		
		public static function reset():void {
			index = 0;
		}
		
		public static function push(t:String):void {
			contents[index] = t;
			index++;
		}
		
		public static var contents:Vector.<String> = new Vector.<String>;
		public static var index:int;
	}
}
