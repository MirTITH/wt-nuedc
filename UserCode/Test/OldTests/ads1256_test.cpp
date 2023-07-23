#pragma once

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ads1256/ads1256_device.hpp"
#include "freertos_io/os_printf.h"
#include "ads1256/old/ADS1256.h"

void Ads1256Test()
{
    os_printf("==== Start %s ====\n", __func__);

    while (false) {
        os_printf("===== Init =====\n");
        VAds.Init();
        auto ads_reg = VAds.ReadAllRegs();
        os_printf("ADCON: %x\n", ads_reg.ADCON);
        os_printf("DRATE: %x\n", ads_reg.DRATE);
        os_printf("IO: %x\n", ads_reg.IO);
        os_printf("MUX: %x\n", ads_reg.MUX);
        os_printf("STATUS: %x\n", ads_reg.STATUS);
        vTaskDelay(1000);

        os_printf("===== Reset =====\n");
        VAds.Reset();
        ads_reg = VAds.ReadAllRegs();
        os_printf("ADCON: %x\n", ads_reg.ADCON);
        os_printf("DRATE: %x\n", ads_reg.DRATE);
        os_printf("IO: %x\n", ads_reg.IO);
        os_printf("MUX: %x\n", ads_reg.MUX);
        os_printf("STATUS: %x\n", ads_reg.STATUS);
        vTaskDelay(1000);
    }

    VAds.Init();
    // vTaskDelay(1000);
    // VAds.Init();
    // vTaskDelay(1000);

    // auto ads_reg = VAds.ReadAllRegs();
    // os_printf("ADCON: %x\n", ads_reg.ADCON);
    // os_printf("DRATE: %x\n", ads_reg.DRATE);
    // os_printf("IO: %x\n", ads_reg.IO);
    // os_printf("MUX: %x\n", ads_reg.MUX);
    // os_printf("STATUS: %x\n", ads_reg.STATUS);
    // vTaskDelay(1000);

    VAds.SetConvQueue({0x01, 0x18});
    VAds.StartConvQueue();

    while (true) {
        os_printf("%10f, %10f, 0, 5, -5\n",
                  VAds.Data2Voltage(VAds.conv_queue_.at(0).value),
                  VAds.Data2Voltage(VAds.conv_queue_.at(1).value));
        // os_printf("%10f, 0, 5, -5\n", VAds.Data2Voltage(VAds.ReadData()));
        vTaskDelay(10);
    }

    os_printf("==== End %s ====\n", __func__);
}
