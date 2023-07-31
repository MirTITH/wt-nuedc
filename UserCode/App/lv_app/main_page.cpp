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
#include "Keyboard/keyboard_device.hpp"
#include "HighPrecisionTime/stat.hpp"
#include "Encoder/encoder_device.hpp"

using namespace std;

static lv_obj_t *kMainPage;
static lv_coord_t kContentWidth;

static void MainPage_ThreadFastLoop(void *)
{
    const uint32_t period = 100;

    LvglLock();
    LvTextField tf_keyboard(kMainPage, "Keyboard", (kContentWidth) / 2, 70, LvglTTF_GetFont());
    LvTextField tf_touch_screen(kMainPage, "触摸点数", (kContentWidth) / 2);
    LvTextField tf_encoder(kMainPage, "编码器，开关，编码器按钮", (kContentWidth));
    LvglUnlock();

    char str_buffer[20];

    uint32_t PreviousWakeTime = xTaskGetTickCount();
    while (1) {
        // tf_keyboard
        std::string str;
        for (size_t i = 0; i < 16; i++) {
            str.append(to_string(Keyboard.ReadKey(i)));
        }

        LvglLock();
        tf_keyboard.SetMsg(str);
        LvglUnlock();

        // TouchScreen
        snprintf(str_buffer, sizeof(str_buffer), "%u", TouchScreen.NumberOfTouchPoint());
        LvglLock();
        tf_touch_screen.SetMsg(str_buffer);
        LvglUnlock();

        // tf_encoder
        snprintf(str_buffer, sizeof(str_buffer), "%ld,%d,%d", KeyboardEncoder.Count(),
                 Keyboard.ReadSwitch(),
                 Keyboard.ReadKey(Key::kEncoderBtn));
        LvglLock();
        tf_encoder.SetMsg(str_buffer);
        LvglUnlock();

        vTaskDelayUntil(&PreviousWakeTime, period);
    }
}

static void MainPage_Thread(void *)
{
    const uint32_t period = 500;

    LvglLock();
    LvTextField tf_drdy(kMainPage, "VAds, IAds DRDY 频率");
    LvTextField tf_adc_rate(kMainPage, "内置 ADC 1,2,3 速率");
    LvTextField tf_fast_tim(kMainPage, "FastTim:频率,时间", (kContentWidth)*2 / 3);
    LvTextField tf_temperature(kMainPage, "内核温度", (kContentWidth) / 3);
    LvTextField tf_main_uart(kMainPage, "UART发送速率", (kContentWidth) / 2);
    LvTextField tf_adc_duration(kMainPage, "内置ADC中断时长", (kContentWidth) / 2);
    LvglUnlock();

    CounterFreqMeter vads_drdy_meter(&VAds.drdy_count_);
    CounterFreqMeter iads_drdy_meter(&IAds.drdy_count_);

    CounterFreqMeter adc_interrupt_meter1(&Adc1.conv_cplt_count);
    CounterFreqMeter adc_interrupt_meter2(&Adc2.conv_cplt_count);
    CounterFreqMeter adc_interrupt_meter3(&Adc3.conv_cplt_count);

    CounterFreqMeter fast_tim_meter(&kFastTimCallbackCount);
    CounterFreqMeter main_uart_meter(&MainUart.uart_device.total_tx_size_);

    char str_buffer[20];

    uint32_t PreviousWakeTime = xTaskGetTickCount();
    while (1) {
        // ADS1256
        snprintf(str_buffer, sizeof(str_buffer), "%lu,%lu",
                 vads_drdy_meter.MeasureFreq(), iads_drdy_meter.MeasureFreq());
        LvglLock();
        tf_drdy.SetMsg(str_buffer);
        LvglUnlock();

        // ADC
        snprintf(str_buffer, sizeof(str_buffer), "%lu,%lu,%lu",
                 adc_interrupt_meter1.MeasureFreq(),
                 adc_interrupt_meter2.MeasureFreq(),
                 adc_interrupt_meter3.MeasureFreq());
        LvglLock();
        tf_adc_rate.SetMsg(str_buffer);
        LvglUnlock();

        // FastTim
        snprintf(str_buffer, sizeof(str_buffer), "%lu,%lu", fast_tim_meter.MeasureFreq(), kFastTimCallbackDuration);
        LvglLock();
        tf_fast_tim.SetMsg(str_buffer);
        LvglUnlock();

        // MainUart
        snprintf(str_buffer, sizeof(str_buffer), "%lu", main_uart_meter.MeasureFreq());
        LvglLock();
        tf_main_uart.SetMsg(str_buffer);
        LvglUnlock();

        // Tempearture
        extern float kCoreTempearture;
        snprintf(str_buffer, sizeof(str_buffer), "%.1f", kCoreTempearture);
        LvglLock();
        tf_temperature.SetMsg(str_buffer);
        LvglUnlock();

        // adc_duration
        extern uint32_t kAdc1CallbackDuration;
        snprintf(str_buffer, sizeof(str_buffer), "%lu", kAdc1CallbackDuration);
        LvglLock();
        tf_adc_duration.SetMsg(str_buffer);
        LvglUnlock();

        vTaskDelayUntil(&PreviousWakeTime, period);
    }
}

void lv_app::MainPage_Init()
{
    LvglLock();
    kMainPage = lv_obj_create(lv_scr_act());
    lv_obj_set_style_border_side(kMainPage, LV_BORDER_SIDE_NONE, 0);
    lv_obj_set_size(kMainPage, lv_pct(100), 440); // 除去底部 monitor
    lv_obj_set_style_pad_hor(kMainPage, 4, 0);
    lv_obj_set_style_pad_ver(kMainPage, 0, 0);
    // lv_obj_set_style_pad_row(kMainPage, 4, 0);
    lv_obj_set_style_pad_column(kMainPage, 0, 0);
    lv_obj_set_flex_flow(kMainPage, LV_FLEX_FLOW_ROW_WRAP);
    // lv_obj_set_flex_align(kMainPage, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_update_layout(kMainPage);
    kContentWidth = lv_obj_get_content_width(kMainPage);
    LvglUnlock();

    xTaskCreate(MainPage_ThreadFastLoop, "main_page_fast", 1024 * 2, nullptr, PriorityBelowNormal, nullptr);
    vTaskDelay(100);
    xTaskCreate(MainPage_Thread, "main_page", 1024 * 2, nullptr, PriorityBelowNormal, nullptr);
}
