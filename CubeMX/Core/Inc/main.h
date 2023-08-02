/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Key_Addr2_Pin GPIO_PIN_2
#define Key_Addr2_GPIO_Port GPIOE
#define Key_Addr3_Pin GPIO_PIN_3
#define Key_Addr3_GPIO_Port GPIOE
#define Key_LedG_Pin GPIO_PIN_5
#define Key_LedG_GPIO_Port GPIOE
#define Key_LedR_Pin GPIO_PIN_6
#define Key_LedR_GPIO_Port GPIOE
#define Key_Addr1_Pin GPIO_PIN_13
#define Key_Addr1_GPIO_Port GPIOC
#define ElectricRelay_GridConnector_Pin GPIO_PIN_0
#define ElectricRelay_GridConnector_GPIO_Port GPIOA
#define ElectricRelay_LoadConnector_Pin GPIO_PIN_1
#define ElectricRelay_LoadConnector_GPIO_Port GPIOA
#define ElectricRelay_BridgeA_Pin GPIO_PIN_2
#define ElectricRelay_BridgeA_GPIO_Port GPIOA
#define ElectricRelay_BridgeB_Pin GPIO_PIN_3
#define ElectricRelay_BridgeB_GPIO_Port GPIOA
#define Key_Addr0_Pin GPIO_PIN_4
#define Key_Addr0_GPIO_Port GPIOC
#define Key_Data_Pin GPIO_PIN_5
#define Key_Data_GPIO_Port GPIOC
#define ISync_Pin GPIO_PIN_2
#define ISync_GPIO_Port GPIOB
#define IAds_nReset_Pin GPIO_PIN_10
#define IAds_nReset_GPIO_Port GPIOB
#define IAds_nCs_Pin GPIO_PIN_12
#define IAds_nCs_GPIO_Port GPIOB
#define IDrdy_Pin GPIO_PIN_11
#define IDrdy_GPIO_Port GPIOD
#define IDrdy_EXTI_IRQn EXTI15_10_IRQn
#define Key_LedB_Pin GPIO_PIN_12
#define Key_LedB_GPIO_Port GPIOD
#define Key_EncoderBtn_Pin GPIO_PIN_13
#define Key_EncoderBtn_GPIO_Port GPIOD
#define LcdBacklight_Pin GPIO_PIN_8
#define LcdBacklight_GPIO_Port GPIOA
#define VDrdy_Pin GPIO_PIN_15
#define VDrdy_GPIO_Port GPIOA
#define VDrdy_EXTI_IRQn EXTI15_10_IRQn
#define VAds_nCs_Pin GPIO_PIN_2
#define VAds_nCs_GPIO_Port GPIOD
#define VAds_nReset_Pin GPIO_PIN_6
#define VAds_nReset_GPIO_Port GPIOD
#define VSync_Pin GPIO_PIN_3
#define VSync_GPIO_Port GPIOB
#define Key_EncoderA_Pin GPIO_PIN_4
#define Key_EncoderA_GPIO_Port GPIOB
#define Key_EncoderA_EXTI_IRQn EXTI4_IRQn
#define Key_EncoderB_Pin GPIO_PIN_5
#define Key_EncoderB_GPIO_Port GPIOB
#define Key_EncoderB_EXTI_IRQn EXTI9_5_IRQn
#define Tp_Int_Pin GPIO_PIN_9
#define Tp_Int_GPIO_Port GPIOB
#define Tp_Rst_Pin GPIO_PIN_0
#define Tp_Rst_GPIO_Port GPIOE
#define Key_Switch_Pin GPIO_PIN_1
#define Key_Switch_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
