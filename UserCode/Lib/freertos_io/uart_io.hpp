#pragma once

#include "uart_port.hpp"
#include "FreeRTOS.h"
#include "freertos_lock/freertos_semphr.hpp"
#include <string>
#include <mutex>

namespace freertos_io
{
class Uart : public port::uart_port
{
private:
    BinarySemphr write_mutex_;
    BinarySemphr read_mutex_;
    using lk_guard = std::lock_guard<BinarySemphr>;

public:
    using uart_port::uart_port;

    void TxCpltCallback()
    {
        write_mutex_.unlock();
    }

    void RxCpltCallback()
    {
        read_mutex_.unlock();
    }

    auto Write(const std::string &str, uint32_t Timeout = HAL_MAX_DELAY)
    {
        lk_guard lock(write_mutex_);
        return uart_port::Write((const uint8_t *)str.c_str(), str.size(), Timeout);
    }

    template <typename T>
    auto Write(const T *pData, uint16_t Size, uint32_t Timeout = HAL_MAX_DELAY)
    {
        lk_guard lock(write_mutex_);
        return uart_port::Write(pData, Size, Timeout);
    }

    template <typename T>
    auto Read(T *pData, uint16_t Size, uint32_t Timeout = HAL_MAX_DELAY)
    {
        lk_guard lock(read_mutex_);
        return uart_port::Read((uint8_t *)pData, Size, Timeout);
    }

    template <typename T>
    auto WriteNonBlock(const T *pData, uint16_t Size)
    {
        if (Size == 0) {
            return HAL_OK;
        }

        write_mutex_.lock();

        if (UseDmaTx() && IsAddressValidForDma(pData)) {
            return WriteDma((const uint8_t *)pData, Size);
        }

        return WriteIt((const uint8_t *)pData, Size);
    }

    auto WriteNonBlock(const std::string &str)
    {
        if (str.size() == 0) {
            return HAL_OK;
        }

        write_mutex_.lock();

        if (UseDmaTx() && IsAddressValidForDma(str.c_str())) {
            return WriteDma((const uint8_t *)str.c_str(), str.size());
        }

        return WriteIt((const uint8_t *)str.c_str(), str.size());
    }

    template <typename T>
    auto ReadNonBlock(T *pData, uint16_t Size)
    {
        if (Size == 0) {
            return HAL_OK;
        }

        read_mutex_.lock();

        if (UseDmaRx() && IsAddressValidForDma(pData)) {
            return ReadDma((uint8_t *)pData, Size);
        }

        return ReadIt((uint8_t *)pData, Size);
    }

    void WaitForWriteCplt()
    {
        lk_guard lock(write_mutex_);
    }

    void WaitForReadCplt()
    {
        lk_guard lock(read_mutex_);
    }
};

} // namespace freertos_io
