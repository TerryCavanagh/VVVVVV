package starling.assets
{
    import starling.utils.SystemUtil;

    /** A helper class that's passed to an AssetFactory's "create" method. */
    public class AssetFactoryHelper
    {
        private var _dataLoader:DataLoader;
        private var _getNameFromUrlFunc:Function;
        private var _getExtensionFromUrlFunc:Function;
        private var _addPostProcessorFunc:Function;
        private var _addAssetFunc:Function;
        private var _onRestoreFunc:Function;
        private var _logFunc:Function;

        /** @private */
        public function AssetFactoryHelper()
        { }

        /** Forwarded to the AssetManager's method with the same name. */
        public function getNameFromUrl(url:String):String
        {
            if (_getNameFromUrlFunc != null) return _getNameFromUrlFunc(url);
            else return "";
        }

        /** Forwarded to the AssetManager's method with the same name. */
        public function getExtensionFromUrl(url:String):String
        {
            if (_getExtensionFromUrlFunc != null) return _getExtensionFromUrlFunc(url);
            else return "";
        }

        /** Accesses a URL (local or remote) and passes the loaded ByteArray to the
         *  'onComplete' callback - or executes 'onError' when the data can't be loaded.
         *
         *  @param url         a string containing an URL.
         *  @param onComplete  function(data:ByteArray, mimeType:String):void;
         *  @param onError     function(error:String):void;
         */
        public function loadDataFromUrl(url:String, onComplete:Function, onError:Function):void
        {
            if (_dataLoader) _dataLoader.load(url, onComplete, onError);
        }

        /** Adds a method to be called by the AssetManager when the queue has finished processing.
         *  Useful e.g. if assets depend on other assets (like an atlas XML depending on the atlas
         *  texture).
         *
         *  @param processor  function(manager:AssetManager):void;
         *  @param priority   Processors with a higher priority will be called first.
         *                    The default processor for texture atlases is called with a
         *                    priority of '100', others with '0'.
         */
        public function addPostProcessor(processor:Function, priority:int=0):void
        {
            if (_addPostProcessorFunc != null) _addPostProcessorFunc(processor, priority);
        }

        /** Textures are required to call this method when they begin their restoration process
         *  after a context loss. */
        public function onBeginRestore():void
        {
            if (_onRestoreFunc != null) _onRestoreFunc(false);
        }

        /** Textures are required to call this method when they have finished their restoration
         *  process after a context loss. */
        public function onEndRestore():void
        {
            if (_onRestoreFunc != null) _onRestoreFunc(true);
        }

        /** Forwarded to the AssetManager's method with the same name. */
        public function log(message:String):void
        {
            if (_logFunc != null) _logFunc(message);
        }

        /** Adds additional assets to the AssetManager. To be called when the factory
         *  creates more than one asset. */
        public function addComplementaryAsset(name:String, asset:Object, type:String=null):void
        {
            if (_addAssetFunc != null) _addAssetFunc(name, asset, type);
        }

        /** Delay the execution of 'call' until it's allowed. (On mobile, the context
         *  may not be accessed while the application is in the background.)
         */
        public function executeWhenContextReady(call:Function, ...args):void
        {
            // On mobile, it is not allowed / endorsed to make stage3D calls while the app
            // is in the background. Thus, we pause execution if that's the case.

            if (SystemUtil.isDesktop) call.apply(this, args);
            else
            {
                args.unshift(call);
                SystemUtil.executeWhenApplicationIsActive.apply(this, args);
            }
        }

        /** @private */
        internal function set getNameFromUrlFunc(value:Function):void { _getNameFromUrlFunc = value; }

        /** @private */
        internal function set getExtensionFromUrlFunc(value:Function):void { _getExtensionFromUrlFunc = value; }

        /** @private */
        internal function set dataLoader(value:DataLoader):void { _dataLoader = value; }

        /** @private */
        internal function set logFunc(value:Function):void { _logFunc = value; }

        /** @private */
        internal function set addAssetFunc(value:Function):void { _addAssetFunc = value; }

        /** @private */
        internal function set onRestoreFunc(value:Function):void { _onRestoreFunc = value; }

        /** @private */
        internal function set addPostProcessorFunc(value:Function):void { _addPostProcessorFunc = value; }
    }
}
