#include <stdint.h>
#include "freertos_io/os_printf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "HighPrecisionTime/high_precision_time.h"


void PrintTestTask(void *argument)
{
    uint32_t counter = 0;

    uint32_t start_us;
    uint32_t duration = 0;

    while (1) {
        start_us = HPT_GetUs();
        os_printf("%s. duration:%lu. Count: %lu\n", (const char *)argument, duration, ++counter);
        duration = (HPT_GetUs() - start_us);

        vTaskDelay(100);
    }
}
