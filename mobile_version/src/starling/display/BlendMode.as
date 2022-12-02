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
    import flash.display3D.Context3DBlendFactor;

    import starling.core.Starling;

    /** A class that provides constant values for visual blend mode effects.
     *   
     *  <p>A blend mode is always defined by two 'Context3DBlendFactor' values. A blend factor 
     *  represents a particular four-value vector that is multiplied with the source or destination
     *  color in the blending formula. The blending formula is:</p>
     * 
     *  <pre>result = source × sourceFactor + destination × destinationFactor</pre>
     * 
     *  <p>In the formula, the source color is the output color of the pixel shader program. The 
     *  destination color is the color that currently exists in the color buffer, as set by 
     *  previous clear and draw operations.</p>
     *  
     *  <p>You can add your own blend modes via <code>BlendMode.register</code>.
     *  To get the math right, remember that all colors in Starling use premultiplied alpha (PMA),
     *  which means that their RGB values were multiplied with the alpha value.</p>
     *
     *  @see flash.display3D.Context3DBlendFactor
     */
    public class BlendMode
    {
        private var _name:String;
        private var _sourceFactor:String;
        private var _destinationFactor:String;

        private static var sBlendModes:Object;

        /** Creates a new BlendMode instance. Don't call this method directly; instead,
         *  register a new blend mode using <code>BlendMode.register</code>. */
        public function BlendMode(name:String, sourceFactor:String, destinationFactor:String)
        {
            _name = name;
            _sourceFactor = sourceFactor;
            _destinationFactor = destinationFactor;
        }
        
        /** Inherits the blend mode from this display object's parent. */
        public static const AUTO:String = "auto";

        /** Deactivates blending, i.e. disabling any transparency. */
        public static const NONE:String = "none";
        
        /** The display object appears in front of the background. */
        public static const NORMAL:String = "normal";
        
        /** Adds the values of the colors of the display object to the colors of its background. */
        public static const ADD:String = "add";
        
        /** Multiplies the values of the display object colors with the the background color. */
        public static const MULTIPLY:String = "multiply";
        
        /** Multiplies the complement (inverse) of the display object color with the complement of 
          * the background color, resulting in a bleaching effect. */
        public static const SCREEN:String = "screen";
        
        /** Erases the background when drawn on a RenderTexture. */
        public static const ERASE:String = "erase";

        /** When used on a RenderTexture, the drawn object will act as a mask for the current
         *  content, i.e. the source alpha overwrites the destination alpha. */
        public static const MASK:String = "mask";

        /** Draws under/below existing objects; useful especially on RenderTextures. */
        public static const BELOW:String = "below";

        // static access methods
        
        /** Returns the blend mode with the given name.
         *  Throws an ArgumentError if the mode does not exist. */
        public static function get(modeName:String):BlendMode
        {
            if (sBlendModes == null) registerDefaults();
            if (modeName in sBlendModes) return sBlendModes[modeName];
            else throw new ArgumentError("Blend mode not found: " + modeName);
        }
        
        /** Registers a blending mode under a certain name. */
        public static function register(name:String, srcFactor:String, dstFactor:String):BlendMode
        {
            if (sBlendModes == null) registerDefaults();
            var blendMode:BlendMode = new BlendMode(name, srcFactor, dstFactor);
            sBlendModes[name] = blendMode;
            return blendMode;
        }
        
        private static function registerDefaults():void
        {
            if (sBlendModes) return;

            sBlendModes = {};
            register("none" , Context3DBlendFactor.ONE, Context3DBlendFactor.ZERO);
            register("normal", Context3DBlendFactor.ONE, Context3DBlendFactor.ONE_MINUS_SOURCE_ALPHA);
            register("add", Context3DBlendFactor.ONE, Context3DBlendFactor.ONE);
            register("multiply", Context3DBlendFactor.DESTINATION_COLOR, Context3DBlendFactor.ONE_MINUS_SOURCE_ALPHA);
            register("screen", Context3DBlendFactor.ONE, Context3DBlendFactor.ONE_MINUS_SOURCE_COLOR);
            register("erase", Context3DBlendFactor.ZERO, Context3DBlendFactor.ONE_MINUS_SOURCE_ALPHA);
            register("mask", Context3DBlendFactor.ZERO, Context3DBlendFactor.SOURCE_ALPHA);
            register("below", Context3DBlendFactor.ONE_MINUS_DESTINATION_ALPHA, Context3DBlendFactor.DESTINATION_ALPHA);
        }

        // instance methods / properties

        /** Sets the appropriate blend factors for source and destination on the current context. */
        public function activate():void
        {
            Starling.context.setBlendFactors(_sourceFactor, _destinationFactor);
        }

        /** Returns the name of the blend mode. */
        public function toString():String { return _name; }

        /** The source blend factor of this blend mode. */
        public function get sourceFactor():String { return _sourceFactor; }

        /** The destination blend factor of this blend mode. */
        public function get destinationFactor():String { return _destinationFactor; }

        /** Returns the name of the blend mode. */
        public function get name():String { return _name; }
    }
}