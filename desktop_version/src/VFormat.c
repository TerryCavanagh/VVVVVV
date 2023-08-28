#include "VFormat.h"

#include <SDL.h>
#include <stdbool.h>

#include "Alloc.h"
#include "ButtonGlyphs.h"
#include "CWrappers.h"
#include "UTF8.h"


static inline bool is_whitespace(char ch)
{
    return ch == ' ' || ch == '\t';
}

static inline void trim_whitespace(const char** string, size_t* bytes)
{
    /* Strips leading and trailing whitespace. */

    while (*bytes > 0 && is_whitespace((*string)[0]))
    {
        (*string)++;
        (*bytes)--;
    }
    while (*bytes > 0 && is_whitespace((*string)[*bytes - 1]))
    {
        (*bytes)--;
    }
}

int vformat_button(ActionSet actionset, int action)
{
    /* Pack an ActionSet and Action into a single vararg int.
     * action is an Action */
    return (((int) actionset) << 16) | action;
}

static void vformat_unbutton(ActionSet* actionset, Action* action, const int vararg_value)
{
    // Unpack the ActionSet and Action from a packed vararg value.
    *actionset = vararg_value >> 16;
    action->intval = vararg_value & 0xFFFF;
}

static inline void call_with_button(format_callback callback, void* userdata, int vararg_value)
{
    /* Call the given callback with the specified button character (from
     * Unicode Private Use Area) so the text renderer can display it. */

    ActionSet actionset;
    Action action;
    vformat_unbutton(&actionset, &action, vararg_value);

    const char* button_text = BUTTONGLYPHS_get_button(actionset, action, -1);
    if (button_text == NULL)
    {
        callback(userdata, "[null]", 6);
        return;
    }
    callback(userdata, button_text, SDL_strlen(button_text));
}

static inline void call_with_upper(format_callback callback, void* userdata, const char* string, size_t bytes)
{
    /* Call the given callback with the specified string, where the first
     * letter is changed to uppercase. */
    if (bytes == 0)
    {
        return;
    }

    uint8_t lower_letter_nbytes;
    uint32_t lower_letter = UTF8_peek_next(string, &lower_letter_nbytes);

    UTF8_encoding upper_letter = UTF8_encode(LOC_toupper_ch(lower_letter));
    callback(userdata, upper_letter.bytes, upper_letter.nbytes);
    callback(userdata, &string[lower_letter_nbytes], bytes - lower_letter_nbytes);
}


