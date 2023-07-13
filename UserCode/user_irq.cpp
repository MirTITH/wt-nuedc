/**
 * @file user_irq.cpp
 * @author X. Y.
 * @brief 所有的中断回调函数放在这个文件中
 * @version 0.1
 * @date 2023-07-06
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "main.h"
#include "freertos_io/uart_device.hpp"
#include "HighPrecisionTime/high_precision_time.h"
#include "freertos_io/os_printf.h"
#include "freertos_io/uart_device.hpp"
#include "Adc/adc_class_device.hpp"
#include "control_system/pll.hpp"
#include <cmath>
#include "control_system/signal_generator.hpp"

#ifdef __cplusplus
extern "C" {
#endif

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);
void MY_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);

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

float TimPllInput = 0;
uint32_t TimStartUs, TimDuration;
control_system::Pll<float> pll(1.0 / 5000, 2 * M_PI * 50, 2);
control_system::SineGenerator<float> sine(2 * M_PI * 50, 1.0 / 5000.0, M_PI);

void MY_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3) {
        TimStartUs  = HPT_GetUs();
        TimPllInput = sine.Step();
        pll.Step(TimPllInput);
        TimDuration = HPT_GetUs() - TimStartUs;
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == Adc1.hadc_->Instance) {
        Adc1.ConvCpltCallback();
    }
}
