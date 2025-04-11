/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32g0xx_hal.h"

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
#define TEST_IN_Pin GPIO_PIN_13
#define TEST_IN_GPIO_Port GPIOC
#define TEST_IN_EXTI_IRQn EXTI4_15_IRQn
#define SENSOR_AC_Pin GPIO_PIN_0
#define SENSOR_AC_GPIO_Port GPIOC
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOC
#define RELE_START_Pin GPIO_PIN_2
#define RELE_START_GPIO_Port GPIOC
#define RELE_MANUAL_Pin GPIO_PIN_3
#define RELE_MANUAL_GPIO_Port GPIOC
#define TX_EXT_Pin GPIO_PIN_2
#define TX_EXT_GPIO_Port GPIOA
#define RX_EXT_Pin GPIO_PIN_3
#define RX_EXT_GPIO_Port GPIOA
#define PIN_WS2812_Pin GPIO_PIN_4
#define PIN_WS2812_GPIO_Port GPIOA
#define CS0_Pin GPIO_PIN_7
#define CS0_GPIO_Port GPIOA
#define TX_UC_NEXT_Pin GPIO_PIN_4
#define TX_UC_NEXT_GPIO_Port GPIOC
#define RX_UC_NEXT_Pin GPIO_PIN_5
#define RX_UC_NEXT_GPIO_Port GPIOC
#define CONTROL_5_Pin GPIO_PIN_0
#define CONTROL_5_GPIO_Port GPIOB
#define CONTROL_6_Pin GPIO_PIN_1
#define CONTROL_6_GPIO_Port GPIOB
#define RELE_AC_Pin GPIO_PIN_12
#define RELE_AC_GPIO_Port GPIOB
#define TEMP1_Pin GPIO_PIN_13
#define TEMP1_GPIO_Port GPIOB
#define TEMP2_Pin GPIO_PIN_14
#define TEMP2_GPIO_Port GPIOB
#define CONTROL_1_Pin GPIO_PIN_8
#define CONTROL_1_GPIO_Port GPIOA
#define CRUCE_CERO_1_Pin GPIO_PIN_9
#define CRUCE_CERO_1_GPIO_Port GPIOA
#define CONTROL_4_Pin GPIO_PIN_6
#define CONTROL_4_GPIO_Port GPIOC
#define CRUCE_CERO_2_Pin GPIO_PIN_7
#define CRUCE_CERO_2_GPIO_Port GPIOC
#define TX_CH340_Pin GPIO_PIN_8
#define TX_CH340_GPIO_Port GPIOD
#define RX_CH340_Pin GPIO_PIN_9
#define RX_CH340_GPIO_Port GPIOD
#define CONTROL_2_Pin GPIO_PIN_10
#define CONTROL_2_GPIO_Port GPIOA
#define CONTROL_3_Pin GPIO_PIN_11
#define CONTROL_3_GPIO_Port GPIOA
#define SENSOR_FIN_Pin GPIO_PIN_12
#define SENSOR_FIN_GPIO_Port GPIOA
#define BUZZER_Pin GPIO_PIN_0
#define BUZZER_GPIO_Port GPIOD
#define CS1_Pin GPIO_PIN_1
#define CS1_GPIO_Port GPIOD
#define CS2_Pin GPIO_PIN_2
#define CS2_GPIO_Port GPIOD
#define CS3_Pin GPIO_PIN_3
#define CS3_GPIO_Port GPIOD
#define CS4_Pin GPIO_PIN_4
#define CS4_GPIO_Port GPIOD
#define CS5_Pin GPIO_PIN_5
#define CS5_GPIO_Port GPIOD
#define CS6_Pin GPIO_PIN_6
#define CS6_GPIO_Port GPIOD
#define SCL_MEM_Pin GPIO_PIN_6
#define SCL_MEM_GPIO_Port GPIOB
#define SDA_MEM_Pin GPIO_PIN_7
#define SDA_MEM_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_9
#define LED2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
