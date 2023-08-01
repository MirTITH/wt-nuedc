#include "user_app.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "thread_priority_def.h"
#include "ads1256/ads1256_device.hpp"
#include "freertos_io/os_printf.h"
#include "Vofa/just_float.hpp"
#include "Led//led_device.hpp"
#include <atomic>
#include "Filters/butterworth.hpp"
#include "HighPrecisionTime/stat.hpp"

std::atomic<bool> kUserAppPrint = true;

static void UserAppEntry(void *argument)
{
    (void)argument;

    KeyboardLed.Power(true);
    KeyboardLed.SetColor(0, 0.02, 0);

    vTaskDelay(200);

    Butter_LP_5_20_40dB_5000Hz<float> butt;

    while (true) {
        VAds.Init(Ads1256::DataRate::SPS_7500);
        if (VAds.CheckForConfig() == true) {
            break;
        } else {
            KeyboardLed.SetColor(0.02, 0, 0);
        }
        vTaskDelay(100);
    }

    VAds.SetConvQueue({0x0f, 0x1f, 0x2f, 0x3f, 0x4f, 0x5f, 0x6f, 0x7f});
    VAds.StartConvQueue();
    // VAds.SetConvQueueCpltCallback([](Ads1256 *ads) {
    //     JFStream << ads->GetVoltage() << EndJFStream;
    // });

    uint32_t filter_duration;

    while (true) {
        auto volt = VAds.GetVoltage();
        // 0 y = 1.0018744205424700 x - 0.0080428892443834
        float average = 0;
        for (auto &var : volt) {
            var = 1.0018744205424700 * var - 0.0080428892443834;
            average += var;
        }

        average /= volt.size();

        decltype(butt.Step(average)) filter_result;

        {
            TimeMeter tm(&filter_duration);
            filter_result = butt.Step(average);
        }

        // 1 y = 0.996947 x - 0.003996
        // for (auto &var : volt) {
        //     var = 0.996947 * var - 0.003996;
        // }
        if (kUserAppPrint) {
            for (auto &var : volt) {
                os_printf("%f,", var);
            }
            os_printf("%f,%f,%lu\n", average, filter_result, filter_duration);
        }
        // auto volt = VAds.GetVoltage();
        // os_printf("%f,%d,%d,%d\n", volt[0], VAds.dma_rx_buffer_[0], VAds.dma_rx_buffer_[1], VAds.dma_rx_buffer_[2]);
        vTaskDelay(1);
    }
}

void StartUserApp()
{
    xTaskCreate(UserAppEntry, "user_app", 512, nullptr, PriorityNormal, nullptr);
}
