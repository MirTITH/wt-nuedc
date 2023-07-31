#pragma once
#include "flexible_button.h"
#include "../keyboard.hpp"

/**
 * @brief 读取按键或开关
 *
 * @param key 
 * @return uint8_t 按键按下为 1，开关向 H 拨为 1
 */
uint8_t Keyboard_Read(Keys key);

void KeyboardScannerStart();
