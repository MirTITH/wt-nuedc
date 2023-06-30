#include "user_main.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include <cstdio>
#include <vector>

using namespace std;

/* void ZtfTest()
{
    stringstream sstr;
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
    sstr << "ztf.Step(1):" << next_step_result << "\n  duration: " << duration << "\n  speed: " << speed << " kps\n";
    for (size_t i = 0; i < 10; i++) {
        sstr << ztf.Step(1) << endl;
    }

    HAL_UART_Transmit(&huart1, (const uint8_t *)sstr.str().c_str(), sstr.str().size(), HAL_MAX_DELAY);
}
 */

// char str[] = "Hello!This is a long buff that need a long time to transmit!!! 太长了😈\n";

void TestThread(void *argument)
{
    (void)argument;

    while (1) {
        HAL_GPIO_TogglePin(Led2_GPIO_Port, Led2_Pin);
        vTaskDelay(100);
    }
}

void StartDefaultTask(void const *argument)
{
    (void)argument;

    xTaskCreate(TestThread, "test_thread", 256, nullptr, 1, nullptr);

    vector<int> vec;

    vec.push_back(12);
    vec.push_back(23);

    float a, b, c;

    while (true) {
        printf("请输入三个数字：\n");
        fflush(stdin);
        auto num = scanf("%f, %f, %f", &a, &b, &c);
        printf("接收到了 %d 个数字\n", num);
        printf("这三个数字是：%g, %g, %g\n", a, b, c);
    }

    vTaskDelete(nullptr);
}
