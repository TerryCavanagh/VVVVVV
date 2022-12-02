// =================================================================================================
//
//	Starling Framework
//	Copyright Gamua GmbH. All Rights Reserved.
//
//	This program is free software. You can redistribute and/or modify it
//	in accordance with the terms of the accompanying license agreement.
//
// =================================================================================================

package starling.rendering
{
    import starling.utils.StringUtil;

    /** Points to a location within a list of MeshBatches.
     *
     *  <p>Starling uses these tokens in its render cache. Each call to
     *  <code>painter.pushState()</code> or <code>painter.popState()</code> provides a token
     *  referencing the current location within the cache. In the next frame, if the relevant
     *  part of the display tree has not changed, these tokens can be used to render directly
     *  from the cache instead of constructing new MeshBatches.</p>
     *
     *  @see Painter
     */
    public class BatchToken
    {
        /** The ID of the current MeshBatch. */
        public var batchID:int;

        /** The ID of the next vertex within the current MeshBatch. */
        public var vertexID:int;

        /** The ID of the next index within the current MeshBatch. */
        public var indexID:int;

        /** Creates a new BatchToken. */
        public function BatchToken(batchID:int=0, vertexID:int=0, indexID:int=0)
        {
            setTo(batchID, vertexID, indexID);
        }

        /** Copies the properties from the given token to this instance. */
        public function copyFrom(token:BatchToken):void
        {
            batchID  = token.batchID;
            vertexID = token.vertexID;
            indexID  = token.indexID;
        }

        /** Changes all properties at once. */
        public function setTo(batchID:int=0, vertexID:int=0, indexID:int=0):void
        {
            this.batchID = batchID;
            this.vertexID = vertexID;
            this.indexID = indexID;
        }

        /** Resets all properties to zero. */
        public function reset():void
        {
            batchID = vertexID = indexID = 0;
        }

        /** Indicates if this token contains the same values as the given one. */
        public function equals(other:BatchToken):Boolean
        {
            return batchID == other.batchID && vertexID == other.vertexID && indexID == other.indexID;
        }

        /** Creates a String representation of this instance. */
        public function toString():String
        {
            return StringUtil.format("[BatchToken batchID={0} vertexID={1} indexID={2}]",
                batchID, vertexID, indexID);
        }
    }
}
