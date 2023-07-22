#include "tempearture_display.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "thread_priority_def.h"
#include "lvgl/lvgl.h"
#include "lvgl/lvgl_thread.h"
#include "Adc/adc_class_device.hpp"
#include "freertos_io/os_printf.h"

void TemperatureDisplayEntry(void *argument)
{
    (void)argument;

    LvglLock();
    auto temperature_frame = lv_obj_create(lv_scr_act());
    lv_obj_set_align(temperature_frame, LV_ALIGN_LEFT_MID);
    lv_obj_set_width(temperature_frame, lv_pct(60));
    lv_obj_set_height(temperature_frame, 100);

    auto temperature_frame_title = lv_label_create(temperature_frame);
    lv_obj_add_style(temperature_frame_title, &Style_NormalFont, 0);
    lv_label_set_text_static(temperature_frame_title, "温度");

    auto temperature_label = lv_label_create(temperature_frame);
    lv_obj_add_style(temperature_label, &Style_LargeFont, 0);
    lv_obj_set_style_text_align(temperature_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(temperature_label, lv_pct(100));
    lv_label_set_long_mode(temperature_label, LV_LABEL_LONG_SCROLL);
    lv_obj_align(temperature_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    auto info_label = lv_label_create(lv_scr_act());
    lv_obj_add_style(info_label, &Style_NormalFont, 0);
    lv_label_set_text(info_label, "滤波前温度：\nAdc 采样速率: "); // 先给两行，以便 lv_obj_align_to 能正确对齐
    lv_obj_align_to(info_label, temperature_frame, LV_ALIGN_OUT_TOP_LEFT, 10, -10);
    LvglUnlock();

    uint32_t last_adc_count = Adc1.conv_cplt_count;
    auto last_ms            = xTaskGetTickCount();
    extern volatile float kFilterResult;

    while (true) {
        LvglLockGuard();

        auto now_ms = xTaskGetTickCount();

        taskENTER_CRITICAL();
        auto tempFilterResult = kFilterResult;
        auto now_adc_count    = Adc1.conv_cplt_count;
        taskEXIT_CRITICAL();

        auto adc_count_per_sec = 1000 * (now_adc_count - last_adc_count) / (now_ms - last_ms);
        last_adc_count         = now_adc_count;
        last_ms                = now_ms;

        lv_label_set_text_fmt(temperature_label, "%.2f", tempFilterResult);

        lv_label_set_text_fmt(info_label, "巴特沃斯滤波前温度：%.2f\nAdc 采样速率: %lu", GetCoreTemperature(), adc_count_per_sec);

        vTaskDelay(100);
    }
}

void StartTemperatureDisplayThread()
{
    xTaskCreate(TemperatureDisplayEntry, "TemperatureDisp", 2048, nullptr, PriorityLow, nullptr);
}