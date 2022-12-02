// =================================================================================================
//
//	Starling Framework
//	Copyright Gamua GmbH. All Rights Reserved.
//
//	This program is free software. You can redistribute and/or modify it
//	in accordance with the terms of the accompanying license agreement.
//
// =================================================================================================

package starling.errors
{
    /** A NotSupportedError is thrown when you attempt to use a feature that is not supported
     *  on the current platform. */
    public class NotSupportedError extends Error
    {
        /** Creates a new NotSupportedError object. */
        public function NotSupportedError(message:* = "", id:* = 0)
        {
            super(message, id);
        }
    }
}
