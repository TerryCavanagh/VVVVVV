#include "DeferCallbacks.h"

#include <SDL.h>

/* Callbacks to be deferred to the end of each sequence of gamestate functions
 * in main. Useful for fixing frame-flicker glitches when doing a state
 * transition in a function that gets executed before the render function.
 *
 * We store a linked list of callbacks, to allow for the possibility of having
 * more than one callback active at a time (otherwise we could easily just
 * have a single pointer here and the header would only be 1 line and an
 * include guard) and to do it without having to allocate memory at runtime.
 */

static struct DEFER_Callback* head = NULL;

/* Add a callback. Don't call this directly; use the DEFER_CALLBACK macro. */
void DEFER_add_callback(struct DEFER_Callback* callback)
{
    struct DEFER_Callback* node;

    /* Are we adding the first node? */
    if (head == NULL)
    {
        head = callback;
        return;
    }

    /* Time to walk the linked list */
    node = head;

    if (node == callback)
    {
        goto fail;
    }

    while (node->next != NULL)
    {
        node = node->next;

        if (node == callback)
        {
            goto fail;
        }
    }

    /* We're at the end */
    node->next = callback;

    /* Success! */
    return;

fail:
    /* Having multiple instances of a callback isn't well-defined
     * and is a bit complicated to reason about */
    SDL_assert(0 && "Duplicate callback added!");
}

/* Call each callback in the list, along with deleting the entire list. */
void DEFER_execute_callbacks(void)
{
    struct DEFER_Callback* node = head;
    struct DEFER_Callback* next;

    head = NULL;

    if (node == NULL)
    {
        return;
    }

    next = node->next;
    node->func();
    node->next = NULL;

    while (next != NULL)
    {
        node = next;
        next = node->next;

        node->func();
        node->next = NULL;
    }
}
