/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32g4xx_hal.h"

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
#define SW_I2C_SCL_Pin GPIO_PIN_13
#define SW_I2C_SCL_GPIO_Port GPIOC
#define SW_I2C_SDA_Pin GPIO_PIN_14
#define SW_I2C_SDA_GPIO_Port GPIOC
#define KEY1_Pin GPIO_PIN_15
#define KEY1_GPIO_Port GPIOC
#define KEY2_Pin GPIO_PIN_4
#define KEY2_GPIO_Port GPIOA
#define NRF_CS_Pin GPIO_PIN_0
#define NRF_CS_GPIO_Port GPIOB
#define NRF_INT_Pin GPIO_PIN_1
#define NRF_INT_GPIO_Port GPIOB
#define NRF_INT_EXTI_IRQn EXTI1_IRQn
#define NRF_CE_Pin GPIO_PIN_10
#define NRF_CE_GPIO_Port GPIOB
#define IMU_INT_Pin GPIO_PIN_11
#define IMU_INT_GPIO_Port GPIOB
#define IMU_INT_EXTI_IRQn EXTI15_10_IRQn
#define IMU_CS_Pin GPIO_PIN_12
#define IMU_CS_GPIO_Port GPIOB
#define KEY2A12_Pin GPIO_PIN_12
#define KEY2A12_GPIO_Port GPIOA
#define FAULT_Pin GPIO_PIN_4
#define FAULT_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
