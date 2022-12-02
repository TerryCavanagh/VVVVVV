package {
	import flash.display.*;
	import flash.geom.*;
  import flash.events.*;
  import flash.net.*; 
	import flash.system.System;
	
	public class helpclass{
		public function init():void {
			sine = new Array();
			cosine = new Array();
				
			for (i = 0; i < 64; i++) {
				sine[i]=Math.sin((i*6.283)/64);
				cosine[i]=Math.cos((i*6.283)/64);
			}
			
			for (i = 0; i < 30; i++) {
				splitseconds.push(int((i * 60) / 30));
			}
			
			glow = 0;
			glowdir = 0;
			slowsine = 0;
		}
		
		public function booltoint(b:Boolean):int {
			if (b) return 1;
			return 0;
		}
		
		public function opa(t:int):int {
			return (t + 32) % 64;
		}
		
		public function number(t:int):String {
			switch(t) {
			  case 0: return "Zero"; break;
				case 1: return "One"; break;
				case 2: return "Two"; break;
				case 3: return "Three"; break;
				case 4: return "Four"; break;
				case 5: return "Five"; break;
				case 6: return "Six"; break;
				case 7: return "Seven"; break;
				case 8: return "Eight"; break;
				case 9: return "Nine"; break;
				case 10: return "Ten"; break;
				case 11: return "Eleven"; break;
				case 12: return "Twelve"; break;
				case 13: return "Thirteen"; break;
				case 14: return "Fourteen"; break;
				case 15: return "Fifteen"; break;
				case 16: return "Sixteen"; break;
				case 17: return "Seventeen"; break;
				case 18: return "Eighteen"; break;
				case 19: return "Nineteen"; break;
				case 20: return "Twenty"; break;
				case 21: return "Twenty One"; break;
			}
			return "Some";
		}
		
		public function removeObject(obj:Object, arr:Array):void{
			var b:String;
			for (b in arr){
				if (arr[b] == obj){
					arr.splice(b,1)
					break;
				}
			}
		}
		
		public function twodigits(t:int):String {
			if (t < 10) return "0" + String(t);
			if (t >= 100) return "??";
			return String(t);
		}
		
		
		public function threedigits(t:int):String {
			if (t < 10) return "0" + String(t);
			if (t < 100) return "0" + String(t);
			if (t >= 100) return String(t);
			return String(t);
		}
		
		public function timestring(t:int):String {
			//given a time t in frames, return a time in seconds
			tempstring = "";
			temp = (t - (t % 30)) / 30;
			if (temp < 60) { //less than one minute
				t = t % 30;
				tempstring = String(temp) + ":" + twodigits(splitseconds[t]);
			}else {
				temp2 = (temp - (temp % 60)) / 60;
				temp = temp % 60;
				t = t % 30;
				tempstring = String(temp2) + ":" + twodigits(temp) + ":" + twodigits(splitseconds[t]);
			}
			return tempstring;
		}
		
		public function updateglow():void {
			slowsine++;
			if (slowsine >= 64) slowsine = 0;
			
		  if (glowdir == 0) {
			  glow+=2; 
				if (glow >= 62) glowdir = 1;
			}else {
			  glow-=2;
				if (glow < 2) glowdir = 0;
			}
		}
		
		public function toclipboard(t:String):void {			
			System.setClipboard(t);
		}
		
	  public var sine:Array;
	  public var cosine:Array;
		public var glow:int, slowsine:int;
		public var glowdir:int;
		public var globaltemp:int, globaltemp2:int, globaltemp3:int, temp:int, temp2:int, tempstring:String;
		public var splitseconds:Array = new Array;
		
		public var i:int;
	}
}
