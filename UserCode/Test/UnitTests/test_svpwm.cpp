#pragma once

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "freertos_io/os_printf.h"
#include "HighPrecisionTime/high_precision_time.h"
#include "control_system/sypwm.hpp"

void TestSvpwm()
{
    os_printf("====== Start %s ======\n", __func__);
    uint32_t start_us = HPT_GetUs();

    control_system::Sypwm<float> sy;

    float phase = 0;

    while (true) {
        auto start_us = HPT_GetUs();
        sy.Calc(phase);
        auto duration = HPT_GetUs() - start_us;

        for (auto var : sy.duty_) {
            os_printf("%f,", var);
        }
        os_printf("%f,%lu\n", phase, duration);

        phase += 2 * M_PI / 360;
        if (phase >= 2 * M_PI) {
            phase -= 2 * M_PI;
        }
        vTaskDelay(10);
    }

    uint32_t duration = HPT_GetUs() - start_us;
    os_printf("====== End %s Duration: %lu us ======\n", __func__, duration);
}
