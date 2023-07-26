/**
 * @file isr_in_thread.hpp
 * @author X. Y.
 * @brief 通过中断触发线程运行，将中断代码放在线程中执行
 * @version 0.1
 * @date 2023-07-25
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include "thread_priority_def.h"
#include <atomic>
#include "in_handle_mode.h"
#include <functional>

class IsrInThread
{
    using Func_t = std::function<void(void)>;

private:
    TaskHandle_t task_handle_ = nullptr;
    Func_t func_to_call_;

    static void ThreadEntry(void *this_class)
    {
        while (true) {
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
            static_cast<IsrInThread *>(this_class)->func_to_call_();
        }
    }

public:
    void Init(const Func_t &func, const char *pcName, uint16_t usStackDepth = 512, UBaseType_t uxPriority = PriorityHigh)
    {
        func_to_call_ = func;
        xTaskCreate(ThreadEntry, pcName, usStackDepth, (void *)this, uxPriority, &task_handle_);
    }

    void CallFunc()
    {
        if (InHandlerMode()) {
            CallFuncFromIsr();
        } else {
            CallFuncFromThread();
        }
    }

    void CallFuncFromIsr()
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(task_handle_, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

    void CallFuncFromThread()
    {
        xTaskNotifyGive(task_handle_);
    }
};
