#include "main.h"

#include "freertos_io/uart_device.hpp"

#ifdef __cplusplus
extern "C" {
#endif

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);

#ifdef __cplusplus
}
#endif

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        Uart1.TxCpltCallback();
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        Uart1.RxCpltCallback();
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
        Uart1.RxEventCallback(Size);
    }
}