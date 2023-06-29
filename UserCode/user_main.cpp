#include "user_main.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

void LedTask(void *argument)
{
    (void)argument;

    while (1) {
        HAL_GPIO_TogglePin(Led3_GPIO_Port, Led3_Pin);
        vTaskDelay(500);
    }
}

void StartDefaultTask(void const *argument)
{
    (void)argument;

    xTaskCreate(LedTask, "led2", 128, nullptr, 3, nullptr);

    while (true) {
        HAL_GPIO_TogglePin(Led2_GPIO_Port, Led2_Pin);
        vTaskDelay(200);
    }
}
