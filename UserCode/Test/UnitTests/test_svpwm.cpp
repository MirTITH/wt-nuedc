#pragma once

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "freertos_io/os_printf.h"
#include "HighPrecisionTime/high_precision_time.h"
#include "control_system/sypwm.hpp"
#include "arm_math.h"

void TestSvpwm()
{
    os_printf("====== Start %s ======\n", __func__);
    uint32_t start_us = HPT_GetUs();

    // 指定使用 arm_cos_f32 计算余弦
    control_system::Sypwm<float, arm_cos_f32> sy;

    // 也可以不指定，默认使用 std::cos
    // control_system::Sypwm<float> sy;

    float phase = 0;

    while (true) {
        auto start_us = HPT_GetUs();
        sy.Calc(phase, 0.5); // 相位和最大占空比
        auto duration = HPT_GetUs() - start_us;

        os_printf("%f,%f,%f,", sy.duty_[0], sy.duty_[1], sy.duty_[2]);
        os_printf("%f,%lu\n", phase, duration);

        // 走完一个周期让相角归 0，防止数字过大造成精度丢失
        phase += 2 * M_PI / 360;
        if (phase >= 2 * M_PI) {
            phase -= 2 * M_PI;
        }
        vTaskDelay(10);
    }

    uint32_t duration = HPT_GetUs() - start_us;
    os_printf("====== End %s Duration: %lu us ======\n", __func__, duration);
}
