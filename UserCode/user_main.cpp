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

UartThread Uart1Thread(Uart1, "Uart1");

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

// size_t tim_counter;

char test_str[] = "Test str\n";

void TestThread(void *argument)
{
    (void)argument;

    // HAL_TIM_Base_Start_IT(&htim3);
    // HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    // HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

    // float a, b, c;

    uint32_t start_time, duration;
    string str;

    while (true) {
        start_time = HPT_GetUs();
        Uart1Thread.Write(str.c_str(), str.size());
        // Uart1.Write(str);
        // Uart1.WaitForWriteCplt();
        duration = HPT_GetUs() - start_time;
        str      = string("用时：") + to_string(duration).append("\n");
        // printf("%lu\n", htim3.Instance->CNT);
        // assert(HAL_GetTick() < 2000);
        // Uart1.WriteNonBlock(test_str, sizeof(test_str) - 1);
        vTaskDelay(100);
        // printf("请输入三个数字：\n");
        // fflush(stdin);
        // auto num = scanf("%f, %f, %f", &a, &b, &c);
        // printf("接收到了 %d 个数字\n", num);
        // printf("这三个数字是：%g, %g, %g\n", a, b, c);
    }
}

void BlinkLedEntry(void *argument)
{
    (void)argument;

    ZtfTest();

    while (true) {
        // HAL_GPIO_TogglePin(Led3_GPIO_Port, Led3_Pin);
        vTaskDelay(500);
    }
}

void StartDefaultTask(void const *argument)
{
    (void)argument;

    xTaskCreate(TestThread, "test_thread", 512, nullptr, PriorityBelowNormal, nullptr);
    // xTaskCreate(BlinkLedEntry, "blink_led", 512, nullptr, PriorityBelowNormal, nullptr);

    vTaskDelete(nullptr);
}
