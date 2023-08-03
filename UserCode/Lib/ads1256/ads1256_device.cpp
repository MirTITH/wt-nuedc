#include "ads1256_device.hpp"
#include "lv_app/screeen_console.h"
#include "Led/led_device.hpp"
#include "Relay/relay_device.hpp"
#include "WatchDog/watchdog.hpp"
#include "line_calis.hpp"

Ads1256 VAds(&hspi3,
             VDrdy_GPIO_Port, VDrdy_Pin,
             VAds_nReset_GPIO_Port, VAds_nReset_Pin,
             VSync_GPIO_Port, VSync_Pin,
             VAds_nCs_GPIO_Port, VAds_nCs_Pin);

Ads1256 IAds(&hspi2,
             IDrdy_GPIO_Port, IDrdy_Pin,
             nullptr, IAds_nReset_Pin,
             ISync_GPIO_Port, ISync_Pin,
             IAds_nCs_GPIO_Port, IAds_nCs_Pin);

static Butter_LP_5_50_20dB_5000Hz<float> kIAdsFilter;
std::atomic<float> kIAdsFilterResult = 0;

static Butter_LP_5_50_20dB_5000Hz<float> kVAdsFilter;
std::atomic<float> kVAdsFilterResult = 0;

WatchDog kIAdsWatchDog([](void *) {
    relay::GridConnector.Set(Relay_State::Close);
    relay::LoadConnector.Set(Relay_State::Close);
    relay::BridgeA.Set(Relay_State::Close);
    relay::BridgeB.Set(Relay_State::Close);
    KeyboardLed.SetColor(5, 0, 0);
},
                       10);

void InitAds()
{
    // VAds
    VAds.SetConvQueueCpltCallback([&](Ads1256 *ads) {
        kVAdsFilterResult = kVAdsFilter.Step(kLineCali_B_VAds.Calc(ads->GetVoltage(0)));
    });
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
    IAds.SetConvQueueCpltCallback([&](Ads1256 *ads) {
        kIAdsWatchDog.Exam(std::abs(kLineCali_B_IAds.Calc(IAds.GetVoltage(0))) < 2.0f);
        kIAdsFilterResult = kIAdsFilter.Step(kLineCali_B_IAds.Calc(ads->GetVoltage(0)));
    });

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
}
