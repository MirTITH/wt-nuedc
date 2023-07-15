#pragma once
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include <cstdio>
#include <cstdlib>
#include "freertos_io/os_printf.h"
#include "freertos_io/uart_device.hpp"
#include "HighPrecisionTime/high_precision_time.h"

namespace user_test
{
namespace hpt_test
{

void SteadyTest_HPT_GetUs(uint32_t test_duration, uint32_t diff_threshold = 5)
{
    os_printf("==== Start %s ====\n", __func__);
    vTaskDelay(100); // 等待打印完
    auto until_tick = HAL_GetTick() + test_duration;
    auto last_value = HPT_GetUs();
    while (true) {
        auto now_value = HPT_GetUs();
        if (now_value < last_value) {
            fprintf(stderr, "now_value < last_value! now_value:%lu, last_value:%lu\n", now_value, last_value);
            break;
        } else if (now_value - last_value > diff_threshold) {
            fprintf(stderr, "now_value - last_value > %lu! now_value:%lu, last_value:%lu\n", diff_threshold, now_value, last_value);
            break;
        }

        last_value = now_value;

        if (HAL_GetTick() >= until_tick) {
            break;
        }
    }
    os_printf("==== End %s ====\n", __func__);
}

void SteadyTest_HPT_GetTotalSysTick(uint32_t test_duration, uint32_t diff_threshold = 5 * 168)
{
    os_printf("==== Start %s ====\n", __func__);
    vTaskDelay(100); // 等待打印完
    auto until_tick = HAL_GetTick() + test_duration;
    auto last_value = HPT_GetTotalSysTick();
    while (true) {
        auto now_value = HPT_GetTotalSysTick();
        if (now_value < last_value) {
            fprintf(stderr, "now_value < last_value! now_value:%lu, last_value:%lu\n", now_value, last_value);
            break;
        } else if (now_value - last_value > diff_threshold) {
            fprintf(stderr, "now_value - last_value > %lu! now_value:%lu, last_value:%lu\n", diff_threshold, now_value, last_value);
            break;
        }

        last_value = now_value;

        if (HAL_GetTick() >= until_tick) {
            break;
        }
    }
    os_printf("==== End %s ====\n", __func__);
}

} // namespace hpt_test

void HighPrecisionTimeTest()
{
    using namespace std;
    using namespace hpt_test;
    os_printf("==== Start %s ====\n", __func__);

    SteadyTest_HPT_GetTotalSysTick(10000);
    SteadyTest_HPT_GetUs(10000);

    os_printf("==== End %s ====\n", __func__);
}

} // namespace user_test
