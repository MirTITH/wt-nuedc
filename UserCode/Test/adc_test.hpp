#pragma once
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Adc/adc_class_device.hpp"
#include "freertos_io/os_printf.h"

extern int AdcCpltCount;

namespace user_test
{
void AdcTest()
{
    os_printf("==== Start %s ====\n", __func__);

    Adc1.Init();
    Adc1.StartDma();

    // int last_count = 0;
    TickType_t pxPreviousWakeTime = xTaskGetTickCount();

    while (true) {
        auto volts = Adc1.GetAllVoltage();
        os_printf("%f,%f,%f,%f\n", volts.at(0), volts.at(1), volts.at(2), volts.at(3));
        // os_printf("adc volt: ");
        // for (auto &var : volts) {
        //     os_printf("%f\t", var);
        // }

        // os_printf("temp: %f\t", GetCoreTemperature());
        // os_printf("count: %d speed: %d", AdcCpltCount, AdcCpltCount - last_count);
        // last_count = AdcCpltCount;
        // os_printf("\n");

        vTaskDelayUntil(&pxPreviousWakeTime, 1);
    }

    os_printf("==== End %s ====\n", __func__);
}

} // namespace user_test
