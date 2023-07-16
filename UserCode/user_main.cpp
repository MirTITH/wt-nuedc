#include "user_main.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Test/user_test.hpp"
#include "tim.h"
#include "HighPrecisionTime/high_precision_time.h"
#include "lvgl/lvgl.h"
#include "lvgl/lv_port_disp.h"
#include "lvgl/lvgl_thread.h"

using namespace std;

void StartDefaultTask(void const *argument)
{
    (void)argument;

    // HAL_TIM_Base_Start_IT(&htim3);
    // HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    // HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

    HPT_Init();

    lv_init();
    lv_port_disp_init();
    // lv_port_indev_init();

    StartLvglThread();

    StartTestThread();

    vTaskDelete(nullptr); // 删除当前线程
}
