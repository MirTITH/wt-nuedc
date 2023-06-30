#include "FreeRTOS.h"
#include "task.h"

void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;
    while (1) {
        /* code */
    }
}
