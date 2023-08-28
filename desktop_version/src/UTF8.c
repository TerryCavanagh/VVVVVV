#include "UTF8.h"

#define STARTS_0(byte) ((byte & 0x80) == 0x00)
#define STARTS_10(byte) ((byte & 0xC0) == 0x80)
#define STARTS_110(byte) ((byte & 0xE0) == 0xC0)
#define STARTS_1110(byte) ((byte & 0xF0) == 0xE0)
#define STARTS_11110(byte) ((byte & 0xF8) == 0xF0)
#define TAKE(byte, nbits) (byte & ((1 << nbits)-1))

static inline bool is_illegal(uint32_t codepoint)
{
    return (codepoint >= 0xD800 && codepoint <= 0xDFFF) || codepoint > 0x10FFFF;
}

uint32_t UTF8_peek_next(const char* s_str, uint8_t* codepoint_nbytes)
{
    /* Get the next codepoint from a string, but instead of advancing the
     * pointer, give the number of bytes the index will need to advance. */
    if (s_str == NULL)
    {
        return 0;
    }

    // Pointer conversion to avoid all those brilliant signedness plot twists...
    const unsigned char* str = (const unsigned char*) s_str;
    uint32_t codepoint;
    *codepoint_nbytes = 1;

    if (STARTS_0(str[0]))
    {
        // 0xxx xxxx - ASCII
        codepoint = str[0];
    }
    else if (STARTS_10(str[0]))
    {
        // 10xx xxxx - unexpected continuation byte
        codepoint = 0xFFFD;
    }
    else if (STARTS_110(str[0]))
    {
        // 110x xxxx - 2-byte sequence
        if (!STARTS_10(str[1]))
        {
            codepoint = 0xFFFD;
        }
        else
        {
            codepoint =
                (TAKE(str[0], 5) << 6) |
                (TAKE(str[1], 6));
            *codepoint_nbytes = 2;
        }
    }
    else if (STARTS_1110(str[0]))
    {
        // 1110 xxxx - 3-byte sequence
        if (!STARTS_10(str[1]) || !STARTS_10(str[2]))
        {
            codepoint = 0xFFFD;
        }
        else
        {
            codepoint =
                (TAKE(str[0], 4) << 12) |
                (TAKE(str[1], 6) << 6) |
                (TAKE(str[2], 6));
            *codepoint_nbytes = 3;
        }
    }
    else if (STARTS_11110(str[0]))
    {
        // 1111 0xxx - 4-byte sequence
        if (!STARTS_10(str[1]) || !STARTS_10(str[2]) || !STARTS_10(str[3]))
        {
            codepoint = 0xFFFD;
        }
        else
        {
            codepoint =
                (TAKE(str[0], 3) << 18) |
                (TAKE(str[1], 6) << 12) |
                (TAKE(str[2], 6) << 6) |
                (TAKE(str[3], 6));
            *codepoint_nbytes = 4;
        }
    }
    else
    {
        // 1111 1xxx - invalid
        codepoint = 0xFFFD;
    }

    // Overlong sequence?
    if (
        (codepoint <= 0x7F && *codepoint_nbytes > 1) ||
        (codepoint > 0x7F && codepoint <= 0x7FF && *codepoint_nbytes > 2) ||
        (codepoint > 0x7FF && codepoint <= 0xFFFF && *codepoint_nbytes > 3)
    ) {
        codepoint = 0xFFFD;
    }

    // UTF-16 surrogates are invalid, so are codepoints after 10FFFF
    if (is_illegal(codepoint))
    {
        codepoint = 0xFFFD;
    }

    return codepoint;
}

uint32_t UTF8_next(const char** p_str)
{
    /* Get the next codepoint from a string, and advance the pointer.
     * Example usage:
     *
     *  const char* str = "asdf";
     *  uint32_t codepoint;
     *  while ((codepoint = UTF8_next(&str)))
     *  {
     *      // you have a codepoint congrats
     *  }
     */
    if (p_str == NULL)
    {
        return 0;
    }

    uint8_t codepoint_nbytes;
    uint32_t codepoint = UTF8_peek_next(*p_str, &codepoint_nbytes);
    *p_str += codepoint_nbytes;
    return codepoint;
}

UTF8_encoding UTF8_encode(uint32_t codepoint)
{
    UTF8_encoding enc = {0};

    // Pretend the bytes array is unsigned...
    unsigned char* bytes = (unsigned char*) &enc.bytes;

    if (is_illegal(codepoint))
    {
        codepoint = 0xFFFD;
        enc.error = true;
    }

    if (codepoint <= 0x7F)
    {
        enc.nbytes = 1;
        bytes[0] = codepoint;
    }
    else if (codepoint <= 0x7FF)
    {
        enc.nbytes = 2;
        bytes[0] = 0xC0 | (codepoint >> 6);
        bytes[1] = 0x80 | (codepoint & 0x3F);
    }
    else if (codepoint <= 0xFFFF)
    {
        enc.nbytes = 3;
        bytes[0] = 0xE0 | (codepoint >> 12);
        bytes[1] = 0x80 | ((codepoint >> 6) & 0x3F);
        bytes[2] = 0x80 | (codepoint & 0x3F);
    }
    else
    {
        enc.nbytes = 4;
        bytes[0] = 0xF0 | (codepoint >> 18);
        bytes[1] = 0x80 | ((codepoint >> 12) & 0x3F);
        bytes[2] = 0x80 | ((codepoint >> 6) & 0x3F);
        bytes[3] = 0x80 | (codepoint & 0x3F);
    }

    return enc;
}

size_t UTF8_total_codepoints(const char* str)
{
    size_t total = 0;
    while (UTF8_next(&str))
    {
        total++;
    }
    return total;
}

size_t UTF8_backspace(const char* str, size_t len)
{
    /* Given a string of length len,
     * give the new length after removing the last character.
     * In other words, the index at which to write a \0 byte. */

    for (len -= 1; len > 0; len--)
    {
        if (!STARTS_10(str[len]))
        {
            break;
        }
    }

    return len;
}