void vformat_cb_valist(
    format_callback callback,
    void* userdata,
    const char* format_string,
    const char* args_index,
    va_list args
)
{
    /* Variant of vformat_cb which takes a va_list instead of `...`
     *
     * Also the core formatting function where everything comes together.
     * The callback receives each part of the resulting string.
     * These parts are not null-terminated. */

    const char* cursor = format_string;

    while (true)
    {
        /* Fast path: maybe we're at the end? */
        if (cursor[0] == '\0')
        {
            return;
        }

        /* Find the next { or }. The } is only needed now for escaping. */
        const char* first_a = SDL_strchr(cursor, '{');
        const char* first_b = SDL_strchr(cursor, '}');
        const char* next_stop = first_a;
        if (next_stop == NULL || first_b < next_stop)
        {
            next_stop = first_b;
        }

        if (next_stop == NULL)
        {
            /* No more placeholders or escapes in this string, run it to the end */
            callback(userdata, cursor, SDL_strlen(cursor));
            return;
        }

        /* Output text until the placeholder... */
        callback(userdata, cursor, next_stop - cursor);
        cursor = next_stop;

        if (cursor[0] == '}')
        {
            /* Just handle }}, for symmetry with {{ */
            callback(userdata, cursor, 1);
            cursor++;
            if (*cursor == '}')
            {
                cursor++;
            }
        }
        else if (cursor[0] == '{' && cursor[1] == '{')
        {
            /* Just a {{ */
            callback(userdata, cursor, 1);
            cursor += 2;
        }
        else if (cursor[0] == '{')
        {
            /* Start of a placeholder!
             * A placeholder can consist of one or more parts separated by |,
             * the first part always being the key, all others being flags. */

            const char* placeholder_start = cursor;
            cursor++;
            const char* name = cursor;
            size_t name_len = 0;

            bool flag_wordy = false;
            int flag_digits = 0;
            bool flag_spaces = false;
            bool flag_upper = false;

            bool first_iter = true;
            do
            {
                first_a = SDL_strchr(cursor, '|');
                first_b = SDL_strchr(cursor, '}');
                next_stop = first_a;
                if (next_stop == NULL || first_b < next_stop)
                {
                    next_stop = first_b;
                }

                if (next_stop == NULL)
                {
                    /* Unterminated placeholder */
                    callback(userdata, placeholder_start, SDL_strlen(placeholder_start));
                    return;
                }

                size_t flag_len = next_stop - cursor;

                if (first_iter)
                {
                    name_len = flag_len;

                    first_iter = false;
                }
                else if (flag_len == 5 && SDL_memcmp(cursor, "wordy", 5) == 0)
                {
                    flag_wordy = true;
                }
                else if (flag_len >= 8 && SDL_memcmp(cursor, "digits=", 7) == 0)
                {
                    /* strtol stops on the first non-digit anyway, so... */
                    flag_digits = SDL_strtol(cursor + 7, NULL, 10);
                }
                else if (flag_len == 6 && SDL_memcmp(cursor, "spaces", 6) == 0)
                {
                    flag_spaces = true;
                }
                else if (flag_len == 5 && SDL_memcmp(cursor, "upper", 5) == 0)
                {
                    flag_upper = true;
                }

                cursor = next_stop + 1;
            }
            while (next_stop[0] != '}');

            /* Now look up the name in the arguments.
             * The arguments index string is comma-separated,
             * each argument is name:type. */
            va_list args_copy;
            va_copy(args_copy, args);

            const char* args_index_cursor = args_index;

            bool match = false;
            do
            {
                while (args_index_cursor[0] == ' ')
                {
                    /* Skip spaces between args */
                    args_index_cursor++;
                }

                const char* next_comma = SDL_strchr(args_index_cursor, ',');
                const char* next_colon = SDL_strchr(args_index_cursor, ':');
                if (next_comma == NULL)
                {
                    next_comma = SDL_strchr(args_index_cursor, '\0');
                }

                if (next_colon == NULL || next_colon > next_comma)
                {
                    break;
                }

                const char* arg_name = args_index_cursor;
                size_t arg_name_len = next_colon - arg_name;
                const char* arg_type = next_colon + 1;
                size_t arg_type_len = next_comma - arg_type;

                trim_whitespace(&arg_name, &arg_name_len);
                trim_whitespace(&arg_type, &arg_type_len);

                match = (arg_name_len == name_len && SDL_memcmp(arg_name, name, name_len) == 0)
                     || (arg_name_len == 1 && arg_name[0] == '_');

                if (arg_type_len == 3 && SDL_memcmp(arg_type, "int", 3) == 0)
                {
                    int value = va_arg(args_copy, int);

                    if (match)
                    {
                        if (flag_wordy)
                        {
                            char* number = HELP_number_words(value);
                            if (flag_upper)
                            {
                                call_with_upper(callback, userdata, number, SDL_strlen(number));
                            }
                            else
                            {
                                callback(userdata, number, SDL_strlen(number));
                            }
                            VVV_free(number);
                        }
                        else
                        {
                            const char* format = flag_spaces ? "%*d" : "%0*d";
                            char buffer[24];
                            SDL_snprintf(buffer, sizeof(buffer), format, flag_digits, value);
                            callback(userdata, buffer, SDL_strlen(buffer));
                        }
                    }
                }
                else if (arg_type_len == 3 && SDL_memcmp(arg_type, "str", 3) == 0)
                {
                    const char* value = va_arg(args_copy, const char*);

                    if (match)
                    {
                        if (value == NULL)
                        {
                            callback(userdata, "[null]", 6);
                        }
                        else if (flag_upper)
                        {
                            call_with_upper(callback, userdata, value, SDL_strlen(value));
                        }
                        else
                        {
                            callback(userdata, value, SDL_strlen(value));
                        }
                    }
                }
                else if (arg_type_len == 3 && SDL_memcmp(arg_type, "but", 3) == 0)
                {
                    int vararg_value = va_arg(args_copy, int);

                    if (match)
                    {
                        call_with_button(callback, userdata, vararg_value);
                    }
                }
                else
                {
                    /* Unknown type, now what type do we give va_arg? */
                    match = false;
                    break;
                }

                if (match)
                {
                    break;
                }

                /* Remember: next_comma can also be the final '\0' */
                args_index_cursor = next_comma + 1;
            }
            while (args_index_cursor[-1] != '\0');

            va_end(args_copy);

            if (!match)
            {
                callback(userdata, "[", 1);
                callback(userdata, name, name_len);
                callback(userdata, "?]", 2);
            }
        }
    }
}


