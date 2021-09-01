#include <SDL_stdinc.h>
#include <stdarg.h>
#include <stdio.h>

#if defined(__unix__) || defined(__APPLE__)
 #include <unistd.h>
 #define STDOUT_IS_TTY isatty(STDOUT_FILENO)
 #define STDERR_IS_TTY isatty(STDERR_FILENO)
#elif defined(_WIN32)
 #include <io.h>
 #include <windows.h>
 #define STDOUT_IS_TTY _isatty(_fileno(stdout))
 #define STDERR_IS_TTY _isatty(_fileno(stderr))
#else
 #define STDOUT_IS_TTY 0
 #define STDERR_IS_TTY 0
#endif

#define COLOR(EXPR) color_enabled ? EXPR : ""

#define Color_RESET COLOR("\x1b[0m")
#define Color_BOLD COLOR("\x1b[1m")
#define Color_BOLD_YELLOW COLOR("\x1b[1;33m")
#define Color_BOLD_RED COLOR("\x1b[1;31m")

static int output_enabled = 1;
static int color_enabled = 0;
static int info_enabled = 1;
static int warn_enabled = 1;
static int error_enabled = 1;

void vlog_init(void)
{
#ifdef _WIN32
    OSVERSIONINFO osvi;
    SDL_zero(osvi);
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionExW(&osvi);
#endif

    if (STDOUT_IS_TTY
    && STDERR_IS_TTY
#ifdef _WIN32
    /* Windows supports ANSI escape sequences starting with Windows 10
     * build 10586. We don't care to emit them on anything lower. */
    && osvi.dwMajorVersion >= 10
    && osvi.dwBuildNumber >= 10586
#endif
    ) {
        color_enabled = 1;
    }
}

void vlog_toggle_output(const int enable_output)
{
    output_enabled = enable_output;
}

void vlog_toggle_color(const int enable_color)
{
    color_enabled = enable_color;
}

void vlog_toggle_info(const int enable_info)
{
    info_enabled = enable_info;
}

void vlog_toggle_warn(const int enable_warn)
{
    warn_enabled = enable_warn;
}

void vlog_toggle_error(const int enable_error)
{
    error_enabled = enable_error;
}

int vlog_info(const char* text, ...)
{
    va_list list;
    int retval;

    if (!output_enabled || !info_enabled)
    {
        return 0;
    }

    printf(Color_BOLD);
    printf("[INFO]");
    printf(Color_RESET);
    printf(" ");

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

    if (!output_enabled || !warn_enabled)
    {
        return 0;
    }

    fprintf(stderr, Color_BOLD_YELLOW);
    fprintf(stderr, "[WARN]");
    fprintf(stderr, Color_RESET);
    fprintf(stderr, " ");

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

    if (!output_enabled || !error_enabled)
    {
        return 0;
    }

    fprintf(stderr, Color_BOLD_RED);
    fprintf(stderr, "[ERROR]");
    fprintf(stderr, Color_RESET);
    fprintf(stderr, " ");

    va_start(list, text);
    retval = vfprintf(stderr, text, list);
    va_end(list);

    fputc('\n', stderr);

    return retval;
}
