package starling.assets
{
    import flash.utils.ByteArray;

    import starling.textures.AtfData;
    import starling.textures.Texture;
    import starling.utils.execute;

    /** This AssetFactory creates texture assets from ATF files. */
    public class AtfTextureFactory extends AssetFactory
    {
        /** Creates a new instance. */
        public function AtfTextureFactory()
        {
            addExtensions("atf"); // not used, actually, since we can parse the ATF header, anyway.
        }

        /** @inheritDoc */
        override public function canHandle(reference:AssetReference):Boolean
        {
            return (reference.data is ByteArray && AtfData.isAtfData(reference.data as ByteArray));
        }

        /** @inheritDoc */
        override public function create(reference:AssetReference, helper:AssetFactoryHelper,
                                        onComplete:Function, onError:Function):void
        {
            helper.executeWhenContextReady(createTexture);

            function createTexture():void
            {
                var onReady:Function = reference.textureOptions.onReady as Function;
                reference.textureOptions.onReady = function():void
                {
                    execute(onReady, texture);
                    onComplete(reference.name, texture);
                };

                var texture:Texture = null;
                var url:String = reference.url;

                try { texture = Texture.fromData(reference.data, reference.textureOptions); }
                catch (e:Error) { onError(e.message); }

                if (url && texture)
                {
                    texture.root.onRestore = function():void
                    {
                        helper.onBeginRestore();
                        helper.loadDataFromUrl(url, function(data:ByteArray):void
                        {
                            helper.executeWhenContextReady(function():void
                            {
                                try { texture.root.uploadAtfData(data); }
                                catch (e:Error) { helper.log("Texture restoration failed: " + e.message); }

                                helper.onEndRestore();
                            });
                        }, onReloadError);
                    };
                }

                reference.data = null; // prevent closures from keeping reference
            }

            function onReloadError(error:String):void
            {
                helper.log("Texture restoration failed for " + reference.url + ". " + error);
                helper.onEndRestore();
            }
        }
    }
}
