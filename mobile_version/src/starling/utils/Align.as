// =================================================================================================
//
//	Starling Framework
//	Copyright Gamua GmbH. All Rights Reserved.
//
//	This program is free software. You can redistribute and/or modify it
//	in accordance with the terms of the accompanying license agreement.
//
// =================================================================================================

package starling.utils
{
    import starling.errors.AbstractClassError;

    /** A class that provides constant values for horizontal and vertical alignment of objects. */
    public final class Align
    {
        /** @private */
        public function Align() { throw new AbstractClassError(); }
        
        /** Horizontal left alignment. */
        public static const LEFT:String   = "left";
        
        /** Horizontal right alignment. */
        public static const RIGHT:String  = "right";

        /** Vertical top alignment. */
        public static const TOP:String = "top";

        /** Vertical bottom alignment. */
        public static const BOTTOM:String = "bottom";

        /** Centered alignment. */
        public static const CENTER:String = "center";
        
        /** Indicates whether the given alignment string is valid. */
        public static function isValid(align:String):Boolean
        {
            return align == LEFT || align == RIGHT || align == CENTER ||
                   align == TOP  || align == BOTTOM;
        }

        /** Indicates if the given string is a valid horizontal alignment. */
        public static function isValidHorizontal(align:String):Boolean
        {
            return align == LEFT || align == CENTER || align == RIGHT;
        }

        /** Indicates if the given string is a valid vertical alignment. */
        public static function isValidVertical(align:String):Boolean
        {
            return align == TOP || align == CENTER || align == BOTTOM;
        }
    }
}