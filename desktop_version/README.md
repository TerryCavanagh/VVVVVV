How to Build
------------
VVVVVV's official desktop versions are built with the following environments:

- Windows: Visual Studio 2010
- macOS: Xcode CLT, currently targeting 10.9 SDK
- GNU/Linux: CentOS 7

The engine depends solely on [SDL2](https://libsdl.org/) 2.24.0+. All other dependencies
are statically linked into the engine. The development libraries for Windows can
be downloaded from SDL's website, Linux developers can find the dev libraries from their
respective repositories, and macOS developers should compile and install from source.
(If you're on Ubuntu and your Ubuntu is too old to have this SDL version, then
[see here](https://github.com/TerryCavanagh/VVVVVV/issues/618#issuecomment-968338212)
for workarounds.)

Since VVVVVV 2.4, git submodules are used for the
[third party libraries](https://github.com/TerryCavanagh/VVVVVV/tree/master/third_party).
After cloning, run `git submodule update --init` to set all of these up.
You can also use this command whenever the submodules need to be updated.

Steamworks support is included and the DLL is loaded dynamically, you do not
need the SDK headers and there is no special Steam or non-Steam version. The
current implementation has been tested with Steamworks SDK v1.46.

To build the Make and Play edition of the game, uncomment `#define MAKEANDPLAY`
in `MakeAndPlay.h`.

To generate the projects on Windows:
```
# Put your SDL2 folders somewhere nice!
mkdir build
cd build
cmake -A Win32 -G "Visual Studio 10 2010" .. -DSDL2_INCLUDE_DIRS="C:\SDL2-2.24.0\include" -DSDL2_LIBRARIES="C:\SDL2-2.24.0\lib\x86\SDL2;C:\SDL2-2.24.0\lib\x86\SDL2main"
```

Then to compile the game, open the solution and click Build.

For more detailed information and troubleshooting, see the [Compiling VVVVVV
Guide](https://vsix.dev/wiki/Guide:Compiling_VVVVVV_on_Windows_with_Visual_Studio)
on the Viki.

To generate everywhere else:
```
mkdir build
cd build
cmake ..
```

Then to compile the game, type `make`.

Including data.zip
------------
You'll need the data.zip file from VVVVVV to actually run the game! You can grab
it from your copy of the game, or you can just download it for free from the [Make
and Play](https://thelettervsixtim.es/makeandplay/) page. Put this file next to
your executable and the game should run.

This is intended for personal use only - our license doesn't allow you to
actually distribute this data.zip file with your own forks without getting
permission from us first. See [LICENSE.md](../LICENSE.md) for more details.
