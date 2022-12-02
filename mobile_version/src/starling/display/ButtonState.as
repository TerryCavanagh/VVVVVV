// =================================================================================================
//
//	Starling Framework
//	Copyright Gamua GmbH. All Rights Reserved.
//
//	This program is free software. You can redistribute and/or modify it
//	in accordance with the terms of the accompanying license agreement.
//
// =================================================================================================

package starling.display
{
    import starling.errors.AbstractClassError;

    /** A class that provides constant values for the states of the Button class. */
    public class ButtonState
    {
        /** @private */
        public function ButtonState() { throw new AbstractClassError(); }

        /** The button's default state. */
        public static const UP:String = "up";

        /** The button is pressed. */
        public static const DOWN:String = "down";

        /** The mouse hovers over the button. */
        public static const OVER:String = "over";

        /** The button was disabled altogether. */
        public static const DISABLED:String = "disabled";
    }
}