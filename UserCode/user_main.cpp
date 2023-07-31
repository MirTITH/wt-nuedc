#include "user_main.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Test/user_test.hpp"
#include "tim.h"
#include "HighPrecisionTime/high_precision_time.h"
#include "lvgl/lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lvgl/lvgl_thread.h"
#include "freertos_io/os_printf.h"
#include "fonts/lvgl_ttf.h"
#include "usercode_vc/User_VC.h"
#include "Adc/adc_class_device.hpp"
#include "Encoder/encoder_device.hpp"
#include "Keyboard/keyboard_device.hpp"
#include "user_app.hpp"

using namespace std;

void StartDefaultTask(void const *argument)
{
    (void)argument;

    // 时间库初始化，需要最先 init
    HPT_Init();

    KeyboardEncoder.Init();
    KeyboardScannerStart();

    Adc1.Init();
    Adc1.StartDma();

    // lvgl
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();
    LvglTTF_Init();
    StartLvglThread();

    HAL_TIM_Base_Start_IT(&htim6);

    // vector control
    // task_vc_start();
    // task_pll_start();

    StartTestThread();

    StartUserApp();

    vTaskDelete(nullptr); // 删除当前线程
}
