#pragma once

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ads1256/ads1256_device.hpp"
#include "freertos_io/os_printf.h"
#include "ads1256/old/ADS1256.h"

namespace user_test
{
void Ads1256Test()
{
    os_printf("==== Start %s ====\n", __func__);

    ADS1256_Init();

    vTaskDelay(1000);

    // Ads.Init();

    auto ads_reg = Ads.ReadAllRegs();
    // auto ads_reg = ADS1256_RREG_All();

    os_printf("ADCON: %x\n", ads_reg.ADCON);
    os_printf("DRATE: %x\n", ads_reg.DRATE);
    os_printf("IO: %x\n", ads_reg.IO);
    os_printf("MUX: %x\n", ads_reg.MUX);
    os_printf("STATUS: %x\n", ads_reg.STATUS);

    uint32_t last_drdy_count = 0;

    while (true) {
        auto drdy_count = Ads.drdy_count;
        os_printf("drdy count: %lu, speed %lu\n", drdy_count, drdy_count - last_drdy_count);
        last_drdy_count = drdy_count;
        vTaskDelay(1000);
    }

    os_printf("==== End %s ====\n", __func__);
}

} // namespace user_test
