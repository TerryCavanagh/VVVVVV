package starling.assets
{
    import flash.system.Capabilities;
    import flash.utils.ByteArray;
    import flash.utils.getQualifiedClassName;

    import starling.errors.AbstractClassError;

    /** An AssetFactory is responsible for creating a concrete instance of an asset.
     *
     *  <p>The AssetManager contains a list of AssetFactories, registered via 'registerFactory'.
     *  When the asset queue is processed, each factory (sorted by priority) will be asked if it
     *  can handle a certain AssetReference (via the 'canHandle') method. If it can, the 'create'
     *  method will be called, which is responsible for creating at least one asset.</p>
     *
     *  <p>By extending 'AssetFactory' and registering your class at the AssetManager, you can
     *  customize how assets are being created and even add new types of assets.</p>
     */
    public class AssetFactory
    {
        private var _priority:int;
        private var _mimeTypes:Vector.<String>;
        private var _extensions:Vector.<String>;

        /** Creates a new instance. */
        public function AssetFactory()
        {
            if (Capabilities.isDebugger &&
                getQualifiedClassName(this) == "starling.assets::AssetFactory")
            {
                throw new AbstractClassError();
            }

            _mimeTypes = new <String>[];
            _extensions = new <String>[];
        }

        /** Returns 'true' if this factory can handle the given reference. The default
         *  implementation checks if extension and/or mime type of the reference match those
         *  of the factory. */
        public function canHandle(reference:AssetReference):Boolean
        {
            var mimeType:String = reference.mimeType;
            var extension:String = reference.extension;

            return reference.data is ByteArray && (
                (mimeType && _mimeTypes.indexOf(reference.mimeType.toLowerCase()) != -1) ||
                (extension && _extensions.indexOf(reference.extension.toLowerCase()) != -1));
        }

        /** This method will only be called if 'canHandle' returned 'true' for the given reference.
         *  It's responsible for creating at least one concrete asset and passing it to 'onComplete'.
         *
         *  @param reference   The asset to be created. If a local or remote URL is referenced,
         *                     it will already have been loaded, and 'data' will contain a ByteArray.
         *  @param helper      Contains useful utility methods to be used by the factory. Look
         *                     at the class documentation for more information.
         *  @param onComplete  To be called when loading is successful. 'type' parameter is optional.
         *                     <pre>function(name:String, asset:Object, type:String):void;</pre>
         *  @param onError     To be called when creation fails for some reason. Do not call
         *                     'onComplete' when that happens. <pre>function(error:String):void</pre>
         */
        public function create(reference:AssetReference, helper:AssetFactoryHelper,
                               onComplete:Function, onError:Function):void
        {
            // to be implemented by subclasses
        }

        /** Add one or more mime types that identify the supported data types. Used by
         *  'canHandle' to figure out if the factory is suitable for an asset reference. */
        public function addMimeTypes(...args):void
        {
            for each (var mimeType:String in args)
            {
                mimeType = mimeType.toLowerCase();

                if (_mimeTypes.indexOf(mimeType) == -1)
                    _mimeTypes[_mimeTypes.length] = mimeType;
            }
        }

        /** Add one or more file extensions (without leading dot) that identify the supported data
         *  types. Used by 'canHandle' to figure out if the factory is suitable for an asset
         *  reference. */
        public function addExtensions(...args):void
        {
            for each (var extension:String in args)
            {
                extension = extension.toLowerCase();

                if (_extensions.indexOf(extension) == -1)
                    _extensions[_extensions.length] = extension;
            }
        }

        /** Returns the mime types this factory supports. */
        public function getMimeTypes(out:Vector.<String>=null):Vector.<String>
        {
            out ||= new Vector.<String>();

            for (var i:int=0; i<_mimeTypes.length; ++i)
                out[i] = _mimeTypes[i];

            return out;
        }

        /** Returns the file extensions this factory supports. */
        public function getExtensions(out:Vector.<String>=null):Vector.<String>
        {
            out ||= new Vector.<String>();

            for (var i:int=0; i<_extensions.length; ++i)
                out[i] = _extensions[i];

            return out;
        }

        /** @private */
        internal function get priority():int { return _priority; }
        internal function set priority(value:int):void { _priority = value; }
    }
}
