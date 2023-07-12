#pragma once
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "control_system/pll.hpp"
#include "HighPrecisionTime/high_precision_time.h"
#include "tim.h"
#include "freertos_io/os_printf.h"

extern float Value;

extern control_system::Pll<float> pll;

namespace user_test
{
void PllTest()
{
    HAL_TIM_Base_Start_IT(&htim3);
    os_printf("==== Start %s ====\n", __func__);

    while (true) {
        // pll.Step(sin(HPT_GetUs() * k));
        // vTaskDelay(1);
        // os_printf("%d\n", TimCounter - last_counter);
        // last_counter = TimCounter;
        os_printf("%f,%f,%f\n", pll.d_, pll.omega_, pll.q_);
        vTaskDelay(100);
    }

    os_printf("==== End %s ====\n", __func__);
}

} // namespace user_test
