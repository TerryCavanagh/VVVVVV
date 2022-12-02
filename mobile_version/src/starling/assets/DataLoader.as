package starling.assets
{
    import flash.events.ErrorEvent;
    import flash.events.Event;
    import flash.events.HTTPStatusEvent;
    import flash.events.IOErrorEvent;
    import flash.events.ProgressEvent;
    import flash.events.SecurityErrorEvent;
    import flash.net.URLLoader;
    import flash.net.URLLoaderDataFormat;
    import flash.net.URLRequest;
    import flash.utils.Dictionary;

    import starling.utils.execute;

    /** Loads binary data from a local or remote URL with a very simple callback system.
     *
     *  <p>The DataLoader is used by the AssetManager to load any local or remote data.
     *  Its single purpose is to get the binary data that's stored at a specific URL.</p>
     *
     *  <p>You can use this class for your own purposes (as an easier to use 'URLLoader'
     *  alternative), or you can extend the class to modify the AssetManager's behavior.
     *  E.g. you could extend this class to add a caching mechanism for remote assets.
     *  Assign an instance of your extended class to the AssetManager's <code>dataLoader</code>
     *  property.</p>
     */
    public class DataLoader
    {
        // This HTTPStatusEvent is only available in AIR
        private static const HTTP_RESPONSE_STATUS:String = "httpResponseStatus";

        private var _urlLoaders:Dictionary;

        /** Creates a new DataLoader instance. */
        public function DataLoader()
        {
            _urlLoaders = new Dictionary(true);
        }

        /** Loads the binary data from a specific URL. Always supply both 'onComplete' and
         *  'onError' parameters; in case of an error, only the latter will be called.
         *
         *  <p>The 'onComplete' callback may have up to four parameters, all of them being optional.
         *  If you pass a callback that just takes zero or one, it will work just as well. The
         *  additional parameters may be used to describe the name and type of the downloaded
         *  data. They are not provided by the base class, but the AssetManager will check
         *  if they are available.</p>
         *
         * @param url          a String containing an URL.
         * @param onComplete   will be called when the data has been loaded.
         * <code>function(data:ByteArray, mimeType:String, name:String, extension:String):void</code>
         * @param onError      will be called in case of an error. The 2nd parameter is optional.
         *                     <code>function(error:String, httpStatus:int):void</code>
         * @param onProgress   will be called multiple times with the current load ratio (0-1).
         *                     <code>function(ratio:Number):void</code>
         */
        public function load(url:String, onComplete:Function,
                             onError:Function, onProgress:Function=null):void
        {
            var message:String;
            var mimeType:String = null;
            var httpStatus:int = 0;
            var request:URLRequest = new URLRequest(url);
            var loader:URLLoader = new URLLoader();
            loader.dataFormat = URLLoaderDataFormat.BINARY;
            loader.addEventListener(IOErrorEvent.IO_ERROR, onLoadError);
            loader.addEventListener(SecurityErrorEvent.SECURITY_ERROR, onLoadError);
            loader.addEventListener(HTTP_RESPONSE_STATUS, onHttpResponseStatus);
            loader.addEventListener(ProgressEvent.PROGRESS, onLoadProgress);
            loader.addEventListener(Event.COMPLETE, onLoadComplete);
            loader.load(request);

            _urlLoaders[loader] = true;

            function onHttpResponseStatus(event:HTTPStatusEvent):void
            {
                httpStatus = event.status;
                mimeType = getHttpHeader(event["responseHeaders"], "Content-Type");
            }

            function onLoadError(event:ErrorEvent):void
            {
                cleanup();
                message = event.type + " - " + event.text;
                execute(onError, message);
            }

            function onLoadProgress(event:ProgressEvent):void
            {
                if (onProgress != null && event.bytesTotal > 0)
                    onProgress(event.bytesLoaded / event.bytesTotal);
            }

            function onLoadComplete(event:Object):void
            {
                cleanup();

                if (httpStatus < 400)
                    execute(onComplete, loader.data, mimeType);
                else
                    execute(onError, "Unexpected HTTP status '" + httpStatus + "'. URL: " +
                        request.url, httpStatus);
            }

            function cleanup():void
            {
                loader.removeEventListener(IOErrorEvent.IO_ERROR, onLoadError);
                loader.removeEventListener(SecurityErrorEvent.SECURITY_ERROR, onLoadError);
                loader.removeEventListener(HTTP_RESPONSE_STATUS, onHttpResponseStatus);
                loader.removeEventListener(ProgressEvent.PROGRESS, onLoadProgress);
                loader.removeEventListener(Event.COMPLETE, onLoadComplete);
                delete _urlLoaders[loader];
            }
        }

        /** Aborts all current load operations. The loader can still be used, though. */
        public function close():void
        {
            for (var loader:* in _urlLoaders)
            {
                try { loader.close(); }
                catch (e:Error) {}
            }

            _urlLoaders = new Dictionary(true);
        }

        private static function getHttpHeader(headers:Array, headerName:String):String
        {
            if (headers)
            {
                for each (var header:Object in headers)
                    if (header.name == headerName) return header.value;
            }
            return null;
        }
    }
}
