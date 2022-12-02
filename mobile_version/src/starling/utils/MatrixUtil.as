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
    import flash.geom.Vector3D;

    import starling.errors.AbstractClassError;

    /** A utility class containing methods related to the Matrix class. */
    public class MatrixUtil
    {
        // helper objects
        private static var sRawData:Vector.<Number> =
            new <Number>[1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1];
        private static var sRawData2:Vector.<Number> = new Vector.<Number>(16, true);
        private static var sPoint3D:Vector3D = new Vector3D();
        private static var sMatrixData:Vector.<Number> =
                new <Number>[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];

        /** @private */
        public function MatrixUtil() { throw new AbstractClassError(); }

        /** Converts a 2D matrix to a 3D matrix. If you pass an <code>out</code>-matrix,
         *  the result will be stored in this matrix instead of creating a new object. */
        public static function convertTo3D(matrix:Matrix, out:Matrix3D=null):Matrix3D
        {
            if (out == null) out = new Matrix3D();

            sRawData[ 0] = matrix.a;
            sRawData[ 1] = matrix.b;
            sRawData[ 4] = matrix.c;
            sRawData[ 5] = matrix.d;
            sRawData[12] = matrix.tx;
            sRawData[13] = matrix.ty;

            out.copyRawDataFrom(sRawData);
            return out;
        }

        /** Converts a 3D matrix to a 2D matrix. Beware that this will work only for a 3D matrix
         *  describing a pure 2D transformation. */
        public static function convertTo2D(matrix3D:Matrix3D, out:Matrix=null):Matrix
        {
            if (out == null) out = new Matrix();

            matrix3D.copyRawDataTo(sRawData2);
            out.a  = sRawData2[ 0];
            out.b  = sRawData2[ 1];
            out.c  = sRawData2[ 4];
            out.d  = sRawData2[ 5];
            out.tx = sRawData2[12];
            out.ty = sRawData2[13];

            return out;
        }

        /** Determines if the matrix is an identity matrix. */
        public static function isIdentity(matrix:Matrix):Boolean
        {
            return matrix.a  == 1.0 && matrix.b  == 0.0 && matrix.c == 0.0 && matrix.d == 1.0 &&
                   matrix.tx == 0.0 && matrix.ty == 0.0;
        }

        /** Determines if the 3D matrix is an identity matrix. */
        public static function isIdentity3D(matrix:Matrix3D):Boolean
        {
            var data:Vector.<Number> = sRawData2;
            matrix.copyRawDataTo(data);

            return data[ 0] == 1.0 && data[ 1] == 0.0 && data[ 2] == 0.0 && data[ 3] == 0.0 &&
                   data[ 4] == 0.0 && data[ 5] == 1.0 && data[ 6] == 0.0 && data[ 7] == 0.0 &&
                   data[ 8] == 0.0 && data[ 9] == 0.0 && data[10] == 1.0 && data[11] == 0.0 &&
                   data[12] == 0.0 && data[13] == 0.0 && data[14] == 0.0 && data[15] == 1.0;
        }

        /** Transform a point with the given matrix. */
        public static function transformPoint(matrix:Matrix, point:Point,
                                              out:Point=null):Point
        {
            return transformCoords(matrix, point.x, point.y, out);
        }

        /** Transforms a 3D point with the given matrix. */
        public static function transformPoint3D(matrix:Matrix3D, point:Vector3D,
                                                out:Vector3D=null):Vector3D
        {
            return transformCoords3D(matrix, point.x, point.y, point.z, out);
        }

        /** Uses a matrix to transform 2D coordinates into a different space. If you pass an
         *  <code>out</code>-point, the result will be stored in this point instead of creating
         *  a new object. */
        public static function transformCoords(matrix:Matrix, x:Number, y:Number,
                                               out:Point=null):Point
        {
            if (out == null) out = new Point();

            out.x = matrix.a * x + matrix.c * y + matrix.tx;
            out.y = matrix.d * y + matrix.b * x + matrix.ty;

            return out;
        }

        /** Uses a matrix to transform 3D coordinates into a different space. If you pass a
         *  'resultVector', the result will be stored in this vector3D instead of creating a
         *  new object. */
        public static function transformCoords3D(matrix:Matrix3D, x:Number, y:Number, z:Number,
                                                 out:Vector3D=null):Vector3D
        {
            if (out == null) out = new Vector3D();

            matrix.copyRawDataTo(sRawData2);
            out.x = x * sRawData2[0] + y * sRawData2[4] + z * sRawData2[ 8] + sRawData2[12];
            out.y = x * sRawData2[1] + y * sRawData2[5] + z * sRawData2[ 9] + sRawData2[13];
            out.z = x * sRawData2[2] + y * sRawData2[6] + z * sRawData2[10] + sRawData2[14];
            out.w = x * sRawData2[3] + y * sRawData2[7] + z * sRawData2[11] + sRawData2[15];

            return out;
        }

        /** Appends a skew transformation to a matrix (angles in radians). The skew matrix
         *  has the following form:
         *  <pre>
         *  | cos(skewY)  -sin(skewX)  0 |
         *  | sin(skewY)   cos(skewX)  0 |
         *  |     0            0       1 |
         *  </pre>
         */
        public static function skew(matrix:Matrix, skewX:Number, skewY:Number):void
        {
            var sinX:Number = Math.sin(skewX);
            var cosX:Number = Math.cos(skewX);
            var sinY:Number = Math.sin(skewY);
            var cosY:Number = Math.cos(skewY);

            matrix.setTo(matrix.a  * cosY - matrix.b  * sinX,
                         matrix.a  * sinY + matrix.b  * cosX,
                         matrix.c  * cosY - matrix.d  * sinX,
                         matrix.c  * sinY + matrix.d  * cosX,
                         matrix.tx * cosY - matrix.ty * sinX,
                         matrix.tx * sinY + matrix.ty * cosX);
        }

        /** Prepends a matrix to 'base' by multiplying it with another matrix. */
        public static function prependMatrix(base:Matrix, prep:Matrix):void
        {
            base.setTo(base.a * prep.a + base.c * prep.b,
                       base.b * prep.a + base.d * prep.b,
                       base.a * prep.c + base.c * prep.d,
                       base.b * prep.c + base.d * prep.d,
                       base.tx + base.a * prep.tx + base.c * prep.ty,
                       base.ty + base.b * prep.tx + base.d * prep.ty);
        }

        /** Prepends an incremental translation to a Matrix object. */
        public static function prependTranslation(matrix:Matrix, tx:Number, ty:Number):void
        {
            matrix.tx += matrix.a * tx + matrix.c * ty;
            matrix.ty += matrix.b * tx + matrix.d * ty;
        }

        /** Prepends an incremental scale change to a Matrix object. */
        public static function prependScale(matrix:Matrix, sx:Number, sy:Number):void
        {
            matrix.setTo(matrix.a * sx, matrix.b * sx,
                         matrix.c * sy, matrix.d * sy,
                         matrix.tx, matrix.ty);
        }

        /** Prepends an incremental rotation to a Matrix object (angle in radians). */
        public static function prependRotation(matrix:Matrix, angle:Number):void
        {
            var sin:Number = Math.sin(angle);
            var cos:Number = Math.cos(angle);

            matrix.setTo(matrix.a * cos + matrix.c * sin,  matrix.b * cos + matrix.d * sin,
                         matrix.c * cos - matrix.a * sin,  matrix.d * cos - matrix.b * sin,
                         matrix.tx, matrix.ty);
        }

        /** Prepends a skew transformation to a Matrix object (angles in radians). The skew matrix
         *  has the following form:
         *  <pre>
         *  | cos(skewY)  -sin(skewX)  0 |
         *  | sin(skewY)   cos(skewX)  0 |
         *  |     0            0       1 |
         *  </pre>
         */
        public static function prependSkew(matrix:Matrix, skewX:Number, skewY:Number):void
        {
            var sinX:Number = Math.sin(skewX);
            var cosX:Number = Math.cos(skewX);
            var sinY:Number = Math.sin(skewY);
            var cosY:Number = Math.cos(skewY);

            matrix.setTo(matrix.a * cosY + matrix.c * sinY,
                         matrix.b * cosY + matrix.d * sinY,
                         matrix.c * cosX - matrix.a * sinX,
                         matrix.d * cosX - matrix.b * sinX,
                         matrix.tx, matrix.ty);
        }

        /** Converts a Matrix3D instance to a String, which is useful when debugging. Per default,
         *  the raw data is displayed transposed, so that the columns are displayed vertically. */
        public static function toString3D(matrix:Matrix3D, transpose:Boolean=true,
                                          precision:int=3):String
        {
            if (transpose) matrix.transpose();
            matrix.copyRawDataTo(sRawData2);
            if (transpose) matrix.transpose();

            return "[Matrix3D rawData=\n" + formatRawData(sRawData2, 4, 4, precision) + "\n]";
        }

        /** Converts a Matrix instance to a String, which is useful when debugging. */
        public static function toString(matrix:Matrix, precision:int=3):String
        {
            sRawData2[0] = matrix.a; sRawData2[1] = matrix.c; sRawData2[2] = matrix.tx;
            sRawData2[3] = matrix.b; sRawData2[4] = matrix.d; sRawData2[5] = matrix.ty;

            return "[Matrix rawData=\n" + formatRawData(sRawData2, 3, 2, precision) + "\n]";
        }

        private static function formatRawData(data:Vector.<Number>, numCols:int, numRows:int,
                                              precision:int, indent:String="  "):String
        {
            var result:String = indent;
            var numValues:int = numCols * numRows;
            var highestValue:Number = 0.0;
            var valueString:String;
            var value:Number;

            for (var i:int=0; i<numValues; ++i)
            {
                value = Math.abs(data[i]);
                if (value > highestValue) highestValue = value;
            }

            var numChars:int = highestValue.toFixed(precision).length + 1;

            for (var y:int=0; y<numRows; ++y)
            {
                for (var x:int=0; x<numCols; ++x)
                {
                    value = data[numCols * y + x];
                    valueString = value.toFixed(precision);

                    while (valueString.length < numChars) valueString = " " + valueString;

                    result += valueString;
                    if (x != numCols - 1) result += ", ";
                }

                if (y != numRows - 1) result += "\n" + indent;
            }

            return result;
        }

        /** Updates the given matrix so that it points exactly to pixel boundaries. This works
         *  only if the object is unscaled and rotated by a multiple of 90 degrees.
         *
         *  @param matrix    The matrix to manipulate in place (normally the modelview matrix).
         *  @param pixelSize The size (in points) that represents one pixel in the back buffer.
         */
        public static function snapToPixels(matrix:Matrix, pixelSize:Number):void
        {
            // Snapping only makes sense if the object is unscaled and rotated only by
            // multiples of 90 degrees. If that's the case can be found out by looking
            // at the modelview matrix.

            const E:Number = 0.0001;

            var doSnap:Boolean = false;
            var aSq:Number, bSq:Number, cSq:Number, dSq:Number;

            if (matrix.b + E > 0 && matrix.b - E < 0 && matrix.c + E > 0 && matrix.c - E < 0)
            {
                // what we actually want is 'Math.abs(matrix.a)', but squaring
                // the value works just as well for our needs & is faster.

                aSq = matrix.a * matrix.a;
                dSq = matrix.d * matrix.d;
                doSnap = aSq + E > 1 && aSq - E < 1 && dSq + E > 1 && dSq - E < 1;
            }
            else if (matrix.a + E > 0 && matrix.a - E < 0 && matrix.d + E > 0 && matrix.d - E < 0)
            {
                bSq = matrix.b * matrix.b;
                cSq = matrix.c * matrix.c;
                doSnap = bSq + E > 1 && bSq - E < 1 && cSq + E > 1 && cSq - E < 1;
            }

            if (doSnap)
            {
                matrix.tx = Math.round(matrix.tx / pixelSize) * pixelSize;
                matrix.ty = Math.round(matrix.ty / pixelSize) * pixelSize;
            }
        }

        /** Creates a perspective projection matrix suitable for 2D and 3D rendering.
         *
         *  <p>The first 4 parameters define which area of the stage you want to view (the camera
         *  will 'zoom' to exactly this region). The final 3 parameters determine the perspective
         *  in which you're looking at the stage.</p>
         *
         *  <p>The stage is always on the rectangle that is spawned up between x- and y-axis (with
         *  the given size). All objects that are exactly on that rectangle (z equals zero) will be
         *  rendered in their true size, without any distortion.</p>
         *
         *  <p>If you pass only the first 4 parameters, the camera will be set up above the center
         *  of the stage, with a field of view of 1.0 rad.</p>
         */
        public static function createPerspectiveProjectionMatrix(
                x:Number, y:Number, width:Number, height:Number,
                stageWidth:Number=0, stageHeight:Number=0, cameraPos:Vector3D=null,
                out:Matrix3D=null):Matrix3D
        {
            if (out == null) out = new Matrix3D();
            if (stageWidth  <= 0) stageWidth = width;
            if (stageHeight <= 0) stageHeight = height;
            if (cameraPos == null)
            {
                cameraPos = sPoint3D;
                cameraPos.setTo(
                        stageWidth / 2, stageHeight / 2,   // -> center of stage
                        stageWidth / Math.tan(0.5) * 0.5); // -> fieldOfView = 1.0 rad
            }

            const focalLength:Number = Math.abs(cameraPos.z);
            const offsetX:Number = cameraPos.x - stageWidth  / 2;
            const offsetY:Number = cameraPos.y - stageHeight / 2;
            const far:Number    = focalLength * 20;
            const near:Number   = 1;
            const scaleX:Number = stageWidth  / width;
            const scaleY:Number = stageHeight / height;

            // set up general perspective
            sMatrixData[ 0] =  2 * focalLength / stageWidth;  // 0,0
            sMatrixData[ 5] = -2 * focalLength / stageHeight; // 1,1  [negative to invert y-axis]
            sMatrixData[10] =  far / (far - near);            // 2,2
            sMatrixData[14] = -far * near / (far - near);     // 2,3
            sMatrixData[11] =  1;                             // 3,2

            // now zoom in to visible area
            sMatrixData[0] *=  scaleX;
            sMatrixData[5] *=  scaleY;
            sMatrixData[8]  =  scaleX - 1 - 2 * scaleX * (x - offsetX) / stageWidth;
            sMatrixData[9]  = -scaleY + 1 + 2 * scaleY * (y - offsetY) / stageHeight;

            out.copyRawDataFrom(sMatrixData);
            out.prependTranslation(
                    -stageWidth /2.0 - offsetX,
                    -stageHeight/2.0 - offsetY,
                    focalLength);

            return out;
        }

        /** Creates a orthographic projection matrix suitable for 2D rendering. */
        public static function createOrthographicProjectionMatrix(
                x:Number, y:Number, width:Number, height:Number, out:Matrix=null):Matrix
        {
            if (out == null) out = new Matrix();

            out.setTo(2.0/width, 0, 0, -2.0/height,
                    -(2*x + width) / width, (2*y + height) / height);

            return out;
        }
    }
}