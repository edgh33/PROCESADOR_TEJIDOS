/*
 * EDG_TIMER.h
 *
 *  Created on: 26/01/2023
 *      Author: Elkin Granados
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef EDG_TIMER_H_
#define EDG_TIMER_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "EDG_CONFIG.h"

#if EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE

#include "main.h"
#include "tim.h"

#endif

/* EDG_RTC.h must be included mandatory -------------------------------------*/
#include "EDG_RTC.h"
/* Include all the files base time related -------------------------------------*/
#include "EDG_AC_CONTROL.h"

/* ADC Definitions for Base Time -------------------------------------*/

#define EDG_TIMER_TIM_BASE				TIM6
#define EDG_TIMER_TIM_BASE_HANDLER		htim6

#define EDG_TIMER_MAX_SECONDS			(60)
#define EDG_TIMER_MAX_MINUTES			(60)
#define EDG_TIMER_MAX_HOURS				(24)
#define EDG_TIMER_BASE_TIME_SECS_1		EDG_AC_CONTROL_SAMPLE_TIME_SECS


typedef union
{
	uint8_t AllFlags;
	struct{

		uint8_t Flag1s :1;
		uint8_t Flag1m :1;
		uint8_t Flag1h :1;
		uint8_t Flag1d :1;
		uint8_t FlagIwdg			:1;
		uint8_t FlagBaseTimeSecs1 	:1;

	};
}EDG_TIMER_FlagsStatusTypeDef;

/**
  * @brief Structure definition
  */
typedef struct __EDG_TIMER_HandleTypeDef
{

	EDG_TIMER_FlagsStatusTypeDef FlagsStatus;
	uint8_t	CounterSegs;
	uint8_t	CounterMins;
	uint8_t	CounterHours;
	uint16_t CounterBaseTimeSecs1;

}EDG_TIMER_HandleTypeDef;

extern EDG_TIMER_HandleTypeDef hedgTimer;

/* Timer Functions -------------------------------------*/
void EDG_TIMER_Init(EDG_TIMER_HandleTypeDef * ptrhedgTimer);

void EDG_TIMER_StarBaseTime(EDG_TIMER_HandleTypeDef * ptrhedgTimer,
						    EDG_RTC_HandleTypeDef * ptrhedgRTC);
void EDG_TIMER_StopBaseTime(EDG_TIMER_HandleTypeDef * ptrhedgTimer);

void EDG_TIMER_SyncRTC(EDG_TIMER_HandleTypeDef * ptrhedgTimer,
					   EDG_RTC_HandleTypeDef * ptrhedgRTC);

#ifdef __cplusplus
}
#endif


#endif /* EDG_TIMER_H_ */
