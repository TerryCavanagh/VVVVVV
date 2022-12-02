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
    import flash.geom.Point;
    import flash.geom.Vector3D;

    import starling.errors.AbstractClassError;

    /** A utility class containing methods you might need for math problems. */
    public class MathUtil
    {
        private static const TWO_PI:Number = Math.PI * 2.0;

        /** @private */
        public function MathUtil() { throw new AbstractClassError(); }

        /** Calculates the intersection point between the xy-plane and an infinite line
         *  that is defined by two 3D points in the same coordinate system. */
        public static function intersectLineWithXYPlane(pointA:Vector3D, pointB:Vector3D,
                                                        out:Point=null):Point
        {
            if (out == null) out = new Point();

            var vectorX:Number = pointB.x - pointA.x;
            var vectorY:Number = pointB.y - pointA.y;
            var vectorZ:Number = pointB.z - pointA.z;
            var lambda:Number = -pointA.z / vectorZ;

            out.x = pointA.x + lambda * vectorX;
            out.y = pointA.y + lambda * vectorY;

            return out;
        }

        /** Calculates if the point <code>p</code> is inside the triangle <code>a-b-c</code>. */
        public static function isPointInTriangle(p:Point, a:Point, b:Point, c:Point):Boolean
        {
            // This algorithm is described well in this article:
            // http://www.blackpawn.com/texts/pointinpoly/default.html

            var v0x:Number = c.x - a.x;
            var v0y:Number = c.y - a.y;
            var v1x:Number = b.x - a.x;
            var v1y:Number = b.y - a.y;
            var v2x:Number = p.x - a.x;
            var v2y:Number = p.y - a.y;

            var dot00:Number = v0x * v0x + v0y * v0y;
            var dot01:Number = v0x * v1x + v0y * v1y;
            var dot02:Number = v0x * v2x + v0y * v2y;
            var dot11:Number = v1x * v1x + v1y * v1y;
            var dot12:Number = v1x * v2x + v1y * v2y;

            var invDen:Number = 1.0 / (dot00 * dot11 - dot01 * dot01);
            var u:Number = (dot11 * dot02 - dot01 * dot12) * invDen;
            var v:Number = (dot00 * dot12 - dot01 * dot02) * invDen;

            return (u >= 0) && (v >= 0) && (u + v < 1);
        }

        /** Moves a radian angle into the range [-PI, +PI], while keeping the direction intact. */
        public static function normalizeAngle(angle:Number):Number
        {
            // move to equivalent value in range [0 deg, 360 deg] without a loop
            angle = angle % TWO_PI;

            // move to [-180 deg, +180 deg]
            if (angle < -Math.PI) angle += TWO_PI;
            if (angle >  Math.PI) angle -= TWO_PI;

            return angle;
        }

        /** Returns the next power of two that is equal to or bigger than the specified number. */
        public static function getNextPowerOfTwo(number:Number):int
        {
            if (number is int && number > 0 && (number & (number - 1)) == 0) // see: http://goo.gl/D9kPj
                return number;
            else
            {
                var result:int = 1;
                number -= 0.000000001; // avoid floating point rounding errors

                while (result < number) result <<= 1;
                return result;
            }
        }

        /** Indicates if two float (Number) values are equal, give or take <code>epsilon</code>. */
        public static function isEquivalent(a:Number, b:Number, epsilon:Number=0.0001):Boolean
        {
            return (a - epsilon < b) && (a + epsilon > b);
        }

        /** Returns the larger of the two values. Different to the native <code>Math.max</code>,
         *  this doesn't create any temporary objects when using the AOT compiler. */
        public static function max(a:Number, b:Number):Number
        {
            return a > b ? a : b;
        }

        /** Returns the smaller of the two values. Different to the native <code>Math.min</code>,
         *  this doesn't create any temporary objects when using the AOT compiler. */
        public static function min(a:Number, b:Number):Number
        {
            return a < b ? a : b;
        }

        /** Moves <code>value</code> into the range between <code>min</code> and <code>max</code>. */
        public static function clamp(value:Number, min:Number, max:Number):Number
        {
            return value < min ? min : (value > max ? max : value);
        }
    }
}