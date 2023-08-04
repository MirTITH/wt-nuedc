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
#include "Adc/adc_class_device.hpp"
#include "Encoder/encoder_device.hpp"
#include "Keyboard/keyboard_device.hpp"
#include "Led/led_device.hpp"
#include "ads1256/ads1256_device.hpp"
#include "common_objs.hpp"
#include "line_calis.hpp"

using namespace std;

void StartDefaultTask(void const *argument)
{
    (void)argument;
    KeyboardLed.Power(true);

    // 时间库初始化，需要最先 init
    HPT_Init();

    KeyboardEncoder.Init();
    KeyboardScannerStart();

    if (Keyboard_Read(Keys::kSwitch)) {
        kWhoAmI = BoardSelector::A; // H
    } else {
        kWhoAmI = BoardSelector::B; // L
    }

    switch (kWhoAmI) {
        case BoardSelector::A:
            kLineCali_IAds.k_    = 4.7898327605292200;
            kLineCali_IAds.b_    = 0.0003700653449598;
            kLineCali_VAds.k_    = 31.6282631438288000;
            kLineCali_VAds.b_    = -0.0113041469861519;
            kLineCali_GridAdc.k_ = 49.7318049981908000;
            kLineCali_GridAdc.b_ = 0.2220309448129640;
            break;
        case BoardSelector::B:
            kLineCali_IAds.k_    = 4.8500909335328100;
            kLineCali_IAds.b_    = 0.0004955869787681;
            kLineCali_VAds.k_    = 31.6595871611746000;
            kLineCali_VAds.b_    = -0.1223529377663620;
            kLineCali_GridAdc.k_ = 45.8800148031247000;
            kLineCali_GridAdc.b_ = -0.0314225827423855;
            break;
        default:
            break;
    }

    // lvgl
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();
    LvglTTF_Init();
    StartLvglThread();

    // Adc1.Init();
    // Adc1.StartDma();
    Adc3.Init();
    Adc3.StartDma();

    InitAds();

    HAL_TIM_Base_Start_IT(&htim6); // FastTim Callback

    StartTestThread();

    vTaskDelete(nullptr); // 删除当前线程
}
