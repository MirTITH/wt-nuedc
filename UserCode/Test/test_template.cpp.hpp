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
        HAL_GPIO_TogglePin(Led6_GPIO_Port, Led6_Pin);
        // HAL_GPIO_TogglePin(Led7_GPIO_Port, Led7_Pin);
        vTaskDelay(500);
    }
}

} // namespace user_test
