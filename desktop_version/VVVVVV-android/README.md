Android port for C++ version
============================

This is _not_ the other existing mobile version (which is a fork of the original Flash
version). This is a port of the C++ desktop version, because the C++ version has gotten
many improvements since the source release (along with Flash being deprecated in
general).

How to Build
------------

The recommended way is to install Android Studio and Maven. These instructions are for
SDL 2.28.5; adapt your SDL version accordingly.

1. Place a copy of `data.zip` in `desktop_version/VVVVVV-android/app/src/main/assets/`.
   (If the `assets/` folder doesn't exist, then create it.)
2. Obtain the SDL 2.28.5 Maven package. As of writing, SDL currently does not publish
   Maven packages, so here is one way to obtain them (other methods are possible):

   1. Download the SDL 2.28.5 source code.
   2. Run the `build-scripts/android-prefab.sh` script in the SDL repository.
   3. After building, run `mvn install:install-file
      -Dfile=build-android-prefab/prefab-2.28.5/SDL2-2.28.5.aar
      -DpomFile=build-android-prefab/prefab-2.28.5/SDL2-2.28.5.pom` to install it to
      Maven Local.

3. Open the `desktop_version/VVVVVV-android/` folder in Android Studio.
4. Click 'Build'.

How to Install and Play
-----------------------

After building, go to `desktop_version/VVVVVV-android/app/build/outputs/apk/`, copy the
APK of the appropriate architecture to your device, and install it.

Touchscreen support is currently not implemented, so you will need to connect a keyboard
or controller to your device in order to play the game.
