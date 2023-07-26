#pragma once
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include <cstdio>
#include "freertos_io/os_printf.h"
#include "freertos_io/uart_device.hpp"
#include "HighPrecisionTime/high_precision_time.h"
#include "HighPrecisionTime/high_precision_time_test.h"

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
            auto val        = SysTick->VAL;
            auto tick_count = xTaskGetTickCount();
            auto load       = SysTick->LOAD;
            fprintf(stderr, "now_value < last_value! now_value:%lu, last_value:%lu, diff: %lu\n", now_value, last_value, last_value - now_value);
            fprintf(stderr, "tick_count: %lu;SysTick val: %lu, load: %lu\n", tick_count, val, load);
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

void PrecisionTest1(uint32_t us)
{
    auto last_systick = HPT_GetTotalSysTick();
    HPT_DelayUs(us);
    auto systick_duration = HPT_GetTotalSysTick() - last_systick;
    os_printf("Delayed %lu us. SysTick count: %lu, duration: %lu ns\n", us, systick_duration, HPT_SysTickToNs(systick_duration));
}

void PrecisionTest2(uint32_t us, size_t loop_time)
{
    auto last_systick = HPT_GetTotalSysTick();
    for (size_t i = 0; i < loop_time; i++) {
        HPT_DelayUs(us);
    }

    auto systick_duration = HPT_GetTotalSysTick() - last_systick;
    os_printf("Delayed %lu us for %u times. SysTick count: %lu, duration: %lu ns\n", us, loop_time, systick_duration, HPT_SysTickToNs(systick_duration));
}

} // namespace hpt_test

void HighPrecisionTimeTest()
{
    using namespace std;
    using namespace hpt_test;
    os_printf("==== Start %s ====\n", __func__);

    os_printf("Start HPT_TestGetUs...\n");
    HPT_TestGetUs(10000);
    os_printf("Start HPT_TestGetTotalSysTick...\n");
    HPT_TestGetTotalSysTick(10000);

    vTaskDelay(1);
    PrecisionTest1(1);
    vTaskDelay(1);
    PrecisionTest2(1, 10);
    vTaskDelay(1);
    PrecisionTest1(2);
    vTaskDelay(1);
    PrecisionTest2(2, 10);
    vTaskDelay(1);
    PrecisionTest1(5);
    vTaskDelay(1);
    PrecisionTest1(10);
    vTaskDelay(1);
    PrecisionTest1(20);
    vTaskDelay(1);
    PrecisionTest1(50);
    vTaskDelay(1);
    PrecisionTest1(100);
    vTaskDelay(1);
    PrecisionTest1(500);
    vTaskDelay(1);
    PrecisionTest1(1000);
    vTaskDelay(1);
    PrecisionTest1(10000);
    vTaskDelay(1);
    PrecisionTest1(100000);
    vTaskDelay(1);
    PrecisionTest1(1000000);
    vTaskDelay(1);

    os_printf("==== End %s ====\n", __func__);
}
