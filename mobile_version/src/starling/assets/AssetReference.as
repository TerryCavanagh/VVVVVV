package starling.assets
{
    import starling.textures.TextureOptions;

    /** The description of an asset to be created by an AssetFactory. */
    public class AssetReference
    {
        private var _name:String;
        private var _url:String;
        private var _data:Object;
        private var _mimeType:String;
        private var _extension:String;
        private var _textureOptions:TextureOptions;

        /** Creates a new instance with the given data, which is typically some kind of file
         *  reference / URL or an instance of an asset class. If 'data' contains an URL, an
         *  equivalent value will be assigned to the 'url' property. */
        public function AssetReference(data:Object)
        {
            _data = data;
            _textureOptions = new TextureOptions();

            if (data is String) _url = data as String;
            else if ("url" in data) _url = data["url"] as String;
        }

        /** The name with which the asset should be added to the AssetManager. */
        public function get name():String { return _name; }
        public function set name(value:String):void { _name = value; }

        /** The url from which the asset needs to be / has been loaded. */
        public function get url():String { return _url; }
        public function set url(value:String):void { _url = value; }

        /** The raw data of the asset. This property often contains an URL; when it's passed
         *  to an AssetFactory, loading has already completed, and the property contains a
         *  ByteArray with the loaded data. */
        public function get data():Object { return _data; }
        public function set data(value:Object):void { _data = value; }

        /** The mime type of the asset, if loaded from a server. */
        public function get mimeType():String { return _mimeType; }
        public function set mimeType(value:String):void { _mimeType = value; }

        /** The file extension of the asset, if the filename or URL contains one. */
        public function get extension():String { return _extension; }
        public function set extension(value:String):void { _extension = value; }

        /** The TextureOptions describing how to create a texture, if the asset references one. */
        public function get textureOptions():TextureOptions { return _textureOptions; }
        public function set textureOptions(value:TextureOptions):void
        {
            _textureOptions.copyFrom(value);
        }
    }
}
