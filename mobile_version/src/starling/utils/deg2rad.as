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
    /** Converts an angle from degrees into radians. */
    public function deg2rad(deg:Number):Number
    {
        return deg / 180.0 * Math.PI;   
    }
}