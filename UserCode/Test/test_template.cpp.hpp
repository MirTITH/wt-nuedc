#pragma once
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "freertos_io/os_printf.h"

namespace user_test
{
void UntitledTest()
{
    os_printf("==== Start %s ====\n", __func__);

    os_printf("==== End %s ====\n", __func__);
}

} // namespace user_test
