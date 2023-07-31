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

    VAds.Init(Ads1256::DataRate::SPS_15000);
    VAds.SetConvQueue({0x0f, 0x1f, 0x2f, 0x3f, 0x4f, 0x5f, 0x6f, 0x7f});
    VAds.StartConvQueue();

    while (true) {
        auto volt = VAds.GetVoltage();
        // 0 y = 1.009981 x - 0.012726
        // for (auto &var : volt) {
        //     var = 1.009981 * var - 0.012726;
        // }

        // 1 y = 0.996947 x - 0.003996
        // for (auto &var : volt) {
        //     var = 0.996947 * var - 0.003996;
        // }

        JFStream << volt << EndJFStream;
        // auto volt = VAds.GetVoltage();
        // os_printf("%f,%d,%d,%d\n", volt[0], VAds.dma_rx_buffer_[0], VAds.dma_rx_buffer_[1], VAds.dma_rx_buffer_[2]);
        vTaskDelay(1);
    }
}

void StartUserApp()
{
    xTaskCreate(UserAppEntry, "user_app", 512, nullptr, PriorityNormal, nullptr);
}
