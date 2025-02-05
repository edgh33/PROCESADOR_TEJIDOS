/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32f0xx_hal.h"

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
#define RELE_ON_Pin GPIO_PIN_0
#define RELE_ON_GPIO_Port GPIOF
#define RELE_OFF_Pin GPIO_PIN_1
#define RELE_OFF_GPIO_Port GPIOF
#define LED_VERDE_1_Pin GPIO_PIN_0
#define LED_VERDE_1_GPIO_Port GPIOC
#define LED_ROJO_1_Pin GPIO_PIN_1
#define LED_ROJO_1_GPIO_Port GPIOC
#define LED_VERDE_2_Pin GPIO_PIN_2
#define LED_VERDE_2_GPIO_Port GPIOC
#define LED_ROJO_2_Pin GPIO_PIN_3
#define LED_ROJO_2_GPIO_Port GPIOC
#define LED_VERDE_3_Pin GPIO_PIN_0
#define LED_VERDE_3_GPIO_Port GPIOA
#define LED_ROJO_3_Pin GPIO_PIN_1
#define LED_ROJO_3_GPIO_Port GPIOA
#define LCD_RST_UC_Pin GPIO_PIN_4
#define LCD_RST_UC_GPIO_Port GPIOF
#define BUZZER_Pin GPIO_PIN_4
#define BUZZER_GPIO_Port GPIOA
#define SPI_CS1_Pin GPIO_PIN_4
#define SPI_CS1_GPIO_Port GPIOC
#define SPI_CS2_Pin GPIO_PIN_5
#define SPI_CS2_GPIO_Port GPIOC
#define SPI_CS3_Pin GPIO_PIN_0
#define SPI_CS3_GPIO_Port GPIOB
#define SPI_CS4_Pin GPIO_PIN_1
#define SPI_CS4_GPIO_Port GPIOB
#define SPI_CS5_Pin GPIO_PIN_2
#define SPI_CS5_GPIO_Port GPIOB
#define SPI_CS6_Pin GPIO_PIN_10
#define SPI_CS6_GPIO_Port GPIOB
#define TEST_IN_Pin GPIO_PIN_11
#define TEST_IN_GPIO_Port GPIOB
#define RELE_AUX_Pin GPIO_PIN_12
#define RELE_AUX_GPIO_Port GPIOB
#define RELE_STOP_Pin GPIO_PIN_13
#define RELE_STOP_GPIO_Port GPIOB
#define CONTROL_4_Pin GPIO_PIN_6
#define CONTROL_4_GPIO_Port GPIOC
#define CRUCE_CERO_2_Pin GPIO_PIN_7
#define CRUCE_CERO_2_GPIO_Port GPIOC
#define CONTROL_5_Pin GPIO_PIN_8
#define CONTROL_5_GPIO_Port GPIOC
#define CONTROL_6_Pin GPIO_PIN_9
#define CONTROL_6_GPIO_Port GPIOC
#define CONTROL_1_Pin GPIO_PIN_8
#define CONTROL_1_GPIO_Port GPIOA
#define CRUCE_CERO_1_Pin GPIO_PIN_9
#define CRUCE_CERO_1_GPIO_Port GPIOA
#define CONTROL_2_Pin GPIO_PIN_10
#define CONTROL_2_GPIO_Port GPIOA
#define CONTROL_3_Pin GPIO_PIN_11
#define CONTROL_3_GPIO_Port GPIOA
#define LED_VERDE_4_Pin GPIO_PIN_15
#define LED_VERDE_4_GPIO_Port GPIOA
#define LED_ROJO_4_Pin GPIO_PIN_10
#define LED_ROJO_4_GPIO_Port GPIOC
#define LED_VERDE_5_Pin GPIO_PIN_11
#define LED_VERDE_5_GPIO_Port GPIOC
#define LED_ROJO_5_Pin GPIO_PIN_12
#define LED_ROJO_5_GPIO_Port GPIOC
#define LED_VERDE_6_Pin GPIO_PIN_4
#define LED_VERDE_6_GPIO_Port GPIOB
#define LED_ROJO_6_Pin GPIO_PIN_5
#define LED_ROJO_6_GPIO_Port GPIOB
#define SENSOR_AC_Pin GPIO_PIN_6
#define SENSOR_AC_GPIO_Port GPIOB
#define SENSOR_FIN_Pin GPIO_PIN_7
#define SENSOR_FIN_GPIO_Port GPIOB
#define RELE_MANUAL_Pin GPIO_PIN_8
#define RELE_MANUAL_GPIO_Port GPIOB
#define RELE_START_Pin GPIO_PIN_9
#define RELE_START_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

#define USR_TEST_TERMOCUPLAS   	0
#define USR_TEST_SIN_CONTROL  	0

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
