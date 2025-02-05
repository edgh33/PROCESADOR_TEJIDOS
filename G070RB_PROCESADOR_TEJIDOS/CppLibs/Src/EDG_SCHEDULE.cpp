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

	ptrhedgSchedule->InactiveHour.Hour = Temp[4];
	ptrhedgSchedule->InactiveHour.Minute = Temp[5];
	ptrhedgSchedule->InactiveHour.AmPm = Temp[6];

	ptrhedgSchedule->Program = Temp[7];

	ptrhedgSchedule->ActiveStatus = EDG_SCHEDULE_STATUS_NO_APPLIDED;
	ptrhedgSchedule->InactiveStatus = EDG_SCHEDULE_STATUS_NO_APPLIDED;

	EDG_SCHEDULE_HourToMinutes(&ptrhedgSchedule->ActiveHour);
	EDG_SCHEDULE_HourToMinutes(&ptrhedgSchedule->InactiveHour);

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
	  	  	   			   	  EDG_RTC_HandleTypeDef * ptrhedgRTC,
							  EDG_AC_CONTROL_HandleTypeDef * ptrhedgAccontrol,
							  EDG_NEXTION_HandleTypeDef * ptrhedgNextion)
{

	uint8_t Temp[EDG_MEM_ADDR_VALUES_X_PROGRAM] = {0};
	uint8_t Counter;

	EDG_SCHEDULE_GetCurrentDate(ptrhedgSchedule, ptrhedgRTC);

	if(ptrhedgSchedule->StateCurrentDay == EDG_SCHEDULE_STATE_DAY_ACTIVE)
	{
		if(ptrhedgSchedule->ActiveStatus != EDG_SCHEDULE_STATUS_APPLIDED)
		{
			if(ptrhedgSchedule->CurrentDate.HourInMinutes >= ptrhedgSchedule->ActiveHour.HourInMinutes)
			{
				EDG_MEMORY_ReadMemory(EDG_MEMORY_ADDRESS_MEM1,
									  (EDG_MEM_ADDR_BASE_PROGRAM + (EDG_MEM_ADDR_PROGRAM_OFFSET * ptrhedgSchedule->Program)),
									  Temp,
									  EDG_MEM_ADDR_VALUES_X_PROGRAM);

				for(Counter = 0; Counter < ptrhedgAccontrol->UnitsQty; Counter++)
				{
					ptrhedgAccontrol->Units[Counter].Pid.SetPoint = (float)Temp[Counter];
				}
				ptrhedgSchedule->ActiveStatus = EDG_SCHEDULE_STATUS_APPLIDED;
				HAL_Delay(50);
				EDG_NEXTION_ChangePage(&hedgNextion, EDG_NEXTION_PAGE_CONFIG);
				EDG_STATE_MACHINE_SetConfigPage();
				EDG_STATE_MACHINE_ShowDate();

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
void EDG_SCHEDULE_CheckInactive(EDG_SCHEDULE_HandleTypeDef * ptrhedgSchedule,
		  	  	  	  	  	    EDG_RTC_HandleTypeDef * ptrhedgRTC,
								EDG_AC_CONTROL_HandleTypeDef * ptrhedgAccontrol,
								EDG_NEXTION_HandleTypeDef * ptrhedgNextion)
{

	uint8_t Counter = 0;

	EDG_SCHEDULE_GetCurrentDate(ptrhedgSchedule, ptrhedgRTC);

	if(ptrhedgSchedule->StateCurrentDay == EDG_SCHEDULE_STATE_DAY_ACTIVE)
	{
		if(ptrhedgSchedule->InactiveStatus != EDG_SCHEDULE_STATUS_APPLIDED)
		{
			/* Se verifica si el tiempo es mayor al de la hora de inactivacion */
			if(ptrhedgSchedule->CurrentDate.HourInMinutes >=  ptrhedgSchedule->InactiveHour.HourInMinutes)
			{

				for(Counter = 0; Counter < ptrhedgAccontrol->UnitsQty; Counter++)
				{
					ptrhedgAccontrol->Units[Counter].Pid.SetPoint = 0;
					/* (07/05/23) Agrego la funcion de detener el PWM */
					EDG_AC_CONTROL_StopPWMOutput(ptrhedgAccontrol, Counter);
				}
				ptrhedgSchedule->InactiveStatus = EDG_SCHEDULE_STATUS_APPLIDED;
				HAL_Delay(50);
				EDG_NEXTION_ChangePage(&hedgNextion, EDG_NEXTION_PAGE_CONFIG);
				EDG_STATE_MACHINE_SetConfigPage();
				EDG_STATE_MACHINE_ShowDate();
			}
			/* Se verifica si el tiempo es menor al de la hora de activacion */
			else if(ptrhedgSchedule->CurrentDate.HourInMinutes <  ptrhedgSchedule->ActiveHour.HourInMinutes)
			{
				if(ptrhedgSchedule->InactiveStatus != EDG_SCHEDULE_STATUS_INIT_OFF)
				{
					for(Counter = 0; Counter < ptrhedgAccontrol->UnitsQty; Counter++)
					{
						ptrhedgAccontrol->Units[Counter].Pid.SetPoint = 0;
						/* (07/05/23) Agrego la funcion de detener el PWM */
						EDG_AC_CONTROL_StopPWMOutput(ptrhedgAccontrol, Counter);
					}

					ptrhedgSchedule->InactiveStatus = EDG_SCHEDULE_STATUS_INIT_OFF;
				}
				/* No se cambia bandera de estado de inactivacion */
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
