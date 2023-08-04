#include "states.hpp"
#include "Relay/relay_device.hpp"
#include "pwm/spwm_device.hpp"

AppStateClass kAppState;

void EmergencyStop()
{
    // 先关闭，防止切换状态卡死，保险
    relay::CloseAllRelay();
    kSpwm.StopPwm();

    // 多次关闭，保险
    kAppState.SwitchTo(AppState_t::Stop);
}
