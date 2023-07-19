#include "lvgl_thread.h"
#include "lvgl/lvgl.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "freertos_io/os_printf.h"
#include "FreeRtosSys/thread_priority_def.h"
#include "fonts/lvgl_ttf.h"

static SemaphoreHandle_t LvglMutex;

void LvglLock()
{
    xSemaphoreTakeRecursive(LvglMutex, portMAX_DELAY);
}

void LvglUnlock()
{
    xSemaphoreGiveRecursive(LvglMutex);
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
    lv_obj_set_style_text_font(label0, LvglTTF_GetFont(), 0);
    lv_label_set_long_mode(label0, LV_LABEL_LONG_WRAP);
    lv_label_set_text(label0, "正常大小字体\nThis is normal sized font");
    lv_obj_align(label0, LV_ALIGN_TOP_MID, 0, 20);

    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_obj_add_style(label, &style, 0);
    lv_obj_set_style_text_font(label, LvglTTF_GetLargeFont(), 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_label_set_text(label, "大就是好！\nBigger!");
    lv_obj_center(label);

    lv_obj_t *label2 = lv_label_create(lv_scr_act());
    lv_obj_add_style(label2, &style, 0);
    lv_obj_set_style_text_font(label2, LvglTTF_GetSmallFont(), 0);
    lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);
    lv_label_set_text(label2, "Tiny qute font\nThis does not support 中文");
    lv_obj_align(label2, LV_ALIGN_BOTTOM_MID, 0, -50);
}

static void LvglThreadEntry(void *argument)
{
    (void)argument;

    lv_example_freetype_1();

    lv_obj_t *spinner = lv_spinner_create(lv_scr_act(), 1000, 60);
    lv_obj_set_size(spinner, 100, 100);
    lv_obj_center(spinner);

    // static lv_ft_info_t info;
    // info.name     = nullptr;
    // info.weight   = 64;
    // info.style    = FT_FONT_STYLE_BOLD;
    // info.mem      = TTF_MEM_START;
    // info.mem_size = TTF_MEM_SIZE;
    // if (!lv_ft_font_init(&info)) {
    //     LV_LOG_ERROR("create failed.");
    // }

    // static lv_style_t style;
    // lv_style_init(&style);
    // lv_style_set_text_font(&style, info.font);
    // lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);
    // lv_style_set_width(&style, lv_pct(90));

    /*Create a label with the new style*/
    // lv_obj_t *label = lv_label_create(lv_scr_act());
    // lv_obj_add_style(label, &style, 0);
    // lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    // lv_label_set_text(label, "电压：\n3.32V");
    // lv_obj_center(label);

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
    LvglMutex = xSemaphoreCreateRecursiveMutex();
    // freetype 要的栈空间实在是太大了 qwq
    xTaskCreate(LvglThreadEntry, "lvgl_thread", 8192, nullptr, PriorityBelowNormal, nullptr);
}
