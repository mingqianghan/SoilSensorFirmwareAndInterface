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
#include "Sensor_Config.h"
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
#define RADIO_RESET_Pin GPIO_PIN_2
#define RADIO_RESET_GPIO_Port GPIOE
#define RADIO_DEVICE_SEL_Pin GPIO_PIN_3
#define RADIO_DEVICE_SEL_GPIO_Port GPIOE
#define RADIO_BUSY_Pin GPIO_PIN_4
#define RADIO_BUSY_GPIO_Port GPIOE
#define RADIO_ANT_SW_Pin GPIO_PIN_5
#define RADIO_ANT_SW_GPIO_Port GPIOE
#define MAG_ADC_Pin GPIO_PIN_0
#define MAG_ADC_GPIO_Port GPIOA
#define PHS_ADC_Pin GPIO_PIN_1
#define PHS_ADC_GPIO_Port GPIOA
#define AD9914_SPI_CS_Pin GPIO_PIN_3
#define AD9914_SPI_CS_GPIO_Port GPIOA
#define AD9914_SPI_CLK_Pin GPIO_PIN_5
#define AD9914_SPI_CLK_GPIO_Port GPIOA
#define AD9914_SPI_MOSI_Pin GPIO_PIN_7
#define AD9914_SPI_MOSI_GPIO_Port GPIOA
#define RADIO_NSS_Pin GPIO_PIN_5
#define RADIO_NSS_GPIO_Port GPIOC
#define RADIO_SPI_MOSI_Pin GPIO_PIN_0
#define RADIO_SPI_MOSI_GPIO_Port GPIOB
#define RADIO_SPI_MISO_Pin GPIO_PIN_1
#define RADIO_SPI_MISO_GPIO_Port GPIOB
#define RADIO_SPI_SCLK_Pin GPIO_PIN_2
#define RADIO_SPI_SCLK_GPIO_Port GPIOB
#define RADIO_DIO_1_Pin GPIO_PIN_7
#define RADIO_DIO_1_GPIO_Port GPIOE
#define RADIO_DIO_1_EXTI_IRQn EXTI9_5_IRQn
#define POWER_EN1_Pin GPIO_PIN_9
#define POWER_EN1_GPIO_Port GPIOE
#define POWER_EN2_Pin GPIO_PIN_10
#define POWER_EN2_GPIO_Port GPIOE
#define POWER_EN3_Pin GPIO_PIN_11
#define POWER_EN3_GPIO_Port GPIOE
#define LED_RED_Pin GPIO_PIN_13
#define LED_RED_GPIO_Port GPIOE
#define LED_GREEN_Pin GPIO_PIN_14
#define LED_GREEN_GPIO_Port GPIOE
#define RF_SWB_Pin GPIO_PIN_11
#define RF_SWB_GPIO_Port GPIOB
#define AD9833_SPI_MOSI_Pin GPIO_PIN_13
#define AD9833_SPI_MOSI_GPIO_Port GPIOB
#define AD9833_SPI_CLK_Pin GPIO_PIN_14
#define AD9833_SPI_CLK_GPIO_Port GPIOB
#define AD9833_SPI_CS_Pin GPIO_PIN_15
#define AD9833_SPI_CS_GPIO_Port GPIOB
#define RF_SWA_Pin GPIO_PIN_9
#define RF_SWA_GPIO_Port GPIOD
#define AD9914_RESET_Pin GPIO_PIN_13
#define AD9914_RESET_GPIO_Port GPIOD
#define AD9914_IOUPDATE_Pin GPIO_PIN_14
#define AD9914_IOUPDATE_GPIO_Port GPIOD
#define uSD_Det_Pin GPIO_PIN_7
#define uSD_Det_GPIO_Port GPIOC
#define uSD_D0_Pin GPIO_PIN_8
#define uSD_D0_GPIO_Port GPIOC
#define uSD_D1_Pin GPIO_PIN_9
#define uSD_D1_GPIO_Port GPIOC
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define uSD_D2_Pin GPIO_PIN_10
#define uSD_D2_GPIO_Port GPIOC
#define uSD_D3_Pin GPIO_PIN_11
#define uSD_D3_GPIO_Port GPIOC
#define uSD_CLK_Pin GPIO_PIN_12
#define uSD_CLK_GPIO_Port GPIOC
#define uSD_CMD_Pin GPIO_PIN_2
#define uSD_CMD_GPIO_Port GPIOD
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
