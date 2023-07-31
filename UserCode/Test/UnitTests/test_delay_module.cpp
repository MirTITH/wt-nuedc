#pragma once

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "freertos_io/os_printf.h"
#include "HighPrecisionTime/high_precision_time.h"
#include "control_system/delay_module.hpp"
#include "HighPrecisionTime/stat.hpp"

void TestDelayModule()
{
    os_printf("====== Start %s ======\n", __func__);
    uint32_t start_us = HPT_GetUs();

    control_system::DelayModule<float> dm(10);

    for (float i = 1.0; i <= 3.0; i += 0.1) {
        os_printf("%f %f\n", i, dm.Step(i));
    }

    dm.Init(1);
    os_printf("dm.Init(1):\n");

    for (float i = 1.0; i <= 3.0; i += 0.1) {
        os_printf("%f %f\n", i, dm.Step(i));
    }

    dm.Init(0);
    os_printf("dm.Init(0):\n");
    for (float i = 1.0; i <= 3.0; i += 0.1) {
        os_printf("%f %f\n", i, dm.Step(i));
    }

    dm.Init(100);
    uint32_t loop_duration;
    size_t loop_count = 1000000;
    {
        TimeMeter time_meter(&loop_duration);
        for (size_t i = 0; i < loop_count; i++) {
            dm.Step(i);
        }
    }
    os_printf("Speed:%f kps, loop_duration:%lu us\n", (float)loop_count / loop_duration * 1000, loop_duration);

    uint32_t duration = HPT_GetUs() - start_us;
    os_printf("====== End %s Duration: %lu us ======\n", __func__, duration);
}
