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
#include "lv_app/main_page.hpp"

std::atomic<bool> kUserAppPrint = true;

static TaskHandle_t UserAppEntry_handle;

static void UserAppEntry(void *argument)
{
    (void)argument;

    KeyboardLed.Power(true);
    KeyboardLed.SetColor(0, 1, 0);

    vTaskDelay(200);

    // Butter_LP_5_50_20dB_3000Hz<double> butt;

    while (true) {
        VAds.Init(Ads1256::DataRate::SPS_7500);
        if (VAds.CheckForConfig() == true) {
            lv_app::ScreenConsole_AddText("VAds.CheckForConfig() == true\n");
            break;
        } else {
            KeyboardLed.SetColor(1, 0, 0);
        }
        vTaskDelay(100);
    }

    while (true) {
        IAds.Init(Ads1256::DataRate::SPS_7500);
        if (IAds.CheckForConfig() == true) {
            lv_app::ScreenConsole_AddText("IAds.CheckForConfig() == true\n");
            break;
        } else {
            KeyboardLed.SetColor(0, 0, 1);
        }
        vTaskDelay(100);
    }

    // VAds.SetConvQueueCpltCallback([](Ads1256 *) {
    //     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    //     vTaskNotifyGiveFromISR(UserAppEntry_handle, &xHigherPriorityTaskWoken);
    //     portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    // });

    VAds.SetConvQueue({0x0f, 0x1f, 0x2f, 0x3f, 0x4f, 0x5f, 0x6f, 0x7f});
    VAds.StartConvQueue();

    IAds.SetConvQueue({0x0f, 0x1f, 0x2f, 0x3f, 0x4f, 0x5f, 0x6f, 0x7f});
    IAds.StartConvQueue();
    // VAds.SetConvQueueCpltCallback([](Ads1256 *ads) {
    //     JFStream << ads->GetVoltage() << EndJFStream;
    // });

    // uint32_t butt_duration;

    while (true) {
        // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        vTaskDelay(1);
        auto volt = VAds.GetVoltage();

        // float average = 0;
        // for (auto &var : volt) {
        //     average += var;
        // }

        // average /= volt.size();

        // decltype(butt.Step(average)) filter_result;

        // {
        //     TimeMeter tm(&butt_duration);
        //     filter_result = butt.Step(average);
        // }

        extern uint32_t kIAdsDrdyDuration;

        if (kUserAppPrint) {
            // JFStream << volt << average << filter_result << butt_duration << EndJFStream;
            JFStream << VAds.GetVoltage() << IAds.GetVoltage() << kIAdsDrdyDuration << EndJFStream;
            // for (auto &var : volt) {
            //     os_printf("%f,", var);
            // }
            // os_printf("%f,%f,%lu\n", average, filter_result, duration);
        }
    }
}

void StartUserApp()
{
    xTaskCreate(UserAppEntry, "user_app", 512, nullptr, PriorityNormal, &UserAppEntry_handle);
}
