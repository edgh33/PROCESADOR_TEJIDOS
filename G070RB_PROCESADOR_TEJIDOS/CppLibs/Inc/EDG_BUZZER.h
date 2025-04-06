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

#define EDG_BUZZER_PIN_HIGH()		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET)
#define EDG_BUZZER_PIN_LOW()		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET)

typedef union
{
	uint16_t AllFlags;
	struct{

		uint16_t FlagActive			:1;
		uint16_t FlagIsHigh			:1;
		uint16_t FlagHasDelay	 	:1;
		uint16_t FlagTrigger		:1;
		uint16_t FlagisFinite		:1;
		uint16_t FlagSoundOn		:1;
		uint16_t FlagError 			:1;
	};
}EDG_BUZZER_FlagsStatusTypeDef;

/**
  * @brief Structure definition
  */
typedef struct __EDG_BUZZER_HandleTypeDef{

	EDG_BUZZER_FlagsStatusTypeDef FlagsStatus;
	uint16_t period10msLow;
	uint16_t period10msHigh;
	uint16_t cicles;
	uint16_t repeats;
	uint16_t delay;

}EDG_BUZZER_HandleTypeDef;


extern EDG_BUZZER_HandleTypeDef hedgBuzzer;

/* Timer Functions -------------------------------------*/
void EDG_BUZZER_Init(EDG_BUZZER_HandleTypeDef * hedgBuzzer);
void EDG_BUZZER_Sound(uint16_t periodHigh10ms,
					  uint16_t periodLow10ms,
					  uint16_t delay10ms,
					  uint16_t cicles,
					  uint16_t repeats);
void EDG_BUZZER_Constant(void);
void EDG_BUZZER_Stop(void);
void EDG_BUZZER_IrqFunction(void);


#ifdef __cplusplus
}
#endif


#endif /* EDG_TIMER_H_ */
