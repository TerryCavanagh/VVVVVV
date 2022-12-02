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
    import flash.display.BitmapData;
    import flash.display3D.textures.RectangleTexture;
    import flash.display3D.textures.TextureBase;

    import starling.core.Starling;

    /** @private
     *
     *  A concrete texture that wraps a <code>RectangleTexture</code> base.
     *  For internal use only. */
    internal class ConcreteRectangleTexture extends ConcreteTexture
    {
        /** Creates a new instance with the given parameters. */
        public function ConcreteRectangleTexture(base:RectangleTexture, format:String,
                                                 width:int, height:int, premultipliedAlpha:Boolean,
                                                 optimizedForRenderTexture:Boolean=false,
                                                 scale:Number=1)
        {
            super(base, format, width, height, false, premultipliedAlpha,
                  optimizedForRenderTexture, scale);
        }

        /** @inheritDoc */
        override public function uploadBitmapData(data:BitmapData):void
        {
            rectangleBase.uploadFromBitmapData(data);
            setDataUploaded();
        }

        /** @inheritDoc */
        override protected function createBase():TextureBase
        {
            return Starling.context.createRectangleTexture(
                    nativeWidth, nativeHeight, format, optimizedForRenderTexture);
        }

        private function get rectangleBase():RectangleTexture
        {
            return base as RectangleTexture;
        }
    }
}
