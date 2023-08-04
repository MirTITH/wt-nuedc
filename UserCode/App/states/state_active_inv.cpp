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
static control_system::IController<float> kIcontroller{{0.1, 1.0f / 5000.0f}, {0.0f, 1.0f}};

void StateActiveInv_Loop()
{
    float v_amtitude_ref = (0 + (KeyboardEncoder.Count() - kStartEncoderCount) / 100.0f) * std::sqrt(2.0f);
    kAcVrefWatcher       = v_amtitude_ref;

    auto controller_output = kIcontroller.Step(v_amtitude_ref - kAcOutPll.d_);

    auto wave_value = controller_output * kSine.Step(); // 闭环
    // auto wave_value =  v_amtitude_ref* kSine.Step(); // 开环

    kSpwm.SetSineValue(wave_value);

    // JFStream << kAcOutPll.d_ << controller_output << kVAdsCaliResult << kIAdsCaliResult << EndJFStream;
    JFStream << kVAdsCaliResult << kIAdsCaliResult << EndJFStream;
    // JFStream << kGridVoltage << EndJFStream;
}

void StateActiveInv_OnEnter()
{
    kStartEncoderCount = KeyboardEncoder.Count();
    kSine.ResetState();
    kIcontroller.ResetState();
    // kIcontroller.SetStateValue(0.2);

    kSpwm.StartPwm();
    switch (kWhoAmI) {
        case BoardSelector::A:
            relay::BridgeA.Set(Relay_State::On);
            relay::LoadConnector.Set(Relay_State::On);
            break;
        case BoardSelector::B:
            relay::BridgeB.Set(Relay_State::On);
            break;
    }
}

void StateActiveInv_OnExit()
{
}
