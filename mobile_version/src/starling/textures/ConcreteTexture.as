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
    import flash.display.Bitmap;
    import flash.display.BitmapData;
    import flash.display3D.textures.TextureBase;
    import flash.media.Camera;
    import flash.net.NetStream;
    import flash.system.Capabilities;
    import flash.utils.ByteArray;
    import flash.utils.getQualifiedClassName;

    import starling.core.Starling;
    import starling.core.starling_internal;
    import starling.errors.AbstractClassError;
    import starling.errors.AbstractMethodError;
    import starling.errors.NotSupportedError;
    import starling.events.Event;
    import starling.rendering.Painter;
    import starling.utils.Color;
    import starling.utils.execute;

    /** A ConcreteTexture wraps a Stage3D texture object, storing the properties of the texture
     *  and providing utility methods for data upload, etc.
     *
     *  <p>This class cannot be instantiated directly; create instances using
     *  <code>Texture.fromTextureBase</code> instead. However, that's only necessary when
     *  you need to wrap a <code>TextureBase</code> object in a Starling texture;
     *  the preferred way of creating textures is to use one of the other
     *  <code>Texture.from...</code> factory methods in the <code>Texture</code> class.</p>
     *
     *  @see Texture
     */
    public class ConcreteTexture extends Texture
    {
        private var _base:TextureBase;
        private var _format:String;
        private var _width:int;
        private var _height:int;
        private var _mipMapping:Boolean;
        private var _premultipliedAlpha:Boolean;
        private var _optimizedForRenderTexture:Boolean;
        private var _scale:Number;
        private var _onRestore:Function;
        private var _dataUploaded:Boolean;

        /** @private
         *
         *  Creates a ConcreteTexture object from a TextureBase, storing information about size,
         *  mip-mapping, and if the channels contain premultiplied alpha values. May only be
         *  called from subclasses.
         *
         *  <p>Note that <code>width</code> and <code>height</code> are expected in pixels,
         *  i.e. they do not take the scale factor into account.</p>
         */
        public function ConcreteTexture(base:TextureBase, format:String, width:int, height:int,
                                        mipMapping:Boolean, premultipliedAlpha:Boolean,
                                        optimizedForRenderTexture:Boolean=false, scale:Number=1)
        {
            if (Capabilities.isDebugger &&
                getQualifiedClassName(this) == "starling.textures::ConcreteTexture")
            {
                throw new AbstractClassError();
            }

            _scale = scale <= 0 ? 1.0 : scale;
            _base = base;
            _format = format;
            _width = width;
            _height = height;
            _mipMapping = mipMapping;
            _premultipliedAlpha = premultipliedAlpha;
            _optimizedForRenderTexture = optimizedForRenderTexture;
            _onRestore = null;
            _dataUploaded = false;
        }
        
        /** Disposes the TextureBase object. */
        public override function dispose():void
        {
            if (_base) _base.dispose();

            this.onRestore = null; // removes event listener
            super.dispose();
        }
        
        // texture data upload
        
        /** Uploads a bitmap to the texture. The existing contents will be replaced.
         *  If the size of the bitmap does not match the size of the texture, the bitmap will be
         *  cropped or filled up with transparent pixels */
        public function uploadBitmap(bitmap:Bitmap):void
        {
            uploadBitmapData(bitmap.bitmapData);
        }
        
        /** Uploads bitmap data to the texture. The existing contents will be replaced.
         *  If the size of the bitmap does not match the size of the texture, the bitmap will be
         *  cropped or filled up with transparent pixels */
        public function uploadBitmapData(data:BitmapData):void
        {
            throw new NotSupportedError();
        }
        
        /** Uploads ATF data from a ByteArray to the texture. Note that the size of the
         *  ATF-encoded data must be exactly the same as the original texture size.
         *  
         *  <p>The 'async' parameter may be either a boolean value or a callback function.
         *  If it's <code>false</code> or <code>null</code>, the texture will be decoded
         *  synchronously and will be visible right away. If it's <code>true</code> or a function,
         *  the data will be decoded asynchronously. The texture will remain unchanged until the
         *  upload is complete, at which time the callback function will be executed. This is the
         *  expected function definition: <code>function(texture:Texture):void;</code></p>
         */
        public function uploadAtfData(data:ByteArray, offset:int=0, async:*=null):void
        {
            throw new NotSupportedError();
        }

        /** Specifies a video stream to be rendered within the texture. */
        public function attachNetStream(netStream:NetStream, onComplete:Function=null):void
        {
            attachVideo("NetStream", netStream, onComplete);
        }

        /** Specifies a video stream from a camera to be rendered within the texture. */
        public function attachCamera(camera:Camera, onComplete:Function=null):void
        {
            attachVideo("Camera", camera, onComplete);
        }

        /** @private */
        internal function attachVideo(type:String, attachment:Object, onComplete:Function=null):void
        {
            throw new NotSupportedError();
        }

        // texture backup (context loss)
        
        private function onContextCreated():void
        {
            _dataUploaded = false;
            _base = createBase();      // recreate the underlying texture
            execute(_onRestore, this); // restore contents

            // if no texture has been uploaded above, we init the texture with transparent pixels.
            if (!_dataUploaded) clear();
        }
        
        /** Recreates the underlying Stage3D texture object with the same dimensions and attributes
         *  as the one that was passed to the constructor. You have to upload new data before the
         *  texture becomes usable again. Beware: this method does <strong>not</strong> dispose
         *  the current base. */
        protected function createBase():TextureBase
        {
            throw new AbstractMethodError();
        }

        /** Recreates the underlying Stage3D texture. May be used to manually restore a texture.
         *  Beware that new data needs to be uploaded to the texture before it can be used. */
        starling_internal function recreateBase():void
        {
            _base = createBase();
        }
        
        /** Clears the texture with a certain color and alpha value. The previous contents of the
         *  texture is wiped out. */
        public function clear(color:uint=0x0, alpha:Number=0.0):void
        {
            if (_premultipliedAlpha && alpha < 1.0)
                color = Color.rgb(Color.getRed(color)   * alpha,
                                  Color.getGreen(color) * alpha,
                                  Color.getBlue(color)  * alpha);

            var painter:Painter = Starling.painter;
            painter.pushState();
            painter.state.renderTarget = this;

            // we wrap the clear call in a try/catch block as a workaround for a problem of
            // FP 11.8 plugin/projector: calling clear on a compressed texture doesn't work there
            // (while it *does* work on iOS + Android).
            
            try { painter.clear(color, alpha); }
            catch (e:Error) {}
            
            painter.popState();
            setDataUploaded();
        }

        /** Notifies the instance that the base texture may now be used for rendering. */
        protected function setDataUploaded():void
        {
            _dataUploaded = true;
        }

        // properties
        
        /** Indicates if the base texture was optimized for being used in a render texture. */
        public function get optimizedForRenderTexture():Boolean { return _optimizedForRenderTexture; }

        /** Indicates if the base texture is a standard power-of-two dimensioned texture of type
         *  <code>flash.display3D.textures.Texture</code>. */
        public function get isPotTexture():Boolean { return false; }
        
        /** The function that you provide here will be called after a context loss.
         *  On execution, a new base texture will already have been created; however,
         *  it will be empty. Call one of the "upload..." methods from within the callback
         *  to restore the actual texture data.
         *
         *  <listing>
         *  var texture:Texture = Texture.fromBitmap(new EmbeddedBitmap());
         *  texture.root.onRestore = function():void
         *  {
         *      texture.root.uploadFromBitmap(new EmbeddedBitmap());
         *  };</listing>
         */
        public function get onRestore():Function { return _onRestore; }
        public function set onRestore(value:Function):void
        {
            Starling.current.removeEventListener(Event.CONTEXT3D_CREATE, onContextCreated);
            
            if (value != null)
            {
                _onRestore = value;
                Starling.current.addEventListener(Event.CONTEXT3D_CREATE, onContextCreated);
            }
            else _onRestore = null;
        }
        
        /** @inheritDoc */
        public override function get base():TextureBase { return _base; }
        
        /** @inheritDoc */
        public override function get root():ConcreteTexture { return this; }
        
        /** @inheritDoc */
        public override function get format():String { return _format; }
        
        /** @inheritDoc */
        public override function get width():Number  { return _width / _scale;  }
        
        /** @inheritDoc */
        public override function get height():Number { return _height / _scale; }
        
        /** @inheritDoc */
        public override function get nativeWidth():Number { return _width; }
        
        /** @inheritDoc */
        public override function get nativeHeight():Number { return _height; }
        
        /** @inheritDoc */
        public override function get scale():Number { return _scale; }
        
        /** @inheritDoc */
        public override function get mipMapping():Boolean { return _mipMapping; }
        
        /** @inheritDoc */
        public override function get premultipliedAlpha():Boolean { return _premultipliedAlpha; }
    }
}