# How to Build

## Requirements

The current version of VVVVVV should be buildable with most modern compilers.
Official releases use the following:

- Windows: Visual Studio 2010 on Windows 7
- macOS: Xcode CLT, currently targeting 10.9 SDK
- GNU/Linux: Default GCC on CentOS 7

Most dependencies are vendored and don't need to be installed manually. The
following are still required:

- [SDL2](https://libsdl.org/) 2.0.14
- [SDL2_mixer](https://www.libsdl.org/projects/SDL_mixer/) 2.0.4, built with:
  - [libogg](https://xiph.org/ogg/)
  - [libvorbis](https://xiph.org/vorbis/)
  - [libvorbisfile](https://xiph.org/vorbis/)

Binaries are available for all of these on Windows. On Linux, you should be able
to get them from your distribution's repositories, though SDL2 may be outdated.
All of these should be built from source on macOS.

Steamworks support is included and the DLL is loaded dynamically, you do not
need the SDK headers and there is no special Steam or non-Steam version. The
current implementation has been tested with Steamworks SDK v1.46.

## Configuration

To generate the projects on Windows:
```
# Put your SDL2/SDL2_mixer folders somewhere nice!
mkdir flibitBuild
cd flibitBuild
cmake -G "Visual Studio 10 2010" .. -DSDL2_INCLUDE_DIRS="C:\SDL2-2.0.10\include;C:\SDL2_mixer-2.0.4\include" -DSDL2_LIBRARIES="C:\SDL2-2.0.10\lib\x86\SDL2;C:\SDL2-2.0.10\lib\x86\SDL2main;C:\SDL2_mixer-2.0.4\lib\x86\SDL2_mixer"
```

Note that on some systems, the `SDL2_LIBRARIES` list on Windows may need
SDL2/SDL2main/SDL2_mixer to have `.lib` at the end of them. The reason for this
inconsistency is unknown.

To generate everywhere else:
```
mkdir flibitBuild
cd flibitBuild
cmake ..
```

macOS may be fussy about the SDK version. How to fix this is up to the whims of
however Apple wants to make `CMAKE_OSX_SYSROOT` annoying to configure and retain
each time Xcode updates.

## Including data.zip

You'll need the data.zip file from VVVVVV to actually run the game! It's
available to download separately for free in the
[Make and Play](https://thelettervsixtim.es/makeandplay/)
edition of the game. Put this file next to your executable and the game should
run.

This is intended for personal use only - our license doesn't allow you to
actually distribute this data.zip file with your own forks without getting
permission from us first. See [LICENSE.md](../LICENSE.md) for more details. (If
you've got a project in mind that requires distributing this
file, [get in touch](http://distractionware.com/email/)!)
