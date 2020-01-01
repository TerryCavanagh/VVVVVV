//Non gamecenter/steam platform specific specific code belongs here
package {
	//import com.sticksports.nativeExtensions.SilentSwitch;
  import flash.events.*;
	import flash.media.SoundMixer;
	import flash.media.AudioPlaybackMode;
	
	public class platformclass {
		public function init():void {
			//SilentSwitch.apply();
			SoundMixer.audioPlaybackMode = AudioPlaybackMode.AMBIENT;
		}
		
		public function callonwake():void {
			//SilentSwitch.apply();
			wakeupcall = 1;
		}
		
		public function callonsleep():void {
			
		}
		
		public var wakeupcall:int;
	}
}