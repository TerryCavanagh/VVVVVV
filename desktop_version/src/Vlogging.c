#include <SDL_stdinc.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef _WIN32
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   define STDOUT_IS_TTY 0
#   define STDERR_IS_TTY 0
#elif defined(__unix__) || defined(__APPLE__)
#   include <unistd.h>
#   define STDOUT_IS_TTY isatty(STDOUT_FILENO)
#   define STDERR_IS_TTY isatty(STDERR_FILENO)
#else
#   define STDOUT_IS_TTY 0
#   define STDERR_IS_TTY 0
#endif

#define COLOR(EXPR) (color_enabled && color_supported ? EXPR : "")

#define Color_RESET COLOR("\x1b[0m")
#define Color_BOLD COLOR("\x1b[1m")
#define Color_BOLD_YELLOW COLOR("\x1b[1;33m")
#define Color_BOLD_RED COLOR("\x1b[1;31m")
#define Color_BOLD_GRAY COLOR("\x1b[1;90m")

static int output_enabled = 1;
static int color_supported = 0;
static int color_enabled = 1;
static int debug_enabled = 0;
static int info_enabled = 1;
static int warn_enabled = 1;
static int error_enabled = 1;

static void check_color_support(void)
{
    if (STDOUT_IS_TTY && STDERR_IS_TTY)
    {
        color_supported = 1;
    }
}

void vlog_init(void)
{
    check_color_support();
}

void vlog_toggle_output(const int enable_output)
{
    output_enabled = enable_output;
}

void vlog_toggle_color(const int enable_color)
{
    color_enabled = enable_color;
}

void vlog_toggle_debug(const int enable_debug)
{
    debug_enabled = enable_debug;
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

SDL_PRINTF_VARARG_FUNC(1) void vlog_debug(const char* text, ...)
{
    va_list list;

    if (!output_enabled || !debug_enabled)
    {
        return;
    }

    printf(Color_BOLD_GRAY);
    printf("[DEBUG]");
    printf(Color_RESET);
    printf(" ");

    va_start(list, text);
    vprintf(text, list);
    va_end(list);

    putchar('\n');
}

SDL_PRINTF_VARARG_FUNC(1) void vlog_info(const char* text, ...)
{
    va_list list;

    if (!output_enabled || !info_enabled)
    {
        return;
    }

    printf(Color_BOLD);
    printf("[INFO]");
    printf(Color_RESET);
    printf(" ");

    va_start(list, text);
    vprintf(text, list);
    va_end(list);

    putchar('\n');
}

SDL_PRINTF_VARARG_FUNC(1) void vlog_warn(const char* text, ...)
{
    va_list list;

    if (!output_enabled || !warn_enabled)
    {
        return;
    }

    fprintf(stderr, Color_BOLD_YELLOW);
    fprintf(stderr, "[WARN]");
    fprintf(stderr, Color_RESET);
    fprintf(stderr, " ");

    va_start(list, text);
    vfprintf(stderr, text, list);
    va_end(list);

    fputc('\n', stderr);
}

SDL_PRINTF_VARARG_FUNC(1) void vlog_error(const char* text, ...)
{
    va_list list;

    if (!output_enabled || !error_enabled)
    {
        return;
    }

    fprintf(stderr, Color_BOLD_RED);
    fprintf(stderr, "[ERROR]");
    fprintf(stderr, Color_RESET);
    fprintf(stderr, " ");

    va_start(list, text);
    vfprintf(stderr, text, list);
    va_end(list);

    fputc('\n', stderr);
}

#ifdef _WIN32
void vlog_open_console(void)
{
    static int run_once = 0;
    if (run_once)
    {
        return;
    }
    run_once = 1;

    const BOOL success = AllocConsole();
    if (!success)
    {
        /* Debug, not error, because it might not be an error.
         * (E.g. there is already an attached console.) */
        vlog_debug(
            "Could not open console: AllocConsole() failed with %d",
            GetLastError()
        );
        return;
    }

    const FILE* handle = freopen("CON", "w", stdout);
    if (handle == NULL)
    {
        vlog_error("Could not redirect STDOUT to console.");
    }

    handle = freopen("CON", "w", stderr);
    if (handle == NULL)
    {
        vlog_error("Could not redirect STDERR to console.");
    }

    check_color_support();
}
#endif /* _WIN32 */
