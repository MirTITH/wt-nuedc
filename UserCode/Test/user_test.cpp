#include "user_test.hpp"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRtosSys/thread_priority_def.h"
#include <cstdio>
#include "freertos_io/os_printf.h"
#include <array>
#include "freertos_io/uart_device.hpp"
#include <string>

// Test includes
// #include "test_template.cpp.hpp"
// #include "control_system_test.cpp.hpp"
#include "adc_test.hpp"
// #include "math_test.cpp.hpp"
#include "pll_test.cpp.hpp"

using namespace user_test;
using namespace std;

void BlinkLedEntry(void *argument)
{
    (void)argument;

    while (true) {
        HAL_GPIO_TogglePin(Led2_GPIO_Port, Led2_Pin);
        vTaskDelay(250);
    }
}

std::array<uint32_t, 1024> kBuffer;

void TestThread(void *argument)
{
    (void)argument;

    // ZtfTest();
    // AdcTest();
    // MathTest();
    // PllTest();
    // PllTestInIsr();

    string str;

    while (true) {
        // __disable_irq();
        // for (auto &var : kBuffer) {
        //     var = HPT_GetTotalSysTick();
        // }
        // __enable_irq();

        auto lastSysTick = HPT_GetTotalSysTick();

        HPT_DelayUs(100000);

        // HAL_GPIO_TogglePin(Led2_GPIO_Port, Led2_Pin);

        auto cpuFreq = HPT_GetTotalSysTick() - lastSysTick;

        str = to_string(cpuFreq).append("\n");

        Uart1.uart_device.WriteDirectly(str.c_str(), str.size());
        // os_printf("%lu\n", cpuFreq);

        // vTaskDelay(1000);
    }

    // BlinkLedEntry(nullptr);

    vTaskDelete(nullptr);
}

void StartTestThread()
{
    xTaskCreate(TestThread, "TestThread", 1024, nullptr, PriorityNormal, nullptr);
}
