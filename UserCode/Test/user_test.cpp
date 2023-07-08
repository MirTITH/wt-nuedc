#include "user_test.hpp"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRtosSys/thread_priority_def.h"
#include <cstdio>
#include "freertos_io/os_printf.h"

// Test includes
#include "test_template.cpp.hpp"

using namespace user_test;

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
