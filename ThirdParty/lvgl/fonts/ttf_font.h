/**
 * @file ttf_font.h
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

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TTF_MEM_BASE  0x8080000                                   // 字体文件信息的地址
#define TTF_MEM_SIZE  (*(uint32_t *)TTF_MEM_BASE)                 // ttf 文件大小
#define TTF_MEM_START ((void *)(TTF_MEM_BASE + sizeof(uint32_t))) // ttf 文件起始地址

#ifdef __cplusplus
}
#endif
