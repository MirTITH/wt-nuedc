#include "lvgl_thread.h"
#include "lvgl/lvgl.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

static SemaphoreHandle_t LvglMutex;

void LvglLock()
{
    xSemaphoreTakeRecursive(LvglMutex, portMAX_DELAY);
}

void LvglUnlock()
{
    xSemaphoreGiveRecursive(LvglMutex);
}

static void LvglThreadEntry(void *argument)
{
    (void)argument;

    uint32_t PreviousWakeTime = xTaskGetTickCount();

    lv_obj_t *spinner = lv_spinner_create(lv_scr_act(), 1000, 60);
    lv_obj_set_size(spinner, 100, 100);
    lv_obj_center(spinner);

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
    xTaskCreate(LvglThreadEntry, "lvgl_thread", 2048, nullptr, 3, nullptr);
}
