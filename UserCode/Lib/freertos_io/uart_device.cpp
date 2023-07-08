#include "uart_device.hpp"
#include "usart.h"

static freertos_io::Uart MainUartDevice(huart3);
UartThread MainUart(MainUartDevice, "MainUart");
