/*
 * EDG_RTC.h
 *
 *  Created on: 20/01/2023
 *      Author: Elkin Granados
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef EDG_RTC_H_
#define EDG_RTC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "EDG_CONFIG.h"

#if EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE

#include "main.h"
#include "i2c.h"

#endif

#define	EDG_RTC_I2C_PORT			I2C1	//Puerto I2C
#define EDG_RTC_I2C_HANDLER			hi2c1
#define EDG_RTC_TIMEOUT_MS			(50)
#define EDG_RTC_WAIT_TIME_MS		(5)

#define EDG_RTC_ADDRESS				(0x68)
#define EDG_RTC_STOP_MASK			(0x80)
#define EDG_RTC_HOUR_FORMAT_MASK	(0x40)
#define EDG_RTC_AMPM_MASK			(0x20)
#define EDG_RTC_AMPM_POS			(5)

#define EDG_RTC_TEMP_SECONDS		(0)
#define EDG_RTC_TEMP_MINUTES		(0)
#define EDG_RTC_TEMP_HOURS			(0)
#define EDG_RTC_TEMP_WEEKDAY		(EDG_RTC_WEEKDAY_WEDNESDAY)
#define EDG_RTC_TEMP_DAY			(1)
#define EDG_RTC_TEMP_MONTH			(EDG_RTC_MONTH_JANUARY)
#define EDG_RTC_TEMP_YEAR			(25)


/**
  * @brief enum RTC State type definition
  */
typedef enum
{
	EDG_RTC_STATE_OK,
	EDG_RTC_STATE_ERROR,

}EDG_RTC_StateTypeDef;

/**
  * @brief enum RTC State type definition
  */
typedef enum
{
	EDG_RTC_DATE_STATE_OK,
	EDG_RTC_DATE_STATE_UNCONFIGURED,

}EDG_RTC_DateStateTypeDef;

/**
  * @brief enum RTC Registers type definition
  */
typedef enum
{
	EDG_RTC_REGISTER_SECONDS = 0,
	EDG_RTC_REGISTER_MINUTES,
	EDG_RTC_REGISTER_HOURS,
	EDG_RTC_REGISTER_WEEKDAY,
	EDG_RTC_REGISTER_DAY,
	EDG_RTC_REGISTER_MONTH,
	EDG_RTC_REGISTER_YEAR,
	EDG_RTC_REGISTER_CONTROL,

}EDG_RTC_RegisterTypeDef;

/**
  * @brief enum RTC Weekdays type definition
  */
typedef enum
{
	EDG_RTC_WEEKDAY_MONDAY = 1,
	EDG_RTC_WEEKDAY_TUESDAY,
	EDG_RTC_WEEKDAY_WEDNESDAY,
	EDG_RTC_WEEKDAY_THURSDAY,
	EDG_RTC_WEEKDAY_FRIDAY,
	EDG_RTC_WEEKDAY_SATURDAY,
	EDG_RTC_WEEKDAY_SUNDAY,

}EDG_RTC_WeekDayTypeDef;

/**
  * @brief enum RTC Weekdays type definition
  */
typedef enum
{

	EDG_RTC_AMPM_AM,
	EDG_RTC_AMPM_PM,

}EDG_RTC_AmPmTypeDef;

/**
  * @brief enum RTC Months type definition
  */
typedef enum
{
	EDG_RTC_MONTH_JANUARY = 1,
	EDG_RTC_MONTH_FEBRUARY,
	EDG_RTC_MONTH_MARCH,
	EDG_RTC_MONTH_APRIL,
	EDG_RTC_MONTH_MAY,
	EDG_RTC_MONTH_JUNE,
	EDG_RTC_MONTH_JULY,
	EDG_RTC_MONTH_AUGUST,
	EDG_RTC_MONTH_SEPTEMBER,
	EDG_RTC_MONTH_OCTOBER,
	EDG_RTC_MONTH_NOVEMBER,
	EDG_RTC_MONTH_DECEMBER,

}EDG_RTC_MonthTypeDef;

/**
  * @brief enum RTC Hour Format type definition
  */
typedef enum
{

	EDG_RTC_HOUR_FORMAT_24H,
	EDG_RTC_HOUR_FORMAT_12H,

}EDG_RTC_HourFormatTypeDef;

/**
  * @brief RTC Date Structure definition
  */
typedef struct __EDG_RTC_DateTypeDef
{
	uint8_t	Day;
	uint8_t WeekDay;
	uint8_t	Month;
	uint8_t	Year;
	uint8_t	Hour;
	uint8_t	Minute;
	uint8_t	Second;
	EDG_RTC_AmPmTypeDef AmPm;
	EDG_RTC_HourFormatTypeDef Format;

}EDG_RTC_DateTypeDef;

/**
  * @brief RTC Handle Structure definition
  */
typedef struct __EDG_RTC_HandleTypeDef
{

	EDG_RTC_DateTypeDef CurrentDate;
	EDG_RTC_DateTypeDef DatetoChange;
	EDG_RTC_StateTypeDef CurrentState;
	EDG_RTC_DateStateTypeDef DateState;

}EDG_RTC_HandleTypeDef;

/**
  * @brief  RTC Handler Definition
  */
extern EDG_RTC_HandleTypeDef hedgRTC;

/* RTC Functions -------------------------------------*/

EDG_RTC_StateTypeDef EDG_RTC_Read(uint8_t Address, uint16_t MemAddress, uint8_t * ptrData, uint16_t Lenght);
EDG_RTC_StateTypeDef EDG_RTC_Write(uint8_t Address, uint16_t MemAddress, uint8_t * ptrData, uint16_t Lenght);

EDG_RTC_StateTypeDef EDG_RTC_Init(EDG_RTC_HandleTypeDef * ptrhedgRTC, uint8_t Address);
EDG_RTC_StateTypeDef EDG_RTC_GetDate(EDG_RTC_HandleTypeDef * ptrhedgRTC, uint8_t Address);
EDG_RTC_StateTypeDef EDG_RTC_SetDate(EDG_RTC_HandleTypeDef * ptrhedgRTC, uint8_t Address);

EDG_RTC_StateTypeDef EDG_RTC_SetHourFormat(EDG_RTC_HandleTypeDef * ptrhedgRTC, uint8_t Address);
EDG_RTC_StateTypeDef EDG_RTC_GetHourFormat(EDG_RTC_HandleTypeDef * ptrhedgRTC, uint8_t Address);

EDG_RTC_StateTypeDef EDG_RTC_CheckCommI2C(EDG_RTC_HandleTypeDef * ptrhedgRTC, uint8_t Address);
EDG_RTC_StateTypeDef EDG_RTC_CheckRun(EDG_RTC_HandleTypeDef * ptrhedgRTC, uint8_t Address);
EDG_RTC_StateTypeDef EDG_RTC_CheckDate(EDG_RTC_HandleTypeDef * ptrhedgRTC, uint8_t Address);

#ifdef __cplusplus
}
#endif


#endif /* EDG_RTC_H_ */
