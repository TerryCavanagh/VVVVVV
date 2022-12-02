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

    import starling.rendering.IndexData;
    import starling.rendering.VertexData;
    import starling.textures.Texture;
    import starling.utils.MathUtil;
    import starling.utils.Padding;
    import starling.utils.Pool;
    import starling.utils.RectangleUtil;

    /** An Image is a quad with a texture mapped onto it.
     *
     *  <p>Typically, the Image class will act as an equivalent of Flash's Bitmap class. Instead
     *  of BitmapData, Starling uses textures to represent the pixels of an image. To display a
     *  texture, you have to map it onto a quad - and that's what the Image class is for.</p>
     *
     *  <p>While the base class <code>Quad</code> already supports textures, the <code>Image</code>
     *  class adds some additional functionality.</p>
     *
     *  <p>First of all, it provides a convenient constructor that will automatically synchronize
     *  the size of the image with the displayed texture.</p>
     *
     *  <p>Furthermore, it adds support for a "Scale9" grid. This splits up the image into
     *  nine regions, the corners of which will always maintain their original size.
     *  The center region stretches in both directions to fill the remaining space; the side
     *  regions will stretch accordingly in either horizontal or vertical direction.</p>
     *
     *  <p>Finally, you can repeat a texture horizontally and vertically within the image's region,
     *  just like the tiles of a wallpaper. Use the <code>tileGrid</code> property to do that.</p>
     *
     *  @see starling.textures.Texture
     *  @see Quad
     */ 
    public class Image extends Quad
    {
        private var _scale9Grid:Rectangle;
        private var _tileGrid:Rectangle;

        // helper objects
        private static var sPadding:Padding = new Padding();
        private static var sBounds:Rectangle = new Rectangle();
        private static var sBasCols:Vector.<Number> = new Vector.<Number>(3, true);
        private static var sBasRows:Vector.<Number> = new Vector.<Number>(3, true);
        private static var sPosCols:Vector.<Number> = new Vector.<Number>(3, true);
        private static var sPosRows:Vector.<Number> = new Vector.<Number>(3, true);
        private static var sTexCols:Vector.<Number> = new Vector.<Number>(3, true);
        private static var sTexRows:Vector.<Number> = new Vector.<Number>(3, true);

        /** Creates an image with a texture mapped onto it. */
        public function Image(texture:Texture)
        {
            super(100, 100);
            this.texture = texture;
            readjustSize();
        }

        /** The current scaling grid that is in effect. If set to null, the image is scaled just
         *  like any other display object; assigning a rectangle will divide the image into a grid
         *  of nine regions, based on the center rectangle. The four corners of this grid will
         *  always maintain their original size; the other regions will stretch (horizontally,
         *  vertically, or both) to fill the complete area.
         *
         *  <p>Notes:</p>
         *
         *  <ul>
         *  <li>Assigning a Scale9 rectangle will change the number of vertices to a maximum of 16
         *  (less if possible) and all vertices will be colored like vertex 0 (the top left vertex).
         *  </li>
         *  <li>For Scale3-grid behavior, assign a zero size for all but the center row / column.
         *  This will cause the 'caps' to scale in a way that leaves the aspect ratio intact.</li>
         *  <li>An image can have either a <code>scale9Grid</code> or a <code>tileGrid</code>, but
         *  not both. Assigning one will delete the other.</li>
         *  <li>Changes will only be applied on assignment. To force an update, simply call
         *  <code>image.scale9Grid = image.scale9Grid</code>.</li>
         *  <li>Assignment causes an implicit call to <code>readjustSize()</code>,
         *  and the same will happen when the texture is changed afterwards.</li>
         *  </ul>
         *
         *  @default null
         */
        public function get scale9Grid():Rectangle { return _scale9Grid; }
        public function set scale9Grid(value:Rectangle):void
        {
            if (value)
            {
                if (_scale9Grid == null) _scale9Grid = value.clone();
                else _scale9Grid.copyFrom(value);

                readjustSize();
                _tileGrid = null;
            }
            else _scale9Grid = null;

            setupVertices();
        }

        /** The current tiling grid that is in effect. If set to null, the image is scaled just
         *  like any other display object; assigning a rectangle will divide the image into a grid
         *  displaying the current texture in each and every cell. The assigned rectangle points
         *  to the bounds of one cell; all other elements will be calculated accordingly. A zero
         *  or negative value for the rectangle's width or height will be replaced with the actual
         *  texture size. Thus, you can make a 2x2 grid simply like this:
         *
         *  <listing>
         *  var image:Image = new Image(texture);
         *  image.tileGrid = new Rectangle();
         *  image.scale = 2;</listing>
         *
         *  <p>Notes:</p>
         *
         *  <ul>
         *  <li>Assigning a tile rectangle will change the number of vertices to whatever is
         *  required by the grid. New vertices will be colored just like vertex 0 (the top left
         *  vertex).</li>
         *  <li>An image can have either a <code>scale9Grid</code> or a <code>tileGrid</code>, but
         *  not both. Assigning one will delete the other.</li>
         *  <li>Changes will only be applied on assignment. To force an update, simply call
         *  <code>image.tileGrid = image.tileGrid</code>.</li>
         *  </ul>
         *
         *  @default null
         */
        public function get tileGrid():Rectangle { return _tileGrid; }
        public function set tileGrid(value:Rectangle):void
        {
            if (value)
            {
                if (_tileGrid == null) _tileGrid = value.clone();
                else _tileGrid.copyFrom(value);

                _scale9Grid = null;
            }
            else _tileGrid = null;

            setupVertices();
        }

        /** @private */
        override protected function setupVertices():void
        {
            if (texture && _scale9Grid) setupScale9Grid();
            else if (texture && _tileGrid) setupTileGrid();
            else super.setupVertices();
        }

        /** @private */
        override public function set scaleX(value:Number):void
        {
            super.scaleX = value;
            if (texture && (_scale9Grid || _tileGrid)) setupVertices();
        }

        /** @private */
        override public function set scaleY(value:Number):void
        {
            super.scaleY = value;
            if (texture && (_scale9Grid || _tileGrid)) setupVertices();
        }

        /** @private */
        override public function set texture(value:Texture):void
        {
            if (value != texture)
            {
                super.texture = value;
                if (_scale9Grid && value) readjustSize();
            }
        }

        // vertex setup

        private function setupScale9Grid():void
        {
            var texture:Texture = this.texture;
            var frame:Rectangle = texture.frame;
            var absScaleX:Number = scaleX > 0 ? scaleX : -scaleX;
            var absScaleY:Number = scaleY > 0 ? scaleY : -scaleY;

            // If top and bottom row / left and right column are empty, this is actually
            // a scale3 grid. In that case, we want the 'caps' to maintain their aspect ratio.

            if (MathUtil.isEquivalent(_scale9Grid.width, texture.frameWidth))
                absScaleY /= absScaleX;
            else if (MathUtil.isEquivalent(_scale9Grid.height, texture.frameHeight))
                absScaleX /= absScaleY;

            var invScaleX:Number = 1.0 / absScaleX;
            var invScaleY:Number = 1.0 / absScaleY;
            var vertexData:VertexData = this.vertexData;
            var indexData:IndexData = this.indexData;
            var prevNumVertices:int = vertexData.numVertices;
            var numVertices:int, numQuads:int;
            var correction:Number;

            // The following rectangles are used to figure everything out.
            // The meaning of each is depicted in this sketch: http://i.imgur.com/KUcv71O.jpg

            var gridCenter:Rectangle = Pool.getRectangle();
            var textureBounds:Rectangle = Pool.getRectangle();
            var pixelBounds:Rectangle = Pool.getRectangle();
            var intersection:Rectangle = Pool.getRectangle();

            gridCenter.copyFrom(_scale9Grid);
            textureBounds.setTo(0, 0, texture.frameWidth, texture.frameHeight);

            if (frame) pixelBounds.setTo(-frame.x, -frame.y, texture.width, texture.height);
            else       pixelBounds.copyFrom(textureBounds);

            // calculate 3x3 grid according to texture and scale9 properties,
            // taking special care about the texture frame (headache included)

            RectangleUtil.intersect(gridCenter, pixelBounds, intersection);

            sBasCols[0] = sBasCols[2] = 0;
            sBasRows[0] = sBasRows[2] = 0;
            sBasCols[1] = intersection.width;
            sBasRows[1] = intersection.height;

            if (pixelBounds.x < gridCenter.x)
                sBasCols[0] = gridCenter.x - pixelBounds.x;

            if (pixelBounds.y < gridCenter.y)
                sBasRows[0] = gridCenter.y - pixelBounds.y;

            if (pixelBounds.right > gridCenter.right)
                sBasCols[2] = pixelBounds.right - gridCenter.right;

            if (pixelBounds.bottom > gridCenter.bottom)
                sBasRows[2] = pixelBounds.bottom - gridCenter.bottom;

            // set vertex positions

            if (pixelBounds.x < gridCenter.x)
                sPadding.left = pixelBounds.x * invScaleX;
            else
                sPadding.left = gridCenter.x * invScaleX + pixelBounds.x - gridCenter.x;

            if (pixelBounds.right > gridCenter.right)
                sPadding.right = (textureBounds.width - pixelBounds.right) * invScaleX;
            else
                sPadding.right = (textureBounds.width - gridCenter.right) * invScaleX + gridCenter.right - pixelBounds.right;

            if (pixelBounds.y < gridCenter.y)
                sPadding.top = pixelBounds.y * invScaleY;
            else
                sPadding.top = gridCenter.y * invScaleY + pixelBounds.y - gridCenter.y;

            if (pixelBounds.bottom > gridCenter.bottom)
                sPadding.bottom = (textureBounds.height - pixelBounds.bottom) * invScaleY;
            else
                sPadding.bottom = (textureBounds.height - gridCenter.bottom) * invScaleY + gridCenter.bottom - pixelBounds.bottom;

            sPosCols[0] = sBasCols[0] * invScaleX;
            sPosCols[2] = sBasCols[2] * invScaleX;
            sPosCols[1] = textureBounds.width - sPadding.left - sPadding.right - sPosCols[0] - sPosCols[2];

            sPosRows[0] = sBasRows[0] * invScaleY;
            sPosRows[2] = sBasRows[2] * invScaleY;
            sPosRows[1] = textureBounds.height - sPadding.top - sPadding.bottom - sPosRows[0] - sPosRows[2];

            // if the total width / height becomes smaller than the outer columns / rows,
            // we hide the center column / row and scale the rest normally.

            if (sPosCols[1] <= 0)
            {
                correction = textureBounds.width / (textureBounds.width - gridCenter.width) * absScaleX;
                sPadding.left *= correction;
                sPosCols[0] *= correction;
                sPosCols[1]  = 0.0;
                sPosCols[2] *= correction;
            }

            if (sPosRows[1] <= 0)
            {
                correction = textureBounds.height / (textureBounds.height - gridCenter.height) * absScaleY;
                sPadding.top *= correction;
                sPosRows[0] *= correction;
                sPosRows[1]  = 0.0;
                sPosRows[2] *= correction;
            }

            // now set the texture coordinates

            sTexCols[0] = sBasCols[0] / pixelBounds.width;
            sTexCols[2] = sBasCols[2] / pixelBounds.width;
            sTexCols[1] = 1.0 - sTexCols[0] - sTexCols[2];

            sTexRows[0] = sBasRows[0] / pixelBounds.height;
            sTexRows[2] = sBasRows[2] / pixelBounds.height;
            sTexRows[1] = 1.0 - sTexRows[0] - sTexRows[2];

            numVertices = setupScale9GridAttributes(
                sPadding.left, sPadding.top, sPosCols, sPosRows, sTexCols, sTexRows);

            // update indices

            numQuads = numVertices / 4;
            vertexData.numVertices = numVertices;
            indexData.numIndices = 0;

            for (var i:int=0; i<numQuads; ++i)
                indexData.addQuad(i*4, i*4 + 1, i*4 + 2, i*4 + 3);

            // if we just switched from a normal to a scale9 image,
            // we need to colorize all vertices just like the first one.

            if (numVertices != prevNumVertices)
            {
                var color:uint   = prevNumVertices ? vertexData.getColor(0) : 0xffffff;
                var alpha:Number = prevNumVertices ? vertexData.getAlpha(0) : 1.0;
                vertexData.colorize("color", color, alpha);
            }

            Pool.putRectangle(textureBounds);
            Pool.putRectangle(pixelBounds);
            Pool.putRectangle(gridCenter);
            Pool.putRectangle(intersection);

            setRequiresRedraw();
        }

        private function setupScale9GridAttributes(startX:Number, startY:Number,
                                                   posCols:Vector.<Number>, posRows:Vector.<Number>,
                                                   texCols:Vector.<Number>, texRows:Vector.<Number>):int
        {
            const posAttr:String = "position";
            const texAttr:String = "texCoords";

            var row:int, col:int;
            var colWidthPos:Number, rowHeightPos:Number;
            var colWidthTex:Number, rowHeightTex:Number;
            var vertexData:VertexData = this.vertexData;
            var texture:Texture = this.texture;
            var currentX:Number = startX;
            var currentY:Number = startY;
            var currentU:Number = 0.0;
            var currentV:Number = 0.0;
            var vertexID:int = 0;

            for (row = 0; row < 3; ++row)
            {
                rowHeightPos = posRows[row];
                rowHeightTex = texRows[row];

                if (rowHeightPos > 0)
                {
                    for (col = 0; col < 3; ++col)
                    {
                        colWidthPos = posCols[col];
                        colWidthTex = texCols[col];

                        if (colWidthPos > 0)
                        {
                            vertexData.setPoint(vertexID, posAttr, currentX, currentY);
                            texture.setTexCoords(vertexData, vertexID, texAttr, currentU, currentV);
                            vertexID++;

                            vertexData.setPoint(vertexID, posAttr, currentX + colWidthPos, currentY);
                            texture.setTexCoords(vertexData, vertexID, texAttr, currentU + colWidthTex, currentV);
                            vertexID++;

                            vertexData.setPoint(vertexID, posAttr, currentX, currentY + rowHeightPos);
                            texture.setTexCoords(vertexData, vertexID, texAttr, currentU, currentV + rowHeightTex);
                            vertexID++;

                            vertexData.setPoint(vertexID, posAttr, currentX + colWidthPos, currentY + rowHeightPos);
                            texture.setTexCoords(vertexData, vertexID, texAttr, currentU + colWidthTex, currentV + rowHeightTex);
                            vertexID++;

                            currentX += colWidthPos;
                        }

                        currentU += colWidthTex;
                    }

                    currentY += rowHeightPos;
                }

                currentX = startX;
                currentU = 0.0;
                currentV += rowHeightTex;
            }

            return vertexID;
        }

        private function setupTileGrid():void
        {
            // calculate the grid of vertices simulating a repeating / tiled texture.
            // again, texture frames make this somewhat more complicated than one would think.

            var texture:Texture = this.texture;
            var frame:Rectangle = texture.frame;
            var vertexData:VertexData = this.vertexData;
            var indexData:IndexData   = this.indexData;
            var bounds:Rectangle = getBounds(this, sBounds);
            var prevNumVertices:int = vertexData.numVertices;
            var color:uint   = prevNumVertices ? vertexData.getColor(0) : 0xffffff;
            var alpha:Number = prevNumVertices ? vertexData.getAlpha(0) : 1.0;
            var invScaleX:Number = scaleX > 0 ? 1.0 / scaleX : -1.0 / scaleX;
            var invScaleY:Number = scaleY > 0 ? 1.0 / scaleY : -1.0 / scaleY;
            var frameWidth:Number  = _tileGrid.width  > 0 ? _tileGrid.width  : texture.frameWidth;
            var frameHeight:Number = _tileGrid.height > 0 ? _tileGrid.height : texture.frameHeight;

            frameWidth  *= invScaleX;
            frameHeight *= invScaleY;

            var tileX:Number = frame ? -frame.x * (frameWidth  / frame.width)  : 0;
            var tileY:Number = frame ? -frame.y * (frameHeight / frame.height) : 0;
            var tileWidth:Number  = texture.width  * (frameWidth  / texture.frameWidth);
            var tileHeight:Number = texture.height * (frameHeight / texture.frameHeight);
            var modX:Number = (_tileGrid.x * invScaleX) % frameWidth;
            var modY:Number = (_tileGrid.y * invScaleY) % frameHeight;

            if (modX < 0) modX += frameWidth;
            if (modY < 0) modY += frameHeight;

            var startX:Number = modX + tileX;
            var startY:Number = modY + tileY;

            if (startX > (frameWidth  - tileWidth))  startX -= frameWidth;
            if (startY > (frameHeight - tileHeight)) startY -= frameHeight;

            var posLeft:Number, posRight:Number, posTop:Number, posBottom:Number;
            var texLeft:Number, texRight:Number, texTop:Number, texBottom:Number;
            var posAttrName:String = "position";
            var texAttrName:String = "texCoords";
            var currentX:Number;
            var currentY:Number = startY;
            var vertexID:int = 0;

            indexData.numIndices = 0;

            while (currentY < bounds.height)
            {
                currentX = startX;

                while (currentX < bounds.width)
                {
                    indexData.addQuad(vertexID, vertexID + 1, vertexID + 2, vertexID + 3);

                    posLeft   = currentX < 0 ? 0 : currentX;
                    posTop    = currentY < 0 ? 0 : currentY;
                    posRight  = currentX + tileWidth  > bounds.width  ? bounds.width  : currentX + tileWidth;
                    posBottom = currentY + tileHeight > bounds.height ? bounds.height : currentY + tileHeight;

                    vertexData.setPoint(vertexID,     posAttrName, posLeft,  posTop);
                    vertexData.setPoint(vertexID + 1, posAttrName, posRight, posTop);
                    vertexData.setPoint(vertexID + 2, posAttrName, posLeft,  posBottom);
                    vertexData.setPoint(vertexID + 3, posAttrName, posRight, posBottom);

                    texLeft   = (posLeft   - currentX) / tileWidth;
                    texTop    = (posTop    - currentY) / tileHeight;
                    texRight  = (posRight  - currentX) / tileWidth;
                    texBottom = (posBottom - currentY) / tileHeight;

                    texture.setTexCoords(vertexData, vertexID,     texAttrName, texLeft,  texTop);
                    texture.setTexCoords(vertexData, vertexID + 1, texAttrName, texRight, texTop);
                    texture.setTexCoords(vertexData, vertexID + 2, texAttrName, texLeft,  texBottom);
                    texture.setTexCoords(vertexData, vertexID + 3, texAttrName, texRight, texBottom);

                    currentX += frameWidth;
                    vertexID += 4;
                }

                currentY += frameHeight;
            }

            // trim to actual size
            vertexData.numVertices = vertexID;

            for (var i:int = prevNumVertices; i < vertexID; ++i)
            {
                vertexData.setColor(i, "color", color);
                vertexData.setAlpha(i, "color", alpha);
            }

            setRequiresRedraw();
        }
    }
}
