#include "common_objs.hpp"
#include "states.hpp"
#include "control_system/pr_controller.hpp"
#include "DelayHolder/delay_holder.hpp"

static control_system::PRController<float> kPrController(1.0 / 5000.0);
static control_system::IController<float> kCorrectionPhaseController{{0.3, 1.0f / 5000.0f}, {-M_PI / 2, M_PI / 2}};
static control_system::IController<float> kIcontroller{{0.1, 1.0f / 5000.0f}, {0.0f, 1.0f}};
static DelayHolder kConnectStateHolder(1000);
static int32_t kStartEncoderCount;

bool kIsAbleToConnect  = false; // 是否满足并网条件
bool kIsAllowToConnect = false; // 用户是否允许并网

void StateOnGridInv_OnEnter()
{
    kStartEncoderCount = KeyboardEncoder.Count();

    kIsAbleToConnect = false;
    kPrController.ResetState();
    kCorrectionPhaseController.ResetState();
    kIcontroller.ResetState();
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
    if (kIsAbleToConnect == false || kIsAllowToConnect == false) {
        float kErrPhase        = LoopSimplify(kGridPll.phase_ - kAcOutPll.phase_);
        float kCorrectionPhase = kCorrectionPhaseController.Step(kErrPhase);

        auto controller_output = kIcontroller.Step(kGridPll.d_ - kAcOutPll.d_); // 电压幅值闭环
        // auto controller_output = kIcontroller.Step(10 - kAcOutPll.d_); // 电压幅值开环

        auto wave_value = controller_output * std::cos(kGridPll.phase_ + kCorrectionPhase);
        kSpwm.SetSineValue(wave_value);
        if (kConnectStateHolder.Exam(std::abs(kGridPll.d_ - kAcOutPll.d_) < 1.0f && std::abs(kErrPhase) < 0.1)) {
            kIsAbleToConnect = true;
            // if (kIsAllowToConnect) { 
            //     relay::GridConnector.Set(Relay_State::On);
            // }
        }
    } else {
        float i_amtitude_ref = (KeyboardEncoder.Count() - kStartEncoderCount) * std::sqrt(2.0f) / 400.0f;
        kAcIrefWatcher       = i_amtitude_ref;
        auto err             = i_amtitude_ref * std::cos(kGridPll.phase_) - kIAdsCaliResult;
        auto wave_value      = kPrController.Step(err);
        kSpwm.SetSineValue(wave_value);
    }

    JFStream << kGridVoltage << kGridPll.phase_ << kVAdsCaliResult << kAcOutPll.phase_ << EndJFStream;
}

void StateOnGridInv_OnExit()
{
}
