#include "Relay/relay_device.hpp"
#include "pwm/spwm_device.hpp"
#include "Filters/butterworth.hpp"
#include "Adc/adc_class_device.hpp"
#include "Vofa/just_float.hpp"
#include "common_objs.hpp"
#include "control_system/pll.hpp"

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
    // JFStream << kGridVoltage << kGridPll.d_ << kGridPll.phase_ << kGridPll.omega_ << EndJFStream;
    // JFStream << kGridVoltage << EndJFStream;
    JFStream << Adc1.GetAllVoltage() << Adc2.GetAllVoltage() << kGridVoltage << EndJFStream;
}
