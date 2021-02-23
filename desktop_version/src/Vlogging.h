#ifndef VLOGGING_H
#define VLOGGING_H

#ifdef __cplusplus
extern "C"
{
#endif

void vlog_init(void);

int vlog_info(const char* text, ...);

int vlog_warn(const char* text, ...);

int vlog_error(const char* text, ...);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VLOGGING_H */
