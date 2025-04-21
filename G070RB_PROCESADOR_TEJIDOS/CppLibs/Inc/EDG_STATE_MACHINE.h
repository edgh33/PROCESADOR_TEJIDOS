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
#define EDG_STATE_MACHINE_TEST_STATE		EDG_STATE_MACHINE_TEST_INACTIVE

#define EDG_STATE_MACHINE_DEBUG_PORT		USART3
#define EDG_STATE_MACHINE_DEBUG_PORT_HANDLE huart3

#define ENG_STATE_MACH_WATCHDOG			1	//Enable watchdog in code

#define EDG_STATE_MACHINE_CHANGE_TEMP_POS_UNIT	 (1)
#define EDG_STATE_MACHINE_CHANGE_TEMP_POS_ACTIVE (2)
#define EDG_STATE_MACHINE_CHANGE_TEMP_POS_VALUE	 (3)

#define EDG_SENSOR_CARRUSEL()	!HAL_GPIO_ReadPin(SENSOR_FIN_GPIO_Port, SENSOR_FIN_Pin)
#define EDG_SENSOR_POSICION()   !HAL_GPIO_ReadPin(SENSOR_AC_GPIO_Port, SENSOR_AC_Pin)

#define EDG_RELE_AGITAR_ACTIVO() HAL_GPIO_WritePin(RELE_START_GPIO_Port, RELE_START_Pin, GPIO_PIN_SET)
#define EDG_RELE_AGITAR_INACTIVO() HAL_GPIO_WritePin(RELE_START_GPIO_Port, RELE_START_Pin, GPIO_PIN_RESET)

#define EDG_RELE_ELEVAR_ACTIVO()	HAL_GPIO_WritePin(RELE_MANUAL_GPIO_Port, RELE_MANUAL_Pin, GPIO_PIN_SET)
#define EDG_RELE_ELEVAR_INACTIVO()	HAL_GPIO_WritePin(RELE_MANUAL_GPIO_Port, RELE_MANUAL_Pin, GPIO_PIN_RESET)
/**
  * @brief State Machine States enum definition
  */
typedef enum
{

	EDG_STATE_MACHINE_STATE_INIT,
	EDG_STATE_MACHINE_STATE_IDLE,
	EDG_STATE_MACHINE_STATE_EXEC_COMMAND,
	EDG_STATE_MACHINE_STATE_TEMPERATURE_CONTROL,
	EDG_STATE_MACHINE_STATE_PROCESS,
	EDG_STATE_MACHINE_STATE_MANUAL,
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
	uint8_t TempValuint8;

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
void EDG_STATE_MACHINE_Process(void);
void EDG_STATE_MACHINE_Manual(void);
void EDG_STATE_MACHINE_ExecCommandState(void);
void EDG_STATE_MACHINE_AlarmState(void);
void EDG_STATE_MACHINE_TestState(void);


/* Auxiliary Functions -------------------------------------*/
void EDG_STATE_MACHINE_SetLedColors(void);
void EDG_STATE_MACHINE_CheckRTC(void);

void EDG_STATE_MACHINE_TestPwmOutput(void);
void EDG_STATE_MACHINE_TestTempetatureChips(void);

void EDG_STATE_MACHINE_ChangePage(EDG_NEXTION_PageTypeDef page);
void EDG_STATE_MACHINE_SetDate(void);
void EDG_STATE_MACHINE_ShowDate(void);
void EDG_STATE_MACHINE_ChangeTemperature(void);
void EDG_STATE_MACHINE_CheckMemory(void);
void EDG_STATE_MACHINE_ResetProgramMemory(void);
void EDG_STATE_MACHINE_ResetScheduleMemory(void);
void EDG_STATE_MACHINE_ResetOffsetMemory(void);
void EDG_STATE_MACHINE_ResetCurrentProcessMemory(void);
void EDG_STATE_MACHINE_CommandResetMemory(void);
void EDG_STATE_MACHINE_SaveProgramValues(uint32_t Program);
void EDG_STATE_MACHINE_LoadProgramValues(uint32_t Program, uint8_t values);
void EDG_STATE_MACHINE_SaveScheduleValues(uint32_t Day);
void EDG_STATE_MACHINE_LoadScheduleValues(void);
void EDG_STATE_MACHINE_ClearScheduleValues(void);
void EDG_STATE_MACHINE_SaveOffsetValues(void);
void EDG_STATE_MACHINE_LoadOffsetValues(void);
void EDG_STATE_MACHINE_BuzzerConfig(void);
void EDG_STATE_MACHINE_BrightSave(void);
void EDG_STATE_MACHINE_CheckButtonSchedule(void);
void EDG_STATE_MACHINE_CheckButtonBuzzer(void);

uint8_t EDG_STATE_MACHINE_CheckActiveProcess(void);
void EDG_STATE_MACHINE_ResumeActiveProcess(void);
uint8_t EDG_STATE_MACHINE_SaveCurrentProcess(void);
void EDG_STATE_MACHINE_RefreshExecutePage(void);
void EDG_STATE_MACHINE_SetTemperatureProcess(void);

void EDG_STATE_MACHINE_HideButtonsExecute(void);
void EDG_STATE_MACHINE_ShowButtonsExecute(void);

#ifdef __cplusplus
}
#endif

#endif /* EDG_STATE_MACHINE_H_ */
