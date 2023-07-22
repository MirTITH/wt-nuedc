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

    // ADS1256_Init();

    VAds.Init();

    auto ads_reg = VAds.ReadAllRegs();
    os_printf("ADCON: %x\n", ads_reg.ADCON);
    os_printf("DRATE: %x\n", ads_reg.DRATE);
    os_printf("IO: %x\n", ads_reg.IO);
    os_printf("MUX: %x\n", ads_reg.MUX);
    os_printf("STATUS: %x\n", ads_reg.STATUS);
    vTaskDelay(1000);

    uint32_t last_drdy_count = 0;
    while (true) {
        auto drdy_count = VAds.drdy_count;
        os_printf("%f, %lu, 5, -5\n", VAds.Data2Voltage(VAds.ReadData()), drdy_count - last_drdy_count);
        last_drdy_count = drdy_count;
        vTaskDelay(10);
    }

    os_printf("==== End %s ====\n", __func__);
}
