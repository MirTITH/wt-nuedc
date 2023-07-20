#pragma once

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "freertos_io/os_printf.h"
#include "HighPrecisionTime/high_precision_time.h"

void TestTemplate()
{
    os_printf("====== Start %s ======\n", __func__);
    uint32_t start_us = HPT_GetUs();

    // Your code here

    uint32_t duration = HPT_GetUs() - start_us;
    os_printf("====== End %s Duration: %lu us ======\n", __func__, duration);
}
