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

    /** A utility class with methods related to the String class. */
    public class StringUtil
    {
        /** @private */
        public function StringUtil() { throw new AbstractClassError(); }

        /** Formats a String in .Net-style, with curly braces ("{0}"). Does not support any
         *  number formatting options yet. */
        public static function format(format:String, ...args):String
        {
            // TODO: add number formatting options

            for (var i:int=0; i<args.length; ++i)
                format = format.replace(new RegExp("\\{"+i+"\\}", "g"), args[i]);

            return format;
        }

        /** Replaces a string's "master string" — the string it was built from —
         *  with a single character to save memory. Find more information about this AS3 oddity
         *  <a href="http://jacksondunstan.com/articles/2260">here</a>.
         *
         *  @param  string The String to clean
         *  @return The input string, but with a master string only one character larger than it.
         *  @author Jackson Dunstan, JacksonDunstan.com
         */
        public static function clean(string:String):String
        {
            return ("_" + string).substr(1);
        }

        /** Removes all leading white-space and control characters from the given String.
         *
         *  <p>Beware: this method does not make a proper Unicode white-space check,
         *  but simply trims all character codes of '0x20' or below.</p>
         */
        public static function trimStart(string:String):String
        {
            var pos:int;
            var length:int = string.length;

            for (pos = 0; pos < length; ++pos)
                if (string.charCodeAt(pos) > 0x20) break;

            return string.substring(pos, length);
        }

        /** Removes all trailing white-space and control characters from the given String.
         *
         *  <p>Beware: this method does not make a proper Unicode white-space check,
         *  but simply trims all character codes of '0x20' or below.</p>
         */
        public static function trimEnd(string:String):String
        {
            for (var pos:int = string.length - 1; pos >= 0; --pos)
                if (string.charCodeAt(pos) > 0x20) break;

            return string.substring(0, pos + 1);
        }

        /** Removes all leading and trailing white-space and control characters from the given
         *  String.
         *
         *  <p>Beware: this method does not make a proper Unicode white-space check,
         *  but simply trims all character codes of '0x20' or below.</p>
         */
        public static function trim(string:String):String
        {
            var startPos:int, endPos:int;
            var length:int = string.length;

            for (startPos = 0; startPos < length; ++startPos)
                if (string.charCodeAt(startPos) > 0x20) break;

            for (endPos = string.length - 1; endPos >= startPos; --endPos)
                if (string.charCodeAt(endPos) > 0x20) break;

            return string.substring(startPos, endPos + 1);
        }
    }
}
