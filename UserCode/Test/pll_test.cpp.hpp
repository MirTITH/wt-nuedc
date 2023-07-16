#pragma once
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "control_system/pll.hpp"
#include "HighPrecisionTime/high_precision_time.h"
#include "tim.h"
#include "freertos_io/os_printf.h"
#include "control_system/signal_generator.hpp"
#include <stdlib.h>

extern control_system::Pll<float> pll;
extern float TimPllInput;
extern uint32_t TimDuration;

namespace user_test
{
void PllTest()
{
    os_printf("==== Start %s ====\n", __func__);

    control_system::Pll<float> pll(1.0 / 5000, 2 * M_PI * 50, 2);
    control_system::SineGenerator<float> sine(2 * M_PI * 50, 1.0 / 5000.0, M_PI);
    control_system::SineGenerator<float> sineA(2 * M_PI * 2, 1.0 / 5000.0);

    uint32_t start_us, duration;

    while (true) {
        auto input = (sineA.Step() + 1) * sine.Step();

        start_us = HPT_GetUs();
        pll.Step(input);
        duration = HPT_GetUs() - start_us;

        os_printf("%f,%f,%f,%f,%f,%lu\n", pll.d_, pll.q_, pll.omega_ / (2 * M_PI), pll.phase_, input, duration);

        vTaskDelay(1);
    }

    os_printf("==== End %s ====\n", __func__);
}

void PllTestInIsr()
{
    os_printf("==== Start %s ====\n", __func__);
    HAL_TIM_Base_Start_IT(&htim3);

    while (true) {
        os_printf("%f,%f,%f,%f,%f,%lu\n", pll.d_, pll.q_, pll.omega_ / (2 * M_PI), pll.phase_, TimPllInput, TimDuration);

        vTaskDelay(1);
    }

    os_printf("==== End %s ====\n", __func__);
}

} // namespace user_test
