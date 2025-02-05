/*
 * EDG_STATE_MACHINE.h
 *
 *  Created on: 06/04/2023
 *      Author: Elkin Granados
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef EDG_STATE_MACHINE_H_
#define EDG_STATE_MACHINE_H_

#ifdef __cplusplus
extern "C" {
#endif

/**** Include the main.h file if the workspace is CubeIDE ****/
#if ENG_CONFIG_IDE_SEL == ENG_CONFIG_IDE_CUBEIDE

#include "main.h"

#endif

#include "MainCpp.h"

#define EDG_STATE_MACHINE_DEBUG_INACTIVE 	0
#define EDG_STATE_MACHINE_DEBUG_ACTIVE 		1
#define EDG_STATE_MACHINE_DEBUG_STATE		EDG_STATE_MACHINE_DEBUG_ACTIVE

#define EDG_STATE_MACHINE_TEST_INACTIVE 	0
#define EDG_STATE_MACHINE_TEST_ACTIVE 		1
#define EDG_STATE_MACHINE_TEST_STATE		EDG_STATE_MACHINE_TEST_ACTIVE

#define EDG_STATE_MACHINE_DEBUG_PORT		USART3
#define EDG_STATE_MACHINE_DEBUG_PORT_HANDLE huart3

#define EDG_STATE_MACHINE_CHANGE_TEMP_POS_UNIT	(1)
#define EDG_STATE_MACHINE_CHANGE_TEMP_POS_VALUE	(3)
/**
  * @brief State Machine States enum definition
  */
typedef enum
{

	EDG_STATE_MACHINE_STATE_INIT,
	EDG_STATE_MACHINE_STATE_IDLE,
	EDG_STATE_MACHINE_STATE_EXEC_COMMAND,
	EDG_STATE_MACHINE_STATE_TEMPERATURE_CONTROL,
	EDG_STATE_MACHINE_STATE_ALARM,
	EDG_STATE_MACHINE_STATE_TEST,
	EDG_STATE_MACHINE_STATE_LAST,

}EDG_STATE_MACHINE_StateTypeDef;


/* Enums and Structures for the program logic ----------------------------*/

/**
  * @brief
  */
typedef enum
{

	EDG_STATE_MACHINE_COMMAND_CHANGE_PAGE,
	EDG_STATE_MACHINE_COMMAND_CHANGE_TEMPERATURE,
	EDG_STATE_MACHINE_COMMAND_SET_DATE,
	EDG_STATE_MACHINE_COMMAND_PASSWORD,
	EDG_STATE_MACHINE_COMMAND_SET_THERMOCOUPLES,
	EDG_STATE_MACHINE_COMMAND_LOAD_PROGRAM_VALUES,
	EDG_STATE_MACHINE_COMMAND_SAVE_PROGRAM_VALUES,
	EDG_STATE_MACHINE_COMMAND_SAVE_SCHEDULE,
	EDG_STATE_MACHINE_COMMAND_RESET,
	EDG_STATE_MACHINE_COMMAND_BUZZER,
	EDG_STATE_MACHINE_COMMAND_NONE, /*** used as default value command ***/
	EDG_STATE_MACHINE_COMMAND_LAST, /*** used for check a valid command ***/

}EDG_STATE_MACHINE_CommandTypeDef;

/**
  * @brief
  */
typedef enum
{

	EDG_STATE_MACHINE_RTC_POS_MINUTE=1,
	EDG_STATE_MACHINE_RTC_POS_HOUR,
	EDG_STATE_MACHINE_RTC_POS_WEEKDAY,
	EDG_STATE_MACHINE_RTC_POS_DAY,
	EDG_STATE_MACHINE_RTC_POS_MONTH,
	EDG_STATE_MACHINE_RTC_POS_YEAR,
	EDG_STATE_MACHINE_RTC_POS_AMPM,

}EDG_STATE_MACHINE_RtcPosTypeDef;

/**
  * @brief State Machine Handle Structure definition
  */
typedef struct __EDG_STATE_MACHINE_HandleTypeDef
{

	EDG_STATE_MACHINE_StateTypeDef CurrentState;
	EDG_STATE_MACHINE_StateTypeDef PastState;

}EDG_STATE_MACHINE_HandleTypeDef;

extern EDG_STATE_MACHINE_HandleTypeDef hedgStateMachine;

/* State Machine Functions -------------------------------------*/
void EDG_STATE_MACHINE_Init(EDG_STATE_MACHINE_HandleTypeDef * ptrhedgStateMachine);
void EDG_STATE_MACHINE_SetNextState(EDG_STATE_MACHINE_HandleTypeDef * ptrhedgStateMachine,
									EDG_STATE_MACHINE_StateTypeDef NextState);

void EDG_STATE_MACHINE_Admin(EDG_STATE_MACHINE_HandleTypeDef * ptrhedgStateMachine);

void EDG_STATE_MACHINE_InitState(void);
void EDG_STATE_MACHINE_IdleState(void);
void EDG_STATE_MACHINE_TemperatureControlState(void);
void EDG_STATE_MACHINE_ExecCommandState(void);
void EDG_STATE_MACHINE_AlarmState(void);
void EDG_STATE_MACHINE_TestState(void);


/* Auxiliary Functions -------------------------------------*/
void EDG_STATE_MACHINE_SetLedColors(void);
void EDG_STATE_MACHINE_CheckRTC(void);

void EDG_STATE_MACHINE_TestPwmOutput(void);
void EDG_STATE_MACHINE_TestTempetatureChips(void);

void EDG_STATE_MACHINE_ChangePage(void);
void EDG_STATE_MACHINE_SetDate(void);
void EDG_STATE_MACHINE_ShowDate(void);
void EDG_STATE_MACHINE_SetConfigPage(void);
void EDG_STATE_MACHINE_RefreshConfigPage(void);
void EDG_STATE_MACHINE_ChangeTemperature(void);
void EDG_STATE_MACHINE_CheckMemory(void);
void EDG_STATE_MACHINE_SaveProgramValues(uint32_t Program);
void EDG_STATE_MACHINE_LoadProgramValues(uint32_t Program);
void EDG_STATE_MACHINE_SaveScheduleValues(uint32_t Day);
void EDG_STATE_MACHINE_LoadScheduleValues(void);
void EDG_STATE_MACHINE_SaveOffsetValues(void);
void EDG_STATE_MACHINE_LoadOffsetValues(void);
void EDG_STATE_MACHINE_BuzzerConfig(void);
void EDG_STATE_MACHINE_BrightSave(void);
void EDG_STATE_MACHINE_CheckButtonSchedule(void);
void EDG_STATE_MACHINE_CheckButtonBuzzer(void);



#ifdef __cplusplus
}
#endif

#endif /* EDG_STATE_MACHINE_H_ */
