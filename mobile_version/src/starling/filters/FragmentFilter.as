// =================================================================================================
//
//	Starling Framework
//	Copyright Gamua GmbH. All Rights Reserved.
//
//	This program is free software. You can redistribute and/or modify it
//	in accordance with the terms of the accompanying license agreement.
//
// =================================================================================================

package starling.filters
{
    import flash.display3D.Context3DTextureFormat;
    import flash.errors.IllegalOperationError;
    import flash.geom.Matrix3D;
    import flash.geom.Rectangle;

    import starling.core.Starling;
    import starling.core.starling_internal;
    import starling.display.DisplayObject;
    import starling.display.Stage;
    import starling.events.Event;
    import starling.events.EventDispatcher;
    import starling.rendering.FilterEffect;
    import starling.rendering.IndexData;
    import starling.rendering.Painter;
    import starling.rendering.VertexData;
    import starling.textures.Texture;
    import starling.textures.TextureSmoothing;
    import starling.utils.MatrixUtil;
    import starling.utils.Padding;
    import starling.utils.Pool;
    import starling.utils.RectangleUtil;

    /** Dispatched when the settings change in a way that requires a redraw. */
    [Event(name="change", type="starling.events.Event")]

    /** Dispatched every frame on filters assigned to display objects connected to the stage. */
    [Event(name="enterFrame", type="starling.events.EnterFrameEvent")]

    /** The FragmentFilter class is the base class for all filter effects in Starling.
     *  All filters must extend this class. You can attach them to any display object through the
     *  <code>filter</code> property.
     *
     *  <p>A fragment filter works in the following way:</p>
     *  <ol>
     *    <li>The object to be filtered is rendered into a texture.</li>
     *    <li>That texture is passed to the <code>process</code> method.</li>
     *    <li>This method processes the texture using a <code>FilterEffect</code> subclass
     *        that processes the input via fragment and vertex shaders to achieve a certain
     *        effect.</li>
     *    <li>If the filter requires several passes, the process method may execute the
     *        effect several times, or even make use of other filters in the process.</li>
     *    <li>In the end, a quad with the output texture is added to the batch renderer.
     *        In the next frame, if the object hasn't changed, the filter is drawn directly
     *        from the render cache.</li>
     *    <li>Alternatively, the last pass may be drawn directly to the back buffer. That saves
     *        one draw call, but means that the object may not be drawn from the render cache in
     *        the next frame. Starling makes an educated guess if that makes sense, but you can
     *        also force it to do so via the <code>alwaysDrawToBackBuffer</code> property.</li>
     *  </ol>
     *
     *  <p>All of this is set up by the basic FragmentFilter class. Concrete subclasses
     *  just need to override the protected method <code>createEffect</code> and (optionally)
     *  <code>process</code>. Multi-pass filters must also override <code>numPasses</code>.</p>
     *
     *  <p>Typically, any properties on the filter are just forwarded to an effect instance,
     *  which is then used automatically by <code>process</code> to render the filter pass.
     *  For a simple example on how to write a single-pass filter, look at the implementation of
     *  the <code>ColorMatrixFilter</code>; for a composite filter (i.e. a filter that combines
     *  several others), look at the <code>GlowFilter</code>.
     *  </p>
     *
     *  <p>Beware that a filter instance may only be used on one object at a time!</p>
     *
     *  <p><strong>Animated filters</strong></p>
     *
     *  <p>The <code>process</code> method of a filter is only called when it's necessary, i.e.
     *  when the filter properties or the target display object changes. This means that you cannot
     *  rely on the method to be called on a regular basis, as needed when creating an animated
     *  filter class. Instead, you can do so by listening for an <code>ENTER_FRAME</code>-event.
     *  It is dispatched on the filter once every frame, as long as the filter is assigned to
     *  a display object that is connected to the stage.</p>
     *
     *  <p><strong>Caching</strong></p>
     *
     *  <p>Per default, whenever the target display object is changed in any way (i.e. the render
     *  cache fails), the filter is reprocessed. However, you can manually cache the filter output
     *  via the method of the same name: this will let the filter redraw the current output texture,
     *  even if the target object changes later on. That's especially useful if you add a filter
     *  to an object that changes only rarely, e.g. a TextField or an Image. Keep in mind, though,
     *  that you have to call <code>cache()</code> again in order for any changes to show up.</p>
     *
     *  @see starling.rendering.FilterEffect
     */
    public class FragmentFilter extends EventDispatcher
    {
        private var _quad:FilterQuad;
        private var _target:DisplayObject;
        private var _effect:FilterEffect;
        private var _vertexData:VertexData;
        private var _indexData:IndexData;
        private var _padding:Padding;
        private var _helper:FilterHelper;
        private var _resolution:Number;
        private var _textureFormat:String;
        private var _textureSmoothing:String;
        private var _alwaysDrawToBackBuffer:Boolean;
        private var _cacheRequested:Boolean;
        private var _cached:Boolean;

        // helpers
        private static var sMatrix3D:Matrix3D;

        /** Creates a new instance. The base class' implementation just draws the unmodified
         *  input texture. */
        public function FragmentFilter()
        {
            _resolution = 1.0;
            _textureFormat = Context3DTextureFormat.BGRA;
            _textureSmoothing = TextureSmoothing.BILINEAR;

            // Handle lost context (using conventional Flash event for weak listener support)
            Starling.current.stage3D.addEventListener(Event.CONTEXT3D_CREATE,
                onContextCreated, false, 0, true);
        }

        /** Disposes all resources that have been created by the filter. */
        public function dispose():void
        {
            Starling.current.stage3D.removeEventListener(Event.CONTEXT3D_CREATE, onContextCreated);

            if (_helper) _helper.dispose();
            if (_effect) _effect.dispose();
            if (_quad)   _quad.dispose();

            _effect = null;
            _quad = null;
        }

        private function onContextCreated(event:Object):void
        {
            setRequiresRedraw();
        }

        /** Renders the filtered target object. Most users will never have to call this manually;
         *  it's executed automatically in the rendering process of the filtered display object.
         */
        public function render(painter:Painter):void
        {
            if (_target == null)
                throw new IllegalOperationError("Cannot render filter without target");

            if (_target.is3D)
                _cached = _cacheRequested = false;

            if (!_cached || _cacheRequested)
            {
                renderPasses(painter, _cacheRequested);
                _cacheRequested = false;
            }
            else if (_quad.visible)
            {
                _quad.render(painter);
            }
        }

        private function renderPasses(painter:Painter, forCache:Boolean):void
        {
            if (_helper  == null) _helper = new FilterHelper(_textureFormat);
            if (_quad  == null) _quad  = new FilterQuad(_textureSmoothing);
            else { _helper.putTexture(_quad.texture); _quad.texture = null; }

            var bounds:Rectangle = Pool.getRectangle(); // might be recursive -> no static var
            var drawLastPassToBackBuffer:Boolean = false;
            var origResolution:Number = _resolution;
            var renderSpace:DisplayObject = _target.stage || _target.parent;
            var isOnStage:Boolean = renderSpace is Stage;
            var stage:Stage = Starling.current.stage;
            var stageBounds:Rectangle;

            if (!forCache && (_alwaysDrawToBackBuffer || _target.requiresRedraw))
            {
                // If 'requiresRedraw' is true, the object is non-static, and we guess that this
                // will be the same in the next frame. So we render directly to the back buffer.
                //
                // -- That, however, is only possible for full alpha values, because
                // (1) 'FilterEffect' can't handle alpha (and that will do the rendering)
                // (2) we don't want lower layers (CompositeFilter!) to shine through.

                drawLastPassToBackBuffer = painter.state.alpha == 1.0;
                painter.excludeFromCache(_target);
            }

            if (_target == Starling.current.root)
            {
                // full-screen filters use exactly the stage bounds
                stage.getStageBounds(_target, bounds);
            }
            else
            {
                // Unfortunately, the following bounds calculation yields the wrong result when
                // drawing a filter to a RenderTexture using a custom matrix. The 'modelviewMatrix'
                // should be used for the bounds calculation, but the API doesn't support this.
                // A future version should change this to: "getBounds(modelviewMatrix, bounds)"

                _target.getBounds(renderSpace, bounds);

                if (!forCache && isOnStage) // normally, we don't need anything outside
                {
                    stageBounds = stage.getStageBounds(null, Pool.getRectangle());
                    RectangleUtil.intersect(bounds, stageBounds, bounds);
                    Pool.putRectangle(stageBounds);
                }
            }

            _quad.visible = !bounds.isEmpty();
            if (!_quad.visible) { Pool.putRectangle(bounds); return; }

            if (_padding) RectangleUtil.extend(bounds,
                _padding.left, _padding.right, _padding.top, _padding.bottom);

            // integer bounds for maximum sharpness + to avoid jiggling
            bounds.setTo(Math.floor(bounds.x),    Math.floor(bounds.y),
                         Math.ceil(bounds.width), Math.ceil(bounds.height));

            _helper.textureScale = Starling.contentScaleFactor * _resolution;
            _helper.projectionMatrix3D = painter.state.projectionMatrix3D;
            _helper.renderTarget = painter.state.renderTarget;
            _helper.targetBounds = bounds;
            _helper.target = _target;
            _helper.start(numPasses, drawLastPassToBackBuffer);

            _quad.setBounds(bounds);
            _resolution = 1.0; // applied via '_helper.textureScale' already;
                               // only 'child'-filters use resolution directly (in 'process')

            var wasCacheEnabled:Boolean = painter.cacheEnabled;
            var input:Texture = _helper.getTexture();
            var output:Texture;

            painter.cacheEnabled = false; // -> what follows should not be cached
            painter.pushState();
            painter.state.alpha = 1.0;
            painter.state.renderTarget = input;
            painter.state.setProjectionMatrix(bounds.x, bounds.y,
                input.root.width, input.root.height,
                stage.stageWidth, stage.stageHeight, stage.cameraPosition);

            _target.render(painter); // -> draw target object into 'input'

            painter.finishMeshBatch();
            painter.state.setModelviewMatricesToIdentity();
            painter.state.clipRect = null;

            output = process(painter, _helper, input); // -> feed 'input' to actual filter code

            painter.popState();
            painter.cacheEnabled = wasCacheEnabled; // -> cache again

            if (output) // indirect rendering
            {
                painter.pushState();

                if (_target.is3D) painter.state.setModelviewMatricesToIdentity(); // -> stage coords
                else              _quad.moveVertices(renderSpace, _target);       // -> local coords

                _quad.texture = output;
                _quad.render(painter);

                painter.finishMeshBatch();
                painter.popState();
            }

            _helper.target = null;
            _helper.putTexture(input);
            _resolution = origResolution;
            Pool.putRectangle(bounds);
        }

        /** Does the actual filter processing. This method will be called with up to four input
         *  textures and must return a new texture (acquired from the <code>helper</code>) that
         *  contains the filtered output. To to do this, it configures the FilterEffect
         *  (provided via <code>createEffect</code>) and calls its <code>render</code> method.
         *
         *  <p>In a standard filter, only <code>input0</code> will contain a texture; that's the
         *  object the filter was applied to, rendered into an appropriately sized texture.
         *  However, filters may also accept multiple textures; that's useful when you need to
         *  combine the output of several filters into one. For example, the DropShadowFilter
         *  uses a BlurFilter to create the shadow and then feeds both input and shadow texture
         *  into a CompositeFilter.</p>
         *
         *  <p>Never create or dispose any textures manually within this method; instead, get
         *  new textures from the provided helper object, and pass them to the helper when you do
         *  not need them any longer. Ownership of both input textures and returned texture
         *  lies at the caller; only temporary textures should be put into the helper.</p>
         */
        public function process(painter:Painter, helper:IFilterHelper,
                                input0:Texture=null, input1:Texture=null,
                                input2:Texture=null, input3:Texture=null):Texture
        {
            var effect:FilterEffect = this.effect;
            var output:Texture = helper.getTexture(_resolution);
            var projectionMatrix:Matrix3D;
            var bounds:Rectangle = null;
            var renderTarget:Texture;

            if (output) // render to texture
            {
                renderTarget = output;
                projectionMatrix = MatrixUtil.createPerspectiveProjectionMatrix(0, 0,
                    output.root.width / _resolution, output.root.height / _resolution,
                    0, 0, null, sMatrix3D);
            }
            else // render to back buffer
            {
                bounds = helper.targetBounds;
                renderTarget = (helper as FilterHelper).renderTarget;
                projectionMatrix = (helper as FilterHelper).projectionMatrix3D;
                effect.textureSmoothing = _textureSmoothing;
            }

            painter.state.renderTarget = renderTarget;
            painter.prepareToDraw();
            painter.drawCount += 1;

            input0.setupVertexPositions(vertexData, 0, "position", bounds);
            input0.setupTextureCoordinates(vertexData);

            effect.texture = input0;
            effect.mvpMatrix3D = projectionMatrix;
            effect.uploadVertexData(vertexData);
            effect.uploadIndexData(indexData);
            effect.render(0, indexData.numTriangles);

            return output;
        }

        /** Creates the effect that does the actual, low-level rendering.
         *  Must be overridden by all subclasses that do any rendering on their own (instead
         *  of just forwarding processing to other filters).
         */
        protected function createEffect():FilterEffect
        {
            return new FilterEffect();
        }

        /** Caches the filter output into a texture.
         *
         *  <p>An uncached filter is rendered every frame (except if it can be rendered from the
         *  global render cache, which happens if the target object does not change its appearance
         *  or location relative to the stage). A cached filter is only rendered once; the output
         *  stays unchanged until you call <code>cache</code> again or change the filter settings.
         *  </p>
         *
         *  <p>Beware: you cannot cache filters on 3D objects; if the object the filter is attached
         *  to is a Sprite3D or has a Sprite3D as (grand-) parent, the request will be silently
         *  ignored. However, you <em>can</em> cache a 2D object that has 3D children!</p>
         */
        public function cache():void
        {
            _cached = _cacheRequested = true;
            setRequiresRedraw();
        }

        /** Clears the cached output of the filter. After calling this method, the filter will be
         *  processed once per frame again. */
        public function clearCache():void
        {
            _cached = _cacheRequested = false;
            setRequiresRedraw();
        }

        // enter frame event

        /** @private */
        override public function addEventListener(type:String, listener:Function):void
        {
            if (type == Event.ENTER_FRAME && _target)
                _target.addEventListener(Event.ENTER_FRAME, onEnterFrame);

            super.addEventListener(type, listener);
        }

        /** @private */
        override public function removeEventListener(type:String, listener:Function):void
        {
            if (type == Event.ENTER_FRAME && _target)
                _target.removeEventListener(type, onEnterFrame);

            super.removeEventListener(type, listener);
        }

        private function onEnterFrame(event:Event):void
        {
            dispatchEvent(event);
        }

        // properties

        /** The effect instance returning the FilterEffect created via <code>createEffect</code>. */
        protected function get effect():FilterEffect
        {
            if (_effect == null) _effect = createEffect();
            return _effect;
        }

        /** The VertexData used to process the effect. Per default, uses the format provided
         *  by the effect, and contains four vertices enclosing the target object. */
        protected function get vertexData():VertexData
        {
            if (_vertexData == null) _vertexData = new VertexData(effect.vertexFormat, 4);
            return _vertexData;
        }

        /** The IndexData used to process the effect. Per default, references a quad (two triangles)
         *  of four vertices. */
        protected function get indexData():IndexData
        {
            if (_indexData == null)
            {
                _indexData = new IndexData(6);
                _indexData.addQuad(0, 1, 2, 3);
            }

            return _indexData;
        }

        /** Call this method when any of the filter's properties changes.
         *  This will make sure the filter is redrawn in the next frame. */
        protected function setRequiresRedraw():void
        {
            dispatchEventWith(Event.CHANGE);
            if (_target) _target.setRequiresRedraw();
            if (_cached) _cacheRequested = true;
        }

        /** Indicates the number of rendering passes required for this filter.
         *  Subclasses must override this method if the number of passes is not <code>1</code>. */
        public function get numPasses():int
        {
            return 1;
        }

        /** Called when assigning a target display object.
         *  Override to plug in class-specific logic. */
        protected function onTargetAssigned(target:DisplayObject):void
        { }

        /** Padding can extend the size of the filter texture in all directions.
         *  That's useful when the filter "grows" the bounds of the object in any direction. */
        public function get padding():Padding
        {
            if (_padding == null)
            {
                _padding = new Padding();
                _padding.addEventListener(Event.CHANGE, setRequiresRedraw);
            }

            return _padding;
        }

        public function set padding(value:Padding):void
        {
            padding.copyFrom(value);
        }

        /** Indicates if the filter is cached (via the <code>cache</code> method). */
        public function get isCached():Boolean { return _cached; }

        /** The resolution of the filter texture. "1" means stage resolution, "0.5" half the stage
         *  resolution. A lower resolution saves memory and execution time, but results in a lower
         *  output quality. Values greater than 1 are allowed; such values might make sense for a
         *  cached filter when it is scaled up. @default 1
         */
        public function get resolution():Number { return _resolution; }
        public function set resolution(value:Number):void
        {
            if (value != _resolution)
            {
                if (value > 0) _resolution = value;
                else throw new ArgumentError("resolution must be > 0");
                setRequiresRedraw();
            }
        }

        /** The smoothing mode of the filter texture. @default bilinear */
        public function get textureSmoothing():String { return _textureSmoothing; }
        public function set textureSmoothing(value:String):void
        {
            if (value != _textureSmoothing)
            {
                _textureSmoothing = value;
                if (_quad) _quad.textureSmoothing = value;
                setRequiresRedraw();
            }
        }

        /** The format of the filter texture. @default BGRA */
        public function get textureFormat():String { return _textureFormat; }
        public function set textureFormat(value:String):void
        {
            if (value != _textureFormat)
            {
                _textureFormat = value;
                if (_helper) _helper.textureFormat = value;
                setRequiresRedraw();
            }
        }

        /** Indicates if the last filter pass is always drawn directly to the back buffer.
         *
         *  <p>Per default, the filter tries to automatically render in a smart way: objects that
         *  are currently moving are rendered to the back buffer, objects that are static are
         *  rendered into a texture first, which allows the filter to be drawn directly from the
         *  render cache in the next frame (in case the object remains static).</p>
         *
         *  <p>However, this fails when filters are added to an object that does not support the
         *  render cache, or to a container with such a child (e.g. a Sprite3D object or a masked
         *  display object). In such a case, enable this property for maximum performance.</p>
         *
         *  @default false
         */
        public function get alwaysDrawToBackBuffer():Boolean { return _alwaysDrawToBackBuffer; }
        public function set alwaysDrawToBackBuffer(value:Boolean):void
        {
            _alwaysDrawToBackBuffer = value;
        }

        // internal methods

        /** @private */
        starling_internal function setTarget(target:DisplayObject):void
        {
            if (target != _target)
            {
                var prevTarget:DisplayObject = _target;
                _target = target;

                if (target == null)
                {
                    if (_helper) _helper.purge();
                    if (_effect) _effect.purgeBuffers();
                    if (_quad)   _quad.disposeTexture();
                }

                if (prevTarget)
                {
                    prevTarget.filter = null;
                    prevTarget.removeEventListener(Event.ENTER_FRAME, onEnterFrame);
                }

                if (target)
                {
                    if (hasEventListener(Event.ENTER_FRAME))
                        target.addEventListener(Event.ENTER_FRAME, onEnterFrame);

                    onTargetAssigned(target);
                }
            }
        }
    }
}

