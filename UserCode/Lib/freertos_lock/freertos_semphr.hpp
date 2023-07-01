#pragma once

#include "semphr.h"
#include "in_handle_mode.h"

class BinarySemphr
{
private:
    StaticSemaphore_t sem_buffer_;
    SemaphoreHandle_t sem_ = nullptr;

public:
    /**
     * @brief 创建一个信号量，注意，和 FreeRTOS 默认行为不同，这里创建的信号量直接是可用状态（解锁状态）
     *
     */
    BinarySemphr()
    {
        sem_ = xSemaphoreCreateBinaryStatic(&sem_buffer_);
        unlock();
    }

    /**
     * @brief 返回信号量计数
     *
     * @return 当信号量可用时，返回 1，当信号量不可用时， 返回 0。
     */
    UBaseType_t get_count()
    {
        return uxSemaphoreGetCount(sem_);
    }

    /**
     * @brief 上锁，如果已上锁，则会等待直到被别人解锁，然后再上锁。但是，在中断中，如果已上锁，则直接返回上锁失败而不会等待
     *
     * @return true 成功上锁
     * @return false 上锁失败
     */
    bool lock()
    {
        BaseType_t result;

        if (InHandlerMode()) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xHigherPriorityTaskWoken            = pdFALSE;

            result = xSemaphoreTakeFromISR(sem_, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        } else {
            result = xSemaphoreTake(sem_, portMAX_DELAY);
        }

        return result;
    }

    bool unlock()
    {
        BaseType_t result;

        if (InHandlerMode()) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xHigherPriorityTaskWoken            = pdFALSE;
            result                              = xSemaphoreGiveFromISR(sem_, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        } else {
            result = xSemaphoreGive(sem_);
        }

        return result;
    }
};
