#include "Keyboard/keyboard_device.hpp"
#include "ElectricRelay/electric_relay_device.hpp"
#include "lv_app/screeen_console.h"

static void ButtonCallback_K0(flex_button_t *btn)
{
    if (btn->event == FLEX_BTN_PRESS_CLICK) {
        kER_BridgeA.Set(ER_State::On);
        ScreenConsole_AddText("BridgeA On\n");
    } else if (btn->event == FLEX_BTN_PRESS_SHORT_START) {
        kER_BridgeA.Set(ER_State::Close);
        ScreenConsole_AddText("BridgeA Close\n");
    }
}

static void ButtonCallback_K1(flex_button_t *btn)
{
    if (btn->event == FLEX_BTN_PRESS_CLICK) {
        kER_BridgeB.Set(ER_State::On);
        ScreenConsole_AddText("BridgeB On\n");
    } else if (btn->event == FLEX_BTN_PRESS_SHORT_START) {
        kER_BridgeB.Set(ER_State::Close);
        ScreenConsole_AddText("BridgeB Close\n");
    }
}

static void ButtonCallback_K2(flex_button_t *btn)
{
    if (btn->event == FLEX_BTN_PRESS_CLICK) {
        kER_LoadConnector.Set(ER_State::On);
        ScreenConsole_AddText("LoadConnector On\n");
    } else if (btn->event == FLEX_BTN_PRESS_SHORT_START) {
        kER_LoadConnector.Set(ER_State::Close);
        ScreenConsole_AddText("LoadConnector Close\n");
    }
}

static void ButtonCallback_K3(flex_button_t *btn)
{
    if (btn->event == FLEX_BTN_PRESS_CLICK) {
        kER_GridConnector.Set(ER_State::On);
        ScreenConsole_AddText("GridConnector On\n");
    } else if (btn->event == FLEX_BTN_PRESS_SHORT_START) {
        kER_GridConnector.Set(ER_State::Close);
        ScreenConsole_AddText("GridConnector Close\n");
    }
}

void ButtonCallbackRegister()
{
    SetButtonCallback(Keys::k0, ButtonCallback_K0);
    SetButtonCallback(Keys::k1, ButtonCallback_K1);
    SetButtonCallback(Keys::k2, ButtonCallback_K2);
    SetButtonCallback(Keys::k3, ButtonCallback_K3);
}
