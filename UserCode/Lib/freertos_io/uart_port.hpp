#pragma once

#include "main.h"

namespace freertos_io
{

namespace port
{
class uart_port
{
private:
    UART_HandleTypeDef &huart_;

    bool IsAddressValidForDma(const void *pData)
    {
        if ((size_t)pData < (0x10000000 + 64 * 1024) && (size_t)pData >= 0x10000000) {
            return false;
        }
        return true;
    }

public:
    uart_port(UART_HandleTypeDef &huart)
        : huart_(huart){};

    template <typename T>
    HAL_StatusTypeDef Write(const T *pData, uint16_t Size, uint32_t Timeout = HAL_MAX_DELAY)
    {
        return HAL_UART_Transmit(&huart_, (const uint8_t *)pData, Size, Timeout);
    }

    template <typename T>
    HAL_StatusTypeDef Read(T *pData, uint16_t Size, uint32_t Timeout = HAL_MAX_DELAY)
    {
        return HAL_UART_Receive(&huart_, pData, Size, Timeout);
    }

    template <typename T>
    HAL_StatusTypeDef WriteDma(const T *pData, uint16_t Size)
    {
        return HAL_UART_Transmit_DMA(&huart_, (const uint8_t *)pData, Size);
    }

    template <typename T>
    HAL_StatusTypeDef ReadDma(T *pData, uint16_t Size)
    {
        return HAL_UART_Receive_DMA(&huart_, pData, Size);
    }

    template <typename T>
    HAL_StatusTypeDef WriteIt(const T *pData, uint16_t Size)
    {
        return HAL_UART_Transmit_IT(&huart_, (const uint8_t *)pData, Size);
    }

    template <typename T>
    HAL_StatusTypeDef ReadIt(T *pData, uint16_t Size)
    {
        return HAL_UART_Receive_DMA(&huart_, pData, Size);
    }

    template <typename T>
    HAL_StatusTypeDef WriteNonBlock(const T *pData, uint16_t Size)
    {
        if (huart_.hdmatx != nullptr && IsAddressValidForDma(pData)) {
            return WriteDma(pData, Size);
        }

        return WriteIt(pData, Size);
    }
    template <typename T>
    HAL_StatusTypeDef ReadNonBlock(T *pData, uint16_t Size)
    {
        if (huart_.hdmatx != nullptr && IsAddressValidForDma(pData)) {
            return ReadDma(pData, Size);
        }

        return ReadIt(pData, Size);
    }
};

} // namespace port

} // namespace freertos_io
