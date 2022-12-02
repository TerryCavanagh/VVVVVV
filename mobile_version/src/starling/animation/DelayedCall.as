// =================================================================================================
//
//	Starling Framework
//	Copyright Gamua GmbH. All Rights Reserved.
//
//	This program is free software. You can redistribute and/or modify it
//	in accordance with the terms of the accompanying license agreement.
//
// =================================================================================================

package starling.animation
{
    import starling.core.starling_internal;
    import starling.events.Event;
    import starling.events.EventDispatcher;

    /** A DelayedCall allows you to execute a method after a certain time has passed. Since it 
     *  implements the IAnimatable interface, it can be added to a juggler. In most cases, you 
     *  do not have to use this class directly; the juggler class contains a method to delay
     *  calls directly. 
     * 
     *  <p>DelayedCall dispatches an Event of type 'Event.REMOVE_FROM_JUGGLER' when it is finished,
     *  so that the juggler automatically removes it when its no longer needed.</p>
     * 
     *  @see Juggler
     */ 
    public class DelayedCall extends EventDispatcher implements IAnimatable
    {
        private var _currentTime:Number;
        private var _totalTime:Number;
        private var _callback:Function;
        private var _args:Array;
        private var _repeatCount:int;
        
        /** Creates a delayed call. */
        public function DelayedCall(callback:Function, delay:Number, args:Array=null)
        {
            reset(callback, delay, args);
        }
        
        /** Resets the delayed call to its default values, which is useful for pooling. */
        public function reset(callback:Function, delay:Number, args:Array=null):DelayedCall
        {
            _currentTime = 0;
            _totalTime = Math.max(delay, 0.0001);
            _callback = callback;
            _args = args;
            _repeatCount = 1;
            
            return this;
        }
        
        /** @inheritDoc */
        public function advanceTime(time:Number):void
        {
            var previousTime:Number = _currentTime;
            _currentTime += time;

            if (_currentTime > _totalTime)
                _currentTime = _totalTime;
            
            if (previousTime < _totalTime && _currentTime >= _totalTime)
            {                
                if (_repeatCount == 0 || _repeatCount > 1)
                {
                    _callback.apply(null, _args);
                    
                    if (_repeatCount > 0) _repeatCount -= 1;
                    _currentTime = 0;
                    advanceTime((previousTime + time) - _totalTime);
                }
                else
                {
                    // save call & args: they might be changed through an event listener
                    var call:Function = _callback;
                    var args:Array = _args;
                    
                    // in the callback, people might want to call "reset" and re-add it to the
                    // juggler; so this event has to be dispatched *before* executing 'call'.
                    dispatchEventWith(Event.REMOVE_FROM_JUGGLER);
                    call.apply(null, args);
                }
            }
        }

        /** Advances the delayed call so that it is executed right away. If 'repeatCount' is
          * anything else than '1', this method will complete only the current iteration. */
        public function complete():void
        {
            var restTime:Number = _totalTime - _currentTime;
            if (restTime > 0) advanceTime(restTime);
        }
        
        /** Indicates if enough time has passed, and the call has already been executed. */
        public function get isComplete():Boolean 
        { 
            return _repeatCount == 1 && _currentTime >= _totalTime;
        }
        
        /** The time for which calls will be delayed (in seconds). */
        public function get totalTime():Number { return _totalTime; }
        
        /** The time that has already passed (in seconds). */
        public function get currentTime():Number { return _currentTime; }
        
        /** The number of times the call will be repeated. 
         *  Set to '0' to repeat indefinitely. @default 1 */
        public function get repeatCount():int { return _repeatCount; }
        public function set repeatCount(value:int):void { _repeatCount = value; }

        /** The callback that will be executed when the time is up. */
        public function get callback():Function { return _callback; }

        /** The arguments that the callback will be executed with.
         *  Beware: not a copy, but the actual object! */
        public function get arguments():Array { return _args; }
        
        // delayed call pooling
        
        private static var sPool:Vector.<DelayedCall> = new <DelayedCall>[];
        
        /** @private */
        starling_internal static function fromPool(call:Function, delay:Number, 
                                                   args:Array=null):DelayedCall
        {
            if (sPool.length) return sPool.pop().reset(call, delay, args);
            else return new DelayedCall(call, delay, args);
        }
        
        /** @private */
        starling_internal static function toPool(delayedCall:DelayedCall):void
        {
            // reset any object-references, to make sure we don't prevent any garbage collection
            delayedCall._callback = null;
            delayedCall._args = null;
            delayedCall.removeEventListeners();
            sPool.push(delayedCall);
        }
    }
}