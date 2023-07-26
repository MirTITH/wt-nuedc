#include "user_test.hpp"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRtosSys/thread_priority_def.h"

// Test includes
// #include "test_template.cpp"
// #include "OldTests/control_system_test.cpp"
// #include "OldTests/adc_test.cpp"
// #include "OldTests/math_test.cpp"
// #include "OldTests/pll_test.cpp"
// #include "OldTests/high_precision_time_test.cpp"
// #include "OldTests/malloc_test.cpp"
// #include "OldTests/ads1256_test.cpp"
// #include "UnitTests/test_lvgl.cpp"
// #include "UnitTests/test_atomic.cpp"
#include "UnitTests/test_svpwm.cpp"

void TestThread(void *argument)
{
    (void)argument;

    // TestTemplate();
    // ZtfTest();
    // AdcTest();
    // MathTest();
    // PllTest();
    // PllTestInIsr();
    // HighPrecisionTimeTest();
    // MallocTest();
    // PidTest();
    // Ads1256Test();
    // TestLvgl();
    // StartTestAtomic();
    TestSvpwm();

    vTaskDelete(nullptr);
}

void StartTestThread()
{
    xTaskCreate(TestThread, "TestThread", 1024 * 2, nullptr, PriorityNormal, nullptr);
}
