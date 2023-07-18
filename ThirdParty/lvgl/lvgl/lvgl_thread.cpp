#include "lvgl_thread.h"
#include "lvgl/lvgl.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "freertos_io/os_printf.h"
#include "FreeRtosSys/thread_priority_def.h"
#include "lvgl_ttf/lvgl_ttf.h"

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
    auto font = LvglTtf_GetFont();

    /*Create style with the new font*/
    static lv_style_t style;
    lv_style_init(&style);
    if (font != NULL) {
        lv_style_set_text_font(&style, font);
    }
    lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_width(&style, lv_pct(90));

    /*Create a label with the new style*/
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_obj_add_style(label, &style, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_label_set_text(label, "Hello world\nI'm a font created with FreeType\n"
                             "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890\n"
                             "~!@#$%^&*()-_=+[{}]\\|;:'"
                             ",<.>?\\/"
                             "~·！@#￥%……&（）——++-=、|【{}】；：‘“，《。》？\n"
                             "相位电压电流频率角度效亮度");
    lv_obj_center(label);
    // lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);
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
