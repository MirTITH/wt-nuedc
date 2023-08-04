#include "common_objs.hpp"

static control_system::IController<float> kIcontroller{{0.1, 1.0f / 5000.0f}, {0.0f, 1.0f}};
static control_system::IController<float> kDeltaPhaseController{{0.3, 1.0f / 5000.0f}, {-M_PI / 2, M_PI / 2}};

static int32_t kStartEncoderCount;

void StateOnGridInv_OnEnter()
{
    kIcontroller.ResetState();

    kStartEncoderCount = KeyboardEncoder.Count();

    kSpwm.StartPwm();
    switch (kWhoAmI) {
        case BoardSelector::A:
            relay::BridgeA.Set(Relay_State::On);
            relay::LoadConnector.Set(Relay_State::On);
            break;
        case BoardSelector::B:
            // relay::BridgeB.Set(Relay_State::On);
            break;
    }
}

/**
 * @brief 循环变量化简
 *
 * @param cycle 周期
 * @param value
 * @return double 化简后的值，在[- T / 2, T / 2] 之间
 */
float LoopSimplify(float value, float cycle = 2 * M_PI)
{
    double mod_value = std::fmod(value, cycle);

    if (mod_value > cycle / 2) {
        mod_value -= cycle;
    }

    if (mod_value < -cycle / 2) {
        mod_value += cycle;
    }

    return mod_value;
}

void StateOnGridInv_Loop()
{
    // kDeltaPhase 大概为 0.48
    // kDeltaPhase = (KeyboardEncoder.Count() - kStartEncoderCount) / 400.0f;

    float kDeltaPhase      = kDeltaPhaseController.Step(LoopSimplify(kGridPll.phase_ - kAcOutPll.phase_));
    auto controller_output = kIcontroller.Step(kGridPll.d_ - kAcOutPll.d_); // 电压幅值闭环
    // auto controller_output = kIcontroller.Step(10 - kAcOutPll.d_); // 电压幅值开环
    auto wave_value = controller_output * std::cos(kGridPll.phase_ + kDeltaPhase);
    kSpwm.SetSineValue(wave_value);

    JFStream << kGridVoltage << kVAdsCaliResult << kGridPll.phase_ << kAcOutPll.phase_ << EndJFStream;
}

void StateOnGridInv_OnExit()
{
}
