#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "uart_io.hpp"

class UartThread
{
private:
    TaskHandle_t task_handle_;
    StaticQueue_t xStaticQueue;
    QueueHandle_t xQueue;
    uint8_t ucQueueStorageArea[128];
    freertos_io::Uart &uart_;

    friend void UartDaemonEntry(void *argument);

public:
    UartThread(freertos_io::Uart &uart, const char *thread_name);

    void Write(const char *data, size_t size);
};
