#include "ads1256_device.hpp"
#include "lv_app/screeen_console.h"
#include "Led/led_device.hpp"
#include "Relay/relay_device.hpp"
#include "WatchDog/watchdog.hpp"
#include "line_calis.hpp"
#include "freertos_io/os_printf.h"
#include "freertos_io/uart_device.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "states.hpp"
#include <string>

Ads1256 VAds(&hspi3,
             VDrdy_GPIO_Port, VDrdy_Pin,
             nullptr, 0,
             VSync_GPIO_Port, VSync_Pin,
             VAds_nCs_GPIO_Port, VAds_nCs_Pin);

Ads1256 IAds(&hspi2,
             IDrdy_GPIO_Port, IDrdy_Pin,
             nullptr, 0,
             ISync_GPIO_Port, ISync_Pin,
             IAds_nCs_GPIO_Port, IAds_nCs_Pin);

// static Butter_LP_5_50_20dB_5000Hz<double> kIAdsFilter;
// std::atomic<float> kIAdsFilterResult = 0;
std::atomic<float> kIAdsCaliResult = 0;

static Butter_LP_5_50_20dB_5000Hz<double> kVAdsFilter;
std::atomic<float> kVAdsFilterResult = 0;
std::atomic<float> kVAdsCaliResult   = 0;

WatchDog kIAdsWatchDog([](void *) {
    KeyboardLed.SetColor(10, 10, 0);
    kAppState.SwitchTo(AppState_t::Stop);
    std::string str = std::string("IAdsWatchDog! Current: ") + std::to_string(kIAdsCaliResult.load()).append("\n");
    ScreenConsole_AddText(str.c_str());
},
                       10);

WatchDog kVAdsWatchDog([](void *) {
    KeyboardLed.SetColor(10, 0, 10);
    kAppState.SwitchTo(AppState_t::Stop);
    std::string str = std::string("VAdsWatchDog! Voltage: ") + std::to_string(kVAdsCaliResult.load()).append("\n");
    ScreenConsole_AddText(str.c_str());
},
                       10);

void InitAds()
{
    // VAds
    VAds.Reset(100 * 1000);
    if (VAds.CheckForPresent() == true) {
        VAds.Init(Ads1256::DataRate::SPS_7500);
        if (VAds.CheckForConfig() == true) {
            ScreenConsole_AddText("VAds.CheckForConfig() == true\n");
            VAds.SetConvQueue({0x01});
            VAds.StartConvQueue();
        } else {
            ScreenConsole_AddText("VAds.CheckForConfig() == false\n");
            KeyboardLed.SetColor(1, 0, 0);
        }
    } else {
        ScreenConsole_AddText("No VAds found!\n");
    }

    // IAds
    IAds.Reset(100 * 1000);
    if (IAds.CheckForPresent() == true) {
        IAds.Init(Ads1256::DataRate::SPS_7500);
        if (IAds.CheckForConfig() == true) {
            ScreenConsole_AddText("IAds.CheckForConfig() == true\n");
            IAds.SetConvQueue({0x01});
            IAds.StartConvQueue();
        } else {
            ScreenConsole_AddText("IAds.CheckForConfig() == false\n");
            KeyboardLed.SetColor(1, 0, 0);
        }
    } else {
        ScreenConsole_AddText("No IAds found!\n");
    }

    vTaskDelay(100);

    // VAds Callback
    VAds.SetConvQueueCpltCallback([&](Ads1256 *) {
        auto cali_result = kLineCali_VAds.Calc(VAds.GetVoltage(0));
        kVAdsCaliResult  = cali_result;
        kVAdsWatchDog.Exam(std::abs(cali_result) <100.0f); // 电压看门狗
        kVAdsFilterResult = kVAdsFilter.Step(cali_result);
    });

    // IAds Callback
    IAds.SetConvQueueCpltCallback([&](Ads1256 *) {
        auto cali_result = kLineCali_IAds.Calc(IAds.GetVoltage(0));
        kIAdsCaliResult  = cali_result;
        kIAdsWatchDog.Exam(std::abs(cali_result) < 4.0f); // 电流看门狗
        // kIAdsFilterResult = kIAdsFilter.Step(cali_result);
    });
}
