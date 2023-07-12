#include "user_test.hpp"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRtosSys/thread_priority_def.h"
#include <cstdio>
#include "freertos_io/os_printf.h"

// Test includes
// #include "test_template.cpp.hpp"
// #include "control_system_test.cpp.hpp"
// #include "adc_test.hpp"
// #include "math_test.cpp.hpp"
#include "pll_test.cpp.hpp"

using namespace user_test;

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

    // ZtfTest();
    // AdcTest();
    // MathTest();
    PllTest();

    BlinkLedEntry(nullptr);

    vTaskDelete(nullptr);
}

void StartTestThread()
{
    xTaskCreate(TestThread, "TestThread", 512, nullptr, PriorityNormal, nullptr);
}
