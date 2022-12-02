package{
	import flash.display.Sprite;
  import starling.core.Starling;
  
	[SWF(backgroundColor="#000000", frameRate="30")]
	public class Load extends Sprite{
		private var _starling:Starling;
		
		public function Load() {
			Starling.multitouchEnabled = true;
			
			_starling = new Starling(Main, stage);
			_starling.start();
		}
	}
}