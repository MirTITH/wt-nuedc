#pragma once

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "freertos_io/os_printf.h"
#include "HighPrecisionTime/high_precision_time.h"
#include "pwm_rec/pwm_rec.hpp"
#include "Vofa/just_float.hpp"
#include "tim.h"
#include "control_system/signal_generator.hpp"
#include "control_system/pll.hpp"

void TestPwmRec()
{
    os_printf("====== Start %s ======\n", __func__);

    PwmRec<float> pwm_rec(1.0 / 5000.0, {&htim8, TIM_CHANNEL_1, &htim8, TIM_CHANNEL_2});
    pwm_rec.Init();
    control_system::SineGenerator<float, std::cos> cosine(50 * 2 * M_PI, 1.0 / 5000.0);
    // control_system::Pll<float> pll(1.0 / 5000.0);

    while (true) {
        auto input        = cosine.Step();
        uint32_t start_us = HPT_GetUs();

        pwm_rec.in_udcref_ = 5;
        pwm_rec.in_udc_    = 4.9;

        pwm_rec.in_phase_ = cosine.GetPhase();
        pwm_rec.in_us_    = 3 * input;
        pwm_rec.in_is_    = 1 * input;

        auto result = pwm_rec.Step();

        uint32_t duration = HPT_GetUs() - start_us;
        JFStream
            << result
            << pwm_rec.in_us_
            << pwm_rec.in_is_
            << pwm_rec.in_phase_ << duration << EndJFStream;
        vTaskDelay(1);
    }

    os_printf("====== End %s ======\n", __func__);
}
