#include "fast_tim_callback.hpp"
#include <atomic>
#include "Vofa/just_float.hpp"
#include "HighPrecisionTime/stat.hpp"
#include "control_system/pll.hpp"
#include "Adc/adc_class_device.hpp"
#include "pwm/spwm_device.hpp"
#include "control_system/signal_generator.hpp"
#include "ads1256/ads1256_device.hpp"
#include "states.hpp"

uint32_t kFastTimCallbackDuration;
uint32_t kFastTimCallbackCount = 0;

static control_system::SineGenerator<float> kCosine(50 * 2 * M_PI, 1.0 / 5000.0, M_PI / 2);
std::atomic<float> kMod = 0.1; // 调制度

void FastTimCb_ActiveInv()
{
    auto sin_value = kMod * kCosine.Step();
    kSpwm.SetSineValue(sin_value);
    JFStream << sin_value << kIAdsCaliResult << EndJFStream;
}

void FastTimCallback()
{
    extern void StateStop_Loop();
    extern void StatePllOnGridInv_Loop();
    extern void StatePllInv_Loop();
    extern void StateActiveInv_Loop();

    // 频率和时间统计
    TimeMeter time_meter(&kFastTimCallbackDuration);
    kFastTimCallbackCount++;
    switch (kAppState.GetState()) {
        case AppState_t::Stop:
            StateStop_Loop();
            break;
        case AppState_t::ActiveInv:
            StateActiveInv_Loop();

            break;
        case AppState_t::PllInv:
            StatePllInv_Loop();
            break;
        case AppState_t::PllOnGridInv:
            StatePllOnGridInv_Loop();
            break;

        default:
            kAppState.SwitchTo(AppState_t::Stop);
            break;
    }
}
