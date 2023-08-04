#include "common_objs.hpp"
#include "control_system/pr_controller.hpp"

static control_system::PRController<float> kPrController(1.0 / 5000.0);
static int32_t kStartEncoderCount;

void StatePassiveInv_Loop()
{
    float i_amtitude_ref = (KeyboardEncoder.Count() - kStartEncoderCount) * std::sqrt(2.0f) / 400.0f;
    kAcIrefWatcher       = i_amtitude_ref;
    auto err             = i_amtitude_ref * std::cos(kAcOutPll.phase_) - kIAdsCaliResult;
    auto wave_value      = kPrController.Step(err);
    kSpwm.SetSineValue(wave_value);

    JFStream << kVAdsCaliResult << kIAdsCaliResult << i_amtitude_ref << wave_value << EndJFStream;
}

void StatePassiveInv_OnEnter()
{
    kStartEncoderCount = KeyboardEncoder.Count();

    kPrController.ResetState();
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

void StatePassiveInv_OnExit()
{
}
