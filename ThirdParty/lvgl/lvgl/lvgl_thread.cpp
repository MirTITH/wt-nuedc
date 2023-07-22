#include "lvgl_thread.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRtosSys/thread_priority_def.h"
#include "freertos_lock/freertos_lock.hpp"
#include "fonts/lvgl_ttf.h"

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

void lv_example_freetype_1(void)
{
    /*Create style with the new font*/
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_width(&style, lv_pct(90));

    /*Create a label with the new style*/
    lv_obj_t *label0 = lv_label_create(lv_scr_act());
    lv_obj_add_style(label0, &style, 0);
    lv_obj_add_style(label0, &Style_NormalFont, 0);
    lv_label_set_long_mode(label0, LV_LABEL_LONG_WRAP);
    lv_label_set_text(label0, "正常大小字体\nThis is normal sized font");
    lv_obj_align(label0, LV_ALIGN_TOP_MID, 0, 20);

    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_obj_add_style(label, &style, 0);
    lv_obj_add_style(label, &Style_LargeFont, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_label_set_text(label, "大就是好！\nBigger!");
    lv_obj_center(label);

    lv_obj_t *label2 = lv_label_create(lv_scr_act());
    lv_obj_add_style(label2, &style, 0);
    lv_obj_add_style(label2, &Style_SmallFont, 0);
    lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);
    lv_label_set_text(label2, "Tiny qute font\nThis does not support 中文");
    lv_obj_align(label2, LV_ALIGN_BOTTOM_MID, 0, -50);
}

static void LvglThreadEntry(void *argument)
{
    (void)argument;

    lv_style_set_text_font(&Style_NormalFont, LvglTTF_GetFont());
    lv_style_set_text_font(&Style_LargeFont, LvglTTF_GetLargeFont());
    lv_style_set_text_font(&Style_SmallFont, LvglTTF_GetSmallFont());

    LvglThreadStartSem.unlock(); // 线程初始化完毕，解锁信号量

    // LvglLock();
    // lv_example_freetype_1();
    // LvglUnlock();

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
    LvglThreadStartSem.lock(); // 等待 lvgl_thread 启动完毕
}
