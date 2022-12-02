// =================================================================================================
//
//	Starling Framework
//	Copyright Gamua GmbH. All Rights Reserved.
//
//	This program is free software. You can redistribute and/or modify it
//	in accordance with the terms of the accompanying license agreement.
//
// =================================================================================================

package starling.rendering
{
    /** Holds the properties of a single attribute in a VertexDataFormat instance.
     *  The member variables must never be changed; they are only <code>public</code>
     *  for performance reasons. */
    internal class VertexDataAttribute
    {
        private static const FORMAT_SIZES:Object = {
            "bytes4": 4,
            "float1": 4,
            "float2": 8,
            "float3": 12,
            "float4": 16
        };

        public var name:String;
        public var format:String;
        public var isColor:Boolean;
        public var offset:int; // in bytes
        public var size:int;   // in bytes

        /** Creates a new instance with the given properties. */
        public function VertexDataAttribute(name:String, format:String, offset:int)
        {
            if (!(format in FORMAT_SIZES))
                throw new ArgumentError(
                    "Invalid attribute format: " + format + ". " +
                    "Use one of the following: 'float1'-'float4', 'bytes4'");

            this.name = name;
            this.format = format;
            this.offset = offset;
            this.size = FORMAT_SIZES[format];
            this.isColor = name.indexOf("color") != -1 || name.indexOf("Color") != -1
        }
    }
}
