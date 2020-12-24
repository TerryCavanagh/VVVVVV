#ifndef DEFERCALLBACKS_H
#define DEFERCALLBACKS_H

#ifdef __cplusplus
extern "C"
{
#endif

struct DEFER_Callback
{
    void (*func)(void);
    struct DEFER_Callback* next;
};

void DEFER_add_callback(struct DEFER_Callback* callback);

void DEFER_execute_callbacks(void);

#define DEFER_CALLBACK(FUNC) \
    do \
    { \
        static struct DEFER_Callback callback = {FUNC, NULL}; \
        \
        DEFER_add_callback(&callback); \
    } while (0)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* DEFERCALLBACKS_H */
