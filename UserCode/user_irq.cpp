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
#include "dac.h"
#include "Adc/adc_class_device.hpp"
#include "Lcd/lcd_device.hpp"
#include "ads1256/ads1256_device.hpp"
#include "App/Filters/butterworth.hpp"

#ifdef __cplusplus
extern "C" {
#endif

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);
void MY_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#ifdef __cplusplus
}
#endif

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == MainUart.uart_device.huart_.Instance) {
        MainUart.uart_device.TxCpltCallback();
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == MainUart.uart_device.huart_.Instance) {
        MainUart.uart_device.RxCpltCallback();
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
    if (huart->Instance == MainUart.uart_device.huart_.Instance) {
        MainUart.uart_device.RxEventCallback(Size);
    }
}

float TimPllInput = 0;
uint32_t TimStartUs, TimDuration;
control_system::Pll<float> pll(1.0 / 5000, 2 * M_PI * 50, 2);
control_system::SineGenerator<float> sine(2 * M_PI * 50, 1.0 / 5000.0, M_PI);

void MY_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3) {
        HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (sine.Step() + 1) * (4095.0 / 3.3));
        TimStartUs  = HPT_GetUs();
        TimPllInput = Adc1.GetVoltage(3);
        pll.Step(TimPllInput);
        TimDuration = HPT_GetUs() - TimStartUs;
    }
}

Butter_LP_5_20_40dB_5000Hz<float> kFilter;
float kFilterResult;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == Adc1.hadc_->Instance) {
        Adc1.ConvCpltCallback();
        kFilterResult = kFilter.Step(GetCoreTemperature());
    }
}

void LcdFmc_DmaXferCpltCallback(DMA_HandleTypeDef *_hdma)
{
    (void)_hdma;
    LCD.DmaXferCpltCallback();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch (GPIO_Pin) {
        case VDrdy_Pin:
            VAds.DRDY_Callback();
            break;

        default:
            break;
    }
}
