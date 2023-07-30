#include "main_page.hpp"
#include "lvgl/lvgl.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ads1256/ads1256_device.hpp"
#include <string>
#include "thread_priority_def.h"
#include "lvgl/lvgl_thread.h"
#include "lv_app_text_field.hpp"
#include "Adc/adc_class_device.hpp"
#include "fast_tim_callback.hpp"
#include "TouchScreen/GT911/gt911_device.hpp"
#include "freertos_io/uart_device.hpp"
#include <cmath>

using namespace std;

static void MainPage_Thread(void *)
{
    const uint32_t period = 500;

    LvglLock();
    lv_obj_t *cont_col = lv_obj_create(lv_scr_act());
    lv_obj_set_style_border_side(cont_col, LV_BORDER_SIDE_NONE, 0);
    lv_obj_set_size(cont_col, lv_pct(100), 440); // 除去底部 monitor
    lv_obj_set_style_pad_hor(cont_col, 4, 0);
    lv_obj_set_style_pad_ver(cont_col, 0, 0);
    // lv_obj_set_style_pad_row(cont_col, 4, 0);
    lv_obj_set_style_pad_column(cont_col, 0, 0);
    lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_ROW_WRAP);
    // lv_obj_set_flex_align(cont_col, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_update_layout(cont_col);
    auto cont_col_width = lv_obj_get_content_width(cont_col);

    LvTextField tf_drdy(cont_col, "VAds, IAds DRDY 频率");
    LvTextField tf_adc_rate(cont_col, "内置 ADC 1,2,3 速率");
    LvTextField tf_fast_tim(cont_col, "FastTim:频率,时间", (cont_col_width)*2 / 3);
    LvTextField tf_touch_screen(cont_col, "触摸点数", (cont_col_width) / 3);
    LvTextField tf_main_uart(cont_col, "UART发送速率", (cont_col_width) / 2);
    LvTextField tf_temperature(cont_col, "内核温度", (cont_col_width) / 2);
    LvTextField tf_adc_duration(cont_col, "内置ADC中断时长", (cont_col_width) / 2);

    LvglUnlock();

    auto vads_last_count = VAds.drdy_count_;
    auto iads_last_count = IAds.drdy_count_;
    uint32_t adc_now_count[3];
    uint32_t adc_last_count[3] = {};
    auto fast_tim_last_count   = kFastTimCallbackCount;
    auto main_uart_last_tx     = MainUart.uart_device.total_tx_size_;

    // std::stringstream sstr;

    std::string str;

    uint32_t PreviousWakeTime = xTaskGetTickCount();
    while (1) {
        // ADS1256
        auto vads_now_count = VAds.drdy_count_;
        auto iads_now_count = IAds.drdy_count_;

        str = to_string((vads_now_count - vads_last_count) * 1000 / period).append(",") +
              to_string((vads_now_count - iads_last_count) * 1000 / period);

        vads_last_count = vads_now_count;
        iads_last_count = iads_now_count;
        LvglLock();
        tf_drdy.SetMsg(str);
        LvglUnlock();

        // ADC
        adc_now_count[0] = Adc1.conv_cplt_count;
        adc_now_count[1] = Adc2.conv_cplt_count;
        adc_now_count[2] = Adc3.conv_cplt_count;

        str = to_string((adc_now_count[0] - adc_last_count[0]) * 1000 / period).append(",") +
              to_string((adc_now_count[1] - adc_last_count[1]) * 1000 / period).append(",") +
              to_string((adc_now_count[2] - adc_last_count[2]) * 1000 / period);

        for (size_t i = 0; i < 3; i++) {
            adc_last_count[i] = adc_now_count[i];
        }
        LvglLock();
        tf_adc_rate.SetMsg(str);
        LvglUnlock();

        // FastTim
        auto fast_tim_now_count = kFastTimCallbackCount;

        str = to_string((fast_tim_now_count - fast_tim_last_count) * 1000 / period).append(",") +
              to_string(kFastTimCallbackDuration);

        fast_tim_last_count = fast_tim_now_count;
        LvglLock();
        tf_fast_tim.SetMsg(str);
        LvglUnlock();

        // TouchScreen
        str = to_string(TouchScreen.NumberOfTouchPoint());
        LvglLock();
        tf_touch_screen.SetMsg(str);
        LvglUnlock();

        // MainUart
        auto main_uart_now_tx = MainUart.uart_device.total_tx_size_;
        str                   = to_string((main_uart_now_tx - main_uart_last_tx) * 1000 / period);
        LvglLock();
        tf_main_uart.SetMsg(str);
        LvglUnlock();
        main_uart_last_tx = main_uart_now_tx;

        // Tempearture
        char temp[10];
        extern float kCoreTempearture;
        snprintf(temp, sizeof(temp), "%.2f", kCoreTempearture);
        LvglLock();
        tf_temperature.SetMsg(temp);
        LvglUnlock();

        // adc_duration
        extern uint32_t kAdc1CallbackDuration;
        str = to_string(kAdc1CallbackDuration);
        LvglLock();
        tf_adc_duration.SetMsg(str);
        LvglUnlock();

        vTaskDelayUntil(&PreviousWakeTime, period);
    }
}

void lv_app::MainPage_Init()
{
    xTaskCreate(MainPage_Thread, "main_page", 1024 * 2, nullptr, PriorityBelowNormal, nullptr);
}