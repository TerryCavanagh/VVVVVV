// =================================================================================================
//
//	Starling Framework
//	Copyright Gamua GmbH. All Rights Reserved.
//
//	This program is free software. You can redistribute and/or modify it
//	in accordance with the terms of the accompanying license agreement.
//
// =================================================================================================

package starling.text 
{
    import starling.errors.AbstractClassError;

    /** This class is an enumeration of constant values used in setting the 
     *  autoSize property of the TextField class. */ 
    public class TextFieldAutoSize
    {
        /** @private */
        public function TextFieldAutoSize() { throw new AbstractClassError(); }
        
        /** No auto-sizing will happen. */
        public static const NONE:String = "none";
        
        /** The text field will grow/shrink sidewards; no line-breaks will be added.
         *  The height of the text field remains unchanged. Not supported for HTML text! */
        public static const HORIZONTAL:String = "horizontal";
        
        /** The text field will grow/shrink downwards, adding line-breaks when necessary.
          * The width of the text field remains unchanged. */
        public static const VERTICAL:String = "vertical";
        
        /** The text field will grow to the right and bottom; no line-breaks will be added. */
        public static const BOTH_DIRECTIONS:String = "bothDirections";
    }
}