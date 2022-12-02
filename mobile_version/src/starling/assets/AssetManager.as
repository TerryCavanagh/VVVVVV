package starling.assets
{
    import flash.media.Sound;
    import flash.media.SoundChannel;
    import flash.media.SoundTransform;
    import flash.net.URLRequest;
    import flash.system.System;
    import flash.utils.ByteArray;
    import flash.utils.Dictionary;
    import flash.utils.describeType;
    import flash.utils.getQualifiedClassName;
    import flash.utils.setTimeout;

    import starling.core.Starling;
    import starling.events.Event;
    import starling.events.EventDispatcher;
    import starling.text.BitmapFont;
    import starling.textures.Texture;
    import starling.textures.TextureAtlas;
    import starling.textures.TextureOptions;
    import starling.utils.MathUtil;
    import starling.utils.execute;

    /** Dispatched when all textures have been restored after a context loss. */
    [Event(name="texturesRestored", type="starling.events.Event")]

    /** The AssetManager handles loading and accessing a variety of asset types. You can
     *  add assets directly (via the 'add...' methods) or asynchronously via a queue. This allows
     *  you to deal with assets in a unified way, no matter if they are loaded from a file,
     *  directory, URL, or from an embedded object.
     *
     *  <p>The class can deal with the following media types:
     *  <ul>
     *    <li>Textures (either from Bitmaps or ATF data)</li>
     *    <li>Texture atlases</li>
     *    <li>Bitmap Fonts</li>
     *    <li>Sounds</li>
     *    <li>XML data</li>
     *    <li>JSON data</li>
     *    <li>ByteArrays</li>
     *    <li>other AssetManagers</li>
     *  </ul>
     *  </p>
     *
     *  <p>For more information on how to add assets from different sources, read the documentation
     *  of the "enqueue()" method.</p>
     *
     *  <strong>Context Loss</strong>
     *
     *  <p>When the stage3D context is lost, the AssetManager will automatically restore all
     *  loaded textures. To save memory, it will get them from their original sources. Since
     *  this is done asynchronously, your images might not reappear all at once, but during
     *  a time frame of several seconds. If you want, you can pause your game during that time;
     *  the AssetManager dispatches an "Event.TEXTURES_RESTORED" event when all textures have
     *  been restored.</p>
     *
     *  <strong>Error Handling</strong>
     *
     *  <p>Loading of some assets may fail while the queue is being processed. In that case, the
     *  AssetManager will call the 'onError' callback that you can optional provide to the
     *  'loadQueue' method. Queue processing will continue after an error, so it's always
     *  guaranteed that the 'onComplete' callback is executed, too.</p>
     *
     *  <strong>Texture Properties</strong>
     *
     *  <p>When you enqueue a texture, its properties for "format", "scale", "mipMapping", and
     *  "repeat" will reflect the settings of the AssetManager at the time they were enqueued.
     *  This means that you can enqueue a bunch of textures, then change the settings and enqueue
     *  some more. Like this:</p>
     *
     *  <listing>
     *  var appDir:File = File.applicationDirectory;
     *  var assets:AssetManager = new AssetManager();
     *  
     *  assets.textureOptions.format = Context3DTextureFormat.BGRA;
     *  assets.enqueue(appDir.resolvePath("textures/32bit"));
     *  
     *  assets.textureOptions.format = Context3DTextureFormat.BGRA_PACKED;
     *  assets.enqueue(appDir.resolvePath("textures/16bit"));
     *  
     *  assets.loadQueue(...);</listing>
     * 
     *  <strong>Nesting</strong>
     *
     *  <p>When you enqueue one or more AssetManagers to another one, the "loadQueue" method will
     *  load the assets of the "child" AssetManager, as well. Later, when accessing assets,
     *  the "parent" AssetManager will return the "child" assets as well - just like it returns,
     *  say, the SubTextures from a contained TextureAtlas.</p>
     *
     *  <p>The main advantage of grouping your assets like this is something else, though: it
     *  allows to remove (and dispose) a complete group of assets in one step. The example
     *  below loads the assets from two directories. When the contents of one of them are no
     *  longer needed, all its assets are removed together.</p>
     *
     *  <listing>
     *  var manager:AssetManager = new AssetManager();
     *  var appDir:File = File.applicationDirectory;
     *  
     *  var redAssets:AssetManager = new AssetManager();
     *  redAssets.enqueueSingle(appDir.resolvePath("textures/red/"));
     *  
     *  var greenAssets:AssetManager = new AssetManager();
     *  greenAssets.enqueueSingle(appDir.resolvePath("textures/green/"));
     *  
     *  manager.enqueueSingle(redAssets, "redAssets");
     *  manager.enqueueSingle(greenAssets, "greenAssets");
     *  manager.loadQueue(...); // loads both "red" and "green" assets
     *  
     *  // ... later, remove all "red" assets together
     *  manager.removeAssetManager("redAssets");</listing>
     *
     *  <strong>Customization</strong>
     *
     *  <p>You can customize how assets are created by extending the 'AssetFactory' class and
     *  registering an instance of your new class at the AssetManager via 'registerFactory'.
     *  Factories are probed by priority; any factory with a priority > 0 will be executed
     *  before the built-in factories.</p>
     *
     *  <p>An asset type is identified by a unique String. You can add your own asset types
     *  by creating a custom 'AssetFactory' and having it add the asset with custom string
     *  identifier.</p>
     *
     *  <p>By overriding the methods 'getNameFromUrl', 'getExtensionFromUrl', 'disposeAsset',
     *  and 'log', you can customize how assets are named and disposed, and you can forward
     *  any logging to an external logger. To customize the way data is loaded from URLs or
     *  files, you can assign a custom 'DataLoader' instance to the AssetManager.</p>
     *
     *  @see starling.assets.AssetFactory
     *  @see starling.assets.AssetType
     *  @see starling.assets.DataLoader
     */
    public class AssetManager extends EventDispatcher
    {
        private var _starling:Starling;
        private var _assets:Dictionary;
        private var _verbose:Boolean;
        private var _numConnections:int;
        private var _dataLoader:DataLoader;
        private var _textureOptions:TextureOptions;
        private var _queue:Vector.<AssetReference>;
        private var _registerBitmapFontsWithFontFace:Boolean;
        private var _assetFactories:Vector.<AssetFactory>;
        private var _numRestoredTextures:int;
        private var _numLostTextures:int;

        // Regex for name / extension extraction from URLs.
        private static const NAME_REGEX:RegExp = /(([^?\/\\]+?)(?:\.([\w\-]+))?)(?:\?.*)?$/;

        // fallback for unnamed assets
        private static const NO_NAME:String = "unnamed";
        private static var sNoNameCount:int = 0;

        // helper objects
        private static var sNames:Vector.<String> = new <String>[];

        /** Create a new instance with the given scale factor. */
        public function AssetManager(scaleFactor:Number=1)
        {
            _assets = new Dictionary();
            _verbose = true;
            _textureOptions = new TextureOptions(scaleFactor);
            _queue = new <AssetReference>[];
            _numConnections = 3;
            _dataLoader = new DataLoader();
            _assetFactories = new <AssetFactory>[];

            registerFactory(new BitmapTextureFactory());
            registerFactory(new AtfTextureFactory());
            registerFactory(new SoundFactory());
            registerFactory(new JsonFactory());
            registerFactory(new XmlFactory());
            registerFactory(new ByteArrayFactory(), -100);
        }

        /** Disposes all assets and purges the queue.
         *
         *  <p>Beware that all references to the assets will remain intact, even though the assets
         *  are no longer valid. Call 'purge' if you want to remove all resources and reuse
         *  the AssetManager later.</p>
         */
        public function dispose():void
        {
            purgeQueue();

            for each (var store:Dictionary in _assets)
                for each (var asset:Object in store)
                    disposeAsset(asset);
        }

        /** Removes assets of all types (disposing them along the way), empties the queue and
         *  aborts any pending load operations. */
        public function purge():void
        {
            log("Purging all assets, emptying queue");

            purgeQueue();
            dispose();

            _assets = new Dictionary();
        }

        // queue processing

        /** Enqueues one or more raw assets; they will only be available after successfully
         *  executing the "loadQueue" method. This method accepts a variety of different objects:
         *
         *  <ul>
         *    <li>Strings or URLRequests containing an URL to a local or remote resource. Supported
         *        types: <code>png, jpg, gif, atf, mp3, xml, fnt, json, binary</code>.</li>
         *    <li>Instances of the File class (AIR only) pointing to a directory or a file.
         *        Directories will be scanned recursively for all supported types.</li>
         *    <li>Classes that contain <code>static</code> embedded assets.</li>
         *    <li>If the file extension is not recognized, the data is analyzed to see if
         *        contains XML or JSON data. If it's neither, it is stored as ByteArray.</li>
         *  </ul>
         *
         *  <p>Suitable object names are extracted automatically: A file named "image.png" will be
         *  accessible under the name "image". When enqueuing embedded assets via a class,
         *  the variable name of the embedded object will be used as its name. An exception
         *  are texture atlases: they will have the same name as the actual texture they are
         *  referencing.</p>
         *
         *  <p>XMLs are made available via "getXml()"; this includes XMLs containing texture
         *  atlases or bitmap fonts, which are processed along the way. Bitmap fonts are also
         *  registered at the TextField class.</p>
         *
         *  <p>If you pass in JSON data, it will be parsed into an object and will be available via
         *  "getObject()".</p>
         */
        public function enqueue(...assets):void
        {
            for each (var asset:Object in assets)
            {
                if (asset is Array)
                {
                    enqueue.apply(this, asset);
                }
                else if (asset is Class)
                {
                    var typeXml:XML = describeType(asset);
                    var childNode:XML;

                    if (_verbose)
                        log("Looking for static embedded assets in '" +
                            (typeXml.@name).split("::").pop() + "'");

                    for each (childNode in typeXml.constant.(@type == "Class"))
                        enqueueSingle(asset[childNode.@name], childNode.@name);

                    for each (childNode in typeXml.variable.(@type == "Class"))
                        enqueueSingle(asset[childNode.@name], childNode.@name);
                }
                else if (getQualifiedClassName(asset) == "flash.filesystem::File")
                {
                    if (!asset["exists"])
                    {
                        log("File or directory not found: '" + asset["url"] + "'");
                    }
                    else if (!asset["isHidden"])
                    {
                        if (asset["isDirectory"])
                            enqueue.apply(this, asset["getDirectoryListing"]());
                        else
                            enqueueSingle(asset);
                    }
                }
                else if (asset is String || asset is URLRequest || asset is AssetManager)
                {
                    enqueueSingle(asset);
                }
                else
                {
                    log("Ignoring unsupported asset type: " + getQualifiedClassName(asset));
                }
            }
        }

        /** Enqueues a single asset with a custom name that can be used to access it later.
         *  If the asset is a texture, you can also add custom texture options.
         *
         *  @param asset    The asset that will be enqueued; accepts the same objects as the
         *                  'enqueue' method.
         *  @param name     The name under which the asset will be found later. If you pass null or
         *                  omit the parameter, it's attempted to generate a name automatically.
         *  @param options  Custom options that will be used if 'asset' points to texture data.
         *  @return         the name with which the asset was registered.
         */
        public function enqueueSingle(asset:Object, name:String=null,
                                      options:TextureOptions=null):String
        {
            if (asset is Class)
                asset = new asset();

            var assetReference:AssetReference = new AssetReference(asset);
            assetReference.name = name || getNameFromUrl(assetReference.url) || getUniqueName();
            assetReference.extension = getExtensionFromUrl(assetReference.url);
            assetReference.textureOptions = options || _textureOptions;
            var logName:String = getFilenameFromUrl(assetReference.url) || assetReference.name;

            _queue.push(assetReference);
            log("Enqueuing '" + logName + "'");
            return assetReference.name;
        }

        /** Removes the asset(s) with the given name(s) from the queue. Note that this won't work
         *  after loading has started, even if these specific assets have not yet been processed. */
        public function dequeue(...assetNames):void
        {
            _queue = _queue.filter(function(asset:AssetReference, i:int, v:*):Boolean
            {
                return assetNames.indexOf(asset.name) == -1;
            });
        }

        /** Empties the queue and aborts any pending load operations. */
        public function purgeQueue():void
        {
            _queue.length = 0;
            _dataLoader.close();
            dispatchEventWith(Event.CANCEL);
        }

        /** Loads all enqueued assets asynchronously. The 'onComplete' callback will be executed
         *  once all assets have been loaded - even when there have been errors, which are
         *  forwarded to the optional 'onError' callback. The 'onProgress' function will be called
         *  with a 'ratio' between '0.0' and '1.0' and is also optional. Furthermore, all
         *  parameters of all the callbacks are optional.
         *
         *  <p>When you call this method, the manager will save a reference to "Starling.current";
         *  all textures that are loaded will be accessible only from within this instance. Thus,
         *  if you are working with more than one Starling instance, be sure to call
         *  "makeCurrent()" on the appropriate instance before processing the queue.</p>
         *
         *  @param onComplete   <code>function(manager:AssetManager):void;</code>
         *  @param onError      <code>function(error:String, asset:AssetReference):void;</code>
         *  @param onProgress   <code>function(ratio:Number):void;</code>
         */
        public function loadQueue(onComplete:Function,
                                  onError:Function=null, onProgress:Function=null):void
        {
            if (_queue.length == 0)
            {
                finish();
                return;
            }

            // By using an event listener, we can make a call to "cancel" affect
            // only the currently active loading process(es).
            addEventListener(Event.CANCEL, onCanceled);

            var factoryHelper:AssetFactoryHelper = new AssetFactoryHelper();
            factoryHelper.getNameFromUrlFunc = getNameFromUrl;
            factoryHelper.getExtensionFromUrlFunc = getExtensionFromUrl;
            factoryHelper.addPostProcessorFunc = addPostProcessor;
            factoryHelper.addAssetFunc = addAsset;
            factoryHelper.onRestoreFunc = onAssetRestored;
            factoryHelper.dataLoader = _dataLoader;
            factoryHelper.logFunc = log;

            var i:int;
            var self:AssetManager = this;
            var canceled:Boolean = false;
            var queue:Vector.<AssetReference> = _queue.concat();
            var numAssets:int = queue.length;
            var numComplete:int = 0;
            var numConnections:int = MathUtil.min(_numConnections, numAssets);
            var assetProgress:Vector.<Number> = new Vector.<Number>(numAssets, true);
            var postProcessors:Vector.<AssetPostProcessor> = new <AssetPostProcessor>[];

            _starling = Starling.current;
            _queue.length = 0;

            for (i=0; i<numAssets; ++i)
                assetProgress[i] = -1;

            for (i=0; i<numConnections; ++i)
                loadNextAsset();

            function loadNextAsset():void
            {
                if (canceled) return;

                for (var j:int=0; j<numAssets; ++j)
                {
                    if (assetProgress[j] < 0)
                    {
                        loadFromQueue(queue, assetProgress, j, factoryHelper,
                            onAssetLoaded, onAssetProgress, onAssetLoadError, onError);
                        break;
                    }
                }
            }

            function onAssetLoaded(name:String=null, asset:Object=null, type:String=null):void
            {
                if (canceled && asset) disposeAsset(asset);
                else
                {
                    if (name && asset) addAsset(name, asset, type);
                    numComplete++;

                    if (numComplete == numAssets)
                    {
                        postProcessors.sort(comparePriorities);
                        setTimeout(runPostProcessors, 1);
                    }
                    else setTimeout(loadNextAsset, 1);
                }
            }

            function onAssetLoadError(error:String, asset:AssetReference):void
            {
                if (!canceled)
                {
                    execute(onError, error, asset);
                    onAssetLoaded();
                }
            }

            function onAssetProgress(ratio:Number):void
            {
                if (!canceled) execute(onProgress, ratio * 0.95);
            }

            function addPostProcessor(processorFunc:Function, priority:int):void
            {
                postProcessors.push(new AssetPostProcessor(processorFunc, priority));
            }

            function runPostProcessors():void
            {
                if (!canceled)
                {
                    if (postProcessors.length)
                    {
                        try { postProcessors.shift().execute(self); }
                        catch (e:Error) { execute(onError, e.message); }

                        setTimeout(runPostProcessors, 1);
                    }
                    else finish();
                }
            }

            function onCanceled():void
            {
                canceled = true;
                removeEventListener(Event.CANCEL, onCanceled);
            }

            function finish():void
            {
                onCanceled();
                execute(onProgress, 1.0);
                execute(onComplete, self);
            }
        }

        private function loadFromQueue(
            queue:Vector.<AssetReference>, progressRatios:Vector.<Number>, index:int,
            helper:AssetFactoryHelper, onComplete:Function, onProgress:Function,
            onError:Function, onIntermediateError:Function):void
        {
            var referenceCount:int = queue.length;
            var reference:AssetReference = queue[index];
            progressRatios[index] = 0;

            if (reference.url)
                _dataLoader.load(reference.url, onLoadComplete, onLoadError, onLoadProgress);
            else if (reference.data is AssetManager)
                (reference.data as AssetManager).loadQueue(onManagerComplete, onIntermediateError, onLoadProgress);
            else
                setTimeout(onLoadComplete, 1, reference.data);

            function onLoadComplete(data:Object, mimeType:String=null,
                                    name:String=null, extension:String=null):void
            {
                if (_starling) _starling.makeCurrent();

                onLoadProgress(1.0);

                if (data)      reference.data = data;
                if (name)      reference.name = name;
                if (extension) reference.extension = extension;
                if (mimeType)  reference.mimeType = mimeType;

                var assetFactory:AssetFactory = getFactoryFor(reference);
                if (assetFactory == null)
                    execute(onAnyError, "Warning: no suitable factory found for '" + reference.name + "'");
                else
                    assetFactory.create(reference, helper, onComplete, onFactoryError);
            }

            function onLoadProgress(ratio:Number):void
            {
                progressRatios[index] = ratio;

                var totalRatio:Number = 0;
                var multiplier:Number = 1.0 / referenceCount;

                for (var k:int=0; k<referenceCount; ++k)
                {
                    var r:Number = progressRatios[k];
                    if (r > 0) totalRatio += multiplier * r;
                }

                execute(onProgress, MathUtil.min(totalRatio, 1.0));
            }

            function onLoadError(error:String):void
            {
                onLoadProgress(1.0);
                execute(onAnyError, "Error loading " + reference.name + ": " + error);
            }

            function onAnyError(error:String):void
            {
                log(error);
                execute(onError, error, reference);
            }

            function onFactoryError(error:String):void
            {
                execute(onAnyError, "Error creating " + reference.name + ": " + error);
            }

            function onManagerComplete():void
            {
                onComplete(reference.name, reference.data);
            }
        }

        private function getFactoryFor(asset:AssetReference):AssetFactory
        {
            var numFactories:int = _assetFactories.length;
            for (var i:int=0; i<numFactories; ++i)
            {
                var factory:AssetFactory = _assetFactories[i];
                if (factory.canHandle(asset)) return factory;
            }

            return null;
        }

        private function onAssetRestored(finished:Boolean):void
        {
            if (finished)
            {
                _numRestoredTextures++;

                if (_starling)
                    _starling.stage.setRequiresRedraw();

                if (_numRestoredTextures == _numLostTextures)
                    dispatchEventWith(Event.TEXTURES_RESTORED);
            }
            else _numLostTextures++;
        }

        // basic accessing methods

        /** Add an asset with a certain name and type.
         *
         *  <p>Beware: if the slot (name + type) was already taken, the existing object will be
         *  disposed and replaced by the new one.</p>
         *
         *  @param name    The name with which the asset can be retrieved later. Must be
         *                 unique within this asset type.
         *  @param asset   The actual asset to add (e.g. a texture, a sound, etc).
         *  @param type    The type of the asset. If omitted, the type will be determined
         *                 automatically (which works for all standard types defined within
         *                 the 'AssetType' class).
         */
        public function addAsset(name:String, asset:Object, type:String=null):void
        {
            type ||= AssetType.fromAsset(asset);

            var store:Dictionary = _assets[type];
            if (store == null)
            {
                store = new Dictionary();
                _assets[type] = store;
            }

            log("Adding " + type + " '" + name + "'");

            var prevAsset:Object = store[name];
            if (prevAsset && prevAsset != asset)
            {
                log("Warning: name was already in use; disposing the previous " + type);
                disposeAsset(prevAsset);
            }

            store[name] = asset;
        }

        /** Retrieves an asset of the given type, with the given name. If 'recursive' is true,
         *  the method will traverse included texture atlases and asset managers.
         *
         *  <p>Typically, you will use one of the type-safe convenience methods instead, like
         *  'getTexture', 'getSound', etc.</p>
         */
        public function getAsset(type:String, name:String, recursive:Boolean=true):Object
        {
            if (recursive)
            {
                var managerStore:Dictionary = _assets[AssetType.ASSET_MANAGER];
                if (managerStore)
                {
                    for each (var manager:AssetManager in managerStore)
                    {
                        var asset:Object = manager.getAsset(type, name, true);
                        if (asset) return asset;
                    }
                }

                if (type == AssetType.TEXTURE)
                {
                    var atlasStore:Dictionary = _assets[AssetType.TEXTURE_ATLAS];
                    if (atlasStore)
                    {
                        for each (var atlas:TextureAtlas in atlasStore)
                        {
                            var texture:Texture = atlas.getTexture(name);
                            if (texture) return texture;
                        }
                    }
                }
            }

            var store:Dictionary = _assets[type];
            if (store) return store[name];
            else return null;
        }

        /** Retrieves an alphabetically sorted list of all assets that have the given type and
         *  start with the given prefix. If 'recursive' is true, the method will traverse included
         *  texture atlases and asset managers. */
        public function getAssetNames(assetType:String, prefix:String="", recursive:Boolean=true,
                                      out:Vector.<String>=null):Vector.<String>
        {
            out ||= new Vector.<String>();

            if (recursive)
            {
                var managerStore:Dictionary = _assets[AssetType.ASSET_MANAGER];
                if (managerStore)
                {
                    for each (var manager:AssetManager in managerStore)
                        manager.getAssetNames(assetType, prefix, true, out);
                }

                if (assetType == AssetType.TEXTURE)
                {
                    var atlasStore:Dictionary = _assets[AssetType.TEXTURE_ATLAS];
                    if (atlasStore)
                    {
                        for each (var atlas:TextureAtlas in atlasStore)
                            atlas.getNames(prefix, out);
                    }
                }
            }

            getDictionaryKeys(_assets[assetType], prefix, out);
            out.sort(Array.CASEINSENSITIVE);
            return out;
        }

        /** Removes the asset with the given name and type, and will optionally dispose it. */
        public function removeAsset(assetType:String, name:String, dispose:Boolean=true):void
        {
            var store:Dictionary = _assets[assetType];
            if (store)
            {
                var asset:Object = store[name];
                if (asset)
                {
                    log("Removing " + assetType + " '" + name + "'");
                    if (dispose) disposeAsset(asset);
                    delete store[name];
                }
            }
        }

        // convenience access methods

        /** Returns a texture with a certain name. Includes textures stored inside atlases. */
        public function getTexture(name:String):Texture
        {
            return getAsset(AssetType.TEXTURE, name) as Texture;
        }

        /** Returns all textures that start with a certain string, sorted alphabetically
         *  (especially useful for "MovieClip"). Includes textures stored inside atlases. */
        public function getTextures(prefix:String="", out:Vector.<Texture>=null):Vector.<Texture>
        {
            if (out == null) out = new <Texture>[];

            for each (var name:String in getTextureNames(prefix, sNames))
                out[out.length] = getTexture(name); // avoid 'push'

            sNames.length = 0;
            return out;
        }

        /** Returns all texture names that start with a certain string, sorted alphabetically.
         *  Includes textures stored inside atlases. */
        public function getTextureNames(prefix:String="", out:Vector.<String>=null):Vector.<String>
        {
            return getAssetNames(AssetType.TEXTURE, prefix, true, out);
        }

        /** Returns a texture atlas with a certain name, or null if it's not found. */
        public function getTextureAtlas(name:String):TextureAtlas
        {
            return getAsset(AssetType.TEXTURE_ATLAS, name) as TextureAtlas;
        }

        /** Returns all texture atlas names that start with a certain string, sorted alphabetically.
         *  If you pass an <code>out</code>-vector, the names will be added to that vector. */
        public function getTextureAtlasNames(prefix:String="", out:Vector.<String>=null):Vector.<String>
        {
            return getAssetNames(AssetType.TEXTURE_ATLAS, prefix, true, out);
        }

        /** Returns a sound with a certain name, or null if it's not found. */
        public function getSound(name:String):Sound
        {
            return getAsset(AssetType.SOUND, name) as Sound;
        }

        /** Returns all sound names that start with a certain string, sorted alphabetically.
         *  If you pass an <code>out</code>-vector, the names will be added to that vector. */
        public function getSoundNames(prefix:String="", out:Vector.<String>=null):Vector.<String>
        {
            return getAssetNames(AssetType.SOUND, prefix, true, out);
        }

        /** Generates a new SoundChannel object to play back the sound. This method returns a
         *  SoundChannel object, which you can access to stop the sound and to control volume. */
        public function playSound(name:String, startTime:Number=0, loops:int=0,
                                  transform:SoundTransform=null):SoundChannel
        {
            var sound:Sound = getSound(name);
            if (sound) return sound.play(startTime, loops, transform);
            else return null;
        }

        /** Returns an XML with a certain name, or null if it's not found. */
        public function getXml(name:String):XML
        {
            return getAsset(AssetType.XML_DOCUMENT, name) as XML;
        }

        /** Returns all XML names that start with a certain string, sorted alphabetically.
         *  If you pass an <code>out</code>-vector, the names will be added to that vector. */
        public function getXmlNames(prefix:String="", out:Vector.<String>=null):Vector.<String>
        {
            return getAssetNames(AssetType.XML_DOCUMENT, prefix, true, out);
        }

        /** Returns an object with a certain name, or null if it's not found. Enqueued JSON
         *  data is parsed and can be accessed with this method. */
        public function getObject(name:String):Object
        {
            return getAsset(AssetType.OBJECT, name);
        }

        /** Returns all object names that start with a certain string, sorted alphabetically.
         *  If you pass an <code>out</code>-vector, the names will be added to that vector. */
        public function getObjectNames(prefix:String="", out:Vector.<String>=null):Vector.<String>
        {
            return getAssetNames(AssetType.OBJECT, prefix, true, out);
        }

        /** Returns a byte array with a certain name, or null if it's not found. */
        public function getByteArray(name:String):ByteArray
        {
            return getAsset(AssetType.BYTE_ARRAY, name) as ByteArray;
        }

        /** Returns all byte array names that start with a certain string, sorted alphabetically.
         *  If you pass an <code>out</code>-vector, the names will be added to that vector. */
        public function getByteArrayNames(prefix:String="", out:Vector.<String>=null):Vector.<String>
        {
            return getAssetNames(AssetType.BYTE_ARRAY, prefix, true, out);
        }

        /** Returns a bitmap font with a certain name, or null if it's not found. */
        public function getBitmapFont(name:String):BitmapFont
        {
            return getAsset(AssetType.BITMAP_FONT, name) as BitmapFont;
        }

        /** Returns all bitmap font names that start with a certain string, sorted alphabetically.
         *  If you pass an <code>out</code>-vector, the names will be added to that vector. */
        public function getBitmapFontNames(prefix:String="", out:Vector.<String>=null):Vector.<String>
        {
            return getAssetNames(AssetType.BITMAP_FONT, prefix, true, out);
        }

        /** Returns an asset manager with a certain name, or null if it's not found. */
        public function getAssetManager(name:String):AssetManager
        {
            return getAsset(AssetType.ASSET_MANAGER, name) as AssetManager;
        }

        /** Returns all asset manager names that start with a certain string, sorted alphabetically.
         *  If you pass an <code>out</code>-vector, the names will be added to that vector. */
        public function getAssetManagerNames(prefix:String="", out:Vector.<String>=null):Vector.<String>
        {
            return getAssetNames(AssetType.ASSET_MANAGER, prefix, true, out);
        }

        /** Removes a certain texture, optionally disposing it. */
        public function removeTexture(name:String, dispose:Boolean=true):void
        {
            removeAsset(AssetType.TEXTURE, name, dispose);
        }

        /** Removes a certain texture atlas, optionally disposing it. */
        public function removeTextureAtlas(name:String, dispose:Boolean=true):void
        {
            removeAsset(AssetType.TEXTURE_ATLAS, name, dispose);
        }

        /** Removes a certain sound. */
        public function removeSound(name:String):void
        {
            removeAsset(AssetType.SOUND, name);
        }

        /** Removes a certain Xml object, optionally disposing it. */
        public function removeXml(name:String, dispose:Boolean=true):void
        {
            removeAsset(AssetType.XML_DOCUMENT, name, dispose);
        }

        /** Removes a certain object. */
        public function removeObject(name:String):void
        {
            removeAsset(AssetType.OBJECT, name);
        }

        /** Removes a certain byte array, optionally disposing its memory right away. */
        public function removeByteArray(name:String, dispose:Boolean=true):void
        {
            removeAsset(AssetType.BYTE_ARRAY, name, dispose);
        }

        /** Removes a certain bitmap font, optionally disposing it. */
        public function removeBitmapFont(name:String, dispose:Boolean=true):void
        {
            removeAsset(AssetType.BITMAP_FONT, name, dispose);
        }

        /** Removes a certain asset manager and optionally disposes it right away. */
        public function removeAssetManager(name:String, dispose:Boolean=true):void
        {
            removeAsset(AssetType.ASSET_MANAGER, name, dispose);
        }

        // registration of factories

        /** Registers a custom AssetFactory. If you use any priority > 0, the factory will
         *  be called before the default factories. The final factory to be invoked is the
         *  'ByteArrayFactory', which is using a priority of '-100'. */
        public function registerFactory(factory:AssetFactory, priority:int=0):void
        {
            factory.priority = priority;

            _assetFactories.push(factory);
            _assetFactories.sort(comparePriorities);
        }

        /** Unregisters the specified AssetFactory. */
        public function unregisterFactory(factory:AssetFactory):void
        {
            var index:int = _assetFactories.indexOf(factory);
            if (index != -1) _assetFactories.removeAt(index);
        }

        private static function comparePriorities(a:Object, b:Object):int
        {
            if (a.priority == b.priority) return 0;
            return a.priority > b.priority ? -1 : 1;
        }

        // helpers

        private function getFilenameFromUrl(url:String):String
        {
            if (url)
            {
                var matches:Array = NAME_REGEX.exec(decodeURIComponent(url));
                if (matches && matches.length > 1) return matches[1];
            }
            return null;
        }

        /** This method is called internally to determine the name under which an asset will be
         *  accessible; override it if you need a custom naming scheme.
         *
         *  @return the name to be used for the asset, or 'null' if it can't be determined. */
        protected function getNameFromUrl(url:String):String
        {
            if (url)
            {
                var matches:Array = NAME_REGEX.exec(decodeURIComponent(url));
                if (matches && matches.length > 2) return matches[2];
            }
            return null;
        }

        /** This method is called internally to determine the extension that's passed to the
         *  'AssetFactory' (via the 'AssetReference'). Override it if you need to customize
         *  e.g. the extension of a server URL.
         *
         *  @return the extension to be used for the asset, or an empty string if it can't be
         *          determined. */
        protected function getExtensionFromUrl(url:String):String
        {
            if (url)
            {
                var matches:Array = NAME_REGEX.exec(decodeURIComponent(url));
                if (matches && matches.length > 3) return matches[3];
            }
            return "";
        }

        /** Disposes the given asset. ByteArrays are cleared, XMLs are disposed using
         *  'System.disposeXML'. If the object contains a 'dispose' method, it will be called.
         *  Override if you need to add custom cleanup code for a certain asset. */
        protected function disposeAsset(asset:Object):void
        {
            if (asset is ByteArray) (asset as ByteArray).clear();
            if (asset is XML) System.disposeXML(asset as XML);
            if ("dispose" in asset) asset["dispose"]();
        }

        /** This method is called during loading of assets when 'verbose' is activated. Per
         *  default, it traces 'message' to the console. */
        protected function log(message:String):void
        {
            if (_verbose) trace("[AssetManager]", message);
        }

        private static function getDictionaryKeys(dictionary:Dictionary, prefix:String="",
                                                  out:Vector.<String>=null):Vector.<String>
        {
            if (out == null) out = new <String>[];
            if (dictionary)
            {
                for (var name:String in dictionary)
                    if (name.indexOf(prefix) == 0)
                        out[out.length] = name; // avoid 'push'

                out.sort(Array.CASEINSENSITIVE);
            }
            return out;
        }

        private static function getUniqueName():String
        {
            return NO_NAME + "-" + sNoNameCount++;
        }

        // properties

        /** When activated, the class will trace information about added/enqueued assets.
         *  @default true */
        public function get verbose():Boolean { return _verbose; }
        public function set verbose(value:Boolean):void { _verbose = value; }

        /** Returns the number of raw assets that have been enqueued, but not yet loaded. */
        public function get numQueuedAssets():int { return _queue.length; }

        /** The maximum number of parallel connections that are spawned when loading the queue.
         *  More connections can reduce loading times, but require more memory. @default 3. */
        public function get numConnections():int { return _numConnections; }
        public function set numConnections(value:int):void
        {
            _numConnections = MathUtil.min(1, value);
        }

        /** Textures will be created with the options set up in this object at the time of
         *  enqueuing. */
        public function get textureOptions():TextureOptions { return _textureOptions; }
        public function set textureOptions(value:TextureOptions):void { _textureOptions.copyFrom(value); }

        /** The DataLoader is used to load any data from files or URLs. If you need to customize
         *  its behavior (e.g. to add a caching mechanism), assign your custom instance here. */
        public function get dataLoader():DataLoader { return _dataLoader; }
        public function set dataLoader(value:DataLoader):void { _dataLoader = value; }

        /** Indicates if bitmap fonts should be registered with their "face" attribute from the
         *  font XML file. Per default, they are registered with the name of the texture file.
         *  @default false */
        public function get registerBitmapFontsWithFontFace():Boolean
        {
            return _registerBitmapFontsWithFontFace;
        }

        public function set registerBitmapFontsWithFontFace(value:Boolean):void
        {
            _registerBitmapFontsWithFontFace = value;
        }
    }
}

import starling.assets.AssetManager;

class AssetPostProcessor
{
    private var _priority:int;
    private var _callback:Function;

    public function AssetPostProcessor(callback:Function, priority:int)
    {
        if (callback == null || callback.length != 1)
            throw new ArgumentError("callback must be a function " +
                "accepting one 'AssetStore' parameter");

        _callback = callback;
        _priority = priority;
    }

    internal function execute(store:AssetManager):void
    {
        _callback(store);
    }

    public function get priority():int { return _priority; }
}