package com.distractionware.vvvvvv;

import org.libsdl.app.SDLActivity;

public class VVVVVV extends SDLActivity {
    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
            "VVVVVV"
        };
    }

    @Override
    protected String[] getArguments() {
        // always pass -debug, logcat has level filtering anyway
        return new String[] {
            "-debug"
        };
    }
}
