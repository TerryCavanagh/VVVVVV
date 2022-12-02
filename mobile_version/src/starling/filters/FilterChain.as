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
    import starling.events.Event;
    import starling.rendering.Painter;
    import starling.textures.Texture;
    import starling.utils.Padding;

    /** The FilterChain allows you to combine several filters into one. The filters will be
     *  processed in the given order, the number of draw calls per filter adding up.
     *  Just like conventional filters, a chain may be attached to any display object.
     */
    public class FilterChain extends FragmentFilter
    {
        private var _filters:Vector.<FragmentFilter>;

        // helpers
        private static var sPadding:Padding = new Padding();

        /** Creates a new chain with the given filters. */
        public function FilterChain(...args)
        {
            _filters = new <FragmentFilter>[];

            for (var i:int = 0, len:int = args.length; i < len; ++i)
            {
                var filter:FragmentFilter = args[i] as FragmentFilter;
                if (filter) addFilterAt(filter, i);
                else throw new ArgumentError("pass only fragment filters to the constructor");
            }

            updatePadding();
            addEventListener(Event.ENTER_FRAME, onEnterFrame);
        }

        /** Disposes the filter chain itself as well as all contained filters. */
        override public function dispose():void
        {
            for each (var filter:FragmentFilter in _filters)
                filter.dispose();

            _filters.length = 0;

            super.dispose();
        }

        /** @private */
        override protected function setRequiresRedraw():void
        {
            updatePadding();
            super.setRequiresRedraw();
        }

        /** @private */
        override public function process(painter:Painter, helper:IFilterHelper,
                                         input0:Texture = null, input1:Texture = null,
                                         input2:Texture = null, input3:Texture = null):Texture
        {
            var numFilters:int = _filters.length;
            var outTexture:Texture = input0;
            var inTexture:Texture;

            for (var i:int=0; i<numFilters; ++i)
            {
                inTexture = outTexture;
                outTexture = _filters[i].process(painter, helper, inTexture);

                if (i) helper.putTexture(inTexture);
            }

            return outTexture;
        }

        /** @private */
        override public function get numPasses():int
        {
            var numPasses:int = 0;
            var numFilters:int = _filters.length;

            for (var i:int=0; i<numFilters; ++i)
                numPasses += _filters[i].numPasses;

            return numPasses;
        }

        /** Returns the filter at a certain index. If you pass a negative index,
         *  '-1' will return the last filter, '-2' the second to last filter, etc. */
        public function getFilterAt(index:int):FragmentFilter
        {
            if (index < 0) index += numFilters;
            return _filters[index];
        }

        /** Adds a filter to the chain. It will be appended at the very end. */
        public function addFilter(filter:FragmentFilter):void
        {
            addFilterAt(filter, _filters.length);
        }

        /** Adds a filter to the chain at the given index. */
        public function addFilterAt(filter:FragmentFilter, index:int):void
        {
            _filters.insertAt(index, filter);
            filter.addEventListener(Event.CHANGE, setRequiresRedraw);
            setRequiresRedraw();
        }

        /** Removes a filter from the chain. If the filter is not part of the chain,
         *  nothing happens. If requested, the filter will be disposed right away. */
        public function removeFilter(filter:FragmentFilter, dispose:Boolean=false):FragmentFilter
        {
            var filterIndex:int = getFilterIndex(filter);
            if (filterIndex != -1) removeFilterAt(filterIndex, dispose);
            return filter;
        }

        /** Removes the filter at a certain index. The indices of any subsequent filters
         *  are decremented. If requested, the filter will be disposed right away. */
        public function removeFilterAt(index:int, dispose:Boolean=false):FragmentFilter
        {
            var filter:FragmentFilter = _filters.removeAt(index) as FragmentFilter;
            filter.removeEventListener(Event.CHANGE, setRequiresRedraw);
            if (dispose) filter.dispose();
            setRequiresRedraw();
            return filter;
        }

        /** Returns the index of a filter within the chain, or "-1" if it is not found. */
        public function getFilterIndex(filter:FragmentFilter):int
        {
            return _filters.indexOf(filter);
        }

        private function updatePadding():void
        {
            sPadding.setTo();

            for each (var filter:FragmentFilter in _filters)
            {
                var padding:Padding = filter.padding;
                if (padding.left   > sPadding.left)   sPadding.left   = padding.left;
                if (padding.right  > sPadding.right)  sPadding.right  = padding.right;
                if (padding.top    > sPadding.top)    sPadding.top    = padding.top;
                if (padding.bottom > sPadding.bottom) sPadding.bottom = padding.bottom;
            }

            this.padding.copyFrom(sPadding);
        }

        private function onEnterFrame(event:Event):void
        {
            var i:int, numFilters:int = _filters.length;
            for (i=0; i<numFilters; ++i) _filters[i].dispatchEvent(event);
        }

        /** Indicates the current chain length. */
        public function get numFilters():int { return _filters.length; }
    }
}
