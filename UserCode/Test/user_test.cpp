#include "user_test.hpp"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRtosSys/thread_priority_def.h"
#include "fast_tim_callback.hpp"
#include <cstdio>
#include <atomic>
#include "Vofa/just_float.hpp"
#include "ads1256/ads1256_device.hpp"
#include "Adc/adc_class_device.hpp"

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
// #include "UnitTests/test_svpwm.cpp"
// #include "UnitTests/test_pwm_rec.cpp"
// #include "UnitTests/test_delay_module.cpp"
// #include "UnitTests/test_prcontroller.cpp"

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
    // TestSvpwm();
    // TestPwmRec();
    // TestDelayModule();
    // TestPrController();

    // extern std::atomic<float> kMod;

    // float value = kMod;
    // while (true) {
    //     scanf("%f", &value);
    //     kMod = value;
    // }

    // while (true) {
    //     JFStream << kIAdsCaliResult << kIAdsFilterResult << EndJFStream;
    //     // JFStream << kVAdsCaliResult << kVAdsFilterResult << EndJFStream;
    //     vTaskDelay(1);
    // }

    while (true) {
        JFStream << kVAdsCaliResult << kVAdsFilterResult << EndJFStream;
        vTaskDelay(1);
    }

    vTaskDelete(nullptr);
}

void StartTestThread()
{
    xTaskCreate(TestThread, "TestThread", 1024 * 2, nullptr, PriorityNormal, nullptr);
}
