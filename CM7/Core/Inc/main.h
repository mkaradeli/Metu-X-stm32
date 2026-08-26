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
//#define HX711_DOUT_GPIO_Port GPIOG
//#define HX711_DOUT_Pin GPIO_PIN_9
//
//#define HX711_SCK_GPIO_Port GPIOG
//#define HX711_SCK_Pin GPIO_PIN_12
//
//#define LEFT_EN_1_GPIO_Port GPIOE
//#define LEFT_EN_1_Pin GPIO_PIN_10
//
//#define RIGHT_EN_1_GPIO_Port GPIOE
//#define RIGHT_EN_1_Pin GPIO_PIN_12



/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TLMTR_SPI4_SCK_Pin GPIO_PIN_2
#define TLMTR_SPI4_SCK_GPIO_Port GPIOE
#define TLMTR_CE_Pin GPIO_PIN_3
#define TLMTR_CE_GPIO_Port GPIOE
#define TLMTR_CSN_Pin GPIO_PIN_4
#define TLMTR_CSN_GPIO_Port GPIOE
#define TLMTR_SPI4_MISO_Pin GPIO_PIN_5
#define TLMTR_SPI4_MISO_GPIO_Port GPIOE
#define TLMTR_SPI4_MOSI_Pin GPIO_PIN_6
#define TLMTR_SPI4_MOSI_GPIO_Port GPIOE
#define ADC3_VBAT_Pin GPIO_PIN_6
#define ADC3_VBAT_GPIO_Port GPIOF
#define ADC3_PT4_Pin GPIO_PIN_7
#define ADC3_PT4_GPIO_Port GPIOF
#define ADC3_PT3_Pin GPIO_PIN_8
#define ADC3_PT3_GPIO_Port GPIOF
#define ADC3_PT0_Pin GPIO_PIN_9
#define ADC3_PT0_GPIO_Port GPIOF
#define ADC3_PT1_Pin GPIO_PIN_10
#define ADC3_PT1_GPIO_Port GPIOF
#define ADC3_PT2_Pin GPIO_PIN_0
#define ADC3_PT2_GPIO_Port GPIOC
#define LEFT_EN_2_Pin GPIO_PIN_2
#define LEFT_EN_2_GPIO_Port GPIOC
#define ADC1_HE2_Pin GPIO_PIN_0
#define ADC1_HE2_GPIO_Port GPIOA
#define ADC2_CS1_Pin GPIO_PIN_3
#define ADC2_CS1_GPIO_Port GPIOA
#define ADC1_HE3_Pin GPIO_PIN_4
#define ADC1_HE3_GPIO_Port GPIOA
#define ADC2_CS2_Pin GPIO_PIN_6
#define ADC2_CS2_GPIO_Port GPIOA
#define ADC2_CS3_Pin GPIO_PIN_1
#define ADC2_CS3_GPIO_Port GPIOB
#define IMU_P0_Pin GPIO_PIN_2
#define IMU_P0_GPIO_Port GPIOB
#define ADC1_HE0_Pin GPIO_PIN_11
#define ADC1_HE0_GPIO_Port GPIOF
#define ADC2_CS0_Pin GPIO_PIN_14
#define ADC2_CS0_GPIO_Port GPIOF
#define BARO_I2C4_SDA_Pin GPIO_PIN_15
#define BARO_I2C4_SDA_GPIO_Port GPIOF
#define RIGHT_EN_1_Pin GPIO_PIN_7
#define RIGHT_EN_1_GPIO_Port GPIOE
#define LEFT_EN_1_Pin GPIO_PIN_12
#define LEFT_EN_1_GPIO_Port GPIOE
#define RIGHT_EN_0_Pin GPIO_PIN_15
#define RIGHT_EN_0_GPIO_Port GPIOE
#define IMU_CS_Pin GPIO_PIN_10
#define IMU_CS_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_11
#define LED2_GPIO_Port GPIOB
#define SAFETY_CONNECTOR_Pin GPIO_PIN_12
#define SAFETY_CONNECTOR_GPIO_Port GPIOB
#define IMU_RST_Pin GPIO_PIN_15
#define IMU_RST_GPIO_Port GPIOB
#define USB_OTG_FS_PWR_EN_Pin GPIO_PIN_10
#define USB_OTG_FS_PWR_EN_GPIO_Port GPIOD
#define BARO_I2C4_SCL_Pin GPIO_PIN_12
#define BARO_I2C4_SCL_GPIO_Port GPIOD
#define LED1_Pin GPIO_PIN_15
#define LED1_GPIO_Port GPIOD
#define ISO_VALVE_Pin GPIO_PIN_8
#define ISO_VALVE_GPIO_Port GPIOG
#define LIDAR_UART_TX_Pin GPIO_PIN_6
#define LIDAR_UART_TX_GPIO_Port GPIOC
#define LIDAR_UART_RX_Pin GPIO_PIN_7
#define LIDAR_UART_RX_GPIO_Port GPIOC
#define IMU_INT_Pin GPIO_PIN_10
#define IMU_INT_GPIO_Port GPIOA
#define IMU_INT_EXTI_IRQn EXTI15_10_IRQn
#define LED3_Pin GPIO_PIN_15
#define LED3_GPIO_Port GPIOA
#define LEFT_EN_3_Pin GPIO_PIN_0
#define LEFT_EN_3_GPIO_Port GPIOD
#define LED4_Pin GPIO_PIN_1
#define LED4_GPIO_Port GPIOD
#define RIGHT_EN_2_Pin GPIO_PIN_4
#define RIGHT_EN_2_GPIO_Port GPIOD
#define TLMTR_IRQ_Pin GPIO_PIN_7
#define TLMTR_IRQ_GPIO_Port GPIOD
#define RIGHT_EN_3_Pin GPIO_PIN_9
#define RIGHT_EN_3_GPIO_Port GPIOG
#define IMU_SPI1_SCK_Pin GPIO_PIN_3
#define IMU_SPI1_SCK_GPIO_Port GPIOB
#define IMU_SPI1_MISO_Pin GPIO_PIN_4
#define IMU_SPI1_MISO_GPIO_Port GPIOB
#define IMU_SPI1_MOSI_Pin GPIO_PIN_5
#define IMU_SPI1_MOSI_GPIO_Port GPIOB
#define SD_CARD_DETECT_Pin GPIO_PIN_9
#define SD_CARD_DETECT_GPIO_Port GPIOB
#define LEFT_EN_0_Pin GPIO_PIN_0
#define LEFT_EN_0_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
