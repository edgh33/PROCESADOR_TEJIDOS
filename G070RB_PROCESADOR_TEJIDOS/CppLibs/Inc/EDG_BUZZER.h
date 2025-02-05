/*
 * EDG_BUZZER.h
 *
 *  Created on: 26/01/2023
 *      Author: Elkin Granados
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef EDG_BUZZER_H_
#define EDG_BUZZER_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "EDG_CONFIG.h"

#if EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE

#include "main.h"
#include "tim.h"

#endif

/* ADC Definitions for Base Time -------------------------------------*/

#define EDG_BUZZER_TIM_BASE				TIM16
#define EDG_BUZZER_TIM_BASE_HANDLER		htim16
#define EDG_BUZZER_TIM_CHANNEL			TIM_CHANNEL_1

#define EDG_BUZZER_PERIOD_ONE_PULSE			199
#define EDG_BUZZER_PULSE_ONE_PULSE			169

#define EDG_BUZZER_PERIOD_REPETITIVE		1999
#define EDG_BUZZER_PULSE_REPETITIVE			999

/**
  * @brief RTC Date Structure definition
  */
typedef enum
{
	EDG_BUZZER_STATUS_INACTIVE,
	EDG_BUZZER_STATUS_ACTIVE,

}EDG_BUZZER_StatusTypeDef;

/**
  * @brief RTC Date Structure definition
  */
typedef enum
{

	EDG_BUZZER_MODE_ONE_PULSE,
	EDG_BUZZER_MODE_REPETITIVE,

}EDG_BUZZER_ModeTypeDef;


/**
  * @brief Structure definition
  */
typedef struct __EDG_BUZZER_HandleTypeDef
{

	EDG_BUZZER_StatusTypeDef Status;
	EDG_BUZZER_ModeTypeDef Mode;

}EDG_BUZZER_HandleTypeDef;

extern EDG_BUZZER_HandleTypeDef hedgBuzzer;

/* Timer Functions -------------------------------------*/
void EDG_BUZZER_Init(EDG_BUZZER_HandleTypeDef * ptrhedgTimer);
void EDG_BUZZER_Pulse(EDG_BUZZER_HandleTypeDef * ptrhedgBuzzer);
void EDG_BUZZER_SetMode(EDG_BUZZER_HandleTypeDef * ptrhedgBuzzer, EDG_BUZZER_ModeTypeDef Mode);


#ifdef __cplusplus
}
#endif


#endif /* EDG_TIMER_H_ */
