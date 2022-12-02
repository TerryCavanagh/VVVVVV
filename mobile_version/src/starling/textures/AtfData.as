// =================================================================================================
//
//	Starling Framework
//	Copyright Gamua GmbH. All Rights Reserved.
//
//	This program is free software. You can redistribute and/or modify it
//	in accordance with the terms of the accompanying license agreement.
//
// =================================================================================================

package starling.textures
{
    import flash.display3D.Context3DTextureFormat;
    import flash.utils.ByteArray;

    /** A parser for the ATF data format. */
    public class AtfData
    {
        private var _format:String;
        private var _width:int;
        private var _height:int;
        private var _numTextures:int;
        private var _isCubeMap:Boolean;
        private var _data:ByteArray;
        
        /** Create a new instance by parsing the given byte array. */
        public function AtfData(data:ByteArray)
        {
            if (!isAtfData(data)) throw new ArgumentError("Invalid ATF data");
            
            if (data[6] == 255) data.position = 12; // new file version
            else                data.position =  6; // old file version

            var format:uint = data.readUnsignedByte();
            switch (format & 0x7f)
            {
                case  0:
                case  1: _format = Context3DTextureFormat.BGRA; break;
                case 12:
                case  2:
                case  3: _format = Context3DTextureFormat.COMPRESSED; break;
                case 13:
                case  4:
                case  5: _format = "compressedAlpha"; break; // explicit string for compatibility
                default: throw new Error("Invalid ATF format");
            }
            
            _width = Math.pow(2, data.readUnsignedByte());
            _height = Math.pow(2, data.readUnsignedByte());
            _numTextures = data.readUnsignedByte();
            _isCubeMap = (format & 0x80) != 0;
            _data = data;
            
            // version 2 of the new file format contains information about
            // the "-e" and "-n" parameters of png2atf
            
            if (data[5] != 0 && data[6] == 255)
            {
                var emptyMipmaps:Boolean = (data[5] & 0x01) == 1;
                var numTextures:int  = data[5] >> 1 & 0x7f;
                _numTextures = emptyMipmaps ? 1 : numTextures;
            }
        }

        /** Checks the first 3 bytes of the data for the 'ATF' signature. */
        public static function isAtfData(data:ByteArray):Boolean
        {
            if (data.length < 3) return false;
            else
            {
                var signature:String = String.fromCharCode(data[0], data[1], data[2]);
                return signature == "ATF";
            }
        }

        /** The texture format. @see flash.display3D.textures.Context3DTextureFormat */
        public function get format():String { return _format; }

        /** The width of the texture in pixels. */
        public function get width():int { return _width; }

        /** The height of the texture in pixels. */
        public function get height():int { return _height; }

        /** The number of encoded textures. '1' means that there are no mip maps. */
        public function get numTextures():int { return _numTextures; }

        /** Indicates if the ATF data encodes a cube map. Not supported by Starling! */
        public function get isCubeMap():Boolean { return _isCubeMap; }

        /** The actual byte data, including header. */
        public function get data():ByteArray { return _data; }
    }
}