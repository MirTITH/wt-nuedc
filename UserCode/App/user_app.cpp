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
#include "lv_app/screeen_console.h"
#include "line_calis.hpp"

std::atomic<bool> kUserAppPrint = true;

static TaskHandle_t UserAppEntry_handle;

static void UserAppEntry(void *argument)
{
    (void)argument;

    KeyboardLed.Power(true);
    KeyboardLed.SetColor(0, 1, 0);

    vTaskDelay(200);

    VAds.Reset(100 * 1000);
    if (VAds.CheckForPresent() == true) {
        VAds.Init(Ads1256::DataRate::SPS_7500);
        if (VAds.CheckForConfig() == true) {
            ScreenConsole_AddText("VAds.CheckForConfig() == true\n");
        } else {
            ScreenConsole_AddText("VAds.CheckForConfig() == false\n");
            KeyboardLed.SetColor(1, 0, 0);
        }
        VAds.SetConvQueue({0x01});
        VAds.StartConvQueue();

    } else {
        ScreenConsole_AddText("No VAds found!\n");
    }

    IAds.Reset(100 * 1000);
    if (IAds.CheckForPresent() == true) {
        IAds.Init(Ads1256::DataRate::SPS_7500);
        if (IAds.CheckForConfig() == true) {
            ScreenConsole_AddText("IAds.CheckForConfig() == true\n");
        } else {
            ScreenConsole_AddText("IAds.CheckForConfig() == false\n");
            KeyboardLed.SetColor(1, 0, 0);
        }

        IAds.SetConvQueue({0x01});
        IAds.StartConvQueue();
    } else {
        ScreenConsole_AddText("No IAds found!\n");
    }

    Butter_LP_5_50_20dB_5000Hz<double> butt;
    double butter_result = 0;

    // VAds.SetConvQueueCpltCallback([](Ads1256 *) {
    //     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    //     vTaskNotifyGiveFromISR(UserAppEntry_handle, &xHigherPriorityTaskWoken);
    //     portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    // });

    IAds.SetConvQueueCpltCallback([&](Ads1256 *ads) {
        butter_result = butt.Step(kLC_B_Ads_Current.Calc(ads->GetVoltage(0)));
    });

    // uint32_t butt_duration;

    while (true) {
        // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        vTaskDelay(1);

        // auto volt = VAds.GetVoltage();
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

        if (kUserAppPrint) {
            // JFStream << volt << average << filter_result << butt_    duration << EndJFStream;
            // JFStream << VAds.GetVoltage() << IAds.GetVoltage() << butter_result << EndJFStream;
            JFStream << butter_result << EndJFStream;
            // for (auto &var : volt) {
            //     os_printf("%f,", var);
            // }
            // os_printf("%f,%f,%lu\n", average, filter_result, duration);
        }
    }
}

void StartUserApp()
{
    xTaskCreate(UserAppEntry, "user_app", 1024, nullptr, PriorityNormal, &UserAppEntry_handle);
}
