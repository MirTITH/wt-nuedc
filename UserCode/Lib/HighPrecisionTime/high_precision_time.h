/**
 * @file micro_second.h
 * @author X. Y.
 * @brief 高精度时间库
 * @version 0.5
 * @date 2023-07-14
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化 HPT
 * @note 需要先初始化，然后才能使用其他函数
 * @note 需要在系统时钟配置完后或修改时钟配置后调用
 */
void HPT_Init();

/**
 * @brief 获取当前微秒值
 *
 * @return uint32_t 微秒值
 */
uint32_t HPT_GetUs();

/**
 * @brief 获取总 SysTick
 *
 * @note SysTick 默认情况下等于 cpu 时钟周期数，也可以配置成 cpu 时钟周期的 1/8
 * @note 这个数字增长得很快，在几十秒内会循环溢出，一般用于测量极短时间。溢出周期取决于 SysTick 速度
 */
uint32_t HPT_GetTotalSysTick();

/**
 * @brief 高精度延时
 * @note 比 vTaskDelay 和 HAL_Delay 精度高
 * @param ms 要延时的毫秒数
 */
void HPT_DelayMs(uint32_t ms);

/**
 * @brief 高精度延时
 * @param us 要延时的微秒数
 */
void HPT_DelayUs(uint32_t us);

#ifdef __cplusplus
}
#endif
