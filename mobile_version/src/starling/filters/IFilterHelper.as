// =================================================================================================
//
//	Starling Framework
//	Copyright Gamua GmbH. All Rights Reserved.
//
//	This program is free software. You can redistribute and/or modify it
//	in accordance with the terms of the accompanying license agreement.
//
// =================================================================================================

package starling.filters
{
    import flash.geom.Rectangle;

    import starling.display.DisplayObject;
    import starling.textures.Texture;

    /** An interface describing the methods available on the <code>helper</code> object passed
     *  to the <code>process</code> call of fragment filters. It provides information about the
     *  target object and methods to acquire and release pass textures.
     *
     *  @see FragmentFilter#process()
     */
    public interface IFilterHelper
    {
        /** Gets a pass texture from the pool, or creates a new one (cleared and ready to be used
         *  as render target). Its size is dictated by the bounds of the target display object
         *  plus padding.
         *
         *  <p>Beware: each call of this method counts as one render pass. For performance reasons,
         *  the filter may be configured to render the last pass directly to the back buffer. In
         *  that case, this method will return <code>null</code> for the last pass! That's the
         *  sign for the <code>process</code> method to draw to the back buffer. If you receive
         *  <code>null</code> too soon, the filter class probably didn't correctly override
         *  <code>numPasses</code>.</p>
         */
        function getTexture(resolution:Number=1.0):Texture;

        /** Puts a texture back into the pool to be reused later (or to be disposed
         *  with the pool). */
        function putTexture(texture:Texture):void;

        /** The bounds of the target object (plus padding) in stage coordinates. */
        function get targetBounds():Rectangle;

        /** The display object the filter is currently attached to. */
        function get target():DisplayObject;
    }
}
