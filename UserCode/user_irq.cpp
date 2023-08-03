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
#include "Adc/adc_class_device.hpp"
#include "Lcd/lcd_device.hpp"
#include "ads1256/ads1256_device.hpp"
#include "Vofa/just_float.hpp"
#include "fast_tim_callback.hpp"
#include "HighPrecisionTime/stat.hpp"
#include "Encoder/encoder_device.hpp"
#include "Keyboard/keyboard_device.hpp"
#include "WatchDog/watchdog.hpp"
#include "Led/led_device.hpp"
#include "Relay/relay_device.hpp"
#include "line_calis.hpp"
#include <cmath>

#ifdef __cplusplus
extern "C" {
#endif

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);
void MY_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi);

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

void MY_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3) {
    } else if (htim->Instance == TIM6) {
        FastTimCallback();
    } else if (htim->Instance == TIM8) { // svpwm
        // kTaskVcStartUs = HPT_GetUs();

        // global_timer = HPT_GetUs() * 1e-6;

        // // 0.2ms spwm
        // Task_Vc_Loop_SVpwm(htim);

        // kTaskVcDuration = HPT_GetUs() - kTaskVcStartUs;
    }
}

#include "Filters/butterworth.hpp"
float kCoreTempearture;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    static Butter_LP_5_50_20dB_5000Hz<float> kTemperatureFilter;
    if (hadc->Instance == ADC1) {
        Adc1.ConvCpltCallback();
        kCoreTempearture = kTemperatureFilter.Step(GetCoreTemperature());
    } else if (hadc->Instance == ADC3) {
        Adc3.ConvCpltCallback();
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
        case IDrdy_Pin:
            IAds.DRDY_Callback();
            break;
        case Key_EncoderA_Pin:
            KeyboardEncoder.ExtiCallback();
            break;
        case Key_EncoderB_Pin:
            KeyboardEncoder.ExtiCallback();
            break;

        default:
            break;
    }
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == VAds.GetSpiHandle()->Instance) {
        VAds.SPI_TxRxCpltCallback();
    } else if (hspi->Instance == IAds.GetSpiHandle()->Instance) {
        IAds.SPI_TxRxCpltCallback();
    }
}

extern std::atomic<bool> kUserAppPrint;
