package starling.assets
{
    import flash.utils.ByteArray;

    import starling.utils.ByteArrayUtil;

    /** This AssetFactory creates objects from JSON data. */
    public class JsonFactory extends AssetFactory
    {
        /** Creates a new instance. */
        public function JsonFactory()
        {
            addExtensions("json");
            addMimeTypes("application/json", "text/json");
        }

        /** @inheritDoc */
        override public function canHandle(reference:AssetReference):Boolean
        {
            return super.canHandle(reference) || (reference.data is ByteArray &&
                ByteArrayUtil.startsWithString(reference.data as ByteArray, "{"));
        }

        /** @inheritDoc */
        override public function create(reference:AssetReference, helper:AssetFactoryHelper,
                                        onComplete:Function, onError:Function):void
        {
            try
            {
                var bytes:ByteArray = reference.data as ByteArray;
                var object:Object = JSON.parse(bytes.readUTFBytes(bytes.length));
                onComplete(reference.name, object);
            }
            catch (e:Error)
            {
                onError("Could not parse JSON: " + e.message);
            }
        }
    }
}
