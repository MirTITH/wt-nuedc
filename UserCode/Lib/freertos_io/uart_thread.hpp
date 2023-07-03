#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include "ring_buffer.hpp"
#include "uart_io.hpp"
#include <string>
#include "freertos_lock/freertos_semphr.hpp"

class UartThread
{
private:
    TaskHandle_t task_handle_;
    RingBuffer<128> ring_buffer_;
    BinarySemphr sem_;

public:
    freertos_io::Uart &uart_device;

    UartThread(freertos_io::Uart &uart, const char *thread_name);

    void Write(const char *data, size_t size);
    void Write(const std::string str);

    template <typename T>
    size_t ReadToIdle(T *pData, uint16_t Size)
    {
        return uart_device.ReadToIdle(pData, Size);
    }

    friend void UartDaemonEntry(void *argument);
};
