/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdlib.h"
#include "USR_NEXTION.h"
#include "USR_RTC.h"
#include "USR_FLASH.h"
#include "USR_TIM.h"
#include "USR_PROCESADOR.h"
#include "USR_TEMPERATURA.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
#if USR_TEST_TERMOCUPLAS == 1

uint8_t contador = 0;
uint8_t texto_temperatura[30];

#endif
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_I2C2_Init();
  MX_RTC_Init();
  MX_TIM6_Init();
  MX_TIM17_Init();
  /* USER CODE BEGIN 2 */

  //SE LEEN LAS MEMORIAS, SE VERIFICAN QUE ESTEN BIEN CONFIGURADAS Y SI NO, SE REINICIAN!!!
	USR_FLASH_Leer_Programa(1);
	if(USR_FLASH_Obtener_Dato_Trama(USR_PROCESADOR_POSICION_COMANDO) != USR_NEXTION_COMANDO_GUARDAR_PROGRAMA)
	{
	  USR_FLASH_Reiniciar_Programas();
	}

	USR_FLASH_Leer_Termocuplas();
	if(USR_FLASH_Obtener_Dato_Trama(USR_PROCESADOR_POSICION_COMANDO) != USR_NEXTION_COMANDO_GUARDAR_TERMOCUPLAS)
	{
	  USR_FLASH_Reiniciar_Termocuplas();
	}

	USR_FLASH_Leer_Estado_Actual();
	if(USR_FLASH_Obtener_Dato_Trama_Estado_Actual(USR_PROCESADOR_POSICION_COMANDO) != USR_NEXTION_COMANDO_GUARDAR_PROGRAMA)
	{
	  USR_FLASH_Reiniciar_Estado_Actual();
	}

	//Se inician los led apagados
	USR_TEMPERATURA_Cambio_Led(USR_TEMPERATURA_LED_1, USR_TEMPERATURA_ESTADO_LED_APAGADO);
	USR_TEMPERATURA_Cambio_Led(USR_TEMPERATURA_LED_2, USR_TEMPERATURA_ESTADO_LED_APAGADO);
	USR_TEMPERATURA_Cambio_Led(USR_TEMPERATURA_LED_3, USR_TEMPERATURA_ESTADO_LED_APAGADO);
	USR_TEMPERATURA_Cambio_Led(USR_TEMPERATURA_LED_4, USR_TEMPERATURA_ESTADO_LED_APAGADO);
	USR_TEMPERATURA_Cambio_Led(USR_TEMPERATURA_LED_5, USR_TEMPERATURA_ESTADO_LED_APAGADO);
	USR_TEMPERATURA_Cambio_Led(USR_TEMPERATURA_LED_6, USR_TEMPERATURA_ESTADO_LED_APAGADO);

	//Se apage el pin del buzzer
	USR_PROCESADOR_BUZZER_OFF();

	//Los pines de los reles inician activos...
	USR_PROCESADOR_RELE_START_ACTIVO();
	USR_PROCESADOR_RELE_MANUAL_ACTIVO();

	//Se inactiva la salida AC de la tarjeta...
	USR_PROCESADOR_ReleAC(USR_PROCESADOR_RELEAC_INACTIVO);

	//Se inician las termocuplas...
	#if USR_TEST_SIN_CONTROL == 0

	USR_TEMPERATURA_Iniciar_Chips();
	if(usr_temperatura_estado_alerta == USR_TEMPERATURA_ALERTA_ACTIVA)
	{
	  USR_NEXTION_Cambiar_Pantalla(USR_NEXTION_PANTALLA_ALERTA);
	}

	#endif

	//Se hace un retardo de 3 segundos para dejar que se encienda la pantalla
	HAL_Delay(3000);

	//Se revisa si hay un programa activo
	USR_FLASH_Leer_Estado_Actual();
	if((USR_FLASH_Obtener_Dato_Trama_Estado_Actual(USR_PROCESADOR_POSICION_ESTADO_ACTUAL) == USR_PROCESADOR_ESTADO_EJECUTANDO) || (USR_FLASH_Obtener_Dato_Trama_Estado_Actual(USR_PROCESADOR_POSICION_ESTADO_ACTUAL) == USR_PROCESADOR_ESTADO_ROTANDO))
	{
	  USR_PROCESADOR_Retomar_Programa_Actual();
	  //Se reinicia el periferico I2C porque se bloquea despues de retomar el programa
	  MX_I2C2_Init();

	}
	else
	{
	  USR_NEXTION_Enviar_Trama(usr_nextion_com_vison_b0, (uint16_t)strlen((const char *)usr_nextion_com_vison_b0));
	  //Se crea una trama de estado actual para evitar errores en los cambios de la misma
	  USR_FLASH_Iniciar_Trama_Estado_Actual();
	}
	HAL_Delay(50);

	USR_NEXTION_Iniciar_Recibir_Trama();
	//Se inicia el control de temperatura...
	USR_TEMPERATURA_Iniciar_Control();
	//Se inicia el conteo del minuto
	USR_TIM_Iniciar_Conteo_1minuto();

	USR_PROCESADOR_Buzzer();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

