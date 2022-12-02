// =================================================================================================
//
//	Starling Framework
//	Copyright Gamua GmbH. All Rights Reserved.
//
//	This program is free software. You can redistribute and/or modify it
//	in accordance with the terms of the accompanying license agreement.
//
// =================================================================================================

package starling.display
{
    import flash.errors.IllegalOperationError;
    import flash.geom.Matrix;
    import flash.geom.Matrix3D;
    import flash.geom.Point;
    import flash.geom.Rectangle;
    import flash.geom.Vector3D;
    import flash.system.Capabilities;
    import flash.ui.Mouse;
    import flash.ui.MouseCursor;
    import flash.utils.getQualifiedClassName;

    import starling.core.Starling;
    import starling.core.starling_internal;
    import starling.errors.AbstractClassError;
    import starling.errors.AbstractMethodError;
    import starling.events.Event;
    import starling.events.EventDispatcher;
    import starling.events.TouchEvent;
    import starling.filters.FragmentFilter;
    import starling.rendering.BatchToken;
    import starling.rendering.Painter;
    import starling.utils.Align;
    import starling.utils.MathUtil;
    import starling.utils.MatrixUtil;

    use namespace starling_internal;

    /** Dispatched when an object is added to a parent. */
    [Event(name="added", type="starling.events.Event")]
    
    /** Dispatched when an object is connected to the stage (directly or indirectly). */
    [Event(name="addedToStage", type="starling.events.Event")]
    
    /** Dispatched when an object is removed from its parent. */
    [Event(name="removed", type="starling.events.Event")]
    
    /** Dispatched when an object is removed from the stage and won't be rendered any longer. */ 
    [Event(name="removedFromStage", type="starling.events.Event")]
    
    /** Dispatched once every frame on every object that is connected to the stage. */ 
    [Event(name="enterFrame", type="starling.events.EnterFrameEvent")]
    
    /** Dispatched when an object is touched. Bubbles. */
    [Event(name="touch", type="starling.events.TouchEvent")]
    
    /** Dispatched when a key on the keyboard is released. */
    [Event(name="keyUp", type="starling.events.KeyboardEvent")]
    
    /** Dispatched when a key on the keyboard is pressed. */
    [Event(name="keyDown", type="starling.events.KeyboardEvent")]
    
    /**
     *  The DisplayObject class is the base class for all objects that are rendered on the 
     *  screen.
     *  
     *  <p><strong>The Display Tree</strong></p> 
     *  
     *  <p>In Starling, all displayable objects are organized in a display tree. Only objects that
     *  are part of the display tree will be displayed (rendered).</p> 
     *   
     *  <p>The display tree consists of leaf nodes (Image, Quad) that will be rendered directly to
     *  the screen, and of container nodes (subclasses of "DisplayObjectContainer", like "Sprite").
     *  A container is simply a display object that has child nodes - which can, again, be either
     *  leaf nodes or other containers.</p> 
     *  
     *  <p>At the base of the display tree, there is the Stage, which is a container, too. To create
     *  a Starling application, you create a custom Sprite subclass, and Starling will add an
     *  instance of this class to the stage.</p>
     *  
     *  <p>A display object has properties that define its position in relation to its parent
     *  (x, y), as well as its rotation and scaling factors (scaleX, scaleY). Use the 
     *  <code>alpha</code> and <code>visible</code> properties to make an object translucent or 
     *  invisible.</p>
     *  
     *  <p>Every display object may be the target of touch events. If you don't want an object to be
     *  touchable, you can disable the "touchable" property. When it's disabled, neither the object
     *  nor its children will receive any more touch events.</p>
     *    
     *  <strong>Transforming coordinates</strong>
     *  
     *  <p>Within the display tree, each object has its own local coordinate system. If you rotate
     *  a container, you rotate that coordinate system - and thus all the children of the 
     *  container.</p>
     *  
     *  <p>Sometimes you need to know where a certain point lies relative to another coordinate 
     *  system. That's the purpose of the method <code>getTransformationMatrix</code>. It will  
     *  create a matrix that represents the transformation of a point in one coordinate system to 
     *  another.</p> 
     *  
     *  <strong>Customization</strong>
     *  
     *  <p>DisplayObject is an abstract class, which means you cannot instantiate it directly,
     *  but have to use one of its many subclasses instead. For leaf nodes, this is typically
     *  'Mesh' or its subclasses 'Quad' and 'Image'. To customize rendering of these objects,
     *  you can use fragment filters (via the <code>filter</code>-property on 'DisplayObject')
     *  or mesh styles (via the <code>style</code>-property on 'Mesh'). Look at the respective
     *  class documentation for more information.</p>
     *
     *  @see DisplayObjectContainer
     *  @see Sprite
     *  @see Stage
     *  @see Mesh
     *  @see starling.filters.FragmentFilter
     *  @see starling.styles.MeshStyle
     */
    public class DisplayObject extends EventDispatcher
    {
        // private members
        
        private var _x:Number;
        private var _y:Number;
        private var _pivotX:Number;
        private var _pivotY:Number;
        private var _scaleX:Number;
        private var _scaleY:Number;
        private var _skewX:Number;
        private var _skewY:Number;
        private var _rotation:Number;
        private var _alpha:Number;
        private var _visible:Boolean;
        private var _touchable:Boolean;
        private var _blendMode:String;
        private var _name:String;
        private var _useHandCursor:Boolean;
        private var _transformationMatrix:Matrix;
        private var _transformationMatrix3D:Matrix3D;
        private var _orientationChanged:Boolean;
        private var _is3D:Boolean;
        private var _maskee:DisplayObject;

        // internal members (for fast access on rendering)

        /** @private */ internal var _parent:DisplayObjectContainer;
        /** @private */ internal var _lastParentOrSelfChangeFrameID:uint;
        /** @private */ internal var _lastChildChangeFrameID:uint;
        /** @private */ internal var _tokenFrameID:uint;
        /** @private */ internal var _pushToken:BatchToken = new BatchToken();
        /** @private */ internal var _popToken:BatchToken = new BatchToken();
        /** @private */ internal var _hasVisibleArea:Boolean;
        /** @private */ internal var _filter:FragmentFilter;
        /** @private */ internal var _mask:DisplayObject;

        // helper objects

        private static var sAncestors:Vector.<DisplayObject> = new <DisplayObject>[];
        private static var sHelperPoint:Point = new Point();
        private static var sHelperPoint3D:Vector3D = new Vector3D();
        private static var sHelperPointAlt3D:Vector3D = new Vector3D();
        private static var sHelperRect:Rectangle = new Rectangle();
        private static var sHelperMatrix:Matrix  = new Matrix();
        private static var sHelperMatrixAlt:Matrix  = new Matrix();
        private static var sHelperMatrix3D:Matrix3D  = new Matrix3D();
        private static var sHelperMatrixAlt3D:Matrix3D  = new Matrix3D();
        
        /** @private */ 
        public function DisplayObject()
        {
            if (Capabilities.isDebugger && 
                getQualifiedClassName(this) == "starling.display::DisplayObject")
            {
                throw new AbstractClassError();
            }
            
            _x = _y = _pivotX = _pivotY = _rotation = _skewX = _skewY = 0.0;
            _scaleX = _scaleY = _alpha = 1.0;
            _visible = _touchable = _hasVisibleArea = true;
            _blendMode = BlendMode.AUTO;
            _transformationMatrix = new Matrix();
        }
        
        /** Disposes all resources of the display object. 
          * GPU buffers are released, event listeners are removed, filters and masks are disposed. */
        public function dispose():void
        {
            if (_filter) _filter.dispose();
            if (_mask) _mask.dispose();
            removeEventListeners();
            mask = null; // clear 'mask._maskee', just to be sure.
        }
        
        /** Removes the object from its parent, if it has one, and optionally disposes it. */
        public function removeFromParent(dispose:Boolean=false):void
        {
            if (_parent) _parent.removeChild(this, dispose);
            else if (dispose) this.dispose();
        }
        
        /** Creates a matrix that represents the transformation from the local coordinate system 
         *  to another. If you pass an <code>out</code>-matrix, the result will be stored in this
         *  matrix instead of creating a new object. */
        public function getTransformationMatrix(targetSpace:DisplayObject, 
                                                out:Matrix=null):Matrix
        {
            var commonParent:DisplayObject;
            var currentObject:DisplayObject;
            
            if (out) out.identity();
            else out = new Matrix();
            
            if (targetSpace == this)
            {
                return out;
            }
            else if (targetSpace == _parent || (targetSpace == null && _parent == null))
            {
                out.copyFrom(transformationMatrix);
                return out;
            }
            else if (targetSpace == null || targetSpace == base)
            {
                // targetCoordinateSpace 'null' represents the target space of the base object.
                // -> move up from this to base
                
                currentObject = this;
                while (currentObject != targetSpace)
                {
                    out.concat(currentObject.transformationMatrix);
                    currentObject = currentObject._parent;
                }
                
                return out;
            }
            else if (targetSpace._parent == this) // optimization
            {
                targetSpace.getTransformationMatrix(this, out);
                out.invert();
                
                return out;
            }
            
            // 1. find a common parent of this and the target space
            
            commonParent = findCommonParent(this, targetSpace);
            
            // 2. move up from this to common parent
            
            currentObject = this;
            while (currentObject != commonParent)
            {
                out.concat(currentObject.transformationMatrix);
                currentObject = currentObject._parent;
            }
            
            if (commonParent == targetSpace)
                return out;
            
            // 3. now move up from target until we reach the common parent
            
            sHelperMatrix.identity();
            currentObject = targetSpace;
            while (currentObject != commonParent)
            {
                sHelperMatrix.concat(currentObject.transformationMatrix);
                currentObject = currentObject._parent;
            }
            
            // 4. now combine the two matrices
            
            sHelperMatrix.invert();
            out.concat(sHelperMatrix);
            
            return out;
        }
        
        /** Returns a rectangle that completely encloses the object as it appears in another 
         *  coordinate system. If you pass an <code>out</code>-rectangle, the result will be
         *  stored in this rectangle instead of creating a new object. */
        public function getBounds(targetSpace:DisplayObject, out:Rectangle=null):Rectangle
        {
            throw new AbstractMethodError();
        }
        
        /** Returns the object that is found topmost beneath a point in local coordinates, or nil
         *  if the test fails. Untouchable and invisible objects will cause the test to fail. */
        public function hitTest(localPoint:Point):DisplayObject
        {
            // on a touch test, invisible or untouchable objects cause the test to fail
            if (!_visible || !_touchable) return null;

            // if we've got a mask and the hit occurs outside, fail
            if (_mask && !hitTestMask(localPoint)) return null;
            
            // otherwise, check bounding box
            if (getBounds(this, sHelperRect).containsPoint(localPoint)) return this;
            else return null;
        }

        /** Checks if a certain point is inside the display object's mask. If there is no mask,
         *  this method always returns <code>true</code> (because having no mask is equivalent
         *  to having one that's infinitely big). */
        public function hitTestMask(localPoint:Point):Boolean
        {
            if (_mask)
            {
                if (_mask.stage) getTransformationMatrix(_mask, sHelperMatrixAlt);
                else
                {
                    sHelperMatrixAlt.copyFrom(_mask.transformationMatrix);
                    sHelperMatrixAlt.invert();
                }

                var helperPoint:Point = localPoint == sHelperPoint ? new Point() : sHelperPoint;
                MatrixUtil.transformPoint(sHelperMatrixAlt, localPoint, helperPoint);
                return _mask.hitTest(helperPoint) != null;
            }
            else return true;
        }

        /** Transforms a point from the local coordinate system to global (stage) coordinates.
         *  If you pass an <code>out</code>-point, the result will be stored in this point instead
         *  of creating a new object. */
        public function localToGlobal(localPoint:Point, out:Point=null):Point
        {
            if (is3D)
            {
                sHelperPoint3D.setTo(localPoint.x, localPoint.y, 0);
                return local3DToGlobal(sHelperPoint3D, out);
            }
            else
            {
                getTransformationMatrix(base, sHelperMatrixAlt);
                return MatrixUtil.transformPoint(sHelperMatrixAlt, localPoint, out);
            }
        }
        
        /** Transforms a point from global (stage) coordinates to the local coordinate system.
         *  If you pass an <code>out</code>-point, the result will be stored in this point instead
         *  of creating a new object. */
        public function globalToLocal(globalPoint:Point, out:Point=null):Point
        {
            if (is3D)
            {
                globalToLocal3D(globalPoint, sHelperPoint3D);
                stage.getCameraPosition(this, sHelperPointAlt3D);
                return MathUtil.intersectLineWithXYPlane(sHelperPointAlt3D, sHelperPoint3D, out);
            }
            else
            {
                getTransformationMatrix(base, sHelperMatrixAlt);
                sHelperMatrixAlt.invert();
                return MatrixUtil.transformPoint(sHelperMatrixAlt, globalPoint, out);
            }
        }
        
        /** Renders the display object with the help of a painter object. Never call this method
         *  directly, except from within another render method.
         *
         *  @param painter Captures the current render state and provides utility functions
         *                 for rendering.
         */
        public function render(painter:Painter):void
        {
            throw new AbstractMethodError();
        }
        
        /** Moves the pivot point to a certain position within the local coordinate system
         *  of the object. If you pass no arguments, it will be centered. */ 
        public function alignPivot(horizontalAlign:String="center",
                                   verticalAlign:String="center"):void
        {
            var bounds:Rectangle = getBounds(this, sHelperRect);
            setOrientationChanged();
            
            if (horizontalAlign == Align.LEFT)        _pivotX = bounds.x;
            else if (horizontalAlign == Align.CENTER) _pivotX = bounds.x + bounds.width / 2.0;
            else if (horizontalAlign == Align.RIGHT)  _pivotX = bounds.x + bounds.width;
            else throw new ArgumentError("Invalid horizontal alignment: " + horizontalAlign);
            
            if (verticalAlign == Align.TOP)         _pivotY = bounds.y;
            else if (verticalAlign == Align.CENTER) _pivotY = bounds.y + bounds.height / 2.0;
            else if (verticalAlign == Align.BOTTOM) _pivotY = bounds.y + bounds.height;
            else throw new ArgumentError("Invalid vertical alignment: " + verticalAlign);
        }

        // 3D transformation

        /** Creates a matrix that represents the transformation from the local coordinate system
         *  to another. This method supports three dimensional objects created via 'Sprite3D'.
         *  If you pass an <code>out</code>-matrix, the result will be stored in this matrix
         *  instead of creating a new object. */
        public function getTransformationMatrix3D(targetSpace:DisplayObject,
                                                  out:Matrix3D=null):Matrix3D
        {
            var commonParent:DisplayObject;
            var currentObject:DisplayObject;

            if (out) out.identity();
            else out = new Matrix3D();

            if (targetSpace == this)
            {
                return out;
            }
            else if (targetSpace == _parent || (targetSpace == null && _parent == null))
            {
                out.copyFrom(transformationMatrix3D);
                return out;
            }
            else if (targetSpace == null || targetSpace == base)
            {
                // targetCoordinateSpace 'null' represents the target space of the base object.
                // -> move up from this to base

                currentObject = this;
                while (currentObject != targetSpace)
                {
                    out.append(currentObject.transformationMatrix3D);
                    currentObject = currentObject._parent;
                }

                return out;
            }
            else if (targetSpace._parent == this) // optimization
            {
                targetSpace.getTransformationMatrix3D(this, out);
                out.invert();

                return out;
            }

            // 1. find a common parent of this and the target space

            commonParent = findCommonParent(this, targetSpace);

            // 2. move up from this to common parent

            currentObject = this;
            while (currentObject != commonParent)
            {
                out.append(currentObject.transformationMatrix3D);
                currentObject = currentObject._parent;
            }

            if (commonParent == targetSpace)
                return out;

            // 3. now move up from target until we reach the common parent

            sHelperMatrix3D.identity();
            currentObject = targetSpace;
            while (currentObject != commonParent)
            {
                sHelperMatrix3D.append(currentObject.transformationMatrix3D);
                currentObject = currentObject._parent;
            }

            // 4. now combine the two matrices

            sHelperMatrix3D.invert();
            out.append(sHelperMatrix3D);

            return out;
        }

        /** Transforms a 3D point from the local coordinate system to global (stage) coordinates.
         *  This is achieved by projecting the 3D point onto the (2D) view plane.
         *
         *  <p>If you pass an <code>out</code>-point, the result will be stored in this point
         *  instead of creating a new object.</p> */
        public function local3DToGlobal(localPoint:Vector3D, out:Point=null):Point
        {
            var stage:Stage = this.stage;
            if (stage == null) throw new IllegalOperationError("Object not connected to stage");

            getTransformationMatrix3D(stage, sHelperMatrixAlt3D);
            MatrixUtil.transformPoint3D(sHelperMatrixAlt3D, localPoint, sHelperPoint3D);
            return MathUtil.intersectLineWithXYPlane(stage.cameraPosition, sHelperPoint3D, out);
        }

        /** Transforms a point from global (stage) coordinates to the 3D local coordinate system.
         *  If you pass an <code>out</code>-vector, the result will be stored in this vector
         *  instead of creating a new object. */
        public function globalToLocal3D(globalPoint:Point, out:Vector3D=null):Vector3D
        {
            var stage:Stage = this.stage;
            if (stage == null) throw new IllegalOperationError("Object not connected to stage");

            getTransformationMatrix3D(stage, sHelperMatrixAlt3D);
            sHelperMatrixAlt3D.invert();
            return MatrixUtil.transformCoords3D(
                sHelperMatrixAlt3D, globalPoint.x, globalPoint.y, 0, out);
        }

        // internal methods
        
        /** @private */
        starling_internal function setParent(value:DisplayObjectContainer):void
        {
            // check for a recursion
            var ancestor:DisplayObject = value;
            while (ancestor != this && ancestor != null)
                ancestor = ancestor._parent;
            
            if (ancestor == this)
                throw new ArgumentError("An object cannot be added as a child to itself or one " +
                                        "of its children (or children's children, etc.)");
            else
                _parent = value;
        }
        
        /** @private */
        internal function setIs3D(value:Boolean):void
        {
            _is3D = value;
        }

        /** @private */
        internal function get isMask():Boolean
        {
            return _maskee != null;
        }

        // render cache

        /** Forces the object to be redrawn in the next frame.
         *  This will prevent the object to be drawn from the render cache.
         *
         *  <p>This method is called every time the object changes in any way. When creating
         *  custom mesh styles or any other custom rendering code, call this method if the object
         *  needs to be redrawn.</p>
         *
         *  <p>If the object needs to be redrawn just because it does not support the render cache,
         *  call <code>painter.excludeFromCache()</code> in the object's render method instead.
         *  That way, Starling's <code>skipUnchangedFrames</code> policy won't be disrupted.</p>
         */
        public function setRequiresRedraw():void
        {
            var parent:DisplayObject = _parent || _maskee;
            var frameID:int = Starling.frameID;

            _lastParentOrSelfChangeFrameID = frameID;
            _hasVisibleArea = _alpha  != 0.0 && _visible && _maskee == null &&
                              _scaleX != 0.0 && _scaleY != 0.0;

            while (parent && parent._lastChildChangeFrameID != frameID)
            {
                parent._lastChildChangeFrameID = frameID;
                parent = parent._parent || parent._maskee;
            }
        }

        /** Indicates if the object needs to be redrawn in the upcoming frame, i.e. if it has
         *  changed its location relative to the stage or some other aspect of its appearance
         *  since it was last rendered. */
        public function get requiresRedraw():Boolean
        {
            var frameID:uint = Starling.frameID;

            return _lastParentOrSelfChangeFrameID == frameID ||
                   _lastChildChangeFrameID == frameID;
        }

        /** @private Makes sure the object is not drawn from cache in the next frame.
         *  This method is meant to be called only from <code>Painter.finishFrame()</code>,
         *  since it requires rendering to be concluded. */
        starling_internal function excludeFromCache():void
        {
            var object:DisplayObject = this;
            var max:uint = 0xffffffff;

            while (object && object._tokenFrameID != max)
            {
                object._tokenFrameID = max;
                object = object._parent;
            }
        }

        // helpers

        private function setOrientationChanged():void
        {
            _orientationChanged = true;
            setRequiresRedraw();
        }
        
        private static function findCommonParent(object1:DisplayObject,
                                                 object2:DisplayObject):DisplayObject
        {
            var currentObject:DisplayObject = object1;

            while (currentObject)
            {
                sAncestors[sAncestors.length] = currentObject; // avoiding 'push'
                currentObject = currentObject._parent;
            }

            currentObject = object2;
            while (currentObject && sAncestors.indexOf(currentObject) == -1)
                currentObject = currentObject._parent;

            sAncestors.length = 0;

            if (currentObject) return currentObject;
            else throw new ArgumentError("Object not connected to target");
        }

        // stage event handling

        /** @private */
        public override function dispatchEvent(event:Event):void
        {
            if (event.type == Event.REMOVED_FROM_STAGE && stage == null)
                return; // special check to avoid double-dispatch of RfS-event.
            else
                super.dispatchEvent(event);
        }
        
        // enter frame event optimization
        
        // To avoid looping through the complete display tree each frame to find out who's
        // listening to ENTER_FRAME events, we manage a list of them manually in the Stage class.
        // We need to take care that (a) it must be dispatched only when the object is
        // part of the stage, (b) it must not cause memory leaks when the user forgets to call
        // dispose and (c) there might be multiple listeners for this event.
        
        /** @inheritDoc */
        public override function addEventListener(type:String, listener:Function):void
        {
            if (type == Event.ENTER_FRAME && !hasEventListener(type))
            {
                addEventListener(Event.ADDED_TO_STAGE, addEnterFrameListenerToStage);
                addEventListener(Event.REMOVED_FROM_STAGE, removeEnterFrameListenerFromStage);
                if (this.stage) addEnterFrameListenerToStage();
            }
            
            super.addEventListener(type, listener);
        }
        
        /** @inheritDoc */
        public override function removeEventListener(type:String, listener:Function):void
        {
            super.removeEventListener(type, listener);
            
            if (type == Event.ENTER_FRAME && !hasEventListener(type))
            {
                removeEventListener(Event.ADDED_TO_STAGE, addEnterFrameListenerToStage);
                removeEventListener(Event.REMOVED_FROM_STAGE, removeEnterFrameListenerFromStage);
                removeEnterFrameListenerFromStage();
            }
        }
        
        /** @inheritDoc */
        public override function removeEventListeners(type:String=null):void
        {
            if ((type == null || type == Event.ENTER_FRAME) && hasEventListener(Event.ENTER_FRAME))
            {
                removeEventListener(Event.ADDED_TO_STAGE, addEnterFrameListenerToStage);
                removeEventListener(Event.REMOVED_FROM_STAGE, removeEnterFrameListenerFromStage);
                removeEnterFrameListenerFromStage();
            }

            super.removeEventListeners(type);
        }
        
        private function addEnterFrameListenerToStage():void
        {
            Starling.current.stage.addEnterFrameListener(this);
        }
        
        private function removeEnterFrameListenerFromStage():void
        {
            Starling.current.stage.removeEnterFrameListener(this);
        }
        
        // properties
 
        /** The transformation matrix of the object relative to its parent.
         * 
         *  <p>If you assign a custom transformation matrix, Starling will try to figure out  
         *  suitable values for <code>x, y, scaleX, scaleY,</code> and <code>rotation</code>.
         *  However, if the matrix was created in a different way, this might not be possible. 
         *  In that case, Starling will apply the matrix, but not update the corresponding 
         *  properties.</p>
         * 
         *  <p>CAUTION: not a copy, but the actual object!</p> */
        public function get transformationMatrix():Matrix
        {
            if (_orientationChanged)
            {
                _orientationChanged = false;
                
                if (_skewX == 0.0 && _skewY == 0.0)
                {
                    // optimization: no skewing / rotation simplifies the matrix math
                    
                    if (_rotation == 0.0)
                    {
                        _transformationMatrix.setTo(_scaleX, 0.0, 0.0, _scaleY,
                            _x - _pivotX * _scaleX, _y - _pivotY * _scaleY);
                    }
                    else
                    {
                        var cos:Number = Math.cos(_rotation);
                        var sin:Number = Math.sin(_rotation);
                        var a:Number   = _scaleX *  cos;
                        var b:Number   = _scaleX *  sin;
                        var c:Number   = _scaleY * -sin;
                        var d:Number   = _scaleY *  cos;
                        var tx:Number  = _x - _pivotX * a - _pivotY * c;
                        var ty:Number  = _y - _pivotX * b - _pivotY * d;
                        
                        _transformationMatrix.setTo(a, b, c, d, tx, ty);
                    }
                }
                else
                {
                    _transformationMatrix.identity();
                    _transformationMatrix.scale(_scaleX, _scaleY);
                    MatrixUtil.skew(_transformationMatrix, _skewX, _skewY);
                    _transformationMatrix.rotate(_rotation);
                    _transformationMatrix.translate(_x, _y);
                    
                    if (_pivotX != 0.0 || _pivotY != 0.0)
                    {
                        // prepend pivot transformation
                        _transformationMatrix.tx = _x - _transformationMatrix.a * _pivotX
                                                      - _transformationMatrix.c * _pivotY;
                        _transformationMatrix.ty = _y - _transformationMatrix.b * _pivotX
                                                      - _transformationMatrix.d * _pivotY;
                    }
                }
            }
            
            return _transformationMatrix;
        }

        public function set transformationMatrix(matrix:Matrix):void
        {
            const PI_Q:Number = Math.PI / 4.0;

            setRequiresRedraw();
            _orientationChanged = false;
            _transformationMatrix.copyFrom(matrix);
            _pivotX = _pivotY = 0;
            
            _x = matrix.tx;
            _y = matrix.ty;
            
            _skewX = Math.atan(-matrix.c / matrix.d);
            _skewY = Math.atan( matrix.b / matrix.a);

            // NaN check ("isNaN" causes allocation)
            if (_skewX != _skewX) _skewX = 0.0;
            if (_skewY != _skewY) _skewY = 0.0;

            _scaleY = (_skewX > -PI_Q && _skewX < PI_Q) ?  matrix.d / Math.cos(_skewX)
                                                        : -matrix.c / Math.sin(_skewX);
            _scaleX = (_skewY > -PI_Q && _skewY < PI_Q) ?  matrix.a / Math.cos(_skewY)
                                                        :  matrix.b / Math.sin(_skewY);

            if (MathUtil.isEquivalent(_skewX, _skewY))
            {
                _rotation = _skewX;
                _skewX = _skewY = 0;
            }
            else
            {
                _rotation = 0;
            }
        }
        
        /** The 3D transformation matrix of the object relative to its parent.
         *
         *  <p>For 2D objects, this property returns just a 3D version of the 2D transformation
         *  matrix. Only the 'Sprite3D' class supports real 3D transformations.</p>
         *
         *  <p>CAUTION: not a copy, but the actual object!</p> */
        public function get transformationMatrix3D():Matrix3D
        {
            // this method needs to be overridden in 3D-supporting subclasses (like Sprite3D).

            if (_transformationMatrix3D == null)
                _transformationMatrix3D = new Matrix3D();

            return MatrixUtil.convertTo3D(transformationMatrix, _transformationMatrix3D);
        }

        /** Indicates if this object or any of its parents is a 'Sprite3D' object. */
        public function get is3D():Boolean { return _is3D; }

        /** Indicates if the mouse cursor should transform into a hand while it's over the sprite.
         *  @default false */
        public function get useHandCursor():Boolean { return _useHandCursor; }
        public function set useHandCursor(value:Boolean):void
        {
            if (value == _useHandCursor) return;
            _useHandCursor = value;
            
            if (_useHandCursor)
                addEventListener(TouchEvent.TOUCH, onTouch);
            else
                removeEventListener(TouchEvent.TOUCH, onTouch);
        }
        
        private function onTouch(event:TouchEvent):void
        {
            Mouse.cursor = event.interactsWith(this) ? MouseCursor.BUTTON : MouseCursor.AUTO;
        }
        
        /** The bounds of the object relative to the local coordinates of the parent. */
        public function get bounds():Rectangle
        {
            return getBounds(_parent);
        }
        
        /** The width of the object in pixels.
         *  Note that for objects in a 3D space (connected to a Sprite3D), this value might not
         *  be accurate until the object is part of the display list. */
        public function get width():Number { return getBounds(_parent, sHelperRect).width; }
        public function set width(value:Number):void
        {
            // this method calls 'this.scaleX' instead of changing _scaleX directly.
            // that way, subclasses reacting on size changes need to override only the scaleX method.

            var actualWidth:Number;
            var scaleIsNaN:Boolean = _scaleX != _scaleX; // avoid 'isNaN' call

            if (_scaleX == 0.0 || scaleIsNaN) { scaleX = 1.0; actualWidth = width; }
            else actualWidth = Math.abs(width / _scaleX);

            if (actualWidth) scaleX = value / actualWidth;
        }
        
        /** The height of the object in pixels.
         *  Note that for objects in a 3D space (connected to a Sprite3D), this value might not
         *  be accurate until the object is part of the display list. */
        public function get height():Number { return getBounds(_parent, sHelperRect).height; }
        public function set height(value:Number):void
        {
            var actualHeight:Number;
            var scaleIsNaN:Boolean = _scaleY != _scaleY; // avoid 'isNaN' call

            if (_scaleY == 0.0 || scaleIsNaN) { scaleY = 1.0; actualHeight = height; }
            else actualHeight = Math.abs(height / _scaleY);

            if (actualHeight) scaleY = value / actualHeight;
        }
        
        /** The x coordinate of the object relative to the local coordinates of the parent. */
        public function get x():Number { return _x; }
        public function set x(value:Number):void 
        { 
            if (_x != value)
            {
                _x = value;
                setOrientationChanged();
            }
        }
        
        /** The y coordinate of the object relative to the local coordinates of the parent. */
        public function get y():Number { return _y; }
        public function set y(value:Number):void 
        {
            if (_y != value)
            {
                _y = value;
                setOrientationChanged();
            }
        }
        
        /** The x coordinate of the object's origin in its own coordinate space (default: 0). */
        public function get pivotX():Number { return _pivotX; }
        public function set pivotX(value:Number):void 
        {
            if (_pivotX != value)
            {
                _pivotX = value;
                setOrientationChanged();
            }
        }
        
        /** The y coordinate of the object's origin in its own coordinate space (default: 0). */
        public function get pivotY():Number { return _pivotY; }
        public function set pivotY(value:Number):void 
        { 
            if (_pivotY != value)
            {
                _pivotY = value;
                setOrientationChanged();
            }
        }
        
        /** The horizontal scale factor. '1' means no scale, negative values flip the object.
         *  @default 1 */
        public function get scaleX():Number { return _scaleX; }
        public function set scaleX(value:Number):void 
        { 
            if (_scaleX != value)
            {
                _scaleX = value;
                setOrientationChanged();
            }
        }
        
        /** The vertical scale factor. '1' means no scale, negative values flip the object.
         *  @default 1 */
        public function get scaleY():Number { return _scaleY; }
        public function set scaleY(value:Number):void 
        { 
            if (_scaleY != value)
            {
                _scaleY = value;
                setOrientationChanged();
            }
        }

        /** Sets both 'scaleX' and 'scaleY' to the same value. The getter simply returns the
         *  value of 'scaleX' (even if the scaling values are different). @default 1 */
        public function get scale():Number { return scaleX; }
        public function set scale(value:Number):void { scaleX = scaleY = value; }
        
        /** The horizontal skew angle in radians. */
        public function get skewX():Number { return _skewX; }
        public function set skewX(value:Number):void 
        {
            value = MathUtil.normalizeAngle(value);
            
            if (_skewX != value)
            {
                _skewX = value;
                setOrientationChanged();
            }
        }
        
        /** The vertical skew angle in radians. */
        public function get skewY():Number { return _skewY; }
        public function set skewY(value:Number):void 
        {
            value = MathUtil.normalizeAngle(value);
            
            if (_skewY != value)
            {
                _skewY = value;
                setOrientationChanged();
            }
        }
        
        /** The rotation of the object in radians. (In Starling, all angles are measured 
         *  in radians.) */
        public function get rotation():Number { return _rotation; }
        public function set rotation(value:Number):void 
        {
            value = MathUtil.normalizeAngle(value);

            if (_rotation != value)
            {            
                _rotation = value;
                setOrientationChanged();
            }
        }

        /** @private Indicates if the object is rotated or skewed in any way. */
        internal function get isRotated():Boolean
        {
            return _rotation != 0.0 || _skewX != 0.0 || _skewY != 0.0;
        }
        
        /** The opacity of the object. 0 = transparent, 1 = opaque. @default 1 */
        public function get alpha():Number { return _alpha; }
        public function set alpha(value:Number):void 
        {
            if (value != _alpha)
            {
                _alpha = value < 0.0 ? 0.0 : (value > 1.0 ? 1.0 : value);
                setRequiresRedraw();
            }
        }
        
        /** The visibility of the object. An invisible object will be untouchable. */
        public function get visible():Boolean { return _visible; }
        public function set visible(value:Boolean):void
        {
            if (value != _visible)
            {
                _visible = value;
                setRequiresRedraw();
            }
        }
        
        /** Indicates if this object (and its children) will receive touch events. */
        public function get touchable():Boolean { return _touchable; }
        public function set touchable(value:Boolean):void { _touchable = value; }
        
        /** The blend mode determines how the object is blended with the objects underneath. 
         *   @default auto
         *   @see starling.display.BlendMode */ 
        public function get blendMode():String { return _blendMode; }
        public function set blendMode(value:String):void
        {
            if (value != _blendMode)
            {
                _blendMode = value;
                setRequiresRedraw();
            }
        }
        
        /** The name of the display object (default: null). Used by 'getChildByName()' of 
         *  display object containers. */
        public function get name():String { return _name; }
        public function set name(value:String):void { _name = value; }
        
        /** The filter that is attached to the display object. The <code>starling.filters</code>
         *  package contains several classes that define specific filters you can use. To combine
         *  several filters, assign an instance of the <code>FilterChain</code> class; to remove
         *  all filters, assign <code>null</code>.
         *
         *  <p>Beware that a filter instance may only be used on one object at a time! Furthermore,
         *  when you remove or replace a filter, it is NOT disposed automatically (since you might
         *  want to reuse it on a different object).</p>
         *
         *  @default null
         *  @see starling.filters.FragmentFilter
         *  @see starling.filters.FilterChain
         */
        public function get filter():FragmentFilter { return _filter; }
        public function set filter(value:FragmentFilter):void
        {
            if (value != _filter)
            {
                if (_filter) _filter.setTarget(null);
                if (value) value.setTarget(this);

                _filter = value;
                setRequiresRedraw();
            }
        }

        /** The display object that acts as a mask for the current object.
         *  Assign <code>null</code> to remove it.
         *
         *  <p>A pixel of the masked display object will only be drawn if it is within one of the
         *  mask's polygons. Texture pixels and alpha values of the mask are not taken into
         *  account. The mask object itself is never visible.</p>
         *
         *  <p>If the mask is part of the display list, masking will occur at exactly the
         *  location it occupies on the stage. If it is not, the mask will be placed in the local
         *  coordinate system of the target object (as if it was one of its children).</p>
         *
         *  <p>For rectangular masks, you can use simple quads; for other forms (like circles
         *  or arbitrary shapes) it is recommended to use a 'Canvas' instance.</p>
         *
         *  <p>Beware that a mask will typically cause at least two additional draw calls:
         *  one to draw the mask to the stencil buffer and one to erase it. However, if the
         *  mask object is an instance of <code>starling.display.Quad</code> and is aligned
         *  parallel to the stage axes, rendering will be optimized: instead of using the
         *  stencil buffer, the object will be clipped using the scissor rectangle. That's
         *  faster and reduces the number of draw calls, so make use of this when possible.</p>
         *
         *  @see Canvas
         *  @default null
         */
        public function get mask():DisplayObject { return _mask; }
        public function set mask(value:DisplayObject):void
        {
            if (_mask != value)
            {
                if (_mask) _mask._maskee = null;
                if (value)
                {
                    value._maskee = this;
                    value._hasVisibleArea = false;
                }

                _mask = value;
                setRequiresRedraw();
            }
        }

        /** The display object container that contains this display object. */
        public function get parent():DisplayObjectContainer { return _parent; }
        
        /** The topmost object in the display tree the object is part of. */
        public function get base():DisplayObject
        {
            var currentObject:DisplayObject = this;
            while (currentObject._parent) currentObject = currentObject._parent;
            return currentObject;
        }
        
        /** The root object the display object is connected to (i.e. an instance of the class 
         *  that was passed to the Starling constructor), or null if the object is not connected
         *  to the stage. */
        public function get root():DisplayObject
        {
            var currentObject:DisplayObject = this;
            while (currentObject._parent)
            {
                if (currentObject._parent is Stage) return currentObject;
                else currentObject = currentObject.parent;
            }
            
            return null;
        }
        
        /** The stage the display object is connected to, or null if it is not connected 
         *  to the stage. */
        public function get stage():Stage { return this.base as Stage; }
    }
}
