/*
 * This code provides a glue layer between PhysicsFS and Simple Directmedia
 *  Layer's (SDL) RWops i/o abstraction.
 *
 * License: this code is public domain. I make no warranty that it is useful,
 *  correct, harmless, or environmentally safe.
 *
 * This particular file may be used however you like, including copying it
 *  verbatim into a closed-source project, exploiting it commercially, and
 *  removing any trace of my name from the source (although I hope you won't
 *  do that). I welcome enhancements and corrections to this file, but I do
 *  not require you to send me patches if you make changes. This code has
 *  NO WARRANTY.
 *
 * Unless otherwise stated, the rest of PhysicsFS falls under the zlib license.
 *  Please see LICENSE.txt in the root of the source tree.
 *
 * SDL 1.2 falls under the LGPL license. SDL 1.3+ is zlib, like PhysicsFS.
 *  You can get SDL at https://www.libsdl.org/
 *
 *  This file was written by Ryan C. Gordon. (icculus@icculus.org).
 */

#ifndef _INCLUDE_PHYSFSRWOPS_H_
#define _INCLUDE_PHYSFSRWOPS_H_

#include "physfs.h"
#include "SDL2/SDL.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Open a platform-independent filename for reading, and make it accessible
 *  via an SDL_RWops structure. The file will be closed in PhysicsFS when the
 *  RWops is closed. PhysicsFS should be configured to your liking before
 *  opening files through this method.
 *
 *   @param filename File to open in platform-independent notation.
 *  @return A valid SDL_RWops structure on success, NULL on error. Specifics
 *           of the error can be gleaned from PHYSFS_getLastError().
 */
PHYSFS_DECL SDL_RWops *PHYSFSRWOPS_openRead(const char *fname);

/**
 * Open a platform-independent filename for writing, and make it accessible
 *  via an SDL_RWops structure. The file will be closed in PhysicsFS when the
 *  RWops is closed. PhysicsFS should be configured to your liking before
 *  opening files through this method.
 *
 *   @param filename File to open in platform-independent notation.
 *  @return A valid SDL_RWops structure on success, NULL on error. Specifics
 *           of the error can be gleaned from PHYSFS_getLastError().
 */
PHYSFS_DECL SDL_RWops *PHYSFSRWOPS_openWrite(const char *fname);

/**
 * Open a platform-independent filename for appending, and make it accessible
 *  via an SDL_RWops structure. The file will be closed in PhysicsFS when the
 *  RWops is closed. PhysicsFS should be configured to your liking before
 *  opening files through this method.
 *
 *   @param filename File to open in platform-independent notation.
 *  @return A valid SDL_RWops structure on success, NULL on error. Specifics
 *           of the error can be gleaned from PHYSFS_getLastError().
 */
PHYSFS_DECL SDL_RWops *PHYSFSRWOPS_openAppend(const char *fname);

/**
 * Make a SDL_RWops from an existing PhysicsFS file handle. You should
 *  dispose of any references to the handle after successful creation of
 *  the RWops. The actual PhysicsFS handle will be destroyed when the
 *  RWops is closed.
 *
 *   @param handle a valid PhysicsFS file handle.
 *  @return A valid SDL_RWops structure on success, NULL on error. Specifics
 *           of the error can be gleaned from PHYSFS_getLastError().
 */
PHYSFS_DECL SDL_RWops *PHYSFSRWOPS_makeRWops(PHYSFS_File *handle);

#ifdef __cplusplus
}
#endif

#endif /* include-once blocker */

/* end of physfsrwops.h ... */

