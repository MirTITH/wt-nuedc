#include "user_main.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Test/user_test.hpp"
#include "tim.h"
#include "HighPrecisionTime/high_precision_time.h"
#include "lvgl/lvgl.h"
#include "lv_port_disp.h"
#include "lvgl/lvgl_thread.h"
#include "freertos_io/os_printf.h"
#include "fonts/lvgl_ttf.h"

using namespace std;

void StartDefaultTask(void const *argument)
{
    (void)argument;

    // 时间库初始化，需要最先 init
    HPT_Init();

    // lvgl
    lv_init();
    lv_port_disp_init();
    LvglTTF_Init();
    StartLvglThread();

    StartTestThread();

    vTaskDelete(nullptr); // 删除当前线程
}
