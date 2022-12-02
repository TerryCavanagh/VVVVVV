package starling.assets
{
    import flash.utils.ByteArray;

    /** This AssetFactory forwards ByteArrays to the AssetManager. It's the fallback when
     *  no other factory can handle an asset reference (default priority: -100). */
    public class ByteArrayFactory extends AssetFactory
    {
        /** Creates a new instance. */
        public function ByteArrayFactory()
        {
            // not used, actually - this factory is used as a fallback with low priority
            addExtensions("bin");
            addMimeTypes("application/octet-stream");
        }

        /** @inheritDoc */
        override public function canHandle(reference:AssetReference):Boolean
        {
            return reference.data is ByteArray;
        }

        /** @inheritDoc */
        override public function create(reference:AssetReference, helper:AssetFactoryHelper,
                                        onComplete:Function, onError:Function):void
        {
            onComplete(reference.name, reference.data as ByteArray);
        }
    }
}
