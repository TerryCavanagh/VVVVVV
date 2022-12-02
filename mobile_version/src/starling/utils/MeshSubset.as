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
    /** A class describing a range of vertices and indices, thus referencing a subset of a Mesh. */
    public class MeshSubset
    {
        /** The ID of the first vertex. */
        public var vertexID:int;

        /** The total number of vertices. */
        public var numVertices:int;

        /** The ID of the first index. */
        public var indexID:int;

        /** The total number of indices. */
        public var numIndices:int;

        /** Creates a new MeshSubset. */
        public function MeshSubset(vertexID:int=0, numVertices:int=-1,
                                   indexID:int=0,  numIndices:int=-1)
        {
            setTo(vertexID, numVertices, indexID, numIndices);
        }

        /** Changes all properties at once.
         *  Call without any arguments to reference a complete mesh. */
        public function setTo(vertexID:int=0, numVertices:int=-1,
                               indexID:int=0, numIndices:int=-1):void
        {
            this.vertexID = vertexID;
            this.numVertices = numVertices;
            this.indexID = indexID;
            this.numIndices = numIndices;
        }
    }
}
