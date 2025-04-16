/*
 * EDG_TIMER.cpp
 *
 *  Created on: 26/01/2023
 *      Author: Elkin Granados
 */

/*** Include header file ***/
#include "EDG_TIMER.h"

EDG_TIMER_HandleTypeDef hedgTimer;

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_TIMER_Init(EDG_TIMER_HandleTypeDef * ptrhedgTimer)
{

	ptrhedgTimer->CounterSegs = 0;
	ptrhedgTimer->CounterMins = 0;
	ptrhedgTimer->CounterHours = 0;
	ptrhedgTimer->CounterBaseTimeSecs1 = 0;
	ptrhedgTimer->FlagsStatus.Flag1s = false;
	ptrhedgTimer->FlagsStatus.Flag1m = false;
	ptrhedgTimer->FlagsStatus.Flag1h = false;
	ptrhedgTimer->FlagsStatus.Flag1d = false;
	ptrhedgTimer->FlagsStatus.FlagBaseTimeSecs1 = false;

	return;

}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_TIMER_StarBaseTime(EDG_TIMER_HandleTypeDef * ptrhedgTimer,
						    EDG_RTC_HandleTypeDef * ptrhedgRTC)
{
	EDG_TIMER_SyncRTC(ptrhedgTimer, ptrhedgRTC);

	__HAL_TIM_SET_COUNTER(&EDG_TIMER_TIM_BASE_HANDLER, 0U);
	HAL_TIM_Base_Start_IT(&EDG_TIMER_TIM_BASE_HANDLER);

	return;

}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_TIMER_StopBaseTime(EDG_TIMER_HandleTypeDef * ptrhedgTimer)
{

	HAL_TIM_Base_Stop_IT(&EDG_TIMER_TIM_BASE_HANDLER);
	return;

}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_TIMER_SyncRTC(EDG_TIMER_HandleTypeDef * ptrhedgTimer,
					   EDG_RTC_HandleTypeDef * ptrhedgRTC)
{

	if(ptrhedgRTC->CurrentState == EDG_RTC_STATE_OK)
	{
		EDG_RTC_GetDate(ptrhedgRTC, EDG_RTC_ADDRESS);
		ptrhedgTimer->CounterSegs = ptrhedgRTC->CurrentDate.Second;
		ptrhedgTimer->CounterMins = ptrhedgRTC->CurrentDate.Minute;
		ptrhedgTimer->CounterHours = ptrhedgRTC->CurrentDate.Hour;
	}
	else
	{
		ptrhedgTimer->CounterSegs = 0;
		ptrhedgTimer->CounterMins = 0;
		ptrhedgTimer->CounterHours = 0;
	}


	return;

}
