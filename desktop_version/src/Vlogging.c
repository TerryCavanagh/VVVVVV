#include <stdarg.h>
#include <stdio.h>

int vlog_info(const char* text, ...)
{
    va_list list;
    int retval;

    printf("[INFO] ");

    va_start(list, text);
    retval = vprintf(text, list);
    va_end(list);

    putchar('\n');

    return retval;
}

int vlog_warn(const char* text, ...)
{
    va_list list;
    int retval;

    fprintf(stderr, "[WARN] ");

    va_start(list, text);
    retval = vfprintf(stderr, text, list);
    va_end(list);

    fputc('\n', stderr);

    return retval;
}

int vlog_error(const char* text, ...)
{
    va_list list;
    int retval;

    fprintf(stderr, "[ERROR] ");

    va_start(list, text);
    retval = vfprintf(stderr, text, list);
    va_end(list);

    fputc('\n', stderr);

    return retval;
}
