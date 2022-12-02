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
    import flash.display3D.textures.TextureBase;
    import flash.events.Event;
    import flash.geom.Matrix;
    import flash.geom.Point;
    import flash.geom.Rectangle;
    import flash.utils.ByteArray;

    import starling.core.Starling;
    import starling.utils.MathUtil;
    import starling.utils.execute;

    /** @private
     *
     *  A concrete texture that wraps a <code>Texture</code> base.
     *  For internal use only. */
    internal class ConcretePotTexture extends ConcreteTexture
    {
        private var _textureReadyCallback:Function;

        private static var sMatrix:Matrix = new Matrix();
        private static var sRectangle:Rectangle = new Rectangle();
        private static var sOrigin:Point = new Point();

        /** Creates a new instance with the given parameters. */
        public function ConcretePotTexture(base:flash.display3D.textures.Texture, format:String,
                                           width:int, height:int, mipMapping:Boolean,
                                           premultipliedAlpha:Boolean,
                                           optimizedForRenderTexture:Boolean=false, scale:Number=1)
        {
            super(base, format, width, height, mipMapping, premultipliedAlpha,
                  optimizedForRenderTexture, scale);

            if (width != MathUtil.getNextPowerOfTwo(width))
                throw new ArgumentError("width must be a power of two");

            if (height != MathUtil.getNextPowerOfTwo(height))
                throw new ArgumentError("height must be a power of two");
        }

        /** @inheritDoc */
        override public function dispose():void
        {
            base.removeEventListener(Event.TEXTURE_READY, onTextureReady);
            super.dispose();
        }

        /** @inheritDoc */
        override protected function createBase():TextureBase
        {
            return Starling.context.createTexture(
                    nativeWidth, nativeHeight, format, optimizedForRenderTexture);
        }

        /** @inheritDoc */
        override public function uploadBitmapData(data:BitmapData):void
        {
            potBase.uploadFromBitmapData(data);

            var buffer:BitmapData = null;

            if (data.width != nativeWidth || data.height != nativeHeight)
            {
                buffer = new BitmapData(nativeWidth, nativeHeight, true, 0);
                buffer.copyPixels(data, data.rect, sOrigin);
                data = buffer;
            }

            if (mipMapping && data.width > 1 && data.height > 1)
            {
                var currentWidth:int  = data.width  >> 1;
                var currentHeight:int = data.height >> 1;
                var level:int = 1;
                var canvas:BitmapData = new BitmapData(currentWidth, currentHeight, true, 0);
                var bounds:Rectangle = sRectangle;
                var matrix:Matrix = sMatrix;
                matrix.setTo(0.5, 0.0, 0.0, 0.5, 0.0, 0.0);

                while (currentWidth >= 1 || currentHeight >= 1)
                {
                    bounds.setTo(0, 0, currentWidth, currentHeight);
                    canvas.fillRect(bounds, 0);
                    canvas.draw(data, matrix, null, null, null, true);
                    potBase.uploadFromBitmapData(canvas, level++);
                    matrix.scale(0.5, 0.5);
                    currentWidth  = currentWidth  >> 1;
                    currentHeight = currentHeight >> 1;
                }

                canvas.dispose();
            }

            if (buffer) buffer.dispose();

            setDataUploaded();
        }

        /** @inheritDoc */
        override public function get isPotTexture():Boolean { return true; }

        /** @inheritDoc */
        override public function uploadAtfData(data:ByteArray, offset:int = 0, async:* = null):void
        {
            var isAsync:Boolean = async is Function || async === true;

            if (async is Function)
            {
                _textureReadyCallback = async as Function;
                base.addEventListener(Event.TEXTURE_READY, onTextureReady);
            }

            potBase.uploadCompressedTextureFromByteArray(data, offset, isAsync);
            setDataUploaded();
        }

        private function onTextureReady(event:Event):void
        {
            base.removeEventListener(Event.TEXTURE_READY, onTextureReady);
            execute(_textureReadyCallback, this);
            _textureReadyCallback = null;
        }

        private function get potBase():flash.display3D.textures.Texture
        {
            return base as flash.display3D.textures.Texture;
        }
    }
}
