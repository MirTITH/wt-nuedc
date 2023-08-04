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
#include "common_objs.hpp"

uint32_t kFastTimCallbackDuration;
uint32_t kFastTimCallbackCount = 0;

void SquareOutput(float phase)
{
    if (phase > M_PI / 2 && phase < 3 * M_PI / 2) {
        HAL_GPIO_WritePin(TestPin1_GPIO_Port, TestPin1_Pin, GPIO_PIN_RESET);
    } else {

        HAL_GPIO_WritePin(TestPin1_GPIO_Port, TestPin1_Pin, GPIO_PIN_SET);
    }
}

void FastTimCallback()
{
    // 频率和时间统计
    TimeMeter time_meter(&kFastTimCallbackDuration);
    kFastTimCallbackCount++;

    auto grid_volt = GetGridVoltage();
    kGridVoltage   = grid_volt;
    kGridPll.Step(grid_volt);

    kAcOutPll.Step(kVAdsCaliResult);
    SquareOutput(kAcOutPll.phase_);

    extern void StateStop_Loop();
    extern void StateOnGridInv_Loop();
    extern void StatePassiveInv_Loop();
    extern void StateActiveInv_Loop();

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
