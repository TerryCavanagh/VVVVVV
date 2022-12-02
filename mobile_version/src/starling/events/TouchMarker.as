// =================================================================================================
//
//	Starling Framework
//	Copyright Gamua GmbH. All Rights Reserved.
//
//	This program is free software. You can redistribute and/or modify it
//	in accordance with the terms of the accompanying license agreement.
//
// =================================================================================================

package starling.events
{
    import flash.display.BitmapData;
    import flash.display.Shape;
    import flash.geom.Point;

    import starling.core.Starling;
    import starling.display.Image;
    import starling.display.Sprite;
    import starling.textures.Texture;

    /** The TouchMarker is used internally to mark touches created through "simulateMultitouch". */
    internal class TouchMarker extends Sprite
    {
        private var _center:Point;
        private var _texture:Texture;
        
        public function TouchMarker()
        {
            _center = new Point();
            _texture = createTexture();
            
            for (var i:int=0; i<2; ++i)
            {
                var marker:Image = new Image(_texture);
                marker.pivotX = _texture.width / 2;
                marker.pivotY = _texture.height / 2;
                marker.touchable = false;
                addChild(marker);
            }
        }
        
        public override function dispose():void
        {
            _texture.dispose();
            super.dispose();
        }
        
        public function moveMarker(x:Number, y:Number, withCenter:Boolean=false):void
        {
            if (withCenter)
            {
                _center.x += x - realMarker.x;
                _center.y += y - realMarker.y;
            }
            
            realMarker.x = x;
            realMarker.y = y;
            mockMarker.x = 2*_center.x - x;
            mockMarker.y = 2*_center.y - y;
        }
        
        public function moveCenter(x:Number, y:Number):void
        {
            _center.x = x;
            _center.y = y;
            moveMarker(realX, realY); // reset mock position
        }
        
        private function createTexture():Texture
        {
            var scale:Number = Starling.contentScaleFactor;
            var radius:Number = 12 * scale;
            var width:int = 32 * scale;
            var height:int = 32 * scale;
            var thickness:Number = 1.5 * scale;
            var shape:Shape = new Shape();
            
            // draw dark outline
            shape.graphics.lineStyle(thickness, 0x0, 0.3);
            shape.graphics.drawCircle(width/2, height/2, radius + thickness);
            
            // draw white inner circle
            shape.graphics.beginFill(0xffffff, 0.4);
            shape.graphics.lineStyle(thickness, 0xffffff);
            shape.graphics.drawCircle(width/2, height/2, radius);
            shape.graphics.endFill();
            
            var bmpData:BitmapData = new BitmapData(width, height, true, 0x0);
            bmpData.draw(shape);
            
            return Texture.fromBitmapData(bmpData, false, false, scale);
        }
        
        private function get realMarker():Image { return getChildAt(0) as Image; }
        private function get mockMarker():Image { return getChildAt(1) as Image; }
        
        public function get realX():Number { return realMarker.x; }
        public function get realY():Number { return realMarker.y; }
        
        public function get mockX():Number { return mockMarker.x; }
        public function get mockY():Number { return mockMarker.y; }
    }        
}