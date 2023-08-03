#include "Keyboard/keyboard_device.hpp"
#include "Relay/relay_device.hpp"
#include "lv_app/screeen_console.h"
#include "pwm/spwm_device.hpp"
#include "states.hpp"

static void ButtonCallback_K0(flex_button_t *btn)
{
    if (btn->event == FLEX_BTN_PRESS_CLICK) {
        relay::BridgeA.Set(Relay_State::Close);
        ScreenConsole_AddText("BridgeA Close\n");
    } else if (btn->event == FLEX_BTN_PRESS_SHORT_START) {
        relay::BridgeA.Set(Relay_State::On);
        ScreenConsole_AddText("BridgeA On\n");
    }
}

static void ButtonCallback_K1(flex_button_t *btn)
{
    if (btn->event == FLEX_BTN_PRESS_CLICK) {
        relay::BridgeB.Set(Relay_State::Close);
        ScreenConsole_AddText("BridgeB Close\n");
    } else if (btn->event == FLEX_BTN_PRESS_SHORT_START) {
        relay::BridgeB.Set(Relay_State::On);
        ScreenConsole_AddText("BridgeB On\n");
    }
}

static void ButtonCallback_K2(flex_button_t *btn)
{
    if (btn->event == FLEX_BTN_PRESS_CLICK) {
        relay::LoadConnector.Set(Relay_State::Close);
        ScreenConsole_AddText("LoadConnector Close\n");
    } else if (btn->event == FLEX_BTN_PRESS_SHORT_START) {
        relay::LoadConnector.Set(Relay_State::On);
        ScreenConsole_AddText("LoadConnector On\n");
    }
}

static void ButtonCallback_K3(flex_button_t *btn)
{
    if (btn->event == FLEX_BTN_PRESS_CLICK) {
        relay::GridConnector.Set(Relay_State::Close);
        ScreenConsole_AddText("GridConnector Close\n");
    } else if (btn->event == FLEX_BTN_PRESS_SHORT_START) {
        relay::GridConnector.Set(Relay_State::On);
        ScreenConsole_AddText("GridConnector On\n");
    }
}

static void ButtonCallback_K9(flex_button_t *btn)
{
    if (btn->event == FLEX_BTN_PRESS_CLICK) {
        kAppState.SwitchTo(AppState_t::Stop);
        kSpwm.StopPwm();
        ScreenConsole_AddText("kSpwm.StopPwm\n");
    } else if (btn->event == FLEX_BTN_PRESS_SHORT_START) {
        kAppState.SwitchTo(AppState_t::ActiveInv);
        kSpwm.StartPwm();
        ScreenConsole_AddText("kSpwm.StartPwm\n");
    }
}

void ButtonCallbackRegister()
{
    SetButtonCallback(Keys::k0, ButtonCallback_K0);
    SetButtonCallback(Keys::k1, ButtonCallback_K1);
    SetButtonCallback(Keys::k2, ButtonCallback_K2);
    SetButtonCallback(Keys::k3, ButtonCallback_K3);
    SetButtonCallback(Keys::k9, ButtonCallback_K9);
}
