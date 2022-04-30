/*
 * == VFormat ==
 *
 * VVVVVV's format strings. See the function declarations below (vformat_*).
 *  - vformat_cb       Calls a user-supplied callback function for each part of
 *                      the resulting string.
 *  - vformat_buf      Fills a user-supplied buffer with the result.
 *  - vformat_alloc    Allocates a buffer with the result (caller must SDL_free).
 *
 * All include the following parameters:
 *  - format_string     The string which needs placeholders to be filled in
 *  - args_index        A string describing the varargs
 *  - ...               The varargs (like printf-family functions)
 *
 * Variants ending in _valist are equivalent, but take a va_list instead of a variable
 * number of arguments. These use va_copy (they have to) and thus leave the given
 * va_list untouched.
 *
 * The FORMAT STRING can include placeholders, which look like {name} or {name|flags}.
 * The name can be user-defined, as long as it appears in the ARGS INDEX.
 * Flags are separated by |, invalid flags are ignored.
 *
 * The valid flags are:
 *  - wordy         [ints only] use number words (Twenty) instead of digits (20)
 *  - digits=n      [ints only] force minimum n digits, like n=5 --> 00031
 *  - spaces        [only if using digits=n] use leading spaces instead of 0s
 *  - upper         uppercase the first character with loc::toupper_ch
 *
 * For example, {aa|digits=3|spaces} fills in the argument with the name "aa".
 * Space is reserved for 3 digits, and unused columns are padded with spaces.
 * This is equivalent to %3d in printf-family functions.
 *
 * Literal { and } characters can be included in the text by doubling them,
 * like {{ and }}. Including curly braces of any kind inside placeholder tags,
 * (as part of the name or flags), is unsupported. Included string arguments
 * aren't subject to further parsing, so including curly braces in string
 * arguments poses no problems.
 *
 * The ARGS INDEX defines the names and types of the arguments you pass.
 * In the args index string, arguments are separated by commas, and each argument is
 * in the format name:type. Whitespace around the : and , separators is ignored.
 *
 * The valid types are:
 *  - int   Signed integer
 *  - str   const char*
 *  - but   Controller button icon
 *
 * Full example:
 *
 *  char buffer[100];
 *  vformat_buf(buffer, sizeof(buffer),
 *      "{crewmate} got {number} out of {total} trinkets in {m}:{s|digits=2}.{ms|digits=3}",
 *      "number:int, total:int, crewmate:str, m:int, s:int, ms:int",
 *      2, 20, "Vermilion", 2, 3, 1
 *  );
 *
 * => "Vermilion got 2 out of 20 trinkets in 2:03.001"
 *
 * ERROR CONDITIONS: These functions should not cause any crashes or UB unless you
 * mislead them on the programming side of things. For example, if your args index doesn't
 * align with the varargs you actually pass, or you pass the wrong buffer size into
 * vformat_buf. That means it should be safe to use user-generated content as a format
 * string. If the format string refers to an argument name that is not found in the index,
 * that placeholder will be filled in as [name?] (where `name` is filled in with the name).
 * If you pass a NULL string argument, it will be rendered as [null].
 */


#ifndef VFORMAT_H
#define VFORMAT_H

#include <stdarg.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*format_callback)(void* userdata, const char* string, size_t bytes);


void vformat_cb_valist(
    format_callback callback,
    void* userdata,
    const char* format_string,
    const char* args_index,
    va_list args
);

void vformat_cb(
    format_callback callback,
    void* userdata,
    const char* format_string,
    const char* args_index,
    ...
);

size_t vformat_buf_valist(
    char* buffer,
    size_t buffer_len,
    const char* format_string,
    const char* args_index,
    va_list args
);

size_t vformat_buf(
    char* buffer,
    size_t buffer_len,
    const char* format_string,
    const char* args_index,
    ...
);

char* vformat_alloc_valist(
    const char* format_string,
    const char* args_index,
    va_list args
);

char* vformat_alloc(
    const char* format_string,
    const char* args_index,
    ...
);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VFORMAT_H */
