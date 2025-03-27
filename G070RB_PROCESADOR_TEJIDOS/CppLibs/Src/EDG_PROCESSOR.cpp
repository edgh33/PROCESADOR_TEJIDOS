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
	phedgProcessor->FlagFirstRunning = EDG_PROCESSOR_FLAG_FIRST_RUNNING;
	phedgProcessor->FlagMinute = EDG_PROCESSOR_FLAG_MINUTE_RUNNING;
	phedgProcessor->CurrentState = EDG_PROCESSOR_STATE_STOP;

	phedgProcessor->CurrentContainer = 0;
	phedgProcessor->ContainerQuantity = 0;

	phedgProcessor->CurrentHour = 0;
	phedgProcessor->CurrentMinute = 0;
	phedgProcessor->TotalTimeCurrenteContainer = 0;

	phedgProcessor->CurrentHourDelay = 0;
	phedgProcessor->CurrentMinuteDelay = 0;
	phedgProcessor->TotalTimeDelayMinutes = 0;

	return;
}

