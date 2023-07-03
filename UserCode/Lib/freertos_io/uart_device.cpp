#include "uart_device.hpp"
#include "usart.h"

static freertos_io::Uart Uart1Device(huart1);
UartThread Uart1(Uart1Device, "Uart1");
