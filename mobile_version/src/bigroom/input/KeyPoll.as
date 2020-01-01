/**
 * Key polling class
 * 
 * To create:
 * var key:KeyPoll = new KeyPoll( displayObject );
 * 
 * the display object will usually be the stage.
 *
 * Full example:
 * package
 *  {
 *  	import flash.display.Sprite;
 *  	import flash.events.Event;
 *  	import flash.ui.Keyboard;
 *  	import fgc.input.KeyPoll;
 *  	
 *  	public class Test 
 *  	{
 *  		var key:KeyPoll;
 *  		
 *  		public function Test()
 *  		{
 *  			key = new KeyPoll( this.stage );
 *  			addEventListener( Event.ENTER_FRAME, enterFrame );
 *  		}
 *  		
 *  		public function enterFrame( ev:Event ):void
 *  		{
 *  			if( key.isDown( Keyboard.LEFT ) )
 *  			{
 *  				trace( "left key is down" );
 *  			}
 *  			if( key.isDown( Keyboard.RIGHT ) )
 *  			{
 *  				trace( "right key is down" );
 *  			}
 *  		}
 *  	}
 *  }
 * 
 * Author: Richard Lord
 * Copyright (c) FlashGameCode.net 2007
 * Version 1.0.2
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

package bigroom.input
{
	import flash.events.KeyboardEvent;
	import flash.events.Event;
	import flash.display.DisplayObject;
	import flash.utils.ByteArray;
  import flash.events.TouchEvent;
  import flash.ui.Multitouch;
  import flash.ui.MultitouchInputMode;
	
	import flash.ui.*;
	import flash.events.MouseEvent;
	
	import flash.net.*;
	
	public class KeyPoll
	{
		private var states:ByteArray;
		private var dispObj:DisplayObject;
		public var click:Boolean = false;
		public var clickheld:Boolean = false;
		public var hasclicked:Boolean = false;
		public var press:Boolean = false;
		public var onscreen:Boolean=true;
		public var gotosite:String = "";
		public var touchx:Vector.<int> = new Vector.<int>;
		public var touchy:Vector.<int> = new Vector.<int>;
		public var touchid:Vector.<int> = new Vector.<int>;
		public var temptouchid:int = 0;
		
		public var controlstick:int = -1;
		public var controlstick_x:int = 0;
		public var controlstick_y:int = 0;
		public var pushleft:Boolean = false;
		public var pushright:Boolean = false;
		public var controlstick_xrange:int = 0;
		public var controlstick_yrange:int = 0;
		public var deadzone:int = 0, deadzone_inner:int = 0;
		public var firstmove:Boolean = false;
		
		public var touchPoints : uint;
		
		
		public function KeyPoll( obj:DisplayObject )
		{
			states = new ByteArray();
			states.writeUnsignedInt( 0 );
			states.writeUnsignedInt( 0 );
			states.writeUnsignedInt( 0 );
			states.writeUnsignedInt( 0 );
			states.writeUnsignedInt( 0 );
			states.writeUnsignedInt( 0 );
			states.writeUnsignedInt( 0 );
			states.writeUnsignedInt( 0 );
			
			touchPoints = 0;
			for (var i:int = 0; i < 20; i++) {
				touchx.push(0);
				touchy.push(0);
				touchid.push(-1);
			}
			
			Multitouch.inputMode = MultitouchInputMode.TOUCH_POINT; 
			dispObj = obj;
			dispObj.addEventListener( KeyboardEvent.KEY_DOWN, keyDownListener, false, 0, true );
			dispObj.addEventListener( KeyboardEvent.KEY_UP, keyUpListener, false, 0, true );
			dispObj.addEventListener( Event.ACTIVATE, activateListener, false, 0, true );
			dispObj.addEventListener( Event.DEACTIVATE, deactivateListener, false, 0, true );
			
			/*
			dispObj.addEventListener( MouseEvent.MOUSE_DOWN, mouseDownListener);
			dispObj.addEventListener( MouseEvent.MOUSE_UP, mouseUpListener );
			*/
			
			
			dispObj.addEventListener( TouchEvent.TOUCH_BEGIN, touchbeginlistener);
      dispObj.addEventListener( TouchEvent.TOUCH_MOVE, touchmovelistener); 
			dispObj.addEventListener( TouchEvent.TOUCH_END, touchendlistener );
			
			
			//dispObj.addEventListener( TouchEvent.TOUCH_TAP, mouseclicker);
		}
		
		public function definestickrange(x:int, y:int, d:int):void {
			/*
			if (device.deviceresolution == device.ANDROID) {
				//Turn d into a sensible value
				d = 6;
			}else {
				//Ignore it and just use 6
				d = 6;
			}
			*/
			controlstick_xrange = x;
			controlstick_yrange = y;
			deadzone = d;
			deadzone_inner = Math.min(int(d * 0.5), 1);
		}
		
		public function gettouchx():int {
			if (touchPoints == 0) {
				return touchx[0];
			}else {
				return touchx[touchPoints - 1];
			}
		}
		
		public function gettouchy():int {
			if (touchPoints == 0) {
				return touchy[0];
			}else {
				return touchy[touchPoints - 1];
			}
		}
		
		/*
		public function mouseUpListener( e:MouseEvent ):void {
			//Identify the point that's been removed, and take it away from the array
			touchPoints = 0;
			
			if (touchPoints == 0){
				press = false;
				
				click = false;
				clickheld = false;
				hasclicked = false;
			}
		}
		
		public function mouseDownListener( e:MouseEvent ):void {
			touchx[0] = e.stageX;
			touchy[0] = e.stageY;
			touchid[0] = 1;
			
			touchPoints=1;
			press = true;
			
			click = true;
			clickheld = true;
			hasclicked = true;
		}
		*/
		
		public function touchbeginlistener( e:TouchEvent ):void {
			touchx[touchPoints] = e.stageX;
			touchy[touchPoints] = e.stageY;
			touchid[touchPoints] = e.touchPointID;
			
			if (controlstick == -1) {
				//Consider this for the controlstick point
				if (touchx[touchPoints] < controlstick_xrange) {
					if (touchy[touchPoints] > controlstick_yrange) {
						controlstick = touchid[touchPoints];
						controlstick_x = touchx[touchPoints];
						controlstick_y = touchy[touchPoints];
						pushleft = false;
						pushright = false;
						firstmove = true;
				  }
				}
			}
			
			touchPoints++;
			press = true;
			
			click = true;
			clickheld = true;
			hasclicked = true;
		}
		
		public function touchendlistener( e:TouchEvent ):void {
			//Identify the point that's been removed, and take it away from the array
			temptouchid = e.touchPointID;
			
			if (temptouchid == controlstick) {
				controlstick = -1;
				pushleft = false;
				pushright = false;
				firstmove = false;
				controlstick_x = -1;
				controlstick_y = -1;
			}
			
			for (var i:int = 0; i < touchPoints; i++) {
				if (touchid[i] == temptouchid) {
					for (var j:int = i; j < touchPoints; j++) {
						touchx[j] = touchx[j + 1];
						touchy[j] = touchy[j + 1];
						touchid[j] = touchid[j + 1];
					}
					i = touchPoints + 1;
					touchPoints--;
				}
			}
			
			if (touchPoints == 0){
				press = false;
				
				click = false;
				clickheld = false;
				hasclicked = false;
			}
		}
		
		public function touchmovelistener(e:TouchEvent):void { 
			//Identify the touch point that's moving, and update it's coordinates
			temptouchid = e.touchPointID;
			for (var i:int = 0; i < touchPoints; i++) {
				if (touchid[i] == temptouchid) {
					touchx[i] = e.stageX;
					touchy[i] = e.stageY;
					
					if (touchid[i] == controlstick) {
						if (firstmove) {
							if (touchx[i] < controlstick_x) {
								pushleft = true; pushright = false;
								firstmove = false;
								controlstick_x = touchx[i] + deadzone;
								controlstick_y = touchy[i];
							}else if (touchx[i] > controlstick_x) {
								pushleft = false; pushright = true;
								firstmove = false;
								controlstick_x = touchx[i] - deadzone;
								controlstick_y = touchy[i];
							}
						}else{
							if (touchx[i] < controlstick_x - deadzone) {
								pushleft = true; pushright = false;
								controlstick_x = touchx[i] + deadzone;
								controlstick_y = touchy[i];
							}else if (touchx[i] > controlstick_x + deadzone) {
								pushleft = false; pushright = true;
								controlstick_x = touchx[i] - deadzone;
								controlstick_y = touchy[i];
							}else if (touchx[i] >= controlstick_x - deadzone_inner &&
												touchx[i] <= controlstick_x + deadzone_inner) {
								pushleft = false; pushright = false;
							}
						}
					}
				}
			}			
		} 
		
		public function visitsite(t:String):void {
			gotosite = t;
		}
		
		/*
		public function mouseclicker(e:TouchEvent):void {
			touchx = e.stageX;
			touchy = e.stageY;
			if (gotosite == "NG") {
				//API.loadNewgrounds();
				gotosite = "";
			}else	if (gotosite != "") {
				var link:URLRequest = new URLRequest(gotosite);
        navigateToURL(link, "_blank");
				gotosite = "";
			}
		}
		*/		
		
		private function keyDownListener( ev:KeyboardEvent ):void
		{
			states[ ev.keyCode >>> 3 ] |= 1 << (ev.keyCode & 7);
			
			if (ev.keyCode == 27){
				ev.preventDefault();
			}else if (ev.keyCode == Keyboard.BACK) {
				// user hit the back button on Android device
				ev.preventDefault();
				ev.stopImmediatePropagation();
      }
		}
		
		private function keyUpListener( ev:KeyboardEvent ):void
		{
			states[ ev.keyCode >>> 3 ] &= ~(1 << (ev.keyCode & 7));
		}
		
		private function activateListener( ev:Event ):void
		{
			for( var i:int = 0; i < 8; ++i )
			{
				states[ i ] = 0;
			}
		}

		private function deactivateListener( ev:Event ):void
		{
			for( var i:int = 0; i < 8; ++i )
			{
				states[ i ] = 0;
			}
		}

		public function isDown( keyCode:uint ):Boolean
		{
			return ( states[ keyCode >>> 3 ] & (1 << (keyCode & 7)) ) != 0;
		}
		
		public function isUp( keyCode:uint ):Boolean
		{
			return ( states[ keyCode >>> 3 ] & (1 << (keyCode & 7)) ) == 0;
		}
	}
}