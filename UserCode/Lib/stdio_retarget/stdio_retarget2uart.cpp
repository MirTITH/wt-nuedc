#include <errno.h>
#include <unistd.h>
#include "freertos_io/uart_device.hpp"

using namespace freertos_io;

// 串口号配置
static UartThread &stdout_uart = MainUart;
static Uart &stderr_uart       = MainUart.uart_device;
static UartThread &stdin_uart  = MainUart;

extern "C" {
int _read(int file, char *ptr, int len);
int _write(int file, char *ptr, int len);
}

int _read(int file, char *ptr, int len)
{
    (void)file;

    return stdin_uart.ReadToIdle(ptr, len);
}

int _write(int file, char *ptr, int len)
{
    switch (file) {
        case STDOUT_FILENO: // 标准输出流
            stdout_uart.Write(ptr, len);
            ;
            break;
        case STDERR_FILENO: // 标准错误流
            stderr_uart.WriteDirectly(ptr, len);
            break;
        default:
            // EBADF, which means the file descriptor is invalid or the file isn't opened for writing;
            errno = EBADF;
            return -1;
            break;
    }
    return len;
}