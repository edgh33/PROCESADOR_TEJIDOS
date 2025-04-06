/*
 * EDG_TIM_CALLBACKS.cpp
 *
 *  Created on: 08/04/2023
 *      Author: Elkin Granados
 */

/*** Include header file ***/
#include "EDG_TIM_CALLBACKS.h"


/**
  * @brief  Period elapsed callback in non-blocking mode
  * @param  htim TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	if(htim->Instance == EDG_TIMER_TIM_BASE)
	{

		hedgTimer.FlagsStatus.Flag1s = true;
		hedgTimer.CounterSegs++;
		hedgTimer.CounterBaseTimeSecs1++;

		if(hedgTimer.CounterSegs == EDG_TIMER_MAX_SECONDS)
		{
			hedgTimer.CounterSegs = 0;
			hedgTimer.CounterMins++;
			hedgTimer.FlagsStatus.Flag1m = true;
			if(hedgTimer.CounterMins == EDG_TIMER_MAX_MINUTES)
			{
				hedgTimer.CounterMins = 0;
				hedgTimer.CounterHours++;
				hedgTimer.FlagsStatus.Flag1h = true;
				if(hedgTimer.CounterHours == EDG_TIMER_MAX_HOURS)
				{
					hedgTimer.CounterHours = 0;
					hedgTimer.FlagsStatus.Flag1d = true;
				}

			}

		}
		if(hedgTimer.CounterBaseTimeSecs1 == EDG_TIMER_BASE_TIME_SECS_1)
		{
			hedgTimer.CounterBaseTimeSecs1 = 0;
			hedgTimer.FlagsStatus.FlagBaseTimeSecs1 = true;
		}

	}
	else if(htim->Instance == EDG_NEXTION_TIM_BRIGHT)
	{
		hedgNextion.Bright.Status = EDG_NEXTION_BRIGHT_STATUS_LOW;
	}
	else if(htim->Instance == EDG_PROCESSOR_TIM)
	{
		hedgProcessor.FlagsStatus.FlagTimComplete = 1;
	}
	else if(htim->Instance == EDG_BUZZER_TIM_BASE)
	{
		EDG_BUZZER_IrqFunction();
	}

	return;
}

