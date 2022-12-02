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
    import flash.geom.Matrix;
    import flash.geom.Matrix3D;
    import flash.geom.Point;
    import flash.geom.Rectangle;
    import flash.geom.Vector3D;

    import starling.rendering.IndexData;
    import starling.rendering.VertexData;
    import starling.styles.MeshStyle;
    import starling.textures.Texture;
    import starling.utils.RectangleUtil;

    /** A Quad represents a colored and/or textured rectangle.
     *
     *  <p>Quads may have a color and a texture. When assigning a texture, the colors of the
     *  vertices will "tint" the texture, i.e. the vertex color will be multiplied with the color
     *  of the texture at the same position. That's why the default color of a quad is pure white:
     *  tinting with white does not change the texture color (that's a multiplication with one).</p>
     *
     *  <p>A quad is, by definition, always rectangular. The basic quad class will always contain
     *  exactly four vertices, arranged like this:</p>
     *
     *  <pre>
     *  0 - 1
     *  | / |
     *  2 - 3
     *  </pre>
     *
     *  <p>You can set the color of each vertex individually; and since the colors will smoothly
     *  fade into each other over the area of the quad, you can use this to create simple linear
     *  color gradients (e.g. by assigning one color to vertices 0 and 1 and another to vertices
     *  2 and 3).</p>
     *
     *  <p>However, note that the number of vertices may be different in subclasses.
     *  Check the property <code>numVertices</code> if you are unsure.</p>
     *
     *  @see starling.textures.Texture
     *  @see Image
     */
    public class Quad extends Mesh
    {
        private var _bounds:Rectangle;

        // helper objects
        private static var sPoint3D:Vector3D = new Vector3D();
        private static var sMatrix:Matrix = new Matrix();
        private static var sMatrix3D:Matrix3D = new Matrix3D();

        /** Creates a quad with a certain size and color. */
        public function Quad(width:Number, height:Number, color:uint=0xffffff)
        {
            _bounds = new Rectangle(0, 0, width, height);

            var vertexData:VertexData = new VertexData(MeshStyle.VERTEX_FORMAT, 4);
            var indexData:IndexData = new IndexData(6);

            super(vertexData, indexData);

            if (width == 0.0 || height == 0.0)
                throw new ArgumentError("Invalid size: width and height must not be zero");

            setupVertices();
            this.color = color;
        }

        /** Sets up vertex- and index-data according to the current settings. */
        protected function setupVertices():void
        {
            var posAttr:String = "position";
            var texAttr:String = "texCoords";
            var texture:Texture = style.texture;
            var vertexData:VertexData = this.vertexData;
            var indexData:IndexData = this.indexData;

            indexData.numIndices = 0;
            indexData.addQuad(0, 1, 2, 3);

            if (vertexData.numVertices != 4)
            {
                vertexData.numVertices = 4;
                vertexData.trim();
            }

            if (texture)
            {
                texture.setupVertexPositions(vertexData, 0, "position", _bounds);
                texture.setupTextureCoordinates(vertexData, 0, texAttr);
            }
            else
            {
                vertexData.setPoint(0, posAttr, _bounds.left,  _bounds.top);
                vertexData.setPoint(1, posAttr, _bounds.right, _bounds.top);
                vertexData.setPoint(2, posAttr, _bounds.left,  _bounds.bottom);
                vertexData.setPoint(3, posAttr, _bounds.right, _bounds.bottom);

                vertexData.setPoint(0, texAttr, 0.0, 0.0);
                vertexData.setPoint(1, texAttr, 1.0, 0.0);
                vertexData.setPoint(2, texAttr, 0.0, 1.0);
                vertexData.setPoint(3, texAttr, 1.0, 1.0);
            }

            setRequiresRedraw();
        }

        /** @inheritDoc */
        public override function getBounds(targetSpace:DisplayObject, out:Rectangle=null):Rectangle
        {
            if (out == null) out = new Rectangle();

            if (targetSpace == this) // optimization
            {
                out.copyFrom(_bounds);
            }
            else if (targetSpace == parent && !isRotated) // optimization
            {
                var scaleX:Number = this.scaleX;
                var scaleY:Number = this.scaleY;

                out.setTo(   x - pivotX * scaleX,     y - pivotY * scaleY,
                          _bounds.width * scaleX, _bounds.height * scaleY);

                if (scaleX < 0) { out.width  *= -1; out.x -= out.width;  }
                if (scaleY < 0) { out.height *= -1; out.y -= out.height; }
            }
            else if (is3D && stage)
            {
                stage.getCameraPosition(targetSpace, sPoint3D);
                getTransformationMatrix3D(targetSpace, sMatrix3D);
                RectangleUtil.getBoundsProjected(_bounds, sMatrix3D, sPoint3D, out);
            }
            else
            {
                getTransformationMatrix(targetSpace, sMatrix);
                RectangleUtil.getBounds(_bounds, sMatrix, out);
            }

            return out;
        }

        /** @inheritDoc */
        override public function hitTest(localPoint:Point):DisplayObject
        {
            if (!visible || !touchable || !hitTestMask(localPoint)) return null;
            else if (_bounds.containsPoint(localPoint)) return this;
            else return null;
        }

        /** Readjusts the dimensions of the quad. Use this method without any arguments to
         *  synchronize quad and texture size after assigning a texture with a different size.
         *  You can also force a certain width and height by passing positive, non-zero
         *  values for width and height. */
        public function readjustSize(width:Number=-1, height:Number=-1):void
        {
            if (width  <= 0) width  = texture ? texture.frameWidth  : _bounds.width;
            if (height <= 0) height = texture ? texture.frameHeight : _bounds.height;

            if (width != _bounds.width || height != _bounds.height)
            {
                _bounds.setTo(0, 0, width, height);
                setupVertices();
            }
        }

        /** Creates a quad from the given texture.
         *  The quad will have the same size as the texture. */
        public static function fromTexture(texture:Texture):Quad
        {
            var quad:Quad = new Quad(100, 100);
            quad.texture = texture;
            quad.readjustSize();
            return quad;
        }

        /** The texture that is mapped to the quad (or <code>null</code>, if there is none).
         *  Per default, it is mapped to the complete quad, i.e. to the complete area between the
         *  top left and bottom right vertices. This can be changed with the
         *  <code>setTexCoords</code>-method.
         *
         *  <p>Note that the size of the quad will not change when you assign a texture, which
         *  means that the texture might be distorted at first. Call <code>readjustSize</code> to
         *  synchronize quad and texture size.</p>
         *
         *  <p>You could also set the texture via the <code>style.texture</code> property.
         *  That way, however, the texture frame won't be taken into account. Since only rectangular
         *  objects can make use of a texture frame, only a property on the Quad class can do that.
         *  </p>
         */
        override public function set texture(value:Texture):void
        {
            if (value != texture)
            {
                super.texture = value;
                setupVertices();
            }
        }
    }
}