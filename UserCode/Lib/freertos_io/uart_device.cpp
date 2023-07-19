#include "uart_device.hpp"
#include "usart.h"

static freertos_io::Uart MainUartDevice(huart1);
UartThread MainUart(MainUartDevice, "MainUart");
