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
    import flash.display3D.textures.TextureBase;
    import flash.errors.IllegalOperationError;
    import flash.geom.Matrix;
    import flash.geom.Rectangle;
    import flash.utils.Dictionary;

    import starling.core.Starling;
    import starling.display.BlendMode;
    import starling.display.DisplayObject;
    import starling.display.Image;
    import starling.filters.FragmentFilter;
    import starling.rendering.Painter;
    import starling.rendering.RenderState;
    import starling.utils.execute;

    /** A RenderTexture is a dynamic texture onto which you can draw any display object.
     * 
     *  <p>After creating a render texture, just call the <code>drawObject</code> method to render 
     *  an object directly onto the texture. The object will be drawn onto the texture at its current
     *  position, adhering its current rotation, scale and alpha properties.</p> 
     *  
     *  <p>Drawing is done very efficiently, as it is happening directly in graphics memory. After 
     *  you have drawn objects onto the texture, the performance will be just like that of a normal 
     *  texture — no matter how many objects you have drawn.</p>
     *  
     *  <p>If you draw lots of objects at once, it is recommended to bundle the drawing calls in 
     *  a block via the <code>drawBundled</code> method, like shown below. That will speed it up 
     *  immensely, allowing you to draw hundreds of objects very quickly.</p>
     *  
     * 	<pre>
     *  renderTexture.drawBundled(function():void
     *  {
     *     for (var i:int=0; i&lt;numDrawings; ++i)
     *     {
     *         image.rotation = (2 &#42; Math.PI / numDrawings) &#42; i;
     *         renderTexture.draw(image);
     *     }   
     *  });
     *  </pre>
     *  
     *  <p>To erase parts of a render texture, you can use any display object like a "rubber" by
     *  setting its blending mode to <code>BlendMode.ERASE</code>. To wipe it completely clean,
     *  use the <code>clear</code> method.</p>
     * 
     *  <strong>Persistence</strong>
     *
     *  <p>Older devices may require double buffering to support persistent render textures. Thus,
     *  you should disable the <code>persistent</code> parameter in the constructor if you only
     *  need to make one draw operation on the texture. The static <code>useDoubleBuffering</code>
     *  property allows you to customize if new textures will be created with or without double
     *  buffering.</p>
     *
     *  <strong>Context Loss</strong>
     *
     *  <p>Unfortunately, render textures are wiped clean when the render context is lost.
     *  This means that you need to manually recreate all their contents in such a case.
     *  One way to do that is by using the <code>root.onRestore</code> callback, like here:</p>
     *
     *  <listing>
     *  renderTexture.root.onRestore = function():void
     *  {
     *      var quad:Quad = new Quad(100, 100, 0xff00ff);
     *      renderTexture.clear(); // required on texture restoration
     *      renderTexture.draw(quad);
     *  });</listing>
     *
     *  <p>For example, a drawing app would need to store information about all draw operations
     *  when they occur, and then recreate them inside <code>onRestore</code> on a context loss
     *  (preferably using <code>drawBundled</code> instead).</p>
     *
     *  <p>However, there is one problem: when that callback is executed, it's very likely that
     *  not all of your textures are already available, since they need to be restored, too (and
     *  that might take a while). You probably loaded your textures with the "AssetManager".
     *  In that case, you can listen to its <code>TEXTURES_RESTORED</code> event instead:</p>
     *
     *  <listing>
     *  assetManager.addEventListener(Event.TEXTURES_RESTORED, function():void
     *  {
     *      var brush:Image = new Image(assetManager.getTexture("brush"));
     *      renderTexture.draw(brush);
     *  });</listing>
     *
     *  <p>[Note that this time, there is no need to call <code>clear</code>, because that's the
     *  default behavior of <code>onRestore</code>, anyway — and we didn't modify that.]</p>
     *
     */
    public class RenderTexture extends SubTexture
    {
        private static const USE_DOUBLE_BUFFERING_DATA_NAME:String =
            "starling.textures.RenderTexture.useDoubleBuffering";

        private var _activeTexture:Texture;
        private var _bufferTexture:Texture;
        private var _helperImage:Image;
        private var _drawing:Boolean;
        private var _bufferReady:Boolean;
        private var _isPersistent:Boolean;

        // helper object
        private static var sClipRect:Rectangle = new Rectangle();
        
        /** Creates a new RenderTexture with a certain size (in points). If the texture is
         *  persistent, its contents remains intact after each draw call, allowing you to use the
         *  texture just like a canvas. If it is not, it will be cleared before each draw call.
         *
         *  <p>Non-persistent textures can be used more efficiently on older devices; on modern
         *  hardware, it does not make a difference. For more information, have a look at the
         *  documentation of the <code>useDoubleBuffering</code> property.</p>
         */
        public function RenderTexture(width:int, height:int, persistent:Boolean=true,
                                      scale:Number=-1, format:String="bgra")
        {
            _isPersistent = persistent;
            _activeTexture = Texture.empty(width, height, true, false, true, scale, format);
            _activeTexture.root.onRestore = _activeTexture.root.clear;

            super(_activeTexture, new Rectangle(0, 0, width, height), true, null, false);

            if (persistent && useDoubleBuffering)
            {
                _bufferTexture = Texture.empty(width, height, true, false, true, scale, format);
                _bufferTexture.root.onRestore = _bufferTexture.root.clear;
                _helperImage = new Image(_bufferTexture);
                _helperImage.textureSmoothing = TextureSmoothing.NONE; // solves some aliasing-issues
            }
        }
        
        /** @inheritDoc */
        public override function dispose():void
        {
            _activeTexture.dispose();
            
            if (isDoubleBuffered)
            {
                _bufferTexture.dispose();
                _helperImage.dispose();
            }
            
            super.dispose();
        }
        
        /** Draws an object into the texture. Note that any filters on the object will currently
         *  be ignored.
         * 
         *  @param object       The object to draw.
         *  @param matrix       If 'matrix' is null, the object will be drawn adhering its 
         *                      properties for position, scale, and rotation. If it is not null,
         *                      the object will be drawn in the orientation depicted by the matrix.
         *  @param alpha        The object's alpha value will be multiplied with this value.
         *  @param antiAliasing Values range from 0 (no antialiasing) to 4 (best quality).
         *                      Beginning with AIR 22, this feature is supported on all platforms
         *                      (except for software rendering mode).
         */
        public function draw(object:DisplayObject, matrix:Matrix=null, alpha:Number=1.0,
                             antiAliasing:int=0):void
        {
            if (object == null) return;
            
            if (_drawing)
                render(object, matrix, alpha);
            else
                renderBundled(render, object, matrix, alpha, antiAliasing);
        }
        
        /** Bundles several calls to <code>draw</code> together in a block. This avoids buffer 
         *  switches and allows you to draw multiple objects into a non-persistent texture.
         *  Note that the 'antiAliasing' setting provided here overrides those provided in
         *  individual 'draw' calls.
         *  
         *  @param drawingBlock  a callback with the form: <pre>function():void;</pre>
         *  @param antiAliasing  Values range from 0 (no antialiasing) to 4 (best quality).
         *                       Beginning with AIR 22, this feature is supported on all platforms
         *                       (except for software rendering mode).
         */
        public function drawBundled(drawingBlock:Function, antiAliasing:int=0):void
        {
            renderBundled(drawingBlock, null, null, 1.0, antiAliasing);
        }
        
        private function render(object:DisplayObject, matrix:Matrix=null, alpha:Number=1.0):void
        {
            var painter:Painter = Starling.painter;
            var state:RenderState = painter.state;
            var wasCacheEnabled:Boolean = painter.cacheEnabled;
            var filter:FragmentFilter = object.filter;
            var mask:DisplayObject = object.mask;

            painter.cacheEnabled = false;
            painter.pushState();

            state.alpha = object.alpha * alpha;
            state.setModelviewMatricesToIdentity();
            state.blendMode = object.blendMode == BlendMode.AUTO ?
                BlendMode.NORMAL : object.blendMode;

            if (matrix) state.transformModelviewMatrix(matrix);
            else        state.transformModelviewMatrix(object.transformationMatrix);

            if (mask)   painter.drawMask(mask);

            if (filter) filter.render(painter);
            else        object.render(painter);

            if (mask)   painter.eraseMask(mask);

            painter.popState();
            painter.cacheEnabled = wasCacheEnabled;
        }
        
        private function renderBundled(renderBlock:Function, object:DisplayObject=null,
                                       matrix:Matrix=null, alpha:Number=1.0,
                                       antiAliasing:int=0):void
        {
            var painter:Painter = Starling.painter;
            var state:RenderState = painter.state;

            if (!Starling.current.contextValid) return;

            // switch buffers
            if (isDoubleBuffered)
            {
                var tmpTexture:Texture = _activeTexture;
                _activeTexture = _bufferTexture;
                _bufferTexture = tmpTexture;
                _helperImage.texture = _bufferTexture;
            }

            painter.pushState();

            var rootTexture:Texture = _activeTexture.root;
            state.setProjectionMatrix(0, 0, rootTexture.width, rootTexture.height, width, height);

            // limit drawing to relevant area
            sClipRect.setTo(0, 0, _activeTexture.width, _activeTexture.height);

            state.clipRect = sClipRect;
            state.setRenderTarget(_activeTexture, true, antiAliasing);
            painter.prepareToDraw();
            
            if (isDoubleBuffered || !isPersistent || !_bufferReady)
                painter.clear();

            // draw buffer
            if (isDoubleBuffered && _bufferReady)
                _helperImage.render(painter);
            else
                _bufferReady = true;
            
            try
            {
                _drawing = true;
                execute(renderBlock, object, matrix, alpha);
            }
            finally
            {
                _drawing = false;
                painter.popState();
            }
        }
        
        /** Clears the render texture with a certain color and alpha value. Call without any
         *  arguments to restore full transparency. */
        public function clear(color:uint=0, alpha:Number=0.0):void
        {
            _activeTexture.root.clear(color, alpha);
            _bufferReady = true;
        }

        // properties

        /** Indicates if the render texture is using double buffering. This might be necessary for
         *  persistent textures, depending on the runtime version and the value of
         *  'forceDoubleBuffering'. */
        private function get isDoubleBuffered():Boolean { return _bufferTexture != null; }

        /** Indicates if the texture is persistent over multiple draw calls. */
        public function get isPersistent():Boolean { return _isPersistent; }
        
        /** @inheritDoc */
        public override function get base():TextureBase { return _activeTexture.base; }
        
        /** @inheritDoc */
        public override function get root():ConcreteTexture { return _activeTexture.root; }

        /** Indicates if new persistent textures should use double buffering. Single buffering
         *  is faster and requires less memory, but is not supported on all hardware.
         *
         *  <p>By default, applications running with the profile "baseline" or "baselineConstrained"
         *  will use double buffering; all others use just a single buffer. You can override this
         *  behavior, though, by assigning a different value at runtime.</p>
         *
         *  @default true for "baseline" and "baselineConstrained", false otherwise
         */
        public static function get useDoubleBuffering():Boolean
        {
            if (Starling.current)
            {
                var painter:Painter = Starling.painter;
                var sharedData:Dictionary = painter.sharedData;

                if (USE_DOUBLE_BUFFERING_DATA_NAME in sharedData)
                {
                    return sharedData[USE_DOUBLE_BUFFERING_DATA_NAME];
                }
                else
                {
                    var profile:String = painter.profile ? painter.profile : "baseline";
                    var value:Boolean = profile == "baseline" || profile == "baselineConstrained";
                    sharedData[USE_DOUBLE_BUFFERING_DATA_NAME] = value;
                    return value;
                }
            }
            else return false;
        }

        public static function set useDoubleBuffering(value:Boolean):void
        {
            if (Starling.current == null)
                throw new IllegalOperationError("Starling not yet initialized");
            else
                Starling.painter.sharedData[USE_DOUBLE_BUFFERING_DATA_NAME] = value;
        }
    }
}