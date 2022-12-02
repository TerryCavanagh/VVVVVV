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
    import flash.geom.Matrix;

    import starling.display.Mesh;
    import starling.rendering.MeshEffect;
    import starling.rendering.RenderState;
    import starling.rendering.VertexData;
    import starling.rendering.VertexDataFormat;
    import starling.utils.Color;
    import starling.utils.MathUtil;

    /** Provides support for signed distance fields to Starling meshes.
     *
     *  <p>Signed distance field rendering allows bitmap fonts and other single colored shapes to
     *  be drawn without jagged edges, even at high magnifications. The technique was introduced in
     *  the SIGGRAPH paper <a href="http://tinyurl.com/AlphaTestedMagnification">Improved
     *  Alpha-Tested Magnification for Vector Textures and Special Effects</a> by Valve Software.
     *  </p>
     *
     *  <p>While bitmap fonts are a great solution to render text in a GPU-friendly way, they
     *  don't scale well. For best results, one has to embed the font in all the sizes used within
     *  the app. The distance field style solves this issue: instead of providing a standard
     *  black and white image of the font, it uses a <em>signed distance field texture</em> as
     *  its input (a texture that encodes, for each pixel, the distance to the closest edge of a
     *  vector shape). With this data, the shape can be rendered smoothly at almost any scale.</p>
     *
     *  <p>Here are some tools that support creation of such distance field textures:</p>
     *
     *  <ul>
     *    <li><a href="http://kvazars.com/littera/">Littera</a> - a free online bitmap font
     *        generator.</li>
     *    <li><a href="http://github.com/libgdx/libgdx/wiki/Hiero">Hiero</a> - a cross platform
     *        tool.</li>
     *    <li><a href="http://www.angelcode.com/products/bmfont/">BMFont</a> - Windows-only, from
     *        AngelCode.</li>
     *  </ul>
     *
     *  <strong>Special effects</strong>
     *
     *  <p>Another advantage of this rendering technique: it supports very efficient rendering of
     *  some popular filter effects, in just one pass, directly on the GPU. You can add an
     *  <em>outline</em> around the shape, let it <em>glow</em> in an arbitrary color, or add
     *  a <em>drop shadow</em>.</p>
     *
     *  <p>The type of effect currently used is called the 'mode'.
     *  Meshes with the same mode will be batched together on rendering.</p>
     */
    public class DistanceFieldStyle extends MeshStyle
    {
        /** The vertex format expected by this style. */
        public static const VERTEX_FORMAT:VertexDataFormat =
            MeshStyle.VERTEX_FORMAT.extend(
                "basic:bytes4, extended:bytes4, outerColor:bytes4");

        /** Basic distance field rendering, without additional effects. */
        public static const MODE_BASIC:String = "basic";

        /** Adds an outline around the edge of the shape. */
        public static const MODE_OUTLINE:String = "outline";

        /** Adds a smooth glow effect around the shape. */
        public static const MODE_GLOW:String = "glow";

        /** Adds a drop shadow behind the shape. */
        public static const MODE_SHADOW:String = "shadow";

        private var _mode:String;

        // basic
        private var _threshold:Number;
        private var _alpha:Number;
        private var _softness:Number;

        // extended
        private var _outerThreshold:Number;
        private var _outerAlphaEnd:Number;
        private var _shadowOffsetX:Number;
        private var _shadowOffsetY:Number;
        
        // outerColor
        private var _outerColor:uint;
        private var _outerAlphaStart:Number;

        /** Creates a new distance field style.
         *
         *  @param softness   adds a soft transition between the inside and the outside.
         *                    This should typically be 1.0 divided by the spread used when
         *                    creating the distance field texture.
         *  @param threshold  the value separating the inside from the outside of the shape.
         *                    Range: 0 - 1.
         */
        public function DistanceFieldStyle(softness:Number=0.125, threshold:Number=0.5)
        {
            _mode = MODE_BASIC;
            _threshold = threshold;
            _softness = softness;
            _alpha = 1.0;

            _outerThreshold = _outerAlphaEnd = 0.0;
            _shadowOffsetX = _shadowOffsetY = 0.0;
            
            _outerColor = 0x0;
            _outerAlphaStart = 0.0;
        }

        /** @private */
        override public function copyFrom(meshStyle:MeshStyle):void
        {
            var otherStyle:DistanceFieldStyle = meshStyle as DistanceFieldStyle;
            if (otherStyle)
            {
                _mode = otherStyle._mode;
                _threshold = otherStyle._threshold;
                _softness = otherStyle._softness;
                _alpha = otherStyle._alpha;

                _outerThreshold = otherStyle._outerThreshold;
                _outerAlphaEnd  = otherStyle._outerAlphaEnd;
                _shadowOffsetX  = otherStyle._shadowOffsetX;
                _shadowOffsetY  = otherStyle._shadowOffsetY;

                _outerColor = otherStyle._outerColor;
                _outerAlphaStart = otherStyle._outerAlphaStart;
            }

            super.copyFrom(meshStyle);
        }

        /** @private */
        override public function createEffect():MeshEffect
        {
            return new DistanceFieldEffect();
        }

        /** @private */
        override public function get vertexFormat():VertexDataFormat
        {
            return VERTEX_FORMAT;
        }

        /** @private */
        override protected function onTargetAssigned(target:Mesh):void
        {
            updateVertices();
        }

        private function updateVertices():void
        {
            if (vertexData == null) return;

            // To save space, all settings are stored in 'bytes4' format; this means we write
            // values in the range 0-255 into the bytes and receive floats in the range 0-1 in the
            // shaders. Since the 'scale' and 'outerOffset' values require a different range,
            // they are encoded with a scale factor and/or offset. The color is stored manually
            // (not via 'setColor') to avoid PMA processing.

            var numVertices:int = vertexData.numVertices;
            var maxScale:int = DistanceFieldEffect.MAX_SCALE;
            var maxOuterOffset:int = DistanceFieldEffect.MAX_OUTER_OFFSET;
            var encodedOuterOffsetX:Number = (_shadowOffsetX + maxOuterOffset) / (2 * maxOuterOffset);
            var encodedOuterOffsetY:Number = (_shadowOffsetY + maxOuterOffset) / (2 * maxOuterOffset);

            var basic:uint = (uint(_threshold      * 255)      ) |
                             (uint(_alpha          * 255) <<  8) |
                             (uint(_softness / 2.0 * 255) << 16) |
                             (uint(1.0 / maxScale  * 255) << 24);
            var extended:uint = (uint(_outerThreshold     * 255)      ) |
                                (uint(_outerAlphaEnd      * 255) <<  8) |
                                (uint(encodedOuterOffsetX * 255) << 16) |
                                (uint(encodedOuterOffsetY * 255) << 24);
            var outerColor:uint = (Color.getRed(_outerColor)         ) |
                                  (Color.getGreen(_outerColor)  <<  8) |
                                  (Color.getBlue(_outerColor)   << 16) |
                                  (uint(_outerAlphaStart * 255) << 24);

            for (var i:int=0; i<numVertices; ++i)
            {
                vertexData.setUnsignedInt(i, "basic", basic);
                vertexData.setUnsignedInt(i, "extended", extended);
                vertexData.setUnsignedInt(i, "outerColor", outerColor);
            }

            setVertexDataChanged();
        }

        /** @private */
        override public function batchVertexData(targetStyle:MeshStyle, targetVertexID:int = 0,
                                                 matrix:Matrix = null, vertexID:int = 0,
                                                 numVertices:int = -1):void
        {
            super.batchVertexData(targetStyle, targetVertexID, matrix, vertexID, numVertices);

            if (matrix)
            {
                var scale:Number = Math.sqrt(matrix.a * matrix.a + matrix.c * matrix.c);

                if (!MathUtil.isEquivalent(scale, 1.0, 0.01))
                {
                    var targetVertexData:VertexData = (targetStyle as DistanceFieldStyle).vertexData;
                    var maxScale:Number = DistanceFieldEffect.MAX_SCALE;
                    var minScale:Number = maxScale / 255;

                    for (var i:int=0; i<numVertices; ++i)
                    {
                        var srcAttr:uint = vertexData.getUnsignedInt(vertexID + i, "basic");
                        var srcScale:Number = ((srcAttr >> 24) & 0xff) / 255.0 * maxScale;
                        var tgtScale:Number = MathUtil.clamp(srcScale * scale, minScale, maxScale);
                        var tgtAttr:uint =
                            (srcAttr & 0x00ffffff) | (uint(tgtScale / maxScale * 255) << 24);

                        targetVertexData.setUnsignedInt(targetVertexID + i, "basic", tgtAttr);
                    }
                }
            }
        }

        /** @private */
        override public function updateEffect(effect:MeshEffect, state:RenderState):void
        {
            var dfEffect:DistanceFieldEffect = effect as DistanceFieldEffect;
            dfEffect.mode = _mode;

            if (state.is3D) dfEffect.scale = 1.0;
            else
            {
                // The softness is adapted automatically with the total scale of the object.
                // However, this only works for 2D objects.

                var matrix:Matrix = state.modelviewMatrix;
                var scale:Number = Math.sqrt(matrix.a * matrix.a + matrix.c * matrix.c);
                dfEffect.scale = scale;
            }

            super.updateEffect(effect, state);
        }

        /** @private */
        override public function canBatchWith(meshStyle:MeshStyle):Boolean
        {
            var dfStyle:DistanceFieldStyle = meshStyle as DistanceFieldStyle;
            if (dfStyle && super.canBatchWith(meshStyle)) return dfStyle.mode == _mode;
            else return false;
        }

        // simplified setup

        /** Restores basic render mode, i.e. smooth rendering of the shape. */
        public function setupBasic():void
        {
            _mode = MODE_BASIC;

            setRequiresRedraw();
        }

        /** Sets up outline rendering mode. The 'width' determines the threshold where the
         *  outline ends; 'width + threshold' must not exceed '1.0'.
         */
        public function setupOutline(width:Number=0.25, color:uint=0x0, alpha:Number=1.0):void
        {
            _mode = MODE_OUTLINE;
            _outerThreshold = MathUtil.clamp(_threshold - width, 0, _threshold);
            _outerColor = color;
            _outerAlphaStart = _outerAlphaEnd = MathUtil.clamp(alpha, 0, 1);
            _shadowOffsetX = _shadowOffsetY = 0.0;

            updateVertices();
        }

        /** Sets up glow rendering mode. The 'blur' determines the threshold where the
         *  blur ends; 'blur + threshold' must not exceed '1.0'.
         */
        public function setupGlow(blur:Number=0.2, color:uint=0xffff00, alpha:Number=0.5):void
        {
            _mode = MODE_GLOW;
            _outerThreshold = MathUtil.clamp(_threshold - blur, 0, _threshold);
            _outerColor = color;
            _outerAlphaStart = MathUtil.clamp(alpha, 0, 1);
            _outerAlphaEnd = 0.0;
            _shadowOffsetX = _shadowOffsetY = 0.0;

            updateVertices();
        }

        /** Sets up shadow rendering mode. The 'blur' determines the threshold where the drop
         *  shadow ends; 'offsetX' and 'offsetY' are expected in points.
         *
         *  <p>Beware that the style can only act within the limits of the mesh's vertices.
         *  This means that not all combinations of blur and offset are possible; too high values
         *  will cause the shadow to be cut off on the sides. Reduce either blur or offset to
         *  compensate.</p>
         */
        public function setupDropShadow(blur:Number=0.2, offsetX:Number=2, offsetY:Number=2,
                                        color:uint=0x0, alpha:Number=0.5):void
        {
            const maxOffset:Number = DistanceFieldEffect.MAX_OUTER_OFFSET;

            _mode = MODE_SHADOW;
            _outerThreshold = MathUtil.clamp(_threshold - blur, 0, _threshold);
            _outerColor = color;
            _outerAlphaStart = MathUtil.clamp(alpha, 0, 1);
            _outerAlphaEnd = 0.0;
            _shadowOffsetX = MathUtil.clamp(offsetX, -maxOffset, maxOffset);
            _shadowOffsetY = MathUtil.clamp(offsetY, -maxOffset, maxOffset);

            updateVertices();
        }

        // properties

        /** The current render mode. It's recommended to use one of the 'setup...'-methods to
         *  change the mode, as those provide useful standard settings, as well. @default basic */
        public function get mode():String { return _mode; }
        public function set mode(value:String):void
        {
            _mode = value;
            setRequiresRedraw();
        }

        /** The threshold that will separate the inside from the outside of the shape. On the
         *  distance field texture, '0' means completely outside, '1' completely inside; the
         *  actual edge runs along '0.5'. @default 0.5 */
        public function get threshold():Number { return _threshold; }
        public function set threshold(value:Number):void
        {
            value = MathUtil.clamp(value, 0, 1);

            if (_threshold != value)
            {
                _threshold = value;
                updateVertices();
            }
        }

        /** Indicates how soft the transition between inside and outside should be rendered.
         *  A value of '0' will lead to a hard, jagged edge; '1' will be just as blurry as the
         *  actual distance field texture. The recommend value should be <code>1.0 / spread</code>
         *  (you determine the spread when creating the distance field texture). @default 0.125 */
        public function get softness():Number { return _softness; }
        public function set softness(value:Number):void
        {
            value = MathUtil.clamp(value, 0, 1);

            if (_softness != value)
            {
                _softness = value;
                updateVertices();
            }
        }

        /** The alpha value with which the inner area (what's rendered in 'basic' mode) is drawn.
         *  @default 1.0 */
        public function get alpha():Number { return _alpha; }
        public function set alpha(value:Number):void
        {
            value = MathUtil.clamp(value, 0, 1);

            if (_alpha != value)
            {
                _alpha = value;
                updateVertices();
            }
        }

        /** The threshold that determines where the outer area (outline, glow, or drop shadow)
         *  ends. Ignored in 'basic' mode. */
        public function get outerThreshold():Number { return _outerThreshold; }
        public function set outerThreshold(value:Number):void
        {
            value = MathUtil.clamp(value, 0, 1);

            if (_outerThreshold != value)
            {
                _outerThreshold = value;
                updateVertices();
            }
        }

        /** The alpha value on the inner side of the outer area's gradient.
         *  Used for outline, glow, and drop shadow modes. */
        public function get outerAlphaStart():Number { return _outerAlphaStart; }
        public function set outerAlphaStart(value:Number):void
        {
            value = MathUtil.clamp(value, 0, 1);

            if (_outerAlphaStart != value)
            {
                _outerAlphaStart = value;
                updateVertices();
            }
        }

        /** The alpha value on the outer side of the outer area's gradient.
         *  Used for outline, glow, and drop shadow modes. */
        public function get outerAlphaEnd():Number { return _outerAlphaEnd; }
        public function set outerAlphaEnd(value:Number):void
        {
            value = MathUtil.clamp(value, 0, 1);

            if (_outerAlphaEnd != value)
            {
                _outerAlphaEnd = value;
                updateVertices();
            }
        }

        /** The color with which the outer area (outline, glow, or drop shadow) will be filled.
         *  Ignored in 'basic' mode. */
        public function get outerColor():uint { return _outerColor; }
        public function set outerColor(value:uint):void
        {
            if (_outerColor != value)
            {
                _outerColor = value;
                updateVertices();
            }
        }

        /** The x-offset of the shadow in points. Note that certain combinations of offset and
         *  blur value can lead the shadow to be cut off at the edges. Reduce blur or offset to
         *  counteract. */
        public function get shadowOffsetX():Number { return _shadowOffsetX; }
        public function set shadowOffsetX(value:Number):void
        {
            const max:Number = DistanceFieldEffect.MAX_OUTER_OFFSET;
            value = MathUtil.clamp(value, -max, max);

            if (_shadowOffsetX != value)
            {
                _shadowOffsetX = value;
                updateVertices();
            }
        }

        /** The y-offset of the shadow in points. Note that certain combinations of offset and
         *  blur value can lead the shadow to be cut off at the edges. Reduce blur or offset to
         *  counteract. */
        public function get shadowOffsetY():Number { return _shadowOffsetY; }
        public function set shadowOffsetY(value:Number):void
        {
            const max:Number = DistanceFieldEffect.MAX_OUTER_OFFSET;
            value = MathUtil.clamp(value, -max, max);

            if (_shadowOffsetY != value)
            {
                _shadowOffsetY = value;
                updateVertices();
            }
        }
    }
}

