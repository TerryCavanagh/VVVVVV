#ifndef TEXTBOOK_H
#define TEXTBOOK_H

#include <SDL_stdinc.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* The purpose of a Textbook is to store, potentially, a lot of text on a pile that shouldn't
 * go anywhere until we change languages or (for example) unload an entire level's text. */
#define TEXTBOOK_MAX_PAGES 1000
#define TEXTBOOK_PAGE_SIZE 50000
typedef struct _Textbook
{
    char* page[TEXTBOOK_MAX_PAGES];
    size_t page_len[TEXTBOOK_MAX_PAGES];

    short pages_used;
    bool protect;
} Textbook;

void textbook_init(Textbook* textbook);
void textbook_clear(Textbook* textbook);
void textbook_set_protected(Textbook* textbook, bool protect);
const void* textbook_store_raw(Textbook* textbook, const void* data, size_t data_len);
const char* textbook_store(Textbook* textbook, const char* text);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TEXTBOOK_H */
