// =================================================================================================
//
//	Starling Framework
//	Copyright Gamua GmbH. All Rights Reserved.
//
//	This program is free software. You can redistribute and/or modify it
//	in accordance with the terms of the accompanying license agreement.
//
// =================================================================================================

package starling.events
{
    import flash.utils.getQualifiedClassName;

    import starling.core.starling_internal;
    import starling.utils.StringUtil;

    use namespace starling_internal;

    /** Event objects are passed as parameters to event listeners when an event occurs.  
     *  This is Starling's version of the Flash Event class. 
     *
     *  <p>EventDispatchers create instances of this class and send them to registered listeners. 
     *  An event object contains information that characterizes an event, most importantly the 
     *  event type and if the event bubbles. The target of an event is the object that 
     *  dispatched it.</p>
     * 
     *  <p>For some event types, this information is sufficient; other events may need additional 
     *  information to be carried to the listener. In that case, you can subclass "Event" and add 
     *  properties with all the information you require. The "EnterFrameEvent" is an example for 
     *  this practice; it adds a property about the time that has passed since the last frame.</p>
     * 
     *  <p>Furthermore, the event class contains methods that can stop the event from being 
     *  processed by other listeners - either completely or at the next bubble stage.</p>
     * 
     *  @see EventDispatcher
     */
    public class Event
    {
        /** Event type for a display object that is added to a parent. */
        public static const ADDED:String = "added";
        /** Event type for a display object that is added to the stage */
        public static const ADDED_TO_STAGE:String = "addedToStage";
        /** Event type for a display object that is entering a new frame. */
        public static const ENTER_FRAME:String = "enterFrame";
        /** Event type for a display object that is removed from its parent. */
        public static const REMOVED:String = "removed";
        /** Event type for a display object that is removed from the stage. */
        public static const REMOVED_FROM_STAGE:String = "removedFromStage";
        /** Event type for a triggered button. */
        public static const TRIGGERED:String = "triggered";
        /** Event type for a resized Flash Player. */
        public static const RESIZE:String = "resize";
        /** Event type that may be used whenever something finishes. */
        public static const COMPLETE:String = "complete";
        /** Event type for a (re)created stage3D rendering context. */
        public static const CONTEXT3D_CREATE:String = "context3DCreate";
        /** Event type that is dispatched by the Starling instance directly before rendering. */
        public static const RENDER:String = "render";
        /** Event type that indicates that the root DisplayObject has been created. */
        public static const ROOT_CREATED:String = "rootCreated";
        /** Event type for an animated object that requests to be removed from the juggler. */
        public static const REMOVE_FROM_JUGGLER:String = "removeFromJuggler";
        /** Event type that is dispatched by the AssetManager after a context loss. */
        public static const TEXTURES_RESTORED:String = "texturesRestored";
        /** Event type that is dispatched by the AssetManager when a file/url cannot be loaded. */
        public static const IO_ERROR:String = "ioError";
        /** Event type that is dispatched by the AssetManager when a file/url cannot be loaded. */
        public static const SECURITY_ERROR:String = "securityError";
        /** Event type that is dispatched by the AssetManager when an xml or json file couldn't
         *  be parsed. */
        public static const PARSE_ERROR:String = "parseError";
        /** Event type that is dispatched by the Starling instance when it encounters a problem
         *  from which it cannot recover, e.g. a lost device context. */
        public static const FATAL_ERROR:String = "fatalError";

        /** An event type to be utilized in custom events. Not used by Starling right now. */
        public static const CHANGE:String = "change";
        /** An event type to be utilized in custom events. Not used by Starling right now. */
        public static const CANCEL:String = "cancel";
        /** An event type to be utilized in custom events. Not used by Starling right now. */
        public static const SCROLL:String = "scroll";
        /** An event type to be utilized in custom events. Not used by Starling right now. */
        public static const OPEN:String = "open";
        /** An event type to be utilized in custom events. Not used by Starling right now. */
        public static const CLOSE:String = "close";
        /** An event type to be utilized in custom events. Not used by Starling right now. */
        public static const SELECT:String = "select";
        /** An event type to be utilized in custom events. Not used by Starling right now. */
        public static const READY:String = "ready";
        /** An event type to be utilized in custom events. Not used by Starling right now. */
        public static const UPDATE:String = "update";
        
        private static var sEventPool:Vector.<Event> = new <Event>[];
        
        private var _target:EventDispatcher;
        private var _currentTarget:EventDispatcher;
        private var _type:String;
        private var _bubbles:Boolean;
        private var _stopsPropagation:Boolean;
        private var _stopsImmediatePropagation:Boolean;
        private var _data:Object;
        
        /** Creates an event object that can be passed to listeners. */
        public function Event(type:String, bubbles:Boolean=false, data:Object=null)
        {
            _type = type;
            _bubbles = bubbles;
            _data = data;
        }
        
        /** Prevents listeners at the next bubble stage from receiving the event. */
        public function stopPropagation():void
        {
            _stopsPropagation = true;
        }
        
        /** Prevents any other listeners from receiving the event. */
        public function stopImmediatePropagation():void
        {
            _stopsPropagation = _stopsImmediatePropagation = true;
        }
        
        /** Returns a description of the event, containing type and bubble information. */
        public function toString():String
        {
            return StringUtil.format("[{0} type=\"{1}\" bubbles={2}]",
                getQualifiedClassName(this).split("::").pop(), _type, _bubbles);
        }
        
        /** Indicates if event will bubble. */
        public function get bubbles():Boolean { return _bubbles; }
        
        /** The object that dispatched the event. */
        public function get target():EventDispatcher { return _target; }
        
        /** The object the event is currently bubbling at. */
        public function get currentTarget():EventDispatcher { return _currentTarget; }
        
        /** A string that identifies the event. */
        public function get type():String { return _type; }
        
        /** Arbitrary data that is attached to the event. */
        public function get data():Object { return _data; }
        
        // properties for internal use
        
        /** @private */
        internal function setTarget(value:EventDispatcher):void { _target = value; }
        
        /** @private */
        internal function setCurrentTarget(value:EventDispatcher):void { _currentTarget = value; }
        
        /** @private */
        internal function setData(value:Object):void { _data = value; }
        
        /** @private */
        internal function get stopsPropagation():Boolean { return _stopsPropagation; }
        
        /** @private */
        internal function get stopsImmediatePropagation():Boolean { return _stopsImmediatePropagation; }
        
        // event pooling
        
        /** @private */
        starling_internal static function fromPool(type:String, bubbles:Boolean=false, data:Object=null):Event
        {
            if (sEventPool.length) return sEventPool.pop().reset(type, bubbles, data);
            else return new Event(type, bubbles, data);
        }
        
        /** @private */
        starling_internal static function toPool(event:Event):void
        {
            event._data = event._target = event._currentTarget = null;
            sEventPool[sEventPool.length] = event; // avoiding 'push'
        }
        
        /** @private */
        starling_internal function reset(type:String, bubbles:Boolean=false, data:Object=null):Event
        {
            _type = type;
            _bubbles = bubbles;
            _data = data;
            _target = _currentTarget = null;
            _stopsPropagation = _stopsImmediatePropagation = false;
            return this;
        }
    }
}