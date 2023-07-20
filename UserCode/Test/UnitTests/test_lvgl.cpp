#pragma once

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "freertos_io/os_printf.h"
#include "HighPrecisionTime/high_precision_time.h"
#include "lvgl/lvgl.h"
#include "lvgl/lvgl_thread.h"
// #include "fonts/lvgl_ttf.h"

uint16_t Rgb888To565(uint32_t rgb888)
{
    return ((((rgb888) >> 19) & 0x1f) << 11) | ((((rgb888) >> 10) & 0x3f) << 5) | (((rgb888) >> 3) & 0x1f);
}

void TestLvgl()
{
    os_printf("====== Start %s ======\n", __func__);
    uint32_t start_us = HPT_GetUs();

    LvglLock();
    lv_obj_t *spinner = lv_spinner_create(lv_scr_act(), 1000, 60);
    lv_obj_set_size(spinner, 100, 100);
    lv_obj_center(spinner);
    LvglUnlock();

    uint32_t duration = HPT_GetUs() - start_us;
    os_printf("====== End %s Duration: %lu us ======\n", __func__, duration);
}
