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
#include "stm32h7xx_hal.h"

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
#define NLED_RED_Pin GPIO_PIN_3
#define NLED_RED_GPIO_Port GPIOE
#define NLED_GREEN_Pin GPIO_PIN_4
#define NLED_GREEN_GPIO_Port GPIOE
#define NLED_BLUE_Pin GPIO_PIN_5
#define NLED_BLUE_GPIO_Port GPIOE
#define SPI3_DRDY2_Pin GPIO_PIN_7
#define SPI3_DRDY2_GPIO_Port GPIOI
#define SPI3_DRDY1_Pin GPIO_PIN_6
#define SPI3_DRDY1_GPIO_Port GPIOI
#define SPI3_NCS2_Pin GPIO_PIN_8
#define SPI3_NCS2_GPIO_Port GPIOI
#define SPI3_NCS1_Pin GPIO_PIN_4
#define SPI3_NCS1_GPIO_Port GPIOI
#define ADC3_6V6_ADC_CURRENT_Pin GPIO_PIN_2
#define ADC3_6V6_ADC_CURRENT_GPIO_Port GPIOC
#define ADC3_3V3_ADC_VIN_Pin GPIO_PIN_3
#define ADC3_3V3_ADC_VIN_GPIO_Port GPIOC
#define SCALED_V5_ADC_5V_Pin GPIO_PIN_1
#define SCALED_V5_ADC_5V_GPIO_Port GPIOB
#define VDD_3V3_SENSORS3_EN_Pin GPIO_PIN_7
#define VDD_3V3_SENSORS3_EN_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
