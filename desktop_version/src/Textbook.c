#include "Textbook.h"

#include <SDL.h>

#include "Vlogging.h"

void textbook_init(Textbook* textbook)
{
    textbook->pages_used = 0;
    textbook->protect = false;
}

void textbook_clear(Textbook* textbook)
{
    if (textbook->protect)
    {
        return;
    }

    for (short p = 0; p < textbook->pages_used; p++)
    {
        SDL_free(textbook->page[p]);
    }
    textbook->pages_used = 0;
}

void textbook_set_protected(Textbook* textbook, bool protect)
{
    /* A protected textbook is silently not cleared when requested.
     * Not a memory leak as long as you unprotect and clear at some point. */
    textbook->protect = protect;
}

const void* textbook_store_raw(Textbook* textbook, const void* data, size_t data_len)
{
    if (data == NULL)
    {
        return NULL;
    }

    if (data_len > TEXTBOOK_PAGE_SIZE)
    {
        vlog_warn(
            "Cannot store data of %ld bytes in Textbook, max page size is %d",
            data_len,
            TEXTBOOK_PAGE_SIZE
        );
        return NULL;
    }

    /* Find a suitable page to place our text on */
    short found_page = -1;
    for (short p = 0; p < textbook->pages_used; p++)
    {
        size_t free = TEXTBOOK_PAGE_SIZE - textbook->page_len[p];

        if (data_len <= free)
        {
            found_page = p;
            break;
        }
    }

    if (found_page == -1)
    {
        /* Create a new page then */
        found_page = textbook->pages_used;

        if (found_page >= TEXTBOOK_MAX_PAGES)
        {
            vlog_warn(
                "Textbook is full! %hd pages used (%d chars per page)",
                textbook->pages_used,
                TEXTBOOK_PAGE_SIZE
            );
            return NULL;
        }

        textbook->page[found_page] = (char*) SDL_malloc(TEXTBOOK_PAGE_SIZE);
        if (textbook->page[found_page] == NULL)
        {
            return NULL;
        }

        textbook->page_len[found_page] = 0;
        textbook->pages_used++;
    }

    size_t cursor = textbook->page_len[found_page];
    char* added_text = &textbook->page[found_page][cursor];
    SDL_memcpy(added_text, data, data_len);
    textbook->page_len[found_page] += data_len;

    return added_text;
}

const char* textbook_store(Textbook* textbook, const char* text)
{
    if (text == NULL)
    {
        return NULL;
    }

    if (text[0] == '\0')
    {
        /* Don't go and store a single null terminator when we have one right here for you: */
        return "";
    }

    return textbook_store_raw(textbook, text, SDL_strlen(text)+1);
}
