/*
 * EDG_PROCESSOR.h
 *
 *  Created on: 24/03/2025
 *      Author: Elkin Granados
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef EDG_PROCESSOR_H_
#define EDG_PROCESSOR_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "EDG_CONFIG.h"

#if EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE

#include "main.h"
#include "tim.h"

#include <string.h>

#endif


/* Definition for Processor -------------------------------------*/

#define EDG_PROCESSOR_TIM			TIM14
#define EDG_PROCESSOR_TIM_HANDLE	htim14

#define EDG_PROCESSOR_SENSOR_CAROUSEL()			!HAL_GPIO_ReadPin(SENSOR_FIN_GPIO_Port, SENSOR_FIN_Pin)
#define EDG_PROCESSOR_SENSOR_POSITION()			!HAL_GPIO_ReadPin(SENSOR_AC_GPIO_Port, SENSOR_AC_Pin)

#define EDG_PROCESSOR_SHAKE_RELAY_ACTIVE()		HAL_GPIO_WritePin(RELE_START_GPIO_Port, RELE_START_Pin, GPIO_PIN_SET)
#define EDG_PROCESSOR_SHAKE_RELAY_INACTIVE() 	HAL_GPIO_WritePin(RELE_START_GPIO_Port, RELE_START_Pin, GPIO_PIN_RESET)

#define EDG_PROCESSOR_RAISE_RELAY_ACTIVE()		HAL_GPIO_WritePin(RELE_MANUAL_GPIO_Port, RELE_MANUAL_Pin, GPIO_PIN_SET)
#define EDG_PROCESSOR_RAISE_RELAY_INACTIVE()	HAL_GPIO_WritePin(RELE_MANUAL_GPIO_Port, RELE_MANUAL_Pin, GPIO_PIN_RESET)

//Active containers (1), Hours and minutes by container (12*2), drip seconds (1), t1 state (1), t1 value (1), t2 state (1), t2 value (1),
//current container(1), carousel pos(1), delay hour(1), delay min(1), Program (1), active run? (1)
#define EDG_PROCESSOR_CURR_STATE_ARRAY_SIZE 		(36U) //Always check with EDG_MEM_ADDR EDG_MEM_ADDR_VALUES_X_CURRENT_PROC
//Program and Current pos
#define EDG_PROCESSOR_ARR_POS_CURR_ACTIVE_CONTAINERS	(0)
#define EDG_PROCESSOR_ARR_POS_CURR_HOUR_CONTAIN_1		(1)
#define EDG_PROCESSOR_ARR_POS_CURR_DRIP_SECONDS			(25)
#define EDG_PROCESSOR_ARR_POS_CURR_T1_STATE				(26)
#define EDG_PROCESSOR_ARR_POS_CURR_T1_VALUE				(27)
#define EDG_PROCESSOR_ARR_POS_CURR_T2_STATE				(28)
#define EDG_PROCESSOR_ARR_POS_CURR_T2_VALUE				(29)
//Current pos
#define EDG_PROCESSOR_ARR_POS_CURR_CONTAINER		(30)
#define EDG_PROCESSOR_ARR_POS_CURR_CAROU_POS		(31)
#define EDG_PROCESSOR_ARR_POS_CURR_DELAY_HOUR		(32)
#define EDG_PROCESSOR_ARR_POS_CURR_DELAY_MIN		(33)
#define EDG_PROCESSOR_ARR_POS_CURR_PROGRAM			(34)
#define EDG_PROCESSOR_ARR_POS_CURR_IS_ACTIVE		(35)

#define EDG_PROCESSOR_CHECK_TIME_MS				(200)
#define EDG_PROCESSOR_DELAY_TIME_MS				(500)
#define EDG_PROCESSOR_WAIT_BASE_MS				(1000)
#define EDG_PROCESSOR_SECONDS_X_MIN				(60)
#define EDG_PROCESSOR_MAX_SECONDS_ROTATING		(30)
#define EDG_PROCESSOR_CHECKS_X_SEC				(5)
#define EDG_PROCESSOR_MAX_CHECKS_ROTATING		(EDG_PROCESSOR_MAX_SECONDS_ROTATING * EDG_PROCESSOR_CHECKS_X_SEC)

