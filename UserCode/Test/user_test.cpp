#include "user_test.hpp"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRtosSys/thread_priority_def.h"
#include <cstdio>
#include "freertos_io/os_printf.h"
#include "freertos_io/uart_device.hpp"
#include "sysmem.h"

// Test includes
// #include "test_template.cpp.hpp"
#include "control_system_test.cpp.hpp"
// #include "adc_test.hpp"
// #include "math_test.cpp.hpp"
// #include "pll_test.cpp.hpp"
// #include "high_precision_time_test.cpp.hpp"
// #include "malloc_test.cpp.hpp"
#include "Lcd/lcd_device.hpp"

using namespace user_test;

void BlinkLedEntry(void *argument)
{
    (void)argument;

    while (true) {
        HAL_GPIO_TogglePin(Led2_GPIO_Port, Led2_Pin);
        vTaskDelay(250);
    }
}

uint16_t Rgb888To565(uint32_t rgb888)
{
    return ((((rgb888) >> 19) & 0x1f) << 11) | ((((rgb888) >> 10) & 0x3f) << 5) | (((rgb888) >> 3) & 0x1f);
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
    // MallocTest();

    // for (size_t i = 0; i < kData_SIZE; i++) {
    //     kData[i] = 0xffff;
    // }

    // LCD.InitAll();

    // while (true) {
    //     LCD.FillScreen(Rgb888To565(0xCC9999));
    //     vTaskDelay(500);
    //     LCD.FillScreen(Rgb888To565(0xFFFF99));
    //     vTaskDelay(500);
    //     LCD.FillScreen(Rgb888To565(0x666699));
    //     vTaskDelay(500);
    //     LCD.WriteScreenDma(0, 0, 320 - 1, 480 - 1, (uint16_t *)(kData));
    //     vTaskDelay(500);
    // }

    while (true) {
        os_printf("%u, %u, %u\n", GetTotalNewlibHeapSize(), GetUsedNewlibHeapSize(), GetAvailableNewlibHeapSize());
        vTaskDelay(100);
    }

    BlinkLedEntry(nullptr);

    vTaskDelete(nullptr);
}

void StartTestThread()
{
    xTaskCreate(TestThread, "TestThread", 1024, nullptr, PriorityNormal, nullptr);
}