import flash.geom.Matrix;
import flash.geom.Rectangle;

import starling.display.DisplayObject;
import starling.display.Mesh;
import starling.rendering.IndexData;
import starling.rendering.VertexData;
import starling.textures.Texture;

class FilterQuad extends Mesh
{
    private static var sMatrix:Matrix = new Matrix();

    public function FilterQuad(smoothing:String)
    {
        var vertexData:VertexData = new VertexData(null, 4);
        vertexData.numVertices = 4;

        var indexData:IndexData = new IndexData(6);
        indexData.addQuad(0, 1, 2, 3);

        super(vertexData, indexData);

        textureSmoothing = smoothing;
        pixelSnapping = false;
    }

    override public function dispose():void
    {
        disposeTexture();
        super.dispose();
    }

    public function disposeTexture():void
    {
        if (texture)
        {
            texture.dispose();
            texture = null;
        }
    }

    public function moveVertices(sourceSpace:DisplayObject, targetSpace:DisplayObject):void
    {
        if (targetSpace.is3D)
            throw new Error("cannot move vertices into 3D space");
        else if (sourceSpace != targetSpace)
        {
            targetSpace.getTransformationMatrix(sourceSpace, sMatrix).invert(); // ss could be null!
            vertexData.transformPoints("position", sMatrix);
        }
    }

    public function setBounds(bounds:Rectangle):void
    {
        var vertexData:VertexData = this.vertexData;
        var attrName:String = "position";

        vertexData.setPoint(0, attrName, bounds.x, bounds.y);
        vertexData.setPoint(1, attrName, bounds.right, bounds.y);
        vertexData.setPoint(2, attrName, bounds.x, bounds.bottom);
        vertexData.setPoint(3, attrName, bounds.right, bounds.bottom);
    }

    override public function set texture(value:Texture):void
    {
        super.texture = value;
        if (value) value.setupTextureCoordinates(vertexData);
    }
}