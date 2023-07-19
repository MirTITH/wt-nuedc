/**
 * @file lvgl_ttf.h
 * @author X. Y.
 * @brief 用于加载 Flash 中的 ttf 字体文件
 * @version 0.1
 * @date 2023-07-18
 *
 * @copyright Copyright (c) 2023
 *
 * 需要先使用 Script 文件夹中的脚本将字体文件烧写到 TTF_MEM_BASE 中
 * 字体文件信息包括两部分：从 TTF_MEM_BASE 开始，前 4 字节是 ttf 的大小，之后是 ttf 文件本体
 */

#pragma once

#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化 TTF 字体
 *
 */
void LvglTTF_Init();

/**
 * @brief 获取字体
 *
 */
lv_font_t *LvglTTF_GetFont();

/**
 * @brief 获取大字体
 *
 */
lv_font_t *LvglTTF_GetLargeFont();

/**
 * @brief 获取小字体
 *
 */
lv_font_t *LvglTTF_GetSmallFont();

#ifdef __cplusplus
}
#endif
