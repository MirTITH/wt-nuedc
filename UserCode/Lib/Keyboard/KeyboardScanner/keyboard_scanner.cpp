#include "keyboard_scanner.hpp"
#include "flexible_button.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include <cassert>
#include "thread_priority_def.h"
#include "freertos_io/os_printf.h"

extern KeyboardClass Keyboard;

static uint16_t button_matrix_;

static uint8_t common_btn_read(void *arg)
{
    return Keyboard_Read((Keys)((flex_button_t *)arg)->id);
}

void UpdateButtonMatrix()
{
    uint16_t result = 0;
    for (size_t i = 0; i < 16; i++) {
        Keyboard.SetAddr(i);
        vTaskDelay(1);
        result |= Keyboard.ReadData() << i;
    }

    button_matrix_ = result;
}

__weak void common_btn_evt_cb(flex_button_t *btn)
{
    (void)btn;
}

void FlexibleButton_Init()
{
    // 矩阵键盘
    static flex_button_t buttonMatrix[16];
    for (int i = 0; i < 16; i++) {
        buttonMatrix[i].id                     = i;
        buttonMatrix[i].usr_button_read        = common_btn_read;
        buttonMatrix[i].cb                     = reinterpret_cast<void (*)(void *)>(common_btn_evt_cb);
        buttonMatrix[i].pressed_logic_level    = 1;
        buttonMatrix[i].short_press_start_tick = FLEX_MS_TO_SCAN_CNT(500);
        buttonMatrix[i].long_press_start_tick  = FLEX_MS_TO_SCAN_CNT(1000);
        buttonMatrix[i].long_hold_start_tick   = FLEX_MS_TO_SCAN_CNT(2000);
        flex_button_register(&buttonMatrix[i]);
    }

    // 旋钮的键
    static flex_button_t buttonKnob;

    buttonKnob.id                     = 16;
    buttonKnob.usr_button_read        = common_btn_read;
    buttonKnob.cb                     = reinterpret_cast<void (*)(void *)>(common_btn_evt_cb);
    buttonKnob.pressed_logic_level    = 1;
    buttonKnob.short_press_start_tick = FLEX_MS_TO_SCAN_CNT(500);
    buttonKnob.long_press_start_tick  = FLEX_MS_TO_SCAN_CNT(1000);
    buttonKnob.long_hold_start_tick   = FLEX_MS_TO_SCAN_CNT(2000);
    flex_button_register(&buttonKnob);
}

void FlexibleButton_Scan()
{
    UpdateButtonMatrix();
    flex_button_scan();
}

uint8_t Keyboard_Read(Keys key)
{
    if ((int)key < 16) {
        return (button_matrix_ >> (int)key) & 1;
    } else {
        return Keyboard.ReadKey(key);
    }
}

void KeyboardScannerEntry(void *)
{
    while (1) {
        FlexibleButton_Scan(); // 这个要花费 16 ms
        vTaskDelay(4);
    }
}

void KeyboardScanner_Init()
{
    FlexibleButton_Init();
}

void KeyboardScannerStart()
{
    KeyboardScanner_Init();
    xTaskCreate(KeyboardScannerEntry, "kb_scanner", 256, nullptr, PriorityNormal, nullptr);
}