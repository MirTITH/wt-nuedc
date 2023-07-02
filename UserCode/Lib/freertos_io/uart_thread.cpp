#include "uart_thread.hpp"
#include "thread_priority_def.h"
#include "queue.h"
#include <assert.h>
#include "in_handle_mode.h"

void UartDaemonEntry(void *argument)
{
    auto this_class = (UartThread *)argument;

    char data;
    while (true) {
        xQueueReceive(this_class->xQueue, &data, portMAX_DELAY);
        this_class->uart_.Write(&data, 1);
    }
}

UartThread::UartThread(freertos_io::Uart &uart, const char *thread_name)
    : uart_(uart)
{
    xQueue = xQueueCreateStatic(sizeof(ucQueueStorageArea), 1, ucQueueStorageArea, &xStaticQueue);
    assert(xQueue != nullptr); /* &xStaticQueue was not NULL so xQueue should not be NULL. */
    xTaskCreate(UartDaemonEntry, thread_name, 512, this, PriorityHigh, &task_handle_);
}

void UartThread::Write(const char *data, size_t size)
{
    if (InHandlerMode()) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        for (size_t i = 0; i < size; i++) {
            xQueueSendFromISR(xQueue, &data[i], &xHigherPriorityTaskWoken);
        }

        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        taskENTER_CRITICAL();
        for (size_t i = 0; i < size; i++) {
            xQueueSend(xQueue, &data[i], portMAX_DELAY);
        }
        taskEXIT_CRITICAL();
    }
}
