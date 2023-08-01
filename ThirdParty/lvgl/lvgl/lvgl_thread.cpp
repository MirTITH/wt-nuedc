#include "lvgl_thread.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRtosSys/thread_priority_def.h"
#include "freertos_lock/freertos_lock.hpp"
#include "Lcd/lcd_device.hpp"
#include "lv_app/lv_app.hpp"
#include "lv_app/main_page.hpp"
#include "Encoder/encoder_device.hpp"
#include <cmath>

freertos_lock::RecursiveMutex LvglMutex;
freertos_lock::BinarySemphr LvglThreadStartSem; // LvglThread 启动后会解锁这个信号量

void LvglLock()
{
    LvglMutex.lock();
}

void LvglUnlock()
{
    LvglMutex.unlock();
}

static void slider_event_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);

    LCD.SetBacklight(std::pow(lv_slider_get_value(slider) / 100.0f, 2));
}

static void LvglThreadEntry(void *argument)
{
    (void)argument;

    // 创建一个会动的东西，用于判断单片机是否卡住
    lv_obj_t *spinner = lv_spinner_create(lv_layer_top(), 1000, 60);
    lv_obj_set_size(spinner, 30, 30);
    lv_obj_set_style_arc_width(spinner, 3, LV_PART_MAIN);
    lv_obj_set_style_arc_width(spinner, 3, LV_PART_INDICATOR);
    lv_obj_align(spinner, LV_ALIGN_BOTTOM_RIGHT, -70, 0);

    /* 屏幕亮度滑条 */
    lv_obj_t *slider = lv_slider_create(lv_layer_top());
    lv_obj_set_width(slider, lv_pct(80));
    lv_obj_align(slider, LV_ALIGN_BOTTOM_MID, 0, -30);
    // lv_obj_set_style_opa(slider, 50, 0);
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, 50, LV_ANIM_ON);
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // 初始化 lv_app
    lv_app::LvApp_Init();
    lv_app::MainPage_Init();

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
    xTaskCreate(LvglThreadEntry, "lvgl_thread", 1024 * 2, nullptr, PriorityBelowNormal, nullptr);

    LvglThreadStartSem.lock(); // 等待 lvgl_thread 启动完毕
}
