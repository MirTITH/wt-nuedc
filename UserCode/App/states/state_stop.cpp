#include "Relay/relay_device.hpp"
#include "pwm/spwm_device.hpp"

void StateStop_OnEnter()
{
    relay::CloseAllRelay();
    kSpwm.StopPwm();
}

void StateStop_OnExit()
{
}

void StateStop_Loop()
{
}
