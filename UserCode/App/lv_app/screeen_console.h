#pragma once

#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

extern lv_obj_t *kTextAreaConsole;
void ScreenConsole_AddText(const char *txt);

#ifdef __cplusplus
}
#endif
