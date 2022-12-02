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
    import flash.geom.Rectangle;
    import flash.ui.Mouse;
    import flash.ui.MouseCursor;

    import starling.events.Event;
    import starling.events.Touch;
    import starling.events.TouchEvent;
    import starling.events.TouchPhase;
    import starling.styles.MeshStyle;
    import starling.text.TextField;
    import starling.text.TextFormat;
    import starling.textures.Texture;

    /** Dispatched when the user triggers the button. Bubbles. */
    [Event(name="triggered", type="starling.events.Event")]
    
    /** A simple button composed of an image and, optionally, text.
     *  
     *  <p>You can use different textures for various states of the button. If you're providing
     *  only an up state, the button is simply scaled a little when it is touched.</p>
     *
     *  <p>In addition, you can overlay text on the button. To customize the text, you can use
     *  properties equivalent to those of the TextField class. Move the text to a certain position
     *  by updating the <code>textBounds</code> property.</p>
     *  
     *  <p>To react on touches on a button, there is special <code>Event.TRIGGERED</code> event.
     *  Use this event instead of normal touch events. That way, users can cancel button
     *  activation by moving the mouse/finger away from the button before releasing.</p>
     */
    public class Button extends DisplayObjectContainer
    {
        private static const MAX_DRAG_DIST:Number = 50;
        
        private var _upState:Texture;
        private var _downState:Texture;
        private var _overState:Texture;
        private var _disabledState:Texture;
        
        private var _contents:Sprite;
        private var _body:Image;
        private var _textField:TextField;
        private var _textBounds:Rectangle;
        private var _overlay:Sprite;
        
        private var _scaleWhenDown:Number;
        private var _scaleWhenOver:Number;
        private var _alphaWhenDown:Number;
        private var _alphaWhenDisabled:Number;
        private var _useHandCursor:Boolean;
        private var _enabled:Boolean;
        private var _state:String;
        private var _triggerBounds:Rectangle;

        /** Creates a button with a set of state-textures and (optionally) some text.
         *  Any state that is left 'null' will display the up-state texture. Beware that all
         *  state textures should have the same dimensions. */
        public function Button(upState:Texture, text:String="", downState:Texture=null,
                               overState:Texture=null, disabledState:Texture=null)
        {
            if (upState == null) throw new ArgumentError("Texture 'upState' cannot be null");
            
            _upState = upState;
            _downState = downState;
            _overState = overState;
            _disabledState = disabledState;

            _state = ButtonState.UP;
            _body = new Image(upState);
            _body.pixelSnapping = true;
            _scaleWhenDown = downState ? 1.0 : 0.9;
            _scaleWhenOver = _alphaWhenDown = 1.0;
            _alphaWhenDisabled = disabledState ? 1.0: 0.5;
            _enabled = true;
            _useHandCursor = true;
            _textBounds = new Rectangle(0, 0, _body.width, _body.height);
            _triggerBounds = new Rectangle();
            
            _contents = new Sprite();
            _contents.addChild(_body);
            addChild(_contents);
            addEventListener(TouchEvent.TOUCH, onTouch);
            
            this.touchGroup = true;
            this.text = text;
        }
        
        /** @inheritDoc */
        public override function dispose():void
        {
            // text field might be disconnected from parent, so we have to dispose it manually
            if (_textField)
                _textField.dispose();
            
            super.dispose();
        }
        
        /** Readjusts the dimensions of the button according to its current state texture.
         *  Call this method to synchronize button and texture size after assigning a texture
         *  with a different size. */
        public function readjustSize():void
        {
            var prevWidth:Number = _body.width;
            var prevHeight:Number = _body.height;

            _body.readjustSize();

            var scaleX:Number = _body.width  / prevWidth;
            var scaleY:Number = _body.height / prevHeight;

            _textBounds.x *= scaleX;
            _textBounds.y *= scaleY;
            _textBounds.width *= scaleX;
            _textBounds.height *= scaleY;

            if (_textField) createTextField();
        }

        private function createTextField():void
        {
            if (_textField == null)
            {
                _textField = new TextField(_textBounds.width, _textBounds.height);
                _textField.pixelSnapping = _body.pixelSnapping;
                _textField.touchable = false;
                _textField.autoScale = true;
                _textField.batchable = true;
            }
            
            _textField.width  = _textBounds.width;
            _textField.height = _textBounds.height;
            _textField.x = _textBounds.x;
            _textField.y = _textBounds.y;
        }
        
        private function onTouch(event:TouchEvent):void
        {
            Mouse.cursor = (_useHandCursor && _enabled && event.interactsWith(this)) ?
                MouseCursor.BUTTON : MouseCursor.AUTO;
            
            var touch:Touch = event.getTouch(this);
            var isWithinBounds:Boolean;

            if (!_enabled)
            {
                return;
            }
            else if (touch == null)
            {
                state = ButtonState.UP;
            }
            else if (touch.phase == TouchPhase.HOVER)
            {
                state = ButtonState.OVER;
            }
            else if (touch.phase == TouchPhase.BEGAN && _state != ButtonState.DOWN)
            {
                _triggerBounds = getBounds(stage, _triggerBounds);
                _triggerBounds.inflate(MAX_DRAG_DIST, MAX_DRAG_DIST);

                state = ButtonState.DOWN;
            }
            else if (touch.phase == TouchPhase.MOVED)
            {
                isWithinBounds = _triggerBounds.contains(touch.globalX, touch.globalY);

                if (_state == ButtonState.DOWN && !isWithinBounds)
                {
                    // reset button when finger is moved too far away ...
                    state = ButtonState.UP;
                }
                else if (_state == ButtonState.UP && isWithinBounds)
                {
                    // ... and reactivate when the finger moves back into the bounds.
                    state = ButtonState.DOWN;
                }
            }
            else if (touch.phase == TouchPhase.ENDED && _state == ButtonState.DOWN)
            {
                state = ButtonState.UP;
                if (!touch.cancelled) dispatchEventWith(Event.TRIGGERED, true);
            }
        }
        
        /** The current state of the button. The corresponding strings are found
         *  in the ButtonState class. */
        public function get state():String { return _state; }
        public function set state(value:String):void
        {
            _state = value;
            _contents.x = _contents.y = 0;
            _contents.scaleX = _contents.scaleY = _contents.alpha = 1.0;

            switch (_state)
            {
                case ButtonState.DOWN:
                    setStateTexture(_downState);
                    _contents.alpha = _alphaWhenDown;
                    _contents.scaleX = _contents.scaleY = _scaleWhenDown;
                    _contents.x = (1.0 - _scaleWhenDown) / 2.0 * _body.width;
                    _contents.y = (1.0 - _scaleWhenDown) / 2.0 * _body.height;
                    break;
                case ButtonState.UP:
                    setStateTexture(_upState);
                    break;
                case ButtonState.OVER:
                    setStateTexture(_overState);
                    _contents.scaleX = _contents.scaleY = _scaleWhenOver;
                    _contents.x = (1.0 - _scaleWhenOver) / 2.0 * _body.width;
                    _contents.y = (1.0 - _scaleWhenOver) / 2.0 * _body.height;
                    break;
                case ButtonState.DISABLED:
                    setStateTexture(_disabledState);
                    _contents.alpha = _alphaWhenDisabled;
                    break;
                default:
                    throw new ArgumentError("Invalid button state: " + _state);
            }
        }

        private function setStateTexture(texture:Texture):void
        {
            _body.texture = texture ? texture : _upState;
        }

        /** The scale factor of the button on touch. Per default, a button without a down state
         *  texture will be made slightly smaller, while a button with a down state texture
         *  remains unscaled. */
        public function get scaleWhenDown():Number { return _scaleWhenDown; }
        public function set scaleWhenDown(value:Number):void { _scaleWhenDown = value; }

        /** The scale factor of the button while the mouse cursor hovers over it. @default 1.0 */
        public function get scaleWhenOver():Number { return _scaleWhenOver; }
        public function set scaleWhenOver(value:Number):void { _scaleWhenOver = value; }

        /** The alpha value of the button on touch. @default 1.0 */
        public function get alphaWhenDown():Number { return _alphaWhenDown; }
        public function set alphaWhenDown(value:Number):void { _alphaWhenDown = value; }

        /** The alpha value of the button when it is disabled. @default 0.5 */
        public function get alphaWhenDisabled():Number { return _alphaWhenDisabled; }
        public function set alphaWhenDisabled(value:Number):void { _alphaWhenDisabled = value; }
        
        /** Indicates if the button can be triggered. */
        public function get enabled():Boolean { return _enabled; }
        public function set enabled(value:Boolean):void
        {
            if (_enabled != value)
            {
                _enabled = value;
                state = value ? ButtonState.UP : ButtonState.DISABLED;
            }
        }
        
        /** The text that is displayed on the button. */
        public function get text():String { return _textField ? _textField.text : ""; }
        public function set text(value:String):void
        {
            if (value.length == 0)
            {
                if (_textField)
                {
                    _textField.text = value;
                    _textField.removeFromParent();
                }
            }
            else
            {
                createTextField();
                _textField.text = value;
                
                if (_textField.parent == null)
                    _contents.addChild(_textField);
            }
        }

        /** The format of the button's TextField. */
        public function get textFormat():TextFormat
        {
            if (_textField == null) createTextField();
            return _textField.format;
        }

        public function set textFormat(value:TextFormat):void
        {
            if (_textField == null) createTextField();
            _textField.format = value;
        }

        /** The style that is used to render the button's TextField. */
        public function get textStyle():MeshStyle
        {
            if (_textField == null) createTextField();
            return _textField.style;
        }

        public function set textStyle(value:MeshStyle):void
        {
            if (_textField == null) createTextField();
            _textField.style = value;
        }

        /** The style that is used to render the Button. */
        public function get style():MeshStyle { return _body.style; }
        public function set style(value:MeshStyle):void { _body.style = value; }

        /** The texture that is displayed when the button is not being touched. */
        public function get upState():Texture { return _upState; }
        public function set upState(value:Texture):void
        {
            if (value == null)
                throw new ArgumentError("Texture 'upState' cannot be null");

            if (_upState != value)
            {
                _upState = value;
                if ( _state == ButtonState.UP ||
                    (_state == ButtonState.DISABLED && _disabledState == null) ||
                    (_state == ButtonState.DOWN && _downState == null) ||
                    (_state == ButtonState.OVER && _overState == null))
                {
                    setStateTexture(value);
                }
            }
        }
        
        /** The texture that is displayed while the button is touched. */
        public function get downState():Texture { return _downState; }
        public function set downState(value:Texture):void
        {
            if (_downState != value)
            {
                _downState = value;
                if (_state == ButtonState.DOWN) setStateTexture(value);
            }
        }

        /** The texture that is displayed while mouse hovers over the button. */
        public function get overState():Texture { return _overState; }
        public function set overState(value:Texture):void
        {
            if (_overState != value)
            {
                _overState = value;
                if (_state == ButtonState.OVER) setStateTexture(value);
            }
        }

        /** The texture that is displayed when the button is disabled. */
        public function get disabledState():Texture { return _disabledState; }
        public function set disabledState(value:Texture):void
        {
            if (_disabledState != value)
            {
                _disabledState = value;
                if (_state == ButtonState.DISABLED) setStateTexture(value);
            }
        }
        
        /** The bounds of the button's TextField. Allows moving the text to a custom position.
         *  CAUTION: not a copy, but the actual object! Text will only update on re-assignment.
         */
        public function get textBounds():Rectangle { return _textBounds; }
        public function set textBounds(value:Rectangle):void
        {
            _textBounds.copyFrom(value);
            createTextField();
        }
        
        /** The color of the button's state image. Just like every image object, each pixel's
         *  color is multiplied with this value. @default white */
        public function get color():uint { return _body.color; }
        public function set color(value:uint):void { _body.color = value; }

        /** The smoothing type used for the button's state image. */
        public function get textureSmoothing():String { return _body.textureSmoothing; }
        public function set textureSmoothing(value:String):void { _body.textureSmoothing = value; }

        /** The overlay sprite is displayed on top of the button contents. It scales with the
         *  button when pressed. Use it to add additional objects to the button (e.g. an icon). */
        public function get overlay():Sprite
        {
            if (_overlay == null)
                _overlay = new Sprite();

            _contents.addChild(_overlay); // make sure it's always on top
            return _overlay;
        }

        /** Indicates if the mouse cursor should transform into a hand while it's over the button. 
         *  @default true */
        public override function get useHandCursor():Boolean { return _useHandCursor; }
        public override function set useHandCursor(value:Boolean):void { _useHandCursor = value; }

        /** Controls whether or not the instance snaps to the nearest pixel. This can prevent the
         *  object from looking blurry when it's not exactly aligned with the pixels of the screen.
         *  @default true */
        public function get pixelSnapping():Boolean { return _body.pixelSnapping; }
        public function set pixelSnapping(value:Boolean):void
        {
            _body.pixelSnapping = value;
            if (_textField) _textField.pixelSnapping = value;
        }

        /** @private */
        override public function set width(value:Number):void
        {
            // The Button might use a Scale9Grid ->
            // we must update the body width/height manually for the grid to scale properly.

            var newWidth:Number = value / (this.scaleX || 1.0);
            var scale:Number = newWidth / (_body.width || 1.0);

            _body.width = newWidth;
            _textBounds.x *= scale;
            _textBounds.width *= scale;

            if (_textField) _textField.width = newWidth;
        }

        /** @private */
        override public function set height(value:Number):void
        {
            var newHeight:Number = value /  (this.scaleY || 1.0);
            var scale:Number = newHeight / (_body.height || 1.0);

            _body.height = newHeight;
            _textBounds.y *= scale;
            _textBounds.height *= scale;

            if (_textField) _textField.height = newHeight;
        }

        /** The current scaling grid used for the button's state image. Use this property to create
         *  buttons that resize in a smart way, i.e. with the four corners keeping the same size
         *  and only stretching the center area.
         *
         *  @see Image#scale9Grid
         *  @default null
         */
        public function get scale9Grid():Rectangle { return _body.scale9Grid; }
        public function set scale9Grid(value:Rectangle):void { _body.scale9Grid = value; }
    }
}