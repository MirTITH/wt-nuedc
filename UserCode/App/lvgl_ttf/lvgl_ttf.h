#pragma once

#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化 TTF 字体
 *
 */
void LvglTtf_Init();

/**
 * @brief 获取字体（如果初始化失败，返回 NULL）
 *
 */
lv_font_t *LvglTtf_GetFont();

#ifdef __cplusplus
}
#endif
