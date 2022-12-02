package starling.assets
{
    import flash.media.Sound;
    import flash.media.Video;
    import flash.utils.ByteArray;

    import starling.errors.AbstractClassError;
    import starling.text.BitmapFont;
    import starling.textures.Texture;
    import starling.textures.TextureAtlas;

    /** An enumeration class containing all the asset types supported by the AssetManager. */
    public class AssetType
    {
        /** @private */
        public function AssetType() { throw new AbstractClassError(); }

        public static const TEXTURE:String = "texture";
        public static const TEXTURE_ATLAS:String = "textureAtlas";
        public static const SOUND:String = "sound";
        public static const XML_DOCUMENT:String = "xml";
        public static const OBJECT:String = "object";
        public static const BYTE_ARRAY:String = "byteArray";
        public static const BITMAP_FONT:String = "bitmapFont";
        public static const ASSET_MANAGER:String = "assetManager";

        /** Figures out the asset type string from the type of the given instance. */
        public static function fromAsset(asset:Object):String
        {
            if (asset is Texture) return TEXTURE;
            else if (asset is TextureAtlas) return TEXTURE_ATLAS;
            else if (asset is Sound) return SOUND;
            else if (asset is XML) return XML_DOCUMENT;
            else if (asset is ByteArray) return BYTE_ARRAY;
            else if (asset is BitmapFont) return BITMAP_FONT;
            else if (asset is AssetManager) return ASSET_MANAGER;
            else return OBJECT;
        }
    }
}
