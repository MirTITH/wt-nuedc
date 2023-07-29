#include "lvgl_thread.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRtosSys/thread_priority_def.h"
#include "freertos_lock/freertos_lock.hpp"
#include "fonts/lvgl_ttf.h"
#include "ads1256/ads1256_device.hpp"
#include "Lcd/lcd_device.hpp"
#include "Adc/adc_class_device.hpp"

freertos_lock::RecursiveMutex LvglMutex;
freertos_lock::BinarySemphr LvglThreadStartSem; // LvglThread 启动后会解锁这个信号量

lv_style_t Style_NormalFont;
lv_style_t Style_LargeFont;
lv_style_t Style_SmallFont;

void LvglLock()
{
    LvglMutex.lock();
}

void LvglUnlock()
{
    LvglMutex.unlock();
}

static void LvAdsMonitor(void *argument)
{
    auto ads = static_cast<Ads1256 *>(argument);

    const uint32_t period = 250;

    LvglLock();
    auto label_drdy = lv_label_create(lv_scr_act());
    lv_obj_add_style(label_drdy, &Style_NormalFont, 0);
    lv_label_set_text_fmt(label_drdy, "DRDY 频率：");
    lv_obj_align(label_drdy, LV_ALIGN_LEFT_MID, 5, 0);

    auto label_drdy_figure = lv_label_create(lv_scr_act());
    lv_obj_add_style(label_drdy_figure, &Style_LargeFont, 0);
    lv_label_set_text_fmt(label_drdy_figure, "0");
    lv_obj_align_to(label_drdy_figure, label_drdy, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

    auto label_adc = lv_label_create(lv_scr_act());
    lv_obj_add_style(label_adc, &Style_NormalFont, 0);
    lv_label_set_text_fmt(label_adc, "ADC 速率：");
    lv_obj_align(label_adc, LV_ALIGN_LEFT_MID, 5, 40);

    auto label_adc_figure = lv_label_create(lv_scr_act());
    lv_obj_add_style(label_adc_figure, &Style_NormalFont, 0);
    lv_label_set_text_fmt(label_adc_figure, "0,0,0");
    lv_obj_align_to(label_adc_figure, label_adc, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    LvglUnlock();

    auto last_count = ads->drdy_count_;
    uint32_t adc_now_count[3];
    uint32_t adc_last_count[3] = {};

    uint32_t PreviousWakeTime = xTaskGetTickCount();
    while (1) {
        auto now_count = ads->drdy_count_;

        adc_now_count[0] = Adc1.conv_cplt_count;
        adc_now_count[1] = Adc2.conv_cplt_count;
        adc_now_count[2] = Adc3.conv_cplt_count;

        LvglLock();
        lv_label_set_text_fmt(label_drdy_figure, "%lu", (now_count - last_count) * 1000 / period);
        lv_label_set_text_fmt(label_adc_figure, "%lu,%lu,%lu",
                              (adc_now_count[0] - adc_last_count[0]) * 1000 / period,
                              (adc_now_count[1] - adc_last_count[1]) * 1000 / period,
                              (adc_now_count[2] - adc_last_count[2]) * 1000 / period);
        LvglUnlock();

        last_count = now_count;
        for (size_t i = 0; i < 3; i++) {
            adc_last_count[i] = adc_now_count[i];
        }

        vTaskDelayUntil(&PreviousWakeTime, period);
    }
}

static void LvglThreadEntry(void *argument)
{
    (void)argument;

    lv_style_set_text_font(&Style_NormalFont, LvglTTF_GetFont());
    lv_style_set_text_font(&Style_LargeFont, LvglTTF_GetLargeFont());
    lv_style_set_text_font(&Style_SmallFont, LvglTTF_GetSmallFont());

    lv_obj_t *spinner = lv_spinner_create(lv_scr_act(), 1000, 60);
    lv_obj_set_size(spinner, 30, 30);
    lv_obj_set_style_arc_width(spinner, 3, LV_PART_MAIN);
    lv_obj_set_style_arc_width(spinner, 3, LV_PART_INDICATOR);
    lv_obj_align(spinner, LV_ALIGN_BOTTOM_RIGHT, -70, 0);

    LvglThreadStartSem.unlock(); // 线程初始化完毕，解锁信号量

    uint32_t PreviousWakeTime = xTaskGetTickCount();

    for (;;) {
        LvglLock();
        lv_timer_handler();
        LvglUnlock();

        vTaskDelayUntil(&PreviousWakeTime, 5);
    }
}

void StartLvglThread()
{
    // freetype 要的栈空间实在是太大了 qwq
    xTaskCreate(LvglThreadEntry, "lvgl_thread", 1024 * 2, nullptr, PriorityBelowNormal, nullptr);
    LCD.SetBacklight(0.5);
    LvglThreadStartSem.lock(); // 等待 lvgl_thread 启动完毕
    xTaskCreate(LvAdsMonitor, "ads_monitor", 1024 * 2, &VAds, PriorityBelowNormal, nullptr);
}
