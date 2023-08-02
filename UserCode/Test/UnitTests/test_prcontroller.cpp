#pragma once

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "freertos_io/os_printf.h"
#include "HighPrecisionTime/high_precision_time.h"
#include "control_system/pr_controller.hpp"
#include "control_system/signal_generator.hpp"
#include <cmath>
#include "Vofa/just_float.hpp"

void TestPrController()
{
    using namespace control_system;
    os_printf("====== Start %s ======\n", __func__);
    uint32_t start_us = HPT_GetUs();

    SineGenerator<float> sine(100 * M_PI, 1.0 / 5000.0);
    PRController<float> pr(1.0 / 5000.0);

    while (true) {
        auto input  = sine.Step();
        auto result = pr.Step(input);
        JFStream << input << result << EndJFStream;
        vTaskDelay(1);
    }

    uint32_t duration = HPT_GetUs() - start_us;
    os_printf("====== End %s Duration: %lu us ======\n", __func__, duration);
}
