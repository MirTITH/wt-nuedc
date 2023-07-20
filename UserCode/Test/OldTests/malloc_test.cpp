#pragma once
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "freertos_io/os_printf.h"
#include <stdlib.h>
#include <array>

std::array<int *, 10> ptrs;

void MallocTest()
{
    os_printf("==== Start %s ====\n", __func__);

    os_printf("==== End %s ====\n", __func__);
}

