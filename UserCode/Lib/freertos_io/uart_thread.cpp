#include "uart_thread.hpp"
#include "thread_priority_def.h"
#include "queue.h"
#include <assert.h>
#include "in_handle_mode.h"
#include "HighPrecisionTime/high_precision_time.h"

void UartDaemonEntry(void *argument)
{
    auto this_class    = (UartThread *)argument;
    auto &uart_device  = this_class->uart_device;
    auto &ring_buffer_ = this_class->ring_buffer_;

    size_t continous_read_size, size;

    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        taskENTER_CRITICAL();
        continous_read_size = ring_buffer_.get_continous_read_size(); // 可以一次连续写入串口的数据大小
        size                = ring_buffer_.size();                    // 总写入串口的数据大小
        taskEXIT_CRITICAL();

        if (size == continous_read_size) {
            // 可以一次连续写入，所以一次写入
            uart_device.WriteNonBlock(ring_buffer_.get_read_address(), continous_read_size);
        } else {
            // 不可以一次连续写入，需要分两次写
            uart_device.WriteNonBlock(ring_buffer_.get_read_address(), continous_read_size);      // 第一部分
            uart_device.WriteNonBlock(ring_buffer_.get_buffer_ptr(), size - continous_read_size); // 第二部分（折返到缓冲区开头）
        }

        // 等待写入完成
        uart_device.WaitForWriteCplt();

        taskENTER_CRITICAL();
        // 从缓冲区中删掉已写入的数据
        ring_buffer_.remove_front(size);
        taskEXIT_CRITICAL();
    }
}

UartThread::UartThread(freertos_io::Uart &uart, const char *thread_name)
    : uart_device(uart)
{
    xTaskCreate(UartDaemonEntry, thread_name, 256, this, PriorityHigh, &task_handle_);
}

void UartThread::Write(const char *data, size_t size)
{
    if (InHandlerMode()) {
        UBaseType_t uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
        ring_buffer_.push_data_back((const uint8_t *)data, size);
        taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(task_handle_, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        taskENTER_CRITICAL();
        ring_buffer_.push_data_back((const uint8_t *)data, size);
        taskEXIT_CRITICAL();
        xTaskNotifyGive(task_handle_);
    }
}

void UartThread::Write(const std::string str)
{
    Write(str.c_str(), str.size());
}
