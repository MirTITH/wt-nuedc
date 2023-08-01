#pragma once

#include "lvgl/lvgl.h"

namespace lv_app
{
void ScreenConsole_AddText(const char *txt);

void MainPage_Init();

} // namespace lv_app

extern lv_obj_t *kTextAreaConsole;
