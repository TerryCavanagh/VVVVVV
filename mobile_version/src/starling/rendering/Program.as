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
    import com.adobe.utils.AGALMiniAssembler;

    import flash.display3D.Context3D;
    import flash.display3D.Context3DProgramType;
    import flash.display3D.Program3D;
    import flash.events.Event;
    import flash.utils.ByteArray;

    import starling.core.Starling;
    import starling.errors.MissingContextError;

    /** A Program represents a pair of a fragment- and vertex-shader.
     *
     *  <p>This class is a convenient replacement for Stage3Ds "Program3D" class. Its main
     *  advantage is that it survives a context loss; furthermore, it makes it simple to
     *  create a program from AGAL source without having to deal with the assembler.</p>
     *
     *  <p>It is recommended to store programs in Starling's "Painter" instance via the methods
     *  <code>registerProgram</code> and <code>getProgram</code>. That way, your programs may
     *  be shared among different display objects or even Starling instances.</p>
     *
     *  @see Painter
     */
    public class Program
    {
        private var _vertexShader:ByteArray;
        private var _fragmentShader:ByteArray;
        private var _program3D:Program3D;

        private static var sAssembler:AGALMiniAssembler = new AGALMiniAssembler();

        /** Creates a program from the given AGAL (Adobe Graphics Assembly Language) bytecode. */
        public function Program(vertexShader:ByteArray, fragmentShader:ByteArray)
        {
            _vertexShader = vertexShader;
            _fragmentShader = fragmentShader;

            // Handle lost context (using conventional Flash event for weak listener support)
            Starling.current.stage3D.addEventListener(Event.CONTEXT3D_CREATE,
                onContextCreated, false, 30, true);
        }

        /** Disposes the internal Program3D instance. */
        public function dispose():void
        {
            Starling.current.stage3D.removeEventListener(Event.CONTEXT3D_CREATE, onContextCreated);
            disposeProgram();
        }

        /** Creates a new Program instance from AGAL assembly language. */
        public static function fromSource(vertexShader:String, fragmentShader:String,
                                          agalVersion:uint=1):Program
        {
            return new Program(
                sAssembler.assemble(Context3DProgramType.VERTEX, vertexShader, agalVersion),
                sAssembler.assemble(Context3DProgramType.FRAGMENT, fragmentShader, agalVersion));
        }

        /** Activates the program on the given context. If you don't pass a context, the current
         *  Starling context will be used. */
        public function activate(context:Context3D=null):void
        {
            if (context == null)
            {
                context = Starling.context;
                if (context == null) throw new MissingContextError();
            }

            if (_program3D == null)
            {
                _program3D = context.createProgram();
                _program3D.upload(_vertexShader, _fragmentShader);
            }

            context.setProgram(_program3D);
        }

        private function onContextCreated(event:Event):void
        {
            disposeProgram();
        }

        private function disposeProgram():void
        {
            if (_program3D)
            {
                _program3D.dispose();
                _program3D = null;
            }
        }
    }
}
