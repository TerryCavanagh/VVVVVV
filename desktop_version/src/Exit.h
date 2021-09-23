#ifndef VVV_EXIT_H
#define VVV_EXIT_H

#ifdef __GNUC__
#   define VVV_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
#   define VVV_NORETURN __declspec(noreturn)
#else
#   define VVV_NORETURN
#endif

VVV_NORETURN void VVV_exit(const int exit_code);

#endif /* VVV_EXIT_H */
