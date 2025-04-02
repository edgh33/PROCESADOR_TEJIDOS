/*
 * EDG_TIMER.cpp
 *
 *  Created on: 26/01/2023
 *      Author: Elkin Granados
 */

/*** Include header file ***/
#include "EDG_PROCESSOR.h"

EDG_PROCESSOR_HandleTypeDef hedgProcessor;

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_PROCESSOR_Init(EDG_PROCESSOR_HandleTypeDef * phedgProcessor)
{

	phedgProcessor->FlagsStatus.AllFlags = 0x00;
	phedgProcessor->CurrentState = EDG_PROCESSOR_STATE_STOP;
	phedgProcessor->CurrentCarouselState = EDG_PROCESSOR_CAROUSEL_STATE_CHECKING;
	memset(phedgProcessor->CurrentProcess, 0, EDG_PROCESSOR_CURR_STATE_ARRAY_SIZE);

	phedgProcessor->CurrentHour = 0;
	phedgProcessor->CurrentMinute = 0;
	phedgProcessor->TotalTimeCurrenteContainer = 0;

	phedgProcessor->CurrentHourDelay = 0;
	phedgProcessor->CurrentMinuteDelay = 0;
	phedgProcessor->TotalTimeDelayMinutes = 0;

	phedgProcessor->CounterDripSeconds= 0;
	phedgProcessor->CounterCheckCarousel = 0;
	phedgProcessor->Index = 0;

	return;
}


/**
  * @brief
  * @param
  * @retval
  */
void EDG_PROCESSOR_StartTim(EDG_PROCESSOR_HandleTypeDef * hedgProcessor, uint16_t seconds)
{

	HAL_TIM_Base_Stop_IT(&EDG_PROCESSOR_TIM_HANDLE);
	__HAL_TIM_SET_AUTORELOAD(&EDG_PROCESSOR_TIM_HANDLE, (uint16_t)((1000 * seconds) - 1));
	__HAL_TIM_SET_COUNTER(&EDG_PROCESSOR_TIM_HANDLE, 0);
	HAL_TIM_Base_Start_IT(&EDG_PROCESSOR_TIM_HANDLE);
	hedgProcessor->FlagsStatus.FlagTimComplete = 0;
	hedgProcessor->FlagsStatus.FlagTimActive = 1;

	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_PROCESSOR_StartTimMs(EDG_PROCESSOR_HandleTypeDef * hedgProcessor, uint16_t ms)
{

	HAL_TIM_Base_Stop_IT(&EDG_PROCESSOR_TIM_HANDLE);
	__HAL_TIM_SET_AUTORELOAD(&EDG_PROCESSOR_TIM_HANDLE, (uint16_t)(ms - 1));
	__HAL_TIM_SET_COUNTER(&EDG_PROCESSOR_TIM_HANDLE, 0);
	HAL_TIM_Base_Start_IT(&EDG_PROCESSOR_TIM_HANDLE);
	hedgProcessor->FlagsStatus.FlagTimComplete = 0;
	hedgProcessor->FlagsStatus.FlagTimActive = 1;

	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_PROCESSOR_StopTim(EDG_PROCESSOR_HandleTypeDef * hedgProcessor)
{
	HAL_TIM_Base_Stop_IT(&EDG_PROCESSOR_TIM_HANDLE);
	__HAL_TIM_SET_COUNTER(&EDG_PROCESSOR_TIM_HANDLE, 0);
	hedgProcessor->FlagsStatus.FlagTimActive = 0;
	return;
}
