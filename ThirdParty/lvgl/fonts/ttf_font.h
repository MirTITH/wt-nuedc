#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t *_ttf_font_start;
extern uint8_t *_ttf_font_end;
extern size_t _ttf_font_size;

#define TTF_MEM_START ((void *)&_ttf_font_start)
#define TTF_MEM_END   ((void *)&_ttf_font_end)
#define TTF_MEM_SIZE  _ttf_font_size

#ifdef __cplusplus
}
#endif