void vformat_cb(
    format_callback callback,
    void* userdata,
    const char* format_string,
    const char* args_index,
    ...
)
{
    /* Format with a user-supplied callback.
     * The callback receives each part of the resulting string.
     * These parts are not null-terminated. */

    va_list args;
    va_start(args, args_index);

    vformat_cb_valist(callback, userdata, format_string, args_index, args);

    va_end(args);
}

typedef struct _buffer_info
{
    size_t total_needed;
    char* buffer_cursor;
    size_t buffer_left;
} buffer_info;

static void callback_buffer_append(void* userdata, const char* string, size_t bytes)
{
    /* Callback for vformat_buf. */

    buffer_info* buf = (buffer_info*) userdata;
    buf->total_needed += bytes;

    if (buf->buffer_cursor != NULL && buf->buffer_left > 0)
    {
        size_t copy_len = SDL_min(bytes, buf->buffer_left);

        SDL_memcpy(buf->buffer_cursor, string, copy_len);

        buf->buffer_cursor += copy_len;
        buf->buffer_left -= copy_len;
    }
}

size_t vformat_buf_valist(
    char* buffer,
    size_t buffer_len,
    const char* format_string,
    const char* args_index,
    va_list args
)
{
    /* Variant of vformat_buf which takes a va_list instead of `...` */

    buffer_info buf = {
        .total_needed = 1,
        .buffer_cursor = buffer,
        .buffer_left = buffer_len
    };

    if (buf.buffer_left != 0)
    {
        /* We do still need to write the null terminator */
        buf.buffer_left--;
    }

    vformat_cb_valist(callback_buffer_append, (void*) &buf, format_string, args_index, args);
    if (buffer != NULL && buffer_len > 0)
    {
        *buf.buffer_cursor = '\0';
    }

    return buf.total_needed;
}

size_t vformat_buf(
    char* buffer,
    size_t buffer_len,
    const char* format_string,
    const char* args_index,
    ...
)
{
    /* Format to the specified buffer.
     * Ensures buffer is not overrun, and that it is null-terminated.
     * Returns total number of bytes that were needed, or that would have been needed.
     * You may pass a NULL buffer and/or size 0 if you only need this needed length. */

    va_list args;
    va_start(args, args_index);

    size_t total_needed = vformat_buf_valist(buffer, buffer_len, format_string, args_index, args);

    va_end(args);

    return total_needed;
}

char* vformat_alloc_valist(
    const char* format_string,
    const char* args_index,
    va_list args
)
{
    /* Variant of vformat_alloc which takes a va_list instead of `...` */

    size_t needed = vformat_buf_valist(NULL, 0, format_string, args_index, args);
    char* buffer = SDL_malloc(needed);
    if (buffer == NULL)
    {
        return NULL;
    }
    vformat_buf_valist(buffer, needed, format_string, args_index, args);

    return buffer;
}

char* vformat_alloc(
    const char* format_string,
    const char* args_index,
    ...
)
{
    /* Format to an automatically allocated and resized buffer.
     * Caller must VVV_free. */

    va_list args;
    va_start(args, args_index);

    char* buffer = vformat_alloc_valist(format_string, args_index, args);

    va_end(args);

    return buffer;
}
