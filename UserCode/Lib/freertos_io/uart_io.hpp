#pragma once

#include "uart_port.hpp"

#include <string>

namespace freertos_io
{
class Uart : public port::uart_port
{
private:
public:
    using uart_port::uart_port;

    auto Write(const std::string str, uint32_t Timeout = HAL_MAX_DELAY)
    {
        return uart_port::Write(str.c_str(), str.size(), Timeout);
    }
};

} // namespace freertos_io
