#include "User_VC.h"

void vc_loop_start(void *argument)
{
    (void)argument;
    Task_VC_init();

    extern uint32_t kTaskVcDuration, kTimCount;
    uint32_t last_count = kTimCount;
    while (1) {
        // Task_Vc_Loop_Spwm();
        // Task_Vc_Loop_SVpwm();
        uint32_t now_count = kTimCount;
        os_printf("%lu,%lu\n", kTaskVcDuration, now_count - last_count);
        last_count = now_count;
        vTaskDelay(100);
    }
    vTaskDelete(NULL); // 删除当前线程
}

void pll_loop_start(void *argument)
{
    (void)argument;
    PLL_init();
    vTaskDelete(NULL); // 删除当前线程
}

void task_vc_start()
{
    xTaskCreate(vc_loop_start, "vc_init", 1024, NULL, 4, NULL);
}

void task_pll_start()
{
    xTaskCreate(pll_loop_start, "pll_init", 128, NULL, 2, NULL);
}