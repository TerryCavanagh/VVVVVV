#ifdef SYSTEM_LODEPNG
#include <lodepng.h>
#else
/* Workaround for being unable to rename lodepng.cpp to lodepng.c */
#include "../../third_party/lodepng/lodepng.cpp"
#endif