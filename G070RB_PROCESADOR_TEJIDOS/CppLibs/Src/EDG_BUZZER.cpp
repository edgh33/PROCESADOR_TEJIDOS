/*
 * EDG_BUZZER.cpp
 *
 *  Created on: 26/01/2023
 *      Author: Elkin Granados
 */

/*** Include header file ***/
#include "EDG_BUZZER.h"

EDG_BUZZER_HandleTypeDef hedgBuzzer;

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_BUZZER_Init(EDG_BUZZER_HandleTypeDef * ptrhedgBuzzer)
{

	ptrhedgBuzzer->Status = EDG_BUZZER_STATUS_ACTIVE;
	ptrhedgBuzzer->Mode = EDG_BUZZER_MODE_ONE_PULSE;

	return;

}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_BUZZER_Pulse(EDG_BUZZER_HandleTypeDef * ptrhedgBuzzer)
{

	if(ptrhedgBuzzer->Status == EDG_BUZZER_STATUS_ACTIVE)
	{
		if(ptrhedgBuzzer->Mode == EDG_BUZZER_MODE_ONE_PULSE)
		{
			__HAL_TIM_SET_COUNTER(&EDG_BUZZER_TIM_BASE_HANDLER, 0);
			__HAL_TIM_ENABLE(&EDG_BUZZER_TIM_BASE_HANDLER);
			HAL_TIM_OnePulse_Start(&EDG_BUZZER_TIM_BASE_HANDLER, EDG_BUZZER_TIM_CHANNEL);
		}
		else if(ptrhedgBuzzer->Mode == EDG_BUZZER_MODE_REPETITIVE)
		{
			__HAL_TIM_SET_COUNTER(&EDG_BUZZER_TIM_BASE_HANDLER, 0);
			__HAL_TIM_ENABLE(&EDG_BUZZER_TIM_BASE_HANDLER);
			HAL_TIM_PWM_Start(&EDG_BUZZER_TIM_BASE_HANDLER, EDG_BUZZER_TIM_CHANNEL);
		}

	}

	return;

}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_BUZZER_SetMode(EDG_BUZZER_HandleTypeDef * ptrhedgBuzzer, EDG_BUZZER_ModeTypeDef Mode)
{

	__HAL_TIM_DISABLE(&EDG_BUZZER_TIM_BASE_HANDLER);
	__HAL_TIM_SET_COUNTER(&EDG_BUZZER_TIM_BASE_HANDLER, 0);


	if(Mode == EDG_BUZZER_MODE_ONE_PULSE)
	{
		__HAL_TIM_SET_AUTORELOAD(&EDG_BUZZER_TIM_BASE_HANDLER, EDG_BUZZER_PERIOD_ONE_PULSE);
		__HAL_TIM_SET_COMPARE(&EDG_BUZZER_TIM_BASE_HANDLER, EDG_BUZZER_TIM_CHANNEL, EDG_BUZZER_PULSE_ONE_PULSE);
		HAL_TIM_OnePulse_Init(&EDG_BUZZER_TIM_BASE_HANDLER, TIM_OPMODE_SINGLE);
	}
	else if(Mode == EDG_BUZZER_MODE_REPETITIVE)
	{
		__HAL_TIM_SET_AUTORELOAD(&EDG_BUZZER_TIM_BASE_HANDLER, EDG_BUZZER_PERIOD_REPETITIVE);
		__HAL_TIM_SET_COMPARE(&EDG_BUZZER_TIM_BASE_HANDLER, EDG_BUZZER_TIM_CHANNEL, EDG_BUZZER_PULSE_REPETITIVE);
		HAL_TIM_OnePulse_Init(&EDG_BUZZER_TIM_BASE_HANDLER, TIM_OPMODE_REPETITIVE);
	}

	return;
}

