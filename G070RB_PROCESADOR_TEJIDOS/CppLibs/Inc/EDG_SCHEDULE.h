/*
 * EDG_SCHEDULE.h
 *
 *  Created on: 12/04/2023
 *      Author: Elkin Granados
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef EDG_TIM_SCHEDULE_H_
#define EDG_TIM_SCHEDULE_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "EDG_CONFIG.h"

#if EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE

#include "main.h"

#endif

/* Include the headers files  related with tim callbacks  ----------------------*/
#include "EDG_TIMER.h"
#include "EDG_MEM_ADDR.h"
#include "EDG_MEMORY.h"
#include "EDG_AC_CONTROL.h"
#include "EDG_STATE_MACHINE.h"

#define EDG_SCHEDULE_MINUTES_X_HOUR		(60)
#define EDG_SCHEDULE_HOUR_OFFSET		(12)

/**
  * @brief RTC Date Structure definition
  */
typedef enum
{

	EDG_SCHEDULE_STATE_DAY_INACTIVE,
	EDG_SCHEDULE_STATE_DAY_ACTIVE,

}EDG_SCHEDULE_StateDayTypeDef;

/**
  * @brief RTC Date Structure definition
  */
typedef enum
{

	EDG_SCHEDULE_STATUS_NO_APPLIED,
	EDG_SCHEDULE_STATUS_TO_APPLY,
	EDG_SCHEDULE_STATUS_APPLIDED,


}EDG_SCHEDULE_StatusTypeDef;

/**
  * @brief RTC Date Structure definition
  */
typedef struct __EDG_SCHEDULE_DateTypeDef
{

	uint8_t WeekDay;
	uint8_t	Hour;
	uint8_t	Minute;
	uint8_t AmPm;

	uint16_t HourInMinutes;

}EDG_SCHEDULE_DateTypeDef;

/**
  * @brief Schedule Handle Structure definition
  */
typedef struct __EDG_SCHEDULE_HandleTypeDef
{

	EDG_SCHEDULE_DateTypeDef CurrentDate;
	EDG_SCHEDULE_DateTypeDef ActiveHour;
	EDG_SCHEDULE_StatusTypeDef ActiveStatus;

	EDG_SCHEDULE_StateDayTypeDef StateCurrentDay;
	uint8_t CurrentWeekDay;
	uint8_t Program;

}EDG_SCHEDULE_HandleTypeDef;

extern EDG_SCHEDULE_HandleTypeDef hedgSchedule;

void EDG_SCHEDULE_Init(EDG_SCHEDULE_HandleTypeDef * ptrhedgSchedule, EDG_RTC_HandleTypeDef * ptrhedgRTC);
void EDG_SCHEDULE_GetScheduleToday(EDG_SCHEDULE_HandleTypeDef * ptrhedgSchedule, EDG_RTC_HandleTypeDef * ptrhedgRTC);
void EDG_SCHEDULE_ClearScheduleToday(EDG_SCHEDULE_HandleTypeDef * ptrhedgSchedule);
void EDG_SCHEDULE_GetCurrentDate(EDG_SCHEDULE_HandleTypeDef * ptrhedgSchedule, EDG_RTC_HandleTypeDef * ptrhedgRTC);
void EDG_SCHEDULE_CheckActive(EDG_SCHEDULE_HandleTypeDef * ptrhedgSchedule,
	  	  	   			   	  EDG_RTC_HandleTypeDef * ptrhedgRTC);
void EDG_SCHEDULE_CheckChangeDay(EDG_SCHEDULE_HandleTypeDef * ptrhedgSchedule,  EDG_RTC_HandleTypeDef * ptrhedgRTC);
void EDG_SCHEDULE_HourToMinutes(EDG_SCHEDULE_DateTypeDef * ptrhedgScheduleDate);


#ifdef __cplusplus
}
#endif


#endif /* EDG_TIM_SCHEDULE_H_ */
