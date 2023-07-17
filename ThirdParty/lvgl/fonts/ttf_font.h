#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t *_ttf_font_start;
extern uint8_t *_ttf_font_end;
extern size_t _ttf_font_size;

#define TTF_MEM_START ((void *)0x8080000)
// #define TTF_MEM_END   ((void *)&_ttf_font_end)
#define TTF_MEM_SIZE  485220

#ifdef __cplusplus
}
#endif
