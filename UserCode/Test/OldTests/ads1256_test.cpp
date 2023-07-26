#pragma once

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ads1256/ads1256_device.hpp"
#include "freertos_io/os_printf.h"
#include "control_system/pll.hpp"

control_system::Pll<float> kPll(1.0 / 3750.0);

void AdsPrintEntry(void *)
{
    while (true) {
        for (auto &voltage : VAds.GetVoltage()) {
            os_printf("%9f, ", voltage);
        }

        os_printf("%f, %f, %f\n", kPll.omega_, kPll.d_, kPll.phase_);
        vTaskDelay(1);
    }
}

void Ads1256Test()
{
    os_printf("==== Start %s ====\n", __func__);

    // 循环初始化测试
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

    os_printf(">>> 等待电源稳定\n");
    vTaskDelay(1000);

    // 检测 ADS 有没有连接或上电
    os_printf(">>> 检测 VAds 是否成功连接\n");
    while (VAds.CheckForPresent() != true) {
        os_printf("==== Error. CheckForPresent failed. Retrying\n");
        VAds.Reset();
        vTaskDelay(500);
    }

    os_printf(">>> 初始化 ADS\n");
    VAds.Init(Ads1256::DataRate::SPS_3750);

    if (VAds.CheckForConfig() == true) {
        os_printf(">>> 初始化 ADS 验证成功\n");
    } else {
        os_printf(">>> 初始化 ADS 验证失败\n");
        auto ads_reg = VAds.ReadAllRegs();
        os_printf("ADCON: %x\n", ads_reg.ADCON);
        os_printf("DRATE: %x\n", ads_reg.DRATE);
        os_printf("IO: %x\n", ads_reg.IO);
        os_printf("MUX: %x\n", ads_reg.MUX);
        os_printf("STATUS: %x\n", ads_reg.STATUS);
    }

    TaskHandle_t ads_print_task_handle;
    xTaskCreate(AdsPrintEntry, "ads_print", 512, nullptr, PriorityNormal, &ads_print_task_handle);

    // 设置转换队列完成后的回调函数
    VAds.SetConvQueueCpltCallback([](Ads1256 *ads) { kPll.Step(ads->GetVoltage(0)); });

    // os_printf(">>>> 转换队列设为只有一个元素，会自动使用连续读取\n");
    VAds.SetConvQueue({0x0f});
    VAds.StartConvQueue();

    // vTaskDelay(5000);

    // os_printf(">>>> 转换队列设为有多个元素，会切换通道读取\n");
    // VAds.StopConvQueue();
    // VAds.SetConvQueue({0x01, 0x23, 0x45, 0x67}); // 差分模式
    // VAds.StartConvQueue();
    // vTaskDelay(5000);

    // VAds.StopConvQueue();
    // VAds.SetConvQueue({0x0f, 0x1f, 0x2f, 0x3f, 0x4f, 0x5f, 0x6f, 0x7f}); // 8 通道单端
    // VAds.StartConvQueue();
    // vTaskDelay(5000);

    // os_printf(">>>> StopConvQueue\n");
    // VAds.StopConvQueue();
    // vTaskDelay(5000);

    // os_printf(">>>> SetGain\n");
    // VAds.SetGain(Ads1256::PGA::Gain2);
    // vTaskDelay(2000);

    // os_printf(">>>> StartConvQueue\n");
    // VAds.StartConvQueue();

    while (true) {
        vTaskDelay(1000);
    }

    vTaskDelete(ads_print_task_handle);

    os_printf("==== End %s ====\n", __func__);
}
