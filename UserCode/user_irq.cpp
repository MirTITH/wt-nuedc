#include "main.h"

#include "freertos_io/uart_device.hpp"
#include "HighPrecisionTime/high_precision_time.h"

#ifdef __cplusplus
extern "C" {
#endif

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);
void MY_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#ifdef __cplusplus
}
#endif

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        Uart1.uart_device.TxCpltCallback();
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        Uart1.uart_device.RxCpltCallback();
    }
}

/**
 * @brief  Reception Event Callback (Rx event notification called after use of advanced reception service).
 * @param  huart UART handle
 * @param  Size  Number of data available in application reception buffer (indicates a position in
 *               reception buffer until which, data are available)
 * @retval None
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART1) {
        Uart1.uart_device.RxEventCallback(Size);
    }
}

// static uint32_t kTimCounter;

// static uint32_t start_us, duration;

void MY_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3) {
        // start_us = HPT_GetUs();
        // printf("TIM3 Callback. Last print duration: %lu Count:%lu\n", duration, ++kTimCounter);
        // duration = HPT_GetUs() - start_us;
    }
}
