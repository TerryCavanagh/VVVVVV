How to Build
------------
VVVVVV's official desktop versions are built with the following environments:

- Windows: Visual Studio 2010
- macOS: Xcode CLT, currently targeting 10.9 SDK
- GNU/Linux: CentOS 7

The engine depends solely on [SDL2](https://libsdl.org/) and
[SDL2_mixer](https://www.libsdl.org/projects/SDL_mixer/). All other dependencies
are statically linked into the engine. The development libraries for Windows can
be downloaded from their respective websites, Linux and macOS developers should
compile and install from source.

Steamworks support is included and the DLL is loaded dynamically, you do not
need the SDK headers and there is no special Steam or non-Steam version. The
current implementation has been tested with Steamworks SDK v1.46.

To generate the projects on Windows:
```
# Put the SDL2/SDL2_mixer VC development libraries next to the VVVVVV folder!
mkdir flibitBuild
cd flibitBuild
cmake -G "Visual Studio 10 2010" .. -DSDL2_INCLUDE_DIRS="../../SDL2-2.0.10/include;../../SDL2_mixer-2.0.4/include" -DSDL2_LIBRARIES="../../SDL2-2.0.10/lib/x86/SDL2;../../SDL2-2.0.10/lib/x86/SDL2main;../../SDL2_mixer-2.0.4/lib/x86/SDL2_mixer"
```

To generate everywhere else:
```
mkdir flibitBuild
cd flibitBuild
cmake ..
```

macOS may be fussy about the SDK version. How to fix this is up to the whims of
however Apple wants to make CMAKE_OSX_SYSROOT annoying to configure and retain
each time Xcode updates.

Including data.zip
------------
You'll need the data.zip file from VVVVVV to actually run the game! It's
available to download seperately for free in the
[Make and Play](http://distractionware.com/blog/category/vvvvvv-make-and-play/)
edition of the game. Put this file next to your executable and the game should
run.

This is intended for personal use only - our license doesn't allow you to
actually distribute this data.zip file with your own forks without getting
permission from us first. See [LICENSE.md](../LICENSE.md) for more details. (If
you've got a project in mind that requires distributing this
file, [get in touch](http://distractionware.com/email/)!)

A Word About Compiler Quirks
----------------------------

This engine is _super_ fussy about optimization levels and runtime checks. In
particular, the Windows version _absolutely positively must_ be compiled in
Debug mode, with /RTC enabled. If you build in Release mode, or have /RTC
disabled, the game behaves dramatically different in ways that were never fully
documented (bizarre softlocks, out-of-bounds issues that don't show up in tools
like Valgrind, stuff like that). There are lots of things about this old code
that could be cleaned up, polished, rewritten, and so on, but this is the one
that will probably bite you the hardest when setting up your own build,
regardless of platform.

We hope you'll enjoy messing with the source anyway!

Love, flibit
