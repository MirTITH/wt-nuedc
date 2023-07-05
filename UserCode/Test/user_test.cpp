#include "user_test.hpp"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRtosSys/thread_priority_def.h"
#include <cstdio>
#include "freertos_io/os_printf.h"

void BlinkLedEntry(void *argument)
{
    (void)argument;

    while (true) {
        HAL_GPIO_TogglePin(Led2_GPIO_Port, Led2_Pin);
        vTaskDelay(250);
    }
}

void TestThread(void *argument)
{
    (void)argument;

    BlinkLedEntry(nullptr);

    vTaskDelete(nullptr);
}

void StartTestThread()
{
    xTaskCreate(TestThread, "TestThread", 512, nullptr, PriorityNormal, nullptr);
}
