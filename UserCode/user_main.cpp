#include "user_main.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include <cstdio>
#include "freertos_io/uart_device.hpp"
#include "control_system/z_tf.hpp"
#include "HighPrecisionTime/high_precision_time.h"
#include "tim.h"
#include "thread_priority_def.h"
#include "freertos_io/uart_thread.hpp"
#include <string>

using namespace std;

void ZtfTest()
{
    // ztf
    ZTf<float> ztf({66.2117333333333, -124.136000000000, 58.1856000000000},
                   {1, -0.333333333333333, -0.666666666666667});

    ztf.ResetState();
    uint32_t start_time = HPT_GetUs();
    for (size_t i = 0; i < 10000000; i++) {
        ztf.Step(1);
    }
    auto duration         = HPT_GetUs() - start_time;
    auto speed            = 10000000.0f / duration * 1000.0f;
    auto next_step_result = ztf.Step(1);

    ztf.ResetState();
    printf("ztf.Step(1): %g\n duration: %lu us\n speed: %g kps\n", next_step_result, duration, speed);
    // sstr << "ztf.Step(1):" << next_step_result << "\n  duration: " << duration << "\n  speed: " << speed << " kps\n";
    for (size_t i = 0; i < 10; i++) {
        printf("%g\n", ztf.Step(1));
    }
}

void TestThread(void *argument)
{
    (void)argument;

    HAL_TIM_Base_Start_IT(&htim3);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

    float a, b, c;

    string str;

    while (true) {
        printf("请输入三个数字：\n");
        fflush(stdin);
        auto num = scanf("%f, %f, %f", &a, &b, &c);
        printf("接收到了 %d 个数字\n", num);
        printf("这三个数字是：%g, %g, %g\n", a, b, c);
    }
}

void BlinkLedEntry(void *argument)
{
    (void)argument;

    ZtfTest();

    while (true) {
        vTaskDelay(500);
    }
}

void StartDefaultTask(void const *argument)
{
    (void)argument;

    xTaskCreate(TestThread, "test_thread", 512, nullptr, PriorityNormal, nullptr);
    xTaskCreate(BlinkLedEntry, "blink_led", 512, nullptr, PriorityNormal, nullptr);

    vTaskDelete(nullptr);
}
