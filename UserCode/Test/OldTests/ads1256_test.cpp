#pragma once

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ads1256/ads1256_device.hpp"
#include "freertos_io/os_printf.h"
#include "ads1256/old/ADS1256.h"

void AdsPrintEntry(void *)
{
    extern volatile uint32_t kDrdyIRQDuration;
    while (true) {
        for (auto &var : VAds.conv_queue_) {
            os_printf("%9f, ", VAds.Data2Voltage(var.value));
        }

        os_printf("0,5,-5,%lu,%d\n", kDrdyIRQDuration, HAL_GPIO_ReadPin(VSync_GPIO_Port, VSync_Pin));
        vTaskDelay(100);
    }
}

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
        os_printf("CheckForReset: %d\n", VAds.CheckForReset());
        vTaskDelay(2000);

        os_printf("===== Read again =====\n");
        ads_reg = VAds.ReadAllRegs();
        os_printf("ADCON: %x\n", ads_reg.ADCON);
        os_printf("DRATE: %x\n", ads_reg.DRATE);
        os_printf("IO: %x\n", ads_reg.IO);
        os_printf("MUX: %x\n", ads_reg.MUX);
        os_printf("STATUS: %x\n", ads_reg.STATUS);
        os_printf("CheckForReset: %d\n", VAds.CheckForReset());
        vTaskDelay(2000);

        os_printf("===== Reset =====\n");
        VAds.Reset();
        ads_reg = VAds.ReadAllRegs();
        os_printf("ADCON: %x\n", ads_reg.ADCON);
        os_printf("DRATE: %x\n", ads_reg.DRATE);
        os_printf("IO: %x\n", ads_reg.IO);
        os_printf("MUX: %x\n", ads_reg.MUX);
        os_printf("STATUS: %x\n", ads_reg.STATUS);
        os_printf("CheckForReset: %d\n", VAds.CheckForReset());
        vTaskDelay(2000);
    }

    vTaskDelay(1000);
    // 检测 ADS 有没有连接或上电
    while (VAds.CheckForPresent() != true) {
        os_printf("==== Error. CheckForPresent failed. Retrying %s ====\n", __func__);
        VAds.Reset();
        vTaskDelay(1000);
    }

    VAds.Init();

    // vTaskDelay(2000);
    // auto ads_reg = VAds.ReadAllRegs();
    // os_printf("ADCON: %x\n", ads_reg.ADCON);
    // os_printf("DRATE: %x\n", ads_reg.DRATE);
    // os_printf("IO: %x\n", ads_reg.IO);
    // os_printf("MUX: %x\n", ads_reg.MUX);
    // os_printf("STATUS: %x\n", ads_reg.STATUS);

    // vTaskDelay(2000);
    VAds.SetConvQueue({0x08, 0x18, 0x28, 0x38});
    vTaskDelay(2000);

    xTaskCreate(AdsPrintEntry, "ads_print", 512, nullptr, PriorityNormal, nullptr);

    os_printf(">>>> StartConvQueue\n");
    VAds.StartConvQueue();
    vTaskDelay(2000);

    os_printf(">>>> StopConvQueue\n");
    VAds.StopConvQueue();
    vTaskDelay(2000);

    os_printf(">>>> SetGain\n");
    VAds.SetGain(Ads1256::PGA::Gain2);
    vTaskDelay(2000);

    os_printf(">>>> StartConvQueue\n");
    VAds.StartConvQueue();

    os_printf("==== End %s ====\n", __func__);
}
