// =================================================================================================
//
//	Starling Framework
//	Copyright Gamua GmbH. All Rights Reserved.
//
//	This program is free software. You can redistribute and/or modify it
//	in accordance with the terms of the accompanying license agreement.
//
// =================================================================================================

package starling.textures
{
    import flash.geom.Rectangle;
    import flash.utils.Dictionary;

    import starling.utils.StringUtil;

    /** A texture atlas is a collection of many smaller textures in one big image. This class
     *  is used to access textures from such an atlas.
     *  
     *  <p>Using a texture atlas for your textures solves two problems:</p>
     *  
     *  <ul>
     *    <li>Whenever you switch between textures, the batching of image objects is disrupted.</li>
     *    <li>Any Stage3D texture has to have side lengths that are powers of two. Starling hides 
     *        this limitation from you, but at the cost of additional graphics memory.</li>
     *  </ul>
     *  
     *  <p>By using a texture atlas, you avoid both texture switches and the power-of-two 
     *  limitation. All textures are within one big "super-texture", and Starling takes care that 
     *  the correct part of this texture is displayed.</p>
     *  
     *  <p>There are several ways to create a texture atlas. One is to use the atlas generator 
     *  script that is bundled with Starling's sibling, the <a href="http://www.sparrow-framework.org">
     *  Sparrow framework</a>. It was only tested in Mac OS X, though. A great multi-platform 
     *  alternative is the commercial tool <a href="http://www.texturepacker.com">
     *  Texture Packer</a>.</p>
     *  
     *  <p>Whatever tool you use, Starling expects the following file format:</p>
     * 
     *  <listing>
     * 	&lt;TextureAtlas imagePath='atlas.png'&gt;
     * 	  &lt;SubTexture name='texture_1' x='0'  y='0' width='50' height='50'/&gt;
     * 	  &lt;SubTexture name='texture_2' x='50' y='0' width='20' height='30'/&gt; 
     * 	&lt;/TextureAtlas&gt;
     *  </listing>
     *  
     *  <strong>Texture Frame</strong>
     *
     *  <p>If your images have transparent areas at their edges, you can make use of the 
     *  <code>frame</code> property of the Texture class. Trim the texture by removing the 
     *  transparent edges and specify the original texture size like this:</p>
     * 
     *  <listing>
     * 	&lt;SubTexture name='trimmed' x='0' y='0' height='10' width='10'
     * 	    frameX='-10' frameY='-10' frameWidth='30' frameHeight='30'/&gt;
     *  </listing>
     *
     *  <strong>Texture Rotation</strong>
     *
     *  <p>Some atlas generators can optionally rotate individual textures to optimize the texture
     *  distribution. This is supported via the boolean attribute "rotated". If it is set to
     *  <code>true</code> for a certain subtexture, this means that the texture on the atlas
     *  has been rotated by 90 degrees, clockwise. Starling will undo that rotation by rotating
     *  it counter-clockwise.</p>
     *
     *  <p>In this case, the positional coordinates (<code>x, y, width, height</code>)
     *  are expected to point at the subtexture as it is present on the atlas (in its rotated
     *  form), while the "frame" properties must describe the texture in its upright form.</p>
     *
     */
    public class TextureAtlas
    {
        private var _atlasTexture:Texture;
        private var _subTextures:Dictionary;
        private var _subTextureNames:Vector.<String>;
        
        /** helper objects */
        private static var sNames:Vector.<String> = new <String>[];
        
        /** Create a texture atlas from a texture by parsing the regions from an XML file. */
        public function TextureAtlas(texture:Texture, atlasXml:XML=null)
        {
            _subTextures = new Dictionary();
            _atlasTexture = texture;
            
            if (atlasXml)
                parseAtlasXml(atlasXml);
        }
        
        /** Disposes the atlas texture. */
        public function dispose():void
        {
            _atlasTexture.dispose();
        }
        
        /** This function is called by the constructor and will parse an XML in Starling's 
         *  default atlas file format. Override this method to create custom parsing logic
         *  (e.g. to support a different file format). */
        protected function parseAtlasXml(atlasXml:XML):void
        {
            var scale:Number = _atlasTexture.scale;
            var region:Rectangle = new Rectangle();
            var frame:Rectangle  = new Rectangle();
            
            for each (var subTexture:XML in atlasXml.SubTexture)
            {
                var name:String        = StringUtil.clean(subTexture.@name);
                var x:Number           = parseFloat(subTexture.@x) / scale;
                var y:Number           = parseFloat(subTexture.@y) / scale;
                var width:Number       = parseFloat(subTexture.@width)  / scale;
                var height:Number      = parseFloat(subTexture.@height) / scale;
                var frameX:Number      = parseFloat(subTexture.@frameX) / scale;
                var frameY:Number      = parseFloat(subTexture.@frameY) / scale;
                var frameWidth:Number  = parseFloat(subTexture.@frameWidth)  / scale;
                var frameHeight:Number = parseFloat(subTexture.@frameHeight) / scale;
                var rotated:Boolean    = parseBool( subTexture.@rotated);

                region.setTo(x, y, width, height);
                frame.setTo(frameX, frameY, frameWidth, frameHeight);

                if (frameWidth > 0 && frameHeight > 0)
                    addRegion(name, region, frame, rotated);
                else
                    addRegion(name, region, null,  rotated);
            }
        }
        
        /** Retrieves a SubTexture by name. Returns <code>null</code> if it is not found. */
        public function getTexture(name:String):Texture
        {
            return _subTextures[name];
        }
        
        /** Returns all textures that start with a certain string, sorted alphabetically
         *  (especially useful for "MovieClip"). */
        public function getTextures(prefix:String="", out:Vector.<Texture>=null):Vector.<Texture>
        {
            if (out == null) out = new <Texture>[];
            
            for each (var name:String in getNames(prefix, sNames)) 
                out[out.length] = getTexture(name); // avoid 'push'

            sNames.length = 0;
            return out;
        }
        
        /** Returns all texture names that start with a certain string, sorted alphabetically. */
        public function getNames(prefix:String="", out:Vector.<String>=null):Vector.<String>
        {
            var name:String;
            if (out == null) out = new <String>[];
            
            if (_subTextureNames == null)
            {
                // optimization: store sorted list of texture names
                _subTextureNames = new <String>[];
                for (name in _subTextures) _subTextureNames[_subTextureNames.length] = name;
                _subTextureNames.sort(Array.CASEINSENSITIVE);
            }

            for each (name in _subTextureNames)
                if (name.indexOf(prefix) == 0)
                    out[out.length] = name;
            
            return out;
        }
        
        /** Returns the region rectangle associated with a specific name, or <code>null</code>
         *  if no region with that name has been registered. */
        public function getRegion(name:String):Rectangle
        {
            var subTexture:SubTexture = _subTextures[name];
            return subTexture ? subTexture.region : null;
        }
        
        /** Returns the frame rectangle of a specific region, or <code>null</code> if that region 
         *  has no frame. */
        public function getFrame(name:String):Rectangle
        {
            var subTexture:SubTexture = _subTextures[name];
            return subTexture ? subTexture.frame : null;
        }
        
        /** If true, the specified region in the atlas is rotated by 90 degrees (clockwise). The
         *  SubTexture is thus rotated counter-clockwise to cancel out that transformation. */
        public function getRotation(name:String):Boolean
        {
            var subTexture:SubTexture = _subTextures[name];
            return subTexture ? subTexture.rotated : false;
        }

        /** Adds a named region for a SubTexture (described by rectangle with coordinates in
         *  points) with an optional frame. */
        public function addRegion(name:String, region:Rectangle, frame:Rectangle=null,
                                  rotated:Boolean=false):void
        {
            _subTextures[name] = new SubTexture(_atlasTexture, region, false, frame, rotated);
            _subTextureNames = null;
        }
        
        /** Removes a region with a certain name. */
        public function removeRegion(name:String):void
        {
            var subTexture:SubTexture = _subTextures[name];
            if (subTexture) subTexture.dispose();
            delete _subTextures[name];
            _subTextureNames = null;
        }
        
        /** The base texture that makes up the atlas. */
        public function get texture():Texture { return _atlasTexture; }
        
        // utility methods

        private static function parseBool(value:String):Boolean
        {
            return value.toLowerCase() == "true";
        }
    }
}