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
    LvSimpleTextField tf_keyboard(kMainPage, "键盘", kContentWidth * 2 / 3);
    LvSimpleTextField tf_touch_screen(kMainPage, "触摸点数", kContentWidth / 3);
    LvSimpleTextField tf_encoder(kMainPage, "Enc,Sw", kContentWidth / 2);
    LvglUnlock();

    uint32_t PreviousWakeTime = xTaskGetTickCount();
    while (1) {
        // tf_keyboard
        std::string str;
        for (size_t i = 0; i < 16; i++) {
            str.append(to_string(Keyboard_Read((Keys)i)));
        }

        LvglLock();
        tf_keyboard.SetMsg(str);

        // TouchScreen
        lv_label_set_text_fmt(tf_touch_screen.GetMsgLabel(), "%u", TouchScreen.NumberOfTouchPoint());

        // tf_encoder
        lv_label_set_text_fmt(tf_encoder.GetMsgLabel(), "%ld,%d,%d", KeyboardEncoder.Count(),
                              Keyboard_Read(Keys::kSwitch),
                              Keyboard_Read(Keys::kKnobBtn));

        LvglUnlock();
        vTaskDelayUntil(&PreviousWakeTime, period);
    }
}

static void MainPage_Thread(void *)
{
    const uint32_t period = 1000;

    LvglLock();
    LvSimpleTextField tf_temperature(kMainPage, "内核温度", kContentWidth / 2);
    LvTextField tf_drdy(kMainPage, "VAds,IAds", kContentWidth, 70, LvglTTF_GetFont());
    LvSimpleTextField tf_adc_rate(kMainPage, "ADC123速率");
    LvTextField tf_fast_tim(kMainPage, "FastTim", kContentWidth / 2, 70, LvglTTF_GetFont());
    LvTextField tf_main_uart(kMainPage, "UART发送速率", kContentWidth / 2);
    LvglUnlock();

    CounterFreqMeter vads_drdy_meter(&VAds.drdy_count_);
    CounterFreqMeter iads_drdy_meter(&IAds.drdy_count_);

    CounterFreqMeter adc_interrupt_meter1(&Adc1.conv_cplt_count);
    CounterFreqMeter adc_interrupt_meter2(&Adc2.conv_cplt_count);
    CounterFreqMeter adc_interrupt_meter3(&Adc3.conv_cplt_count);

    CounterFreqMeter fast_tim_meter(&kFastTimCallbackCount);
    CounterFreqMeter main_uart_meter(&MainUart.uart_device.total_tx_size_);

    // char str_buffer[20];

    uint32_t PreviousWakeTime = xTaskGetTickCount();
    while (1) {
        LvglLock();
        // ADS1256
        lv_label_set_text_fmt(tf_drdy.GetMsgLabel(),
                              "drdy 频率: %lu, %lu\nbusy:%lu, %lu | err:%lu, %lu",
                              vads_drdy_meter.MeasureFreq(),
                              iads_drdy_meter.MeasureFreq(),
                              VAds.dma_busy_count_,
                              IAds.dma_busy_count_,
                              VAds.ads_err_count_,
                              IAds.ads_err_count_);

        // ADC
        lv_label_set_text_fmt(tf_adc_rate.GetMsgLabel(), "%lu,%lu,%lu",
                              adc_interrupt_meter1.MeasureFreq(),
                              adc_interrupt_meter2.MeasureFreq(),
                              adc_interrupt_meter3.MeasureFreq());

        // FastTim
        lv_label_set_text_fmt(tf_fast_tim.GetMsgLabel(),
                              "%lu Hz\n%lu us",
                              fast_tim_meter.MeasureFreq(), kFastTimCallbackDuration);

        // MainUart
        lv_label_set_text_fmt(tf_main_uart.GetMsgLabel(), "%lu", main_uart_meter.MeasureFreq());

        // Tempearture
        extern float kCoreTempearture;
        lv_label_set_text_fmt(tf_temperature.GetMsgLabel(), "%.3f", kCoreTempearture);

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
    lv_obj_set_flex_align(kMainPage, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_update_layout(kMainPage);
    kContentWidth = lv_obj_get_content_width(kMainPage);
    LvglUnlock();

    xTaskCreate(MainPage_ThreadFastLoop, "main_page_fast", 1024 * 2, nullptr, PriorityBelowNormal, nullptr);
    vTaskDelay(100);
    xTaskCreate(MainPage_Thread, "main_page", 1024 * 2, nullptr, PriorityBelowNormal, nullptr);
}
