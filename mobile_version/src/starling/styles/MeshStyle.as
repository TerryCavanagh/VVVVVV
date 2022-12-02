// =================================================================================================
//
//	Starling Framework
//	Copyright Gamua GmbH. All Rights Reserved.
//
//	This program is free software. You can redistribute and/or modify it
//	in accordance with the terms of the accompanying license agreement.
//
// =================================================================================================

package starling.styles
{
    import flash.display3D.textures.TextureBase;
    import flash.geom.Matrix;
    import flash.geom.Point;

    import starling.core.starling_internal;
    import starling.display.Mesh;
    import starling.events.Event;
    import starling.events.EventDispatcher;
    import starling.rendering.*;
    import starling.textures.Texture;
    import starling.textures.TextureSmoothing;

    /** Dispatched every frame on styles assigned to display objects connected to the stage. */
    [Event(name="enterFrame", type="starling.events.EnterFrameEvent")]

    /** MeshStyles provide a means to completely modify the way a mesh is rendered.
     *  The base class provides Starling's standard mesh rendering functionality: colored and
     *  (optionally) textured meshes. Subclasses may add support for additional features like
     *  color transformations, normal mapping, etc.
     *
     *  <p><strong>Using styles</strong></p>
     *
     *  <p>First, create an instance of the desired style. Configure the style by updating its
     *  properties, then assign it to the mesh. Here is an example that uses a fictitious
     *  <code>ColorStyle</code>:</p>
     *
     *  <listing>
     *  var image:Image = new Image(heroTexture);
     *  var colorStyle:ColorStyle = new ColorStyle();
     *  colorStyle.redOffset = 0.5;
     *  colorStyle.redMultiplier = 2.0;
     *  image.style = colorStyle;</listing>
     *
     *  <p>Beware:</p>
     *
     *  <ul>
     *    <li>A style instance may only be used on one object at a time.</li>
     *    <li>A style might require the use of a specific vertex format;
     *        when the style is assigned, the mesh is converted to that format.</li>
     *  </ul>
     *
     *  <p><strong>Creating your own styles</strong></p>
     *
     *  <p>To create custom rendering code in Starling, you need to extend two classes:
     *  <code>MeshStyle</code> and <code>MeshEffect</code>. While the effect class contains
     *  the actual AGAL rendering code, the style provides the API that other developers will
     *  interact with.</p>
     *
     *  <p>Subclasses of <code>MeshStyle</code> will add specific properties that configure the
     *  style's outcome, like the <code>redOffset</code> and <code>redMultiplier</code> properties
     *  in the sample above. Here's how to properly create such a class:</p>
     *
     *  <ul>
     *    <li>Always provide a constructor that can be called without any arguments.</li>
     *    <li>Override <code>copyFrom</code> — that's necessary for batching.</li>
     *    <li>Override <code>createEffect</code> — this method must return the
     *        <code>MeshEffect</code> that will do the actual Stage3D rendering.</li>
     *    <li>Override <code>updateEffect</code> — this configures the effect created above
     *        right before rendering.</li>
     *    <li>Override <code>canBatchWith</code> if necessary — this method figures out if one
     *        instance of the style can be batched with another. If they all can, you can leave
     *        this out.</li>
     *  </ul>
     *
     *  <p>If the style requires a custom vertex format, you must also:</p>
     *
     *  <ul>
     *    <li>add a static constant called <code>VERTEX_FORMAT</code> to the class and</li>
     *    <li>override <code>get vertexFormat</code> and let it return exactly that format.</li>
     *  </ul>
     *
     *  <p>When that's done, you can turn to the implementation of your <code>MeshEffect</code>;
     *  the <code>createEffect</code>-override will return an instance of this class.
     *  Directly before rendering begins, Starling will then call <code>updateEffect</code>
     *  to set it up.</p>
     *
     *  @see MeshEffect
     *  @see VertexDataFormat
     *  @see starling.display.Mesh
     */
    public class MeshStyle extends EventDispatcher
    {
        /** The vertex format expected by this style (the same as found in the MeshEffect-class). */
        public static const VERTEX_FORMAT:VertexDataFormat = MeshEffect.VERTEX_FORMAT;

        private var _type:Class;
        private var _target:Mesh;
        private var _texture:Texture;
        private var _textureBase:TextureBase;
        private var _textureSmoothing:String;
        private var _textureRepeat:Boolean;
        private var _vertexData:VertexData;   // just a reference to the target's vertex data
        private var _indexData:IndexData;     // just a reference to the target's index data

        // helper objects
        private static var sPoint:Point = new Point();

        /** Creates a new MeshStyle instance.
         *  Subclasses must provide a constructor that can be called without any arguments. */
        public function MeshStyle()
        {
            _textureSmoothing = TextureSmoothing.BILINEAR;
            _type = Object(this).constructor as Class;
        }

        /** Copies all properties of the given style to the current instance (or a subset, if the
         *  classes don't match). Must be overridden by all subclasses!
         */
        public function copyFrom(meshStyle:MeshStyle):void
        {
            _texture = meshStyle._texture;
            _textureBase = meshStyle._textureBase;
            _textureRepeat = meshStyle._textureRepeat;
            _textureSmoothing = meshStyle._textureSmoothing;
        }

        /** Creates a clone of this instance. The method will work for subclasses automatically,
         *  no need to override it. */
        public function clone():MeshStyle
        {
            var clone:MeshStyle = new _type();
            clone.copyFrom(this);
            return clone;
        }

        /** Creates the effect that does the actual, low-level rendering.
         *  To be overridden by subclasses!
         */
        public function createEffect():MeshEffect
        {
            return new MeshEffect();
        }

        /** Updates the settings of the given effect to match the current style.
         *  The given <code>effect</code> will always match the class returned by
         *  <code>createEffect</code>.
         *
         *  <p>To be overridden by subclasses!</p>
         */
        public function updateEffect(effect:MeshEffect, state:RenderState):void
        {
            effect.texture = _texture;
            effect.textureRepeat = _textureRepeat;
            effect.textureSmoothing = _textureSmoothing;
            effect.mvpMatrix3D = state.mvpMatrix3D;
            effect.alpha = state.alpha;
            effect.tinted = _vertexData.tinted;
        }

        /** Indicates if the current instance can be batched with the given style.
         *  To be overridden by subclasses if default behavior is not sufficient.
         *  The base implementation just checks if the styles are of the same type
         *  and if the textures are compatible.
         */
        public function canBatchWith(meshStyle:MeshStyle):Boolean
        {
            if (_type == meshStyle._type)
            {
                var newTexture:Texture = meshStyle._texture;

                if (_texture == null && newTexture == null) return true;
                else if (_texture && newTexture)
                    return _textureBase == meshStyle._textureBase &&
                           _textureSmoothing == meshStyle._textureSmoothing &&
                           _textureRepeat == meshStyle._textureRepeat;
                else return false;
            }
            else return false;
        }

        /** Copies the vertex data of the style's current target to the target of another style.
         *  If you pass a matrix, all vertices will be transformed during the process.
         *
         *  <p>This method is used when batching meshes together for rendering. The parameter
         *  <code>targetStyle</code> will point to the style of a <code>MeshBatch</code> (a
         *  subclass of <code>Mesh</code>). Subclasses may override this method if they need
         *  to modify the vertex data in that process.</p>
         */
        public function batchVertexData(targetStyle:MeshStyle, targetVertexID:int=0,
                                        matrix:Matrix=null, vertexID:int=0, numVertices:int=-1):void
        {
            _vertexData.copyTo(targetStyle._vertexData, targetVertexID, matrix, vertexID, numVertices);
        }

        /** Copies the index data of the style's current target to the target of another style.
         *  The given offset value will be added to all indices during the process.
         *
         *  <p>This method is used when batching meshes together for rendering. The parameter
         *  <code>targetStyle</code> will point to the style of a <code>MeshBatch</code> (a
         *  subclass of <code>Mesh</code>). Subclasses may override this method if they need
         *  to modify the index data in that process.</p>
         */
        public function batchIndexData(targetStyle:MeshStyle, targetIndexID:int=0, offset:int=0,
                                       indexID:int=0, numIndices:int=-1):void
        {
            _indexData.copyTo(targetStyle._indexData, targetIndexID, offset, indexID, numIndices);
        }

        /** Call this method if the target needs to be redrawn.
         *  The call is simply forwarded to the target mesh. */
        protected function setRequiresRedraw():void
        {
            if (_target) _target.setRequiresRedraw();
        }

        /** Call this method when the vertex data changed.
         *  The call is simply forwarded to the target mesh. */
        protected function setVertexDataChanged():void
        {
            if (_target) _target.setVertexDataChanged();
        }

        /** Call this method when the index data changed.
         *  The call is simply forwarded to the target mesh. */
        protected function setIndexDataChanged():void
        {
            if (_target) _target.setIndexDataChanged();
        }

        /** Called when assigning a target mesh. Override to plug in class-specific logic. */
        protected function onTargetAssigned(target:Mesh):void
        { }

        // enter frame event

        override public function addEventListener(type:String, listener:Function):void
        {
            if (type == Event.ENTER_FRAME && _target)
                _target.addEventListener(Event.ENTER_FRAME, onEnterFrame);

            super.addEventListener(type, listener);
        }

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

        // internal methods

        /** @private */
        starling_internal function setTarget(target:Mesh=null, vertexData:VertexData=null,
                                             indexData:IndexData=null):void
        {
            if (_target != target)
            {
                if (_target) _target.removeEventListener(Event.ENTER_FRAME, onEnterFrame);
                if (vertexData) vertexData.format = vertexFormat;

                _target = target;
                _vertexData = vertexData;
                _indexData = indexData;

                if (target)
                {
                    if (hasEventListener(Event.ENTER_FRAME))
                        target.addEventListener(Event.ENTER_FRAME, onEnterFrame);

                    onTargetAssigned(target);
                }
            }
        }

        // vertex manipulation

        /** The position of the vertex at the specified index, in the mesh's local coordinate
         *  system.
         *
         *  <p>Only modify the position of a vertex if you know exactly what you're doing, as
         *  some classes might not work correctly when their vertices are moved. E.g. the
         *  <code>Quad</code> class expects its vertices to spawn up a perfectly rectangular
         *  area; some of its optimized methods won't work correctly if that premise is no longer
         *  fulfilled or the original bounds change.</p>
         */
        public function getVertexPosition(vertexID:int, out:Point=null):Point
        {
            return _vertexData.getPoint(vertexID, "position", out);
        }

        public function setVertexPosition(vertexID:int, x:Number, y:Number):void
        {
            _vertexData.setPoint(vertexID, "position", x, y);
            setVertexDataChanged();
        }

        /** Returns the alpha value of the vertex at the specified index. */
        public function getVertexAlpha(vertexID:int):Number
        {
            return _vertexData.getAlpha(vertexID);
        }

        /** Sets the alpha value of the vertex at the specified index to a certain value. */
        public function setVertexAlpha(vertexID:int, alpha:Number):void
        {
            _vertexData.setAlpha(vertexID, "color", alpha);
            setVertexDataChanged();
        }

        /** Returns the RGB color of the vertex at the specified index. */
        public function getVertexColor(vertexID:int):uint
        {
            return _vertexData.getColor(vertexID);
        }

        /** Sets the RGB color of the vertex at the specified index to a certain value. */
        public function setVertexColor(vertexID:int, color:uint):void
        {
            _vertexData.setColor(vertexID, "color", color);
            setVertexDataChanged();
        }

        /** Returns the texture coordinates of the vertex at the specified index. */
        public function getTexCoords(vertexID:int, out:Point = null):Point
        {
            if (_texture) return _texture.getTexCoords(_vertexData, vertexID, "texCoords", out);
            else return _vertexData.getPoint(vertexID, "texCoords", out);
        }

        /** Sets the texture coordinates of the vertex at the specified index to the given values. */
        public function setTexCoords(vertexID:int, u:Number, v:Number):void
        {
            if (_texture) _texture.setTexCoords(_vertexData, vertexID, "texCoords", u, v);
            else _vertexData.setPoint(vertexID, "texCoords", u, v);

            setVertexDataChanged();
        }

        // properties

        /** Returns a reference to the vertex data of the assigned target (or <code>null</code>
         *  if there is no target). Beware: the style itself does not own any vertices;
         *  it is limited to manipulating those of the target mesh. */
        protected function get vertexData():VertexData { return _vertexData; }

        /** Returns a reference to the index data of the assigned target (or <code>null</code>
         *  if there is no target). Beware: the style itself does not own any indices;
         *  it is limited to manipulating those of the target mesh. */
        protected function get indexData():IndexData { return _indexData; }

        /** The actual class of this style. */
        public function get type():Class { return _type; }

        /** Changes the color of all vertices to the same value.
         *  The getter simply returns the color of the first vertex. */
        public function get color():uint
        {
            if (_vertexData.numVertices > 0) return _vertexData.getColor(0);
            else return 0x0;
        }

        public function set color(value:uint):void
        {
            var i:int;
            var numVertices:int = _vertexData.numVertices;

            for (i=0; i<numVertices; ++i)
                _vertexData.setColor(i, "color", value);

            if (value == 0xffffff && _vertexData.tinted)
                _vertexData.updateTinted();

            setVertexDataChanged();
        }

        /** The format used to store the vertices. */
        public function get vertexFormat():VertexDataFormat
        {
            return VERTEX_FORMAT;
        }

        /** The texture that is mapped to the mesh (or <code>null</code>, if there is none). */
        public function get texture():Texture { return _texture; }
        public function set texture(value:Texture):void
        {
            if (value != _texture)
            {
                if (value)
                {
                    var i:int;
                    var numVertices:int = _vertexData ? _vertexData.numVertices : 0;

                    for (i = 0; i < numVertices; ++i)
                    {
                        getTexCoords(i, sPoint);
                        value.setTexCoords(_vertexData, i, "texCoords", sPoint.x, sPoint.y);
                    }

                    setVertexDataChanged();
                }
                else setRequiresRedraw();

                _texture = value;
                _textureBase = value ? value.base : null;
            }
        }

        /** The smoothing filter that is used for the texture. @default bilinear */
        public function get textureSmoothing():String { return _textureSmoothing; }
        public function set textureSmoothing(value:String):void
        {
            if (value != _textureSmoothing)
            {
                _textureSmoothing = value;
                setRequiresRedraw();
            }
        }

        /** Indicates if pixels at the edges will be repeated or clamped.
         *  Only works for power-of-two textures. @default false */
        public function get textureRepeat():Boolean { return _textureRepeat; }
        public function set textureRepeat(value:Boolean):void { _textureRepeat = value; }

        /** The target the style is currently assigned to. */
        public function get target():Mesh { return _target; }
    }
}
