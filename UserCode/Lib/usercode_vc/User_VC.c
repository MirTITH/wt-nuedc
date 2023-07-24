#include "User_VC.h"

void vc_loop_start(void *argument)
{
    (void)argument;
    Task_VC_init();
    while(1) {
        // Task_Vc_Loop_Spwm();
        vTaskDelay(1);
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
    xTaskCreate(vc_loop_start, "vc_loop", 1024, NULL, 4, NULL);
}

void task_pll_start()
{
    xTaskCreate(pll_loop_start, "pll_init", 128, NULL, 2, NULL);
}