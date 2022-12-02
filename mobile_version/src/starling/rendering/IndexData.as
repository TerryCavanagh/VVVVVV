// =================================================================================================
//
//  Starling Framework
//  Copyright Gamua GmbH. All Rights Reserved.
//
//	This program is free software. You can redistribute and/or modify it
//	in accordance with the terms of the accompanying license agreement.
//
// =================================================================================================

package starling.rendering
{
    import flash.display3D.Context3D;
    import flash.display3D.IndexBuffer3D;
    import flash.errors.EOFError;
    import flash.utils.ByteArray;
    import flash.utils.Endian;

    import starling.core.Starling;
    import starling.errors.MissingContextError;
    import starling.utils.StringUtil;

    /** The IndexData class manages a raw list of vertex indices, allowing direct upload
     *  to Stage3D index buffers. <em>You only have to work with this class if you're writing
     *  your own rendering code (e.g. if you create custom display objects).</em>
     *
     *  <p>To render objects with Stage3D, you have to organize vertices and indices in so-called
     *  vertex- and index-buffers. Vertex buffers store the coordinates of the vertices that make
     *  up an object; index buffers reference those vertices to determine which vertices spawn
     *  up triangles. Those buffers reside in graphics memory and can be accessed very
     *  efficiently by the GPU.</p>
     *
     *  <p>Before you can move data into the buffers, you have to set it up in conventional
     *  memory — that is, in a Vector or a ByteArray. Since it's quite cumbersome to manually
     *  create and manipulate those data structures, the IndexData and VertexData classes provide
     *  a simple way to do just that. The data is stored in a ByteArray (one index or vertex after
     *  the other) that can easily be uploaded to a buffer.</p>
     *
     *  <strong>Basic Quad Layout</strong>
     *
     *  <p>In many cases, the indices we are working with will reference just quads, i.e.
     *  triangles composing rectangles. That means that many IndexData instances will contain
     *  similar or identical data — a great opportunity for optimization!</p>
     *
     *  <p>If an IndexData instance follows a specific layout, it will be recognized
     *  automatically and many operations can be executed much faster. In Starling, that
     *  layout is called "basic quad layout". In order to recognize this specific sequence,
     *  the indices of each quad have to use the following order:</p>
     *
     *  <pre>n, n+1, n+2, n+1, n+3, n+2</pre>
     *
     *  <p>The subsequent quad has to use <code>n+4</code> as starting value, the next one
     *  <code>n+8</code>, etc. Here is an example with 3 quads / 6 triangles:</p>
     *
     *  <pre>0, 1, 2, 1, 3, 2,   4, 5, 6, 5, 7, 6,   8, 9, 10, 9, 11, 10</pre>
     *
     *  <p>If you are describing quad-like meshes, make sure to always use this layout.</p>
     *
     *  @see VertexData
     */
    public class IndexData
    {
        /** The number of bytes per index element. */
        private static const INDEX_SIZE:int = 2;

        private var _rawData:ByteArray;
        private var _numIndices:int;
        private var _initialCapacity:int;
        private var _useQuadLayout:Boolean;

        // basic quad layout
        private static var sQuadData:ByteArray = new ByteArray();
        private static var sQuadDataNumIndices:uint = 0;

        // helper objects
        private static var sVector:Vector.<uint> = new <uint>[];
        private static var sTrimData:ByteArray = new ByteArray();

        /** Creates an empty IndexData instance with the given capacity (in indices).
         *
         *  @param initialCapacity
         *
         *  The initial capacity affects just the way the internal ByteArray is allocated, not the
         *  <code>numIndices</code> value, which will always be zero when the constructor returns.
         *  The reason for this behavior is the peculiar way in which ByteArrays organize their
         *  memory:
         *
         *  <p>The first time you set the length of a ByteArray, it will adhere to that:
         *  a ByteArray with length 20 will take up 20 bytes (plus some overhead). When you change
         *  it to a smaller length, it will stick to the original value, e.g. with a length of 10
         *  it will still take up 20 bytes. However, now comes the weird part: change it to
         *  anything above the original length, and it will allocate 4096 bytes!</p>
         *
         *  <p>Thus, be sure to always make a generous educated guess, depending on the planned
         *  usage of your IndexData instances.</p>
         */
        public function IndexData(initialCapacity:int=48)
        {
            _numIndices = 0;
            _initialCapacity = initialCapacity;
            _useQuadLayout = true;
        }

        /** Explicitly frees up the memory used by the ByteArray, thus removing all indices.
         *  Quad layout will be restored (until adding data violating that layout). */
        public function clear():void
        {
            if (_rawData)
                _rawData.clear();

            _numIndices = 0;
            _useQuadLayout = true;
        }

        /** Creates a duplicate of the IndexData object. */
        public function clone():IndexData
        {
            var clone:IndexData = new IndexData(_numIndices);

            if (!_useQuadLayout)
            {
                clone.switchToGenericData();
                clone._rawData.writeBytes(_rawData);
            }

            clone._numIndices = _numIndices;
            return clone;
        }

        /** Copies the index data (or a range of it, defined by 'indexID' and 'numIndices')
         *  of this instance to another IndexData object, starting at a certain target index.
         *  If the target is not big enough, it will grow to fit all the new indices.
         *
         *  <p>By passing a non-zero <code>offset</code>, you can raise all copied indices
         *  by that value in the target object.</p>
         */
        public function copyTo(target:IndexData, targetIndexID:int=0, offset:int=0,
                               indexID:int=0, numIndices:int=-1):void
        {
            if (numIndices < 0 || indexID + numIndices > _numIndices)
                numIndices = _numIndices - indexID;

            var sourceData:ByteArray, targetData:ByteArray;
            var newNumIndices:int = targetIndexID + numIndices;

            if (target._numIndices < newNumIndices)
            {
                target._numIndices = newNumIndices;

                if (sQuadDataNumIndices  < newNumIndices)
                    ensureQuadDataCapacity(newNumIndices);
            }

            if (_useQuadLayout)
            {
                if (target._useQuadLayout)
                {
                    var keepsQuadLayout:Boolean = true;
                    var distance:int = targetIndexID - indexID;
                    var distanceInQuads:int = distance / 6;
                    var offsetInQuads:int = offset / 4;

                    // This code is executed very often. If it turns out that both IndexData
                    // instances use a quad layout, we don't need to do anything here.
                    //
                    // When "distance / 6 == offset / 4 && distance % 6 == 0 && offset % 4 == 0",
                    // the copy operation preserves the quad layout. In that case, we can exit
                    // right away. The code below is a little more complex, though, to avoid the
                    // (surprisingly costly) mod-operations.

                    if (distanceInQuads == offsetInQuads && (offset & 3) == 0 &&
                        distanceInQuads * 6 == distance)
                    {
                        keepsQuadLayout = true;
                    }
                    else if (numIndices > 2)
                    {
                        keepsQuadLayout = false;
                    }
                    else
                    {
                        for (var i:int=0; i<numIndices; ++i)
                            keepsQuadLayout &&=
                                getBasicQuadIndexAt(indexID + i) + offset ==
                                getBasicQuadIndexAt(targetIndexID + i);
                    }

                    if (keepsQuadLayout) return;
                    else target.switchToGenericData();
                }

                sourceData = sQuadData;
                targetData = target._rawData;

                if ((offset & 3) == 0) // => offset % 4 == 0
                {
                    indexID += 6 * offset / 4;
                    offset = 0;
                    ensureQuadDataCapacity(indexID + numIndices);
                }
            }
            else
            {
                if (target._useQuadLayout)
                    target.switchToGenericData();

                sourceData = _rawData;
                targetData = target._rawData;
            }

            targetData.position = targetIndexID * INDEX_SIZE;

            if (offset == 0)
                targetData.writeBytes(sourceData, indexID * INDEX_SIZE, numIndices * INDEX_SIZE);
            else
            {
                sourceData.position = indexID * INDEX_SIZE;

                // by reading junks of 32 instead of 16 bits, we can spare half the time
                while (numIndices > 1)
                {
                    var indexAB:uint = sourceData.readUnsignedInt();
                    var indexA:uint  = ((indexAB & 0xffff0000) >> 16) + offset;
                    var indexB:uint  = ((indexAB & 0x0000ffff)      ) + offset;
                    targetData.writeUnsignedInt(indexA << 16 | indexB);
                    numIndices -= 2;
                }

                if (numIndices)
                    targetData.writeShort(sourceData.readUnsignedShort() + offset);
            }
        }

        /** Sets an index at the specified position. */
        public function setIndex(indexID:int, index:uint):void
        {
            if (_numIndices < indexID + 1)
                 numIndices = indexID + 1;

            if (_useQuadLayout)
            {
                if (getBasicQuadIndexAt(indexID) == index) return;
                else switchToGenericData();
            }

            _rawData.position = indexID * INDEX_SIZE;
            _rawData.writeShort(index);
        }

        /** Reads the index from the specified position. */
        public function getIndex(indexID:int):int
        {
            if (_useQuadLayout)
            {
                if (indexID < _numIndices)
                    return getBasicQuadIndexAt(indexID);
                else
                    throw new EOFError();
            }
            else
            {
                _rawData.position = indexID * INDEX_SIZE;
                return _rawData.readUnsignedShort();
            }
        }

        /** Adds an offset to all indices in the specified range. */
        public function offsetIndices(offset:int, indexID:int=0, numIndices:int=-1):void
        {
            if (numIndices < 0 || indexID + numIndices > _numIndices)
                numIndices = _numIndices - indexID;

            var endIndex:int = indexID + numIndices;

            for (var i:int=indexID; i<endIndex; ++i)
                setIndex(i, getIndex(i) + offset);
        }

        /** Appends three indices representing a triangle. Reference the vertices clockwise,
         *  as this defines the front side of the triangle. */
        public function addTriangle(a:uint, b:uint, c:uint):void
        {
            if (_useQuadLayout)
            {
                if (a == getBasicQuadIndexAt(_numIndices))
                {
                    var oddTriangleID:Boolean = (_numIndices & 1) != 0;
                    var evenTriangleID:Boolean = !oddTriangleID;

                    if ((evenTriangleID && b == a + 1 && c == b + 1) ||
                         (oddTriangleID && c == a + 1 && b == c + 1))
                    {
                        _numIndices += 3;
                        ensureQuadDataCapacity(_numIndices);
                        return;
                    }
                }

                switchToGenericData();
            }

            _rawData.position = _numIndices * INDEX_SIZE;
            _rawData.writeShort(a);
            _rawData.writeShort(b);
            _rawData.writeShort(c);
            _numIndices += 3;
        }

        /** Appends two triangles spawning up the quad with the given indices.
         *  The indices of the vertices are arranged like this:
         *
         *  <pre>
         *  a - b
         *  | / |
         *  c - d
         *  </pre>
         *
         *  <p>To make sure the indices will follow the basic quad layout, make sure each
         *  parameter increments the one before it (e.g. <code>0, 1, 2, 3</code>).</p>
         */
        public function addQuad(a:uint, b:uint, c:uint, d:uint):void
        {
            if (_useQuadLayout)
            {
                if (a == getBasicQuadIndexAt(_numIndices) &&
                    b == a + 1 && c == b + 1 && d == c + 1)
                {
                    _numIndices += 6;
                    ensureQuadDataCapacity(_numIndices);
                    return;
                }
                else switchToGenericData();
            }

            _rawData.position = _numIndices * INDEX_SIZE;
            _rawData.writeShort(a);
            _rawData.writeShort(b);
            _rawData.writeShort(c);
            _rawData.writeShort(b);
            _rawData.writeShort(d);
            _rawData.writeShort(c);
            _numIndices += 6;
        }

        /** Creates a vector containing all indices. If you pass an existing vector to the method,
         *  its contents will be overwritten. */
        public function toVector(out:Vector.<uint>=null):Vector.<uint>
        {
            if (out == null) out = new Vector.<uint>(_numIndices);
            else out.length = _numIndices;

            var rawData:ByteArray = _useQuadLayout ? sQuadData : _rawData;
            rawData.position = 0;

            for (var i:int=0; i<_numIndices; ++i)
                out[i] = rawData.readUnsignedShort();

            return out;
        }

        /** Returns a string representation of the IndexData object,
         *  including a comma-separated list of all indices. */
        public function toString():String
        {
            var string:String = StringUtil.format("[IndexData numIndices={0} indices=\"{1}\"]",
                _numIndices, toVector(sVector).join());

            sVector.length = 0;
            return string;
        }

        // private helpers

        private function switchToGenericData():void
        {
            if (_useQuadLayout)
            {
                _useQuadLayout = false;

                if (_rawData == null)
                {
                    _rawData = new ByteArray();
                    _rawData.endian = Endian.LITTLE_ENDIAN;
                    _rawData.length = _initialCapacity * INDEX_SIZE; // -> allocated memory
                    _rawData.length = _numIndices * INDEX_SIZE;      // -> actual length
                }

                if (_numIndices)
                    _rawData.writeBytes(sQuadData, 0, _numIndices * INDEX_SIZE);
            }
        }

        /** Makes sure that the ByteArray containing the normalized, basic quad data contains at
         *  least <code>numIndices</code> indices. The array might grow, but it will never be
         *  made smaller. */
        private function ensureQuadDataCapacity(numIndices:int):void
        {
            if (sQuadDataNumIndices >= numIndices) return;

            var i:int;
            var oldNumQuads:int = sQuadDataNumIndices / 6;
            var newNumQuads:int = Math.ceil(numIndices / 6);

            sQuadData.endian = Endian.LITTLE_ENDIAN;
            sQuadData.position = sQuadData.length;
            sQuadDataNumIndices = newNumQuads * 6;

            for (i = oldNumQuads; i < newNumQuads; ++i)
            {
                sQuadData.writeShort(4 * i);
                sQuadData.writeShort(4 * i + 1);
                sQuadData.writeShort(4 * i + 2);
                sQuadData.writeShort(4 * i + 1);
                sQuadData.writeShort(4 * i + 3);
                sQuadData.writeShort(4 * i + 2);
            }
        }

        /** Returns the index that's expected at this position if following basic quad layout. */
        private static function getBasicQuadIndexAt(indexID:int):int
        {
            var quadID:int = indexID / 6;
            var posInQuad:int = indexID - quadID * 6; // => indexID % 6
            var offset:int;

            if (posInQuad == 0) offset = 0;
            else if (posInQuad == 1 || posInQuad == 3) offset = 1;
            else if (posInQuad == 2 || posInQuad == 5) offset = 2;
            else offset = 3;

            return quadID * 4 + offset;
        }

        // IndexBuffer helpers

        /** Creates an index buffer object with the right size to fit the complete data.
         *  Optionally, the current data is uploaded right away. */
        public function createIndexBuffer(upload:Boolean=false,
                                          bufferUsage:String="staticDraw"):IndexBuffer3D
        {
            var context:Context3D = Starling.context;
            if (context == null) throw new MissingContextError();
            if (_numIndices == 0) return null;

            var buffer:IndexBuffer3D = context.createIndexBuffer(_numIndices, bufferUsage);

            if (upload) uploadToIndexBuffer(buffer);
            return buffer;
        }

        /** Uploads the complete data (or a section of it) to the given index buffer. */
        public function uploadToIndexBuffer(buffer:IndexBuffer3D, indexID:int=0, numIndices:int=-1):void
        {
            if (numIndices < 0 || indexID + numIndices > _numIndices)
                numIndices = _numIndices - indexID;

            if (numIndices > 0)
                buffer.uploadFromByteArray(rawData, 0, indexID, numIndices);
        }

        /** Optimizes the ByteArray so that it has exactly the required capacity, without
         *  wasting any memory. If your IndexData object grows larger than the initial capacity
         *  you passed to the constructor, call this method to avoid the 4k memory problem. */
        public function trim():void
        {
            if (_useQuadLayout) return;

            sTrimData.length = _rawData.length;
            sTrimData.position = 0;
            sTrimData.writeBytes(_rawData);

            _rawData.clear();
            _rawData.length = sTrimData.length;
            _rawData.writeBytes(sTrimData);

            sTrimData.clear();
        }

        // properties

        /** The total number of indices.
         *
         *  <p>If this instance contains only standardized, basic quad indices, resizing
         *  will automatically fill up with appropriate quad indices. Otherwise, it will fill
         *  up with zeroes.</p>
         *
         *  <p>If you set the number of indices to zero, quad layout will be restored.</p> */
        public function get numIndices():int { return _numIndices; }
        public function set numIndices(value:int):void
        {
            if (value != _numIndices)
            {
                if (_useQuadLayout) ensureQuadDataCapacity(value);
                else _rawData.length = value * INDEX_SIZE;
                if (value == 0) _useQuadLayout = true;

                _numIndices = value;
            }
        }

        /** The number of triangles that can be spawned up with the contained indices.
         *  (In other words: the number of indices divided by three.) */
        public function get numTriangles():int { return _numIndices / 3; }
        public function set numTriangles(value:int):void { numIndices = value * 3; }

        /** The number of quads that can be spawned up with the contained indices.
         *  (In other words: the number of triangles divided by two.) */
        public function get numQuads():int { return _numIndices / 6; }
        public function set numQuads(value:int):void { numIndices = value * 6; }

        /** The number of bytes required for each index value. */
        public function get indexSizeInBytes():int { return INDEX_SIZE; }

        /** Indicates if all indices are following the basic quad layout.
         *
         *  <p>This property is automatically updated if an index is set to a value that violates
         *  basic quad layout. Once the layout was violated, the instance will always stay that
         *  way, even if you fix that violating value later. Only calling <code>clear</code> or
         *  manually enabling the property will restore quad layout.</p>
         *
         *  <p>If you enable this property on an instance, all indices will immediately be
         *  replaced with indices following standard quad layout.</p>
         *
         *  <p>Please look at the class documentation for more information about that kind
         *  of layout, and why it is important.</p>
         *
         *  @default true
         */
        public function get useQuadLayout():Boolean { return _useQuadLayout; }
        public function set useQuadLayout(value:Boolean):void
        {
            if (value != _useQuadLayout)
            {
                if (value)
                {
                    ensureQuadDataCapacity(_numIndices);
                    _rawData.length = 0;
                    _useQuadLayout = true;
                }
                else switchToGenericData();
            }
        }

        /** The raw index data; not a copy! Beware: the referenced ByteArray may change any time.
         *  Never store a reference to it, and never modify its contents manually. */
        public function get rawData():ByteArray
        {
            if (_useQuadLayout) return sQuadData;
            else return _rawData;
        }
    }
}
