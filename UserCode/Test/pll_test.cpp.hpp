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

extern int TimCounter;

extern control_system::Pll<float> pll;

namespace user_test
{
void PllTest()
{
    HAL_TIM_Base_Start_IT(&htim3);
    os_printf("==== Start %s ====\n", __func__);

    // int last_counter = 0;

    control_system::Pll<float> pll(1.0 / 5000);
    control_system::SineGenerator<float> sine(2 * M_PI * 50, 1.0 / 5000.0, 0);
    // control_system::SineGenerator<float> sine2(2 * M_PI * 100, 1.0 / 5000.0, 0);
    // control_system::SineGenerator<float> sine4(2 * M_PI * 200, 1.0 / 5000.0, 0);

    uint32_t start_us, duration;

    while (true) {
        auto input = sine.Step() + ((float)rand() / RAND_MAX + (float)rand() / RAND_MAX + (float)rand() / RAND_MAX + (float)rand() / RAND_MAX) / 10;
        start_us   = HPT_GetUs();
        pll.Step(input);
        duration = HPT_GetUs() - start_us;
        // vTaskDelay(1);
        // auto now_counter = TimCounter;
        os_printf("%f,%f,%f,%f,%f,%lu\n", pll.d_, pll.q_, pll.omega_ / (2 * M_PI), pll.phase_, input, duration);
        // last_counter = now_counter;

        // sine_generator.SetOmega(2 * M_PI);
        // for (size_t i = 0; i < 78; i++) {
        //     os_printf("%f\n", sine_generator.Step());
        //     vTaskDelay(10);
        // }

        // sine_generator.SetOmega(2 * M_PI * 2.65);
        // for (size_t i = 0; i < 189; i++) {
        //     os_printf("%f\n", sine_generator.Step());
        //     vTaskDelay(10);
        // }

        vTaskDelay(10);
    }

    os_printf("==== End %s ====\n", __func__);
}

} // namespace user_test
