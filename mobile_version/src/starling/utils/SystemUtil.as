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
    import flash.display3D.Context3D;
    import flash.events.Event;
    import flash.events.EventDispatcher;
    import flash.system.Capabilities;
    import flash.text.Font;
    import flash.text.FontStyle;
    import flash.utils.getDefinitionByName;

    import starling.errors.AbstractClassError;

    /** A utility class with methods related to the current platform and runtime. */
    public class SystemUtil
    {
        private static var sInitialized:Boolean = false;
        private static var sApplicationActive:Boolean = true;
        private static var sWaitingCalls:Array = [];
        private static var sPlatform:String;
        private static var sDesktop:Boolean;
        private static var sVersion:String;
        private static var sAIR:Boolean;
        private static var sEmbeddedFonts:Array = null;
        private static var sSupportsDepthAndStencil:Boolean = true;
        
        /** @private */
        public function SystemUtil() { throw new AbstractClassError(); }
        
        /** Initializes the <code>ACTIVATE/DEACTIVATE</code> event handlers on the native
         *  application. This method is automatically called by the Starling constructor. */
        public static function initialize():void
        {
            if (sInitialized) return;
            
            sInitialized = true;
            sPlatform = Capabilities.version.substr(0, 3);
            sVersion = Capabilities.version.substr(4);
            sDesktop = /(WIN|MAC|LNX)/.exec(sPlatform) != null;

            try
            {
                var nativeAppClass:Object = getDefinitionByName("flash.desktop::NativeApplication");
                var nativeApp:EventDispatcher = nativeAppClass["nativeApplication"] as EventDispatcher;

                nativeApp.addEventListener(Event.ACTIVATE, onActivate, false, 0, true);
                nativeApp.addEventListener(Event.DEACTIVATE, onDeactivate, false, 0, true);

                var appDescriptor:XML = nativeApp["applicationDescriptor"];
                var ns:Namespace = appDescriptor.namespace();
                var ds:String = appDescriptor.ns::initialWindow.ns::depthAndStencil.toString().toLowerCase();

                sSupportsDepthAndStencil = (ds == "true");
                sAIR = true;
            }
            catch (e:Error)
            {
                sAIR = false;
            }
        }
        
        private static function onActivate(event:Object):void
        {
            sApplicationActive = true;
            
            for each (var call:Array in sWaitingCalls)
            {
                try { call[0].apply(null, call[1]); }
                catch (e:Error)
                {
                    trace("[Starling] Error in 'executeWhenApplicationIsActive' call:", e.message);
                }
            }

            sWaitingCalls = [];
        }
        
        private static function onDeactivate(event:Object):void
        {
            sApplicationActive = false;
        }
        
        /** Executes the given function with its arguments the next time the application is active.
         *  (If it <em>is</em> active already, the call will be executed right away.) */
        public static function executeWhenApplicationIsActive(call:Function, ...args):void
        {
            initialize();
            
            if (sApplicationActive) call.apply(null, args);
            else sWaitingCalls.push([call, args]);
        }

        /** Indicates if the application is currently active. On Desktop, this means that it has
         *  the focus; on mobile, that it is in the foreground. In the Flash Plugin, always
         *  returns true. */
        public static function get isApplicationActive():Boolean
        {
            initialize();
            return sApplicationActive;
        }

        /** Indicates if the code is executed in an Adobe AIR runtime (true)
         *  or Flash plugin/projector (false). */
        public static function get isAIR():Boolean
        {
            initialize();
            return sAIR;
        }
        
        /** Indicates if the code is executed on a Desktop computer with Windows, OS X or Linux
         *  operating system. If the method returns 'false', it's probably a mobile device
         *  or a Smart TV. */
        public static function get isDesktop():Boolean
        {
            initialize();
            return sDesktop;
        }
        
        /** Returns the three-letter platform string of the current system. These are
         *  the most common platforms: <code>WIN, MAC, LNX, IOS, AND, QNX</code>. Except for the
         *  last one, which indicates "Blackberry", all should be self-explanatory. */
        public static function get platform():String
        {
            initialize();
            return sPlatform;
        }

        /** Returns the Flash Player/AIR version string. The format of the version number is:
         *  <em>majorVersion,minorVersion,buildNumber,internalBuildNumber</em>. */
        public static function get version():String
        {
            initialize();
            return sVersion;
        }

        /** Returns the value of the 'initialWindow.depthAndStencil' node of the application
         *  descriptor, if this in an AIR app; otherwise always <code>true</code>. */
        public static function get supportsDepthAndStencil():Boolean
        {
            return sSupportsDepthAndStencil;
        }

        /** Indicates if Context3D supports video textures. At the time of this writing,
         *  video textures are only supported on Windows, OS X and iOS, and only in AIR
         *  applications (not the Flash Player). */
        public static function get supportsVideoTexture():Boolean
        {
            return Context3D["supportsVideoTexture"];
        }

        /** Updates the list of embedded fonts. To be called when a font is loaded at runtime. */
        public static function updateEmbeddedFonts():void
        {
            sEmbeddedFonts = null; // will be updated in 'isEmbeddedFont()'
        }

        /** Figures out if an embedded font with the specified style is available.
         *  The fonts are enumerated only once; if you load a font at runtime, be sure to call
         *  'updateEmbeddedFonts' before calling this method.
         *
         *  @param fontName  the name of the font
         *  @param bold      indicates if the font has a bold style
         *  @param italic    indicates if the font has an italic style
         *  @param fontType  the type of the font (one of the constants defined in the FontType class)
         */
        public static function isEmbeddedFont(fontName:String, bold:Boolean=false, italic:Boolean=false,
                                              fontType:String="embedded"):Boolean
        {
            if (sEmbeddedFonts == null)
                sEmbeddedFonts = Font.enumerateFonts(false);

            for each (var font:Font in sEmbeddedFonts)
            {
                var style:String = font.fontStyle;
                var isBold:Boolean = style == FontStyle.BOLD || style == FontStyle.BOLD_ITALIC;
                var isItalic:Boolean = style == FontStyle.ITALIC || style == FontStyle.BOLD_ITALIC;

                if (fontName == font.fontName && bold == isBold && italic == isItalic &&
                    fontType == font.fontType)
                {
                    return true;
                }
            }

            return false;
        }
    }
}