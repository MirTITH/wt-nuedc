#include "led_device.hpp"

RgbLed KeyboardLed(&htim9, TIM_CHANNEL_2, &htim9, TIM_CHANNEL_1, &htim4, TIM_CHANNEL_1);
