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
    import flash.display3D.Context3DProgramType;
    import flash.utils.getQualifiedClassName;

    import starling.utils.RenderUtil;

    /** An effect drawing a mesh of textured, colored vertices.
     *  This is the standard effect that is the base for all mesh styles;
     *  if you want to create your own mesh styles, you will have to extend this class.
     *
     *  <p>For more information about the usage and creation of effects, please have a look at
     *  the documentation of the root class, "Effect".</p>
     *
     *  @see Effect
     *  @see FilterEffect
     *  @see starling.styles.MeshStyle
     */
    public class MeshEffect extends FilterEffect
    {
        /** The vertex format expected by <code>uploadVertexData</code>:
         *  <code>"position:float2, texCoords:float2, color:bytes4"</code> */
        public static const VERTEX_FORMAT:VertexDataFormat =
            FilterEffect.VERTEX_FORMAT.extend("color:bytes4");

        private var _alpha:Number;
        private var _tinted:Boolean;
        private var _optimizeIfNotTinted:Boolean;

        // helper objects
        private static var sRenderAlpha:Vector.<Number> = new Vector.<Number>(4, true);

        /** Creates a new MeshEffect instance. */
        public function MeshEffect()
        {
            // Non-tinted meshes may be rendered with a simpler fragment shader, which brings
            // a huge performance benefit on some low-end hardware. However, I don't want
            // subclasses to become any more complicated because of this optimization (they
            // probably use much longer shaders, anyway), so I only apply this optimization if
            // this is actually the "MeshEffect" class.

            _alpha = 1.0;
            _optimizeIfNotTinted = getQualifiedClassName(this) == "starling.rendering::MeshEffect";
        }

        /** @private */
        override protected function get programVariantName():uint
        {
            var noTinting:uint = uint(_optimizeIfNotTinted && !_tinted && _alpha == 1.0);
            return super.programVariantName | (noTinting << 3);
        }

        /** @private */
        override protected function createProgram():Program
        {
            var vertexShader:String, fragmentShader:String;

            if (texture)
            {
                if (_optimizeIfNotTinted && !_tinted && _alpha == 1.0)
                    return super.createProgram();

                vertexShader =
                    "m44 op, va0, vc0 \n" + // 4x4 matrix transform to output clip-space
                    "mov v0, va1      \n" + // pass texture coordinates to fragment program
                    "mul v1, va2, vc4 \n";  // multiply alpha (vc4) with color (va2), pass to fp

                fragmentShader =
                    tex("ft0", "v0", 0, texture) +
                    "mul oc, ft0, v1  \n";  // multiply color with texel color
            }
            else
            {
                vertexShader =
                    "m44 op, va0, vc0 \n" + // 4x4 matrix transform to output clipspace
                    "mul v0, va2, vc4 \n";  // multiply alpha (vc4) with color (va2)

                fragmentShader =
                    "mov oc, v0       \n";  // output color
            }

            return Program.fromSource(vertexShader, fragmentShader);
        }

        /** This method is called by <code>render</code>, directly before
         *  <code>context.drawTriangles</code>. It activates the program and sets up
         *  the context with the following constants and attributes:
         *
         *  <ul>
         *    <li><code>vc0-vc3</code> — MVP matrix</li>
         *    <li><code>vc4</code> — alpha value (same value for all components)</li>
         *    <li><code>va0</code> — vertex position (xy)</li>
         *    <li><code>va1</code> — texture coordinates (uv)</li>
         *    <li><code>va2</code> — vertex color (rgba), using premultiplied alpha</li>
         *    <li><code>fs0</code> — texture</li>
         *  </ul>
         */
        override protected function beforeDraw(context:Context3D):void
        {
            super.beforeDraw(context);

            sRenderAlpha[0] = sRenderAlpha[1] = sRenderAlpha[2] = sRenderAlpha[3] = _alpha;
            context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, 4, sRenderAlpha);

            if (_tinted || _alpha != 1.0 || !_optimizeIfNotTinted || texture == null)
                vertexFormat.setVertexBufferAt(2, vertexBuffer, "color");
        }

        /** This method is called by <code>render</code>, directly after
         *  <code>context.drawTriangles</code>. Resets texture and vertex buffer attributes. */
        override protected function afterDraw(context:Context3D):void
        {
            context.setVertexBufferAt(2, null);

            super.afterDraw(context);
        }

        /** The data format that this effect requires from the VertexData that it renders:
         *  <code>"position:float2, texCoords:float2, color:bytes4"</code> */
        override public function get vertexFormat():VertexDataFormat { return VERTEX_FORMAT; }

        /** The alpha value of the object rendered by the effect. Must be taken into account
         *  by all subclasses. */
        public function get alpha():Number { return _alpha; }
        public function set alpha(value:Number):void { _alpha = value; }

        /** Indicates if the rendered vertices are tinted in any way, i.e. if there are vertices
         *  that have a different color than fully opaque white. The base <code>MeshEffect</code>
         *  class uses this information to simplify the fragment shader if possible. May be
         *  ignored by subclasses. */
        public function get tinted():Boolean { return _tinted; }
        public function set tinted(value:Boolean):void { _tinted = value; }
    }
}
