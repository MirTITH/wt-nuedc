#include "user_main.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"
#include <string>
#include "control_system/z_tf.hpp"
#include <sstream>
#include "HighPrecisionTime/high_precision_time.h"
#include "assert.h"

using namespace std;

stringstream sstr;

void StartDefaultTask(void const *argument)
{
    (void)argument;

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

    while (true) {
        HAL_GPIO_TogglePin(Led2_GPIO_Port, Led2_Pin);

        vTaskDelay(500);
    }
}
