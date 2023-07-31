#include "user_app.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "thread_priority_def.h"
#include "ads1256/ads1256_device.hpp"
#include "freertos_io/os_printf.h"
#include "Vofa/just_float.hpp"

static void UserAppEntry(void *argument)
{
    (void)argument;

    vTaskDelay(500); // 等待供电稳定

    VAds.Init(Ads1256::DataRate::SPS_7500);
    VAds.SetConvQueue({0x0f, 0x1f, 0x2f, 0x3f});
    VAds.StartConvQueue();

    while (true) {
        JFStream << VAds.GetVoltage() << EndJFStream;
        // auto volt = VAds.GetVoltage();
        // os_printf("%f,%d,%d,%d\n", volt[0], VAds.dma_rx_buffer_[0], VAds.dma_rx_buffer_[1], VAds.dma_rx_buffer_[2]);
        vTaskDelay(1);
    }
}

void StartUserApp()
{
    xTaskCreate(UserAppEntry, "user_app", 512, nullptr, PriorityNormal, nullptr);
}