import flash.display3D.Context3D;
import flash.display3D.Context3DProgramType;

import starling.rendering.MeshEffect;
import starling.rendering.Program;
import starling.rendering.VertexDataFormat;
import starling.styles.DistanceFieldStyle;
import starling.utils.StringUtil;

class DistanceFieldEffect extends MeshEffect
{
    public static const VERTEX_FORMAT:VertexDataFormat = DistanceFieldStyle.VERTEX_FORMAT;
    public static const MAX_OUTER_OFFSET:int = 8;
    public static const MAX_SCALE:int = 8;

    private var _mode:String;
    private var _scale:Number;

    private static const sVector:Vector.<Number> = new Vector.<Number>(4, true);

    public function DistanceFieldEffect()
    {
        _scale = 1.0;
        _mode = DistanceFieldStyle.MODE_BASIC;
    }

    override protected function createProgram():Program
    {
        if (texture)
        {
            // va0 - position
            // va1 - tex coords
            // va2 - color
            // va3 - basic settings (threshold, alpha, softness, local scale [encoded])
            // va4 - outer settings (outerThreshold, outerAlphaEnd, outerOffsetX/Y)
            // va5 - outer color (rgb, outerAlphaStart)
            // vc5 - shadow offset multiplier (x, y), max local scale (z), global scale (w)

            var isBasicMode:Boolean  = _mode == DistanceFieldStyle.MODE_BASIC;
            var isShadowMode:Boolean = _mode == DistanceFieldStyle.MODE_SHADOW;

            /// *** VERTEX SHADER ***

            var vertexShader:Vector.<String> = new <String>[
                "m44 op, va0, vc0", // 4x4 matrix transform to output clip-space
                "mov v0, va1     ", // pass texture coordinates to fragment program
                "mul v1, va2, vc4", // multiply alpha (vc4) with color (va2), pass to fp
                "mov v3, va3     ",
                "mov v4, va4     ",
                "mov v5, va5     ",

                // update softness to take current scale into account
                "mul vt0.x, va3.w, vc5.z", // vt0.x = local scale [decoded]
                "mul vt0.x, vt0.x, vc5.w", // vt0.x *= global scale
                "div vt0.x, va3.z, vt0.x", // vt0.x = softness / total scale

                // calculate min-max of threshold
                "mov vt1, vc4",             // initialize vt1 with something (anything)
                "sub vt1.x, va3.x, vt0.x",  // vt1.x = thresholdMin
                "add vt1.y, va3.x, vt0.x"   // vt1.y = thresholdMax
            ];

            if (!isBasicMode)
            {
                vertexShader.push(
                    // calculate min-max of outer threshold
                    "sub vt1.z, va4.x, vt0.x",     // vt1.z = outerThresholdMin
                    "add vt1.w, va4.x, vt0.x"      // vt1.w = outerThresholdMax
                );
            }

            vertexShader.push("sat v6, vt1"); // v6.xyzw = thresholdMin/Max, outerThresholdMin/Max

            if (isShadowMode)
            {
                vertexShader.push(
                    // calculate shadow offset
                    "mul vt0.xy, va4.zw, vc6.zz", // vt0.x/y = outerOffsetX/Y * 2
                    "sub vt0.xy, vt0.xy, vc6.yy", // vt0.x/y -= 1   -> range -1, 1
                    "mul vt0.xy, vt0.xy, vc5.xy", // vt0.x/y = outerOffsetX/Y in point size
                    "sub v7, va1, vt0.xyxy",      // v7.xy = shadow tex coords

                    // on shadows, the inner threshold is further inside than on glow & outline
                    "sub vt0.z, va3.x, va4.x",    // get delta between threshold and outer threshold
                    "add v7.z, va3.x, vt0.z"      // v7.z = inner threshold of shadow
                );
            }

            /// *** FRAGMENT SHADER ***

            var fragmentShader:Vector.<String> = new <String>[
                // create basic inner area
                tex("ft0", "v0", 0, texture),     // ft0 = texture color
                "mov ft1, ft0",                   // ft1 = texture color
                step("ft1.w", "v6.x", "v6.y"),    // make soft inner mask
                "mov ft3, ft1",                   // store copy of inner mask in ft3 (for outline)
                "mul ft1, v1, ft1.wwww"           // multiply with color
            ];

            if (isShadowMode)
            {
                fragmentShader.push(
                    tex("ft0", "v7", 0, texture), // sample at shadow tex coords
                    "mov ft5.x, v7.z"             // ft5.x = inner threshold of shadow
                );
            }
            else if (!isBasicMode)
            {
                fragmentShader.push(
                    "mov ft5.x, v6.x"             // ft5.x = inner threshold of outer area
                );
            }

            if (!isBasicMode)
            {
                fragmentShader.push(
                    // outer area
                    "mov ft2, ft0",                 // ft2 = texture color
                    step("ft2.w", "v6.z", "v6.w"),  // make soft outer mask
                    "sub ft2.w, ft2.w, ft3.w",      // subtract inner area
                    "sat ft2.w, ft2.w",             // but stay within 0-1

                    // add alpha gradient to outer area
                    "mov ft4, ft0",                 // ft4 = texture color
                    step("ft4.w", "v6.z", "ft5.x"), // make soft mask ranging between thresholds
                    "sub ft6.w, v5.w, v4.y",        // ft6.w  = alpha range (outerAlphaStart - End)
                    "mul ft4.w, ft4.w, ft6.w",      // ft4.w *= alpha range
                    "add ft4.w, ft4.w, v4.y",       // ft4.w += alpha end

                    // colorize outer area
                    "mul ft2.w, ft2.w, ft4.w",      // get final outline alpha at this position
                    "mul ft2.xyz, v5.xyz, ft2.www"  // multiply with outerColor
                );
            }

            if (isBasicMode) fragmentShader.push("mov oc, ft1");
            else             fragmentShader.push("add oc, ft1, ft2");

            return Program.fromSource(vertexShader.join("\n"), fragmentShader.join("\n"));
        }
        else return super.createProgram();
    }

