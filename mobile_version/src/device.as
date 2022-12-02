package {
	public class device {
		public static var DESKTOP:int = 0;
		public static var MOBILE:int = 1;
		public static var CONSOLE:int = 2;
		
		public static var IPHONE:int = 1;
		public static var IPHONERETINA:int = 2;
		public static var IPAD:int = 3;
		public static var IPADRETINA:int = 4;
		public static var IPHONE5:int = 5;
		public static var ANDROID:int = 6;
		public static var OUYA:int = 7;
		
		public static function init(t:int, r:int):void {
			for (var i:int = 0; i < 20; i++) devicetext.push("");
			if (t == 0) { // PC/MAC DESKTOP
				devicetext[0] = "BACK - MENU";
				devicetext[1] = "PRESS ACTION TO START";
				devicetext[2] = "ACTION - STAGE SELECT";
				devicetext[3] = "PRESS LEFT AND RIGHT KEYS";
				devicetext[4] = "TO MOVE AROUND THE HEXAGON";
				devicetext[5] = "PRESS ACTION TO CONTINUE";
				devicetext[6] = "PRESS LEFT OR RIGHT TO RETRY";
				devicetext[7] = "BACK - CREDITS";
				devicetext[8] = "BACK - CANCEL";
				devicetext[9] = "PRESS ACTION TO CONFIRM";
				devicetext[10] = "C - CLEAR RECORDS";
				
				devicetype = DESKTOP;
				deviceresolution = DESKTOP;
			}else if (t == 1) { //Mobile
				devicetext[0] = "MAIN MENU";
				devicetext[1] = "TAP TO START";
				devicetext[2] = "STAGE SELECT";
				devicetext[3] = "TOUCH THE LEFT AND RIGHT";
				devicetext[4] = "SIDES OF THE SCREEN TO MOVE";
				devicetext[5] = "TAP TO CONTINUE";
				devicetext[6] = "TAP THE SCREEN TO RETRY";
				devicetext[7] = "CREDITS";
				devicetext[8] = "CANCEL";
				devicetext[9] = "TAP TO CONFIRM";
				devicetext[10] = "CLEAR RECORDS";
				
				devicetype = MOBILE;
				deviceresolution = r;
			}
		  
		}
		
	  public static var devicetext:Vector.<String> = new Vector.<String>;
		public static var devicetype:int;
		
		public static var deviceresolution:int;
		public static var xres:int, yres:int;
		
		public static var localtesting:Boolean = false;
	}
}
