// =================================================================================================
//
//	Starling Framework
//	Copyright Gamua GmbH. All Rights Reserved.
//
//	This program is free software. You can redistribute and/or modify it
//	in accordance with the terms of the accompanying license agreement.
//
// =================================================================================================

package starling.rendering
{
    import flash.display3D.Context3D;

    import starling.textures.Texture;
    import starling.textures.TextureSmoothing;
    import starling.utils.RenderUtil;

    /** An effect drawing a mesh of textured vertices.
     *  This is the standard effect that is the base for all fragment filters;
     *  if you want to create your own fragment filters, you will have to extend this class.
     *
     *  <p>For more information about the usage and creation of effects, please have a look at
     *  the documentation of the parent class, "Effect".</p>
     *
     *  @see Effect
     *  @see MeshEffect
     *  @see starling.filters.FragmentFilter
     */
    public class FilterEffect extends Effect
    {
        /** The vertex format expected by <code>uploadVertexData</code>:
         *  <code>"position:float2, texCoords:float2"</code> */
        public static const VERTEX_FORMAT:VertexDataFormat =
            Effect.VERTEX_FORMAT.extend("texCoords:float2");

        /** The AGAL code for the standard vertex shader that most filters will use.
         *  It simply transforms the vertex coordinates to clip-space and passes the texture
         *  coordinates to the fragment program (as 'v0'). */
        public static const STD_VERTEX_SHADER:String =
            "m44 op, va0, vc0 \n"+  // 4x4 matrix transform to output clip-space
            "mov v0, va1";          // pass texture coordinates to fragment program

        private var _texture:Texture;
        private var _textureSmoothing:String;
        private var _textureRepeat:Boolean;

        /** Creates a new FilterEffect instance. */
        public function FilterEffect()
        {
            _textureSmoothing = TextureSmoothing.BILINEAR;
        }

        /** Override this method if the effect requires a different program depending on the
         *  current settings. Ideally, you do this by creating a bit mask encoding all the options.
         *  This method is called often, so do not allocate any temporary objects when overriding.
         *
         *  <p>Reserve 4 bits for the variant name of the base class.</p>
         */
        override protected function get programVariantName():uint
        {
            return RenderUtil.getTextureVariantBits(_texture);
        }

        /** @private */
        override protected function createProgram():Program
        {
            if (_texture)
            {
                var vertexShader:String = STD_VERTEX_SHADER;
                var fragmentShader:String = tex("oc", "v0", 0, _texture);
                return Program.fromSource(vertexShader, fragmentShader);
            }
            else
            {
                return super.createProgram();
            }
        }

        /** This method is called by <code>render</code>, directly before
         *  <code>context.drawTriangles</code>. It activates the program and sets up
         *  the context with the following constants and attributes:
         *
         *  <ul>
         *    <li><code>vc0-vc3</code> — MVP matrix</li>
         *    <li><code>va0</code> — vertex position (xy)</li>
         *    <li><code>va1</code> — texture coordinates (uv)</li>
         *    <li><code>fs0</code> — texture</li>
         *  </ul>
         */
        override protected function beforeDraw(context:Context3D):void
        {
            super.beforeDraw(context);

            if (_texture)
            {
                var repeat:Boolean = _textureRepeat && _texture.root.isPotTexture;
                RenderUtil.setSamplerStateAt(0, _texture.mipMapping, _textureSmoothing, repeat);
                context.setTextureAt(0, _texture.base);
                vertexFormat.setVertexBufferAt(1, vertexBuffer, "texCoords");
            }
        }

        /** This method is called by <code>render</code>, directly after
         *  <code>context.drawTriangles</code>. Resets texture and vertex buffer attributes. */
        override protected function afterDraw(context:Context3D):void
        {
            if (_texture)
            {
                context.setTextureAt(0, null);
                context.setVertexBufferAt(1, null);
            }

            super.afterDraw(context);
        }

        /** Creates an AGAL source string with a <code>tex</code> operation, including an options
         *  list with the appropriate format flag. This is just a convenience method forwarding
         *  to the respective RenderUtil method.
         *
         *  @see starling.utils.RenderUtil#createAGALTexOperation()
         */
        protected static function tex(resultReg:String, uvReg:String, sampler:int, texture:Texture,
                                      convertToPmaIfRequired:Boolean=true):String
        {
            return RenderUtil.createAGALTexOperation(resultReg, uvReg, sampler, texture,
                convertToPmaIfRequired);
        }

        /** The data format that this effect requires from the VertexData that it renders:
         *  <code>"position:float2, texCoords:float2"</code> */
        override public function get vertexFormat():VertexDataFormat { return VERTEX_FORMAT; }

        /** The texture to be mapped onto the vertices. */
        public function get texture():Texture { return _texture; }
        public function set texture(value:Texture):void { _texture = value; }

        /** The smoothing filter that is used for the texture. @default bilinear */
        public function get textureSmoothing():String { return _textureSmoothing; }
        public function set textureSmoothing(value:String):void { _textureSmoothing = value; }

        /** Indicates if pixels at the edges will be repeated or clamped.
         *  Only works for power-of-two textures. @default false */
        public function get textureRepeat():Boolean { return _textureRepeat; }
        public function set textureRepeat(value:Boolean):void { _textureRepeat = value; }
    }
}
