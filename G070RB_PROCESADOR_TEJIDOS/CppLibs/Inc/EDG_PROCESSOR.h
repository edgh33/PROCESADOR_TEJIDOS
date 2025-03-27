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

#endif


/* ADC Definitions for Base Time -------------------------------------*/

#define EDG_PROCESSOR_SENSOR_CAROUSEL()	!HAL_GPIO_ReadPin(SENSOR_FIN_GPIO_Port, SENSOR_FIN_Pin)
#define EDG_PROCESSOR_SENSOR_POSITION()	!HAL_GPIO_ReadPin(SENSOR_AC_GPIO_Port, SENSOR_AC_Pin)

#define EDG_PROCESSOR_SHAKE_RELAY_ACTIVE()	 HAL_GPIO_WritePin(RELE_START_GPIO_Port, RELE_START_Pin, GPIO_PIN_SET)
#define EDG_PROCESSOR_SHAKE_RELAY_INACTIVE() HAL_GPIO_WritePin(RELE_START_GPIO_Port, RELE_START_Pin, GPIO_PIN_RESET)

#define EDG_PROCESSOR_RAISE_RELAY_ACTIVE()	HAL_GPIO_WritePin(RELE_MANUAL_GPIO_Port, RELE_MANUAL_Pin, GPIO_PIN_SET)
#define EDG_PROCESSOR_RAISE_RELAY_INACTIVE()	HAL_GPIO_WritePin(RELE_MANUAL_GPIO_Port, RELE_MANUAL_Pin, GPIO_PIN_RESET)


typedef union
{
	uint8_t AllFlags;
	struct{

		uint8_t Flag1 :1;
		uint8_t Flag2 :1;
		uint8_t Flag3 :1;
		uint8_t Flag4 :1;
		uint8_t Flag5 :1;

	};
}EDG_PROCESSOR_FlagsStatusTypeDef;

/**
  * @brief Structure definition
  */
typedef enum
{

	EDG_PROCESSOR_STATE_STOP,
	EDG_PROCESSOR_STATE_RUNNING,
	EDG_PROCESSOR_STATE_PAUSE,
	EDG_PROCESSOR_STATE_ALERT,
	EDG_PROCESSOR_STATE_ROTATING,

}EDG_PROCESSOR_States;

/**
  * @brief Structure definition
  */
typedef enum
{

	EDG_PROCESSOR_FLAG_FIRST_RUNNING,
	EDG_PROCESSOR_FLAG_NO_FIRST_RUNNING,

}EDG_PROCESSOR_FlagFirstRunning;

/**
  * @brief Structure definition
  */
typedef enum
{

	EDG_PROCESSOR_FLAG_MINUTE_RUNNING,
	EDG_PROCESSOR_FLAG_MINUTE_COMPLET,

}EDG_PROCESSOR_FlagMinute;

/**
  * @brief Structure definition
  */
typedef struct __EDG_PROCESSOR_HandleTypeDef
{

	EDG_PROCESSOR_FlagsStatusTypeDef FlagsStatus;
	EDG_PROCESSOR_FlagFirstRunning FlagFirstRunning;
	EDG_PROCESSOR_FlagMinute FlagMinute;
	EDG_PROCESSOR_States CurrentState;

	uint32_t CurrentContainer;
	uint32_t ContainerQuantity;

	uint32_t CurrentHour;
	uint32_t CurrentMinute;
	uint32_t TotalTimeCurrenteContainer;

	uint32_t CurrentHourDelay;
	uint32_t CurrentMinuteDelay;
	uint32_t TotalTimeDelayMinutes;

}EDG_PROCESSOR_HandleTypeDef;


extern EDG_PROCESSOR_HandleTypeDef hedgProcessor;

/* Timer Functions -------------------------------------*/
void EDG_PROCESSOR_Init(EDG_PROCESSOR_HandleTypeDef * phedgProcessor);


#ifdef __cplusplus
}
#endif


#endif /* EDG_PROCESSOR_H_ */
