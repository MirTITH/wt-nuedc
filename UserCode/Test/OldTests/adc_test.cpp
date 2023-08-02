#pragma once
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Adc/adc_class_device.hpp"
#include "freertos_io/os_printf.h"
#include "dac.h"

void AdcTest()
{
    os_printf("==== Start %s ====\n", __func__);

    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);

    // int dac_value = 0;

    Adc1.Init();
    Adc1.StartDma();

    // int last_count                = 0;
    // TickType_t pxPreviousWakeTime = xTaskGetTickCount();

    // int loop_period_ms = 10;

    // while (true) {

    // dac_value += 1;
    // if (dac_value >= 4096) {
    //     dac_value = 0;
    // }
    // if (dac_value <= 2048) {

    //     auto volts = Adc1.GetAllVoltage();
    //     os_printf("%f,%f\n", dac_value * (3.3 / 4095.0), volts.at(3));
    // } else {
    //     auto volts = Adc1.GetAllVoltage();
    //     os_printf("%f,%f\n", 2048 * (3.3 / 4095.0), volts.at(3));
    // }

    // os_printf("dac value: %-4d ", dac_value);

    // os_printf("adc volt: ");
    // for (auto &var : volts) {
    //     os_printf("%-7.5f ", var);
    // }

    // os_printf("temp: %f\t", GetCoreTemperature());
    // auto count = Adc1.conv_cplt_count;
    // os_printf("count: %lu speed: %g kps", count, (float)(count - last_count) / loop_period_ms);
    // last_count = count;
    // os_printf("\n");

    //     vTaskDelayUntil(&pxPreviousWakeTime, loop_period_ms);
    // }

    os_printf("==== End %s ====\n", __func__);
}
