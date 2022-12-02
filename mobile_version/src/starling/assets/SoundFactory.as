package starling.assets
{
    import flash.media.Sound;
    import flash.utils.ByteArray;

    import starling.utils.ByteArrayUtil;

    /** This AssetFactory creates sound assets. */
    public class SoundFactory extends AssetFactory
    {
        private static const MAGIC_NUMBERS_A:Array = [0xFF, 0xFB];
        private static const MAGIC_NUMBERS_B:Array = [0x49, 0x44, 0x33];

        /** Creates a new instance. */
        public function SoundFactory()
        {
            addMimeTypes("audio/mp3", "audio/mpeg3", "audio/mpeg");
            addExtensions("mp3");
        }

        /** @inheritDoc */
        override public function canHandle(reference:AssetReference):Boolean
        {
            if (reference.data is Sound || super.canHandle(reference))
                return true;
            else if (reference.data is ByteArray)
            {
                var byteData:ByteArray = reference.data as ByteArray;
                return ByteArrayUtil.startsWithBytes(byteData, MAGIC_NUMBERS_A) ||
                       ByteArrayUtil.startsWithBytes(byteData, MAGIC_NUMBERS_B);
            }
            else return false;
        }

        /** @inheritDoc */
        override public function create(reference:AssetReference, helper:AssetFactoryHelper,
                                        onComplete:Function, onError:Function):void
        {
            var sound:Sound = reference.data as Sound;
            var bytes:ByteArray = reference.data as ByteArray;

            if (bytes)
            {
                try
                {
                    sound = new Sound();
                    sound.loadCompressedDataFromByteArray(bytes, bytes.length);
                }
                catch (e:Error)
                {
                    onError("Could not load sound data: " + e.message);
                    return;
                }

            }

            onComplete(reference.name, sound);
        }
    }
}
