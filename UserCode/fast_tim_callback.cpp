#include "fast_tim_callback.hpp"
#include <atomic>
#include "pwm_rec/pwm_rec.hpp"
#include "Vofa/just_float.hpp"
#include "HighPrecisionTime/time_meter.hpp"
#include "control_system/pll.hpp"
#include "Adc/adc_class_device.hpp"

uint32_t kFastTimCallbackDuration;
uint32_t kFastTimCallbackCount = 0;

static control_system::Pll<float> pll(1.0 / 5000.0);

void FastTimCallback()
{
    // 频率和时间统计
    TimeMeter time_meter(&kFastTimCallbackDuration);
    kFastTimCallbackCount++;

    extern std::atomic<bool> kStartPwmRec;
    extern std::atomic<float> kMod;
    extern std::atomic<float> kDuty;
    extern UnipolarSpwm kSpwm;

    if (kStartPwmRec) {
        auto ac_volt = Adc2.GetVoltage(0);
        pll.Step(ac_volt);
        kDuty = (kMod * std::cos(pll.phase_) + 1) / 2;
        kSpwm.SetDuty(kDuty);
        // JFStream << ac_volt << pll.phase_ << kDuty << EndJFStream;
    }
}
