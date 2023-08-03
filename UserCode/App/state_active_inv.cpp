#include "pwm/spwm_device.hpp"
#include <atomic>
#include "control_system/signal_generator.hpp"
#include "ads1256/ads1256_device.hpp"
#include "Vofa/just_float.hpp"

std::atomic<float> kMod = 0.1; // 调制度

static control_system::SineGenerator<float> kCosine(50 * 2 * M_PI, 1.0 / 5000.0, M_PI / 2);

void StateActiveInv_Loop()
{
    auto sin_value = kMod * kCosine.Step();
    kSpwm.SetSineValue(sin_value);
    // JFStream << sin_value << kIAdsCaliResult << EndJFStream;
}

void StateActiveInv_OnEnter()
{
}

void StateActiveInv_OnExit()
{
}