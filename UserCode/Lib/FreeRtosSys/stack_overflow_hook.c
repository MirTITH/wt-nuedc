#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;

    fprintf(stderr, "Stack overflow! TaskHandle: 0x%x TaskName: %s\n", (unsigned int)xTask, pcTaskName);

    __disable_irq();

    while (1) {
        /* code */
    }
}
