#pragma once
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "control_system/pll.hpp"

namespace user_test
{
void PllTest()
{
    os_printf("==== Start %s ====\n", __func__);

    os_printf("==== End %s ====\n", __func__);
}

} // namespace user_test
