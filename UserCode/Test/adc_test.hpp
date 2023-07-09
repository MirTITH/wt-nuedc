#pragma once
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Adc/adc_class_device.hpp"
#include "freertos_io/os_printf.h"

namespace user_test
{
void AdcTest()
{
    os_printf("==== Start %s ====\n", __func__);

    Adc1.Init();
    Adc1.StartDma();

    int last_count = 0;
    TickType_t pxPreviousWakeTime = xTaskGetTickCount();

    int loop_period_ms = 100;

    while (true) {
        auto volts = Adc1.GetAllVoltage();
        os_printf("adc volt: ");
        for (auto &var : volts) {
            os_printf("%f\t", var);
        }

        // os_printf("temp: %f\t", GetCoreTemperature());
        auto count = Adc1.conv_cplt_count;
        os_printf("count: %lu speed: %g kps", count, (float)(count - last_count) / loop_period_ms);
        last_count = count;
        os_printf("\n");

        vTaskDelayUntil(&pxPreviousWakeTime, loop_period_ms);
    }

    os_printf("==== End %s ====\n", __func__);
}

} // namespace user_test