#if USR_TEST_TERMOCUPLAS == 0

	//Rutina principal de maquina de estados...
	USR_NEXTION_Administrador();
	if(usr_procesador_estado_actual == USR_PROCESADOR_ESTADO_EJECUTANDO)
	{
		USR_PROCESADOR_Ejecutar_Programa();

	}
#if USR_TEST_SIN_CONTROL == 0
	if(usr_tim_estado_control_temperatura == USR_TIM_BANDERA_CONTROL_TEMPERATURA_CAMBIO)
	{
		USR_NEXTION_Inhabilitar_Touch_Pantalla();
		USR_TEMPERATURA_Control();
		if(usr_temperatura_estado_alerta == USR_TEMPERATURA_ALERTA_ACTIVA)
	    {
			USR_NEXTION_Cambiar_Pantalla(USR_NEXTION_PANTALLA_ALERTA);
		}
		USR_NEXTION_Verificar_Temperatura();
		USR_NEXTION_Habilitar_Touch_Pantalla();
	}
#endif

#endif

#if USR_TEST_TERMOCUPLAS == 1

	for(contador = 0; contador < USR_TEMPERATURA_CANTIDAD_TERMOCUPLAS; contador++)
	{
	  USR_TEMPERATURA_Leer_Temperatura(&arreglo_termocuplas[contador]);

	  if(arreglo_termocuplas[contador].estado_termocupla == USR_TEMPERATURA_ESTADO_TERMOCUPLA_OK)
	  {
		  sprintf((char *)texto_temperatura, "Temp. Termocupla %d: %d\n", arreglo_termocuplas[contador].numero_termocupla+1,arreglo_termocuplas[contador].temperatura_actual);
		  HAL_UART_Transmit(&huart2, texto_temperatura, strlen((char *)texto_temperatura), 50);
	  }
	  else if (arreglo_termocuplas[contador].estado_termocupla == USR_TEMPERATURA_ESTADO_TERMOCUPLA_ABIERTA)
	  {
		  sprintf((char *)texto_temperatura, "Termocupla %d abierta...\n", arreglo_termocuplas[contador].numero_termocupla+1);
		  HAL_UART_Transmit(&huart2, texto_temperatura, strlen((char *)texto_temperatura), 50);
	  }
	  else
	  {
		  sprintf((char *)texto_temperatura, "Termocupla %d en error...\n", arreglo_termocuplas[contador].numero_termocupla+1);
		  HAL_UART_Transmit(&huart2, texto_temperatura, strlen((char *)texto_temperatura), 50);
	  }

	}
	HAL_Delay(1000);
#endif

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
	USR_TEMPERATURA_Cambio_Led(USR_TEMPERATURA_LED_6, USR_TEMPERATURA_ESTADO_LED_ROJO);
	HAL_Delay(250);
	USR_TEMPERATURA_Cambio_Led(USR_TEMPERATURA_LED_6, USR_TEMPERATURA_ESTADO_LED_APAGADO);
	HAL_Delay(250);
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
