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
    /** An EnterFrameEvent is triggered once per frame and is dispatched to all objects in the
     *  display tree.
     *
     *  It contains information about the time that has passed since the last frame. That way, you 
     *  can easily make animations that are independent of the frame rate, taking the passed time
     *  into account.
     */ 
    public class EnterFrameEvent extends Event
    {
        /** Event type for a display object that is entering a new frame. */
        public static const ENTER_FRAME:String = "enterFrame";
        
        /** Creates an enter frame event with the passed time. */
        public function EnterFrameEvent(type:String, passedTime:Number, bubbles:Boolean=false)
        {
            super(type, bubbles, passedTime);
        }
        
        /** The time that has passed since the last frame (in seconds). */
        public function get passedTime():Number { return data as Number; }
    }
}