typedef union
{
	uint16_t AllFlags;
	struct{

		uint16_t FlagTimActive		:1;
		uint16_t FlagTimComplete	:1;
		uint16_t FlagSetRunning 	:1;
		uint16_t FlagRunning		:1;
		uint16_t FlagError 			:1;
		uint16_t FlagCheckCarousel 	:1;
		uint16_t FlagSetManual   	:1;
		uint16_t FlagManual 		:1;
		uint16_t FlagResume 		:1;
		uint16_t FlagShaking		:1;
		uint16_t FlagCarouselPos	:1;

	};
}EDG_PROCESSOR_FlagsStatusTypeDef;

/**
  * @brief Structure definition
  */
typedef enum
{

	EDG_PROCESSOR_CAROUSEL_POS_UNKNOW,
	EDG_PROCESSOR_CAROUSEL_POS_DOWN,
	EDG_PROCESSOR_CAROUSEL_POS_RAISING_1,
	EDG_PROCESSOR_CAROUSEL_POS_RAISING_2,
	EDG_PROCESSOR_CAROUSEL_POS_UP,
	EDG_PROCESSOR_CAROUSEL_POS_WAITING,
	EDG_PROCESSOR_CAROUSEL_POS_DESCENDING

}EDG_PROCESSOR_CarouselPos;

/**
  * @brief Structure definition
  */
typedef enum
{

	EDG_PROCESSOR_CAROUSEL_STATE_CHECKING,
	EDG_PROCESSOR_CAROUSEL_STATE_RAISING,
	EDG_PROCESSOR_CAROUSEL_STATE_WAITING,
	EDG_PROCESSOR_CAROUSEL_STATE_DESCENDING,

}EDG_PROCESSOR_CarouselState;

/**
  * @brief Structure definition
  */
typedef enum
{

	EDG_PROCESSOR_STATE_STOP,
	EDG_PROCESSOR_STATE_DELAY,
	EDG_PROCESSOR_STATE_CHECK_CAROUSEL,
	EDG_PROCESSOR_STATE_RUNNING,
	EDG_PROCESSOR_STATE_ROTATING,
	EDG_PROCESSOR_STATE_PAUSE,
	EDG_PROCESSOR_STATE_FINISH,
	EDG_PROCESSOR_STATE_ALERT,

}EDG_PROCESSOR_States;

/**
  * @brief Structure definition
  */
typedef enum
{

	EDG_PROCESSOR_MANUAL_STATE_DOWN,
	EDG_PROCESSOR_MANUAL_STATE_RAISE,
	EDG_PROCESSOR_MANUAL_STATE_SHAKE,
	EDG_PROCESSOR_MANUAL_STATE_NO_SHAKE,
	EDG_PROCESSOR_MANUAL_STATE_FINISH,

}EDG_PROCESSOR_ManualState;


/**
  * @brief Structure definition
  */
typedef struct __EDG_PROCESSOR_HandleTypeDef
{

	EDG_PROCESSOR_FlagsStatusTypeDef FlagsStatus;
	EDG_PROCESSOR_States CurrentState;
	EDG_PROCESSOR_CarouselState CurrentCarouselState;
	EDG_PROCESSOR_ManualState ManualState;
	uint8_t CurrentProcess[EDG_PROCESSOR_CURR_STATE_ARRAY_SIZE];

	uint8_t CurrentHour;
	uint8_t CurrentMinute;
	uint16_t TotalTimeCurrenteContainer;

	uint8_t CurrentHourDelay;
	uint8_t CurrentMinuteDelay;
	uint16_t TotalTimeDelayMinutes;

	uint8_t CounterDripSeconds;
	uint16_t CounterCheckCarousel;
	uint16_t Index;

}EDG_PROCESSOR_HandleTypeDef;


extern EDG_PROCESSOR_HandleTypeDef hedgProcessor;

/* Timer Functions -------------------------------------*/
void EDG_PROCESSOR_Init(EDG_PROCESSOR_HandleTypeDef * phedgProcessor);

void EDG_PROCESSOR_StartTim(EDG_PROCESSOR_HandleTypeDef * hedgProcessor, uint16_t seconds);
void EDG_PROCESSOR_StartTimMs(EDG_PROCESSOR_HandleTypeDef * hedgProcessor, uint16_t ms);
void EDG_PROCESSOR_StopTim(EDG_PROCESSOR_HandleTypeDef * hedgProcessor);


#ifdef __cplusplus
}
#endif


#endif /* EDG_PROCESSOR_H_ */
