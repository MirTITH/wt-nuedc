#include "common_objs.hpp"

static control_system::IController<float> kPassiveIcontroller{{0.1, 1.0f / 5000.0f}, {0.0f, 1.0f}};

void StatePassiveInv_Loop()
{
    float amtitude         = 1;
    auto controller_output = kPassiveIcontroller.Step(amtitude - kAcOutPll.d_);

    auto wave_value = controller_output * std::cos(kAcOutPll.phase_);

    kSpwm.SetSineValue(wave_value);

    JFStream << kVAdsCaliResult << kIAdsCaliResult << EndJFStream;
}

void StatePassiveInv_OnEnter()
{
    kPassiveIcontroller.ResetState();
    kPassiveIcontroller.SetStateValue(0.21);
    StatePassiveInv_Loop();
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
