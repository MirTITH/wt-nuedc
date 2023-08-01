#include "lvgl_thread.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRtosSys/thread_priority_def.h"
#include "freertos_lock/freertos_lock.hpp"
#include "Lcd/lcd_device.hpp"
#include "lv_app/lv_app.hpp"
#include "lv_app/main_page.hpp"

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

static void LvglThreadEntry(void *argument)
{
    (void)argument;

    // 创建一个会动的东西，用于判断单片机是否卡住
    lv_obj_t *spinner = lv_spinner_create(lv_layer_top(), 1000, 60);
    lv_obj_set_size(spinner, 30, 30);
    lv_obj_set_style_arc_width(spinner, 3, LV_PART_MAIN);
    lv_obj_set_style_arc_width(spinner, 3, LV_PART_INDICATOR);
    lv_obj_align(spinner, LV_ALIGN_BOTTOM_RIGHT, -70, 0);

    lv_app::LvApp_Init();

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
    LCD.SetBacklight(0.1);
    LvglThreadStartSem.lock(); // 等待 lvgl_thread 启动完毕
    lv_app::MainPage_Init();
}