    private static function step(inOutReg:String, minReg:String, maxReg:String,
                                 tmpReg:String="ft6"):String
    {
        var ops:Vector.<String> = new <String>[
            StringUtil.format("sub {0}, {1}, {2}", tmpReg, maxReg, minReg), // tmpReg = range
            StringUtil.format("rcp {0}, {0}", tmpReg),                      // tmpReg = scale
            StringUtil.format("sub {0}, {0}, {1}", inOutReg, minReg),       // inOut -= minimum
            StringUtil.format("mul {0}, {0}, {1}", inOutReg, tmpReg),       // inOut *= scale
            StringUtil.format("sat {0}, {0}", inOutReg)                     // clamp to 0-1
        ];

        return ops.join("\n");
    }

    override protected function beforeDraw(context:Context3D):void
    {
        super.beforeDraw(context);

        if (texture)
        {
            vertexFormat.setVertexBufferAt(3, vertexBuffer, "basic");
            vertexFormat.setVertexBufferAt(4, vertexBuffer, "extended");
            vertexFormat.setVertexBufferAt(5, vertexBuffer, "outerColor");

            var pixelWidth:Number  = 1.0 / (texture.root.nativeWidth  / texture.scale);
            var pixelHeight:Number = 1.0 / (texture.root.nativeHeight / texture.scale);

            sVector[0] = MAX_OUTER_OFFSET * pixelWidth;
            sVector[1] = MAX_OUTER_OFFSET * pixelHeight;
            sVector[2] = MAX_SCALE;
            sVector[3] = _scale;

            context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, 5, sVector);

            sVector[0] = 0.0;
            sVector[1] = 1.0;
            sVector[2] = 2.0;

            context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, 6, sVector);
        }
    }

    override protected function afterDraw(context:Context3D):void
    {
        if (texture)
        {
            context.setVertexBufferAt(3, null);
            context.setVertexBufferAt(4, null);
            context.setVertexBufferAt(5, null);
        }
        super.afterDraw(context);
    }

    override public function get vertexFormat():VertexDataFormat
    {
        return VERTEX_FORMAT;
    }

    override protected function get programVariantName():uint
    {
        var modeBits:uint;

        switch (_mode)
        {
            case DistanceFieldStyle.MODE_SHADOW:  modeBits = 3; break;
            case DistanceFieldStyle.MODE_GLOW:    modeBits = 2; break;
            case DistanceFieldStyle.MODE_OUTLINE: modeBits = 1; break;
            default:                              modeBits = 0;
        }

        return super.programVariantName | (modeBits << 8);
    }

    public function get scale():Number { return _scale; }
    public function set scale(value:Number):void { _scale = value; }

    public function get mode():String { return _mode; }
    public function set mode(value:String):void { _mode = value; }
}
