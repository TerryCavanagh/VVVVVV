#ifndef UTF8_H
#define UTF8_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    char bytes[5];
    uint8_t nbytes;
    bool error;
}
UTF8_encoding;


uint32_t UTF8_peek_next(const char* s_str, uint8_t* codepoint_nbytes);

uint32_t UTF8_next(const char** p_str);
UTF8_encoding UTF8_encode(uint32_t codepoint);

size_t UTF8_total_codepoints(const char* str);
size_t UTF8_backspace(const char* str, size_t len);

char* UTF8_substr(const char* str, size_t start, size_t end);
char* UTF8_erase(const char* str, size_t start, size_t end);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // UTF8_H
