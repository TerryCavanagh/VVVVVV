package starling.assets
{
    import flash.utils.ByteArray;

    import starling.text.BitmapFont;
    import starling.text.TextField;
    import starling.textures.Texture;
    import starling.textures.TextureAtlas;
    import starling.utils.ByteArrayUtil;

    /** This AssetFactory creates XML assets, texture atlases and bitmap fonts. */
    public class XmlFactory extends AssetFactory
    {
        /** Creates a new instance. */
        public function XmlFactory()
        {
            addMimeTypes("application/xml", "text/xml");
            addExtensions("xml", "fnt");
        }

        /** Returns true if mime type or extension fit for XML data, or if the data starts
         *  with a "&lt;" character. */
        override public function canHandle(reference:AssetReference):Boolean
        {
            return super.canHandle(reference) || (reference.data is ByteArray &&
                ByteArrayUtil.startsWithString(reference.data as ByteArray, "<"));
        }

        /** Creates the XML asset and passes it to 'onComplete'. If the XML contains a
         *  TextureAtlas or a BitmapFont, adds suitable post processors. */
        override public function create(reference:AssetReference, helper:AssetFactoryHelper,
                                        onComplete:Function, onError:Function):void
        {
            var xml:XML = reference.data as XML;
            var bytes:ByteArray = reference.data as ByteArray;

            if (bytes)
            {
                try { xml = new XML(bytes); }
                catch (e:Error)
                {
                    onError("Could not parse XML: " + e.message);
                    return;
                }
            }

            var rootNode:String = xml.localName();

            if (rootNode == "TextureAtlas")
                helper.addPostProcessor(textureAtlasPostProcessor, 100);
            else if (rootNode == "font")
                helper.addPostProcessor(bitmapFontPostProcessor);

            onComplete(reference.name, xml);

            // prevent closures from keeping references
            reference.data = bytes = null;

            function textureAtlasPostProcessor(store:AssetManager):void
            {
                var name:String = helper.getNameFromUrl(xml.@imagePath.toString());
                var texture:Texture = store.getTexture(name);
                if (texture) store.addAsset(name, new TextureAtlas(texture, xml));
                else helper.log("Cannot create atlas: texture '" + name + "' is missing.");
            }

            function bitmapFontPostProcessor(store:AssetManager):void
            {
                var textureName:String = helper.getNameFromUrl(xml.pages.page.@file.toString());
                var texture:Texture = store.getTexture(textureName);
                var fontName:String = store.registerBitmapFontsWithFontFace ?
                    xml.info.@face.toString() : textureName;

                if (texture)
                {
                    var bitmapFont:BitmapFont = new BitmapFont(texture, xml);
                    store.addAsset(fontName, bitmapFont);
                    TextField.registerCompositor(bitmapFont, fontName);
                }
                else helper.log("Cannot create bitmap font: texture '" + textureName + "' is missing.");
            }
        }
    }
}
