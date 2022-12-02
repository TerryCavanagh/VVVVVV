package {         
	import flash.media.*; 
  import flash.events.*;
	
	public class musicclass {	
		//For Music stuff
		public function play(t:int):void {
			if (currentsong !=t) {
				if (currentsong != -1) {
					//Stop the old song first
					musicchannel.stop();
					if (currentsong != 0) {
					  musicchannel.removeEventListener(Event.SOUND_COMPLETE, loopmusic);
					}
				}
				if (t != -1) {
			    musicfade = 0;
			    currentsong = t;
					if (currentsong == 0 || currentsong == 7) {
						//Level Complete theme, no fade in or repeat
					  musicchannel = musicchan[currentsong].play(0);
						musicchannel.soundTransform = new SoundTransform(1.0);
					}else {
						musicfadein = 90;
						musicchannel = musicchan[currentsong].play(0);
						musicchannel.soundTransform = new SoundTransform(0);
						musicchannel.addEventListener(Event.SOUND_COMPLETE, loopmusic);
					}
				}else {	
			    currentsong = -1;
				}
			}
    }   
		
		public function loopmusic(e:Event):void { 
			musicchannel.removeEventListener(Event.SOUND_COMPLETE, loopmusic);
			if(currentsong>-1){
				musicchannel = musicchan[currentsong].play();
				musicchannel.addEventListener(Event.SOUND_COMPLETE, loopmusic);
			}
		}
		
		public function stopmusic(e:Event):void { 
			musicchannel.removeEventListener(Event.SOUND_COMPLETE, stopmusic);
			musicchannel.stop();
			currentsong = -1;
		}
		
		public function haltdasmusik():void {
			musicchannel.removeEventListener(Event.SOUND_COMPLETE, stopmusic);
			musicchannel.stop();
			resumesong = currentsong;
			currentsong = -1;
		}
		
		public function silencedasmusik():void {
			if(currentsong>-1){
			  musicchannel.soundTransform = new SoundTransform(0);
			}
		}
		
		public function fadeout():void { 
			if(currentsong>-1){
				if (musicfade == 0) {
					musicchannel.removeEventListener(Event.SOUND_COMPLETE, stopmusic);
					musicfade = 61;
				}
			}
		}
		
		public function processmusicfade():void {
			musicfade--;
			if (musicfade > 0) {
				musicchannel.soundTransform = new SoundTransform(musicfade / 60);
			}else {
			  musicchannel.stop();
			  currentsong = -1;
			}
		}
		
		public function processmusicfadein():void {
			musicfadein--;
			if (musicfadein > 0) {
				musicchannel.soundTransform = new SoundTransform((90-musicfadein) / 90 );
			}else {
				musicchannel.soundTransform = new SoundTransform(1.0);
			}
		}
		
		
   	public function processmusic():void {
			if (musicfade > 0) processmusicfade();
			if (musicfadein > 0) processmusicfadein();
			
			if (nicefade == 1 && currentsong==-1) {
				play(nicechange);
				nicechange = -1; nicefade = 0;
			}
			
			musicstopother--;
			if (musicstopother == 1) {
				musicstopother = 0;
				if (currentmusicchan == 0) musicchannel2.stop();
				if (currentmusicchan == 1) musicchannel.stop();
			}
			if (musicstopother < 0) musicstopother = 0;
			
			musicchancur--;
			if (musicchancur <= 0 && currentsong > -1 && musicchanlen > 0) {
				musicchancur = musicchanlen;
				if (currentmusicchan == 0) {
          musicchannel2 = musicchan[currentsong].play();
					musicstopother = 3; 
          currentmusicchan = 1;					
				}else {
					musicchannel = musicchan[currentsong].play();
					musicstopother = 3; 
					currentmusicchan = 0;
				}
			}
		}
		
		public function niceplay(t:int):void {
			//important: do nothing if the correct song is playing!
			if(currentsong!=t){
			  if(currentsong!=-1) fadeout();
			  nicefade = 1;
			  nicechange = t;
			}
		}
		
		public function musicroom(x:int, y:int):int {
			return x + (y * 20);
		}
		
		public function changemusicarea(x:int, y:int):void {
			switch(musicroom(x, y)) {
				case musicroom(11, 4):
				  niceplay(2);
				break;
				
				case musicroom(2, 4):
				case musicroom(7, 15):
				  niceplay(3);
				break;
				
				case musicroom(18, 1):
				case musicroom(15, 0):
				  niceplay(12);
				break;
				
				case musicroom(0, 0):
				case musicroom(0, 16):
				case musicroom(2, 11):
				case musicroom(7, 9):
				case musicroom(8, 11):
				case musicroom(13, 2):
				case musicroom(17, 12):
				case musicroom(14, 19):
				case musicroom(17, 17):
				  niceplay(4);
				break;
				
				default:
				  niceplay(1);
				break;
			}
		}
		
		public var musicchan:Array = new Array();	
		public var musicchannel:SoundChannel, musicchannel2:SoundChannel; 
		public var currentmusicchan:int, musicchanlen:int, musicchancur:int, musicstopother:int, resumesong:int;
		public var currentsong:int, musicfade:int, musicfadein:int;
		
		public var nicefade:int, nicechange:int;
		
		//Play a sound effect! There are 16 channels, which iterate
		public function initefchannels():void {
			for (var i:int = 0; i < 16; i++) efchannel.push(new SoundChannel);
		}
		
		public function playef(t:int, offset:int=0):void {
			efchannel[currentefchan] = efchan[t].play(offset);
			currentefchan++;
			if (currentefchan > 15) currentefchan -= 16;
		}
		
		public var currentefchan:int;
		public var efchannel:Array = new Array();
		public var efchan:Array = new Array();
		public var numplays:int;
	}
}
