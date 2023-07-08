#include "user_test.hpp"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRtosSys/thread_priority_def.h"
#include <cstdio>
#include "freertos_io/os_printf.h"

// Test includes
#include "test_template.cpp.hpp"
#include "control_system_test.cpp.hpp"
#include "ads1256_test.cpp.hpp"

using namespace user_test;

void TestThread(void *argument)
{
    (void)argument;

    os_printf("Start %s\n", __func__);

    // PidTest();
    Ads1256Test();

    BlinkLedEntry(nullptr);

    os_printf("Exit %s\n", __func__);
    vTaskDelete(nullptr);
}

void StartTestThread()
{
    xTaskCreate(TestThread, "TestThread", 512, nullptr, PriorityNormal, nullptr);
}
