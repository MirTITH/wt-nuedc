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
#include "HighPrecisionTime/stat.hpp"

void TestPrController()
{
    using namespace control_system;
    os_printf("====== Start %s ======\n", __func__);

    SineGenerator<float> sine(100 * M_PI, 1.0 / 5000.0);
    PRController<float> pr(1.0 / 5000.0);

    uint32_t duration;

    TimeMeter tm(&duration);

    while (true) {
        auto input = sine.Step();
        tm.StartMeasure();
        auto result = pr.Step(input);
        tm.EndMeasure();
        JFStream << input << result << duration << EndJFStream;
        vTaskDelay(1);
    }

    os_printf("====== End %s ======\n", __func__);
}
