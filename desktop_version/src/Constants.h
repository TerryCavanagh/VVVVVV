#ifndef CONSTANTS_H
#define CONSTANTS_H

/*
 * FIXME: These should be used everywhere...
 * FIXME: This should include more constants!
 */

#define SCREEN_WIDTH_TILES 40
#define SCREEN_HEIGHT_TILES 30

#define SCREEN_WIDTH_PIXELS (SCREEN_WIDTH_TILES * 8)
#define SCREEN_HEIGHT_PIXELS (SCREEN_HEIGHT_TILES * 8)

#define TILE_IDX(x, y) (x + y * SCREEN_WIDTH_TILES)

/* 4 bytes per char, for UTF-8 encoding. */
#define SCREEN_WIDTH_CHARS (SCREEN_WIDTH_TILES * 4)

#endif /* CONSTANTS_H */
