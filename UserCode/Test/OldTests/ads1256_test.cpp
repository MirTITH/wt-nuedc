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
        os_printf("STATUS: %x\n\n", ads_reg.STATUS);
        vTaskDelay(2000);

        while (true) {
            auto ads_reg = VAds.ReadAllRegs();
            os_printf("ADCON: %x\n", ads_reg.ADCON);
            os_printf("DRATE: %x\n", ads_reg.DRATE);
            os_printf("IO: %x\n", ads_reg.IO);
            os_printf("MUX: %x\n", ads_reg.MUX);
            os_printf("STATUS: %x\n\n", ads_reg.STATUS);
            vTaskDelay(100);
        }

        // os_printf("===== Reset =====\n");
        // VAds.Reset();
        // ads_reg = VAds.ReadAllRegs();
        // os_printf("ADCON: %x\n", ads_reg.ADCON);
        // os_printf("DRATE: %x\n", ads_reg.DRATE);
        // os_printf("IO: %x\n", ads_reg.IO);
        // os_printf("MUX: %x\n", ads_reg.MUX);
        // os_printf("STATUS: %x\n", ads_reg.STATUS);
        // vTaskDelay(2000);
    }

    // 检测 ADS 有没有连接或上电
    if (VAds.WaitForDataReady(1000 * 1000) != true) {
        os_printf("==== Error. WaitForDataReady timeout. End %s ====\n", __func__);
        return;
    }

    VAds.Init();

    auto ads_reg = VAds.ReadAllRegs();
    os_printf("ADCON: %x\n", ads_reg.ADCON);
    os_printf("DRATE: %x\n", ads_reg.DRATE);
    os_printf("IO: %x\n", ads_reg.IO);
    os_printf("MUX: %x\n", ads_reg.MUX);
    os_printf("STATUS: %x\n", ads_reg.STATUS);
    vTaskDelay(2000);

    VAds.SetDataRate(Ads1256::DataRate::SPS_15000);
    // vTaskDelay(2000);

    VAds.SetConvQueue({0x18});
    VAds.StartConvQueue();

    extern volatile uint32_t kDrdyIRQDuration;

    while (true) {
        for (auto &var : VAds.conv_queue_) {
            os_printf("%9f, ", VAds.Data2Voltage(var.value));
        }

        os_printf("0,5,-5,%lu\n", kDrdyIRQDuration);
        vTaskDelay(10);
    }

    os_printf("==== End %s ====\n", __func__);
}
