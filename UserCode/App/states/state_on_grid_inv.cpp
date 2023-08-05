#include "common_objs.hpp"
#include "states.hpp"
#include "control_system/pr_controller.hpp"
#include "DelayHolder/delay_holder.hpp"
#include "Led/led_device.hpp"
#include "lv_app/screeen_console.h"

static control_system::PRController<float> kPrController(1.0 / 5000.0);
static control_system::IController<float> kCorrectionPhaseController{{0.3, 1.0f / 5000.0f}, {-M_PI / 2, M_PI / 2}};
static control_system::IController<float> kIcontroller{{0.1, 1.0f / 5000.0f}, {0.0f, 1.0f}};
static DelayHolder kConnectStateHolder(1000);
static int32_t kStartEncoderCount;

bool kIsAbleToConnect  = false; // 是否满足并网条件
bool kIsAllowToConnect = false; // 用户是否允许并网

enum class OnGridState {
    NotReady,
    Ready
};

OnGridState kOnGridState;

void StateOnGridInv_OnEnter()
{
    kStartEncoderCount = KeyboardEncoder.Count();

    kIsAbleToConnect = false;
    kPrController.ResetState();
    kCorrectionPhaseController.ResetState();
    kIcontroller.ResetState();

    kIcontroller.SetStateValue(kActiveInv_ControllerOutputWatcher);

    kOnGridState = OnGridState::NotReady;

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
    if (kOnGridState == OnGridState::NotReady) {
        float kErrPhase            = LoopSimplify(kGridPll.phase_ - kAcOutPll.phase_);
        kOnGridInv_ErrPhaseWatcher = kErrPhase;

        float kCorrectionPhase = kCorrectionPhaseController.Step(kErrPhase);
        auto controller_output = kIcontroller.Step(kGridPll.d_ - kAcOutPll.d_); // 电压幅值闭环
        // auto controller_output = kIcontroller.Step(10 - kAcOutPll.d_); // 电压幅值开环

        auto wave_value = controller_output * std::cos(kGridPll.phase_ + kCorrectionPhase);
        kSpwm.SetSineValue(wave_value);

        // 检查条件
        if (kConnectStateHolder.Exam(std::abs(kGridPll.d_ - kAcOutPll.d_) < 1.0f && std::abs(kErrPhase) < 0.1)) {
            kIsAbleToConnect = true;

            // 切换模式
            if (kIsAllowToConnect) {
                KeyboardLed.SetColor(1, 1, 1, 0.1);
                relay::GridConnector.Set(Relay_State::On);
                kStartEncoderCount = KeyboardEncoder.Count();
                kOnGridState       = OnGridState::Ready;
            }
        } else {
            kIsAbleToConnect = false;
        }
    } else if (kOnGridState == OnGridState::Ready) {
        float i_amtitude_ref = (KeyboardEncoder.Count() - kStartEncoderCount) * std::sqrt(2.0f) / 400.0f;
        kAcIrefWatcher       = i_amtitude_ref;
        auto err             = i_amtitude_ref * std::cos(kGridPll.phase_) - kIAdsCaliResult;
        auto wave_value      = kPrController.Step(err);
        kSpwm.SetSineValue(wave_value);

        KeyboardLed.SetColor(0, 1, 1, 0.1);
    } else {
        ScreenConsole_AddText("StateOnGridInv_Loop Err State!\n");
        kAppState.SwitchTo(AppState_t::Stop);
    }

    JFStream << kVAdsCaliResult << kIAdsCaliResult << kAcOutPll.phase_ << kGridVoltage << kGridPll.phase_ << EndJFStream;
}

void StateOnGridInv_OnExit()
{
}
