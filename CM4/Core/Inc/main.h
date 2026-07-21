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

#include "stm32h7xx_nucleo.h"
#include <stdio.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "sd_functions.h"
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
#define TLMTR_SPI4_SCK_Pin GPIO_PIN_2
#define TLMTR_SPI4_SCK_GPIO_Port GPIOE
#define TLMTR_SPI4_MISO_Pin GPIO_PIN_5
#define TLMTR_SPI4_MISO_GPIO_Port GPIOE
#define TLMTR_SPI4_MOSI_Pin GPIO_PIN_6
#define TLMTR_SPI4_MOSI_GPIO_Port GPIOE
#define LIDAR_UART_TX_Pin GPIO_PIN_6
#define LIDAR_UART_TX_GPIO_Port GPIOC
#define LIDAR_UART_RX_Pin GPIO_PIN_7
#define LIDAR_UART_RX_GPIO_Port GPIOC
#define SD_CARD_DETECT_Pin GPIO_PIN_9
#define SD_CARD_DETECT_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
