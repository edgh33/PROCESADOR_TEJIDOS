/*
 * EDG_SCHEDULE.cpp
 *
 *  Created on: 12/04/2023
 *      Author: Elkin Granados
 */

/*** Include header file ***/
#include <EDG_SCHEDULE.h>

EDG_SCHEDULE_HandleTypeDef hedgSchedule;
/**
  * @brief
  * @param
  * @retval
  */
void EDG_SCHEDULE_Init(EDG_SCHEDULE_HandleTypeDef * ptrhedgSchedule, EDG_RTC_HandleTypeDef * ptrhedgRTC)
{

	EDG_SCHEDULE_GetScheduleToday(ptrhedgSchedule, ptrhedgRTC);

	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_SCHEDULE_GetScheduleToday(EDG_SCHEDULE_HandleTypeDef * ptrhedgSchedule, EDG_RTC_HandleTypeDef * ptrhedgRTC)
{
	uint8_t Temp[EDG_MEM_ADDR_VALUES_X_SCHEDULE] = {0};

	EDG_SCHEDULE_GetCurrentDate(ptrhedgSchedule, ptrhedgRTC);

	EDG_MEMORY_ReadMemory(EDG_MEMORY_ADDRESS_MEM1,
						  (EDG_MEM_ADDR_BASE_SCHEDULE+ (EDG_MEM_ADDR_SCHEDULE_OFFSET * (ptrhedgSchedule->CurrentDate.WeekDay - 1))),
						  Temp,
						  EDG_MEM_ADDR_VALUES_X_SCHEDULE);

	ptrhedgSchedule->StateCurrentDay = (EDG_SCHEDULE_StateDayTypeDef)Temp[0];
	ptrhedgSchedule->CurrentWeekDay = ptrhedgSchedule->CurrentDate.WeekDay;

	ptrhedgSchedule->ActiveHour.Hour = Temp[1];
	ptrhedgSchedule->ActiveHour.Minute = Temp[2];
	ptrhedgSchedule->ActiveHour.AmPm = Temp[3];

	ptrhedgSchedule->Program = Temp[4] - 1; // minus 1 because the program is load between 1 -10

	ptrhedgSchedule->ActiveStatus = EDG_SCHEDULE_STATUS_NO_APPLIED;

	EDG_SCHEDULE_HourToMinutes(&ptrhedgSchedule->ActiveHour);

	return;

}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_SCHEDULE_ClearScheduleToday(EDG_SCHEDULE_HandleTypeDef * ptrhedgSchedule)
{
	uint8_t Temp[EDG_MEM_ADDR_VALUES_X_SCHEDULE] = {0};


	EDG_MEMORY_ReadMemory(EDG_MEMORY_ADDRESS_MEM1,
						  (EDG_MEM_ADDR_BASE_SCHEDULE+ (EDG_MEM_ADDR_SCHEDULE_OFFSET * (ptrhedgSchedule->CurrentDate.WeekDay - 1))),
						  Temp,
						  EDG_MEM_ADDR_VALUES_X_SCHEDULE);

	Temp[0] = EDG_SCHEDULE_STATE_DAY_INACTIVE;
	ptrhedgSchedule->StateCurrentDay = (EDG_SCHEDULE_StateDayTypeDef)Temp[0];
	ptrhedgSchedule->CurrentWeekDay = ptrhedgSchedule->CurrentDate.WeekDay;
	ptrhedgSchedule->ActiveHour.Hour = Temp[1];
	ptrhedgSchedule->ActiveHour.Minute = Temp[2];
	ptrhedgSchedule->ActiveHour.AmPm = Temp[3];
	ptrhedgSchedule->Program = Temp[4] - 1; // minus 1 because the program is load between 1 -10

	EDG_MEMORY_WriteMemory(EDG_MEMORY_ADDRESS_MEM1,
							  (EDG_MEM_ADDR_BASE_SCHEDULE + (EDG_MEM_ADDR_SCHEDULE_OFFSET * (ptrhedgSchedule->CurrentDate.WeekDay - 1))),
							  Temp,
							  EDG_MEM_ADDR_VALUES_X_SCHEDULE);

	return;

}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_SCHEDULE_GetCurrentDate(EDG_SCHEDULE_HandleTypeDef * ptrhedgSchedule, EDG_RTC_HandleTypeDef * ptrhedgRTC)
{

	EDG_RTC_GetDate(ptrhedgRTC, EDG_RTC_ADDRESS);

	ptrhedgSchedule->CurrentDate.Hour = ptrhedgRTC->CurrentDate.Hour;
	ptrhedgSchedule->CurrentDate.Minute = ptrhedgRTC->CurrentDate.Minute;
	ptrhedgSchedule->CurrentDate.AmPm = ptrhedgRTC->CurrentDate.AmPm;
	ptrhedgSchedule->CurrentDate.WeekDay = ptrhedgRTC->CurrentDate.WeekDay;

	EDG_SCHEDULE_HourToMinutes(&ptrhedgSchedule->CurrentDate);

	return;

}
/**
  * @brief
  * @param
  * @retval
  */
void EDG_SCHEDULE_CheckActive(EDG_SCHEDULE_HandleTypeDef * ptrhedgSchedule,
	  	  	   			   	  EDG_RTC_HandleTypeDef * ptrhedgRTC)
{

	EDG_SCHEDULE_GetCurrentDate(ptrhedgSchedule, ptrhedgRTC);

	if(ptrhedgSchedule->StateCurrentDay == EDG_SCHEDULE_STATE_DAY_ACTIVE)
	{
		if(ptrhedgSchedule->ActiveStatus == EDG_SCHEDULE_STATUS_NO_APPLIED)
		{
			if(ptrhedgSchedule->CurrentDate.HourInMinutes == ptrhedgSchedule->ActiveHour.HourInMinutes)
			{
				ptrhedgSchedule->ActiveStatus = EDG_SCHEDULE_STATUS_TO_APPLY;
			}
			else if(ptrhedgSchedule->CurrentDate.HourInMinutes > ptrhedgSchedule->ActiveHour.HourInMinutes)
			{
				EDG_SCHEDULE_ClearScheduleToday(&hedgSchedule);
			}
		}
	}

	return;
}


/**
  * @brief
  * @param
  * @retval
  */
void EDG_SCHEDULE_CheckChangeDay(EDG_SCHEDULE_HandleTypeDef * ptrhedgSchedule,  EDG_RTC_HandleTypeDef * ptrhedgRTC)
{

	EDG_SCHEDULE_GetCurrentDate(ptrhedgSchedule, ptrhedgRTC);

	if(ptrhedgSchedule->CurrentWeekDay != ptrhedgSchedule->CurrentDate.WeekDay)
	{
		EDG_SCHEDULE_GetScheduleToday(ptrhedgSchedule, ptrhedgRTC);
	}

	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_SCHEDULE_HourToMinutes(EDG_SCHEDULE_DateTypeDef * ptrhedgScheduleDate)
{

	if(ptrhedgScheduleDate->AmPm == 1)
	{
		if(ptrhedgScheduleDate->Hour != 12)
		{
			ptrhedgScheduleDate->HourInMinutes = ((EDG_SCHEDULE_HOUR_OFFSET + ptrhedgScheduleDate->Hour) * EDG_SCHEDULE_MINUTES_X_HOUR) + ptrhedgScheduleDate->Minute;
		}
		else
		{
			ptrhedgScheduleDate->HourInMinutes = (ptrhedgScheduleDate->Hour * EDG_SCHEDULE_MINUTES_X_HOUR) + ptrhedgScheduleDate->Minute;
		}
	}
	else
	{
		if(ptrhedgScheduleDate->Hour != 12)
		{
			ptrhedgScheduleDate->HourInMinutes = (ptrhedgScheduleDate->Hour * EDG_SCHEDULE_MINUTES_X_HOUR) + ptrhedgScheduleDate->Minute;
		}
		else
		{
			ptrhedgScheduleDate->HourInMinutes = ptrhedgScheduleDate->Minute;
		}
	}
	return;
}
