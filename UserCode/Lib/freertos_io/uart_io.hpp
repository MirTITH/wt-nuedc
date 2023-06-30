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
    using lk_guard             = std::lock_guard<BinarySemphr>;
    uint16_t read_to_idle_size = 0;

public:
    using uart_port::uart_port;

    /**
     * @brief 发送完成回调
     *
     * @note 在 void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) 中调用
     *
     */
    void TxCpltCallback()
    {
        write_mutex_.unlock();
    }

    /**
     * @brief 接收完成回调
     *
     * @note 在 void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) 中调用
     *
     */
    void RxCpltCallback()
    {
        read_mutex_.unlock();
    }

    /**
     * @brief 接收事件回调，目前用于空闲中断事件
     *
     * @note 在 void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) 中调用
     *
     * @param size
     */
    void RxEventCallback(uint16_t size)
    {
        read_to_idle_size = size;
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

    template <typename T>
    size_t ReadToIdle(T *pData, uint16_t Size)
    {
        if (Size == 0) {
            return HAL_OK;
        }

        read_mutex_.lock();

        HAL_StatusTypeDef result;

        if (UseDmaRx() && IsAddressValidForDma(pData)) {
            result = ReadToIdleDma((uint8_t *)pData, Size);
        } else {
            result = ReadToIdleIt((uint8_t *)pData, Size);
        }

        if (result != HAL_OK) {
            return 0;
        }

        {
            lk_guard lock(read_mutex_);
            return read_to_idle_size;
        }
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
