#pragma once
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

namespace user_test
{
void BlinkLedEntry(void *argument)
{
    (void)argument;

    while (true) {
        HAL_GPIO_TogglePin(Led2_GPIO_Port, Led2_Pin);
        vTaskDelay(250);
    }
}

} // namespace user_test
