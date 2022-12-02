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
    import flash.geom.Matrix;
    import flash.geom.Matrix3D;
    import flash.geom.Point;
    import flash.geom.Rectangle;
    import flash.geom.Vector3D;

    import starling.display.DisplayObject;
    import starling.display.Stage;
    import starling.errors.AbstractClassError;
    import starling.rendering.IndexData;
    import starling.rendering.VertexData;

    /** A utility class that helps with tasks that are common when working with meshes. */
    public class MeshUtil
    {
        // helper objects
        private static var sPoint3D:Vector3D = new Vector3D();
        private static var sMatrix:Matrix = new Matrix();
        private static var sMatrix3D:Matrix3D = new Matrix3D();

        /** @private */
        public function MeshUtil() { throw new AbstractClassError(); }

        /** Determines if a point is inside a mesh that is spawned up by the given
         *  vertex- and index-data. */
        public static function containsPoint(vertexData:VertexData, indexData:IndexData,
                                             point:Point):Boolean
        {
            var i:int;
            var result:Boolean = false;
            var numIndices:int = indexData.numIndices;
            var p0:Point = Pool.getPoint();
            var p1:Point = Pool.getPoint();
            var p2:Point = Pool.getPoint();

            for (i=0; i<numIndices; i+=3)
            {
                vertexData.getPoint(indexData.getIndex(i  ), "position", p0);
                vertexData.getPoint(indexData.getIndex(i+1), "position", p1);
                vertexData.getPoint(indexData.getIndex(i+2), "position", p2);

                if (MathUtil.isPointInTriangle(point, p0, p1, p2))
                {
                    result = true;
                    break;
                }
            }

            Pool.putPoint(p0);
            Pool.putPoint(p1);
            Pool.putPoint(p2);

            return result;
        }

        /** Calculates the bounds of the given vertices in the target coordinate system. */
        public static function calculateBounds(vertexData:VertexData,
                                               sourceSpace:DisplayObject,
                                               targetSpace:DisplayObject,
                                               out:Rectangle=null):Rectangle
        {
            if (out == null) out = new Rectangle();

            var stage:Stage = sourceSpace.stage;

            if (sourceSpace.is3D && stage)
            {
                stage.getCameraPosition(targetSpace, sPoint3D);
                sourceSpace.getTransformationMatrix3D(targetSpace, sMatrix3D);
                vertexData.getBoundsProjected("position", sMatrix3D, sPoint3D, 0, -1, out);
            }
            else
            {
                sourceSpace.getTransformationMatrix(targetSpace, sMatrix);
                vertexData.getBounds("position", sMatrix, 0, -1, out);
            }

            return out;
        }
    }
}
