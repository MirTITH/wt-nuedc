#include "user_test.hpp"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRtosSys/thread_priority_def.h"
#include <cstdio>
#include "freertos_io/os_printf.h"
#include "freertos_io/uart_device.hpp"

// Test includes
// #include "test_template.cpp.hpp"
// #include "control_system_test.cpp.hpp"
// #include "adc_test.hpp"
// #include "math_test.cpp.hpp"
// #include "pll_test.cpp.hpp"
#include "high_precision_time_test.cpp.hpp"
#include "CoreMark/coremark.h"

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
    // PllTest();
    // PllTestInIsr();
    // HighPrecisionTimeTest();
    printf("Start CoreMark\n");
    vTaskDelay(1000);
    core_mark_main(0, nullptr);

    BlinkLedEntry(nullptr);

    vTaskDelete(nullptr);
}

void StartTestThread()
{
    xTaskCreate(TestThread, "TestThread", 1024, nullptr, PriorityNormal, nullptr);
}
