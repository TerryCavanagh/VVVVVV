#ifndef VLOGGING_H
#define VLOGGING_H

#ifdef __cplusplus
extern "C"
{
#endif

void vlog_init(void);

void vlog_toggle_output(int enable_output);

void vlog_toggle_color(int enable_color);

void vlog_toggle_debug(int enable_debug);

void vlog_toggle_info(int enable_info);

void vlog_toggle_warn(int enable_warn);

void vlog_toggle_error(int enable_error);

int vlog_debug(const char* text, ...);

int vlog_info(const char* text, ...);

int vlog_warn(const char* text, ...);

int vlog_error(const char* text, ...);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VLOGGING_H */
