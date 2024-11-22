#ifndef CONSTANTS_H
#define CONSTANTS_H

/*
 * FIXME: These should be used everywhere...
 * FIXME: This should include more constants!
 */

#define NO_DATA_ZIP "Error: data.zip missing!\nYou do not have data.zip!\nGrab it from your purchased copy of the game, or get it from the free Make and Play Edition.\n\nIf you are getting this error, and you do\n have data.zip, move it into the folder\ncontaining the executable.\n");
#define SCREEN_WIDTH_TILES 40
#define SCREEN_HEIGHT_TILES 30

#define SCREEN_WIDTH_PIXELS (SCREEN_WIDTH_TILES * 8)
#define SCREEN_HEIGHT_PIXELS (SCREEN_HEIGHT_TILES * 8)

#define TILE_IDX(x, y) (x + y * SCREEN_WIDTH_TILES)

/* 4 bytes per char, for UTF-8 encoding. */
#define SCREEN_WIDTH_CHARS (SCREEN_WIDTH_TILES * 4)

#endif /* CONSTANTS_H */
