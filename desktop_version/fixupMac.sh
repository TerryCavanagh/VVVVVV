#!/bin/bash

# flibit has this solely for convenience when cleaning up Mac binaries.

install_name_tool -change /usr/local/lib/libSDL2-2.0.0.dylib @rpath/libSDL2-2.0.0.dylib VVVVVV 
install_name_tool -change /usr/local/lib/libSDL2_mixer-2.0.0.dylib @rpath/libSDL2_mixer-2.0.0.dylib VVVVVV 
strip -S VVVVVV
otool -L VVVVVV
