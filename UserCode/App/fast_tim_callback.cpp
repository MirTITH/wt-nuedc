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

void FastTimCallback()
{
    extern void StateStop_Loop();
    extern void StateOnGridInv_Loop();
    extern void StatePassiveInv_Loop();
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
        case AppState_t::PassiveInv:
            StatePassiveInv_Loop();
            break;
        case AppState_t::OnGridInv:
            StateOnGridInv_Loop();
            break;

        default:
            kAppState.SwitchTo(AppState_t::Stop);
            break;
    }
}
