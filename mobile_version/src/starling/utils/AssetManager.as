// =================================================================================================
//
//	Starling Framework
//	Copyright Gamua GmbH. All Rights Reserved.
//
//	This program is free software. You can redistribute and/or modify it
//	in accordance with the terms of the accompanying license agreement.
//
// =================================================================================================

package starling.utils
{
    import flash.display.Bitmap;
    import flash.display.Loader;
    import flash.display.LoaderInfo;
    import flash.events.HTTPStatusEvent;
    import flash.events.IOErrorEvent;
    import flash.events.ProgressEvent;
    import flash.events.SecurityErrorEvent;
    import flash.media.Sound;
    import flash.media.SoundChannel;
    import flash.media.SoundTransform;
    import flash.net.FileReference;
    import flash.net.URLLoader;
    import flash.net.URLLoaderDataFormat;
    import flash.net.URLRequest;
    import flash.system.ImageDecodingPolicy;
    import flash.system.LoaderContext;
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
    import starling.text.TextField;
    import starling.textures.AtfData;
    import starling.textures.Texture;
    import starling.textures.TextureAtlas;
    import starling.textures.TextureOptions;

    /** Dispatched when all textures have been restored after a context loss. */
    [Event(name="texturesRestored", type="starling.events.Event")]
    
    /** Dispatched when an URLLoader fails with an IO_ERROR while processing the queue.
     *  The 'data' property of the Event contains the URL-String that could not be loaded. */
    [Event(name="ioError", type="starling.events.Event")]

    /** Dispatched when an URLLoader fails with a SECURITY_ERROR while processing the queue.
     *  The 'data' property of the Event contains the URL-String that could not be loaded. */
    [Event(name="securityError", type="starling.events.Event")]

    /** Dispatched when an XML or JSON file couldn't be parsed.
     *  The 'data' property of the Event contains the name of the asset that could not be parsed. */
    [Event(name="parseError", type="starling.events.Event")]

    /** The AssetManager handles loading and accessing a variety of asset types. You can 
     *  add assets directly (via the 'add...' methods) or asynchronously via a queue. This allows
     *  you to deal with assets in a unified way, no matter if they are loaded from a file, 
     *  directory, URL, or from an embedded object.
     *  
     *  <p>The class can deal with the following media types:
     *  <ul>
     *    <li>Textures, either from Bitmaps or ATF data</li>
     *    <li>Texture atlases</li>
     *    <li>Bitmap Fonts</li>
     *    <li>Sounds</li>
     *    <li>XML data</li>
     *    <li>JSON data</li>
     *    <li>ByteArrays</li>
     *  </ul>
     *  </p>
     *  
     *  <p>For more information on how to add assets from different sources, read the documentation
     *  of the "enqueue()" method.</p>
     * 
     *  <strong>Context Loss</strong>
     *  
     *  <p>When the stage3D context is lost (and you have enabled 'Starling.handleLostContext'),
     *  the AssetManager will automatically restore all loaded textures. To save memory, it will
     *  get them from their original sources. Since this is done asynchronously, your images might
     *  not reappear all at once, but during a timeframe of several seconds. If you want, you can
     *  pause your game during that time; the AssetManager dispatches an "Event.TEXTURES_RESTORED"
     *  event when all textures have been restored.</p>
     *
     *  <strong>Error handling</strong>
     *
     *  <p>Loading of some assets may fail while the queue is being processed. In that case, the
     *  AssetManager will dispatch events of type "IO_ERROR", "SECURITY_ERROR" or "PARSE_ERROR".
     *  You can listen to those events and handle the errors manually (e.g., you could enqueue
     *  them once again and retry, or provide placeholder textures). Queue processing will
     *  continue even when those events are dispatched.</p>
     *
     *  <strong>Using variable texture formats</strong>
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
     *  assets.textureFormat = Context3DTextureFormat.BGRA;
     *  assets.enqueue(appDir.resolvePath("textures/32bit"));
     *  
     *  assets.textureFormat = Context3DTextureFormat.BGRA_PACKED;
     *  assets.enqueue(appDir.resolvePath("textures/16bit"));
     *  
     *  assets.loadQueue(...);</listing>
     */
    public class AssetManager extends EventDispatcher
    {
        // This HTTPStatusEvent is only available in AIR
        private static const HTTP_RESPONSE_STATUS:String = "httpResponseStatus";

        private var _starling:Starling;
        private var _numLostTextures:int;
        private var _numRestoredTextures:int;
        private var _numLoadingQueues:int;

        private var _defaultTextureOptions:TextureOptions;
        private var _checkPolicyFile:Boolean;
        private var _keepAtlasXmls:Boolean;
        private var _keepFontXmls:Boolean;
        private var _numConnections:int;
        private var _verbose:Boolean;
        private var _queue:Array;
        
        private var _textures:Dictionary;
        private var _atlases:Dictionary;
        private var _sounds:Dictionary;
        private var _xmls:Dictionary;
        private var _objects:Dictionary;
        private var _byteArrays:Dictionary;
        
        /** helper objects */
        private static var sNames:Vector.<String> = new <String>[];
        
        /** Regex for name / extension extraction from URL. */
        private static const NAME_REGEX:RegExp = /([^\?\/\\]+?)(?:\.([\w\-]+))?(?:\?.*)?$/;

        /** Create a new AssetManager. The 'scaleFactor' and 'useMipmaps' parameters define
         *  how enqueued bitmaps will be converted to textures. */
        public function AssetManager(scaleFactor:Number=1, useMipmaps:Boolean=false)
        {
            _defaultTextureOptions = new TextureOptions(scaleFactor, useMipmaps);
            _textures = new Dictionary();
            _atlases = new Dictionary();
            _sounds = new Dictionary();
            _xmls = new Dictionary();
            _objects = new Dictionary();
            _byteArrays = new Dictionary();
            _numConnections = 3;
            _verbose = true;
            _queue = [];
        }
        
        /** Disposes all contained textures, XMLs and ByteArrays.
         *
         *  <p>Beware that all references to the assets will remain intact, even though the assets
         *  are no longer valid. Call 'purge' if you want to remove all resources and reuse
         *  the AssetManager later.</p>
         */
        public function dispose():void
        {
            for each (var texture:Texture in _textures)
                texture.dispose();
            
            for each (var atlas:TextureAtlas in _atlases)
                atlas.dispose();
            
            for each (var xml:XML in _xmls)
                System.disposeXML(xml);
            
            for each (var byteArray:ByteArray in _byteArrays)
                byteArray.clear();
        }
        
        // retrieving
        
        /** Returns a texture with a certain name. The method first looks through the directly
         *  added textures; if no texture with that name is found, it scans through all 
         *  texture atlases. */
        public function getTexture(name:String):Texture
        {
            if (name in _textures) return _textures[name];
            else
            {
                for each (var atlas:TextureAtlas in _atlases)
                {
                    var texture:Texture = atlas.getTexture(name);
                    if (texture) return texture;
                }
                return null;
            }
        }
        
        /** Returns all textures that start with a certain string, sorted alphabetically
         *  (especially useful for "MovieClip"). */
        public function getTextures(prefix:String="", out:Vector.<Texture>=null):Vector.<Texture>
        {
            if (out == null) out = new <Texture>[];
            
            for each (var name:String in getTextureNames(prefix, sNames))
                out[out.length] = getTexture(name); // avoid 'push'

            sNames.length = 0;
            return out;
        }
        
        /** Returns all texture names that start with a certain string, sorted alphabetically. */
        public function getTextureNames(prefix:String="", out:Vector.<String>=null):Vector.<String>
        {
            out = getDictionaryKeys(_textures, prefix, out);
            
            for each (var atlas:TextureAtlas in _atlases)
                atlas.getNames(prefix, out);
            
            out.sort(Array.CASEINSENSITIVE);
            return out;
        }
        
        /** Returns a texture atlas with a certain name, or null if it's not found. */
        public function getTextureAtlas(name:String):TextureAtlas
        {
            return _atlases[name] as TextureAtlas;
        }

        /** Returns all texture atlas names that start with a certain string, sorted alphabetically.
         *  If you pass an <code>out</code>-vector, the names will be added to that vector. */
        public function getTextureAtlasNames(prefix:String="", out:Vector.<String>=null):Vector.<String>
        {
            return getDictionaryKeys(_atlases, prefix, out);
        }

        /** Returns a sound with a certain name, or null if it's not found. */
        public function getSound(name:String):Sound
        {
            return _sounds[name];
        }
        
        /** Returns all sound names that start with a certain string, sorted alphabetically.
         *  If you pass an <code>out</code>-vector, the names will be added to that vector. */
        public function getSoundNames(prefix:String="", out:Vector.<String>=null):Vector.<String>
        {
            return getDictionaryKeys(_sounds, prefix, out);
        }
        
        /** Generates a new SoundChannel object to play back the sound. This method returns a 
         *  SoundChannel object, which you can access to stop the sound and to control volume. */ 
        public function playSound(name:String, startTime:Number=0, loops:int=0, 
                                  transform:SoundTransform=null):SoundChannel
        {
            if (name in _sounds)
                return getSound(name).play(startTime, loops, transform);
            else 
                return null;
        }
        
        /** Returns an XML with a certain name, or null if it's not found. */
        public function getXml(name:String):XML
        {
            return _xmls[name];
        }
        
        /** Returns all XML names that start with a certain string, sorted alphabetically. 
         *  If you pass an <code>out</code>-vector, the names will be added to that vector. */
        public function getXmlNames(prefix:String="", out:Vector.<String>=null):Vector.<String>
        {
            return getDictionaryKeys(_xmls, prefix, out);
        }

        /** Returns an object with a certain name, or null if it's not found. Enqueued JSON
         *  data is parsed and can be accessed with this method. */
        public function getObject(name:String):Object
        {
            return _objects[name];
        }
        
        /** Returns all object names that start with a certain string, sorted alphabetically. 
         *  If you pass an <code>out</code>-vector, the names will be added to that vector. */
        public function getObjectNames(prefix:String="", out:Vector.<String>=null):Vector.<String>
        {
            return getDictionaryKeys(_objects, prefix, out);
        }
        
        /** Returns a byte array with a certain name, or null if it's not found. */
        public function getByteArray(name:String):ByteArray
        {
            return _byteArrays[name];
        }
        
        /** Returns all byte array names that start with a certain string, sorted alphabetically. 
         *  If you pass an <code>out</code>-vector, the names will be added to that vector. */
        public function getByteArrayNames(prefix:String="", out:Vector.<String>=null):Vector.<String>
        {
            return getDictionaryKeys(_byteArrays, prefix, out);
        }
        
        // direct adding
        
        /** Register a texture under a certain name. It will be available right away.
         *  If the name was already taken, the existing texture will be disposed and replaced
         *  by the new one. */
        public function addTexture(name:String, texture:Texture):void
        {
            log("Adding texture '" + name + "'");
            
            if (name in _textures)
            {
                log("Warning: name was already in use; the previous texture will be replaced.");
                _textures[name].dispose();
            }
            
            _textures[name] = texture;
        }
        
        /** Register a texture atlas under a certain name. It will be available right away. 
         *  If the name was already taken, the existing atlas will be disposed and replaced
         *  by the new one. */
        public function addTextureAtlas(name:String, atlas:TextureAtlas):void
        {
            log("Adding texture atlas '" + name + "'");
            
            if (name in _atlases)
            {
                log("Warning: name was already in use; the previous atlas will be replaced.");
                _atlases[name].dispose();
            }
            
            _atlases[name] = atlas;
        }
        
        /** Register a sound under a certain name. It will be available right away.
         *  If the name was already taken, the existing sound will be replaced by the new one. */
        public function addSound(name:String, sound:Sound):void
        {
            log("Adding sound '" + name + "'");
            
            if (name in _sounds)
                log("Warning: name was already in use; the previous sound will be replaced.");

            _sounds[name] = sound;
        }
        
        /** Register an XML object under a certain name. It will be available right away.
         *  If the name was already taken, the existing XML will be disposed and replaced
         *  by the new one. */
        public function addXml(name:String, xml:XML):void
        {
            log("Adding XML '" + name + "'");
            
            if (name in _xmls)
            {
                log("Warning: name was already in use; the previous XML will be replaced.");
                System.disposeXML(_xmls[name]);
            }

            _xmls[name] = xml;
        }
        
        /** Register an arbitrary object under a certain name. It will be available right away. 
         *  If the name was already taken, the existing object will be replaced by the new one. */
        public function addObject(name:String, object:Object):void
        {
            log("Adding object '" + name + "'");
            
            if (name in _objects)
                log("Warning: name was already in use; the previous object will be replaced.");
            
            _objects[name] = object;
        }
        
        /** Register a byte array under a certain name. It will be available right away.
         *  If the name was already taken, the existing byte array will be cleared and replaced
         *  by the new one. */
        public function addByteArray(name:String, byteArray:ByteArray):void
        {
            log("Adding byte array '" + name + "'");
            
            if (name in _byteArrays)
            {
                log("Warning: name was already in use; the previous byte array will be replaced.");
                _byteArrays[name].clear();
            }
            
            _byteArrays[name] = byteArray;
        }
        
        // removing
        
        /** Removes a certain texture, optionally disposing it. */
        public function removeTexture(name:String, dispose:Boolean=true):void
        {
            log("Removing texture '" + name + "'");
            
            if (dispose && name in _textures)
                _textures[name].dispose();
            
            delete _textures[name];
        }
        
        /** Removes a certain texture atlas, optionally disposing it. */
        public function removeTextureAtlas(name:String, dispose:Boolean=true):void
        {
            log("Removing texture atlas '" + name + "'");
            
            if (dispose && name in _atlases)
                _atlases[name].dispose();
            
            delete _atlases[name];
        }
        
        /** Removes a certain sound. */
        public function removeSound(name:String):void
        {
            log("Removing sound '"+ name + "'");
            delete _sounds[name];
        }
        
        /** Removes a certain Xml object, optionally disposing it. */
        public function removeXml(name:String, dispose:Boolean=true):void
        {
            log("Removing xml '"+ name + "'");
            
            if (dispose && name in _xmls)
                System.disposeXML(_xmls[name]);
            
            delete _xmls[name];
        }
        
        /** Removes a certain object. */
        public function removeObject(name:String):void
        {
            log("Removing object '"+ name + "'");
            delete _objects[name];
        }
        
        /** Removes a certain byte array, optionally disposing its memory right away. */
        public function removeByteArray(name:String, dispose:Boolean=true):void
        {
            log("Removing byte array '"+ name + "'");
            
            if (dispose && name in _byteArrays)
                _byteArrays[name].clear();
            
            delete _byteArrays[name];
        }
        
        /** Empties the queue and aborts any pending load operations. */
        public function purgeQueue():void
        {
            _queue.length = 0;
            dispatchEventWith(Event.CANCEL);
        }
        
        /** Removes assets of all types (disposing them along the way), empties the queue and
         *  aborts any pending load operations. */
        public function purge():void
        {
            log("Purging all assets, emptying queue");
            
            purgeQueue();
            dispose();

            _textures = new Dictionary();
            _atlases = new Dictionary();
            _sounds = new Dictionary();
            _xmls = new Dictionary();
            _objects = new Dictionary();
            _byteArrays = new Dictionary();
        }
        
        // queued adding
        
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
         *  <p>XMLs that contain texture atlases or bitmap fonts are processed directly: fonts are
         *  registered at the TextField class, atlas textures can be acquired with the
         *  "getTexture()" method. All other XMLs are available via "getXml()".</p>
         *  
         *  <p>If you pass in JSON data, it will be parsed into an object and will be available via
         *  "getObject()".</p>
         */
        public function enqueue(...rawAssets):void
        {
            for each (var rawAsset:Object in rawAssets)
            {
                if (rawAsset is Array)
                {
                    enqueue.apply(this, rawAsset);
                }
                else if (rawAsset is Class)
                {
                    var typeXml:XML = describeType(rawAsset);
                    var childNode:XML;
                    
                    if (_verbose)
                        log("Looking for static embedded assets in '" + 
                            (typeXml.@name).split("::").pop() + "'"); 
                    
                    for each (childNode in typeXml.constant.(@type == "Class"))
                        enqueueWithName(rawAsset[childNode.@name], childNode.@name);
                    
                    for each (childNode in typeXml.variable.(@type == "Class"))
                        enqueueWithName(rawAsset[childNode.@name], childNode.@name);
                }
                else if (getQualifiedClassName(rawAsset) == "flash.filesystem::File")
                {
                    if (!rawAsset["exists"])
                    {
                        log("File or directory not found: '" + rawAsset["url"] + "'");
                    }
                    else if (!rawAsset["isHidden"])
                    {
                        if (rawAsset["isDirectory"])
                            enqueue.apply(this, rawAsset["getDirectoryListing"]());
                        else
                            enqueueWithName(rawAsset);
                    }
                }
                else if (rawAsset is String || rawAsset is URLRequest)
                {
                    enqueueWithName(rawAsset);
                }
                else
                {
                    log("Ignoring unsupported asset type: " + getQualifiedClassName(rawAsset));
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
        public function enqueueWithName(asset:Object, name:String=null,
                                        options:TextureOptions=null):String
        {
            var filename:String = null;

            if (getQualifiedClassName(asset) == "flash.filesystem::File")
            {
                filename = asset["name"];
                asset = decodeURI(asset["url"]);
            }

            if (name == null)    name = getName(asset);
            if (options == null) options = _defaultTextureOptions.clone();
            else                 options = options.clone();

            log("Enqueuing '" + (filename || name) + "'");

            _queue.push({
                name: name,
                asset: asset,
                options: options
            });
            
            return name;
        }
        
        /** Loads all enqueued assets asynchronously. The 'onProgress' function will be called
         *  with a 'ratio' between '0.0' and '1.0', with '1.0' meaning that it's complete.
         *
         *  <p>When you call this method, the manager will save a reference to "Starling.current";
         *  all textures that are loaded will be accessible only from within this instance. Thus,
         *  if you are working with more than one Starling instance, be sure to call
         *  "makeCurrent()" on the appropriate instance before processing the queue.</p>
         *
         *  @param onProgress <code>function(ratio:Number):void;</code>
         */
        public function loadQueue(onProgress:Function):void
        {
            if (onProgress == null)
                throw new ArgumentError("Argument 'onProgress' must not be null");

            if (_queue.length == 0)
            {
                onProgress(1.0);
                return;
            }

            _starling = Starling.current;
            
            if (_starling == null || _starling.context == null)
                throw new Error("The Starling instance needs to be ready before assets can be loaded.");

            const PROGRESS_PART_ASSETS:Number = 0.9;
            const PROGRESS_PART_XMLS:Number = 1.0 - PROGRESS_PART_ASSETS;

            var i:int;
            var canceled:Boolean = false;
            var xmls:Vector.<XML> = new <XML>[];
            var assetInfos:Array = _queue.concat();
            var assetCount:int = _queue.length;
            var assetProgress:Array = [];
            var assetIndex:int = 0;
            
            for (i=0; i<assetCount; ++i)
                assetProgress[i] = 0.0;

            for (i=0; i<_numConnections; ++i)
                loadNextQueueElement();

            _queue.length = 0;
            _numLoadingQueues++;
            addEventListener(Event.CANCEL, cancel);

            function loadNextQueueElement():void
            {
                if (assetIndex < assetInfos.length)
                {
                    // increment asset index *before* using it, since
                    // 'loadQueueElement' could by synchronous in subclasses.
                    var index:int = assetIndex++;
                    loadQueueElement(index, assetInfos[index]);
                }
            }

            function loadQueueElement(index:int, assetInfo:Object):void
            {
                if (canceled) return;
                
                var onElementProgress:Function = function(progress:Number):void
                {
                    updateAssetProgress(index, progress * 0.8); // keep 20 % for completion
                };
                var onElementLoaded:Function = function():void
                {
                    updateAssetProgress(index, 1.0);
                    assetCount--;

                    if (assetCount > 0) loadNextQueueElement();
                    else                processXmls();
                };

                processRawAsset(assetInfo.name, assetInfo.asset, assetInfo.options,
                    xmls, onElementProgress, onElementLoaded);
            }
            
            function updateAssetProgress(index:int, progress:Number):void
            {
                assetProgress[index] = progress;

                var sum:Number = 0.0;
                var len:int = assetProgress.length;

                for (i=0; i<len; ++i)
                    sum += assetProgress[i];

                onProgress(sum / len * PROGRESS_PART_ASSETS);
            }
            
            function processXmls():void
            {
                // xmls are processed separately at the end, because the textures they reference
                // have to be available for other XMLs. Texture atlases are processed first:
                // that way, their textures can be referenced, too.
                
                xmls.sort(function(a:XML, b:XML):int { 
                    return a.localName() == "TextureAtlas" ? -1 : 1; 
                });

                setTimeout(processXml, 1, 0);
            }

            function processXml(index:int):void
            {
                if (canceled) return;
                else if (index == xmls.length)
                {
                    finish();
                    return;
                }

                var name:String;
                var texture:Texture;
                var xml:XML = xmls[index];
                var rootNode:String = xml.localName();
                var xmlProgress:Number = (index + 1) / (xmls.length + 1);

                if (rootNode == "TextureAtlas")
                {
                    name = getName(xml.@imagePath.toString());
                    texture = getTexture(name);

                    if (texture)
                    {
                        addTextureAtlas(name, new TextureAtlas(texture, xml));
                        removeTexture(name, false);

                        if (_keepAtlasXmls) addXml(name, xml);
                        else System.disposeXML(xml);
                    }
                    else log("Cannot create atlas: texture '" + name + "' is missing.");
                }
                else if (rootNode == "font")
                {
                    name = getName(xml.pages.page.@file.toString());
                    texture = getTexture(name);

                    if (texture)
                    {
                        log("Adding bitmap font '" + name + "'");
                        TextField.registerCompositor(new BitmapFont(texture, xml), name);
                        removeTexture(name, false);

                        if (_keepFontXmls) addXml(name, xml);
                        else System.disposeXML(xml);
                    }
                    else log("Cannot create bitmap font: texture '" + name + "' is missing.");
                }
                else
                    throw new Error("XML contents not recognized: " + rootNode);

                onProgress(PROGRESS_PART_ASSETS + PROGRESS_PART_XMLS * xmlProgress);
                setTimeout(processXml, 1, index + 1);
            }
            
            function cancel():void
            {
                removeEventListener(Event.CANCEL, cancel);
                _numLoadingQueues--;
                canceled = true;
            }

            function finish():void
            {
                // We dance around the final "onProgress" call with some "setTimeout" calls here
                // to make sure the progress bar gets the chance to be rendered. Otherwise, all
                // would happen in one frame.

                setTimeout(function():void
                {
                    if (!canceled)
                    {
                        cancel();
                        onProgress(1.0);
                    }
                }, 1);
            }
        }
        
        private function processRawAsset(name:String, rawAsset:Object, options:TextureOptions,
                                         xmls:Vector.<XML>,
                                         onProgress:Function, onComplete:Function):void
        {
            var canceled:Boolean = false;
            
            addEventListener(Event.CANCEL, cancel);
            loadRawAsset(rawAsset, progress, process);
            
            function process(asset:Object):void
            {
                var texture:Texture;
                var bytes:ByteArray;
                var object:Object = null;
                var xml:XML = null;
                
                // the 'current' instance might have changed by now
                // if we're running in a set-up with multiple instances.
                _starling.makeCurrent();
                
                if (canceled)
                {
                    // do nothing
                }
                else if (asset == null)
                {
                    onComplete();
                }
                else if (asset is Sound)
                {
                    addSound(name, asset as Sound);
                    onComplete();
                }
                else if (asset is XML)
                {
                    xml = asset as XML;
                    
                    if (xml.localName() == "TextureAtlas" || xml.localName() == "font")
                        xmls.push(xml);
                    else
                        addXml(name, xml);
                    
                    onComplete();
                }
                else if (_starling.context.driverInfo == "Disposed")
                {
                    log("Context lost while processing assets, retrying ...");
                    setTimeout(process, 1, asset);
                    return; // to keep CANCEL event listener intact
                }
                else if (asset is Bitmap)
                {
                    texture = Texture.fromData(asset, options);
                    texture.root.onRestore = function():void
                    {
                        _numLostTextures++;
                        loadRawAsset(rawAsset, null, function(asset:Object):void
                        {
                            try
                            {
                                if (asset == null) throw new Error("Reload failed");
                                texture.root.uploadBitmap(asset as Bitmap);
                                asset.bitmapData.dispose();
                            }
                            catch (e:Error)
                            {
                                log("Texture restoration failed for '" + name + "': " + e.message);
                            }

                            _numRestoredTextures++;
                            Starling.current.stage.setRequiresRedraw();
                            
                            if (_numLostTextures == _numRestoredTextures)
                                dispatchEventWith(Event.TEXTURES_RESTORED);
                        });
                    };

                    asset.bitmapData.dispose();
                    addTexture(name, texture);
                    onComplete();
                }
                else if (asset is ByteArray)
                {
                    bytes = asset as ByteArray;
                    
                    if (AtfData.isAtfData(bytes))
                    {
                        options.onReady = prependCallback(options.onReady, function():void
                        {
                            addTexture(name, texture);
                            onComplete();
                        });

                        texture = Texture.fromData(bytes, options);
                        texture.root.onRestore = function():void
                        {
                            _numLostTextures++;
                            loadRawAsset(rawAsset, null, function(asset:Object):void
                            {
                                try
                                {
                                    if (asset == null) throw new Error("Reload failed");
                                    texture.root.uploadAtfData(asset as ByteArray, 0, false);
                                    asset.clear();
                                }
                                catch (e:Error)
                                {
                                    log("Texture restoration failed for '" + name + "': " + e.message);
                                }
                                
                                _numRestoredTextures++;
                                Starling.current.stage.setRequiresRedraw();
                                
                                if (_numLostTextures == _numRestoredTextures)
                                    dispatchEventWith(Event.TEXTURES_RESTORED);
                            });
                        };
                        
                        bytes.clear();
                    }
                    else if (byteArrayStartsWith(bytes, "{") || byteArrayStartsWith(bytes, "["))
                    {
                        try { object = JSON.parse(bytes.readUTFBytes(bytes.length)); }
                        catch (e:Error)
                        {
                            log("Could not parse JSON: " + e.message);
                            dispatchEventWith(Event.PARSE_ERROR, false, name);
                        }

                        if (object) addObject(name, object);

                        bytes.clear();
                        onComplete();
                    }
                    else if (byteArrayStartsWith(bytes, "<"))
                    {
                        try { xml = new XML(bytes); }
                        catch (e:Error)
                        {
                            log("Could not parse XML: " + e.message);
                            dispatchEventWith(Event.PARSE_ERROR, false, name);
                        }

                        process(xml);
                        bytes.clear();
                    }
                    else
                    {
                        addByteArray(name, bytes);
                        onComplete();
                    }
                }
                else
                {
                    addObject(name, asset);
                    onComplete();
                }
                
                // avoid that objects stay in memory (through 'onRestore' functions)
                asset = null;
                bytes = null;
                
                removeEventListener(Event.CANCEL, cancel);
            }
            
            function progress(ratio:Number):void
            {
                if (!canceled) onProgress(ratio);
            }
            
            function cancel():void
            {
                canceled = true;
            }
        }
        
        /** This method is called internally for each element of the queue when it is loaded.
         *  'rawAsset' is typically either a class (pointing to an embedded asset) or a string
         *  (containing the path to a file). For texture data, it will also be called after a
         *  context loss.
         *
         *  <p>The method has to transform this object into one of the types that the AssetManager
         *  can work with, e.g. a Bitmap, a Sound, XML data, or a ByteArray. This object needs to
         *  be passed to the 'onComplete' callback.</p>
         *
         *  <p>The calling method will then process this data accordingly (e.g. a Bitmap will be
         *  transformed into a texture). Unknown types will be available via 'getObject()'.</p>
         *
         *  <p>When overriding this method, you can call 'onProgress' with a number between 0 and 1
         *  to update the total queue loading progress.</p>
         */
        protected function loadRawAsset(rawAsset:Object, onProgress:Function, onComplete:Function):void
        {
            var extension:String = null;
            var loaderInfo:LoaderInfo = null;
            var urlLoader:URLLoader = null;
            var urlRequest:URLRequest = null;
            var url:String = null;

            if (rawAsset is Class)
            {
                setTimeout(complete, 1, new rawAsset());
            }
            else if (rawAsset is String || rawAsset is URLRequest)
            {
                urlRequest = rawAsset as URLRequest || new URLRequest(rawAsset as String);
                url = urlRequest.url;
                extension = getExtensionFromUrl(url);

                urlLoader = new URLLoader();
                urlLoader.dataFormat = URLLoaderDataFormat.BINARY;
                urlLoader.addEventListener(IOErrorEvent.IO_ERROR, onIoError);
                urlLoader.addEventListener(SecurityErrorEvent.SECURITY_ERROR, onSecurityError);
                urlLoader.addEventListener(HTTP_RESPONSE_STATUS, onHttpResponseStatus);
                urlLoader.addEventListener(ProgressEvent.PROGRESS, onLoadProgress);
                urlLoader.addEventListener(Event.COMPLETE, onUrlLoaderComplete);
                urlLoader.load(urlRequest);
            }

            function onIoError(event:IOErrorEvent):void
            {
                log("IO error: " + event.text);
                dispatchEventWith(Event.IO_ERROR, false, url);
                complete(null);
            }

            function onSecurityError(event:SecurityErrorEvent):void
            {
                log("security error: " + event.text);
                dispatchEventWith(Event.SECURITY_ERROR, false, url);
                complete(null);
            }

            function onHttpResponseStatus(event:HTTPStatusEvent):void
            {
                if (extension == null)
                {
                    var headers:Array = event["responseHeaders"];
                    var contentType:String = getHttpHeader(headers, "Content-Type");

                    if (contentType && /(audio|image)\//.exec(contentType))
                        extension = contentType.split("/").pop();
                }
            }

            function onLoadProgress(event:ProgressEvent):void
            {
                if (onProgress != null && event.bytesTotal > 0)
                    onProgress(event.bytesLoaded / event.bytesTotal);
            }
            
            function onUrlLoaderComplete(event:Object):void
            {
                var bytes:ByteArray = transformData(urlLoader.data as ByteArray, url);
                var sound:Sound;

                if (bytes == null)
                {
                    complete(null);
                    return;
                }
                
                if (extension)
                    extension = extension.toLowerCase();

                switch (extension)
                {
                    case "mpeg":
                    case "mp3":
                        sound = new Sound();
                        sound.loadCompressedDataFromByteArray(bytes, bytes.length);
                        bytes.clear();
                        complete(sound);
                        break;
                    case "jpg":
                    case "jpeg":
                    case "png":
                    case "gif":
                        var loaderContext:LoaderContext = new LoaderContext(_checkPolicyFile);
                        var loader:Loader = new Loader();
                        loaderContext.imageDecodingPolicy = ImageDecodingPolicy.ON_LOAD;
                        loaderInfo = loader.contentLoaderInfo;
                        loaderInfo.addEventListener(IOErrorEvent.IO_ERROR, onIoError);
                        loaderInfo.addEventListener(Event.COMPLETE, onLoaderComplete);
                        loader.loadBytes(bytes, loaderContext);
                        break;
                    default: // any XML / JSON / binary data 
                        complete(bytes);
                        break;
                }
            }
            
            function onLoaderComplete(event:Object):void
            {
                urlLoader.data.clear();
                complete(event.target.content);
            }
            
            function complete(asset:Object):void
            {
                // clean up event listeners

                if (urlLoader)
                {
                    urlLoader.removeEventListener(IOErrorEvent.IO_ERROR, onIoError);
                    urlLoader.removeEventListener(SecurityErrorEvent.SECURITY_ERROR, onSecurityError);
                    urlLoader.removeEventListener(HTTP_RESPONSE_STATUS, onHttpResponseStatus);
                    urlLoader.removeEventListener(ProgressEvent.PROGRESS, onLoadProgress);
                    urlLoader.removeEventListener(Event.COMPLETE, onUrlLoaderComplete);
                }

                if (loaderInfo)
                {
                    loaderInfo.removeEventListener(IOErrorEvent.IO_ERROR, onIoError);
                    loaderInfo.removeEventListener(Event.COMPLETE, onLoaderComplete);
                }

                // On mobile, it is not allowed / endorsed to make stage3D calls while the app
                // is in the background. Thus, we pause queue processing if that's the case.
                
                if (SystemUtil.isDesktop)
                    onComplete(asset);
                else
                    SystemUtil.executeWhenApplicationIsActive(onComplete, asset);
            }
        }
        
        // helpers

        /** This method is called by 'enqueue' to determine the name under which an asset will be
         *  accessible; override it if you need a custom naming scheme. Note that this method won't
         *  be called for embedded assets.
         *
         *  @param rawAsset   either a String, an URLRequest or a FileReference.
         */
        protected function getName(rawAsset:Object):String
        {
            var name:String;

            if      (rawAsset is String)        name =  rawAsset as String;
            else if (rawAsset is URLRequest)    name = (rawAsset as URLRequest).url;
            else if (rawAsset is FileReference) name = (rawAsset as FileReference).name;

            if (name)
            {
                name = name.replace(/%20/g, " "); // URLs use '%20' for spaces
                name = getBasenameFromUrl(name);

                if (name) return name;
                else throw new ArgumentError("Could not extract name from String '" + rawAsset + "'");
            }
            else
            {
                name = getQualifiedClassName(rawAsset);
                throw new ArgumentError("Cannot extract names for objects of type '" + name + "'");
            }
        }

        /** This method is called when raw byte data has been loaded from an URL or a file.
         *  Override it to process the downloaded data in some way (e.g. decompression) or
         *  to cache it on disk.
         *
         *  <p>It's okay to call one (or more) of the 'add...' methods from here. If the binary
         *  data contains multiple objects, this allows you to process all of them at once.
         *  Return 'null' to abort processing of the current item.</p> */
        protected function transformData(data:ByteArray, url:String):ByteArray
        {
            return data;
        }

        /** This method is called during loading of assets when 'verbose' is activated. Per
         *  default, it traces 'message' to the console. */
        protected function log(message:String):void
        {
            if (_verbose) trace("[AssetManager]", message);
        }
        
        private function byteArrayStartsWith(bytes:ByteArray, char:String):Boolean
        {
            var start:int = 0;
            var length:int = bytes.length;
            var wanted:int = char.charCodeAt(0);
            
            // recognize BOMs
            
            if (length >= 4 &&
                (bytes[0] == 0x00 && bytes[1] == 0x00 && bytes[2] == 0xfe && bytes[3] == 0xff) ||
                (bytes[0] == 0xff && bytes[1] == 0xfe && bytes[2] == 0x00 && bytes[3] == 0x00))
            {
                start = 4; // UTF-32
            }
            else if (length >= 3 && bytes[0] == 0xef && bytes[1] == 0xbb && bytes[2] == 0xbf)
            {
                start = 3; // UTF-8
            }
            else if (length >= 2 &&
                (bytes[0] == 0xfe && bytes[1] == 0xff) || (bytes[0] == 0xff && bytes[1] == 0xfe))
            {
                start = 2; // UTF-16
            }
            
            // find first meaningful letter
            
            for (var i:int=start; i<length; ++i)
            {
                var byte:int = bytes[i];
                if (byte == 0 || byte == 10 || byte == 13 || byte == 32) continue; // null, \n, \r, space
                else return byte == wanted;
            }
            
            return false;
        }
        
        private function getDictionaryKeys(dictionary:Dictionary, prefix:String="",
                                           out:Vector.<String>=null):Vector.<String>
        {
            if (out == null) out = new <String>[];
            
            for (var name:String in dictionary)
                if (name.indexOf(prefix) == 0)
                    out[out.length] = name; // avoid 'push'

            out.sort(Array.CASEINSENSITIVE);
            return out;
        }
        
        private function getHttpHeader(headers:Array, headerName:String):String
        {
            if (headers)
            {
                for each (var header:Object in headers)
                    if (header.name == headerName) return header.value;
            }
            return null;
        }

        /** Extracts the base name of a file path or URL, i.e. the file name without extension. */
        protected function getBasenameFromUrl(url:String):String
        {
            var matches:Array = NAME_REGEX.exec(url);
            if (matches && matches.length > 0) return matches[1];
            else return null;
        }

        /** Extracts the file extension from an URL. */
        protected function getExtensionFromUrl(url:String):String
        {
            var matches:Array = NAME_REGEX.exec(url);
            if (matches && matches.length > 1) return matches[2];
            else return null;
        }

        private function prependCallback(oldCallback:Function, newCallback:Function):Function
        {
            // TODO: it might make sense to add this (together with "appendCallback")
            //       as a public utility method ("FunctionUtil"?)

            if (oldCallback == null) return newCallback;
            else if (newCallback == null) return oldCallback;
            else return function():void
            {
                newCallback();
                oldCallback();
            };
        }

        // properties
        
        /** The queue contains one 'Object' for each enqueued asset. Each object has 'asset'
         *  and 'name' properties, pointing to the raw asset and its name, respectively. */
        protected function get queue():Array { return _queue; }
        
        /** Returns the number of raw assets that have been enqueued, but not yet loaded. */
        public function get numQueuedAssets():int { return _queue.length; }
        
        /** When activated, the class will trace information about added/enqueued assets.
         *  @default true */
        public function get verbose():Boolean { return _verbose; }
        public function set verbose(value:Boolean):void { _verbose = value; }
        
        /** Indicates if a queue is currently being loaded. */
        public function get isLoading():Boolean { return _numLoadingQueues > 0; }

        /** For bitmap textures, this flag indicates if mip maps should be generated when they 
         *  are loaded; for ATF textures, it indicates if mip maps are valid and should be
         *  used. @default false */
        public function get useMipMaps():Boolean { return _defaultTextureOptions.mipMapping; }
        public function set useMipMaps(value:Boolean):void { _defaultTextureOptions.mipMapping = value; }
        
        /** Textures that are created from Bitmaps or ATF files will have the scale factor
         *  assigned here. @default 1 */
        public function get scaleFactor():Number { return _defaultTextureOptions.scale; }
        public function set scaleFactor(value:Number):void { _defaultTextureOptions.scale = value; }

        /** Textures that are created from Bitmaps will be uploaded to the GPU with the
         *  <code>Context3DTextureFormat</code> assigned to this property. @default "bgra" */
        public function get textureFormat():String { return _defaultTextureOptions.format; }
        public function set textureFormat(value:String):void { _defaultTextureOptions.format = value; }

        /** Indicates if the underlying Stage3D textures should be created as the power-of-two based
         *  <code>Texture</code> class instead of the more memory efficient <code>RectangleTexture</code>.
         *  @default false */
        public function get forcePotTextures():Boolean { return _defaultTextureOptions.forcePotTexture; }
        public function set forcePotTextures(value:Boolean):void { _defaultTextureOptions.forcePotTexture = value; }
        
        /** Specifies whether a check should be made for the existence of a URL policy file before
         *  loading an object from a remote server. More information about this topic can be found 
         *  in the 'flash.system.LoaderContext' documentation. @default false */
        public function get checkPolicyFile():Boolean { return _checkPolicyFile; }
        public function set checkPolicyFile(value:Boolean):void { _checkPolicyFile = value; }

        /** Indicates if atlas XML data should be stored for access via the 'getXml' method.
         *  If true, you can access an XML under the same name as the atlas.
         *  If false, XMLs will be disposed when the atlas was created. @default false. */
        public function get keepAtlasXmls():Boolean { return _keepAtlasXmls; }
        public function set keepAtlasXmls(value:Boolean):void { _keepAtlasXmls = value; }

        /** Indicates if bitmap font XML data should be stored for access via the 'getXml' method.
         *  If true, you can access an XML under the same name as the bitmap font.
         *  If false, XMLs will be disposed when the font was created. @default false. */
        public function get keepFontXmls():Boolean { return _keepFontXmls; }
        public function set keepFontXmls(value:Boolean):void { _keepFontXmls = value; }

        /** The maximum number of parallel connections that are spawned when loading the queue.
         *  More connections can reduce loading times, but require more memory. @default 3. */
        public function get numConnections():int { return _numConnections; }
        public function set numConnections(value:int):void { _numConnections = value; }
    }
}
