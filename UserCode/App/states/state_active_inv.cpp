#include "pwm/spwm_device.hpp"
#include <atomic>
#include "control_system/signal_generator.hpp"
#include "ads1256/ads1256_device.hpp"
#include "Vofa/just_float.hpp"
#include "common_objs.hpp"
#include "control_system/pid_controller.hpp"
#include "Encoder/encoder_device.hpp"
#include "Relay/relay_device.hpp"

static control_system::SineGenerator<float> kSine(50 * 2 * M_PI, 1.0 / 5000.0);
static int32_t kStartEncoderCount;
std::atomic<float> kAcReference;
static control_system::IController<float> kIcontroller{{0.1, 1.0f / 5000.0f}, {0.0f, 1.0f}};

void StateActiveInv_Loop()
{
    kAcReference = (KeyboardEncoder.Count() - kStartEncoderCount) / 50.0f;

    kAcOutPll.Step(kVAdsCaliResult);
    auto i_output  = kIcontroller.Step(kAcReference - kAcOutPll.d_);
    auto sin_value = i_output * kSine.Step();
    kSpwm.SetSineValue(sin_value);

    JFStream << i_output << kVAdsCaliResult << kIAdsCaliResult << EndJFStream;
}

void StateActiveInv_OnEnter()
{
    kStartEncoderCount = KeyboardEncoder.Count();
    kSine.ResetState();
    kIcontroller.ResetState();
    // kIcontroller.SetStateValue(0.2);

    kSpwm.StartPwm();
    relay::BridgeB.Set(Relay_State::On);
    relay::LoadConnector.Set(Relay_State::On);
}

void StateActiveInv_OnExit()
{
}