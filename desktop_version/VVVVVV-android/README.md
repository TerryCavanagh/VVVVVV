Android port for C++ version
============================

This is _not_ the other existing mobile version (which is a fork of the original Flash
version). This is a port of the C++ desktop version, because the C++ version has gotten
many improvements since the source release (along with Flash being deprecated in
general).

How to Build
------------

The recommended way is to install Android Studio.

1. Place a copy of `data.zip` in `desktop_version/VVVVVV-android/app/src/main/assets/`.
   (If the `assets/` folder doesn't exist, then create it.)
2. Clone the SDL repository and place it next to where you've cloned the VVVVVV
   repository. Make sure to check out SDL 2 and not the latest as of writing (SDL 3).
3. Open the `desktop_version/VVVVVV-android/` folder in Android Studio.
4. Click 'Build'.

How to Install and Play
-----------------------

After building, go to `desktop_version/VVVVVV-android/app/build/outputs/apk/`, copy the
APK of the appropriate architecture to your device, and install it.

Touchscreen support is currently not implemented, so you will need to connect a keyboard
or controller to your device in order to play the game.
