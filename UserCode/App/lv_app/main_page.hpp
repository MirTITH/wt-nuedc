#pragma once

#include "lvgl/lvgl.h"

void ScreenConsole_AddText(const char *txt);

void MainPage_Init();

extern lv_obj_t *kTextAreaConsole;
