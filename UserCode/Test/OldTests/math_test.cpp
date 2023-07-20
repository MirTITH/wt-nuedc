#pragma once
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include <cmath>
#include "freertos_io/os_printf.h"
#include "HighPrecisionTime/high_precision_time.h"
#include <vector>
#include <stdlib.h>
#include "arm_math.h"

using namespace std;

void MathTest()
{
    os_printf("==== Start %s ====\n", __func__);

    size_t size = 1000;

    vector<float> inputs(size), outputs(size);

    uint32_t start_us = HPT_GetUs();

    for (size_t i = 0; i < size; i++) {
        inputs[i] = (float)i / size * M_PI;
    }

    os_printf("数据准备时间: %lu us\n", HPT_GetUs() - start_us);

    start_us = HPT_GetUs();
    for (size_t i = 0; i < size; i++) {
        outputs[i] = sin(inputs[i]);
    }
    os_printf("sin time: %lu us\n", HPT_GetUs() - start_us);

    for (size_t i = 0; i < 10; i++) {
        os_printf("sin(%g): %g\n", inputs.at(i), outputs.at(i));
    }

    start_us = HPT_GetUs();
    for (size_t i = 0; i < size; i++) {
        outputs[i] = arm_sin_f32(inputs[i]);
    }
    os_printf("arm_sin_f32 time: %lu us\n", HPT_GetUs() - start_us);

    for (size_t i = 0; i < 10; i++) {
        os_printf("sin(%g): %g\n", inputs.at(i), outputs.at(i));
    }

    float omega = 2 * M_PI * 50 / 1000.0f;

    while (true) {
        os_printf("%g\n", arm_sin_f32(xTaskGetTickCount() * omega));
        vTaskDelay(1);
    }

    os_printf("==== End %s ====\n", __func__);
}

