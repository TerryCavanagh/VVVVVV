#ifndef CWRAPPERS_H
#define CWRAPPERS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

char* HELP_number_words(int _t, const char* number_class);
uint32_t LOC_toupper_ch(uint32_t ch);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CWRAPPERS_H */
